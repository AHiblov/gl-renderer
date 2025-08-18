/* base_3d_renderer.cpp
 * Common drawing stuff for 3D objects
 * OpenGL 4.5
 *
 * Author: Artem Hiblov
 */

#include "graphics_lib/base_3d_renderer.h"

#include <chrono>

#include <GL/glew.h>
#include <glm/gtc/matrix_transform.hpp>

#include "log.h"
#include "data/shader_properties.h"
#include "graphics_lib/light_setters.h"

using namespace std;
using namespace std::chrono;
using namespace renderer;
using namespace renderer::data;
using namespace renderer::graphics_lib;
using namespace renderer::graphics_lib::videocard_data;

namespace
{
	constexpr float MILLISECONDS_IN_SECOND = 1000.f;

	const int8_t quadFlagsMaskArray[4] = { VisibilityFlags::QUAD1_FLAG, VisibilityFlags::QUAD2_FLAG, VisibilityFlags::QUAD3_FLAG, VisibilityFlags::QUAD4_FLAG };
}

Base3DRenderer::Base3DRenderer(const vector<ShaderIds> &shaderIds, const ShaderIds &sky, const map<int, unsigned long long> &shaderFlags, bool isDirectional):
	previousShader(-1), shaders(nullptr), shaderAmount(0), renderingScene(nullptr), visibilityFlagsPtr(nullptr), cameraPositionPtr(nullptr), skyShader(sky), triangleCount(0)
{
	initialize(shaderFlags, isDirectional);
	copyShaderArray(shaderIds);
}

Base3DRenderer::~Base3DRenderer()
{
	if(shaders)
	{
		delete[] shaders;
		shaders = nullptr;
	}

	if(renderingScene)
	{
		delete renderingScene;
		renderingScene = nullptr;
	}
}

void Base3DRenderer::renderOpaqueMeshes()
{
	const int chunkAmount = renderingScene->chunkAmount;

	//Opaque objects
	for(int chunkIndex = 0; chunkIndex < chunkAmount; chunkIndex++)
	{
		const VisibilityFlags &chunkFlags = (*visibilityFlagsPtr)[chunkIndex];

		if(!(chunkFlags.isChunkVisible))
			continue;

		for(int quadIndex = 0; quadIndex < 4; quadIndex++)
		{
			if(!(chunkFlags.quadVisibility & quadFlagsMaskArray[quadIndex]))
				continue;

			const int objectsInQuad = renderingScene->opaqueObjects[chunkIndex].quad[quadIndex].amount;

			if(objectsInQuad == 0)
				continue;

			const ObjectQuadSubdivision::ObjectNode *objectArray = renderingScene->opaqueObjects[chunkIndex].quad[quadIndex].objectsInQuad;

			for(int i = 0; i < objectsInQuad; i++)
			{
				if(previousShader != objectArray[i].shaderIndex)
				{
					glUseProgram(shaders[objectArray[i].shaderIndex].id);
					previousShader = objectArray[i].shaderIndex;
				}

				glUniformMatrix4fv(shaders[objectArray[i].shaderIndex].unifView, 1, GL_FALSE, &viewMatrix[0][0]); //viewMatrix is common for objects, terrain and particles

				(this->*renderObjects[objectArray[i].shaderIndex])(objectArray[i]);
			}
		}
	}

	//Particles
	for(int chunkIndex = 0; chunkIndex < chunkAmount; chunkIndex++)
	{
		const VisibilityFlags &chunkFlags = (*visibilityFlagsPtr)[chunkIndex];

		if(!(chunkFlags.isChunkVisible))
			continue;

		for(int quadIndex = 0; quadIndex < 4; quadIndex++)
		{
			if(!(chunkFlags.quadVisibility & quadFlagsMaskArray[quadIndex]))
				continue;

			const int groupsInQuad = renderingScene->particles[chunkIndex].quad[quadIndex].amount;

			if(groupsInQuad == 0)
				continue;

			const ParticleQuadSubdivision::ParticleNode *particleArray = renderingScene->particles[chunkIndex].quad[quadIndex].groupsInQuad;

			for(int i = 0; i < groupsInQuad; i++)
			{
				if(previousShader != particleArray[i].shaderIndex)
				{
					glUseProgram(shaders[particleArray[i].shaderIndex].id);
					previousShader = particleArray[i].shaderIndex;
				}

				glUniformMatrix4fv(shaders[particleArray[i].shaderIndex].unifView, 1, GL_FALSE, &viewMatrix[0][0]); //viewMatrix is common for objects, terrain and particles

				(this->*renderParticles)(particleArray[i]);
			}
		}
	}

	//Terrain (opaque)

	glUseProgram(shaders[renderingScene->terrain[0].shaderIndex].id); //The same shader for all chunks
	glUniformMatrix4fv(shaders[renderingScene->terrain[0].shaderIndex].unifView, 1, GL_FALSE, &viewMatrix[0][0]); //viewMatrix is common for objects, terrain and particles
	previousShader = renderingScene->terrain[0].shaderIndex;

	for(int i = 0; i < renderingScene->chunkAmount; i++)
	{
		if(!((*visibilityFlagsPtr)[i].isChunkVisible))
			continue;

		(this->*renderTerrain)(i);
	}
}

void Base3DRenderer::renderTransparentMeshes()
{
	if(!renderingScene->transparentObjects)
		return;

	const int chunkAmount = renderingScene->chunkAmount;

	for(int chunkIndex = 0; chunkIndex < chunkAmount; chunkIndex++)
	{
		const VisibilityFlags &chunkFlags = (*visibilityFlagsPtr)[chunkIndex];

		if(!(chunkFlags.isChunkVisible))
			continue;

		for(int quadIndex = 0; quadIndex < 4; quadIndex++)
		{
			if(!(chunkFlags.quadVisibility & quadFlagsMaskArray[quadIndex]))
				continue;

			const int objectsInQuad = renderingScene->transparentObjects[chunkIndex].quad[quadIndex].amount;

			if(objectsInQuad == 0)
				continue;

			const ObjectQuadSubdivision::ObjectNode *objectArray = renderingScene->transparentObjects[chunkIndex].quad[quadIndex].objectsInQuad;

			for(int i = 0; i < objectsInQuad; i++)
			{
				if(previousShader != objectArray[i].shaderIndex)
				{
					glUseProgram(shaders[objectArray[i].shaderIndex].id);
					previousShader = objectArray[i].shaderIndex;
				}

				glUniformMatrix4fv(shaders[objectArray[i].shaderIndex].unifView, 1, GL_FALSE, &viewMatrix[0][0]); //viewMatrix is common for objects, terrain and particles

				(this->*renderObjects[objectArray[i].shaderIndex])(objectArray[i]);
			}
		}
	}
}

void Base3DRenderer::renderTerrainDirectional(int index)
{
	RenderingTerrain &currentRenderingChunk = renderingScene->terrain[index];
	ObjectRenderingData &terrainData = renderingScene->terrain[index].terrainData;

	glBindVertexArray(terrainData.vaoId);
	glBindTextureUnit(0, terrainData.textureId);

	glUniform1i(shaders[currentRenderingChunk.shaderIndex].unifTextureId, 0);

	glUniformMatrix4fv(shaders[currentRenderingChunk.shaderIndex].unifModel, 1, GL_FALSE, &(currentRenderingChunk.position)[0][0]);
	static glm::mat3 rotation(1.f); //No rotation support for terrain
	glUniformMatrix3fv(shaders[currentRenderingChunk.shaderIndex].unifRotation, 1, GL_FALSE, &rotation[0][0]);

	glDrawArrays(GL_TRIANGLE_STRIP, 0, terrainData.vertexAmount);
	triangleCount += terrainData.vertexAmount / 3;
}

void Base3DRenderer::renderTerrainPoint(int index)
{
	RenderingTerrain &currentRenderingChunk = renderingScene->terrain[index];
	ObjectRenderingData &terrainData = renderingScene->terrain[index].terrainData;

	glBindVertexArray(terrainData.vaoId);
	glBindTextureUnit(0, terrainData.textureId);

	glUniform1i(shaders[currentRenderingChunk.shaderIndex].unifTextureId, 0);

	glUniformMatrix4fv(shaders[currentRenderingChunk.shaderIndex].unifModel, 1, GL_FALSE, &(currentRenderingChunk.position)[0][0]);

	glDrawArrays(GL_TRIANGLE_STRIP, 0, terrainData.vertexAmount);
	triangleCount += terrainData.vertexAmount / 3;
}

void Base3DRenderer::renderObjectsDirectional(const ObjectQuadSubdivision::ObjectNode &objectInstances)
{
	const ObjectRenderingData &objectData = objectInstances.objectData;

	glBindVertexArray(objectData.vaoId);
	glBindTextureUnit(0, objectData.textureId);

	glUniform1i(shaders[objectInstances.shaderIndex].unifTextureId, 0);

	for(int i = 0; i < objectInstances.amount; i++) //Iterate all positions of object instances
	{
		glUniformMatrix4fv(shaders[objectInstances.shaderIndex].unifModel, 1, GL_FALSE, &objectInstances.arrangement[i][0][0]);
		glUniformMatrix3fv(shaders[objectInstances.shaderIndex].unifRotation, 1, GL_FALSE, &objectInstances.rotation[i][0][0]);

		glDrawArrays(GL_TRIANGLES, 0, objectData.vertexAmount);
		triangleCount += objectData.vertexAmount / 3;
	}
}

void Base3DRenderer::renderObjectsDirectionalNormalmap(const ObjectQuadSubdivision::ObjectNode &objectInstances)
{
	const ObjectRenderingData &objectData = objectInstances.objectData;

	glBindVertexArray(objectData.vaoId);
	glBindTextureUnit(0, objectData.textureId);
	glBindTextureUnit(1, objectData.normalTextureId);

	glUniform1i(shaders[objectInstances.shaderIndex].unifTextureId, 0);
	glUniform1i(shaders[objectInstances.shaderIndex].unifNormalTextureId, 1);

	for(int i = 0; i < objectInstances.amount; i++) //Iterate all positions of object instances
	{
		glUniformMatrix4fv(shaders[objectInstances.shaderIndex].unifModel, 1, GL_FALSE, &objectInstances.arrangement[i][0][0]);
		glUniformMatrix3fv(shaders[objectInstances.shaderIndex].unifRotation, 1, GL_FALSE, &objectInstances.rotation[i][0][0]);

		glDrawArrays(GL_TRIANGLES, 0, objectData.vertexAmount);
		triangleCount += objectData.vertexAmount / 3;
	}
}

void Base3DRenderer::renderObjectsDirectionalWaves(const ObjectQuadSubdivision::ObjectNode &objectInstances)
{
	const ObjectRenderingData &objectData = objectInstances.objectData;

	glBindVertexArray(objectData.vaoId);
	glBindTextureUnit(0, objectData.textureId);

	glUniform1i(shaders[objectInstances.shaderIndex].unifTextureId, 0);

	steady_clock::time_point curTime = steady_clock::now();
	float fTime = duration_cast<milliseconds>(curTime.time_since_epoch()).count() / MILLISECONDS_IN_SECOND;
	glUniform1f(shaders[objectInstances.shaderIndex].unifTime, fTime);

	for(int i = 0; i < objectInstances.amount; i++) //Iterate all positions of object instances
	{
		glUniformMatrix4fv(shaders[objectInstances.shaderIndex].unifModel, 1, GL_FALSE, &objectInstances.arrangement[i][0][0]);
		glUniformMatrix3fv(shaders[objectInstances.shaderIndex].unifRotation, 1, GL_FALSE, &objectInstances.rotation[i][0][0]);

		glDrawArrays(GL_TRIANGLES, 0, objectData.vertexAmount);
		triangleCount += objectData.vertexAmount / 3;
	}
}

void Base3DRenderer::renderObjectsDirectionalGlitter(const ObjectQuadSubdivision::ObjectNode &objectInstances)
{
	const ObjectRenderingData &objectData = objectInstances.objectData;

	glBindVertexArray(objectData.vaoId);
	glBindTextureUnit(0, objectData.textureId);

	glUniform1i(shaders[objectInstances.shaderIndex].unifTextureId, 0);

	for(int i = 0; i < objectInstances.amount; i++) //Iterate all positions of object instances
	{
		glUniformMatrix4fv(shaders[objectInstances.shaderIndex].unifModel, 1, GL_FALSE, &objectInstances.arrangement[i][0][0]);
		glUniformMatrix3fv(shaders[objectInstances.shaderIndex].unifRotation, 1, GL_FALSE, &objectInstances.rotation[i][0][0]);
		glUniform3fv(shaders[objectInstances.shaderIndex].unifCameraPosition, 1, &((*cameraPositionPtr)[0]));

		glDrawArrays(GL_TRIANGLES, 0, objectData.vertexAmount);
		triangleCount += objectData.vertexAmount / 3;
	}
}

void Base3DRenderer::renderObjectsDirectionalInstancing(const ParticleQuadSubdivision::ParticleNode &instanceGroup)
{
	const ParticleRenderingData &particleData = instanceGroup.data;
    const ObjectRenderingData &objectData = particleData.objectData;

	glBindVertexArray(objectData.vaoId);
	glBindTextureUnit(0, objectData.textureId);

	glUniform1i(shaders[instanceGroup.shaderIndex].unifTextureId, 0);

	glm::mat3 unit(1.f);
	glUniformMatrix4fv(shaders[instanceGroup.shaderIndex].unifModel, 1, GL_FALSE, &instanceGroup.arrangement[0][0]);
	glUniformMatrix3fv(shaders[instanceGroup.shaderIndex].unifRotation, 1, GL_FALSE, &unit[0][0]); //Rotation is defined per-particle

    glDrawArraysInstanced(GL_TRIANGLES, 0, objectData.vertexAmount, particleData.particleAmount);
    triangleCount += (objectData.vertexAmount / 3) * particleData.particleAmount;
}

void Base3DRenderer::renderObjectsPoint(const ObjectQuadSubdivision::ObjectNode &objectInstances)
{
	const ObjectRenderingData &objectData = objectInstances.objectData;

	glBindVertexArray(objectData.vaoId);
	glBindTextureUnit(0, objectData.textureId);

	glUniform1i(shaders[objectInstances.shaderIndex].unifTextureId, 0);

	for(int i = 0; i < objectInstances.amount; i++) //Iterate all positions of object instances
	{
		glUniformMatrix4fv(shaders[objectInstances.shaderIndex].unifModel, 1, GL_FALSE, &objectInstances.arrangement[i][0][0]);

		glDrawArrays(GL_TRIANGLES, 0, objectData.vertexAmount);
		triangleCount += objectData.vertexAmount / 3;
	}
}

void Base3DRenderer::renderObjectsPointNormalmap(const ObjectQuadSubdivision::ObjectNode &objectInstances)
{
	const ObjectRenderingData &objectData = objectInstances.objectData;

	glBindVertexArray(objectData.vaoId);
	glBindTextureUnit(0, objectData.textureId);
	glBindTextureUnit(1, objectData.normalTextureId);

	glUniform1i(shaders[objectInstances.shaderIndex].unifTextureId, 0);
	glUniform1i(shaders[objectInstances.shaderIndex].unifNormalTextureId, 1);

	for(int i = 0; i < objectInstances.amount; i++) //Iterate all positions of object instances
	{
		glUniformMatrix4fv(shaders[objectInstances.shaderIndex].unifModel, 1, GL_FALSE, &objectInstances.arrangement[i][0][0]);

		glDrawArrays(GL_TRIANGLES, 0, objectData.vertexAmount);
		triangleCount += objectData.vertexAmount / 3;
	}
}

void Base3DRenderer::renderObjectsPointWaves(const ObjectQuadSubdivision::ObjectNode &objectInstances)
{
	const ObjectRenderingData &objectData = objectInstances.objectData;

	glBindVertexArray(objectData.vaoId);
	glBindTextureUnit(0, objectData.textureId);

	glUniform1i(shaders[objectInstances.shaderIndex].unifTextureId, 0);

	steady_clock::time_point curTime = steady_clock::now();
	float fTime = duration_cast<milliseconds>(curTime.time_since_epoch()).count() / MILLISECONDS_IN_SECOND;
	glUniform1f(shaders[objectInstances.shaderIndex].unifTime, fTime);

	for(int i = 0; i < objectInstances.amount; i++) //Iterate all positions of object instances
	{
		glUniformMatrix4fv(shaders[objectInstances.shaderIndex].unifModel, 1, GL_FALSE, &objectInstances.arrangement[i][0][0]);

		glDrawArrays(GL_TRIANGLES, 0, objectData.vertexAmount);
		triangleCount += objectData.vertexAmount / 3;
	}
}

void Base3DRenderer::renderObjectsPointGlitter(const ObjectQuadSubdivision::ObjectNode &objectInstances)
{
	const ObjectRenderingData &objectData = objectInstances.objectData;

	glBindVertexArray(objectData.vaoId);
	glBindTextureUnit(0, objectData.textureId);

	glUniform1i(shaders[objectInstances.shaderIndex].unifTextureId, 0);

	for(int i = 0; i < objectInstances.amount; i++) //Iterate all positions of object instances
	{
		glUniformMatrix4fv(shaders[objectInstances.shaderIndex].unifModel, 1, GL_FALSE, &objectInstances.arrangement[i][0][0]);
		glUniform3fv(shaders[objectInstances.shaderIndex].unifCameraPosition, 1, &((*cameraPositionPtr)[0]));

		glDrawArrays(GL_TRIANGLES, 0, objectData.vertexAmount);
		triangleCount += objectData.vertexAmount / 3;
	}
}

void Base3DRenderer::renderObjectsPointInstancing(const ParticleQuadSubdivision::ParticleNode &instanceGroup)
{
	const ParticleRenderingData &particleData = instanceGroup.data;
	const ObjectRenderingData &objectData = particleData.objectData;

	glBindVertexArray(objectData.vaoId);
	glBindTextureUnit(0, objectData.textureId);

	glUniform1i(shaders[instanceGroup.shaderIndex].unifTextureId, 0);

	glm::mat3 unit(1.f);
	glUniformMatrix4fv(shaders[instanceGroup.shaderIndex].unifModel, 1, GL_FALSE, &instanceGroup.arrangement[0][0]);
	glUniformMatrix3fv(shaders[instanceGroup.shaderIndex].unifRotation, 1, GL_FALSE, &unit[0][0]); //Rotation is defined per-particle

    glDrawArraysInstanced(GL_TRIANGLES, 0, objectData.vertexAmount, particleData.particleAmount);
    triangleCount += (objectData.vertexAmount / 3) * particleData.particleAmount;
}

void Base3DRenderer::renderSky()
{
	auto &objectData = renderingScene->sky;

	glBindVertexArray(objectData.vaoId);
	glBindTextureUnit(0, objectData.textureId);

	glUniform1i(skyShader.unifTextureId, 0);

	glm::mat4 model = glm::translate(glm::mat4(1.), *cameraPositionPtr);
	glUniformMatrix4fv(skyShader.unifModel, 1, GL_FALSE, &model[0][0]);

	glDrawArrays(GL_TRIANGLES, 0, objectData.vertexAmount);
	triangleCount += objectData.vertexAmount / 3;
}

void Base3DRenderer::setLightDirection(const glm::vec3 &direction)
{
	for(int i = 0; i < shaderAmount; i++)
	{
		if(!shaders[i].isDirectionalLight)
			continue;

		glUseProgram(shaders[i].id);
		setCustomLightDirection(shaders[i], direction);
	}

	previousShader = -1; //Request to set appropriate shader when needed
}

void Base3DRenderer::setDiffuseLightColour(const glm::vec3 &colour)
{
	for(int i = 0; i < shaderAmount; i++)
	{
		if(!shaders[i].isDirectionalLight)
			continue;

		glUseProgram(shaders[i].id);
		setCustomDiffuseLightColour(shaders[i], colour);
	}

	previousShader = -1; //Request to set appropriate shader when needed
}

void Base3DRenderer::setAmbientLightColour(const glm::vec3 &colour)
{
	for(int i = 0; i < shaderAmount; i++)
	{
		if(!shaders[i].isDirectionalLight)
			continue;

		glUseProgram(shaders[i].id);
		setCustomAmbientLightColour(shaders[i], colour);
	}

	previousShader = -1; //Request to set appropriate shader when needed
}

void Base3DRenderer::updateCamera(const glm::mat4 &newViewMatrix)
{
	viewMatrix = newViewMatrix;
}

void Base3DRenderer::setVisibilityFlags(vector<VisibilityFlags> *visibility)
{
	visibilityFlagsPtr = visibility;
}

void Base3DRenderer::setCameraPosition(glm::vec3 *viewPosition)
{
	cameraPositionPtr = viewPosition;
}

void Base3DRenderer::setRenderingScene(RenderingScene *scene)
{
	renderingScene = scene;
}

RenderingScene* Base3DRenderer::getRenderingScene()
{
	return renderingScene;
}


int Base3DRenderer::getDrawnTriangleCount() const
{
	return triangleCount;
}

void Base3DRenderer::initialize(const map<int, unsigned long long> &shaderFlags, bool isDirectional)
{
	renderTerrain = isDirectional ? &Base3DRenderer::renderTerrainDirectional: &Base3DRenderer::renderTerrainPoint;

	for(int i = 0; i < RENDER_METHODS; i++)
		renderObjects[i] = nullptr;

	unsigned long long lightModelFlag = isDirectional ? ShaderFlags::FEATURE_DIRECTIONAL_LIGHT: ShaderFlags::FEATURE_POINT_LIGHT;
	for(auto [index, flags]: shaderFlags)
	{
		unsigned long long combinedFlags = flags | lightModelFlag;

		bool isShaderFound = false;

		if(combinedFlags & ShaderFlags::FEATURE_DIRECTIONAL_LIGHT)
		{
			if((combinedFlags & ShaderFlags::FEATURE_DIRECTIONAL_LIGHT) || (combinedFlags & ShaderFlags::FEATURE_SPECULAR))
			{
				renderObjects[index] = &Base3DRenderer::renderObjectsDirectional;
				isShaderFound = true;
			}

			if(combinedFlags & ShaderFlags::FEATURE_NORMALMAP)
			{
				renderObjects[index] = &Base3DRenderer::renderObjectsDirectionalNormalmap;
				isShaderFound = true;
			}

			if(combinedFlags & ShaderFlags::FEATURE_SMALL_WAVES)
			{
				renderObjects[index] = &Base3DRenderer::renderObjectsDirectionalWaves;
				isShaderFound = true;
			}

			if(combinedFlags & ShaderFlags::FEATURE_GLITTER)
			{
				renderObjects[index] = &Base3DRenderer::renderObjectsDirectionalGlitter;
				isShaderFound = true;
			}

			if(combinedFlags & ShaderFlags::FEATURE_INSTANCING)
			{
				renderParticles = &Base3DRenderer::renderObjectsDirectionalInstancing;
				isShaderFound = true;
			}
		}
		else if(combinedFlags & ShaderFlags::FEATURE_POINT_LIGHT)
		{
			if((combinedFlags & ShaderFlags::FEATURE_POINT_LIGHT) || (combinedFlags & ShaderFlags::FEATURE_SPECULAR)) //Both point-basic and point-specular
			{
				renderObjects[index] = &Base3DRenderer::renderObjectsPoint;
				isShaderFound = true;
			}

			if(combinedFlags & ShaderFlags::FEATURE_NORMALMAP)
			{
				renderObjects[index] = &Base3DRenderer::renderObjectsPointNormalmap;
				isShaderFound = true;
			}

			if(combinedFlags & ShaderFlags::FEATURE_SMALL_WAVES)
			{
				renderObjects[index] = &Base3DRenderer::renderObjectsPointWaves;
				isShaderFound = true;
			}

			if(combinedFlags & ShaderFlags::FEATURE_GLITTER)
			{
				renderObjects[index] = &Base3DRenderer::renderObjectsPointGlitter;
				isShaderFound = true;
			}

			if(combinedFlags & ShaderFlags::FEATURE_INSTANCING)
			{
				renderParticles = &Base3DRenderer::renderObjectsDirectionalInstancing;
				isShaderFound = true;
			}
		}

		if(!isShaderFound)
			Log::getInstance().error("Flag combination does not match any forward renderer method");
	}
}

void Base3DRenderer::copyShaderArray(const vector<ShaderIds> &shaderIds)
{
	shaderAmount = shaderIds.size();

	shaders = new ShaderIds [shaderAmount];
	for(int i = 0; i < shaderAmount; i++)
		shaders[i] = shaderIds[i];
}

/* rendering_scene_builder.cpp
 * Makes scene structure for renderer
 * OpenGL 3.3
 *
 * Author: Artem hiblov
 */

#include "graphics_lib/rendering_scene_builder.h"

#include <algorithm>
#include <vector>

#include <glm/gtc/matrix_transform.hpp>

#include "log.h"

using namespace std;
using namespace renderer;
using namespace renderer::data;
using namespace renderer::graphics_lib;
using namespace renderer::graphics_lib::videocard_data;
using namespace renderer::managers;

namespace
{
	struct QuadObjects
	{
		vector<ObjectQuadSubdivision::ObjectNode> quad[4];
	};

	struct QuadParticles
	{
		vector<ParticleQuadSubdivision::ParticleNode> quad[4];
	};

	const char *OBJECT_SKY_NAME = "sky";

	void arrangeTerrain(const Scene &scene, bool isDeferredRendering, TerrainManager *terrainManager, ShaderManager *shaderManager, RenderingScene *renderingScene);
	void arrangeObjects(const Scene &scene, bool isDeferredRendering, ObjectManager *objectManager, const map<int, ChunkMargins> &chunkMargins, ShaderManager *shaderManager, RenderingScene *renderingScene);
	void populateParticles(const Scene &scene, bool isDeferredRendering, ObjectManager *objectManager, ParticleManager *particleManager, const map<int, ChunkMargins> &chunkMargins, ShaderManager *shaderManager,
		RenderingScene *renderingScene);
	void createSky(const Scene &scene, ObjectManager *objectManager, RenderingScene *renderingScene);

	/*
	@brief Returns quad index for given coordinates
	*/
	int findQuad(const ChunkMargins &margins, float x, float z);

	void groupObjectsByShader(vector<ObjectQuadSubdivision::ObjectNode*> *objectsSorted, QuadObjects &quadObjects);
	void copyObjectsToRenderingScene(vector<ObjectQuadSubdivision::ObjectNode*> *objectsSorted, QuadObjects &quadObjects, int chunkIndex, RenderingObjects *renderingObjects);
}



RenderingScene* renderer::graphics_lib::makeRenderingScene(const Scene &scene, bool isDeferredRendering, TerrainManager *terrainManager, ObjectManager *objectManager, ParticleManager *particleManager,
	const map<int, ChunkMargins> &chunkMargins, ShaderManager *shaderManager)
{
	RenderingScene *renderingScene = new RenderingScene();

	renderingScene->chunkAmount = scene.chunks.size();

	arrangeTerrain(scene, isDeferredRendering, terrainManager, shaderManager, renderingScene);
	arrangeObjects(scene, isDeferredRendering, objectManager, chunkMargins, shaderManager, renderingScene);
	populateParticles(scene, isDeferredRendering, objectManager, particleManager, chunkMargins, shaderManager, renderingScene);
	createSky(scene, objectManager, renderingScene);

	return renderingScene;
}

void renderer::graphics_lib::updateRenderingSceneObjects(const Scene &scene, bool isDeferredRendering, ObjectManager *objectManager, const map<int, ChunkMargins> &chunkMargins,
	ShaderManager *shaderManager, RenderingScene *renderingScene)
{
	arrangeObjects(scene, isDeferredRendering, objectManager, chunkMargins, shaderManager, renderingScene);
}



namespace
{
	void arrangeTerrain(const Scene &scene, bool isDeferredRendering, TerrainManager *terrainManager, ShaderManager *shaderManager, RenderingScene *renderingScene)
	{
		renderingScene->terrain = new RenderingTerrain[renderingScene->chunkAmount];

		int i = 0;
		for(auto &currentPatch: scene.chunks)
		{
			glm::vec3 vecPosition(currentPatch.x, 0, currentPatch.z);
			glm::mat4 matPosition(glm::translate(glm::mat4(1.f), vecPosition));

			ObjectRenderingData data;
			terrainManager->getRenderingData(currentPatch.name, data);

			int shaderIndex = 0;
			bool status = shaderManager->getShaderIndexByProperty(scene.terrainTexturing, scene.fog.enable, isDeferredRendering, shaderIndex);
			if(!status)
			{
				Log::getInstance().error("Can't require shader for terrain");
			}

			renderingScene->terrain[i] = RenderingTerrain(shaderIndex, data, matPosition);

			i++;
		}
	}

	void arrangeObjects(const Scene &scene, bool isDeferredRendering, ObjectManager *objectManager, const map<int, ChunkMargins> &chunkMargins, ShaderManager *shaderManager,
		RenderingScene *renderingScene)
	{
		const int chunkAmount = renderingScene->chunkAmount;

		renderingScene->opaqueObjects = new RenderingObjects[chunkAmount];

		for(int chunkIndex = 0; chunkIndex < chunkAmount; chunkIndex++)
		{
			const int objectAmount = scene.instances[chunkIndex].size();
			QuadObjects opaqueObjects, transparentObjects;

			for(int i = 0; i < objectAmount; i++)
			{
				const InstanceArray &currentInstance = scene.instances[chunkIndex][i];

				//4 elements for 4 parts of chunk
				vector<glm::mat4> arrangement[4];
				vector<glm::mat3> rotation[4];

				int quadIndex = 0;
				const int positionArraySize = static_cast<int>(currentInstance.positions.size());
				for(int j = 0; j < positionArraySize; j += 4)
				{
					glm::mat4 translationMatrix = glm::translate(glm::mat4(1.), glm::vec3(currentInstance.positions[j], currentInstance.positions[j+1], currentInstance.positions[j+2]));
					glm::mat4 rotationMatrix = glm::rotate(glm::mat4(1.), glm::radians(currentInstance.positions[j+3]), glm::vec3(0, 1, 0));

					const auto iter = chunkMargins.find(chunkIndex);
					quadIndex = findQuad(iter->second, currentInstance.positions[j], currentInstance.positions[j+2]);

					arrangement[quadIndex].push_back(translationMatrix * rotationMatrix);
					rotation[quadIndex].push_back(rotationMatrix);
				}

				ObjectRenderingData data;
				bool status = objectManager->getRenderingData(currentInstance.name, data);
				if(!status)
				{
					Log::getInstance().error(string("Can't create object \"") + currentInstance.name + "\"");
					continue;
				}

				bool hasTransparentTexture = objectManager->isTextureTransparent(currentInstance.name);
				bool useDeferredRenderingShader = isDeferredRendering;
				if(useDeferredRenderingShader)
					useDeferredRenderingShader = !hasTransparentTexture;

				int shaderIndex = 0;
				status = shaderManager->getShaderIndexByProperty(currentInstance.shaderFeature, scene.fog.enable, useDeferredRenderingShader, shaderIndex);
				if(!status)
				{
					Log::getInstance().error(string("Can't find shader with property \"") + currentInstance.shaderFeature + "\" for object");
					continue;
				}

				if(hasTransparentTexture)
				{
					for(int quadIndex = 0; quadIndex < 4; quadIndex++)
					{
						if(arrangement[quadIndex].size())
							transparentObjects.quad[quadIndex].push_back(ObjectQuadSubdivision::ObjectNode(shaderIndex, data, arrangement[quadIndex].data(), rotation[quadIndex].data(),
								arrangement[quadIndex].size()));
					}

				}
				else
				{
					for(int quadIndex = 0; quadIndex < 4; quadIndex++)
					{
						if(arrangement[quadIndex].size())
							opaqueObjects.quad[quadIndex].push_back(ObjectQuadSubdivision::ObjectNode(shaderIndex, data, arrangement[quadIndex].data(), rotation[quadIndex].data(),
								arrangement[quadIndex].size()));
					}
				}
			}

			//Note. Texture transparency property is obtained inside the above loop so we know exact amount only here

			{
				vector<ObjectQuadSubdivision::ObjectNode*> opaqueObjectsSorted[4];
				groupObjectsByShader(opaqueObjectsSorted, opaqueObjects);
				copyObjectsToRenderingScene(opaqueObjectsSorted, opaqueObjects, chunkIndex, renderingScene->opaqueObjects);
			}

			vector<ObjectQuadSubdivision::ObjectNode*> transparentObjectsSorted[4];
			groupObjectsByShader(transparentObjectsSorted, transparentObjects);

			bool hasTransparentObjects = false;
			for(int i = 0; i < 4; i++)
			{
				if(!transparentObjects.quad[i].empty())
				{
					hasTransparentObjects = true;
					break;
				}
			}

			if(hasTransparentObjects)
			{
				renderingScene->transparentObjects = new RenderingObjects[chunkAmount];
				copyObjectsToRenderingScene(transparentObjectsSorted, transparentObjects, chunkIndex, renderingScene->transparentObjects);
			}
		}
	}

	void populateParticles(const Scene &scene, bool isDeferredRendering, ObjectManager *objectManager, ParticleManager *particleManager, const map<int, ChunkMargins> &chunkMargins,
		ShaderManager *shaderManager, RenderingScene *renderingScene)
	{
		const int chunkAmount = renderingScene->chunkAmount;

		renderingScene->particles = new RenderingParticles[chunkAmount];

		for(int chunkIndex = 0; chunkIndex < chunkAmount; chunkIndex++)
		{
			const int groupAmount = scene.particles[chunkIndex].size();
			QuadParticles particles;

			for(int i = 0; i < groupAmount; i++)
			{
				const ParticleSet &currentGroup = scene.particles[chunkIndex][i];

				const auto iter = chunkMargins.find(chunkIndex);
				const int quadIndex = findQuad(iter->second, currentGroup.x, currentGroup.z);

				ParticleRenderingData data;
				objectManager->getRenderingData(currentGroup.name, data.objectData);
				particleManager->getRenderingData(currentGroup, scene.chunks[chunkIndex], data); //TODO out arg for 4 quads for per-instance classification

				glm::mat4 arrangement = glm::translate(glm::mat4(1.f), glm::vec3(currentGroup.x, 0, currentGroup.z));

				int shaderIndex = 0;
				bool status = shaderManager->getShaderIndexByProperty(currentGroup.shaderFeature, scene.fog.enable, isDeferredRendering, shaderIndex);
				if(!status)
				{
					Log::getInstance().error(string("Can't find shader with property \"") + currentGroup.shaderFeature + "\" for particle group");
					continue;
				}

				particles.quad[quadIndex].push_back(ParticleQuadSubdivision::ParticleNode(shaderIndex, data, arrangement));
			}

			for(int quadIndex = 0; quadIndex < 4; quadIndex++)
			{
				const int groupsInQuad = particles.quad[quadIndex].size();
				renderingScene->particles[chunkIndex].quad[quadIndex].amount = groupsInQuad;
				if(groupsInQuad)
				{
					renderingScene->particles[chunkIndex].quad[quadIndex].groupsInQuad = new ParticleQuadSubdivision::ParticleNode[groupsInQuad];
					for(int i = 0; i < groupsInQuad; i++)
					{
						renderingScene->particles[chunkIndex].quad[quadIndex].groupsInQuad[i] = move(particles.quad[quadIndex][i]);
					}
				}
			}
		}
	}

	void createSky(const Scene &scene, ObjectManager *objectManager, RenderingScene *renderingScene)
	{
		ObjectRenderingData data;
		objectManager->getRenderingData(OBJECT_SKY_NAME, data);

		renderingScene->sky = data;
	}

	int findQuad(const ChunkMargins &margins, float x, float z)
	{
		if((x >= margins.leftX) && (x <= margins.centerX))
		{
			if((z <= margins.nearZ) && (z >= margins.centerZ))
				return 0;
			else if((z <= margins.centerZ) && (z >= margins.farZ))
				return 2;
			else
			{
				Log::getInstance().error("Quad not found");
				return 0;
			}
		}
		if((x >= margins.centerX) && (x <= margins.rightX))
		{
			if((z <= margins.nearZ) && (z >= margins.centerZ))
				return 1;
			else if((z <= margins.centerZ) && (z >= margins.farZ))
				return 3;
			else
			{
				Log::getInstance().error("Quad not found");
				return 0;
			}
		}

		Log::getInstance().error("Quad not found");
		return 0;
	}

	void groupObjectsByShader(vector<ObjectQuadSubdivision::ObjectNode*> *objectsSorted, QuadObjects &quadObjects)
	{
		for(int quadIndex = 0; quadIndex < 4; quadIndex++)
		{
			const size_t objectAmount = quadObjects.quad[quadIndex].size();
			for(size_t i = 0; i < objectAmount; i++)
			{
				objectsSorted[quadIndex].push_back(&(quadObjects.quad[quadIndex][i]));
			}

			if(!objectsSorted[quadIndex].empty())
			{
				const int curIndex = (objectsSorted[quadIndex][0])->shaderIndex;

				stable_partition(objectsSorted[quadIndex].begin(), objectsSorted[quadIndex].end(),
					[curIndex](ObjectQuadSubdivision::ObjectNode *current)
					{
						return current->shaderIndex == curIndex;
					});
			}
		}
	}

	void copyObjectsToRenderingScene(vector<ObjectQuadSubdivision::ObjectNode*> *objectsSorted, QuadObjects &quadObjects, int chunkIndex, RenderingObjects *renderingObjects)
	{
		for(int quadIndex = 0; quadIndex < 4; quadIndex++)
		{
			const int objectsInQuad = quadObjects.quad[quadIndex].size();
			renderingObjects[chunkIndex].quad[quadIndex].amount = objectsInQuad;
			if(objectsInQuad)
			{
				renderingObjects[chunkIndex].quad[quadIndex].objectsInQuad = new ObjectQuadSubdivision::ObjectNode[objectsInQuad];
				for(int i = 0; i < objectsInQuad; i++)
				{
					renderingObjects[chunkIndex].quad[quadIndex].objectsInQuad[i] = move(*(objectsSorted[quadIndex][i]));
				}
			}
		}
	}
}

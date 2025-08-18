/* deferred_renderer.cpp
 * Deferred renderer implementation
 * OpenGL 3.3
 *
 * Author: Artem hiblov
 */

#include "graphics_lib/deferred_renderer.h"

#include <GL/glew.h>
#include <glm/gtc/matrix_transform.hpp>

#include "common_constants.h"
#include "log.h"
#include "graphics_lib/light_setters.h"
#include "graphics_lib/operations/mesh_operations.h"
#include "utils/math_tools.h"

using namespace std;
using namespace renderer;
using namespace renderer::graphics_lib;
using namespace renderer::graphics_lib::operations;
using namespace renderer::graphics_lib::videocard_data;
using namespace renderer::utils;

DeferredRenderer::DeferredRenderer(const vector<ShaderIds> &shaderIds, const ShaderIds &sky, const map<int, unsigned long long> &shaderFlags, const ObjectRenderingData &sphere, bool isDirectional,
	bool isDeferredDirectional, bool isFog, const ShaderIds &directionalLightPass, const ShaderIds &stencilPass, const ShaderIds &pointLightPass, const glm::vec3 &lightPosition, int width, int height):
	Base3DRenderer(shaderIds, sky, shaderFlags, isDirectional), writeFramebufferId(0), framebufferWidth(width), framebufferHeight(height), useFog(isFog), directionalLightPassShaderId(directionalLightPass),
	stencilPassId(stencilPass), pointLightPassShaderId(pointLightPass), lightSphere(sphere), pointLightPosition(lightPosition), isDeferredLightDirectional(isDeferredDirectional)
{
	initializeBuffer();
	initializeQuadMesh(quadBufferId);
}

DeferredRenderer::~DeferredRenderer()
{
	glDeleteFramebuffers(1, &depthBufferId);
	glDeleteBuffers(1, &quadBufferId);
	glDeleteTextures(TOTAL_TEXTURES, textureIds);
	glDeleteFramebuffers(1, &framebufferId);
}

void DeferredRenderer::render()
{
	triangleCount = 0;
	previousShader = -1; //Other classes may set their own shaders

	glEnable(GL_DEPTH_TEST);
	glDepthMask(GL_TRUE);

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	renderOpaqueMeshes(); //Geometry pass

	glDepthMask(GL_FALSE);

	if(!isDeferredLightDirectional) //Stencil pass
		performStencilPass();

	glDisable(GL_DEPTH_TEST);

	//Light passes

	glBindFramebuffer(GL_FRAMEBUFFER, writeFramebufferId);
	glViewport(0, 0, framebufferWidth, framebufferHeight);

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	performDirectionalLightPass();

	if(!isDeferredLightDirectional)
		performPointLightPass();

	glBindFramebuffer(GL_READ_FRAMEBUFFER, framebufferId);
	glBindFramebuffer(GL_DRAW_FRAMEBUFFER, writeFramebufferId);
	glBlitFramebuffer(0, 0, framebufferWidth, framebufferHeight, 0, 0, framebufferWidth, framebufferHeight, GL_DEPTH_BUFFER_BIT, GL_NEAREST);
	glBindFramebuffer(GL_FRAMEBUFFER, writeFramebufferId);

	glEnable(GL_DEPTH_TEST);

	drawSkyAsForwardRendering();

	previousShader = -1; //After sky
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	renderTransparentMeshes(); //Forward rendering
	glDisable(GL_BLEND);

	glDisable(GL_DEPTH_TEST);
}

void DeferredRenderer::setTargetFramebuffer()
{
	glBindFramebuffer(GL_FRAMEBUFFER, framebufferId);
	glViewport(0, 0, framebufferWidth, framebufferHeight);
}

void DeferredRenderer::setWriteFramebufferId(unsigned int writeBuffer)
{
	writeFramebufferId = writeBuffer;
}

void DeferredRenderer::setLightDirection(const glm::vec3 &direction)
{
	Base3DRenderer::setLightDirection(direction);

	if(directionalLightPassShaderId.isDirectionalLight)
	{
		glUseProgram(directionalLightPassShaderId.id);
		setCustomLightDirection(directionalLightPassShaderId, direction);
	}
}

void DeferredRenderer::setDiffuseLightColour(const glm::vec3 &colour)
{
	Base3DRenderer::setDiffuseLightColour(colour);

	if(directionalLightPassShaderId.isDirectionalLight)
	{
		glUseProgram(directionalLightPassShaderId.id);
		setCustomDiffuseLightColour(directionalLightPassShaderId, colour);
	}
}

void DeferredRenderer::setAmbientLightColour(const glm::vec3 &colour)
{
	Base3DRenderer::setAmbientLightColour(colour);

	if(directionalLightPassShaderId.isDirectionalLight)
	{
		glUseProgram(directionalLightPassShaderId.id);
		setCustomAmbientLightColour(directionalLightPassShaderId, colour);
	}
}

void DeferredRenderer::initializeBuffer()
{
	glGenFramebuffers(1, &framebufferId);
	glBindFramebuffer(GL_FRAMEBUFFER, framebufferId);

	glGenTextures(TOTAL_TEXTURES, textureIds);
	for(int i = 0; i < TOTAL_TEXTURES; i++)
	{
		glBindTexture(GL_TEXTURE_2D, textureIds[i]);

		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB32F, framebufferWidth, framebufferHeight, 0, GL_RGB, GL_FLOAT, 0);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

		glFramebufferTexture(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0 + i, textureIds[i], 0);
	}

	glGenRenderbuffers(1, &depthBufferId);
	glBindRenderbuffer(GL_RENDERBUFFER, depthBufferId);
	glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, framebufferWidth, framebufferHeight); //The depth is 24-bit instead of 32 so that sky is drawn correctly
	glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, depthBufferId);

	GLenum drawBuffers[TOTAL_TEXTURES] = {GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1, GL_COLOR_ATTACHMENT2, GL_COLOR_ATTACHMENT3};
	glDrawBuffers(TOTAL_TEXTURES, drawBuffers); //Multiple elements in drawBuffers allows multi-render target

	if(glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
	{
		Log::getInstance().error("Can't initialize deferred renderer feature");
	}
}

void DeferredRenderer::performStencilPass()
{
	glEnable(GL_STENCIL_TEST);

	glUseProgram(stencilPassId.id);

	glDrawBuffer(GL_NONE);
	glDisable(GL_CULL_FACE);

	glStencilFunc(GL_ALWAYS, 0, 0);
	glStencilOpSeparate(GL_FRONT, GL_KEEP, GL_DECR_WRAP, GL_KEEP);
	glStencilOpSeparate(GL_BACK, GL_KEEP, GL_INCR_WRAP, GL_KEEP);

	glm::mat4 modelMatrix(1.);
	float sphereRadius = computeBoundingSphereRadius(POINT_LIGHT_POWER);
	modelMatrix = glm::scale(modelMatrix, glm::vec3(sphereRadius, sphereRadius, sphereRadius));
	modelMatrix = glm::translate(modelMatrix, pointLightPosition);
	glUniformMatrix4fv(pointLightPassShaderId.unifModel, 1, GL_FALSE, &modelMatrix[0][0]);
	glUniformMatrix4fv(pointLightPassShaderId.unifView, 1, GL_FALSE, &viewMatrix[0][0]);

	glBindVertexArray(lightSphere.vaoId);

	glEnableVertexAttribArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, lightSphere.vertexBufferId);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, (void*) nullptr);

	glDrawArrays(GL_TRIANGLES, 0, lightSphere.vertexAmount);

	glDisableVertexAttribArray(0);

	glEnable(GL_CULL_FACE);
	static const GLenum drawBuffers[TOTAL_TEXTURES] = {GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1, GL_COLOR_ATTACHMENT2, GL_COLOR_ATTACHMENT3};
	glDrawBuffers(TOTAL_TEXTURES, drawBuffers);

	glDisable(GL_STENCIL_TEST);
}

void DeferredRenderer::performDirectionalLightPass()
{
	glUseProgram(directionalLightPassShaderId.id);

	if(useFog)
	{
		glUniformMatrix4fv(directionalLightPassShaderId.unifView, 1, GL_FALSE, &viewMatrix[0][0]);

		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, textureIds[TEXTURE_INDEX_POSITION]);
		glUniform1i(directionalLightPassShaderId.unifPositionComponentId, 0);
	}

	glActiveTexture(GL_TEXTURE2);
	glBindTexture(GL_TEXTURE_2D, textureIds[TEXTURE_INDEX_NORMAL]);
	glUniform1i(directionalLightPassShaderId.unifNormalComponentId, 2);

	glActiveTexture(GL_TEXTURE3);
	glBindTexture(GL_TEXTURE_2D, textureIds[TEXTURE_INDEX_DIFFUSE]);
	glUniform1i(directionalLightPassShaderId.unifDiffuseComponentId, 3);

	glEnableVertexAttribArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, quadBufferId);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);

	glDrawArrays(GL_TRIANGLES, 0, 6);

	glDisableVertexAttribArray(0);
}

void DeferredRenderer::performPointLightPass()
{
	glUseProgram(pointLightPassShaderId.id);

	glStencilFunc(GL_NOTEQUAL, 0, 0xff);

	glEnable(GL_CULL_FACE);
	glCullFace(GL_FRONT);

	glEnable(GL_BLEND);
	glBlendEquation(GL_FUNC_ADD);
	glBlendFunc(GL_ONE, GL_ONE);

	glm::mat4 translationMatrix = glm::translate(glm::mat4(1.), pointLightPosition);
	glUniformMatrix4fv(pointLightPassShaderId.unifModel, 1, GL_FALSE, &translationMatrix[0][0]);
	glUniformMatrix4fv(pointLightPassShaderId.unifView, 1, GL_FALSE, &viewMatrix[0][0]);

	glBindVertexArray(lightSphere.vaoId);

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, textureIds[TEXTURE_INDEX_POSITION]);
	glUniform1i(pointLightPassShaderId.unifPositionComponentId, 0);

	glActiveTexture(GL_TEXTURE2);
	glBindTexture(GL_TEXTURE_2D, textureIds[TEXTURE_INDEX_NORMAL]);
	glUniform1i(pointLightPassShaderId.unifNormalComponentId, 2);

	glActiveTexture(GL_TEXTURE3);
	glBindTexture(GL_TEXTURE_2D, textureIds[TEXTURE_INDEX_DIFFUSE]);
	glUniform1i(pointLightPassShaderId.unifDiffuseComponentId, 3);

	glEnableVertexAttribArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, lightSphere.vertexBufferId);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, (void*) nullptr);

	glDrawArrays(GL_TRIANGLES, 0, lightSphere.vertexAmount);

	glDisableVertexAttribArray(0);

	glDisable(GL_BLEND);
	glCullFace(GL_BACK);
}

void DeferredRenderer::drawSkyAsForwardRendering()
{
	glDepthMask(GL_FALSE);
	glUseProgram(skyShader.id);
	glUniformMatrix4fv(skyShader.unifView, 1, GL_FALSE, &viewMatrix[0][0]);

	renderSky();

	glDepthMask(GL_TRUE);
}

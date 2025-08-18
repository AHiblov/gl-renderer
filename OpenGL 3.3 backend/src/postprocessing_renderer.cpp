/* postprocessing_renderer.cpp
 * Applies 2D fullscreen effects on framebuffer
 * OpenGL 3.3
 *
 * Author: Artem Hiblov
 */

#include "graphics_lib/postprocessing_renderer.h"

#include <GL/glew.h>

#include "log.h"
#include "graphics_lib/operations/mesh_operations.h"

using namespace renderer;
using namespace renderer::graphics_lib;
using namespace renderer::graphics_lib::operations;
using namespace renderer::graphics_lib::videocard_data;

PostprocessingRenderer::PostprocessingRenderer(PostprocessingShaderIds &ids, int width, int height):
	postprocessingIds(ids), framebufferWidth(width), framebufferHeight(height)
{
	initializeFramebuffer();
	initializeQuadMesh(quadBufferId);
}

PostprocessingRenderer::~PostprocessingRenderer()
{
	glDeleteFramebuffers(1, &depthBufferId);
	glDeleteBuffers(1, &quadBufferId);
	glDeleteTextures(1, &textureId);
	glDeleteFramebuffers(1, &framebufferId);
}

void PostprocessingRenderer::render()
{
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	glViewport(0, 0, framebufferWidth, framebufferHeight);

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glUseProgram(postprocessingIds.quadShaderId);

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, textureId);
	glUniform1i(postprocessingIds.unifTextureId, 0);

	glEnableVertexAttribArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, quadBufferId);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);

	glDrawArrays(GL_TRIANGLES, 0, 6); //6 vertices -> 2 triangles

	glDisableVertexAttribArray(0);
}

void PostprocessingRenderer::setTargetFramebuffer()
{
	glBindFramebuffer(GL_FRAMEBUFFER, framebufferId);
	glViewport(0, 0, framebufferWidth, framebufferHeight);
}

unsigned int PostprocessingRenderer::getFramebufferId() const
{
	return framebufferId;
}

void PostprocessingRenderer::initializeFramebuffer()
{
	glGenFramebuffers(1, &framebufferId);
	glBindFramebuffer(GL_FRAMEBUFFER, framebufferId);

	glGenTextures(1, &textureId);
	glBindTexture(GL_TEXTURE_2D, textureId);

	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, framebufferWidth, framebufferHeight, 0, GL_RGB, GL_UNSIGNED_BYTE, 0);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

	glGenRenderbuffers(1, &depthBufferId);
	glBindRenderbuffer(GL_RENDERBUFFER, depthBufferId);
	glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT, framebufferWidth, framebufferHeight);
	glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, depthBufferId);

	glFramebufferTexture(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, textureId, 0);

	GLenum drawBuffers[1] = {GL_COLOR_ATTACHMENT0};
	glDrawBuffers(1, drawBuffers);

	if(glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
	{
		Log::getInstance().error("Can't initialize render to texture feature");
	}
}

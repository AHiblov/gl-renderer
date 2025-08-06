/* postprocessing_renderer.cpp
 * Applies 2D fullscreen effects on framebuffer
 * OpenGL 4.5
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

namespace
{
	constexpr float colourClearValue[4] = {0., 0., 0., 1.};
	constexpr float depthClearValue = 1.;
}

PostprocessingRenderer::PostprocessingRenderer(PostprocessingShaderIds &ids, int width, int height):
	postprocessingIds(ids), framebufferWidth(width), framebufferHeight(height)
{
	initializeFramebuffer();
	initializeQuadMesh(quadVaoId);
}

PostprocessingRenderer::~PostprocessingRenderer()
{
	glDeleteTextures(1, &depthTextureId);
	glDeleteVertexArrays(1, &quadVaoId);
	glDeleteTextures(1, &textureId);
	glDeleteFramebuffers(1, &framebufferId);
}

void PostprocessingRenderer::render()
{
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	glViewport(0, 0, framebufferWidth, framebufferHeight);

	glClearBufferfv(GL_COLOR, 0, colourClearValue);
	glClearBufferfv(GL_DEPTH, 0, &depthClearValue);

	glUseProgram(postprocessingIds.quadShaderId);

	glBindVertexArray(quadVaoId);
	glBindTextureUnit(0, textureId);

	glUniform1i(postprocessingIds.unifTextureId, 0);

	glDrawArrays(GL_TRIANGLES, 0, 6); //6 vertices -> 2 triangles
}

void PostprocessingRenderer::setTargetFramebuffer()
{
	glBindFramebuffer(GL_FRAMEBUFFER, framebufferId);
	glViewport(0, 0, framebufferWidth, framebufferHeight);

	glClearBufferfv(GL_COLOR, 0, colourClearValue);
	glClearBufferfv(GL_DEPTH, 0, &depthClearValue);
}

unsigned int PostprocessingRenderer::getFramebufferId() const
{
	return framebufferId;
}

void PostprocessingRenderer::initializeFramebuffer()
{
	glCreateFramebuffers(1, &framebufferId);

	glCreateTextures(GL_TEXTURE_2D, 1, &textureId);
	glTextureStorage2D(textureId, 1, GL_RGB8, framebufferWidth, framebufferHeight);
	glTextureParameteri(textureId, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTextureParameteri(textureId, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTextureParameteri(textureId, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTextureParameteri(textureId, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

	glNamedFramebufferTexture(framebufferId, GL_COLOR_ATTACHMENT0, textureId, 0);

	glCreateTextures(GL_TEXTURE_2D, 1, &depthTextureId);
	glTextureStorage2D(depthTextureId, 1, GL_DEPTH24_STENCIL8, framebufferWidth, framebufferHeight);

	glNamedFramebufferTexture(framebufferId, GL_DEPTH_STENCIL_ATTACHMENT, depthTextureId, 0);

	GLenum drawBuffers[1] = { GL_COLOR_ATTACHMENT0 };
	glNamedFramebufferDrawBuffers(framebufferId, 1, drawBuffers);
}

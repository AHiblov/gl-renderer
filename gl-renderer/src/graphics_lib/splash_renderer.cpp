/* splash_renderer.cpp
 * Splash screen component
 * OpenGL 4.5
 *
 * Author: Artem Hiblov
 */

#include "graphics_lib/splash_renderer.h"

#include <GL/glew.h>

using namespace renderer::graphics_lib;
using namespace renderer::graphics_lib::videocard_data;

SplashRenderer::SplashRenderer(const ObjectRenderingData &data, const ShaderIds &id):
	objectData(data), shaderId(id)
{
}

SplashRenderer::~SplashRenderer()
{
}

void SplashRenderer::render()
{
	glUseProgram(shaderId.id);

	glClearColor(0.09, 0.09, 0.09, 0);
	glClear(GL_COLOR_BUFFER_BIT);

	glBindVertexArray(objectData.vaoId);
	glBindTextureUnit(0, objectData.textureId);

	glUniform1i(shaderId.unifTextureId, 0);

	glDrawArrays(GL_TRIANGLES, 0, objectData.vertexAmount);
}

/* splash_renderer.cpp
 * Splash screen component
 * OpenGL 3.3
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

	glEnableVertexAttribArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, objectData.vertexBufferId);
	glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 0, (void*) nullptr);

	glEnableVertexAttribArray(1);
	glBindBuffer(GL_ARRAY_BUFFER, objectData.uvBufferId);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 0, (void*) nullptr);

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, objectData.textureId);
	glUniform1i(shaderId.unifTextureId, 0);

	glDrawArrays(GL_TRIANGLES, 0, objectData.vertexAmount);

	glDisableVertexAttribArray(1);
	glDisableVertexAttribArray(0);
}

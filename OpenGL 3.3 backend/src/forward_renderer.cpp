/* forward_renderer.cpp
 * Forward renderer implementation
 * OpenGL 3.3
 *
 * Author: Artem Hiblov
 */

#include "graphics_lib/forward_renderer.h"

#include <GL/glew.h>

using namespace std;
using namespace renderer::graphics_lib;
using namespace renderer::graphics_lib::videocard_data;

ForwardRenderer::ForwardRenderer(const vector<ShaderIds> &shaderIds, const ShaderIds &sky, const map<int, unsigned long long> &shaderFlags, bool isDirectional):
	Base3DRenderer(shaderIds, sky, shaderFlags, isDirectional)
{
}

ForwardRenderer::~ForwardRenderer()
{
}

void ForwardRenderer::render()
{
	triangleCount = 0;
	previousShader = -1; //Other classes may set their own shaders

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	renderOpaqueMeshes();

	//Sky

	glDepthMask(GL_FALSE);
	glUseProgram(skyShader.id);
	glUniformMatrix4fv(skyShader.unifView, 1, GL_FALSE, &viewMatrix[0][0]);

	renderSky();

	glDepthMask(GL_TRUE);

	previousShader = -1; //After sky
	glEnable(GL_BLEND);
	renderTransparentMeshes();
	glDisable(GL_BLEND);
}

void ForwardRenderer::setTargetFramebuffer()
{
}

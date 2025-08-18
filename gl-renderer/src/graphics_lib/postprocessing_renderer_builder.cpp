/* postprocessing_renderer_builder.cpp
 * Initializes all the shaders for PostprocessingRenderer class
 * OpenGL 4.5
 *
 * Author: Artem Hiblov
 */

#include "graphics_lib/postprocessing_renderer_builder.h"

#include <GL/glew.h>
#include <glm/glm.hpp>

#include "graphics_lib/videocard_data/postprocessing_shader_ids.h"

using namespace std;
using namespace renderer::data;
using namespace renderer::graphics_lib;
using namespace renderer::graphics_lib::videocard_data;

namespace
{
	const char *RENDERED_TEXTURE_UNIFORM_NAME = "renderedTexture";
	const char *POSTPROCESSING_SCREEN_RATIO_UNIFORM_NAME = "screenRatio";
	const char *POSTPROCESSING_SCREEN_WIDTH_HEIGHT_UNIFORM_NAME = "screenWidthHeight"; //x = width, y = height
}

unique_ptr<PostprocessingRenderer> renderer::graphics_lib::buildPostprocessingRenderer(float screenWidth, float screenHeight, const string &postprocessingEffect, ShaderManager *shaderManager)
{
	PostprocessingShaderIds *postprocessingId = nullptr;
	bool status = shaderManager->getPostprocessingShaderId(postprocessingEffect, &postprocessingId);
	if(!status)
		return nullptr;

	glUseProgram(postprocessingId->quadShaderId);

	postprocessingId->unifTextureId = glGetUniformLocation(postprocessingId->quadShaderId, RENDERED_TEXTURE_UNIFORM_NAME);

	unsigned long long flags = shaderManager->getPostprocessingShaderFlags(postprocessingEffect);
	if((flags & PostprocessingFlags::POSTPROCESSING_VIGNETTE) || (flags & PostprocessingFlags::POSTPROCESSING_DROPS_ON_LENS))
	{
		postprocessingId->unifScreenRatio = glGetUniformLocation(postprocessingId->quadShaderId, POSTPROCESSING_SCREEN_RATIO_UNIFORM_NAME);
		float screenRatio = static_cast<float>(screenWidth) / screenHeight;
		glUniform1f(postprocessingId->unifScreenRatio, screenRatio);
	}

	if(flags & PostprocessingFlags::POSTPROCESSING_EMBOSS)
	{
		postprocessingId->unifScreenWidthHeight = glGetUniformLocation(postprocessingId->quadShaderId, POSTPROCESSING_SCREEN_WIDTH_HEIGHT_UNIFORM_NAME);
		glm::vec2 dimensions(screenWidth, screenHeight);
		glUniform2fv(postprocessingId->unifScreenWidthHeight, 1, &dimensions[0]);
	}

	unique_ptr<PostprocessingRenderer> postprocessingRenderer = make_unique<PostprocessingRenderer>(*postprocessingId, screenWidth, screenHeight);

	return postprocessingRenderer;
}

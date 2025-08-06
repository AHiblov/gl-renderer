/* splash_renderer_builder.cpp
 * Initializes all the shaders for SplashRenderer class
 * OpenGL 4.5
 *
 * Author: Artem Hiblov
 */

#include "graphics_lib/splash_renderer_builder.h"

#include <GL/glew.h>

#include "log.h"
#include "graphics_lib/videocard_data/shader_ids.h"

using namespace std;
using namespace renderer;
using namespace renderer::graphics_lib;
using namespace renderer::graphics_lib::videocard_data;
using namespace renderer::managers;

namespace
{
	const char *OBJECT_SPLASH_NAME = "splash";

	const char *SHADER_2D = "2D";
	const char *SCREEN_RATIO_UNIFORM_NAME = "screenRatio";


	void initSplashUniforms(ShaderIds &shaderId);
	void setSplashUniformsData(ShaderIds &shaderId, float screenRatio);
}

unique_ptr<SplashRenderer> renderer::graphics_lib::buildSplashRenderer(ObjectManager *objectManager, ShaderManager *shaderManager, float screenRatio)
{
	ObjectRenderingData splashData;
	bool status = objectManager->getRenderingData(OBJECT_SPLASH_NAME, splashData);
	if(!status)
	{
		Log::getInstance().error("Can't initialize splash screen data");
	}

	ShaderIds splashShaderId;

	shaderManager->getShaderId(SHADER_2D, splashShaderId);

	glUseProgram(splashShaderId.id);
	initSplashUniforms(splashShaderId);
	setSplashUniformsData(splashShaderId, screenRatio);

	unique_ptr<SplashRenderer> splashRenderer = make_unique<SplashRenderer>(splashData, splashShaderId);

	return splashRenderer;
}

namespace
{
	void initSplashUniforms(ShaderIds &shaderId)
	{
		shaderId.unifScreenRatio = glGetUniformLocation(shaderId.id, SCREEN_RATIO_UNIFORM_NAME);
	}

	void setSplashUniformsData(ShaderIds &shaderId, float screenRatio)
	{
		glUniform1f(shaderId.unifScreenRatio, screenRatio);
	}
}

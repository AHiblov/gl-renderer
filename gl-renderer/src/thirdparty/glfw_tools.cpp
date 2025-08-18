/* glfw_tools.cpp
 * GLFW library initialization and deinitialization
 *
 * Author: Artem Hiblov
 */

#include "thirdparty/glfw_tools.h"

#include "log.h"

using namespace renderer;
using namespace renderer::thirdparty;

namespace
{
	constexpr int CONTEXT_GLFW_SAMPLES_AMOUNT = 4;
	constexpr int CONTEXT_GLFW_OGL_MAJOR_VERSION = 4;
	constexpr int CONTEXT_GLFW_OGL_MINOR_VERSION = 5;

	const char *CONTEXT_GLFW_WINDOW_TITLE = "3D Real-time Renderer";
}

bool renderer::thirdparty::initGlfw(const AppParameters &appParameters, GLFWwindow **window)
{
	GLFWwindow *localWindow = nullptr;

	if(!window)
	{
		Log::getInstance().error("Pointer to window isn't provided");
		return false;
	}

	if(!glfwInit())
	{
		Log::getInstance().error("Can't initialize GLFW");
		return false;
	}

	if(appParameters.useSmoothing)
		glfwWindowHint(GLFW_SAMPLES, CONTEXT_GLFW_SAMPLES_AMOUNT);

	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, CONTEXT_GLFW_OGL_MAJOR_VERSION);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, CONTEXT_GLFW_OGL_MINOR_VERSION);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	localWindow = glfwCreateWindow(appParameters.screenWidth, appParameters.screenHeight, CONTEXT_GLFW_WINDOW_TITLE,
		appParameters.isFullScreen ? glfwGetPrimaryMonitor(): nullptr, nullptr);
	if(!localWindow)
	{
		Log::getInstance().error("Can't create window");
		return false;
	}

	glfwMakeContextCurrent(localWindow);

	glfwSetInputMode(localWindow, GLFW_STICKY_KEYS, GL_TRUE);
	glfwSwapInterval(0); //0 = off, 1 = on (<= 60 FPS)

	*window = localWindow;
	return true;
}

void renderer::thirdparty::shutdownGlfw()
{
	glfwTerminate();
}

/* glfw_tools.h
 * GLFW library initialization and deinitialization
 *
 * Author: Artem Hiblov
 */

#pragma once

#include <GLFW/glfw3.h>

#include "app_parameters.h"

namespace renderer::thirdparty
{

/*
@brief Initializes GLFW
*/
bool initGlfw(const renderer::AppParameters &appParameters, GLFWwindow **window);

/*
@brief Deinitializes GLFW
*/
void shutdownGlfw();

}

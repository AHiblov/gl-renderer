/* imgui_tools.h
 * ImGui library initialization and deinitialization
 *
 * Author: Artem Hiblov
 */

#pragma once

#include <GLFW/glfw3.h>

namespace renderer::thirdparty
{

/*
@brief Initializes ImGui library
@param[in] window - existing GLFW window
*/
void initImgui(GLFWwindow *window);

/*
@brief Deinitializes ImGui
*/
void shutdownImgui();

}

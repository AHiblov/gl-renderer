/* imgui_tools.cpp
 * ImGui library initialization and deinitialization
 *
 * Author: Artem Hiblov
 */

#include "thirdparty/imgui_tools.h"

#include <imgui.h>
#include <imgui_impl_glfw.h>
#include <imgui_impl_opengl3.h>

using namespace renderer::thirdparty;

void renderer::thirdparty::initImgui(GLFWwindow *window)
{
	IMGUI_CHECKVERSION();

	ImGui::CreateContext();

	ImGuiIO &imGuiIo = ImGui::GetIO();

	//ImGui::StyleColorsDark();

	ImGui_ImplGlfw_InitForOpenGL(window, true);
    ImGui_ImplOpenGL3_Init("#version 330");
}

void renderer::thirdparty::shutdownImgui()
{
	ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();
}

/* frame_renderer.cpp
 * Dispatches single frame drawing
 * OpenGL 4.5
 *
 * Author: Artem Hiblov
 */

#include "graphics_lib/frame_renderer.h"

#include <cstring>

#include <GL/glew.h>
#include <imgui.h>
#include <imgui_impl_glfw.h>
#include <imgui_impl_opengl3.h>

using namespace std;
using namespace renderer::data;
using namespace renderer::graphics_lib;

namespace
{
	const ImVec2 THIRDPARTY_FRAME_POSITION(20., 20.);
	const ImVec2 THIRDPARTY_FRAME_SIZE(250., 70.);
	const char *THIRDPARTY_FRAME_TITLE = "Statistics";
}

FrameRenderer::FrameRenderer(Base3DRenderer *mainRend, PostprocessingRenderer *postprocessingRend, bool isDeferred):
	mainRenderer(mainRend), postprocessingRenderer(postprocessingRend), isDeferredRenderer(isDeferred)
{
	statisticsString[0] = '\0';
	simulationString[0] = '\0';
}

FrameRenderer::~FrameRenderer()
{
	if(mainRenderer)
	{
		delete mainRenderer;
		mainRenderer = nullptr;
	}

	if(postprocessingRenderer)
	{
		delete postprocessingRenderer;
		postprocessingRenderer = nullptr;
	}
}

void FrameRenderer::renderFrame()
{
	mainRenderer->setTargetFramebuffer();
	if(!isDeferredRenderer && postprocessingRenderer)
		postprocessingRenderer->setTargetFramebuffer();

	mainRenderer->render();

	if(postprocessingRenderer)
		postprocessingRenderer->render();
}

void FrameRenderer::renderUi()
{
	ImGui_ImplOpenGL3_NewFrame();
	ImGui_ImplGlfw_NewFrame();

	ImGui::NewFrame();

	ImGui::SetNextWindowPos(THIRDPARTY_FRAME_POSITION, ImGuiCond_Once);
	ImGui::SetNextWindowSize(THIRDPARTY_FRAME_SIZE, ImGuiCond_Once);
	ImGui::Begin(THIRDPARTY_FRAME_TITLE, nullptr, ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize);

	ImGui::Text(statisticsString);
	ImGui::Text(simulationString);

	ImGui::End();

	ImGui::Render();
	ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
}

void FrameRenderer::updateCamera(const glm::mat4 &newViewMatrix)
{
	mainRenderer->updateCamera(newViewMatrix);
}

void FrameRenderer::setLightDirection(const glm::vec3 &direction)
{
	mainRenderer->setLightDirection(direction);
}

void FrameRenderer::setDiffuseLightColour(const glm::vec3 &colour)
{
	mainRenderer->setDiffuseLightColour(colour);
}

void FrameRenderer::setAmbientLightColour(const glm::vec3 &colour)
{
	mainRenderer->setAmbientLightColour(colour);
}

void FrameRenderer::setVisibilityFlags(vector<VisibilityFlags> *visibility)
{
	mainRenderer->setVisibilityFlags(visibility);
}

void FrameRenderer::setCameraPosition(glm::vec3 *viewPosition)
{
	mainRenderer->setCameraPosition(viewPosition);
}

void FrameRenderer::setRendererLibraryProperties()
{
	glClearColor(0.f / 255.f, 162.f / 255.f, 232.f / 255.f, 1.0);
	glEnable(GL_CULL_FACE);
	glCullFace(GL_BACK);
	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LEQUAL);

	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
}

int FrameRenderer::getDrawnTriangleCount() const
{
	return mainRenderer->getDrawnTriangleCount();
}

void FrameRenderer::setStatisticsLine(const string &str)
{
	strncpy(statisticsString, str.c_str(), UI_STR_MAX_LENGTH - 1);
}

void FrameRenderer::setSimulationLine(const std::string &str)
{
	strncpy(simulationString, str.c_str(), UI_STR_MAX_LENGTH - 1);
}

/* editor_frame_renderer.cpp
 * FrameRenderer class extension for editor
 * OpenGL 3.3
 *
 * Author: Artem Hiblov
 */

#include "graphics_lib/editor_frame_renderer.h"

#include <cstring>

#include <GL/glew.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include <imgui.h>
#include <imgui_impl_glfw.h>
#include <imgui_impl_opengl3.h>

using namespace std;
using namespace renderer::graphics_lib;
using namespace renderer::graphics_lib::videocard_data;

namespace
{
	const char *EDITOR_SHADER_NAME = "directional-forward";

	const ImVec2 THIRDPRT_EDITOR_FRAME_POSITION(20., 20.);
	const ImVec2 THIRDPRT_EDITOR_FRAME_SIZE(450., 90.);
	const char *THIRDPRT_EDITOR_FRAME_TITLE = "Editor";
}

EditorFrameRenderer::EditorFrameRenderer(Base3DRenderer *mainRend, PostprocessingRenderer *postprocessingRend, bool isDeferred, ShaderManager *shaderMgr):
	FrameRenderer(mainRend, postprocessingRend, isDeferred), editorIsInstanceSelected(false), objectRenderingData(nullptr), editorSelectedObjectIndex(0),
	areaMarkers(nullptr), markerAmount(0), markerRenderingData(nullptr), shaderManager(shaderMgr)
{
	editorModeString[0] = '\0';
	isAnyInstanceActive = false;
	editorActiveInstanceString[0] = '\0';
}

void EditorFrameRenderer::renderFrame()
{
	FrameRenderer::renderFrame();

	if(objectRenderingData || areaMarkers)
	{
		glUseProgram(editorShaderId.id);
	}

	if(objectRenderingData)
	{
		renderSelectedInstance();
	}

	if(areaMarkers)
	{
		renderMarkers();
	}
}

void EditorFrameRenderer::setSelectedInstanceData(const glm::mat4 &selectedInstanceModel, const glm::mat3 &selectedInstanceRotation, int selectedObjectIndex)
{
	editorSelectedInstanceModel = selectedInstanceModel;
	editorSelectedInstanceRotation = selectedInstanceRotation;
	editorSelectedObjectIndex = selectedObjectIndex;

	editorIsInstanceSelected = true;
}

void EditorFrameRenderer::setObjectRenderingData(ObjectRenderingData *objectData)
{
	objectRenderingData = objectData;
}

void EditorFrameRenderer::setMarkerRenderingData(float *markers, int amount, ObjectRenderingData *markerData)
{
	areaMarkers = markers;
	markerAmount = amount;
	markerRenderingData = markerData;
}

void EditorFrameRenderer::setRenderingScene(RenderingScene *scene)
{
	mainRenderer->setRenderingScene(scene);
}

RenderingScene* EditorFrameRenderer::getRenderingScene()
{
	return mainRenderer->getRenderingScene();
}

void EditorFrameRenderer::initEditorShader()
{
	shaderManager->getEditorShaderId(editorShaderId);
}

void EditorFrameRenderer::setModeLine(const string &mode)
{
	strncpy(editorModeString, mode.c_str(), EDITOR_MODE_STR_LENGTH - 1);
}

void EditorFrameRenderer::setActiveInstanceLine(const std::string &activeInstance)
{
	strncpy(editorActiveInstanceString, activeInstance.c_str(), EDITOR_ACTIVE_INSTANCE_STR_LENGTH - 1);
}

void EditorFrameRenderer::showActiveInstanceLine()
{
	isAnyInstanceActive = true;
}

void EditorFrameRenderer::hideActiveInstanceLine()
{
	isAnyInstanceActive = false;
}

void EditorFrameRenderer::renderEditorUi()
{
	ImGui_ImplOpenGL3_NewFrame();
	ImGui_ImplGlfw_NewFrame();

	ImGui::NewFrame();

	ImGui::SetNextWindowPos(THIRDPRT_EDITOR_FRAME_POSITION, ImGuiCond_Once);
	ImGui::SetNextWindowSize(THIRDPRT_EDITOR_FRAME_SIZE, ImGuiCond_Once);
	ImGui::Begin(THIRDPRT_EDITOR_FRAME_TITLE, nullptr, ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize);

	ImGui::Text(statisticsString);
	ImGui::Text(editorModeString);
	if(isAnyInstanceActive)
		ImGui::Text(editorActiveInstanceString);

	ImGui::End();

	ImGui::Render();
	ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
}

void EditorFrameRenderer::renderSelectedInstance()
{
	auto &objectData = *objectRenderingData;

	glBindVertexArray(objectData.vaoId);

	glEnableVertexAttribArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, objectData.vertexBufferId);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, (void*) nullptr);

	glEnableVertexAttribArray(1);
	glBindBuffer(GL_ARRAY_BUFFER, objectData.uvBufferId);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 0, (void*) nullptr);

	glEnableVertexAttribArray(2);
	glBindBuffer(GL_ARRAY_BUFFER, objectData.normalBufferId);
	glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, 0, (void*) nullptr);

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, objectData.textureId);
	glUniform1i(editorShaderId.unifTextureId, 0);

	glUniformMatrix4fv(editorShaderId.unifModel, 1, GL_FALSE, &editorSelectedInstanceModel[0][0]);
	glUniformMatrix3fv(editorShaderId.unifRotation, 1, GL_FALSE, &editorSelectedInstanceRotation[0][0]);

	glDrawArrays(GL_TRIANGLES, 0, objectData.vertexAmount);

	glDisableVertexAttribArray(2);
	glDisableVertexAttribArray(1);
	glDisableVertexAttribArray(0);
}

void EditorFrameRenderer::renderMarkers()
{
	auto &objectData = *markerRenderingData;

	glBindVertexArray(objectData.vaoId);

	glEnableVertexAttribArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, objectData.vertexBufferId);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, (void*) nullptr);

	glEnableVertexAttribArray(1);
	glBindBuffer(GL_ARRAY_BUFFER, objectData.uvBufferId);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 0, (void*) nullptr);

	glEnableVertexAttribArray(2);
	glBindBuffer(GL_ARRAY_BUFFER, objectData.normalBufferId);
	glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, 0, (void*) nullptr);

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, objectData.textureId);
	glUniform1i(editorShaderId.unifTextureId, 0);

	for(int i = 0; i < markerAmount; i++)
	{
		glm::mat4 modelMatrix = glm::translate(glm::mat4(1.), glm::vec3(areaMarkers[i*3], areaMarkers[i*3+1], areaMarkers[i*3+2]));
		glUniformMatrix4fv(editorShaderId.unifModel, 1, GL_FALSE, &modelMatrix[0][0]);

		glm::mat3 unitMatrix(1.f);
		glUniformMatrix3fv(editorShaderId.unifRotation, 1, GL_FALSE, &unitMatrix[0][0]);

		glDrawArrays(GL_TRIANGLES, 0, objectData.vertexAmount);
		//triangleCount += objectData.vertexAmount / 3;
	}

	glDisableVertexAttribArray(2);
	glDisableVertexAttribArray(1);
	glDisableVertexAttribArray(0);
}

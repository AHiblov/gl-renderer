/* editor_frame_renderer.h
 * FrameRenderer class extension for editor
 * OpenGL 4.5
 *
 * Author: Artem Hiblov
 */

#pragma once

#include "graphics_lib/frame_renderer.h"
#include "graphics_lib/forward_renderer.h"
#include "graphics_lib/postprocessing_renderer.h"
#include "graphics_lib/shader_manager.h"
#include "graphics_lib/videocard_data/object_rendering_data.h"

namespace renderer::graphics_lib
{

class EditorFrameRenderer: public FrameRenderer
{
	static constexpr int EDITOR_MODE_STR_LENGTH = 40;
	static constexpr int EDITOR_ACTIVE_INSTANCE_STR_LENGTH = 40;

public:
	EditorFrameRenderer(renderer::graphics_lib::Base3DRenderer *mainRend, renderer::graphics_lib::PostprocessingRenderer *postprocessingRend, bool isDeferred,
		renderer::graphics_lib::ShaderManager *shaderMgr);

	void renderFrame();

	/*
	@brief Copies instance rendering data to class properties
	*/
	void setSelectedInstanceData(const glm::mat4 &selectedInstanceModel, const glm::mat3 &selectedInstanceRotation, int selectedObjectIndex);

	/*
	@brief draw markers for group insertion
	*/
	void renderMarkers();

	/*
	@brief Copies object data
	*/
	void setObjectRenderingData(renderer::graphics_lib::videocard_data::ObjectRenderingData *objectData);

	/*
	@brief Draws markers on terrain
	@param[in] markers - array of markers
	@param[in] amount - marker amount
	@param[in] markerData - component ids for marker
	*/
	void setMarkerRenderingData(float *markers, int amount, renderer::graphics_lib::videocard_data::ObjectRenderingData *markerData);

	void setRenderingScene(renderer::graphics_lib::videocard_data::RenderingScene *scene);
	renderer::graphics_lib::videocard_data::RenderingScene* getRenderingScene();

	/*
	@brief Initializes editor shader pointer
	*/
	void initEditorShader();

	/*
	@brief Sets editor mode string
	*/
	void setModeLine(const std::string &mode);

	/*
	@brief Sets active instance string
	*/
	void setActiveInstanceLine(const std::string &activeInstance);

	/*
	@brief Shows unchanged active instance string
	*/
	void showActiveInstanceLine();

	/*
	@brief Hides active instance string
	*/
	void hideActiveInstanceLine();

	/*
	@brief Renders editor UI for one frame
	*/
	void renderEditorUi();

private:
	/*
	@brief Renders instance selected for modifying
	*/
	void renderSelectedInstance();



	bool editorIsInstanceSelected;
	renderer::graphics_lib::videocard_data::ObjectRenderingData *objectRenderingData;
	glm::mat4 editorSelectedInstanceModel;
	glm::mat3 editorSelectedInstanceRotation;
	int editorSelectedObjectIndex;

	float *areaMarkers;
	int markerAmount;
	renderer::graphics_lib::videocard_data::ObjectRenderingData *markerRenderingData;

	renderer::graphics_lib::videocard_data::ShaderIds editorShaderId;

	renderer::graphics_lib::ShaderManager *shaderManager;

	char editorModeString[EDITOR_MODE_STR_LENGTH];
	bool isAnyInstanceActive;
	char editorActiveInstanceString[EDITOR_ACTIVE_INSTANCE_STR_LENGTH];
};

}

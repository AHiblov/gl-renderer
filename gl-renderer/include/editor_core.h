/* editor_core.h
 * Editor core module
 *
 * Author: Artem Hiblov
 */

#pragma once

#include <memory>
#include <string>
#include <vector>

#include <GLFW/glfw3.h>
#include <glm/glm.hpp>

#include "core.h"
#include "editor_scene_modifier.h"
#include "graphics_lib/editor_frame_renderer.h"
#include "graphics_lib/shader_manager.h"
#include "managers/object_manager.h"
#include "managers/particle_manager.h"
#include "managers/scene_manager.h"
#include "managers/terrain_manager.h"
#include "visibility/camera_controller.h"

namespace renderer
{

class EditorCore: public Core
{
	enum EditorState
	{
		state_Look = 0,
		state_ModifyInstance,
		state_InsertInstance,
		state_InsertGroup
	};

	static constexpr int EDITOR_PRESSED_KEYS_ARRAY_SIZE = 5;
	static constexpr int PRESSED_M_INDEX = 0;
	static constexpr int PRESSED_I_INDEX = 1;
	static constexpr int PRESSED_U_INDEX = 2;
	static constexpr int PRESSED_ENTER_INDEX = 3;
	static constexpr int PRESSED_F4_INDEX = 4;

	static constexpr int EDITOR_MARKER_INSTANCES = 12;

public:
	EditorCore(renderer::graphics_lib::EditorFrameRenderer *renderer, GLFWwindow *mainWindow, renderer::visibility::TCameraController &cameraCtl, renderer::managers::SceneManager &sceneMgr,
		renderer::managers::TerrainManager *terrainMgr, renderer::managers::ObjectManager *objectMgr, renderer::managers::ParticleManager *particleMgr, renderer::graphics_lib::ShaderManager *shaderMgr);
	virtual ~EditorCore();

	virtual void mainLoop();

private:
	/*
	@brief Initializes the class
	*/
	void initializeEditor(renderer::managers::ObjectManager *objectMgr);

	void processUserInputs();

	/*
	@brief Processes mouse scroll
	*/
	void handleMouseScroll();

	/*
	@brief Returns selected instance into the scene
	*/
	void handleModeReset();

	/*
	@brief Modifies clisest to the camera instance
	*/
	void handleModifyInstance();

	/*
	@brief Inserts new instance
	*/
	void handleInsertSingleInstance();

	/*
	@brief Inserts instance group
	*/
	void handleInsertInstanceGroup();

	/*
	@brief Sets new position and rotation for camera
	*/
	void handleSetCameraData();

	/*
	@brief Updates data in frame renderer
	*/
	void updateSelectedInstanceRenderingData();

	/*
	@brief Figures out indices for insertion into scene
	@param[in] name - object name
	@param[in] chunkIdx - chunk index to find object in
	@param[out] objectIdx - object index
	@param[out] instanceIdx - instance index
	*/
	void getObjectIndex(const std::string &name, int chunkIdx, int &objectIdx, int &instanceIdx);

	void initAndShowSelectedInstance(const glm::vec3 &instancePosition);

	/*
	@brief Calculates positions of area markers
	@param[in] terrainManager - terrain manager
	@param[in] scene - scene structure
	@param[in] chunkIdx - chunk to insert into
	*/
	void calculateMarkerPositions(renderer::managers::TerrainManager &terrainManager,
		const renderer::data::Scene &scene, int chunkIdx);



	EditorState state;
	int chunkIndex, objectIndex, instanceIndex;
	float selectedInstance[4];

	//Object insertion
	std::vector<std::string> objectNames;
	int nameIndex, prevNameIndex;
	renderer::graphics_lib::videocard_data::ObjectRenderingData objectRenderingData;

	//Group insertion
	float radius;
	float markers[EDITOR_MARKER_INSTANCES*3];
	glm::vec3 areaCenter;
	renderer::graphics_lib::videocard_data::ObjectRenderingData markerRenderingData;

	bool editorPressedKeys[EDITOR_PRESSED_KEYS_ARRAY_SIZE];

	renderer::graphics_lib::EditorFrameRenderer &editorFrameRenderer;
	renderer::managers::TerrainManager *terrainManager; //Non-owning pointer
	renderer::managers::ObjectManager *objectManager; //Non-owning pointer
	renderer::managers::ParticleManager *particleManager; //Non-owning pointer
	renderer::graphics_lib::ShaderManager *shaderManager; //Non-owning pointer

	renderer::EditorSceneModifier editorSceneModifier;

	std::vector<std::string> editorStates;
};

}

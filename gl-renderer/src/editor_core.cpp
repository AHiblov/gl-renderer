/* editor_core.cpp
 * Editor core module
 *
 * Author: Artem Hiblov
 */

#include "editor_core.h"

#include <algorithm>
#include <chrono>
#include <cstring>
#include <sstream>

#include <glm/gtc/matrix_transform.hpp>

#include "log.h"
#include "editor_commands/copy_back_instance.h"
#include "editor_commands/insert_instance_group.h"
#include "editor_commands/insert_new_instance.h"
#include "editor_commands/set_camera_position.h"
#include "graphics_lib/rendering_scene_builder.h"
#include "utils/chunk_tools.h"
#include "utils/editor_tools.h"
#include "visibility/region_visibility_calculation.h"

using namespace std;
using namespace std::chrono;
using namespace renderer;
using namespace renderer::data;
using namespace renderer::editor_commands;
using namespace renderer::graphics_lib;
using namespace renderer::graphics_lib::videocard_data;
using namespace renderer::managers;
using namespace renderer::utils;
using namespace renderer::visibility;

namespace
{
	constexpr float TIME_MILLISECONDS_IN_SECOND = 1000.f;
	constexpr float EDITOR_INSTANCE_MOVEMENT_SPEED = 3.f;
	constexpr float EDITOR_INSTANCE_ROTATION_SPEED = 15.f;
	constexpr int EDITOR_FLOATS_PER_INSTANCE = 4;
	constexpr float EDITOR_INSERTION_DISTANCE = 5.f;

	const char *EDITOR_NEW_SCENE_FILE_NAME = "new-scene";

	const char *FEATURE_BASIC = "--";

	const char *OBJECT_SPLASH_NAME = "splash";
	const char *OBJECT_SKY_NAME = "sky";
	const char *OBJECT_MARKER_NAME = "area-marker";

	int *nameIndexPtr = nullptr;

	/*
	@brief Handles mouse wheel scrolling
	*/
	void scrollCallback(GLFWwindow *window, double xOffset, double yOffset);
}

EditorCore::EditorCore(EditorFrameRenderer *renderer, GLFWwindow *mainWindow, TCameraController &cameraCtl, SceneManager &sceneMgr,
	TerrainManager *terrainMgr, ObjectManager *objectMgr, ParticleManager *particleMgr, ShaderManager *shaderMgr):
	Core(mainWindow, renderer, cameraCtl, sceneMgr, *shaderMgr),
	state(state_Look), chunkIndex(-1), objectIndex(-1), instanceIndex(-1), nameIndex(0), prevNameIndex(0), radius(1.f), editorFrameRenderer(*renderer),
	terrainManager(terrainMgr), objectManager(objectMgr), particleManager(particleMgr), shaderManager(shaderMgr)
{
	initializeEditor(objectMgr);
}

EditorCore::~EditorCore()
{
}

void EditorCore::mainLoop()
{
	if(!window)
	{
		Log::getInstance().error("Window is null");
		return;
	}

	const glm::vec3 &cameraPosition = cameraController.getPosition();

	visibilityFlags.resize(chunkMargins->size());
	recalculateVisibility(chunkMargins, cameraPosition.x, cameraPosition.z, visibilityFlags);
	frameRenderer->setVisibilityFlags(&visibilityFlags);

	steady_clock::time_point secondAgo, prevTime, curTime;
	secondAgo = prevTime = curTime = steady_clock::now();

	int fps = 0, maxFps = 0;
	stringstream ss;

	while((glfwGetKey(window, GLFW_KEY_ESCAPE) != GLFW_PRESS) && !glfwWindowShouldClose(window))
	{
		prevTime = curTime;
		curTime = steady_clock::now();
		deltaTime = duration_cast<milliseconds>(curTime - prevTime).count() / TIME_MILLISECONDS_IN_SECOND;

		fps++;
		if(duration_cast<milliseconds>(curTime - secondAgo).count() >= TIME_MILLISECONDS_IN_SECOND)
		{
			recalculateVisibility(chunkMargins, cameraPosition.x, cameraPosition.z, visibilityFlags);
			frameRenderer->setVisibilityFlags(&visibilityFlags);

			ss.clear();
			ss.seekp(0, ios::beg);
			ss << fps << " FPS  " << frameRenderer->getDrawnTriangleCount() << " triangles drawn   ";
			editorFrameRenderer.setStatisticsLine(ss.str());

			if(maxFps < fps)
				maxFps = fps;
			fps = 0;

			secondAgo = curTime;
		}

		glfwPollEvents();
		processUserInputs();

		editorFrameRenderer.renderFrame();
		editorFrameRenderer.renderEditorUi();

		glfwSwapBuffers(window);
	}

	Log::getInstance().info(to_string(maxFps) + " FPS max in this scene");

	Log::getInstance().info(("Saving scene"));
	saveScene(EDITOR_NEW_SCENE_FILE_NAME, sceneManager.getScene());
}

void EditorCore::initializeEditor(ObjectManager *objectMgr)
{
	selectedInstance[0] = 0;
	selectedInstance[1] = 0;
	selectedInstance[2] = 0;
	selectedInstance[3] = 0;

	objectNames = move(objectMgr->getObjectNames());
	auto iter = remove_if(objectNames.begin(), objectNames.end(), [](const string &str) { return (str == OBJECT_MARKER_NAME) || (str == OBJECT_SKY_NAME) || (str == OBJECT_SPLASH_NAME); });
	objectNames.erase(iter, objectNames.end());

	nameIndexPtr = &nameIndex;

	for(int i = 0; i < EDITOR_MARKER_INSTANCES * 3; i++)
		markers[i] = 0;

	for(int i = 0; i < EDITOR_PRESSED_KEYS_ARRAY_SIZE; i++)
		editorPressedKeys[i] = false;

	glfwSetScrollCallback(window, scrollCallback);

	editorSceneModifier.setScene(&sceneManager.getScene());

	editorStates = {"Look", "Modify instance", "Insert instance", "Insert group"};
	editorFrameRenderer.setModeLine(string("Mode: ") + editorStates[state]);

	editorFrameRenderer.setActiveInstanceLine(string("Object: ") + objectNames[nameIndex]);
}

void EditorCore::processUserInputs()
{
	if(glfwGetKey(window, GLFW_KEY_M) == GLFW_PRESS)
		editorPressedKeys[PRESSED_M_INDEX] = true;
	else if(editorPressedKeys[PRESSED_M_INDEX])
	{
		handleModifyInstance();
		editorPressedKeys[PRESSED_M_INDEX] = false;

		editorFrameRenderer.setModeLine(string("Mode: ") + editorStates[state]);
	}

	if(glfwGetKey(window, GLFW_KEY_I) == GLFW_PRESS)
		editorPressedKeys[PRESSED_I_INDEX] = true;
	else if(editorPressedKeys[PRESSED_I_INDEX])
	{
		handleInsertSingleInstance();
		editorPressedKeys[PRESSED_I_INDEX] = false;

		editorFrameRenderer.setModeLine(string("Mode: ") + editorStates[state]);
		editorFrameRenderer.showActiveInstanceLine();
	}

	if(glfwGetKey(window, GLFW_KEY_U) == GLFW_PRESS)
		editorPressedKeys[PRESSED_U_INDEX] = true;
	else if(editorPressedKeys[PRESSED_U_INDEX])
	{
		handleInsertInstanceGroup();
		editorPressedKeys[PRESSED_U_INDEX] = false;

		editorFrameRenderer.setModeLine(string("Mode: ") + editorStates[state]);
		editorFrameRenderer.showActiveInstanceLine();
	}

	if(glfwGetKey(window, GLFW_KEY_ENTER) == GLFW_PRESS) //Returns instance to scene structure for insertion/modification mode or inserts instances in group insertion mode
		editorPressedKeys[PRESSED_ENTER_INDEX] = true;
	else if(editorPressedKeys[PRESSED_ENTER_INDEX])
	{
		handleModeReset();
		editorPressedKeys[PRESSED_ENTER_INDEX] = false;

		editorFrameRenderer.setModeLine(string("Mode: ") + editorStates[state]);
		editorFrameRenderer.hideActiveInstanceLine();
	}

	if(glfwGetKey(window, GLFW_KEY_F4) == GLFW_PRESS)
		editorPressedKeys[PRESSED_F4_INDEX] = true;
	else if(editorPressedKeys[PRESSED_F4_INDEX])
	{
		handleSetCameraData();
		editorPressedKeys[PRESSED_F4_INDEX] = false;
	}

	if(glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
	{
		if(state == state_ModifyInstance || state == state_InsertInstance)
		{
			selectedInstance[2] -= EDITOR_INSTANCE_MOVEMENT_SPEED * deltaTime;
			updateSelectedInstanceRenderingData();
		}
	}
	else if(glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
	{
		if(state == state_ModifyInstance || state == state_InsertInstance)
		{
			selectedInstance[2] += EDITOR_INSTANCE_MOVEMENT_SPEED * deltaTime;
			updateSelectedInstanceRenderingData();
		}
	}

	if(glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
	{
		if(state == state_ModifyInstance || state == state_InsertInstance)
		{
			selectedInstance[0] -= EDITOR_INSTANCE_MOVEMENT_SPEED * deltaTime;
			updateSelectedInstanceRenderingData();
		}
	}
	else if(glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
	{
		if(state == state_ModifyInstance || state == state_InsertInstance)
		{
			selectedInstance[0] += EDITOR_INSTANCE_MOVEMENT_SPEED * deltaTime;
			updateSelectedInstanceRenderingData();
		}
	}

	if(glfwGetKey(window, GLFW_KEY_X) == GLFW_PRESS)
	{
		if(state == state_ModifyInstance || state == state_InsertInstance)
		{
			selectedInstance[1] -= EDITOR_INSTANCE_MOVEMENT_SPEED * deltaTime;
			updateSelectedInstanceRenderingData();
		}
	}
	else if(glfwGetKey(window, GLFW_KEY_C) == GLFW_PRESS)
	{
		if(state == state_ModifyInstance || state == state_InsertInstance)
		{
			selectedInstance[1] += EDITOR_INSTANCE_MOVEMENT_SPEED * deltaTime;
			updateSelectedInstanceRenderingData();
		}
	}

	if(glfwGetKey(window, GLFW_KEY_G) == GLFW_PRESS)
	{
		if(state == state_ModifyInstance || state == state_InsertInstance)
		{
			selectedInstance[3] -= EDITOR_INSTANCE_ROTATION_SPEED * deltaTime;
			updateSelectedInstanceRenderingData();
		}
	}
	else if(glfwGetKey(window, GLFW_KEY_H) == GLFW_PRESS)
	{
		if(state == state_ModifyInstance || state == state_InsertInstance)
		{
			selectedInstance[3] += EDITOR_INSTANCE_ROTATION_SPEED * deltaTime;
			updateSelectedInstanceRenderingData();
		}
	}

	if(glfwGetKey(window, GLFW_KEY_KP_ADD) == GLFW_PRESS)
	{
		if(state == state_InsertGroup)
		{
			radius += EDITOR_INSTANCE_MOVEMENT_SPEED * deltaTime;
			calculateMarkerPositions(*terrainManager, sceneManager.getScene(), chunkIndex);
		}
	}
	if(glfwGetKey(window, GLFW_KEY_KP_SUBTRACT) == GLFW_PRESS)
	{
		if(state == state_InsertGroup)
		{
			radius -= EDITOR_INSTANCE_MOVEMENT_SPEED * deltaTime;
			calculateMarkerPositions(*terrainManager, sceneManager.getScene(), chunkIndex);
		}
	}

	handleMouseScroll();

	Core::processUserInputs();
}

void EditorCore::handleMouseScroll()
{
	if(nameIndex != prevNameIndex) //Object was changed via mouse scroll
	{
		if(state == state_InsertInstance || state == state_InsertGroup)
		{
			const int nameAmount = objectNames.size();
			if(*nameIndexPtr == nameAmount)
				*nameIndexPtr = 0;
			else if(*nameIndexPtr < 0)
				*nameIndexPtr = nameAmount - 1;

			objectManager->getRenderingData(objectNames[nameIndex], objectRenderingData);
			editorFrameRenderer.setObjectRenderingData(&objectRenderingData);

			prevNameIndex = nameIndex;

			editorFrameRenderer.setActiveInstanceLine(string("Object: ") + objectNames[nameIndex]);
			editorFrameRenderer.showActiveInstanceLine();
		}
		else nameIndex = prevNameIndex; //Modify mode, don't allow to change object
	}
}

void EditorCore::handleModeReset()
{
	switch(state)
	{
	case state_Look:
		break;
	case state_ModifyInstance:
	{
		CopyBackInstance copyBackInstance(&editorSceneModifier, chunkIndex, objectIndex, instanceIndex, selectedInstance);
		copyBackInstance.execute();
	}
		break;
	case state_InsertInstance:
	{
		InsertNewInstance insertNewInstance(&editorSceneModifier, chunkIndex, objectIndex, instanceIndex, selectedInstance[0], selectedInstance[1], selectedInstance[2], selectedInstance[3], objectNames[nameIndex],
			FEATURE_BASIC);
		insertNewInstance.execute();

		//Update structure for frame renderer: take into consideration removed and inserted instances
		RenderingScene *renderingScene = editorFrameRenderer.getRenderingScene();
		updateRenderingSceneObjects(sceneManager.getScene(), sceneManager.isDeferredRendering(), objectManager, sceneManager.getChunkMargins(), shaderManager, renderingScene);

		editorFrameRenderer.setObjectRenderingData(nullptr); //Remove new instance
		break;
	}
	case state_InsertGroup:
	{
		InsertInstanceGroup insertInstanceGroup(&editorSceneModifier, chunkIndex, areaCenter, radius, terrainManager, objectNames[nameIndex], FEATURE_BASIC);
		insertInstanceGroup.execute();

		editorFrameRenderer.setObjectRenderingData(nullptr); //Remove sample object
		editorFrameRenderer.setMarkerRenderingData(nullptr, 0, nullptr); //Remove markers

		//Update structure for frame renderer: take into consideration inserted instances
		RenderingScene *renderingScene = editorFrameRenderer.getRenderingScene();
		updateRenderingSceneObjects(sceneManager.getScene(), sceneManager.isDeferredRendering(), objectManager, sceneManager.getChunkMargins(), shaderManager, renderingScene);
		break;
	}
	default:
		Log::getInstance().warning("Unknown state in mode reset. Did you forget something?");
	}

	state = state_Look;
}

void EditorCore::handleModifyInstance()
{
	//Save previous selected instance to structure before selecting the new one
	if(state == state_ModifyInstance)
	{
		CopyBackInstance copyBackInstance(&editorSceneModifier, chunkIndex, objectIndex, instanceIndex, selectedInstance);
		copyBackInstance.execute();
	}
	else if(state == state_InsertInstance)
	{
		InsertNewInstance insertNewInstance(&editorSceneModifier, chunkIndex, objectIndex, instanceIndex, selectedInstance[0], selectedInstance[1], selectedInstance[2], selectedInstance[3],
			objectNames[nameIndex], FEATURE_BASIC);
		insertNewInstance.execute();
	}

	state = state_ModifyInstance;

	findClosestObject(cameraController.getPosition(), sceneManager.getScene().instances, chunkIndex, objectIndex, instanceIndex);

	//Copy instance transform data
	memcpy(selectedInstance, &(sceneManager.getScene().instances[chunkIndex][objectIndex].positions[instanceIndex]), sizeof(float) * EDITOR_FLOATS_PER_INSTANCE);

	//Delete instance from array
	auto &positionsRef = sceneManager.getScene().instances[chunkIndex][objectIndex].positions;
	positionsRef.erase(positionsRef.begin() + instanceIndex, positionsRef.begin() + instanceIndex + EDITOR_FLOATS_PER_INSTANCE);

	//Update structure for frame renderer: take into consideration removed and inserted instances
	RenderingScene *renderingScene = editorFrameRenderer.getRenderingScene();
	updateRenderingSceneObjects(sceneManager.getScene(), sceneManager.isDeferredRendering(), objectManager, sceneManager.getChunkMargins(), shaderManager, renderingScene);

	//Start drawing selected instance

	objectManager->getRenderingData(sceneManager.getScene().instances[chunkIndex][objectIndex].name, objectRenderingData);
	editorFrameRenderer.setObjectRenderingData(&objectRenderingData);

	updateSelectedInstanceRenderingData();
}

void EditorCore::handleInsertSingleInstance()
{
	//Save previous selected instance to structure before inserting new
	if(state == state_ModifyInstance)
	{
		CopyBackInstance copyBackInstance(&editorSceneModifier, chunkIndex, objectIndex, instanceIndex, selectedInstance);
		copyBackInstance.execute();
	}
	else if(state == state_InsertInstance)
	{
		InsertNewInstance insertNewInstance(&editorSceneModifier, chunkIndex, objectIndex, instanceIndex, selectedInstance[0], selectedInstance[1], selectedInstance[2], selectedInstance[3],
			objectNames[nameIndex], FEATURE_BASIC);
		insertNewInstance.execute();
	}

	state = state_InsertInstance;

	//Determine insertion position (5 units in view direction)

	glm::vec3 cameraDirection(cos(verticalRotation) * sin(horizontalRotation), 0, cos(verticalRotation) * cos(horizontalRotation));

	Scene &scene = sceneManager.getScene();

	glm::vec3 insertionPosition(cameraController.getPosition());
	insertionPosition += cameraDirection * EDITOR_INSERTION_DISTANCE;
	findChunk(scene.chunks, insertionPosition.x, insertionPosition.z, chunkIndex);
	if(chunkIndex == -1)
	{
		state = state_Look;

		Log::getInstance().warning("No chunk found for given coordinates. No instance will be inserted");
		return;
	}
	insertionPosition.y = terrainManager->getHeight(scene.chunks[chunkIndex].x, scene.chunks[chunkIndex].z, scene.chunks[chunkIndex].name, insertionPosition.x, insertionPosition.z);

	initAndShowSelectedInstance(insertionPosition);

	//Update structure for frame renderer: take into consideration removed and inserted instances
	RenderingScene *renderingScene = editorFrameRenderer.getRenderingScene();
	updateRenderingSceneObjects(sceneManager.getScene(), sceneManager.isDeferredRendering(), objectManager, sceneManager.getChunkMargins(), shaderManager, renderingScene);
}

void EditorCore::handleInsertInstanceGroup()
{
	if(state == state_InsertGroup) //Don't allow start new insertion while running
		return;

	//Save previous selected instance to structure before inserting new
	if(state == state_ModifyInstance)
	{
		CopyBackInstance copyBackInstance(&editorSceneModifier, chunkIndex, objectIndex, instanceIndex, selectedInstance);
		copyBackInstance.execute();
	}
	else if(state == state_InsertInstance)
	{
		InsertNewInstance insertNewInstance(&editorSceneModifier, chunkIndex, objectIndex, instanceIndex, selectedInstance[0], selectedInstance[1], selectedInstance[2], selectedInstance[3],
			objectNames[nameIndex], FEATURE_BASIC);
		insertNewInstance.execute();
	}

	state = state_InsertGroup;

	//Determine position of group center (5 units in view direction)

	glm::vec3 cameraDirection(cos(verticalRotation) * sin(horizontalRotation), 0, cos(verticalRotation) * cos(horizontalRotation));

	Scene &scene = sceneManager.getScene();

	areaCenter = cameraController.getPosition();
	areaCenter += cameraDirection * EDITOR_INSERTION_DISTANCE;
	findChunk(scene.chunks, areaCenter.x, areaCenter.z, chunkIndex);
	if(chunkIndex == -1)
	{
		Log::getInstance().warning("No chunk found for given coordinates. No instance group will be inserted");

		state = state_Look;
		return;
	}

	calculateMarkerPositions(*terrainManager, scene, chunkIndex);

	areaCenter.y = terrainManager->getHeight(scene.chunks[chunkIndex].x, scene.chunks[chunkIndex].z, scene.chunks[chunkIndex].name, areaCenter.x, areaCenter.z);

	initAndShowSelectedInstance(areaCenter);
}

void EditorCore::handleSetCameraData()
{
	const glm::vec3 &cameraPosition = cameraController.getPosition();

	SetCameraPosition setCameraPosition(&editorSceneModifier, cameraPosition, horizontalRotation, verticalRotation);
	setCameraPosition.execute();
}

void EditorCore::updateSelectedInstanceRenderingData()
{
	glm::mat4 rotationMatrix = glm::rotate(glm::mat4(1.), glm::radians(selectedInstance[3]), glm::vec3(0, 1, 0));
	glm::mat4 translationMatrix = glm::translate(glm::mat4(1.), glm::vec3(selectedInstance[0], selectedInstance[1], selectedInstance[2]));

	glm::mat3 rotationPass = glm::mat3(rotationMatrix);
	glm::mat4 arrangementPass = translationMatrix * rotationMatrix;
	editorFrameRenderer.setSelectedInstanceData(arrangementPass, rotationPass, objectIndex);
}

void EditorCore::getObjectIndex(const string &name, int chunkIdx, int &objectIdx, int &instanceIdx)
{
	int index = 0;
	for(auto &current: sceneManager.getScene().instances[chunkIdx])
	{
		if(current.name == name)
		{
			objectIdx = index;
			instanceIdx = current.positions.size();

			return;
		}

		index++;
	}

	//Nothing found
	objectIdx = index;
	instanceIdx = 0;
}

void EditorCore::initAndShowSelectedInstance(const glm::vec3 &instancePosition)
{
	//Determine indices for instance in scene
	objectManager->getRenderingData(objectNames[nameIndex], objectRenderingData);

	//Set instance transform data
	getObjectIndex(objectNames[nameIndex], chunkIndex, objectIndex, instanceIndex);
	selectedInstance[0] = instancePosition.x;
	selectedInstance[1] = instancePosition.y;
	selectedInstance[2] = instancePosition.z;
	selectedInstance[3] = 0;

	//Start drawing selected instance
	editorFrameRenderer.setObjectRenderingData(&objectRenderingData);
	updateSelectedInstanceRenderingData();
}

void EditorCore::calculateMarkerPositions(TerrainManager &terrainManager, const Scene &scene, int chunkIdx)
{
	const float rotationStep = 360.f / static_cast<float>(EDITOR_MARKER_INSTANCES);
	float currentRotation = 0.f;
	for(int i = 0; i < EDITOR_MARKER_INSTANCES; i++)
	{
		markers[i*3] = areaCenter.x + radius * cos(glm::radians(currentRotation));
		markers[i*3+2] = areaCenter.z + radius * sin(glm::radians(currentRotation));
		markers[i*3+1] = terrainManager.getHeight(scene.chunks[chunkIdx].x, scene.chunks[chunkIdx].z, scene.chunks[chunkIdx].name, markers[i*3], markers[i*3+2]);

		currentRotation += rotationStep;
	}

	objectManager->getRenderingData(OBJECT_MARKER_NAME, markerRenderingData);
	editorFrameRenderer.setMarkerRenderingData(markers, EDITOR_MARKER_INSTANCES, &markerRenderingData);
}

namespace
{
	void scrollCallback(GLFWwindow *window, double xOffset, double yOffset)
	{
		*nameIndexPtr -= static_cast<int>(yOffset);
	}
}

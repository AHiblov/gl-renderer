/* core.h
 * Real-time renderer core module
 *
 * Author: Artem Hiblov
 */

#pragma once

#include <map>
#include <thread>

#include <GLFW/glfw3.h>

#include "data/chunk_margins.h"
#include "data/visibility_flags.h"
#include "graphics_lib/frame_renderer.h"
#include "graphics_lib/shader_manager.h"
#include "managers/scene_manager.h"
#include "simulation/simulation_model.h"
#include "visibility/camera_controller.h"

namespace renderer
{

class Core
{
	static constexpr int PRESSED_KEYS_ARRAY_SIZE = 3;
	static constexpr int PRESSED_F5_INDEX = 0;
	static constexpr int PRESSED_F8_INDEX = 1;
	static constexpr int PRESSED_F11_INDEX = 2;

public:
	Core(GLFWwindow *wnd, renderer::graphics_lib::FrameRenderer *frameRend, renderer::visibility::TCameraController &camera, renderer::managers::SceneManager &sceneMgr,
		renderer::graphics_lib::ShaderManager &shaderMgr);
	virtual ~Core();

	/*
	@brief Renderer loop with reading inputs and frame drawing
	*/
	virtual void mainLoop();

	void onResize(int width, int height);

protected:
	/*
	@brief Obtains info on pressed keys and mouse movements and updates camera
	*/
	void processUserInputs();

	void processSimulationChanges();

	/*
	@brief Starts thread with world simulation
	*/
	void startSimulation();

	void stopSimulation();
	void restoreLight();

	/*
	@brief Saves camera position and rotation to file
	*/
	void saveCameraData();

	/*
	@brief Loads camera position and rotation from file
	*/
	void loadCameraData();

	/*
	@brief Initializes the class
	*/
	void initialize(const renderer::data::Camera &camera);



	renderer::graphics_lib::FrameRenderer *frameRenderer;

	std::map<int, renderer::data::ChunkMargins> *chunkMargins;
	std::vector<renderer::data::VisibilityFlags> visibilityFlags;

	renderer::visibility::TCameraController cameraController;
	float horizontalRotation;
	float verticalRotation;

	float deltaTime;

	GLFWwindow *window;

	bool pressedKeys[PRESSED_KEYS_ARRAY_SIZE];

	renderer::managers::SceneManager &sceneManager;
	renderer::graphics_lib::ShaderManager &shaderManager;


	renderer::simulation::SimulationModel simulationModel;
	std::thread *simulationThread;

	bool needTerminate;
};

}

/* main_component.h
 * Controls application top-level actions
 *
 * Author: Artem Hiblov
 */

#pragma once

#include <memory>

#include "statistics.h"

#include "thirdparty/glfw_tools.h" //GLFW must be included after GLEW
#include "app_parameters.h"
#include "editor_core.h"
#include "graphics_lib/shader_manager.h"
#include "graphics_lib/splash_renderer_builder.h"
#include "managers/object_manager.h"
#include "managers/particle_manager.h"
#include "managers/scene_manager.h"
#include "managers/terrain_manager.h"

namespace renderer
{

class MainComponent
{
public:
	MainComponent(const renderer::AppParameters &parameters);
	~MainComponent();

	/*
	@brief Initializes data needed for both splash and main renderer
	*/
	bool initBase();

	/*
	@brief Initializes and shows splash screen
	*/
	void showSplash();

	/*
	@brief Initializes all the data needed for renderer
	*/
	bool initAll();

	/*
	@brief Runs rendering loop
	*/
	void run();

private:
	//Dispatching
	std::unique_ptr<renderer::Core> core;

	//Resources
	std::unique_ptr<renderer::managers::SceneManager> sceneManager;
	std::unique_ptr<renderer::managers::TerrainManager> terrainManager;
	std::unique_ptr<renderer::managers::ObjectManager> objectManager;
	std::unique_ptr<renderer::managers::ParticleManager> particleManager;

	std::unique_ptr<renderer::graphics_lib::ShaderManager> shaderManager;

	std::unique_ptr<renderer::graphics_lib::SplashRenderer> splashRenderer;
public:
	GLFWwindow *window;
	private:
	const renderer::AppParameters &appParameters;

	SceneStatistics statistics;

	bool isInitOk;
};

}

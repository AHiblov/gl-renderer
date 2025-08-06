/* core.cpp
 * Real-time renderer core module
 *
 * Author: Artem Hiblov
 */

#include <GL/glew.h>

#include "core.h"

#include <chrono>
#include <fstream>
#include <sstream>

#include <glm/glm.hpp>

#include "log.h"
#include "graphics_lib/uniform_setters.h"
#include "simulation/simulation_thread.h"
#include "utils/math_tools.h"
#include "visibility/region_visibility_calculation.h"

using namespace std;
using namespace std::chrono;
using namespace std::chrono_literals;
using namespace renderer;
using namespace renderer::data;
using namespace renderer::managers;
using namespace renderer::graphics_lib;
using namespace renderer::graphics_lib::videocard_data;
using namespace renderer::simulation;
using namespace renderer::utils;
using namespace renderer::visibility;

namespace
{
	constexpr float MATH_PI_RADIANS = 3.14159f;
	constexpr float MOVEMENT_SPEED_FACTOR = 7.f;
	constexpr float TIME_MILLISECONDS_IN_SECOND = 1000.f;

	const char *CAMERA_FILENAME = "camera";
	const char *DIRECTIONAL_LIGHT_STRING = "directional";

	const glm::vec3 LIGHT_DIRECTION1 = glm::vec3(-1.f, -0.05f, -0.02f); //Sunrise
	const glm::vec3 LIGHT_DIRECTION2 = glm::vec3(1.f, -0.05f, -0.02f); //Sunset
	const float LIGHT_MAX_HEIGHT = 0.6f; //[0; 1] -> [0; 90] degrees
	const glm::vec3 LIGHT_GOLDEN_HOUR_COLOUR = glm::vec3(1.f, 0.24f, 0.f);
	const glm::vec3 LIGHT_DAY_COLOUR = glm::vec3(1.f, 1.f, 1.f);
	const glm::vec3 LIGHT_DIFFUSE_NIGHT_COLOUR = glm::vec3(0.f, 0.f, 0.f); //Diffuse component is not used at night

	//Twilight
	const glm::vec3 LIGHT_AMBIENT_COLOUR_DARK = glm::vec3(0.6f, 0.6f, 0.6f);
	const glm::vec3 LIGHT_AMBIENT_COLOUR_LIGHT = glm::vec3(1.f, 1.f, 1.f);

	constexpr float GOLDEN_HOUR_MORNING_MIX_START = 0.3f;
	constexpr float GOLDEN_HOUR_MORNING_MIX_END = 1.f;
	constexpr float GOLDEN_HOUR_EVENING_MIX_START = 0.f;
	constexpr float GOLDEN_HOUR_EVENING_MIX_END = 0.7f;

	bool isLmbPressed = false;
	Core *corePtr = nullptr;

	/*
	@brief Mouse buttons click callback
	@param[in] window - window to catch clicks
	@param[in] button - mouse button ID
	@param[in] action - press or release
	*/
	void mouseClickCallback(GLFWwindow *window, int button, int action, int mods);

	void windowSizeCallback(GLFWwindow *window, int width, int height);

	/*
	@brief Used to initialize X cursor position
	*/
	double initXPosition(GLFWwindow *window);

	/*
	@brief Used to initialize Y cursor position
	*/
	double initYPosition(GLFWwindow *window);
}

Core::Core(GLFWwindow *wnd, FrameRenderer *frameRend, TCameraController &camera, SceneManager &sceneMgr, ShaderManager &shaderMgr):
	frameRenderer(frameRend), cameraController(camera), horizontalRotation(0), verticalRotation(0), deltaTime(0), window(wnd), sceneManager(sceneMgr), shaderManager(shaderMgr),
	simulationThread(nullptr), needTerminate(false)
{
	initialize(sceneManager.getCameraData());
}

Core::~Core()
{
	if(frameRenderer)
	{
		delete frameRenderer;
		frameRenderer = nullptr;
	}
}

void Core::mainLoop()
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
			ss.str(string());
			ss << fps << " FPS  " << frameRenderer->getDrawnTriangleCount() << " triangles drawn";
			frameRenderer->setStatisticsLine(ss.str());

			if(maxFps < fps)
				maxFps = fps;
			fps = 0;

			secondAgo = curTime;

			ss.clear();
			ss.seekp(0, ios::beg);
			ss.str(string());
			ss << (simulationThread ? "Simulation is ON": "Simulation is OFF");
			frameRenderer->setSimulationLine(ss.str());
		}

		glfwPollEvents();
		processUserInputs();

		processSimulationChanges();

		frameRenderer->renderFrame();
		frameRenderer->renderUi();

		glfwSwapBuffers(window);
	}

	Log::getInstance().info(to_string(maxFps) + " FPS max in this scene");
}

void Core::processUserInputs()
{
	static double prevXPosition = initXPosition(window), prevYPosition = initYPosition(window);
	double xPosition, yPosition;
	glfwGetCursorPos(window, &xPosition, &yPosition);

	if(isLmbPressed)
	{
		horizontalRotation -= (xPosition - prevXPosition) * deltaTime; //+= rotation in the opposite to cusror movement direction, -= same direction (here and the next line)
		verticalRotation -= (yPosition - prevYPosition) * deltaTime;
	}
	prevXPosition = xPosition;
	prevYPosition = yPosition;

	float forwardDelta = 0;
	float rightDelta = 0;

	if(glfwGetKey(window, GLFW_KEY_UP) == GLFW_PRESS)
	{
		forwardDelta = deltaTime * MOVEMENT_SPEED_FACTOR;
	}
	else if(glfwGetKey(window, GLFW_KEY_DOWN) == GLFW_PRESS)
	{
		forwardDelta = -deltaTime * MOVEMENT_SPEED_FACTOR;
	}

	if(glfwGetKey(window, GLFW_KEY_LEFT) == GLFW_PRESS)
	{
		rightDelta = -deltaTime * MOVEMENT_SPEED_FACTOR;
	}
	else if(glfwGetKey(window, GLFW_KEY_RIGHT) == GLFW_PRESS)
	{
		rightDelta = deltaTime * MOVEMENT_SPEED_FACTOR;
	}

	cameraController.update(horizontalRotation, verticalRotation, forwardDelta, rightDelta);
	frameRenderer->updateCamera(cameraController.getViewMatrix());

	if(glfwGetKey(window, GLFW_KEY_F5) == GLFW_PRESS)
		pressedKeys[PRESSED_F5_INDEX] = true;
	else if(pressedKeys[PRESSED_F5_INDEX])
	{
		saveCameraData();
		pressedKeys[PRESSED_F5_INDEX] = false;
	}

	if(glfwGetKey(window, GLFW_KEY_F8) == GLFW_PRESS)
		pressedKeys[PRESSED_F8_INDEX] = true;
	else if(pressedKeys[PRESSED_F8_INDEX])
	{
		loadCameraData();
		pressedKeys[PRESSED_F8_INDEX] = false;
	}

	if(glfwGetKey(window, GLFW_KEY_F11) == GLFW_PRESS)
		pressedKeys[PRESSED_F11_INDEX] = true;
	else if(pressedKeys[PRESSED_F11_INDEX])
	{
		if(!simulationThread)
		{
			if(sceneManager.getScene().light.lightType == DIRECTIONAL_LIGHT_STRING)
				startSimulation();
			else Log::getInstance().error("Day and night simulation is implemented for directional light only");
		}
		else
		{
			stopSimulation();
			restoreLight();
		}

		pressedKeys[PRESSED_F11_INDEX] = false;
	}
}

void Core::processSimulationChanges()
{
	if(!simulationThread)
		return;

	bool update;

	float currentTime;

	bool isDay = false;
	float sunRaysInterpolation = 0;

	bool isMorningGoldenHour = false, isEveningGoldenHour = false;
	float goldenHourInterpolation = 0;

	bool isDawnTwilight = false, isDuskTwilight = false;
	float twilightInterpolation = 0;

	{
		lock_guard<mutex> lockGuard(simulationModel.accessLock);

		update = simulationModel.wasUpdated;
		if(update)
		{
			currentTime = simulationModel.time;

			isDay = simulationModel.isDay;
			sunRaysInterpolation = simulationModel.sunRaysInterpolation;

			if(isDay)
			{
				isMorningGoldenHour = simulationModel.isMorningGoldenHour;
				isEveningGoldenHour = simulationModel.isEveningGoldenHour;

				if(isMorningGoldenHour || isEveningGoldenHour)
					goldenHourInterpolation = simulationModel.goldenHourInterpolation;
			}
			else
			{
				isDawnTwilight = simulationModel.isDawnTwilight;
				isDuskTwilight = simulationModel.isDuskTwilight;
				if(isDawnTwilight || isDuskTwilight)
					twilightInterpolation = simulationModel.twilightInterpolation;
			}
		}

		simulationModel.wasUpdated = false;
	}

	if(update)
	{
		if(isDay)
		{
			glm::vec3 lightDirection = slerp(LIGHT_DIRECTION1, LIGHT_DIRECTION2, sunRaysInterpolation);
			lightDirection.y = -sinf(sunRaysInterpolation * MATH_PI_RADIANS) * LIGHT_MAX_HEIGHT;
			frameRenderer->setLightDirection(lightDirection);

			if(isMorningGoldenHour)
			{
				glm::vec3 currentLightColour = glm::mix(LIGHT_GOLDEN_HOUR_COLOUR, LIGHT_DAY_COLOUR, glm::smoothstep(GOLDEN_HOUR_MORNING_MIX_START, GOLDEN_HOUR_MORNING_MIX_END, goldenHourInterpolation));
				frameRenderer->setDiffuseLightColour(currentLightColour);
			}
			else if(isEveningGoldenHour)
			{
				glm::vec3 currentLightColour = glm::mix(LIGHT_DAY_COLOUR, LIGHT_GOLDEN_HOUR_COLOUR, glm::smoothstep(GOLDEN_HOUR_EVENING_MIX_START, GOLDEN_HOUR_EVENING_MIX_END, goldenHourInterpolation));
				frameRenderer->setDiffuseLightColour(currentLightColour);
			}
			else frameRenderer->setDiffuseLightColour(LIGHT_DAY_COLOUR);
		}
		else
		{
			if(isDawnTwilight)
			{
				glm::vec3 currentAmbientLightColour = glm::mix(LIGHT_AMBIENT_COLOUR_DARK, LIGHT_AMBIENT_COLOUR_LIGHT, twilightInterpolation);
				frameRenderer->setAmbientLightColour(currentAmbientLightColour);
			}
			else if(isDuskTwilight)
			{
				glm::vec3 currentAmbientLightColour = glm::mix(LIGHT_AMBIENT_COLOUR_LIGHT, LIGHT_AMBIENT_COLOUR_DARK, twilightInterpolation);
				frameRenderer->setAmbientLightColour(currentAmbientLightColour);
			}
			else frameRenderer->setAmbientLightColour(LIGHT_AMBIENT_COLOUR_DARK);

			frameRenderer->setDiffuseLightColour(LIGHT_DIFFUSE_NIGHT_COLOUR);
		}
	}
}

void Core::startSimulation()
{
	stopSimulation();

	needTerminate = false;

	Log::getInstance().info("Starting simulation");

	simulationThread = new thread(performSimulation, ref(simulationModel), ref(needTerminate));
}

void Core::stopSimulation()
{
	if(!simulationThread)
		return;

	needTerminate = true; //Ask existing thread to terminate
	simulationThread->join();

	delete simulationThread;
	simulationThread = nullptr;

	Log::getInstance().info("Simulation is stopped");
}

void Core::restoreLight()
{
	frameRenderer->setDiffuseLightColour(LIGHT_DAY_COLOUR);
	frameRenderer->setAmbientLightColour(LIGHT_AMBIENT_COLOUR_LIGHT);

	const Light &light = sceneManager.getLightData();
	glm::vec3 lightDirection = glm::vec3(light.x, light.y, light.z);
	frameRenderer->setLightDirection(lightDirection);
}

void Core::saveCameraData()
{
	ofstream data(CAMERA_FILENAME);
	if(!data.is_open())
		return;

	const float horizontalRotationDegrees = (horizontalRotation * 180.f) / MATH_PI_RADIANS;
	const float verticalRotationDegrees = (verticalRotation * 180.f) / MATH_PI_RADIANS;

	const glm::vec3 &cameraPosition = cameraController.getPosition();

	data << cameraPosition.x << ' ' << cameraPosition.y << ' ' << cameraPosition.z <<
		' ' << horizontalRotationDegrees << ' '<< verticalRotationDegrees;
	data.close();
}

void Core::loadCameraData()
{
	ifstream data(CAMERA_FILENAME);
	if(!data.is_open())
	{
		Log::getInstance().error("Can't open camera file for reading");
		return;
	}

	glm::vec3 &cameraPosition = cameraController.getPosition();
	float horizontalRotationDegrees = 0, verticalRotationDegrees = 0;

	data >> cameraPosition.x >> cameraPosition.y >> cameraPosition.z >>
		horizontalRotationDegrees >> verticalRotationDegrees;
	data.close();

	horizontalRotation = (horizontalRotationDegrees * MATH_PI_RADIANS) / 180.f;
	verticalRotation = (verticalRotationDegrees * MATH_PI_RADIANS) / 180.f;

	recalculateVisibility(chunkMargins, cameraPosition.x, cameraPosition.z, visibilityFlags);
	frameRenderer->setVisibilityFlags(&visibilityFlags);
}

void Core::initialize(const Camera &camera)
{
	horizontalRotation = camera.horizontalRotationRadians;
	verticalRotation = camera.verticalRotationRadians;

	chunkMargins = &sceneManager.getChunkMargins();

	frameRenderer->setCameraPosition(&(cameraController.getPosition()));

	glfwSetMouseButtonCallback(window, mouseClickCallback);

	glfwSetFramebufferSizeCallback(window, windowSizeCallback);
	corePtr = this;

	for(int i = 0; i < PRESSED_KEYS_ARRAY_SIZE; i++)
		pressedKeys[i] = false;
}

void Core::onResize(int width, int height)
{
	glViewport(0, 0, width, height);

	const map<int, unsigned long long> &shaderFlags = shaderManager.getShaderFlags();
	vector<ShaderIds> &shaderIds = shaderManager.getOrderedShaderIds();

	for(auto &[index, flags]: shaderFlags)
	{
		ShaderIds &current = shaderIds[index];
		glUseProgram(current.id);

		if(flags & ShaderFlags::FEATURE_DIRECTIONAL_LIGHT)
		{
			setDirectionalShaderUniforms(current, static_cast<float>(width) / height, sceneManager.getLightData());
		}

		if(flags & ShaderFlags::FEATURE_POINT_LIGHT)
		{
			setPointShaderUniforms(current, static_cast<float>(width) / height, sceneManager.getLightData());
		}
	}
}

namespace
{
	void mouseClickCallback(GLFWwindow *window, int button, int action, int mods)
	{
		if(button == GLFW_MOUSE_BUTTON_LEFT)
		{
			if(action == GLFW_PRESS)
				isLmbPressed = true;
			else if(action == GLFW_RELEASE)
				isLmbPressed = false;
		}
	}

	void windowSizeCallback(GLFWwindow *window, int width, int height)
	{
		if(!corePtr)
			return;

		corePtr->onResize(width, height);
	}

	double initXPosition(GLFWwindow *window)
	{
		double x, y;
		glfwGetCursorPos(window, &x, &y);
		return x;
	}

	double initYPosition(GLFWwindow *window)
	{
		double x, y;
		glfwGetCursorPos(window, &x, &y);
		return y;
	}
}

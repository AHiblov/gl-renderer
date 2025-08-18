/* main_component.cpp
 * Controls application top-level actions
 *
 * Author: Artem Hiblov
 */

#include "thirdparty/glew_tools.h" //GLEW must go before GLFW (main_component.h)

#include "main_component.h"

#include <iomanip>
#include <sstream>
#include <string>

#include "log.h"
#include "graphics_lib/deferred_renderer.h"
#include "graphics_lib/editor_frame_renderer.h"
#include "graphics_lib/main_renderer_builder.h"
#include "graphics_lib/message_callback.h"
#include "graphics_lib/postprocessing_renderer_builder.h"
#include "graphics_lib/rendering_scene_builder.h"
#include "graphics_lib/splash_renderer_builder.h"
#include "graphics_lib/videocard_data/rendering_scene.h"
#include "thirdparty/imgui_tools.h"
#include "visibility/camera_controller.h"

using namespace std;
using namespace renderer;
using namespace renderer::data;
using namespace renderer::graphics_lib;
using namespace renderer::graphics_lib::videocard_data;
using namespace renderer::managers;
using namespace renderer::thirdparty;
using namespace renderer::visibility;

namespace
{
	const char *TERRAIN_DESCRIPTION_PATH = "terrain/terrain-description";
	const char *OBJECT_DESCRIPTION_PATH = "objects/object-description";
	const char *SHADER_DESCRIPTION_PATH = "shaders/shader-description";

	const char *LIGHT_TYPE_DIRECTIONAL = "directional";

	const char *SCENE_STATISTICS_PATH = "statistics";

	void writeContextInfoLogMessages();
}

MainComponent::MainComponent(const AppParameters &parameters):
	window(nullptr), appParameters(parameters), statistics(SCENE_STATISTICS_PATH), isInitOk(false)
{
}

MainComponent::~MainComponent()
{
	if(!isInitOk)
		return;

	shutdownGlfw();
}

bool MainComponent::initBase()
{
	//Initialize thirdparty libraries

	bool status = initGlfw(appParameters, &window);
	if(!status)
		return false;

	status = initGlew();
	if(!status)
		return false;

	writeContextInfoLogMessages();

	//Initialize components needed for both splash and main renderer
	objectManager = make_unique<ObjectManager>(OBJECT_DESCRIPTION_PATH);
	shaderManager = make_unique<ShaderManager>(SHADER_DESCRIPTION_PATH);

	//OpenGL 4.3+
	if(appParameters.enableDebug)
	{
		glEnable(GL_DEBUG_OUTPUT);
		glDebugMessageCallback(graphicsLibMessageCallback, nullptr);
	}

	isInitOk = true;
	return true;
}

void MainComponent::showSplash()
{
	splashRenderer = move(buildSplashRenderer(objectManager.get(), shaderManager.get(), static_cast<float>(appParameters.screenWidth) / appParameters.screenHeight));
	splashRenderer->render();

	glfwSwapBuffers(window);
}

bool MainComponent::initAll()
{
	if(!isInitOk || !objectManager || !shaderManager)
	{
		Log::getInstance().error("Can't proceed due to initialization error");
		return false;
	}

	initImgui(window);

	//Initialize scene

	Log::getInstance().info(string("Initializing scene \"") + appParameters.scenePath + "\"");

	sceneManager = make_unique<SceneManager>(appParameters.scenePath);
	terrainManager = make_unique<TerrainManager>(TERRAIN_DESCRIPTION_PATH);
	particleManager = make_unique<ParticleManager>(terrainManager.get());

	sceneManager->computeChunkMargins(terrainManager.get());
	sceneManager->appendChunkDimensions(terrainManager.get());

	bool isDeferredRendering = sceneManager->isDeferredRendering();

	const Scene &scene = sceneManager->getScene();
	bool isDirectional = false;
	if(isDeferredRendering)
	{
		isDirectional = true; //Directional light is used as ambient component for point light
	}
	else
	{
		if(scene.light.lightType == LIGHT_TYPE_DIRECTIONAL)
			isDirectional = true;
	}

	shaderManager->setLightType(isDirectional);
	RenderingScene *renderingScene = makeRenderingScene(scene, isDeferredRendering, terrainManager.get(), objectManager.get(), particleManager.get(), sceneManager->getChunkMargins(),
		shaderManager.get());

	//Create and initialize shaders
	std::map<int, unsigned long long> shaderFlags;
	vector<ShaderIds> &sceneShaders = shaderManager->createNeededShaders(shaderFlags);

	unique_ptr<Base3DRenderer> mainRenderer = buildMainRenderer(objectManager.get(), shaderManager.get(), appParameters.screenWidth, appParameters.screenHeight,
		sceneManager->getScene().light, sceneManager->getScene().fog, shaderFlags, sceneShaders, isDirectional, isDeferredRendering);
	mainRenderer->setRenderingScene(renderingScene);

	unique_ptr<PostprocessingRenderer> postprocessingRenderer;
	if(sceneManager->isPostprocessingRequired())
	{
		postprocessingRenderer = buildPostprocessingRenderer(appParameters.screenWidth, appParameters.screenHeight, sceneManager->getPostprocessingEffect(), shaderManager.get());

		if(sceneManager->isDeferredRendering())
		{
			static_cast<DeferredRenderer*>(mainRenderer.get())->setWriteFramebufferId(postprocessingRenderer->getFramebufferId());
		}
	}

	//Create core and renderer stuff

	EditorFrameRenderer *frameRenderer = new EditorFrameRenderer(mainRenderer.release(), postprocessingRenderer.release(), sceneManager->isDeferredRendering(), shaderManager.get());
	frameRenderer->setRendererLibraryProperties();

	//FlyingCamera
	TCameraController cameraController(sceneManager->getCameraData().xPos, sceneManager->getCameraData().yPos, sceneManager->getCameraData().zPos,
		sceneManager->getCameraData().horizontalRotationRadians, sceneManager->getCameraData().verticalRotationRadians);
	//FirstPersonCamera
	/*TCameraController cameraController(sceneManager->getCameraData().xPos, sceneManager->getCameraData().yPos, sceneManager->getCameraData().zPos,
		sceneManager->getCameraData().horizontalRotationRadians, sceneManager->getCameraData().verticalRotationRadians,
		*terrainManager, sceneManager->getScene());*/

	if(appParameters.isEditorMode)
	{
		Log::getInstance().info("Starting in editor mode");
		frameRenderer->initEditorShader();
		core = make_unique<EditorCore>(frameRenderer, window, cameraController, *sceneManager, terrainManager.get(), objectManager.get(), particleManager.get(), shaderManager.get());
	}
	else core = make_unique<Core>(window, frameRenderer, cameraController, *sceneManager, *shaderManager);

	stringstream memoryMessage;
	float bytesUsed = static_cast<float>(terrainManager->getTransferedBytesAmount()) + static_cast<float>(objectManager->getTransferedBytesAmount()) +
        static_cast<float>(particleManager->getTransferedBytesAmount());

	memoryMessage << std::fixed << setprecision(1) << (bytesUsed / 1024.f) / 1024.f;
	memoryMessage << " MB of data transfered to videocard";
	Log::getInstance().info(memoryMessage.str());

	return true;
}

void MainComponent::run()
{
	if(!core)
	{
		Log::getInstance().error("Can't start main loop due to initialization errors");
		return;
	}

	Log::getInstance().info("Entering the main loop");

	const time_t sceneStartTime = time(nullptr);

	core->mainLoop();

	const time_t sceneEndTime = time(nullptr);
	statistics.addTime(appParameters.scenePath, sceneEndTime - sceneStartTime);
	statistics.save();
}

namespace
{
	void writeContextInfoLogMessages()
	{
		const unsigned char *vendor = glGetString(GL_VENDOR);
		Log::getInstance().info(string("OpenGL vendor string:   ") + reinterpret_cast<const char*>(vendor));

		const unsigned char *version = glGetString(GL_VERSION);
		Log::getInstance().info(string("OpenGL version string:  ") + reinterpret_cast<const char*>(version));

		const unsigned char *renderer = glGetString(GL_RENDERER);
		Log::getInstance().info(string("OpenGL renderer string: ") + reinterpret_cast<const char*>(renderer));
	}
}

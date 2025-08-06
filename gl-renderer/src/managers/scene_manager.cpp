/* scene_manager.cpp
 * Keeps data of scene
 *
 * Author: Artem Hiblov
 */

#include "managers/scene_manager.h"

#include "log.h"
#include "loaders/scene_loader.h"

using namespace std;
using namespace renderer;
using namespace renderer::data;
using namespace renderer::managers;
using namespace renderer::loaders;

namespace
{
	const char *DEFERRED_RENDERER_STRING = "deferred";
	const char *NO_EFFECT_STRING = "--";
}

SceneManager::SceneManager(const string &scenePath)
{
	initialize(scenePath);
}

SceneManager::~SceneManager()
{
}

void SceneManager::computeChunkMargins(TerrainManager *terrainManager)
{
	/*
	Quad subdivision:
             farZ

	      +---+---+ rightX
	leftX | 3 | 4 |
	      +---+---+centerZ
	      | 1 | 2 |
	      +---+---+
	          centerX

	        nearZ

	*/

	if(!terrainManager)
		return;

	const int chunkAmount = scene.chunks.size();

	for(int i = 0; i < chunkAmount; i++)
	{
		const ChunkData &currentChunk = scene.chunks[i];
		const float sideLength = terrainManager->getChunkDimensions(currentChunk.name);

		ChunkMargins currentMargins;
		currentMargins.leftX = currentChunk.x;
		currentMargins.rightX = currentChunk.x + sideLength;
		currentMargins.nearZ = currentChunk.z;
		currentMargins.farZ = currentChunk.z - sideLength;
		currentMargins.centerX = (currentMargins.leftX + currentMargins.rightX) / 2.f;
		currentMargins.centerZ = (currentMargins.nearZ + currentMargins.farZ) / 2.f;

		chunkMargins[i] = currentMargins;
	}
}

const Camera& SceneManager::getCameraData() const
{
	return scene.camera;
}

const Light& SceneManager::getLightData() const
{
	return scene.light;
}

const Fog& SceneManager::getFogData() const
{
	return scene.fog;
}

map<int, ChunkMargins>& SceneManager::getChunkMargins()
{
	return chunkMargins;
}

Scene& SceneManager::getScene()
{
	return scene;
}

bool SceneManager::isPostprocessingRequired() const
{
	return scene.postprocessingEffect != NO_EFFECT_STRING;
}

const string& SceneManager::getPostprocessingEffect() const
{
	return scene.postprocessingEffect;
}

bool SceneManager::isDeferredRendering() const
{
	return scene.rendererType == DEFERRED_RENDERER_STRING;
}

void SceneManager::appendChunkDimensions(TerrainManager *terrainManager)
{
	if(!terrainManager)
			return;

	for(auto &current: scene.chunks)
	{
		float dimensions = terrainManager->getChunkDimensions(current.name);
		current.size = dimensions;
	}
}

void SceneManager::initialize(const string &scenePath)
{
	bool status = loadScene(scenePath, scene);
	if(!status)
	{
		Log::getInstance().error(string("Error initializing scene \"") + scenePath + "\"");
	}
}

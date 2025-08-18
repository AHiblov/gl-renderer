/* scene_manager.h
 * Keeps data of scene
 *
 * Author: Artem Hiblov
 */

#pragma once

#include <map>
#include <string>

#include "data/chunk_margins.h"
#include "data/scene.h"
#include "managers/terrain_manager.h"

namespace renderer::managers
{

class SceneManager
{
public:
	SceneManager(const std::string &scenePath);
	~SceneManager();

	void computeChunkMargins(renderer::managers::TerrainManager *terrainManager);

	//----- Getters -----

	const renderer::data::Camera& getCameraData() const;
	const renderer::data::Light& getLightData() const;
	const renderer::data::Fog& getFogData() const;

	std::map<int, renderer::data::ChunkMargins>& getChunkMargins();

	renderer::data::Scene& getScene();

	bool isPostprocessingRequired() const;
	const std::string& getPostprocessingEffect() const;
	bool isDeferredRendering() const;

	//----- Setters -----

	void appendChunkDimensions(renderer::managers::TerrainManager *terrainManager);

private:
	void initialize(const std::string &scenePath);



	renderer::data::Scene scene;
	std::map<int, renderer::data::ChunkMargins> chunkMargins;
};

}

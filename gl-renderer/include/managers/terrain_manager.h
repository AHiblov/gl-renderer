/* terrain_manager.h
 * Keeps data of all terrain chunks
 *
 * Author: Artem Hiblov
 */

#pragma once

#include <map>
#include <string>

#include "data/heightmap.h"
#include "data/terrain_file_paths.h"
#include "graphics_lib/videocard_data/object_rendering_data.h"

namespace renderer::managers
{

class TerrainManager
{
public:
	TerrainManager(const std::string &descriptionPath);
	~TerrainManager();

	float getChunkDimensions(const std::string &chunkName);

	/*
	@brief Initializes structure needed for chunk rendering
	@param[in] name - chunk name
	@param[out] data - structure to be filled
	*/
	bool getRenderingData(const std::string &name, renderer::graphics_lib::videocard_data::ObjectRenderingData &data);

	/*
	@brief Finds height on chunk for given coordinates
	@param[in] xOffset - X offset of chunk in world coordinates
	@param[in] zOffset - Z offset of chunk in world coordinates
	@param[in] chunkName - chunk name to find height in
	@param[in] xCoord - X coordinate on chunk for sought height
	@param[in] zCoord - Z coordinate on chunk for sought height
	@return Height
	*/
	float getHeight(float xOffset, float zOffset, const std::string &chunkName, float xCoord, float zCoord);

	int getTransferedBytesAmount() const;

private:
	/*
	@brief Loads descriptions from file
	*/
	void initDescription(const std::string &descriptionPath);

	/*
	@brief Initializes data needed for chunk rendering
	*/
	bool initTerrainData(const std::string &chunkName);



	std::map<std::string, renderer::graphics_lib::videocard_data::ObjectRenderingData> chunkIds;
	std::map<std::string, renderer::data::Heightmap> heightmap;
	std::map<std::string, float> dimensions;
	std::map<std::string, renderer::data::TerrainFilePaths> description;

	int transferedBytes;
};

}

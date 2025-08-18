/* terrain_loader.h
 * Loads terrain mesh into memory
 *
 * Author: Artem Hiblov
 */

#pragma once

#include <string>
#include <vector>

#include "data/heightmap.h"
#include "data/mesh.h"
#include "data/terrain_file_paths.h"

namespace renderer::loaders
{

/*
@brief Reads terrain from file
@param[in] path - path to file
@param[out] chunk - structure to store data
@param[out] heightmap - heights for all vertices
@param[out] sideLength - chunk side length
*/
bool loadTerrain(const std::string &path, renderer::data::Mesh &chunk, renderer::data::Heightmap &heightmap, float &sideLength);

bool loadTerrainDescription(const std::string &path, std::map<std::string, renderer::data::TerrainFilePaths> &description);

float readChunkDimensions(const std::string &path);

}

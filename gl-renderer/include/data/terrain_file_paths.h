/* terrain_file_paths.h
 * Keeps chunk data files paths
 *
 * Author: Artem Hiblov
 */

#pragma once

#include <map>
#include <string>

namespace renderer::data
{

struct TerrainFilePaths
{
	TerrainFilePaths()
	{}

	TerrainFilePaths(const std::string &mesh, const std::string &texture):
		meshPath(mesh), texturePath(texture)
	{}

	TerrainFilePaths(TerrainFilePaths &other):
		meshPath(other.meshPath), texturePath(other.texturePath)
	{}

	std::string meshPath;
	std::string texturePath;
};

}

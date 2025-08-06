/* object_file_paths.h
 * Keeps object files paths
 *
 * Author: Artem Hiblov
 */

#pragma once

#include <map>
#include <string>

namespace renderer::data
{

struct ObjectFilePaths
	{
		ObjectFilePaths()
		{}

		ObjectFilePaths(const std::string &mesh, const std::string &texture, const std::string &normalmap):
			meshPath(mesh), texturePath(texture), normalmapPath(normalmap)
		{}

		ObjectFilePaths(const ObjectFilePaths &other):
			meshPath(other.meshPath), texturePath(other.texturePath), normalmapPath(other.normalmapPath)
		{}

		std::string meshPath;
		std::string texturePath;
		std::string normalmapPath;
	};

}

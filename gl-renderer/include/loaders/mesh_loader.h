/* mesh_loader.h
 * Loads object mesh into memory
 *
 * Author: Artem Hiblov
 */

#pragma once

#include <string>

#include "data/mesh.h"
#include "data/object_file_paths.h"

namespace renderer::loaders
{

/*
@brief Reads mesh from file
*/
bool loadMesh(const std::string &path, renderer::data::Mesh &mesh);

bool loadObjectDescription(const std::string &path, std::map<std::string, renderer::data::ObjectFilePaths> &description);

}

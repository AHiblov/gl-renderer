/* scene_loader.h
 * Loads scene structure
 *
 * Author: Artem Hiblov
 */

#pragma once

#include <string>

#include "data/scene.h"

namespace renderer::loaders
{

/*
@brief Loads scene from given file
*/
bool loadScene(const std::string &path, renderer::data::Scene &scene);

}
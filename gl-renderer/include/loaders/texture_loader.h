/* texture_loader.h
 * Loads texture into memory
 *
 * Author: Artem Hiblov
 */

#pragma once

#include <string>

#include "data/texture.h"

namespace renderer::loaders
{

/*
@brief Loads texture from file
*/
bool loadTexture(const std::string &path, renderer::data::Texture &texture);

}
/* texture_operations.h
 * Creates and deletes texture
 * OpenGL 4.5
 *
 * Author: Artem Hiblov
 */

#pragma once

#include "data/texture.h"
#include "graphics_lib/videocard_data/object_rendering_data.h"

namespace renderer::graphics_lib::operations
{

/*
@brief Fills texture-related structure fields
*/
bool makeTexture(const renderer::data::Texture &texture, unsigned int &textureId);

/*
@brief Deletes texture on videocard
*/
void deleteTexture(unsigned int textureId);

}

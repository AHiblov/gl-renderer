/* terrain_operations.h
 * Creates and deletes terrain components
 * OpenGL 3.3
 *
 * Author: Artem Hiblov
 */

#pragma once

#include "data/mesh.h"
#include "graphics_lib/videocard_data/object_rendering_data.h"

namespace renderer::graphics_lib::operations
{

/*
@brief Fills mesh-related structure fields with data needed for terrain rendering
*/
bool makeTerrain(const renderer::data::Mesh &terrainData, renderer::graphics_lib::videocard_data::ObjectRenderingData &terrainIds);

/*
@brief Deletes all terrain components from videocard
*/
void deleteTerrain(const renderer::graphics_lib::videocard_data::ObjectRenderingData &objectIds);

}

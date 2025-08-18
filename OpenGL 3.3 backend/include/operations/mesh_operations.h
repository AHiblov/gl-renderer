/* mesh_operations.h
 * Creates and deletes mesh components
 * OpenGL 3.3
 *
 * Author: Artem Hiblov
 */

#pragma once

#include <vector>

#include "data/mesh.h"
#include "graphics_lib/videocard_data/object_rendering_data.h"

namespace renderer::graphics_lib::operations
{

/*
@brief Fills mesh-related structure fields with data needed for mesh rendering
*/
bool makeMesh(const renderer::data::Mesh &meshData, renderer::graphics_lib::videocard_data::ObjectRenderingData &meshIds);

/*
@brief Creates full-screen quad for texture output
*/
void initializeQuadMesh(unsigned int &bufferId);

/*
@brief Detetes all mesh components from videocard
*/
void deleteMesh(const renderer::graphics_lib::videocard_data::ObjectRenderingData &objectIds);

//Stub
void deleteContainer(unsigned int id);

}

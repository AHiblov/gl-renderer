/* mesh_operations.h
 * Creates and deletes mesh components
 * OpenGL 4.5
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

bool makeMeshWithClonedVbo(const renderer::graphics_lib::videocard_data::ObjectRenderingData &data, renderer::graphics_lib::videocard_data::ObjectRenderingData &meshIds);

/*
@brief Creates full-screen quad for texture output
*/
void initializeQuadMesh(unsigned int &vaoId);

/*
@brief Detetes all mesh components from videocard
*/
void deleteMesh(const renderer::graphics_lib::videocard_data::ObjectRenderingData &objectIds);

/*
@brief Deletes VAO
*/
void deleteContainer(unsigned int id);

}

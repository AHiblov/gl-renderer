/* particle_operations.h
 * Creates and deletes geometry instancing data
 * OpenGL 3.3
 *
 * Author: Artem Hiblov
 */

#pragma once

#include <glm/glm.hpp>

#include "graphics_lib/videocard_data/particle_rendering_data.h"

namespace renderer::graphics_lib::operations
{

/*
@brief Fills particle-related fields
*/
bool makeParticleGroup(const glm::vec3 *arrangement, const float *rotation, int recordAmount, renderer::graphics_lib::videocard_data::ParticleRenderingData &groupIds);

/*
@brief Detetes all particle components from videocard. Mesh is deleted by ObjectManager
@param[in] groupIds - IDs of mesh components
*/
void deleteParticleGroup(const renderer::graphics_lib::videocard_data::ParticleRenderingData &groupIds);

}

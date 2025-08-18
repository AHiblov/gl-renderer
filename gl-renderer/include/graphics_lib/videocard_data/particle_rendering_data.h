/* particle_rendering_data.h
 * Keeps particle data for frame renderer
 * OpenGL 4.5
 *
 * Author: Artem Hiblov
 */

#pragma once

#include "graphics_lib/videocard_data/object_rendering_data.h"

namespace renderer::graphics_lib::videocard_data
{

struct ParticleRenderingData
{
	ObjectRenderingData objectData;
	
	unsigned int arrangementBufferId = -1u;
	unsigned int rotationBufferId = -1u;
	
	int particleAmount = 0;
};

}
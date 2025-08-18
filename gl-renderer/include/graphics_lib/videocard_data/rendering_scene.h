/* rendering_scene.h
 * Keeps data needed for scene rendering
 * OpenGL 4.5
 *
 * Author: Artem Hiblov
 */

#pragma once

#include <glm/glm.hpp>

#include "graphics_lib/videocard_data/object_rendering_data.h"
#include "graphics_lib/videocard_data/particle_rendering_data.h"
#include "graphics_lib/videocard_data/quad_subdivision.h"

namespace renderer::graphics_lib::videocard_data
{

struct RenderingTerrain
{
	RenderingTerrain()
	{
		shaderIndex = 0;
	}

	RenderingTerrain(int shaderIdx, renderer::graphics_lib::videocard_data::ObjectRenderingData &obj, glm::mat4 &pos):
		terrainData(obj), position(pos)
	{
		shaderIndex = shaderIdx;
	}

	RenderingTerrain& operator=(const RenderingTerrain &other)
	{
		shaderIndex = other.shaderIndex;
		terrainData = other.terrainData;
		position = other.position;

		return *this;
	}

	int shaderIndex; //In array of created in shader manager indices
	renderer::graphics_lib::videocard_data::ObjectRenderingData terrainData;
	glm::mat4 position;
};

struct RenderingObjects
{
	renderer::graphics_lib::videocard_data::ObjectQuadSubdivision quad[4]; //Four parts for each chunk
};

struct RenderingParticles
{
	renderer::graphics_lib::videocard_data::ParticleQuadSubdivision quad[4]; //Four parts for each chunk
};

struct RenderingScene
{
	~RenderingScene()
	{
		if(terrain)
		{
			delete[] terrain;
			terrain = nullptr;
		}

		if(opaqueObjects)
		{
			delete[] opaqueObjects;
			opaqueObjects = nullptr;
		}

		if(transparentObjects)
		{
			delete[] transparentObjects;
			transparentObjects = nullptr;
		}

		if(particles)
        {
            delete[] particles;
            particles = nullptr;
        }
	}

	int chunkAmount = 0;

	RenderingTerrain *terrain = nullptr;

	renderer::graphics_lib::videocard_data::RenderingObjects *opaqueObjects = nullptr;
	renderer::graphics_lib::videocard_data::RenderingParticles *particles = nullptr;
	renderer::graphics_lib::videocard_data::RenderingObjects *transparentObjects = nullptr;

	renderer::graphics_lib::videocard_data::ObjectRenderingData sky;
};

}

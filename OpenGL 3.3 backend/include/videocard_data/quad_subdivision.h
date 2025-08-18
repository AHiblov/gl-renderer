/* quad_subdivision.h
 * Chunk division into parts
 * OpenGL 3.3
 *
 * Author: Artem hiblov
 */

#pragma once

#include "graphics_lib/videocard_data/object_rendering_data.h"
#include "graphics_lib/videocard_data/particle_rendering_data.h"

namespace renderer::graphics_lib::videocard_data
{

struct ObjectQuadSubdivision
{
	struct ObjectNode
	{
		ObjectNode():
			arrangement(nullptr), rotation(nullptr), amount(0)
		{
			shaderIndex = 0;
		}

		ObjectNode(int shaderIdx, renderer::graphics_lib::videocard_data::ObjectRenderingData &obj, glm::mat4 *arrangementArray, glm::mat3 *rotationArray, int objectAmount):
			objectData(obj), amount(objectAmount)
		{
			shaderIndex = shaderIdx;

			arrangement = new glm::mat4 [amount];
			for(int i = 0; i < amount; i++)
				arrangement[i] = arrangementArray[i];

			rotation = new glm::mat3 [amount];
			for(int i = 0; i < amount; i++)
				rotation[i] = rotationArray[i];
		}

		ObjectNode(const ObjectNode &other):
			objectData(other.objectData), amount(other.amount)
		{
			shaderIndex = other.shaderIndex;

			arrangement = new glm::mat4 [amount];
			for(int i = 0; i < amount; i++)
				arrangement[i] = other.arrangement[i];

			rotation = new glm::mat3 [amount];
			for(int i = 0; i < amount; i++)
				rotation[i] = other.rotation[i];
		}

		~ObjectNode()
		{
			if(arrangement)
			{
				delete[] arrangement;
				arrangement = nullptr;
			}

			if(rotation)
			{
				delete[] rotation;
				rotation = nullptr;
			}
		}

		ObjectNode& operator=(const ObjectNode &other)
		{
			shaderIndex = other.shaderIndex;

			objectData = other.objectData;
			amount = other.amount;

			arrangement = new glm::mat4 [amount];
			for(int i = 0; i < amount; i++)
				arrangement[i] = other.arrangement[i];

			rotation = new glm::mat3 [amount];
			for(int i = 0; i < amount; i++)
				rotation[i] = other.rotation[i];

			return *this;
		}

		ObjectNode& operator=(ObjectNode &&other)
		{
			shaderIndex = other.shaderIndex;

			objectData = other.objectData;
			amount = other.amount;

			glm::mat4 *tempArrangement = other.arrangement;
			other.arrangement = arrangement;
			arrangement = tempArrangement;

			glm::mat3 *tempRotation = other.rotation;
			other.rotation = rotation;
			rotation = tempRotation;

			return *this;
		}

		int shaderIndex; //In array of created in shader manager indices
		renderer::graphics_lib::videocard_data::ObjectRenderingData objectData;
		glm::mat4 *arrangement;
		glm::mat3 *rotation;
		int amount;
	};

	renderer::graphics_lib::videocard_data::ObjectQuadSubdivision::ObjectNode *objectsInQuad = nullptr;
	int amount = 0;
};

struct ParticleQuadSubdivision
{
	struct ParticleNode
	{
		ParticleNode()
		{
			shaderIndex = 0;
		}

		ParticleNode(int shaderIdx, const renderer::graphics_lib::videocard_data::ParticleRenderingData &obj, const glm::mat4 &arrangementMatr):
			data(obj), arrangement(arrangementMatr)
		{
			shaderIndex = shaderIdx;
		}

		ParticleNode(const ParticleNode &other):
			data(other.data), arrangement(other.arrangement)
		{
			shaderIndex = other.shaderIndex;
		}

		~ParticleNode()
		{}

		ParticleNode& operator=(const ParticleNode &other)
		{
			shaderIndex = other.shaderIndex;

			data = other.data;
			arrangement = other.arrangement;

			return *this;
		}

		int shaderIndex;
		renderer::graphics_lib::videocard_data::ParticleRenderingData data;
		glm::mat4 arrangement;
	};

	renderer::graphics_lib::videocard_data::ParticleQuadSubdivision::ParticleNode *groupsInQuad = nullptr;
	int amount = 0;
};

}

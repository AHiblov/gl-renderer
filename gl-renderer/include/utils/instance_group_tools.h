/* instance_group_tools.h
 * Generates instance positions
 *
 * Author: Artem Hiblov
 */

#pragma once

#include <random>
#include <vector>

#include <glm/glm.hpp>

#include "data/scene.h"
#include "managers/terrain_manager.h"

namespace renderer::utils
{

/*
@brief Generates random absolute positions for given density
@param[in] density - instance density
@param[in] center - center of the area for insertions
@param[in] radius - area radius
@param[in] terrainManager - provides height for given coordinates
@param[in] chunk - chunk to insert in
@return Array of 3-component positions
*/
std::vector<float> generateInstanceAbsolutePositions(float density, const glm::vec3 &center, float radius, renderer::managers::TerrainManager &terrainManager, const renderer::data::ChunkData &chunk,
	std::uniform_int_distribution<> &distribution, std::mt19937 &rng);

/*
@brief Generates random positions relative to center for given density
@param[in] density - instance density
@param[in] center - center of the area for insertions
@param[in] radius - area radius
@param[in] terrainManager - provides height for given coordinates
@param[in] chunk - chunk to insert in
@return Array of 3-component positions
*/
std::vector<float> generateInstanceRelativePositions(float density, const glm::vec3 &center, float radius, renderer::managers::TerrainManager &terrainManager, const renderer::data::ChunkData &chunk,
	std::uniform_int_distribution<> &distribution, std::mt19937 &rng);

}

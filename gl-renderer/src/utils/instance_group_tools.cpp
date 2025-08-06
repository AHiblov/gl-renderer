/* instance_group_tools.cpp
 * Generates instance positions
 *
 * Author: Artem Hiblov
 */

#include "utils/instance_group_tools.h"

#include <cmath>
#include <cstdlib>
#include <random>

using namespace std;
using namespace renderer::data;
using namespace renderer::managers;
using namespace renderer::utils;

namespace
{
	constexpr int TWO_PI_INTEGER = 62831; //62831 = 2 * pi * 10000
	constexpr float MATH_PI = 3.14159f;
}

vector<float> renderer::utils::generateInstanceAbsolutePositions(float density, const glm::vec3 &center, float radius, TerrainManager &terrainManager, const ChunkData &chunk,
	uniform_int_distribution<> &distribution, mt19937 &rng)
{
	int instanceAmount = static_cast<int>((MATH_PI * (radius * radius)) * density);
	if(instanceAmount < 1)
		instanceAmount = 1;

	vector<float> positions;
	positions.reserve(instanceAmount * 3);

	for(int i = 0; i < instanceAmount; i++)
	{
		float positionAngle = static_cast<float>((distribution(rng) % TWO_PI_INTEGER) / 10000.f); //Angle on circle
		float curRadius = radius * sqrt(static_cast<float>(distribution(rng) % 10000) / 10000.f);

		float curX = center.x + curRadius * cos(positionAngle);
		float curZ = center.z + curRadius * sin(positionAngle);
		float curY = terrainManager.getHeight(chunk.x, chunk.z, chunk.name, curX, curZ);

		positions.push_back(curX);
		positions.push_back(curY);
		positions.push_back(curZ);
	}

	return positions;
}

vector<float> renderer::utils::generateInstanceRelativePositions(float density, const glm::vec3 &center, float radius, TerrainManager &terrainManager, const ChunkData &chunk,
	uniform_int_distribution<> &distribution, mt19937 &rng)
{
	int instanceAmount = static_cast<int>((MATH_PI * (radius * radius)) * density);
	if(instanceAmount < 1)
		instanceAmount = 1;

	vector<float> positions;
	positions.reserve(instanceAmount * 3);

	for(int i = 0; i < instanceAmount; i++)
	{
		float positionAngle = static_cast<float>((distribution(rng) % TWO_PI_INTEGER) / 10000.f); //Angle on circle
		float curRadius = radius * sqrt(static_cast<float>(distribution(rng) % 10000) / 10000.f);

		float relativeX = curRadius * cos(positionAngle);
		float relativeZ = curRadius * sin(positionAngle);

		float curX = relativeX;
		float curZ = relativeZ;
		float curY = terrainManager.getHeight(chunk.x, chunk.z, chunk.name, center.x + relativeX, center.z + relativeZ);

		positions.push_back(curX);
		positions.push_back(curY);
		positions.push_back(curZ);
	}

	return positions;
}

/* heightmap.h
 * Keeps all heightmap related data
 *
 * Author: Artem Hiblov
 */

#pragma once

#include <vector>

namespace renderer::data
{

struct Heightmap
{
	std::vector<std::vector<float>> heights;
	int verticesInSide;
	float gridStep;

	Heightmap():
		verticesInSide(0), gridStep(0)
	{}

	Heightmap(const Heightmap &heightmap)
	{
		heights = heightmap.heights;
		verticesInSide = heightmap.verticesInSide;
		gridStep = heightmap.gridStep;
	}

	Heightmap(Heightmap &&heightmap)
	{
		heights = std::move(heightmap.heights);

		verticesInSide = heightmap.verticesInSide;
		gridStep = heightmap.gridStep;
	}

	Heightmap& operator=(const Heightmap &heightmap)
	{
		heights = heightmap.heights;
		verticesInSide = heightmap.verticesInSide;
		gridStep = heightmap.gridStep;

		return *this;
	}

	Heightmap& operator=(Heightmap &&heightmap)
	{
		heights = std::move(heightmap.heights);

		verticesInSide = heightmap.verticesInSide;
		gridStep = heightmap.gridStep;

		return *this;
	}
};

}
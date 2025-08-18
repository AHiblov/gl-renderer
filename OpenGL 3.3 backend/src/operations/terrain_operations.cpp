/* terrain_operations.cpp
 * Creates and deletes terrain components
 * OpenGL 3.3
 *
 * Author: Artem Hiblov
 */

#include "graphics_lib/operations/terrain_operations.h"

#include <vector>

#include <GL/glew.h>

#include "log.h"

using namespace std;
using namespace renderer;
using namespace renderer::data;
using namespace renderer::graphics_lib;
using namespace renderer::graphics_lib::operations;
using namespace renderer::graphics_lib::videocard_data;

namespace
{
	/*
	@brief Makes VBO and transfers data to it
	@param[in] data - data to transfer
	@return VBO ID
	*/
	unsigned int makeVBO(const std::vector<float> &data);
}

bool renderer::graphics_lib::operations::makeTerrain(const Mesh &terrainData, ObjectRenderingData &terrainIds)
{
	if(!terrainData.vertices.size() || !terrainData.uvs.size())
	{
		Log::getInstance().error("Not enough data to create mesh");
		return false;
	}

	unsigned int vaoId = -1u;
	glGenVertexArrays(1, &vaoId);
	glBindVertexArray(vaoId);

	unsigned int vertexVboId = makeVBO(terrainData.vertices);
	unsigned int uvVboId = makeVBO(terrainData.uvs);
	unsigned int normalsVboId = makeVBO(terrainData.normals);

	terrainIds.vaoId = vaoId;
	terrainIds.vertexBufferId = vertexVboId;
	terrainIds.uvBufferId = uvVboId;
	terrainIds.normalBufferId = normalsVboId;

	terrainIds.vertexAmount = terrainData.vertices.size() / 3; //Always 3 coordinates per vertex

	return true;
}

void renderer::graphics_lib::operations::deleteTerrain(const ObjectRenderingData &objectIds)
{
	glDeleteBuffers(1, &objectIds.vertexBufferId);
	glDeleteBuffers(1, &objectIds.uvBufferId);
	glDeleteBuffers(1, &objectIds.normalBufferId);
	glDeleteVertexArrays(1, &objectIds.vaoId);
}

namespace
{
	unsigned int makeVBO(const vector<float> &data)
	{
		unsigned int vboId = -1u;

		glGenBuffers(1, &vboId);

		glBindBuffer(GL_ARRAY_BUFFER, vboId);
		glBufferData(GL_ARRAY_BUFFER, data.size() * sizeof(float), data.data(), GL_STATIC_DRAW);

		return vboId;
	}
}

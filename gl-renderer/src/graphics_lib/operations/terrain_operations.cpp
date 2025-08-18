/* terrain_operations.cpp
 * Creates and deletes terrain components
 * OpenGL 4.5
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

	//Array of structures

	size_t mergedSize = terrainData.vertices.size() + terrainData.uvs.size() + terrainData.normals.size();
	vector<float> mergedBuffer;
	mergedBuffer.reserve(mergedSize);
	int vertexAmount = terrainData.vertices.size() / 3;
	for(int i = 0; i < vertexAmount; i++)
	{
		mergedBuffer.push_back(terrainData.vertices[i*3]);
		mergedBuffer.push_back(terrainData.vertices[i*3+1]);
		mergedBuffer.push_back(terrainData.vertices[i*3+2]);
		mergedBuffer.push_back(terrainData.uvs[i*2]);
		mergedBuffer.push_back(terrainData.uvs[i*2+1]);
		mergedBuffer.push_back(terrainData.normals[i*3]);
		mergedBuffer.push_back(terrainData.normals[i*3+1]);
		mergedBuffer.push_back(terrainData.normals[i*3+2]);
	}

	unsigned int vboId = makeVBO(mergedBuffer);

	int unitSize = (3 + 2 + 3) * sizeof(float);

	unsigned int vaoId = -1u;
	glCreateVertexArrays(1, &vaoId);
	glVertexArrayVertexBuffer(vaoId, 0, vboId, 0, unitSize);

	glEnableVertexArrayAttrib(vaoId, 0);
	glVertexArrayAttribFormat(vaoId, 0, 3, GL_FLOAT, GL_FALSE, 0);
	glVertexArrayAttribBinding(vaoId, 0, 0);
	glVertexArrayBindingDivisor(vaoId, 0, 0);

	glEnableVertexArrayAttrib(vaoId, 1);
	glVertexArrayAttribFormat(vaoId, 1, 2, GL_FLOAT, GL_FALSE, 3 * sizeof(float));
	glVertexArrayAttribBinding(vaoId, 1, 0);

	glEnableVertexArrayAttrib(vaoId, 2);
	glVertexArrayAttribFormat(vaoId, 2, 3, GL_FLOAT, GL_FALSE, (3 + 2) * sizeof(float));
	glVertexArrayAttribBinding(vaoId, 2, 0);

	terrainIds.vaoId = vaoId;
	terrainIds.vertexBufferId = vboId;

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

		glCreateBuffers(1, &vboId);
		glNamedBufferStorage(vboId, data.size() * sizeof(float), data.data(), 0);

		return vboId;
	}
}

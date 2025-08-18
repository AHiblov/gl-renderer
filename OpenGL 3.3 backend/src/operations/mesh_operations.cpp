/* mesh_operations.cpp
 * Creates and deletes mesh components
 * OpenGL 3.3
 *
 * Author: Artem Hiblov
 */

#include "graphics_lib/operations/mesh_operations.h"

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
	constexpr int QUAD_MESH_ARRAY_SIZE = 6 * 3; //6 vertices, 3 components per vertex

	/*
	@brief Makes VBO and transfers data to it
	@param[in] data - data to transfer
	@return VBO ID
	*/
	unsigned int makeVBO(const std::vector<float> &data);
}

bool renderer::graphics_lib::operations::makeMesh(const Mesh &meshData, ObjectRenderingData &meshIds)
{
	if(!meshData.vertices.size() || !meshData.uvs.size())
	{
		Log::getInstance().error("Not enough data to create mesh");
		return false;
	}

	unsigned int vaoId = -1u;
	glGenVertexArrays(1, &vaoId);
	glBindVertexArray(vaoId);

	unsigned int vertexVboId = makeVBO(meshData.vertices);
	unsigned int uvVboId = makeVBO(meshData.uvs);
	unsigned int normalsVboId = -1u;
	if(!meshData.normals.empty())
	{
		normalsVboId = makeVBO(meshData.normals);
	}

	meshIds.vaoId = vaoId;
	meshIds.vertexBufferId = vertexVboId;
	meshIds.uvBufferId = uvVboId;
	meshIds.normalBufferId = normalsVboId;

	meshIds.vertexAmount = meshData.vertices.size() / meshData.floatsPerVertex;

	//Tangent basis
	if(meshData.tangent.size() && meshData.bitangent.size())
	{
		unsigned int tangentBufferId = makeVBO(meshData.tangent);
		unsigned int bitangentBufferId = makeVBO(meshData.bitangent);

		meshIds.tangentBufferId = tangentBufferId;
		meshIds.bitangentBufferId = bitangentBufferId;
	}

	return true;
}

void renderer::graphics_lib::operations::initializeQuadMesh(unsigned int &bufferId)
{
	float quadMesh[QUAD_MESH_ARRAY_SIZE] = {
		-1.f, -1.f, 0.f,
		1.f, -1.f, 0.f,
		-1.f, 1.f, 0.f,
		-1.f, 1.f, 0.f,
		1.f, -1.f, 0.f,
		1.f, 1.f, 0.f
	};

	glGenBuffers(1, &bufferId);
	glBindBuffer(GL_ARRAY_BUFFER, bufferId);
	glBufferData(GL_ARRAY_BUFFER, sizeof(quadMesh), quadMesh, GL_STATIC_DRAW);
}

void renderer::graphics_lib::operations::deleteMesh(const ObjectRenderingData &objectIds)
{
	glDeleteBuffers(1, &objectIds.vertexBufferId);
	glDeleteBuffers(1, &objectIds.uvBufferId);
	if(objectIds.normalBufferId != -1u)
		glDeleteBuffers(1, &objectIds.normalBufferId);
	if(objectIds.tangentBufferId != -1u)
		glDeleteBuffers(1, &objectIds.tangentBufferId);
	if(objectIds.bitangentBufferId != -1u)
		glDeleteBuffers(1, &objectIds.bitangentBufferId);
	glDeleteVertexArrays(1, &objectIds.vaoId);
}

void renderer::graphics_lib::operations::deleteContainer(unsigned int id)
{
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

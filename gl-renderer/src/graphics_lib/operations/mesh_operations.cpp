/* mesh_operations.cpp
 * Creates and deletes mesh components
 * OpenGL 4.5
 *
 * Author: Artem Hiblov
 */

#include "graphics_lib/operations/mesh_operations.h"

#include <vector>

#include <GL/glew.h>

#include "log.h"
#include "graphics_lib/videocard_data/component_indices.h"

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

	const bool is3DMesh = meshData.floatsPerVertex == 3;
	const bool useTangentBasis = !meshData.tangent.empty() && !meshData.bitangent.empty();

	//Structure of arrays

	unsigned int vertexVboId = makeVBO(meshData.vertices);
	unsigned int uvVboId = makeVBO(meshData.uvs);
	unsigned int normalsVboId = -1u;
	if(!meshData.normals.empty())
	{
		normalsVboId = makeVBO(meshData.normals);
	}

	unsigned int vaoId = -1u;
	glCreateVertexArrays(1, &vaoId);

	glVertexArrayVertexBuffer(vaoId, COMPONENT_VERTEX, vertexVboId, 0, meshData.floatsPerVertex * sizeof(float));
	glVertexArrayVertexBuffer(vaoId, 1, uvVboId, 0, 2 * sizeof(float));
	if(is3DMesh)
		glVertexArrayVertexBuffer(vaoId, 2, normalsVboId, 0, 3 * sizeof(float));

	glEnableVertexArrayAttrib(vaoId, COMPONENT_VERTEX);
	glVertexArrayAttribFormat(vaoId, COMPONENT_VERTEX, meshData.floatsPerVertex, GL_FLOAT, GL_FALSE, 0);
	glVertexArrayAttribBinding(vaoId, COMPONENT_VERTEX, COMPONENT_VERTEX);
	glVertexArrayBindingDivisor(vaoId, COMPONENT_VERTEX, 0);

	glEnableVertexArrayAttrib(vaoId, COMPONENT_UV);
	glVertexArrayAttribFormat(vaoId, COMPONENT_UV, 2, GL_FLOAT, GL_FALSE, 0);
	glVertexArrayAttribBinding(vaoId, COMPONENT_UV, COMPONENT_UV);

	if(is3DMesh)
	{
		glEnableVertexArrayAttrib(vaoId, COMPONENT_NORMAL);
		glVertexArrayAttribFormat(vaoId, COMPONENT_NORMAL, 3, GL_FLOAT, GL_FALSE, 0);
		glVertexArrayAttribBinding(vaoId, COMPONENT_NORMAL, COMPONENT_NORMAL);

		if(useTangentBasis)
		{
			unsigned int tangentBufferId = makeVBO(meshData.tangent);
			unsigned int bitangentBufferId = makeVBO(meshData.bitangent);

			glVertexArrayVertexBuffer(vaoId, COMPONENT_TANGENT, tangentBufferId, 0, meshData.floatsPerVertex * sizeof(float));
			glVertexArrayVertexBuffer(vaoId, COMPONENT_BITANGENT, bitangentBufferId, 0, meshData.floatsPerVertex * sizeof(float));

			glEnableVertexArrayAttrib(vaoId, COMPONENT_TANGENT);
			glVertexArrayAttribFormat(vaoId, COMPONENT_TANGENT, 3, GL_FLOAT, GL_FALSE, 0);
			glVertexArrayAttribBinding(vaoId, COMPONENT_TANGENT, COMPONENT_TANGENT);

			glEnableVertexArrayAttrib(vaoId, COMPONENT_BITANGENT);
			glVertexArrayAttribFormat(vaoId, COMPONENT_BITANGENT, 3, GL_FLOAT, GL_FALSE, 0);
			glVertexArrayAttribBinding(vaoId, COMPONENT_BITANGENT, COMPONENT_BITANGENT);

			meshIds.tangentBufferId = tangentBufferId;
			meshIds.bitangentBufferId = bitangentBufferId;
		}
	}

	meshIds.vaoId = vaoId;
	meshIds.vertexBufferId = vertexVboId;
	meshIds.uvBufferId = uvVboId;
	meshIds.normalBufferId = normalsVboId;

	meshIds.vertexAmount = meshData.vertices.size() / meshData.floatsPerVertex;

	return true;
}

bool renderer::graphics_lib::operations::makeMeshWithClonedVbo(const ObjectRenderingData &data, ObjectRenderingData &meshIds)
{
	if(data.vertexBufferId == -1u || data.uvBufferId == -1u || data.normalBufferId == -1u)
	{
		Log::getInstance().error("Not enough data for cloning");
		return false;
	}

	unsigned int vaoId = -1u;
	glCreateVertexArrays(1, &vaoId);

	glVertexArrayVertexBuffer(vaoId, COMPONENT_VERTEX, data.vertexBufferId, 0, 3 * sizeof(float));
	glVertexArrayVertexBuffer(vaoId, 1, data.uvBufferId, 0, 2 * sizeof(float));
	glVertexArrayVertexBuffer(vaoId, 2, data.normalBufferId, 0, 3 * sizeof(float));

	glEnableVertexArrayAttrib(vaoId, COMPONENT_VERTEX);
	glVertexArrayAttribFormat(vaoId, COMPONENT_VERTEX, 3, GL_FLOAT, GL_FALSE, 0);
	glVertexArrayAttribBinding(vaoId, COMPONENT_VERTEX, COMPONENT_VERTEX);
	glVertexArrayBindingDivisor(vaoId, COMPONENT_VERTEX, 0);

	glEnableVertexArrayAttrib(vaoId, COMPONENT_UV);
	glVertexArrayAttribFormat(vaoId, COMPONENT_UV, 2, GL_FLOAT, GL_FALSE, 0);
	glVertexArrayAttribBinding(vaoId, COMPONENT_UV, COMPONENT_UV);

	glEnableVertexArrayAttrib(vaoId, COMPONENT_NORMAL);
	glVertexArrayAttribFormat(vaoId, COMPONENT_NORMAL, 3, GL_FLOAT, GL_FALSE, 0);
	glVertexArrayAttribBinding(vaoId, COMPONENT_NORMAL, COMPONENT_NORMAL);

	meshIds.vaoId = vaoId;
	meshIds.vertexBufferId = data.vertexBufferId;
	meshIds.uvBufferId = data.uvBufferId;
	meshIds.normalBufferId = data.normalBufferId;

	meshIds.vertexAmount = data.vertexAmount;

	meshIds.textureId = data.textureId;

	return true;
}

void renderer::graphics_lib::operations::initializeQuadMesh(unsigned int &vaoId)
{
	float quadMesh[QUAD_MESH_ARRAY_SIZE] = {
		-1.f, -1.f, 0.f,
		1.f, -1.f, 0.f,
		-1.f, 1.f, 0.f,
		-1.f, 1.f, 0.f,
		1.f, -1.f, 0.f,
		1.f, 1.f, 0.f
	};

	unsigned int vboId = -1u;
	glCreateBuffers(1, &vboId);
	glNamedBufferStorage(vboId, QUAD_MESH_ARRAY_SIZE * sizeof(float), quadMesh, 0);

	glCreateVertexArrays(1, &vaoId);

	glVertexArrayVertexBuffer(vaoId, 0, vboId, 0, 3 * sizeof(float));

	glEnableVertexArrayAttrib(vaoId, 0);
	glVertexArrayAttribFormat(vaoId, 0, 3, GL_FLOAT, GL_FALSE, 0);
	glVertexArrayAttribBinding(vaoId, 0, 0);
}

void renderer::graphics_lib::operations::deleteMesh(const ObjectRenderingData &objectIds)
{
	glDeleteBuffers(1, &objectIds.vertexBufferId);
	glDeleteVertexArrays(1, &objectIds.vaoId);
}

void renderer::graphics_lib::operations::deleteContainer(unsigned int id)
{
	glDeleteVertexArrays(1, &id);
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

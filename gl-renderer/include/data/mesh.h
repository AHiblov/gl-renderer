/* mesh.h
 * Contains all mesh data
 *
 * Author: Artem Hiblov
 */

#pragma once

#include <vector>

namespace renderer::data
{

struct Mesh
{
	Mesh():
		floatsPerVertex(0)
	{}

	Mesh(std::vector<float> &vertices, std::vector<float> &uvs, std::vector<float> &normals, int floatsPerVertex)
	{
		this->vertices = std::move(vertices);
		this->uvs = std::move(uvs);
		this->normals = std::move(normals);
		this->floatsPerVertex = floatsPerVertex;
	}

	Mesh(const Mesh &other)
	{
		this->vertices = other.vertices;
		this->uvs = other.uvs;
		this->normals = other.normals;
		this->tangent = other.tangent;
		this->bitangent = other.bitangent;
		this->floatsPerVertex = other.floatsPerVertex;
	}

	Mesh& operator=(const Mesh &other)
	{
		this->vertices = other.vertices;
		this->uvs = other.uvs;
		this->normals = other.normals;
		this->tangent = other.tangent;
		this->bitangent = other.bitangent;
		this->floatsPerVertex = other.floatsPerVertex;

		return *this;
	}

	std::vector<float> vertices;
	std::vector<float> uvs;
	std::vector<float> normals;

	std::vector<float> tangent;
	std::vector<float> bitangent;

	int floatsPerVertex; //Not used for terrain
};

}
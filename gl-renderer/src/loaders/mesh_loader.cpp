/* mesh_loader.cpp
 * Loads object mesh into memory
 *
 * Author: Artem Hiblov
 */

#include <cstring>
#include <fstream>

#include "log.h"
#include "loaders/mesh_loader.h"

using namespace std;
using namespace renderer::data;
using namespace renderer::loaders;

namespace
{
	constexpr int MESH_FILE_SIGNATURE_LENGTH = 4;

	constexpr int TWO_FLOATS_PER_COORD_FLAG = 1;
	constexpr int TANGENT_BASIS_FLAG = 2;

	const char *MESH_FILE_SIGNATURE = "mesh";
}

bool renderer::loaders::loadMesh(const string &path, Mesh &mesh)
{
	if(path.empty())
	{
		Log::getInstance().error("Path for mesh is not provided");
		return false;
	}

	ifstream data(path, ios::in | ios::binary);
	if(!data.is_open())
	{
		Log::getInstance().error(path + " can't be opened");
		return false;
	}

	char signature[MESH_FILE_SIGNATURE_LENGTH+1] = {'\0'}; //1 is for \0 cheracter
	data.read(signature, MESH_FILE_SIGNATURE_LENGTH);
	signature[MESH_FILE_SIGNATURE_LENGTH] = '\0';
	if(strcmp(signature, MESH_FILE_SIGNATURE) != 0)
	{
		Log::getInstance().error(string("Invalid signature for file ") + path);
		return false;
	}

	int formatType = 0;
	int vertexAmount = 0;

	data.read(reinterpret_cast<char*>(&formatType), sizeof(int));
	data.read(reinterpret_cast<char*>(&vertexAmount), sizeof(int));

	int floatsPerVertex = 3;
	if((formatType & TWO_FLOATS_PER_COORD_FLAG) == 1) //Floats per position: 0 = 3 floats, 1 = 2 floats
		floatsPerVertex = 2;
	bool hasTangent = formatType & TANGENT_BASIS_FLAG;

	mesh.floatsPerVertex = floatsPerVertex;

	int vertexArraySize = vertexAmount * floatsPerVertex;
	mesh.vertices.resize(vertexArraySize);
	data.read(reinterpret_cast<char*>(mesh.vertices.data()), vertexArraySize * sizeof(float));

	int uvArraySize = vertexAmount * 2; //Always 2 floats per vertex
	mesh.uvs.resize(uvArraySize);
	data.read(reinterpret_cast<char*>(mesh.uvs.data()), uvArraySize * sizeof(float));

	if(floatsPerVertex == 3) //2D meshes don't have normals
	{
		int normalsArraySize = vertexAmount * 3; //Always 3 floats per vertex
		mesh.normals.resize(normalsArraySize);
		data.read(reinterpret_cast<char*>(mesh.normals.data()), normalsArraySize * sizeof(float));
	}

	if(hasTangent)
	{
		mesh.tangent.resize(vertexArraySize);
		data.read(reinterpret_cast<char*>(mesh.tangent.data()), vertexArraySize * sizeof(float));
		mesh.bitangent.resize(vertexArraySize);
		data.read(reinterpret_cast<char*>(mesh.bitangent.data()), vertexArraySize * sizeof(float));
	}

	data.close();
	return true;
}

bool renderer::loaders::loadObjectDescription(const string &path, map<string, ObjectFilePaths> &description)
{
	ifstream data(path);
	if(!data.is_open())
	{
		Log::getInstance().error(string("\"") + path + "\" can't be opened");
		return false;
	}

	int amount = 0;
	data >> amount;

	for(int i = 0; i < amount; i++)
	{
		string objectName, meshPath, texturePath, normalmapPath;

		data >> objectName >> meshPath >> texturePath >> normalmapPath;
		description[objectName] = ObjectFilePaths(meshPath, texturePath, normalmapPath);
	}

	data.close();

	return true;
}

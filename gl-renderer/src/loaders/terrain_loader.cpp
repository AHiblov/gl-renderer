/* terrain_loader.cpp
 * Loads terrain into memory
 *
 * Author: Artem Hiblov
 */

#include "loaders/terrain_loader.h"

#include <cstring>
#include <fstream>

#include "log.h"

using namespace std;
using namespace renderer::data;
using namespace renderer::loaders;

namespace
{
	constexpr int TERRAIN_FILE_SIGNATURE_LENGTH = 7;

	const char *TERRAIN_FILE_SIGNATURE = "terrain";
}

bool renderer::loaders::loadTerrain(const string &path, Mesh &chunk, Heightmap &heightmap, float &sideLength)
{
	if(path.empty())
	{
		Log::getInstance().error("Path for terrain mesh is not provided");
		return false;
	}

	ifstream data(path, ios::in | ios::binary);
	if(!data.is_open())
	{
		Log::getInstance().error(path + " can't be opened");
		return false;
	}

	char signature[TERRAIN_FILE_SIGNATURE_LENGTH+1] = {'\0'};
	data.read(signature, TERRAIN_FILE_SIGNATURE_LENGTH);
	signature[TERRAIN_FILE_SIGNATURE_LENGTH] = '\0';
	if(strcmp(signature, TERRAIN_FILE_SIGNATURE) != 0)
	{
		Log::getInstance().error(string("Invalid signature for file ") + path);
		return false;
	}

	data.read(reinterpret_cast<char*>(&sideLength), sizeof(float));

	chunk.floatsPerVertex = 0;

	int vertexAmount = 0;
	data.read(reinterpret_cast<char*>(&vertexAmount), sizeof(int));

	int vertexArraySize = vertexAmount * 3; //3 coordinates per vertex and normal
	chunk.vertices.resize(vertexArraySize);
	data.read(reinterpret_cast<char*>(chunk.vertices.data()), vertexArraySize * sizeof(float));

	int uvArraySize = vertexAmount * 2; //2 coordinates for texture
	chunk.uvs.resize(uvArraySize);
	data.read(reinterpret_cast<char*>(chunk.uvs.data()), uvArraySize * sizeof(float));

	chunk.normals.resize(vertexArraySize);
	data.read(reinterpret_cast<char*>(chunk.normals.data()), vertexArraySize * sizeof(float));

	//Heightmap

	data.read(reinterpret_cast<char*>(&heightmap.verticesInSide), sizeof(int));
	data.read(reinterpret_cast<char*>(&heightmap.gridStep), sizeof(float));
	for(int i = 0; i < heightmap.verticesInSide; i++)
	{
		vector<float> row(heightmap.verticesInSide);
		for(int j = 0; j < heightmap.verticesInSide; j++)
		{
			data.read(reinterpret_cast<char*>(&row[j]), sizeof(float));
		}

		heightmap.heights.emplace_back(row);
	}

	data.close();

	return true;
}

bool renderer::loaders::loadTerrainDescription(const string &path, map<string, TerrainFilePaths> &description)
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
		string chunkName, meshPath, texturePath;

		data >> chunkName >> meshPath >> texturePath;
		description[chunkName] = TerrainFilePaths(meshPath, texturePath);
	}

	data.close();

	return true;
}

float renderer::loaders::readChunkDimensions(const std::string &path)
{
	if(path.empty())
	{
		Log::getInstance().error("Path for terrain mesh is not provided");
		return false;
	}

	ifstream data(path, ios::in | ios::binary);
	if(!data.is_open())
	{
		Log::getInstance().error(path + " can't be opened");
		return false;
	}

	char signature[TERRAIN_FILE_SIGNATURE_LENGTH+1] = {'\0'};
	data.read(signature, TERRAIN_FILE_SIGNATURE_LENGTH);
	if(strcmp(signature, TERRAIN_FILE_SIGNATURE) != 0)
	{
		Log::getInstance().error(string("Invalid signature for file ") + path);
		return false;
	}

	float sideLength = 0;
	data.read(reinterpret_cast<char*>(&sideLength), sizeof(float));

	return sideLength;
}

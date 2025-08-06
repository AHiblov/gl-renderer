/* editor_scene_modifier.cpp
 * Makes changes to the Scene structure
 *
 * Author: Artem Hiblov
 */

#include "editor_scene_modifier.h"

#include <random>

#include "utils/chunk_tools.h"
#include "utils/instance_group_tools.h"

using namespace std;
using namespace renderer;
using namespace renderer::data;
using namespace renderer::managers;
using namespace renderer::utils;

namespace
{
	constexpr int TWO_PI_INTEGER = 62831; //62831 = 2 * pi * 10000
}

EditorSceneModifier::EditorSceneModifier():
	scene(nullptr)
{

}

EditorSceneModifier::~EditorSceneModifier()
{
	scene = nullptr;
}

void EditorSceneModifier::setScene(renderer::data::Scene *fscene)
{
	scene = fscene;
}

void EditorSceneModifier::copyBackSelectedInstance(int chunkIndex, int objectIndex, int instanceIndex, const float *selectedInstance)
{
	if(!scene)
		return;

	auto &positionsArrayRef = scene->instances[chunkIndex][objectIndex].positions;

	auto insertionIterator = positionsArrayRef.begin();
	advance(insertionIterator, instanceIndex);
	scene->instances[chunkIndex][objectIndex].positions.insert(insertionIterator, selectedInstance[0]);

	insertionIterator = positionsArrayRef.begin();
	advance(insertionIterator, instanceIndex + 1);
	scene->instances[chunkIndex][objectIndex].positions.insert(insertionIterator, selectedInstance[1]);

	insertionIterator = positionsArrayRef.begin();
	advance(insertionIterator, instanceIndex + 2);
	scene->instances[chunkIndex][objectIndex].positions.insert(insertionIterator, selectedInstance[2]);

	insertionIterator = positionsArrayRef.begin();
	advance(insertionIterator, instanceIndex + 3);
	scene->instances[chunkIndex][objectIndex].positions.insert(insertionIterator, selectedInstance[3]);
}

void EditorSceneModifier::insertNewInstanceIntoScene(int currentChunkIndex, int currentObjectIndex, int instanceIndex, float x, float y, float z, float rotation, const string &objectName,
	const string &shaderFeature)
{
	if(!scene)
		return;

	findChunk(scene->chunks, x, z, currentChunkIndex);
	getObjectIndex(objectName, currentChunkIndex, currentObjectIndex, instanceIndex);

	auto &instancesArrayRef = scene->instances[currentChunkIndex];

	if(instancesArrayRef.size() <= static_cast<size_t>(currentObjectIndex)) //No such object in chunk
	{
		instancesArrayRef.push_back(InstanceArray());
		instancesArrayRef[currentObjectIndex].name = objectName;
		instancesArrayRef[currentObjectIndex].shaderFeature = shaderFeature;
	}

	auto &positionsArrayRef = scene->instances[currentChunkIndex][currentObjectIndex].positions;

	positionsArrayRef.push_back(x);
	positionsArrayRef.push_back(y);
	positionsArrayRef.push_back(z);
	positionsArrayRef.push_back(rotation);
}

void EditorSceneModifier::insertInstanceGroupIntoScene(int chunkIdx, const glm::vec3 &insertionPosition, int radius, TerrainManager *terrainManager, const string &objectName,
	const string &shaderFeature)
{
	if(!scene)
		return;

	const float density = 0.06f;

	mt19937 rng{ random_device()() };
	uniform_int_distribution<> positionDistribution(0, TWO_PI_INTEGER);
	uniform_int_distribution<> rotationDistribution(0, 360);

	int objectIdx = 0, instanceIdx = 0;
	getObjectIndex(objectName, chunkIdx, objectIdx, instanceIdx);

	vector<float> positions = generateInstanceAbsolutePositions(density, insertionPosition, radius, *terrainManager, scene->chunks[chunkIdx], positionDistribution, rng);
	int instanceAmount = positions.size() / 3;

	for(int i = 0; i < instanceAmount; i++)
	{
		float rotationAngle = static_cast<float>(rotationDistribution(rng) % 360);
		insertNewInstanceIntoScene(chunkIdx, objectIdx, instanceIdx, positions[i*3], positions[i*3+1], positions[i*3+2], rotationAngle, objectName, shaderFeature);
	}
}

void EditorSceneModifier::setInitialCameraPosition(const glm::vec3 &cameraPosition, float horizontalRotation, float verticalRotation)
{
	if(!scene)
		return;

	scene->camera.xPos = cameraPosition.x;
	scene->camera.yPos = cameraPosition.y;
	scene->camera.zPos = cameraPosition.z;
	scene->camera.horizontalRotationRadians = horizontalRotation;
	scene->camera.verticalRotationRadians = verticalRotation;
}

void EditorSceneModifier::getObjectIndex(const std::string &name, int chunkIdx, int &objectIdx, int &instanceIdx)
{
	int index = 0;
	for(auto &current: scene->instances[chunkIdx])
	{
		if(current.name == name)
		{
			objectIdx = index;
			instanceIdx = current.positions.size();

			return;
		}

		index++;
	}

	//Nothing found
	objectIdx = index;
	instanceIdx = 0;
}

/* camera_controller.cpp
 * Controls camera position and direction
 *
 * Author: Artem Hiblov
 */

#include "visibility/camera_controller.h"

using namespace renderer::data;
using namespace renderer::managers;
using namespace renderer::utils;
using namespace renderer::visibility;

FlyingCamera::FlyingCamera(float x, float y, float z, float horizontalRot, float verticalRot):
	horizontalRotation(horizontalRot), verticalRotation(verticalRot)
{
	initialize(x, y, z);
}

FlyingCamera::FlyingCamera(const FlyingCamera &other)
{
	cameraPosition = other.cameraPosition;
	horizontalRotation = other.horizontalRotation;
	verticalRotation = other.verticalRotation;
	viewMatrix = other.viewMatrix;
}

FlyingCamera::~FlyingCamera()
{
}

const glm::mat4& FlyingCamera::getViewMatrix() const
{
	return viewMatrix;
}

glm::vec3& FlyingCamera::getPosition()
{
	return cameraPosition;
}

void FlyingCamera::initialize(float x, float y, float z)
{
	cameraPosition = glm::vec3(x, y, z);
}

//-------------------------

FirstPersonCamera::FirstPersonCamera(float x, float y, float z, float horizontalRot, float verticalRot, TerrainManager &terrainMgr, Scene &sceneData):
	terrainManager(terrainMgr), scene(sceneData), horizontalRotation(horizontalRot), verticalRotation(verticalRot)
{
	initialize(x, z);
}

FirstPersonCamera::FirstPersonCamera(const FirstPersonCamera &other):
	terrainManager(other.terrainManager), scene(other.scene)
{
	cameraPosition = other.cameraPosition;
	horizontalRotation = other.horizontalRotation;
	verticalRotation = other.verticalRotation;
	viewMatrix = other.viewMatrix;
}

FirstPersonCamera::~FirstPersonCamera()
{
}

const glm::mat4& FirstPersonCamera::getViewMatrix() const
{
	return viewMatrix;
}

glm::vec3& FirstPersonCamera::getPosition()
{
	return cameraPosition;
}

void FirstPersonCamera::initialize(float x, float z)
{
	float y = 0;

	int chunkIndex = -1;
	findChunk(scene.chunks, x, z, chunkIndex);
	if(chunkIndex == -1)
	{
		y = 0;
	}
	else y = terrainManager.getHeight(scene.chunks[chunkIndex].x, scene.chunks[chunkIndex].z, scene.chunks[chunkIndex].name, x, z);

	cameraPosition = glm::vec3(x, y, z);
}

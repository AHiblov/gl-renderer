/* camera_controller.h
 * Controls camera position and direction
 *
 * Author: Artem Hiblov
 */

#pragma once

#include <memory>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include "data/scene.h"
#include "managers/terrain_manager.h"
#include "utils/chunk_tools.h"

namespace renderer::visibility
{

//Free-fly camera
class FlyingCamera
{
	static constexpr float MATH_HALF_PI_RADIANS = 1.57079f;

public:
	FlyingCamera(float x, float y, float z, float horizontalRot, float verticalRot);
	FlyingCamera(const renderer::visibility::FlyingCamera &other);
	~FlyingCamera();

	/*
	@brief Updates state with inputs
	@param[in] horizontalRotation - rotation around X axis
	@param[in] verticalRotation - rotation around Y axis
	@param[in] forwardDelta - forward movement
	@param[in] rightDelta - moving aside
	*/
	void update(float horizontalRotation, float verticalRotation, float forwardDelta, float rightDelta)
	{
		glm::vec3 direction(cos(verticalRotation) * sin(horizontalRotation), sin(verticalRotation), cos(verticalRotation) * cos(horizontalRotation));
		glm::vec3 right(sin(horizontalRotation - MATH_HALF_PI_RADIANS), 0, cos(horizontalRotation - MATH_HALF_PI_RADIANS));
		glm::vec3 up(glm::cross(right, direction));

		cameraPosition += direction * forwardDelta;
		cameraPosition += right * rightDelta;

		viewMatrix = glm::lookAt(cameraPosition, cameraPosition + direction, up);
	}

	const glm::mat4& getViewMatrix() const;

	glm::vec3& getPosition();

private:
	void initialize(float x, float y, float z);


	glm::vec3 cameraPosition;
	float horizontalRotation;
	float verticalRotation;

	glm::mat4 viewMatrix;
};


//Located at certain height above terrain
class FirstPersonCamera
{
	static constexpr float MATH_HALF_PI_RADIANS = 1.57079f;
	static constexpr float EYES_HEIGHT = 1.8f;

public:
	FirstPersonCamera(float x, float y, float z, float horizontalRot, float verticalRot, renderer::managers::TerrainManager &terrainMgr, renderer::data::Scene &sceneData);
	FirstPersonCamera(const renderer::visibility::FirstPersonCamera &other);
	~FirstPersonCamera();

	/*
	@brief Updates state with inputs
	@param[in] horizontalRotation - rotation around X axis
	@param[in] verticalRotation - rotation around Y axis
	@param[in] forwardDelta - forward movement
	@param[in] rightDelta - moving aside
	*/
	void update(float horizontalRotation, float verticalRotation, float forwardDelta, float rightDelta)
	{
		glm::vec3 direction(cos(verticalRotation) * sin(horizontalRotation), sin(verticalRotation), cos(verticalRotation) * cos(horizontalRotation));
		glm::vec3 right(sin(horizontalRotation - MATH_HALF_PI_RADIANS), 0, cos(horizontalRotation - MATH_HALF_PI_RADIANS));
		glm::vec3 up(glm::cross(right, direction));

		cameraPosition += direction * forwardDelta;
		cameraPosition += right * rightDelta;

		int chunkIndex = -1;
		renderer::utils::findChunk(scene.chunks, cameraPosition.x, cameraPosition.z, chunkIndex);
		if(chunkIndex == -1)
		{
			cameraPosition.y = 0;
		}
		else cameraPosition.y = terrainManager.getHeight(scene.chunks[chunkIndex].x, scene.chunks[chunkIndex].z, scene.chunks[chunkIndex].name, cameraPosition.x, cameraPosition.z) + EYES_HEIGHT;

		viewMatrix = glm::lookAt(cameraPosition, cameraPosition + direction, up);
	}

	const glm::mat4& getViewMatrix() const;

	glm::vec3& getPosition();

private:
	void initialize(float x, float z);


	renderer::managers::TerrainManager &terrainManager;
	renderer::data::Scene &scene;

	glm::vec3 cameraPosition;
	float horizontalRotation;
	float verticalRotation;

	glm::mat4 viewMatrix;
};

using TCameraController = FlyingCamera;

}

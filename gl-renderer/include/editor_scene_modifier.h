/* editor_scene_modifier.h
 * Makes changes to the Scene structure
 *
 * Author: Artem Hiblov
 */

#pragma once

#include <string>

#include <glm/vec3.hpp>

#include "data/scene.h"
#include "managers/terrain_manager.h"

namespace renderer
{

class EditorSceneModifier
{
public:
	EditorSceneModifier();
	~EditorSceneModifier();

	void setScene(renderer::data::Scene *fscene);

	/*
	@brief Copies selected instance data back to array of instances
	*/
	void copyBackSelectedInstance(int chunkIndex, int objectIndex, int instanceIndex, const float *selectedInstance);

	/*
	@brief Copies new instance data to the end of array
	@param[in] currentChunkIndex - chunk to insert in
	@param[in] currentObjectIndex - object to insert
	@param[in] instanceIndex - index in instance array
	@param[in] x - X axis
	@param[in] y - Y axis
	@param[in] z - Z axis
	@param[in] rotation - rotation angle in degrees
	@param[in] objectName - new object name
	@param[in] shaderFeature - shader effect to apply
	*/
	void insertNewInstanceIntoScene(int currentChunkIndex, int currentObjectIndex, int instanceIndex, float x, float y, float z, float rotation, const std::string &objectName,
		const std::string &shaderFeature);

	/*
	@brief Inserts instance group to the end of array
	@param[in] chunkIdx - terrain chunk index
	@param[in] insertionPosition - coordinates of insertion area center
	@param[in] radius - radius of the area for insertion
	@param[in] terrainManager - source for heights
	@param[in] objectName - object to insert
	@param[in] shaderFeature - shader effect to apply
	*/
	void insertInstanceGroupIntoScene(int chunkIdx, const glm::vec3 &insertionPosition, int radius, renderer::managers::TerrainManager *terrainManager, const std::string &objectName,
		const std::string &shaderFeature);

	void setInitialCameraPosition(const glm::vec3 &cameraPosition, float horizontalRotation, float verticalRotation);

private:
	/*
	@brief Figures out indices for insertion into scene
	@param[in] name - object name
	@param[in] chunkIdx - chunk index to find object in
	@param[out] objectIdx - object index
	@param[out] instanceIdx - instance index
	*/
	void getObjectIndex(const std::string &name, int chunkIdx, int &objectIdx, int &instanceIdx);



	renderer::data::Scene *scene; //Non-owning pointer
};

}

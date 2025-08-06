/* editor_tools.h
 * Functions needed for editor
 *
 * Author: Artem Hiblov
 */

#pragma once

#include <string>
#include <vector>

#include <glm/glm.hpp>

#include "data/scene.h"

namespace renderer::utils
{

/*
@brief Finds closest object to the camera by base point
@param[in] cameraPosition - camera position
@param[in] instances - all instances in the scene
@param[out] chunkIndex - index of chunk with the closest instance
@param[out] objectIndex - index of instance group with closest instance
@param[out] instanceIndex - index of the closest object
*/
void findClosestObject(const glm::vec3 &cameraPosition, const std::vector<std::vector<renderer::data::InstanceArray>> &instanceGroups, int &chunkIndex, int &objectIndex, int &instanceIndex);

/*
@brief Saves edited scene to file
*/
void saveScene(const std::string &fileName, renderer::data::Scene &scene);

}

/* math_tools.h
 * Math computations absent in the standard library
 *
 * Author: Artem Hiblov
 */

#pragma once

#include <glm/vec3.hpp>

namespace renderer::utils
{

glm::vec3 slerp(const glm::vec3 &vector1, const glm::vec3 &vector2, float t);

/*
@brief Finds radius for deferred shading point light sphere
*/
float computeBoundingSphereRadius(float lightPower);

}

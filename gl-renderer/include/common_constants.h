/* common_constants.h
 * Defines constants used in multiple files
 *
 * Author: Artem Hiblov
 */

#pragma once

#include <glm/vec3.hpp>

namespace renderer
{

constexpr float POINT_LIGHT_POWER = 50.f;

const glm::vec3 LIGHT_DIFFUSE_NIGHT_COLOUR = glm::vec3(0.f, 0.f, 0.f);

}

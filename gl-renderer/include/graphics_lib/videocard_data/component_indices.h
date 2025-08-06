/* component_indices.h
 * Defines attribute/VBO indices in shader
 *
 * Author: Artem Hiblov
 */

#pragma once

namespace renderer::graphics_lib::videocard_data
{

constexpr int COMPONENT_VERTEX = 0;
constexpr int COMPONENT_UV = 1;
constexpr int COMPONENT_NORMAL = 2;

constexpr int COMPONENT_TANGENT = 3;
constexpr int COMPONENT_BITANGENT = 4;

//Same as tangent/bitangent
constexpr int COMPONENT_INSTANCE_OFFSET = 3;
constexpr int COMPONENT_INSTANCE_ROTATION = 4;

}

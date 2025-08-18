/* light_setters.h
 * Sets directional light colour and direction
 * OpenGL 4.5
 *
 * Author: Artem Hiblov
 */

#pragma once

#include <glm/glm.hpp>

#include "graphics_lib/videocard_data/shader_ids.h"

namespace renderer::graphics_lib
{

void setCustomLightDirection(renderer::graphics_lib::videocard_data::ShaderIds &shaderId, const glm::vec3 &direction);
void setCustomDiffuseLightColour(renderer::graphics_lib::videocard_data::ShaderIds &shaderId, const glm::vec3 &colour);
void setCustomAmbientLightColour(renderer::graphics_lib::videocard_data::ShaderIds &shaderId, const glm::vec3 &colour);

}

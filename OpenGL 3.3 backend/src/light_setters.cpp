/* light_setters.cpp
 * Sets directional light colour and direction
 * OpenGL 3.3
 *
 * Author: Artem Hiblov
 */

#include "graphics_lib/light_setters.h"

#include <GL/glew.h>

using namespace renderer::graphics_lib;
using namespace renderer::graphics_lib::videocard_data;

void renderer::graphics_lib::setCustomLightDirection(ShaderIds &shaderId, const glm::vec3 &direction)
{
	glUniform3fv(shaderId.unifLightDirection, 1, &direction[0]);
}

void renderer::graphics_lib::setCustomDiffuseLightColour(ShaderIds &shaderId, const glm::vec3 &colour)
{
	glUniform3fv(shaderId.unifDiffuseLightColour, 1, &colour[0]);
}

void renderer::graphics_lib::setCustomAmbientLightColour(ShaderIds &shaderId, const glm::vec3 &colour)
{
	glUniform3fv(shaderId.unifAmbientLightColour, 1, &colour[0]);
}

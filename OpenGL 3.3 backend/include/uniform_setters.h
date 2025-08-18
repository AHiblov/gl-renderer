/* uniform_setters.h
 * Functions for setting uniforms to all shaders
 * OpenGL 3.3
 *
 * Author: Artem Hiblov
 */

#pragma once

#include "data/scene.h"
#include "graphics_lib/videocard_data/shader_ids.h"

namespace renderer::graphics_lib
{

void setDirectionalShaderUniforms(renderer::graphics_lib::videocard_data::ShaderIds &shaderId, float screenRatio, const renderer::data::Light &light);
void setPointShaderUniforms(renderer::graphics_lib::videocard_data::ShaderIds &shaderId, float screenRatio, const renderer::data::Light &light);
void setGlitterShaderUniforms(renderer::graphics_lib::videocard_data::ShaderIds &shaderId, float materialAlphaX, float materialAlphaY);
void setFogShaderUniforms(renderer::graphics_lib::videocard_data::ShaderIds &shaderId, const renderer::data::Fog &fog);
void setSkyShaderUniforms(renderer::graphics_lib::videocard_data::ShaderIds &shaderId, float screenRatio);
void setStencilPassShaderUniforms(renderer::graphics_lib::videocard_data::ShaderIds &shaderId, float screenRatio);
void setDeferredDirectionalLightPassShaderUniforms(renderer::graphics_lib::videocard_data::ShaderIds &shaderId, float screenWidth, float screenHeight, const renderer::data::Light &light,
	const renderer::data::Fog &fog);
void setDeferredPointLightPassShaderUniforms(renderer::graphics_lib::videocard_data::ShaderIds &shaderId, float screenWidth, float screenHeight, const renderer::data::Light &light);

}

/* postprocessing_renderer_builder.h
 * Initializes all the shaders for PostprocessingRenderer class
 * OpenGL 4.5
 *
 * Author: Artem Hiblov
 */

#pragma once

#include <memory>

#include "graphics_lib/postprocessing_renderer.h"
#include "graphics_lib/shader_manager.h"

namespace renderer::graphics_lib
{

std::unique_ptr<renderer::graphics_lib::PostprocessingRenderer> buildPostprocessingRenderer(float screenWidth, float screenHeight, const std::string &postprocessingEffect,
	renderer::graphics_lib::ShaderManager *shaderManager);

}

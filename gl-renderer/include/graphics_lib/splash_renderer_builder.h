/* splash_renderer_builder.h
 * Initializes all the shaders for SplashRenderer class
 * OpenGL 4.5
 *
 * Author: Artem Hiblov
 */

#pragma once

#include <memory>

#include "graphics_lib/shader_manager.h"
#include "graphics_lib/splash_renderer.h"
#include "managers/object_manager.h"

namespace renderer::graphics_lib
{

std::unique_ptr<renderer::graphics_lib::SplashRenderer> buildSplashRenderer(renderer::managers::ObjectManager *objectManager, renderer::graphics_lib::ShaderManager *shaderManager, float screenRatio);

}

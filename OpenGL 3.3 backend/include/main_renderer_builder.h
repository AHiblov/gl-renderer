/* main_renderer_builder.h
 * Initializes all the shaders for ForwardRenderer and DeferredRenderer classes
 * OpenGL 3.3
 *
 * Author: Artem Hiblov
 */

#pragma once

#include <map>
#include <memory>
#include <vector>

#include "data/scene.h"
#include "graphics_lib/base_3d_renderer.h"
#include "graphics_lib/shader_manager.h"
#include "managers/object_manager.h"

namespace renderer::graphics_lib
{

std::unique_ptr<renderer::graphics_lib::Base3DRenderer> buildMainRenderer(renderer::managers::ObjectManager *objectManager, renderer::graphics_lib::ShaderManager *shaderManager,
	int screenWidth, int screenHeight, const renderer::data::Light &light, const renderer::data::Fog &fog, const std::map<int, unsigned long long> &shaderFlags,
	std::vector<renderer::graphics_lib::videocard_data::ShaderIds> &shaderIds, bool isDirectional, bool isDeferred);

}

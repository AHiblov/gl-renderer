/* rendering_scene_builder.h
 * Makes scene structure for renderer
 * OpenGL 3.3
 *
 * Author: Artem hiblov
 */

#pragma once

#include <map>

#include "data/chunk_margins.h"
#include "data/scene.h"
#include "graphics_lib/shader_manager.h"
#include "graphics_lib/videocard_data/rendering_scene.h"
#include "managers/object_manager.h"
#include "managers/particle_manager.h"
#include "managers/terrain_manager.h"

namespace renderer::graphics_lib
{

/*
@brief Creates RenderingScene structure and loads data but not shaders if neccessary
*/
renderer::graphics_lib::videocard_data::RenderingScene* makeRenderingScene(const renderer::data::Scene &scene, bool isDeferredRendering, renderer::managers::TerrainManager *terrainManager,
	renderer::managers::ObjectManager *objectManager, renderer::managers::ParticleManager *particleManager, const std::map<int, renderer::data::ChunkMargins> &chunkMargins,
	renderer::graphics_lib::ShaderManager *shaderManager);

/*
@brief Updates opaque and transparent objects withous changing the rest of data
*/
void updateRenderingSceneObjects(const renderer::data::Scene &scene, bool isDeferredRendering, renderer::managers::ObjectManager *objectManager,
	const std::map<int, renderer::data::ChunkMargins> &chunkMargins, renderer::graphics_lib::ShaderManager *shaderManager, renderer::graphics_lib::videocard_data::RenderingScene *renderingScene);
}

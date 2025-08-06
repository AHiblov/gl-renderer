/* particle_manager.h
 * Keeps data for particles
 *
 * Author: Artem Hiblov
 */

#pragma once

#include <memory>
#include <vector>

#include "data/scene.h"
#include "graphics_lib/videocard_data/particle_rendering_data.h"
#include "managers/terrain_manager.h"

namespace renderer::managers
{

class ParticleManager
{
public:
	ParticleManager(renderer::managers::TerrainManager *terrainMgr);
    ~ParticleManager();

    /*
	@brief Returns structure needed for particle rendering
	@param[in] particleSet - particle group parameters
	@param[in] chunk - chunk for insertion
	@param[out] data - structure to be filled
	*/
	bool getRenderingData(const renderer::data::ParticleSet &particleSet, const renderer::data::ChunkData &chunk, renderer::graphics_lib::videocard_data::ParticleRenderingData &data);

	int getTransferedBytesAmount();

private:
	/*
	@brief Initializes particle positions and rotations
	@param[in] particleSet - particle group parameters
	@param[in] chunk - chunk for insertion
	@param[out] data - structure to be filled
	*/
	bool initParticleData(const renderer::data::ParticleSet &particleSet, const renderer::data::ChunkData &chunk, renderer::graphics_lib::videocard_data::ParticleRenderingData &data);

	std::vector<renderer::graphics_lib::videocard_data::ParticleRenderingData> groupIds;
    renderer::managers::TerrainManager *terrainManager;

    int transferedBytes; //How many bytes transfered to videocard
};

}

/* particle_manager.cpp
 * Keeps data for particles
 *
 * Author: Artem Hiblov
 */

#include "managers/particle_manager.h"

#include "log.h"
#include "graphics_lib/operations/particle_operations.h"
#include "utils/instance_group_tools.h"

using namespace std;
using namespace renderer;
using namespace renderer::data;
using namespace renderer::graphics_lib::videocard_data;
using namespace renderer::managers;
using namespace renderer::utils;

namespace
{
	constexpr int TWO_PI_INTEGER = 62831; //62831 = 2 * pi * 10000
}

ParticleManager::ParticleManager(TerrainManager *terrainMgr):
	terrainManager(terrainMgr), transferedBytes(0)
{

}

ParticleManager::~ParticleManager()
{
}

bool ParticleManager::getRenderingData(const ParticleSet &particleSet, const ChunkData &chunk, ParticleRenderingData &data)
{
	bool status = initParticleData(particleSet, chunk, data);
	if(!status)
	{
		Log::getInstance().error(string("Particle \"") + particleSet.name + "\" isn't initialized");
		return false;
	}

	return true;
}

int ParticleManager::getTransferedBytesAmount()
{
    return transferedBytes;
}

bool ParticleManager::initParticleData(const ParticleSet &particleSet, const ChunkData &chunk, ParticleRenderingData &data)
{
    //Generate data

    glm::vec3 areaCenter(particleSet.x, 0, particleSet.z);

    mt19937 rng{ random_device()() };
	uniform_int_distribution<> positionDistribution(0, TWO_PI_INTEGER);
	uniform_int_distribution<> rotationDistribution(0, 360);

    vector<float> positions = generateInstanceRelativePositions(particleSet.density, areaCenter, particleSet.radius, *terrainManager, chunk, positionDistribution, rng);
	int instanceAmount = positions.size() / 3;

	glm::vec3 *arrangement = new glm::vec3 [instanceAmount];
	float *rotation = new float [instanceAmount];

    for(int i = 0; i < instanceAmount; i++)
    {
		glm::vec3 translationVector = glm::vec3(positions[i*3], positions[i*3+1], positions[i*3+2]);
		arrangement[i] = translationVector;

        rotation[i] = glm::radians(static_cast<float>(rotationDistribution(rng) % 360));
    }

    //Submit to videocard
    bool status = graphics_lib::operations::makeParticleGroup(arrangement, rotation, instanceAmount, data);
    if(!status)
    {
        Log::getInstance().error("Can't create particle group");
		return false;
    }

    delete[] rotation;
    delete[] arrangement;

    groupIds.push_back(data);

    transferedBytes += instanceAmount * sizeof(glm::vec3) + instanceAmount * sizeof(float);

    return true;
}

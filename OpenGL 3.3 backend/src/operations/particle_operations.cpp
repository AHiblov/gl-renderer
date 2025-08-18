/* particle_operations.cpp
 * Creates and deletes geometry instancing data
 * OpenGL 3.3
 *
 * Author: Artem Hiblov
 */

#include "graphics_lib/operations/particle_operations.h"

#include <GL/glew.h>

#include "log.h"

using namespace glm;
using namespace renderer::graphics_lib::videocard_data;

namespace
{
	/*
	@brief Makes VBO and transfers data to it
	@param[in] data - data to transfer
	@param[in] matrixAmount - number of matrices
	@return VBO ID
	*/
	unsigned int makeVBO(const glm::vec3 *data, int matrixAmount);
	unsigned int makeVBO(const float *data, int amount);
}

bool renderer::graphics_lib::operations::makeParticleGroup(const glm::vec3 *arrangement, const float *rotation, int recordAmount, ParticleRenderingData &groupIds)
{
    if(!arrangement || !rotation || !recordAmount)
    {
        Log::getInstance().error("Not enough data to create particle group");
        return false;
    }
    if(groupIds.objectData.vaoId == -1u)
    {
        Log::getInstance().error("Object for particle group is not created");
        return false;
    }

    glBindVertexArray(groupIds.objectData.vaoId);

    unsigned int arrangementVboId = makeVBO(arrangement, recordAmount);
    unsigned int rotationVboId = makeVBO(rotation, recordAmount);

    groupIds.arrangementBufferId = arrangementVboId;
    groupIds.rotationBufferId = rotationVboId;

    groupIds.particleAmount = recordAmount;

    return true;
}

void renderer::graphics_lib::operations::deleteParticleGroup(const ParticleRenderingData &groupIds)
{
	glDeleteBuffers(1, &groupIds.arrangementBufferId);
	glDeleteBuffers(1, &groupIds.rotationBufferId);
}

namespace
{
    unsigned int makeVBO(const glm::vec3 *data, int matrixAmount)
    {
        unsigned int vboId = -1u;

        glGenBuffers(1, &vboId);

        glBindBuffer(GL_ARRAY_BUFFER, vboId);
		glBufferData(GL_ARRAY_BUFFER, matrixAmount * sizeof(glm::vec3), data, GL_STATIC_DRAW);

        return vboId;
    }

	unsigned int makeVBO(const float *data, int amount)
	{
	    unsigned int vboId = -1u;

        glGenBuffers(1, &vboId);

        glBindBuffer(GL_ARRAY_BUFFER, vboId);
		glBufferData(GL_ARRAY_BUFFER, amount * sizeof(float), data, GL_STATIC_DRAW);

        return vboId;
	}
}

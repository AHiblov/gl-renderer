/* particle_operations.cpp
 * Creates and deletes geometry instancing data
 * OpenGL 4.5
 *
 * Author: Artem Hiblov
 */

#include "graphics_lib/operations/particle_operations.h"

#include <GL/glew.h>

#include "log.h"
#include "graphics_lib/videocard_data/component_indices.h"

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

	glVertexArrayVertexBuffer(groupIds.objectData.vaoId, COMPONENT_INSTANCE_OFFSET, arrangementVboId, 0, 3 * sizeof(float));
	glVertexArrayVertexBuffer(groupIds.objectData.vaoId, COMPONENT_INSTANCE_ROTATION, rotationVboId, 0, sizeof(float));

	glEnableVertexArrayAttrib(groupIds.objectData.vaoId, COMPONENT_INSTANCE_OFFSET);
	glVertexArrayAttribFormat(groupIds.objectData.vaoId, COMPONENT_INSTANCE_OFFSET, 3, GL_FLOAT, GL_FALSE, 0);
	glVertexArrayAttribBinding(groupIds.objectData.vaoId, COMPONENT_INSTANCE_OFFSET, COMPONENT_INSTANCE_OFFSET);
	glVertexArrayBindingDivisor(groupIds.objectData.vaoId, COMPONENT_INSTANCE_OFFSET, 1);

	glEnableVertexArrayAttrib(groupIds.objectData.vaoId, COMPONENT_INSTANCE_ROTATION);
	glVertexArrayAttribFormat(groupIds.objectData.vaoId, COMPONENT_INSTANCE_ROTATION, 1, GL_FLOAT, GL_FALSE, 0);
	glVertexArrayAttribBinding(groupIds.objectData.vaoId, COMPONENT_INSTANCE_ROTATION, COMPONENT_INSTANCE_ROTATION);
	glVertexArrayBindingDivisor(groupIds.objectData.vaoId, COMPONENT_INSTANCE_ROTATION, 1);

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

        glCreateBuffers(1, &vboId);
        glNamedBufferStorage(vboId, matrixAmount * sizeof(glm::vec3), data, 0);

        return vboId;
    }

	unsigned int makeVBO(const float *data, int amount)
	{
	    unsigned int vboId = -1u;

	    glCreateBuffers(1, &vboId);
        glNamedBufferStorage(vboId, amount * sizeof(float), data, 0);

        return vboId;
	}
}

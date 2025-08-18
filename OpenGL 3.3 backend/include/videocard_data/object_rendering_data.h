/* object_rendering_data.h
 * Keeps data for frame renderer
 * OpenGL 3.3
 *
 * Author: Artem Hiblov
 */

#pragma once

namespace renderer::graphics_lib::videocard_data
{

struct ObjectRenderingData
{
	unsigned int vaoId = -1u;
	unsigned int vertexBufferId = -1u;
	unsigned int uvBufferId = -1u;
	unsigned int normalBufferId = -1u;
	unsigned int tangentBufferId = -1u;
	unsigned int bitangentBufferId = -1u;

	int vertexAmount = 0;

	unsigned int textureId = -1u;
	unsigned int normalTextureId = -1u;
};

}
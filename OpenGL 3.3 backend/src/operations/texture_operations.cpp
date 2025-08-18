/* texture_operations.cpp
 * Creates and deletes texture
 * OpenGL 3.3
 *
 * Author: Artem Hiblov
 */

#include "graphics_lib/operations/texture_operations.h"

#include <GL/glew.h>

#include "log.h"

using namespace renderer::data;
using namespace renderer::graphics_lib;
using namespace renderer::graphics_lib::operations;
using namespace renderer::graphics_lib::videocard_data;

namespace
{
	constexpr int TEXTURE_RGB = 3;
}

bool renderer::graphics_lib::operations::makeTexture(const Texture &texture, unsigned int &textureId)
{
	unsigned int newTextureId = -1u;
	glGenTextures(1, &newTextureId);

	glBindTexture(GL_TEXTURE_2D, newTextureId);

	glTexImage2D(GL_TEXTURE_2D, 0, (texture.bytesPerPixel == TEXTURE_RGB) ? GL_RGB: GL_RGBA,
				texture.width, texture.height, 0, (texture.bytesPerPixel == TEXTURE_RGB) ? GL_BGR: GL_BGRA, GL_UNSIGNED_BYTE, texture.data.get());
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	glGenerateMipmap(GL_TEXTURE_2D);

	textureId = newTextureId;
	return true;
}

void renderer::graphics_lib::operations::deleteTexture(unsigned int textureId)
{
	glDeleteTextures(1, &textureId);
}

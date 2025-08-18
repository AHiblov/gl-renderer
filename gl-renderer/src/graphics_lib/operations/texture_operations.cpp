/* texture_operations.cpp
 * Creates and deletes texture
 * OpenGL 4.5
 *
 * Author: Artem Hiblov
 */

#include "graphics_lib/operations/texture_operations.h"

#include <algorithm>
#include <cmath>

#include <GL/glew.h>

#include "log.h"

using namespace std;
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
	glCreateTextures(GL_TEXTURE_2D, 1, &newTextureId);

	glTextureParameteri(newTextureId, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTextureParameteri(newTextureId, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTextureParameteri(newTextureId, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTextureParameteri(newTextureId, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);

	int minDimension = min<>(texture.width, texture.height);
	int levels = log2(minDimension);

	glTextureStorage2D(newTextureId, levels, (texture.bytesPerPixel == TEXTURE_RGB) ? GL_RGB8: GL_RGBA8, texture.width, texture.height);
	glTextureSubImage2D(newTextureId, 0, 0, 0, texture.width, texture.height, (texture.bytesPerPixel == TEXTURE_RGB) ? GL_BGR: GL_BGRA, GL_UNSIGNED_BYTE, texture.data.get());

	glGenerateTextureMipmap(newTextureId);

	textureId = newTextureId;
	return true;
}

void renderer::graphics_lib::operations::deleteTexture(unsigned int textureId)
{
	glDeleteTextures(1, &textureId);
}

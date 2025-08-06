/* texture.h
 * Contains all texture data
 *
 * Author: Artem Hiblov
 */

#pragma once

#include <memory>

namespace renderer::data
{

struct Texture
{
	Texture():
		width(0), height(0), bytesPerPixel(0)
	{}

	Texture(std::shared_ptr<unsigned char[]> textureData, int textureWidth, int textureHeight, int textureBytesPerPixel):
		width(textureWidth), height(textureHeight), bytesPerPixel(textureBytesPerPixel)
	{
		data = std::move(textureData);
	}

	Texture(const Texture &other)
	{
		data = other.data;
		width = other.width;
		height = other.height;
		bytesPerPixel = other.bytesPerPixel;
	}

	std::shared_ptr<unsigned char[]> data;
	int width;
	int height;
	int bytesPerPixel;
};

}
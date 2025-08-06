/* texture_loader.cpp
 * Loads texture into memory
 *
 * Author: Artem Hiblov
 */

#include "loaders/texture_loader.h"

#include <fstream>
#include <memory>

#include "log.h"

using namespace std;
using namespace renderer::data;
using namespace renderer::loaders;

namespace
{
	constexpr int BMP_HEADER_SIZE = 54;
	constexpr int BMP_DATA_OFFSET = 10;
	constexpr int BMP_IMAGE_WIDTH_OFFSET = 18;
	constexpr int BMP_IMAGE_HEIGHT_OFFSET = 22;
	constexpr int BMP_IMAGE_BITS_PER_PIXEL_OFFSET = 28;
	constexpr int BMP_IMAGE_SIZE_OFFSET = 34;
}

bool renderer::loaders::loadTexture(const string &path, Texture &texture)
{
	if(path.empty())
	{
		Log::getInstance().error("The path to texture is empty");
		return false;
	}

	ifstream data(path, ios::in | ios::binary);
	if(!data.is_open())
	{
		Log::getInstance().error(string("Can't open ") + path);
		return false;
	}

	unsigned char header[BMP_HEADER_SIZE] = {0};

	data.read(reinterpret_cast<char*>(header), BMP_HEADER_SIZE);
	if(header[0] != 'B' || header[1] != 'M')
	{
		Log::getInstance().error(string("Signature for ") + path + " is invalid.");
		return false;
	}

	int dataOffset = *(reinterpret_cast<int*>(&header[BMP_DATA_OFFSET]));
	texture.width = *(reinterpret_cast<int*>(&header[BMP_IMAGE_WIDTH_OFFSET]));
	texture.height = *(reinterpret_cast<int*>(&header[BMP_IMAGE_HEIGHT_OFFSET]));
	short bitsPerPixel = *(reinterpret_cast<short*>(&header[BMP_IMAGE_BITS_PER_PIXEL_OFFSET]));
	texture.bytesPerPixel = bitsPerPixel / 8;

	int textureSize = *(reinterpret_cast<int*>(&header[BMP_IMAGE_SIZE_OFFSET]));
	if(textureSize == 0)
		textureSize = texture.width * texture.height * texture.bytesPerPixel;

	shared_ptr<unsigned char[]> image = shared_ptr<unsigned char[]>(new unsigned char [textureSize]);
	data.seekg(dataOffset);
	data.read(reinterpret_cast<char*>(image.get()), textureSize);

	texture.data = move(image);

	data.close();
	return true;
}

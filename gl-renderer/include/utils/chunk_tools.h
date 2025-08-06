/* chunk_tools.h
 * Chunk-related functionality
 *
 * Author: Artem Hiblov
 */

#pragma once

#include <vector>

#include "data/scene.h"

namespace renderer::utils
{

/*
@brief Finds chunk for which coordinates are inside
*/
void findChunk(std::vector<renderer::data::ChunkData> &chunks, float x, float z, int &chunkIndex);

}

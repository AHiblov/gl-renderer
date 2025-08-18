/* chunk_tools.cpp
 * Chunk-related functionality
 *
 * Author: Artem Hiblov
 */

#include "utils/chunk_tools.h"

using namespace std;
using namespace renderer::data;
using namespace renderer::utils;

void renderer::utils::findChunk(vector<ChunkData> &chunks, float x, float z, int &chunkIndex)
{
	int chunkAmount = chunks.size();
	for(int i = 0; i < chunkAmount; i++)
	{
		ChunkData &curChunk = chunks[i];
		if((curChunk.x <= x) && (x <= curChunk.x + curChunk.size) &&
			(curChunk.z - curChunk.size <= z) && (z <= curChunk.z))
		{
			chunkIndex = i;
			return;
		}
	}

	chunkIndex = -1;
}

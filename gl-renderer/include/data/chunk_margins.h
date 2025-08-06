/* chunk_margins.h
 * Chunk margins and subdivision
 *
 * Author: Artem Hiblov
 */

#pragma once

namespace renderer::data
{

struct ChunkMargins
{
	float nearZ = 0;
	float farZ = 0;
	float leftX = 0;
	float rightX = 0;
	float centerX = 0;
	float centerZ = 0;
};

}

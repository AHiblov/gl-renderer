/* region_visibility_calculation.cpp
 * Handles if certain chunk with all its objects is visible
 *
 * Author: Artem Hiblov
 */

#include "visibility/region_visibility_calculation.h"

using namespace std;
using namespace renderer::data;
using namespace renderer::visibility;

namespace
{
	constexpr float CHUNK_VISIBILITY_DISTANCE = 60.f;
	constexpr float QUAD_VISIBILITY_DISTANCE = 60.f;
}

void renderer::visibility::recalculateVisibility(map<int, ChunkMargins> *margins, float x, float z, vector<VisibilityFlags> &visibilityFlags)
{
	const int amount = margins->size();
	for(int i = 0; i < amount; i++)
	{
		ChunkMargins &current = (*margins)[i];

		//Cnunk visibility
		if((x > (current.leftX - CHUNK_VISIBILITY_DISTANCE)) && (x < (current.rightX + CHUNK_VISIBILITY_DISTANCE)) &&
			(z < (current.nearZ + CHUNK_VISIBILITY_DISTANCE)) && (z > (current.farZ - CHUNK_VISIBILITY_DISTANCE)))
			visibilityFlags[i].isChunkVisible = true;
		else visibilityFlags[i].isChunkVisible = false;

		//Chunk quads vilibility

		int8_t quadFlags = 0;
		if((x > (current.leftX - QUAD_VISIBILITY_DISTANCE)) && (x < (current.centerX + QUAD_VISIBILITY_DISTANCE)) &&
			(z < (current.nearZ + QUAD_VISIBILITY_DISTANCE)) && (z > (current.centerZ - QUAD_VISIBILITY_DISTANCE)))
			quadFlags |= VisibilityFlags::QUAD1_FLAG;
		if((x > (current.centerX - QUAD_VISIBILITY_DISTANCE)) && (x < (current.rightX + QUAD_VISIBILITY_DISTANCE)) &&
			(z < (current.nearZ + QUAD_VISIBILITY_DISTANCE)) && (z > (current.centerZ - QUAD_VISIBILITY_DISTANCE)))
			quadFlags |= VisibilityFlags::QUAD2_FLAG;
		if((x > (current.leftX - QUAD_VISIBILITY_DISTANCE)) && (x < (current.centerX + QUAD_VISIBILITY_DISTANCE)) &&
			(z < (current.centerZ + QUAD_VISIBILITY_DISTANCE)) && (z > (current.farZ - QUAD_VISIBILITY_DISTANCE)))
			quadFlags |= VisibilityFlags::QUAD3_FLAG;
		if((x > (current.centerX - QUAD_VISIBILITY_DISTANCE)) && (x < (current.rightX + QUAD_VISIBILITY_DISTANCE)) &&
			(z < (current.centerZ + QUAD_VISIBILITY_DISTANCE)) && (z > (current.farZ - QUAD_VISIBILITY_DISTANCE)))
			quadFlags |= VisibilityFlags::QUAD4_FLAG;

		visibilityFlags[i].quadVisibility = quadFlags;
	}
}

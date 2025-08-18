/* visibility_flags.h
 * Defines on-screen visibility of chunks and their parts
 *
 * Author: Artem Hiblov
 */

#pragma once

#include <cstdint>

namespace renderer::data
{

struct VisibilityFlags
{
	bool isChunkVisible = false;
	int8_t quadVisibility = 0;

	static constexpr int8_t QUAD1_FLAG = 0x1;
	static constexpr int8_t QUAD2_FLAG = 0x2;
	static constexpr int8_t QUAD3_FLAG = 0x4;
	static constexpr int8_t QUAD4_FLAG = 0x8;
};

}

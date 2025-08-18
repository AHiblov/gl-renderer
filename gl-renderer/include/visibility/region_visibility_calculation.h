/* region_visibility_calculation.h
 * Handles if certain chunk with all its objects is visible
 *
 * Author: Artem Hiblov
 */

#pragma once

#include <map>
#include <vector>

#include "data/chunk_margins.h"
#include "data/visibility_flags.h"

namespace renderer::visibility
{

void recalculateVisibility(std::map<int, renderer::data::ChunkMargins> *margins, float x, float z, std::vector<renderer::data::VisibilityFlags> &visibilityFlags);

}

/**
 * @file floatinginfobox.hpp
 *
 * Adds floating info box QoL feature
 */
#pragma once

#include "engine.h"
#include "engine/point.hpp"
#include "engine/surface.hpp"
#include "items.h"

namespace devilution {

enum modified_item_val : int16_t {
	MIV_MINDAM,
	MIV_MAXDAM,
	MIV_AC,
};

void DrawFloatingInfoBox(const Surface &out, Point position);
void PrintFloatingItemDetails(const Item &item);
void PrintFloatingItemDur(const Item &item);

} // namespace devilution

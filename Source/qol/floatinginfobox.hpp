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

enum modified_item_val : uint8_t {
	MIV_MINDAM,
	MIV_MAXDAM,
	MIV_AC,
	MIV_DUR,
};

void DrawFloatingInfoBox(const Surface &out, Point position);

} // namespace devilution

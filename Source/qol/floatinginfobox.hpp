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

void DrawFloatingItemInfoBox(const Surface &out, Point position);

} // namespace devilution

/**
 * @file floatingnumbers.h
 *
 * Adds floating numbers QoL feature
 */
#pragma once

#include "engine.h"
#include "engine/point.hpp"

namespace devilution {

enum class FloatingType : uint8_t {
	Experience,
	DamagePhysical,
	DamageFire,
	DamageLightning,
	DamageMagic,
};

void AddFloatingNumber(Point pos, FloatingType type, int value);
void DrawFloatingNumbers(const Surface &out);
void UpdateFloatingNumbersCoordsMap(Point dungeon, Point screen);

} // namespace devilution

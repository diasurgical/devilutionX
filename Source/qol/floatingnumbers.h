/**
 * @file floatingnumbers.h
 *
 * Adds floating numbers QoL feature
 */
#pragma once

#include "engine.h"
#include "engine/point.hpp"
#include "misdat.h"

namespace devilution {

enum class FloatingType : uint8_t {
	Experience,
	DamagePhysical,
	DamageFire,
	DamageLightning,
	DamageMagic,
	DamageAcid,
};

void AddFloatingNumber(bool isMyPlayer, Point pos, FloatingType type, int value);
void DrawFloatingNumbers(const Surface &out);
void UpdateFloatingNumbersCoordsMap(Point dungeon, Point screen);
void ClearFloatingNumbersCoordsMap();
FloatingType GetFloatingNumberTypeFromMissile(missile_resistance mir);

} // namespace devilution

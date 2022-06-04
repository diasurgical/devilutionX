/**
 * @file floatingnumbers.h
 *
 * Adds floating numbers QoL feature
 */
#pragma once

#include "DiabloUI/ui_flags.hpp"
#include "engine.h"
#include "engine/point.hpp"
#include "misdat.h"

namespace devilution {

enum class FloatingType : uint8_t {
	None,
	Experience,
	DamagePhysical,
	DamageFire,
	DamageLightning,
	DamageMagic,
	DamageAcid,
};

void AddFloatingNumber(bool isMyPlayer, Point pos, FloatingType type, int value, int index, UiFlags style = UiFlags::None, bool damageToPlayer = false);
void DrawFloatingNumbers(const Surface &out);
void UpdateFloatingNumbersCoordsMap(Point dungeon, Point screen);
void ClearFloatingNumbersCoordsMap();
void ClearFloatingNumbers();
FloatingType GetFloatingNumberTypeFromMissile(missile_resistance mir);

} // namespace devilution

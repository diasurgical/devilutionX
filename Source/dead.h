/**
 * @file dead.h
 *
 * Interface of functions for placing dead monsters.
 */
#pragma once

#include <array>
#include <cstdint>

#include "engine/point.hpp"
#include "engine.h"

namespace devilution {

static constexpr unsigned MaxDead = 31;

struct DeadStruct {
	std::array<const byte *, 8> data;
	int frame;
	int width;
	uint8_t translationPaletteIndex;
};

extern DeadStruct Dead[MaxDead];
extern int8_t stonendx;

void InitDead();
void AddDead(Point tilePosition, int8_t dv, Direction ddir);
void SyncUniqDead();

} // namespace devilution

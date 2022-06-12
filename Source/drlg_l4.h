/**
 * @file drlg_l4.h
 *
 * Interface of the hell level generation algorithms.
 */
#pragma once

#include "gendung.h"

namespace devilution {

extern Point DiabloQuad1;
extern Point DiabloQuad2;
extern Point DiabloQuad3;
extern Point DiabloQuad4;
void CreateL4Dungeon(uint32_t rseed, lvl_entry entry);
void LoadL4Dungeon(const char *path, int vx, int vy);
void LoadPreL4Dungeon(const char *path);

} // namespace devilution

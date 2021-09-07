/**
 * @file drlg_l6.h
 *
 * Interface of the nest level generation algorithms.
 */
#pragma once

#include "gendung.h"

namespace devilution {

void CreateL6Dungeon(uint32_t rseed, lvl_entry entry);
void LoadL6Dungeon(const char *sFileName, int vx, int vy);
void LoadPreL6Dungeon(const char *sFileName);

} // namespace devilution

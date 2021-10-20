/**
 * @file drlg_l3.h
 *
 * Interface of the caves level generation algorithms.
 */
#pragma once

#include "gendung.h"

namespace devilution {

void CreateL3Dungeon(uint32_t rseed, lvl_entry entry);
void LoadL3Dungeon(const char *sFileName, int vx, int vy);
void LoadPreL3Dungeon(const char *sFileName);

} // namespace devilution

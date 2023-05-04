/**
 * @file levels/drlg_l3.h
 *
 * Interface of the caves level generation algorithms.
 */
#pragma once

#include "levels/gendung.h"

namespace devilution {

void CreateL3Dungeon(uint32_t rseed, lvl_entry entry);
void LoadPreL3Dungeon(const char *sFileName);
void LoadL3Dungeon(const char *sFileName, Point spawn);
;

} // namespace devilution

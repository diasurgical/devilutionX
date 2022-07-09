/**
 * @file levels/drlg_l1.h
 *
 * Interface of the cathedral level generation algorithms.
 */
#pragma once

#include "levels/gendung.h"

namespace devilution {

void PlaceMiniSetRandom(const Miniset &miniset, int rndper);
Point SelectChamber();
void CreateL5Dungeon(uint32_t rseed, lvl_entry entry);
void LoadPreL1Dungeon(const char *path);
void LoadL1Dungeon(const char *path, Point spawn);

} // namespace devilution

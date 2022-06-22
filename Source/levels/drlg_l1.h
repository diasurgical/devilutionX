/**
 * @file levels/drlg_l1.h
 *
 * Interface of the cathedral level generation algorithms.
 */
#pragma once

#include "levels/gendung.h"

namespace devilution {

extern int UberRow;
extern int UberCol;
extern bool IsUberRoomOpened;
extern bool IsUberLeverActivated;
extern int UberDiabloMonsterIndex;

void CreateL5Dungeon(uint32_t rseed, lvl_entry entry);
void LoadPreL1Dungeon(const char *path);
void LoadL1Dungeon(const char *path, Point spawn);

} // namespace devilution

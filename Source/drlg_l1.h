/**
 * @file drlg_l1.h
 *
 * Interface of the cathedral level generation algorithms.
 */
#pragma once

#include "gendung.h"

namespace devilution {

extern int UberRow;
extern int UberCol;
extern bool IsUberRoomOpened;
extern bool IsUberLeverActivated;
extern int UberDiabloMonsterIndex;

void LoadL1Dungeon(const char *path, int vx, int vy);
void LoadPreL1Dungeon(const char *path);
void CreateL5Dungeon(uint32_t rseed, lvl_entry entry);

} // namespace devilution

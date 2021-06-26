/**
 * @file drlg_l2.h
 *
 * Interface of the catacombs level generation algorithms.
 */
#pragma once

#include "gendung.h"

namespace devilution {

struct HALLNODE {
	int nHallx1;
	int nHally1;
	int nHallx2;
	int nHally2;
	int nHalldir;
};

struct ROOMNODE {
	int nRoomx1;
	int nRoomy1;
	int nRoomx2;
	int nRoomy2;
	int nRoomDest;
};

extern BYTE predungeon[DMAXX][DMAXY];

void InitDungeon();
void LoadL2Dungeon(const char *path, int vx, int vy);
void LoadPreL2Dungeon(const char *path);
void CreateL2Dungeon(uint32_t rseed, lvl_entry entry);

} // namespace devilution

/**
 * @file drlg_l2.h
 *
 * Interface of the catacombs level generation algorithms.
 */
#pragma once

#include "gendung.h"
#include "miniwin/miniwin.h"

namespace devilution {

struct HALLNODE {
	int nHallx1;
	int nHally1;
	int nHallx2;
	int nHally2;
	int nHalldir;
	struct HALLNODE *pNext;
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
void LoadL2Dungeon(const char *sFileName, int vx, int vy);
void LoadPreL2Dungeon(const char *sFileName);
void CreateL2Dungeon(DWORD rseed, lvl_entry entry);

} // namespace devilution

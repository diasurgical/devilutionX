/**
 * @file drlg_l2.h
 *
 * Interface of the catacombs level generation algorithms.
 */
#pragma once

namespace devilution {

struct HALLNODE {
	Sint32 nHallx1;
	Sint32 nHally1;
	Sint32 nHallx2;
	Sint32 nHally2;
	Sint32 nHalldir;
	struct HALLNODE *pNext;
};

struct ROOMNODE {
	Sint32 nRoomx1;
	Sint32 nRoomy1;
	Sint32 nRoomx2;
	Sint32 nRoomy2;
	Sint32 nRoomDest;
};

extern BYTE predungeon[DMAXX][DMAXY];

void InitDungeon();
void LoadL2Dungeon(const char *sFileName, int vx, int vy);
void LoadPreL2Dungeon(const char *sFileName, int vx, int vy);
void CreateL2Dungeon(DWORD rseed, lvl_entry entry);

}

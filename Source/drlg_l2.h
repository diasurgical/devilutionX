/**
 * @file drlg_l2.h
 *
 * Interface of the catacombs level generation algorithms.
 */
#pragma once

namespace devilution {

#ifdef __cplusplus
extern "C" {
#endif

typedef struct HALLNODE {
	Sint32 nHallx1;
	Sint32 nHally1;
	Sint32 nHallx2;
	Sint32 nHally2;
	Sint32 nHalldir;
	struct HALLNODE *pNext;
} HALLNODE;

typedef struct ROOMNODE {
	Sint32 nRoomx1;
	Sint32 nRoomy1;
	Sint32 nRoomx2;
	Sint32 nRoomy2;
	Sint32 nRoomDest;
} ROOMNODE;

extern BYTE predungeon[DMAXX][DMAXY];

void InitDungeon();
void LoadL2Dungeon(const char *sFileName, int vx, int vy);
void LoadPreL2Dungeon(const char *sFileName, int vx, int vy);
void CreateL2Dungeon(DWORD rseed, int entry);

#ifdef __cplusplus
}
#endif

}

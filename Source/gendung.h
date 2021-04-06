/**
 * @file gendung.h
 *
 * Interface of general dungeon generation code.
 */
#pragma once

namespace devilution {

#ifdef __cplusplus
extern "C" {
#endif

typedef struct ScrollStruct {
	/** @brief X-offset of camera position. This usually corresponds to a negative version of plr[myplr]._pxoff */
	Sint32 _sxoff;
	/** @brief Y-offset of camera position. This usually corresponds to a negative version of plr[myplr]._pyoff */
	Sint32 _syoff;
	Sint32 _sdx;
	Sint32 _sdy;
	Sint32 _sdir;
} ScrollStruct;

typedef struct THEME_LOC {
	Sint32 x;
	Sint32 y;
	Sint32 ttval;
	Sint32 width;
	Sint32 height;
} THEME_LOC;

typedef struct MICROS {
	Uint16 mt[16];
} MICROS;

typedef struct ShadowStruct {
	Uint8 strig;
	Uint8 s1;
	Uint8 s2;
	Uint8 s3;
	Uint8 nv1;
	Uint8 nv2;
	Uint8 nv3;
} ShadowStruct;

extern BYTE dungeon[DMAXX][DMAXY];
extern BYTE pdungeon[DMAXX][DMAXY];
extern char dflags[DMAXX][DMAXY];
extern int setpc_x;
extern int setpc_y;
extern int setpc_w;
extern int setpc_h;
extern BYTE *pSetPiece;
extern BOOL setloadflag;
extern BYTE *pSpecialCels;
extern BYTE *pMegaTiles;
extern BYTE *pLevelPieces;
extern BYTE *pDungeonCels;
extern char block_lvid[MAXTILES + 1];
extern BOOLEAN nBlockTable[MAXTILES + 1];
extern BOOLEAN nSolidTable[MAXTILES + 1];
extern BOOLEAN nTransTable[MAXTILES + 1];
extern BOOLEAN nMissileTable[MAXTILES + 1];
extern BOOLEAN nTrapTable[MAXTILES + 1];
extern int dminx;
extern int dminy;
extern int dmaxx;
extern int dmaxy;
extern int gnDifficulty;
extern dungeon_type leveltype;
extern BYTE currlevel;
extern BOOLEAN setlevel;
extern BYTE setlvlnum;
extern dungeon_type setlvltype;
extern int ViewX;
extern int ViewY;
extern int ViewBX;
extern int ViewBY;
extern int ViewDX;
extern int ViewDY;
extern ScrollStruct ScrollInfo;
extern int LvlViewX;
extern int LvlViewY;
extern int MicroTileLen;
extern char TransVal;
extern BOOLEAN TransList[256];
extern int dPiece[MAXDUNX][MAXDUNY];
extern MICROS dpiece_defs_map_2[MAXDUNX][MAXDUNY];
extern char dTransVal[MAXDUNX][MAXDUNY];
extern char dLight[MAXDUNX][MAXDUNY];
extern char dPreLight[MAXDUNX][MAXDUNY];
extern char dFlags[MAXDUNX][MAXDUNY];
extern char dPlayer[MAXDUNX][MAXDUNY];
extern int dMonster[MAXDUNX][MAXDUNY];
extern char dDead[MAXDUNX][MAXDUNY];
extern char dObject[MAXDUNX][MAXDUNY];
extern char dItem[MAXDUNX][MAXDUNY];
extern char dMissile[MAXDUNX][MAXDUNY];
extern char dSpecial[MAXDUNX][MAXDUNY];
extern int themeCount;
extern THEME_LOC themeLoc[MAXTHEMES];

void FillSolidBlockTbls();
void SetDungeonMicros();
void DRLG_InitTrans();
void DRLG_MRectTrans(int x1, int y1, int x2, int y2);
void DRLG_RectTrans(int x1, int y1, int x2, int y2);
void DRLG_CopyTrans(int sx, int sy, int dx, int dy);
void DRLG_ListTrans(int num, BYTE *List);
void DRLG_AreaTrans(int num, BYTE *List);
void DRLG_InitSetPC();
void DRLG_SetPC();
void Make_SetPC(int x, int y, int w, int h);
void DRLG_PlaceThemeRooms(int minSize, int maxSize, int floor, int freq, int rndSize);
void DRLG_HoldThemeRooms();
BOOL SkipThemeRoom(int x, int y);
void InitLevels();

#ifdef __cplusplus
}
#endif

}

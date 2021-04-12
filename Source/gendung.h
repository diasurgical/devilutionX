/**
 * @file gendung.h
 *
 * Interface of general dungeon generation code.
 */
#pragma once

#include <stdint.h>

#include "scrollrt.h"

namespace devilution {

enum _setlevels : int8_t {
	SL_NONE,
	SL_SKELKING,
	SL_BONECHAMB,
	SL_MAZE,
	SL_POISONWATER,
	SL_VILEBETRAYER,
};

enum dungeon_type : int8_t {
	DTYPE_TOWN,
	DTYPE_CATHEDRAL,
	DTYPE_CATACOMBS,
	DTYPE_CAVES,
	DTYPE_HELL,
	DTYPE_NEST,
	DTYPE_CRYPT,
	DTYPE_NONE = -1,
};

enum lvl_entry : uint8_t {
	ENTRY_MAIN,
	ENTRY_PREV,
	ENTRY_SETLVL,
	ENTRY_RTNLVL,
	ENTRY_LOAD,
	ENTRY_WARPLVL,
	ENTRY_TWARPDN,
	ENTRY_TWARPUP,
};

enum {
	// clang-format off
	DLRG_HDOOR     = 1 << 0,
	DLRG_VDOOR     = 1 << 1,
	DLRG_CHAMBER   = 1 << 6,
	DLRG_PROTECTED = 1 << 7,
	// clang-format on
};

enum {
	// clang-format off
	BFLAG_MISSILE     = 1 << 0,
	BFLAG_VISIBLE     = 1 << 1,
	BFLAG_DEAD_PLAYER = 1 << 2,
	BFLAG_POPULATED   = 1 << 3,
	BFLAG_MONSTLR     = 1 << 4,
	BFLAG_PLAYERLR    = 1 << 5,
	BFLAG_LIT         = 1 << 6,
	BFLAG_EXPLORED    = 1 << 7,
	// clang-format on
};

enum _difficulty : uint8_t {
	DIFF_NORMAL,
	DIFF_NIGHTMARE,
	DIFF_HELL,
	NUM_DIFFICULTIES,
};

struct ScrollStruct {
	/** @brief X-offset of camera position. This usually corresponds to a negative version of plr[myplr]._pxoff */
	Sint32 _sxoff;
	/** @brief Y-offset of camera position. This usually corresponds to a negative version of plr[myplr]._pyoff */
	Sint32 _syoff;
	Sint32 _sdx;
	Sint32 _sdy;
	_scroll_direction _sdir;
};

struct THEME_LOC {
	Sint16 x;
	Sint16 y;
	Sint16 ttval;
	Sint16 width;
	Sint16 height;
};

struct MICROS {
	Uint16 mt[16];
};

struct ShadowStruct {
	Uint8 strig;
	Uint8 s1;
	Uint8 s2;
	Uint8 s3;
	Uint8 nv1;
	Uint8 nv2;
	Uint8 nv3;
};

extern BYTE dungeon[DMAXX][DMAXY];
extern BYTE pdungeon[DMAXX][DMAXY];
extern char dflags[DMAXX][DMAXY];
extern int setpc_x;
extern int setpc_y;
extern int setpc_w;
extern int setpc_h;
extern BYTE *pSetPiece;
extern bool setloadflag;
extern BYTE *pSpecialCels;
extern BYTE *pMegaTiles;
extern BYTE *pLevelPieces;
extern BYTE *pDungeonCels;
extern char block_lvid[MAXTILES + 1];
extern bool nBlockTable[MAXTILES + 1];
extern bool nSolidTable[MAXTILES + 1];
extern bool nTransTable[MAXTILES + 1];
extern bool nMissileTable[MAXTILES + 1];
extern bool nTrapTable[MAXTILES + 1];
extern int dminx;
extern int dminy;
extern int dmaxx;
extern int dmaxy;
extern dungeon_type leveltype;
extern BYTE currlevel;
extern bool setlevel;
extern _setlevels setlvlnum;
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
extern bool TransList[256];
extern int dPiece[MAXDUNX][MAXDUNY];
extern MICROS dpiece_defs_map_2[MAXDUNX][MAXDUNY];
extern char dTransVal[MAXDUNX][MAXDUNY];
extern char dLight[MAXDUNX][MAXDUNY];
extern char dPreLight[MAXDUNX][MAXDUNY];
extern char dFlags[MAXDUNX][MAXDUNY];
extern char dPlayer[MAXDUNX][MAXDUNY];
extern int16_t dMonster[MAXDUNX][MAXDUNY];
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
bool SkipThemeRoom(int x, int y);
void InitLevels();

}

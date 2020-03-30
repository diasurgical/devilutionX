/**
 * @file gendung.h
 *
 * Interface of general dungeon generation code.
 */
#ifndef __GENDUNG_H__
#define __GENDUNG_H__

extern WORD level_frame_types[MAXTILES];
extern int themeCount;
extern BOOLEAN nTransTable[2049];
//int dword_52D204;
extern int dMonster[MAXDUNX][MAXDUNY];
extern BYTE dungeon[DMAXX][DMAXY];
extern char dObject[MAXDUNX][MAXDUNY];
extern BYTE pdungeon[DMAXX][DMAXY];
extern char dDead[MAXDUNX][MAXDUNY];
extern char dPreLight[MAXDUNX][MAXDUNY];
extern char TransVal;
extern int MicroTileLen;
extern char dflags[DMAXX][DMAXY];
extern int dPiece[MAXDUNX][MAXDUNY];
extern char dLight[MAXDUNX][MAXDUNY];
extern BOOL setloadflag;
extern BYTE *pMegaTiles;
extern BYTE *pLevelPieces;
extern int gnDifficulty;
extern char block_lvid[2049];
//char byte_5B78EB;
extern char dTransVal[MAXDUNX][MAXDUNY];
extern BOOLEAN nTrapTable[2049];
extern BYTE leveltype;
extern BYTE currlevel;
extern BOOLEAN TransList[256];
extern BOOLEAN nSolidTable[2049];
extern ScrollStruct ScrollInfo;
extern BYTE *pDungeonCels;
extern THEME_LOC themeLoc[MAXTHEMES];
extern char dPlayer[MAXDUNX][MAXDUNY];
extern int ViewBX;
extern int ViewBY;
extern int ViewDX;
extern int ViewDY;
extern char dSpecial[MAXDUNX][MAXDUNY];
extern BOOLEAN nBlockTable[2049];
extern BYTE *pSpecialCels;
extern char dFlags[MAXDUNX][MAXDUNY];
extern char dItem[MAXDUNX][MAXDUNY];
extern BYTE setlvlnum;
extern BOOLEAN nMissileTable[2049];
extern BYTE *pSetPiece;
extern char setlvltype;
extern BOOLEAN setlevel;
extern int LvlViewY;
extern int LvlViewX;
extern int dmaxx;
extern int dmaxy;
extern int setpc_h;
extern int setpc_w;
extern int setpc_x;
extern int ViewX;
extern int ViewY;
extern int setpc_y;
extern char dMissile[MAXDUNX][MAXDUNY];
extern int dminx;
extern int dminy;
extern MICROS dpiece_defs_map_2[MAXDUNX][MAXDUNY];

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
BOOL DRLG_WillThemeRoomFit(int floor, int x, int y, int minSize, int maxSize, int *width, int *height);
void DRLG_CreateThemeRoom(int themeIndex);
void DRLG_PlaceThemeRooms(int minSize, int maxSize, int floor, int freq, int rndSize);
void DRLG_HoldThemeRooms();
BOOL SkipThemeRoom(int x, int y);
void InitLevels();

#endif /* __GENDUNG_H__ */

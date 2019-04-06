//HEADER_GOES_HERE
#ifndef __GENDUNG_H__
#define __GENDUNG_H__

extern WORD level_frame_types[MAXTILES];
extern int themeCount;
extern char nTransTable[2049];
//int dword_52D204;
extern int dMonster[MAXDUNX][MAXDUNY];
extern unsigned char dungeon[40][40];
extern char dObject[MAXDUNX][MAXDUNY];
extern BYTE *pSpeedCels;
extern int nlevel_frames; // weak
extern char pdungeon[40][40];
extern char dDead[MAXDUNX][MAXDUNY];
extern WORD dpiece_defs_map_1[MAXDUNX * MAXDUNY][16];
extern char dPreLight[MAXDUNX][MAXDUNY];
extern char TransVal; // weak
extern int MicroTileLen;
extern char dflags[40][40];
extern int dPiece[MAXDUNX][MAXDUNY];
extern char dLight[MAXDUNX][MAXDUNY];
extern int setloadflag_2; // weak
extern int tile_defs[MAXTILES];
extern BYTE *pMegaTiles;
extern BYTE *pLevelPieces;
extern int gnDifficulty; // idb
extern char block_lvid[2049];
//char byte_5B78EB;
extern char dTransVal[MAXDUNX][MAXDUNY];
extern BOOLEAN nTrapTable[2049];
extern BYTE leveltype;
extern unsigned char currlevel; // idb
extern char TransList[256];
extern BOOLEAN nSolidTable[2049];
extern int level_frame_count[MAXTILES];
extern ScrollStruct ScrollInfo;
extern BYTE *pDungeonCels;
extern int SpeedFrameTbl[128][16];
extern THEME_LOC themeLoc[MAXTHEMES];
extern char dPlayer[MAXDUNX][MAXDUNY];
extern int dword_5C2FF8;   // weak
extern int dword_5C2FFC;   // weak
extern int scr_pix_width;  // weak
extern int scr_pix_height; // weak
extern char dArch[MAXDUNX][MAXDUNY];
extern char nBlockTable[2049];
extern void *level_special_cel;
extern char dFlags[MAXDUNX][MAXDUNY];
extern char dItem[MAXDUNX][MAXDUNY];
extern BYTE setlvlnum;
extern int level_frame_sizes[MAXTILES];
extern char nMissileTable[2049];
extern char *pSetPiece_2;
extern char setlvltype; // weak
extern char setlevel;   // weak
extern int LvlViewY;    // weak
extern int LvlViewX;    // weak
extern int dmaxx;       // weak
extern int dmaxy;       // weak
extern int setpc_h;     // weak
extern int setpc_w;     // weak
extern int setpc_x;     // idb
extern int ViewX;       // idb
extern int ViewY;       // idb
extern int setpc_y;     // idb
extern char dMissile[MAXDUNX][MAXDUNY];
extern int dminx; // weak
extern int dminy; // weak
extern WORD dpiece_defs_map_2[MAXDUNX][MAXDUNY][16];

void __cdecl FillSolidBlockTbls();
void __cdecl MakeSpeedCels();
void __fastcall SortTiles(int frames);
void __fastcall SwapTile(int f1, int f2);
int __fastcall IsometricCoord(int x, int y);
void __cdecl SetSpeedCels();
void __cdecl SetDungeonMicros();
void __cdecl DRLG_InitTrans();
void __fastcall DRLG_MRectTrans(int x1, int y1, int x2, int y2);
void __fastcall DRLG_RectTrans(int x1, int y1, int x2, int y2);
void __fastcall DRLG_CopyTrans(int sx, int sy, int dx, int dy);
void __fastcall DRLG_ListTrans(int num, unsigned char *List);
void __fastcall DRLG_AreaTrans(int num, unsigned char *List);
void __cdecl DRLG_InitSetPC();
void __cdecl DRLG_SetPC();
void __fastcall Make_SetPC(int x, int y, int w, int h);
BOOL __fastcall DRLG_WillThemeRoomFit(int floor, int x, int y, int minSize, int maxSize, int *width, int *height);
void __fastcall DRLG_CreateThemeRoom(int themeIndex);
void __fastcall DRLG_PlaceThemeRooms(int minSize, int maxSize, int floor, int freq, int rndSize);
void __cdecl DRLG_HoldThemeRooms();
BOOL __fastcall SkipThemeRoom(int x, int y);
void __cdecl InitLevels();

#endif /* __GENDUNG_H__ */

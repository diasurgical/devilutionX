//HEADER_GOES_HERE
#ifndef __GENDUNG_H__
#define __GENDUNG_H__

extern short level_frame_types[2048];
extern int themeCount;
extern char nTransTable[2049];
//int dword_52D204;
extern int dMonster[MAXDUNX][MAXDUNY];
extern char dungeon[40][40];
extern char dObject[MAXDUNX][MAXDUNY];
extern void *pSpeedCels;
extern int nlevel_frames; // weak
extern char pdungeon[40][40];
extern char dDead[MAXDUNX][MAXDUNY];
extern short dpiece_defs_map_1[16][MAXDUNX][MAXDUNY];
extern char dTransVal2[MAXDUNX][MAXDUNY];
extern char TransVal; // weak
extern int dword_5A5594;
extern char dflags[40][40];
extern int dPiece[MAXDUNX][MAXDUNY];
extern char dTransVal[MAXDUNX][MAXDUNY];
extern int setloadflag_2; // weak
extern int tile_defs[2048];
extern void *pMegaTiles;
extern void *pLevelPieces;
extern int gnDifficulty; // idb
extern char block_lvid[2049];
//char byte_5B78EB;
extern char dung_map[MAXDUNX][MAXDUNY];
extern char nTrapTable[2049];
extern char leveltype; // weak
extern unsigned char currlevel; // idb
extern char TransList[256];
extern UCHAR nSolidTable[2049];
extern int level_frame_count[2048];
extern ScrollStruct ScrollInfo;
extern void *pDungeonCels;
extern int speed_cel_frame_num_from_light_index_frame_num[16][128];
extern THEME_LOC themeLoc[MAXTHEMES];
extern char dPlayer[MAXDUNX][MAXDUNY];
extern int dword_5C2FF8; // weak
extern int dword_5C2FFC; // weak
extern int scr_pix_width; // weak
extern int scr_pix_height; // weak
extern char dArch[MAXDUNX][MAXDUNY];
extern char nBlockTable[2049];
extern void *level_special_cel;
extern char dFlags[MAXDUNX][MAXDUNY];
extern char dItem[MAXDUNX][MAXDUNY];
extern char setlvlnum; // weak
extern int level_frame_sizes[2048];
extern char nMissileTable[2049];
extern char *pSetPiece_2;
extern char setlvltype; // weak
extern char setlevel; // weak
extern int LvlViewY; // weak
extern int LvlViewX; // weak
extern int dmaxx; // weak
extern int dmaxy; // weak
extern int setpc_h; // weak
extern int setpc_w; // weak
extern int setpc_x; // idb
extern int ViewX; // idb
extern int ViewY; // idb
extern int setpc_y; // idb
extern char dMissile[MAXDUNX][MAXDUNY];
extern int dminx; // weak
extern int dminy; // weak
extern short dpiece_defs_map_2[16][MAXDUNX][MAXDUNY];

void __cdecl FillSolidBlockTbls();
void __cdecl gendung_418D91();
void __fastcall gendung_4191BF(int frames);
void __fastcall gendung_4191FB(int a1, int a2);
int __fastcall gendung_get_dpiece_num_from_coord(int x, int y);
void __cdecl gendung_4192C2();
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
bool __fastcall DRLG_WillThemeRoomFit(int floor, int x, int y, int minSize, int maxSize, int *width, int *height);
void __fastcall DRLG_CreateThemeRoom(int themeIndex);
void __fastcall DRLG_PlaceThemeRooms(int minSize, int maxSize, int floor, int freq, int rndSize);
void __cdecl DRLG_HoldThemeRooms();
bool __fastcall SkipThemeRoom(int x, int y);
void __cdecl InitLevels();

#endif /* __GENDUNG_H__ */

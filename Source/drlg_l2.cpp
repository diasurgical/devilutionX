//HEADER_GOES_HERE

#include "../types.h"

#ifndef NO_GLOBALS
int nSx1;
int nSx2; // weak
int nSy1;
int nSy2; // weak
int nRoomCnt;
char predungeon[40][40];
ROOMNODE RoomList[81];
HALLNODE *pHallList;
#endif

int Area_Min = 2; // weak
int Room_Max = 10; // weak
int Room_Min = 4; // weak
int Dir_Xadd[5] = { 0, 0, 1, 0, -1 };
int Dir_Yadd[5] = { 0, -1, 0, 1, 0 };
ShadowStruct SPATSL2[2] = { { 6u, 3u, 0u, 3u, 48u, 0u, 50u }, { 9u, 3u, 0u, 3u, 48u, 0u, 50u } };
//short word_48489A = 0; // weak
unsigned char BTYPESL2[161] = { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 0, 0, 0, 0, 0, 0, 0, 17, 18, 1, 1, 2, 2, 1, 1, 1, 1, 1, 1, 2, 2, 2, 2, 2, 0, 0, 0, 0, 8, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 0, 0, 2, 2, 2, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 3, 3, 3, 3, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 3, 3, 3, 0, 3, 0, 3, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 };
unsigned char BSTYPESL2[161] = { 0, 1, 2, 3, 0, 0, 6, 0, 0, 9, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 2, 2, 1, 1, 1, 1, 1, 1, 2, 2, 2, 2, 2, 0, 0, 0, 0, 0, 6, 6, 6, 9, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 0, 0, 2, 2, 2, 0, 0, 0, 1, 1, 1, 1, 6, 2, 2, 2, 0, 3, 3, 3, 3, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 1, 1, 2, 2, 3, 3, 3, 3, 1, 1, 2, 2, 3, 3, 3, 3, 1, 1, 3, 3, 2, 2, 3, 3, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 };
unsigned char VARCH1[] = { 2, 4, 3, 0, 3, 1, 3, 4, 0, 7, 48, 0, 51, 39, 47, 44, 0, 0 };
unsigned char VARCH2[] = { 2, 4, 3, 0, 3, 1, 3, 4, 0, 8, 48, 0, 51, 39, 47, 44, 0, 0 };
unsigned char VARCH3[] = { 2, 4, 3, 0, 3, 1, 3, 4, 0, 6, 48, 0, 51, 39, 47, 44, 0, 0 };
unsigned char VARCH4[] = { 2, 4, 3, 0, 3, 1, 3, 4, 0, 9, 48, 0, 51, 39, 47, 44, 0, 0 };
unsigned char VARCH5[] = { 2, 4, 3, 0, 3, 1, 3, 4, 0, 14, 48, 0, 51, 39, 47, 44, 0, 0 };
unsigned char VARCH6[] = { 2, 4, 3, 0, 3, 1, 3, 4, 0, 13, 48, 0, 51, 39, 47, 44, 0, 0 };
unsigned char VARCH7[] = { 2, 4, 3, 0, 3, 1, 3, 4, 0, 16, 48, 0, 51, 39, 47, 44, 0, 0 };
unsigned char VARCH8[] = { 2, 4, 3, 0, 3, 1, 3, 4, 0, 15, 48, 0, 51, 39, 47, 44, 0, 0 };
unsigned char VARCH9[] = { 2, 4, 3, 0, 3, 8, 3, 4, 0, 7, 48, 0, 51, 42, 47, 44, 0, 0 };
unsigned char VARCH10[] = { 2, 4, 3, 0, 3, 8, 3, 4, 0, 8, 48, 0, 51, 42, 47, 44, 0, 0 };
unsigned char VARCH11[] = { 2, 4, 3, 0, 3, 8, 3, 4, 0, 6, 48, 0, 51, 42, 47, 44, 0, 0 };
unsigned char VARCH12[] = { 2, 4, 3, 0, 3, 8, 3, 4, 0, 9, 48, 0, 51, 42, 47, 44, 0, 0 };
unsigned char VARCH13[] = { 2, 4, 3, 0, 3, 8, 3, 4, 0, 14, 48, 0, 51, 42, 47, 44, 0, 0 };
unsigned char VARCH14[] = { 2, 4, 3, 0, 3, 8, 3, 4, 0, 13, 48, 0, 51, 42, 47, 44, 0, 0 };
unsigned char VARCH15[] = { 2, 4, 3, 0, 3, 8, 3, 4, 0, 16, 48, 0, 51, 42, 47, 44, 0, 0 };
unsigned char VARCH16[] = { 2, 4, 3, 0, 3, 8, 3, 4, 0, 15, 48, 0, 51, 42, 47, 44, 0, 0 };
unsigned char VARCH17[] = { 2, 3, 2, 7, 3, 4, 0, 7, 141, 39, 47, 44, 0, 0 };
unsigned char VARCH18[] = { 2, 3, 2, 7, 3, 4, 0, 8, 141, 39, 47, 44, 0, 0 };
unsigned char VARCH19[] = { 2, 3, 2, 7, 3, 4, 0, 6, 141, 39, 47, 44, 0, 0 };
unsigned char VARCH20[] = { 2, 3, 2, 7, 3, 4, 0, 9, 141, 39, 47, 44, 0, 0 };
unsigned char VARCH21[] = { 2, 3, 2, 7, 3, 4, 0, 14, 141, 39, 47, 44, 0, 0 };
unsigned char VARCH22[] = { 2, 3, 2, 7, 3, 4, 0, 13, 141, 39, 47, 44, 0, 0 };
unsigned char VARCH23[] = { 2, 3, 2, 7, 3, 4, 0, 16, 141, 39, 47, 44, 0, 0 };
unsigned char VARCH24[] = { 2, 3, 2, 7, 3, 4, 0, 15, 141, 39, 47, 44, 0, 0 };
unsigned char VARCH25[] = { 2, 4, 3, 0, 3, 4, 3, 1, 0, 7, 48, 0, 51, 39, 47, 44, 0, 0 };
unsigned char VARCH26[] = { 2, 4, 3, 0, 3, 4, 3, 1, 0, 8, 48, 0, 51, 39, 47, 44, 0, 0 };
unsigned char VARCH27[] = { 2, 4, 3, 0, 3, 4, 3, 1, 0, 6, 48, 0, 51, 39, 47, 44, 0, 0 };
unsigned char VARCH28[] = { 2, 4, 3, 0, 3, 4, 3, 1, 0, 9, 48, 0, 51, 39, 47, 44, 0, 0 };
unsigned char VARCH29[] = { 2, 4, 3, 0, 3, 4, 3, 1, 0, 14, 48, 0, 51, 39, 47, 44, 0, 0 };
unsigned char VARCH30[] = { 2, 4, 3, 0, 3, 4, 3, 1, 0, 13, 48, 0, 51, 39, 47, 44, 0, 0 };
unsigned char VARCH31[] = { 2, 4, 3, 0, 3, 4, 3, 1, 0, 16, 48, 0, 51, 39, 47, 44, 0, 0 };
unsigned char VARCH32[] = { 2, 4, 3, 0, 3, 4, 3, 1, 0, 15, 48, 0, 51, 39, 47, 44, 0, 0 };
unsigned char VARCH33[] = { 2, 4, 2, 0, 3, 8, 3, 4, 0, 7, 142, 0, 51, 42, 47, 44, 0, 0 };
unsigned char VARCH34[] = { 2, 4, 2, 0, 3, 8, 3, 4, 0, 8, 142, 0, 51, 42, 47, 44, 0, 0 };
unsigned char VARCH35[] = { 2, 4, 2, 0, 3, 8, 3, 4, 0, 6, 142, 0, 51, 42, 47, 44, 0, 0 };
unsigned char VARCH36[] = { 2, 4, 2, 0, 3, 8, 3, 4, 0, 9, 142, 0, 51, 42, 47, 44, 0, 0 };
unsigned char VARCH37[] = { 2, 4, 2, 0, 3, 8, 3, 4, 0, 14, 142, 0, 51, 42, 47, 44, 0, 0 };
unsigned char VARCH38[] = { 2, 4, 2, 0, 3, 8, 3, 4, 0, 13, 142, 0, 51, 42, 47, 44, 0, 0 };
unsigned char VARCH39[] = { 2, 4, 2, 0, 3, 8, 3, 4, 0, 16, 142, 0, 51, 42, 47, 44, 0, 0 };
unsigned char VARCH40[] = { 2, 4, 2, 0, 3, 8, 3, 4, 0, 15, 142, 0, 51, 42, 47, 44, 0, 0 };
unsigned char HARCH1[] = { 3, 2, 3, 3, 0, 2, 5, 9, 49, 46, 0, 40, 45, 0 };
unsigned char HARCH2[] = { 3, 2, 3, 3, 0, 2, 5, 6, 49, 46, 0, 40, 45, 0 };
unsigned char HARCH3[] = { 3, 2, 3, 3, 0, 2, 5, 8, 49, 46, 0, 40, 45, 0 };
unsigned char HARCH4[] = { 3, 2, 3, 3, 0, 2, 5, 7, 49, 46, 0, 40, 45, 0 };
unsigned char HARCH5[] = { 3, 2, 3, 3, 0, 2, 5, 15, 49, 46, 0, 40, 45, 0 };
unsigned char HARCH6[] = { 3, 2, 3, 3, 0, 2, 5, 16, 49, 46, 0, 40, 45, 0 };
unsigned char HARCH7[] = { 3, 2, 3, 3, 0, 2, 5, 13, 49, 46, 0, 40, 45, 0 };
unsigned char HARCH8[] = { 3, 2, 3, 3, 0, 2, 5, 14, 49, 46, 0, 40, 45, 0 };
unsigned char HARCH9[] = { 3, 2, 3, 3, 0, 8, 5, 9, 49, 46, 0, 43, 45, 0 };
unsigned char HARCH10[] = { 3, 2, 3, 3, 0, 8, 5, 6, 49, 46, 0, 43, 45, 0 };
unsigned char HARCH11[] = { 3, 2, 3, 3, 0, 8, 5, 8, 49, 46, 0, 43, 45, 0 };
unsigned char HARCH12[] = { 3, 2, 3, 3, 0, 8, 5, 7, 49, 46, 0, 43, 45, 0 };
unsigned char HARCH13[] = { 3, 2, 3, 3, 0, 8, 5, 15, 49, 46, 0, 43, 45, 0 };
unsigned char HARCH14[] = { 3, 2, 3, 3, 0, 8, 5, 16, 49, 46, 0, 43, 45, 0 };
unsigned char HARCH15[] = { 3, 2, 3, 3, 0, 8, 5, 13, 49, 46, 0, 43, 45, 0 };
unsigned char HARCH16[] = { 3, 2, 3, 3, 0, 8, 5, 14, 49, 46, 0, 43, 45, 0 };
unsigned char HARCH17[] = { 3, 2, 1, 3, 0, 8, 5, 9, 140, 46, 0, 43, 45, 0 };
unsigned char HARCH18[] = { 3, 2, 1, 3, 0, 8, 5, 6, 140, 46, 0, 43, 45, 0 };
unsigned char HARCH19[] = { 3, 2, 1, 3, 0, 8, 5, 8, 140, 46, 0, 43, 45, 0 };
unsigned char HARCH20[] = { 3, 2, 1, 3, 0, 8, 5, 7, 140, 46, 0, 43, 45, 0 };
unsigned char HARCH21[] = { 3, 2, 1, 3, 0, 8, 5, 15, 140, 46, 0, 43, 45, 0 };
unsigned char HARCH22[] = { 3, 2, 1, 3, 0, 8, 5, 16, 140, 46, 0, 43, 45, 0 };
unsigned char HARCH23[] = { 3, 2, 1, 3, 0, 8, 5, 13, 140, 46, 0, 43, 45, 0 };
unsigned char HARCH24[] = { 3, 2, 1, 3, 0, 8, 5, 14, 140, 46, 0, 43, 45, 0 };
unsigned char HARCH25[] = { 3, 2, 3, 3, 0, 5, 2, 9, 49, 46, 0, 40, 45, 0 };
unsigned char HARCH26[] = { 3, 2, 3, 3, 0, 5, 2, 6, 49, 46, 0, 40, 45, 0 };
unsigned char HARCH27[] = { 3, 2, 3, 3, 0, 5, 2, 8, 49, 46, 0, 40, 45, 0 };
unsigned char HARCH28[] = { 3, 2, 3, 3, 0, 5, 2, 7, 49, 46, 0, 40, 45, 0 };
unsigned char HARCH29[] = { 3, 2, 3, 3, 0, 5, 2, 15, 49, 46, 0, 40, 45, 0 };
unsigned char HARCH30[] = { 3, 2, 3, 3, 0, 5, 2, 16, 49, 46, 0, 40, 45, 0 };
unsigned char HARCH31[] = { 3, 2, 3, 3, 0, 5, 2, 13, 49, 46, 0, 40, 45, 0 };
unsigned char HARCH32[] = { 3, 2, 3, 3, 0, 5, 2, 14, 49, 46, 0, 40, 45, 0 };
unsigned char HARCH33[] = { 3, 2, 1, 3, 0, 9, 5, 9, 140, 46, 0, 40, 45, 0 };
unsigned char HARCH34[] = { 3, 2, 1, 3, 0, 9, 5, 6, 140, 46, 0, 40, 45, 0 };
unsigned char HARCH35[] = { 3, 2, 1, 3, 0, 9, 5, 8, 140, 46, 0, 40, 45, 0 };
unsigned char HARCH36[] = { 3, 2, 1, 3, 0, 9, 5, 7, 140, 46, 0, 40, 45, 0 };
unsigned char HARCH37[] = { 3, 2, 1, 3, 0, 9, 5, 15, 140, 46, 0, 40, 45, 0 };
unsigned char HARCH38[] = { 3, 2, 1, 3, 0, 9, 5, 16, 140, 46, 0, 40, 45, 0 };
unsigned char HARCH39[] = { 3, 2, 1, 3, 0, 9, 5, 13, 140, 46, 0, 40, 45, 0 };
unsigned char HARCH40[] = { 3, 2, 1, 3, 0, 9, 5, 14, 140, 46, 0, 40, 45, 0 };
unsigned char USTAIRS[] = { 4, 4, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 0, 0, 0, 0, 0, 72, 77, 0, 0, 76, 0, 0, 0, 0, 0, 0 };
unsigned char DSTAIRS[] = { 4, 4, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 0, 0, 0, 0, 0, 48, 71, 0, 0, 50, 78, 0, 0, 0, 0, 0 };
unsigned char WARPSTAIRS[] = { 4, 4, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 0, 0, 0, 0, 0, 158, 160, 0, 0, 159, 0, 0, 0, 0, 0, 0 };
unsigned char CRUSHCOL[] = { 3, 3, 3, 1, 3, 2, 6, 3, 3, 3, 3, 0, 0, 0, 0, 83, 0, 0, 0, 0 };
unsigned char BIG1[] = { 2, 2, 3, 3, 3, 3, 113, 0, 112, 0 };
unsigned char BIG2[] = { 2, 2, 3, 3, 3, 3, 114, 115, 0, 0 };
unsigned char BIG3[] = { 1, 2, 1, 1, 117, 116 };
unsigned char BIG4[] = { 2, 1, 2, 2, 118, 119 };
unsigned char BIG5[] = { 2, 2, 3, 3, 3, 3, 120, 122, 121, 123 };
unsigned char BIG6[] = { 1, 2, 1, 1, 125, 124 };
unsigned char BIG7[] = { 2, 1, 2, 2, 126, 127 };
unsigned char BIG8[] = { 2, 2, 3, 3, 3, 3, 128, 130, 129, 131 };
unsigned char BIG9[] = { 2, 2, 1, 3, 1, 3, 133, 135, 132, 134 };
unsigned char BIG10[] = { 2, 2, 2, 2, 3, 3, 136, 137, 3, 3 };
unsigned char RUINS1[] = { 1, 1, 1, 80 };
unsigned char RUINS2[] = { 1, 1, 1, 81 };
unsigned char RUINS3[] = { 1, 1, 1, 82 };
unsigned char RUINS4[] = { 1, 1, 2, 84 };
unsigned char RUINS5[] = { 1, 1, 2, 85 };
unsigned char RUINS6[] = { 1, 1, 2, 86 };
unsigned char RUINS7[] = { 1, 1, 8, 87 };
unsigned char PANCREAS1[] = { 5, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 0, 0, 0, 0, 0, 0, 0, 108, 0, 0, 0, 0, 0, 0, 0 };
unsigned char PANCREAS2[] = { 5, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 0, 0, 0, 0, 0, 0, 0, 110, 0, 0, 0, 0, 0, 0, 0 };
unsigned char CTRDOOR1[] = { 3, 3, 3, 1, 3, 0, 4, 0, 0, 9, 0, 0, 4, 0, 0, 1, 0, 0, 0, 0 };
unsigned char CTRDOOR2[] = { 3, 3, 3, 1, 3, 0, 4, 0, 0, 8, 0, 0, 4, 0, 0, 1, 0, 0, 0, 0 };
unsigned char CTRDOOR3[] = { 3, 3, 3, 1, 3, 0, 4, 0, 0, 6, 0, 0, 4, 0, 0, 1, 0, 0, 0, 0 };
unsigned char CTRDOOR4[] = { 3, 3, 3, 1, 3, 0, 4, 0, 0, 7, 0, 0, 4, 0, 0, 1, 0, 0, 0, 0 };
unsigned char CTRDOOR5[] = { 3, 3, 3, 1, 3, 0, 4, 0, 0, 15, 0, 0, 4, 0, 0, 1, 0, 0, 0, 0 };
unsigned char CTRDOOR6[] = { 3, 3, 3, 1, 3, 0, 4, 0, 0, 13, 0, 0, 4, 0, 0, 1, 0, 0, 0, 0 };
unsigned char CTRDOOR7[] = { 3, 3, 3, 1, 3, 0, 4, 0, 0, 16, 0, 0, 4, 0, 0, 1, 0, 0, 0, 0 };
unsigned char CTRDOOR8[] = { 3, 3, 3, 1, 3, 0, 4, 0, 0, 14, 0, 0, 4, 0, 0, 1, 0, 0, 0, 0 };
int Patterns[100][10] =
{
  { 0, 0, 0, 0, 0, 0, 0, 0, 0, 3 },
  { 0, 0, 0, 0, 2, 0, 0, 0, 0, 3 },
  { 0, 7, 0, 0, 1, 0, 0, 5, 0, 2 },
  { 0, 5, 0, 0, 1, 0, 0, 7, 0, 2 },
  { 0, 0, 0, 7, 1, 5, 0, 0, 0, 1 },
  { 0, 0, 0, 5, 1, 7, 0, 0, 0, 1 },
  { 0, 1, 0, 0, 3, 0, 0, 1, 0, 4 },
  { 0, 0, 0, 1, 3, 1, 0, 0, 0, 5 },
  { 0, 6, 0, 6, 1, 0, 0, 0, 0, 6 },
  { 0, 6, 0, 0, 1, 6, 0, 0, 0, 9 },
  { 0, 0, 0, 6, 1, 0, 0, 6, 0, 7 },
  { 0, 0, 0, 0, 1, 6, 0, 6, 0, 8 },
  { 0, 6, 0, 6, 6, 0, 8, 6, 0, 7 },
  { 0, 6, 8, 6, 6, 6, 0, 0, 0, 9 },
  { 0, 6, 0, 0, 6, 6, 0, 6, 8, 8 },
  { 6, 6, 6, 6, 6, 6, 0, 6, 0, 8 },
  { 2, 6, 6, 6, 6, 6, 0, 6, 0, 8 },
  { 7, 7, 7, 6, 6, 6, 0, 6, 0, 8 },
  { 6, 6, 2, 6, 6, 6, 0, 6, 0, 8 },
  { 6, 2, 6, 6, 6, 6, 0, 6, 0, 8 },
  { 2, 6, 6, 6, 6, 6, 0, 6, 0, 8 },
  { 6, 7, 7, 6, 6, 6, 0, 6, 0, 8 },
  { 4, 4, 6, 6, 6, 6, 2, 6, 2, 8 },
  { 2, 2, 2, 2, 6, 2, 2, 6, 2, 7 },
  { 2, 2, 2, 2, 6, 2, 6, 6, 6, 7 },
  { 2, 2, 6, 2, 6, 6, 2, 2, 6, 9 },
  { 2, 6, 2, 2, 6, 2, 2, 2, 2, 6 },
  { 2, 2, 2, 2, 6, 6, 2, 2, 2, 9 },
  { 2, 2, 2, 6, 6, 2, 2, 2, 2, 6 },
  { 2, 2, 0, 2, 6, 6, 2, 2, 0, 9 },
  { 0, 0, 0, 0, 4, 0, 0, 0, 0, 12 },
  { 0, 1, 0, 0, 1, 4, 0, 1, 0, 10 },
  { 0, 0, 0, 1, 1, 1, 0, 4, 0, 11 },
  { 0, 0, 0, 6, 1, 4, 0, 1, 0, 14 },
  { 0, 6, 0, 1, 1, 0, 0, 4, 0, 16 },
  { 0, 6, 0, 0, 1, 1, 0, 4, 0, 15 },
  { 0, 0, 0, 0, 1, 1, 0, 1, 4, 13 },
  { 8, 8, 8, 8, 1, 1, 0, 1, 1, 13 },
  { 8, 8, 4, 8, 1, 1, 0, 1, 1, 10 },
  { 0, 0, 0, 1, 1, 1, 1, 1, 1, 11 },
  { 1, 1, 1, 1, 1, 1, 2, 2, 8, 2 },
  { 0, 1, 0, 1, 1, 4, 1, 1, 0, 16 },
  { 0, 0, 0, 1, 1, 1, 1, 1, 4, 11 },
  { 1, 1, 4, 1, 1, 1, 0, 2, 2, 2 },
  { 1, 1, 1, 1, 1, 1, 6, 2, 6, 2 },
  { 4, 1, 1, 1, 1, 1, 6, 2, 6, 2 },
  { 2, 2, 2, 1, 1, 1, 4, 1, 1, 11 },
  { 4, 1, 1, 1, 1, 1, 2, 2, 2, 2 },
  { 1, 1, 4, 1, 1, 1, 2, 2, 1, 2 },
  { 4, 1, 1, 1, 1, 1, 1, 2, 2, 2 },
  { 2, 2, 6, 1, 1, 1, 4, 1, 1, 11 },
  { 4, 1, 1, 1, 1, 1, 2, 2, 6, 2 },
  { 1, 2, 2, 1, 1, 1, 4, 1, 1, 11 },
  { 0, 1, 1, 0, 1, 1, 0, 1, 1, 10 },
  { 2, 1, 1, 3, 1, 1, 2, 1, 1, 14 },
  { 1, 1, 0, 1, 1, 2, 1, 1, 0, 1 },
  { 0, 4, 0, 1, 1, 1, 0, 1, 1, 14 },
  { 4, 1, 0, 1, 1, 0, 1, 1, 0, 1 },
  { 0, 1, 0, 4, 1, 1, 0, 1, 1, 15 },
  { 1, 1, 1, 1, 1, 1, 0, 2, 2, 2 },
  { 0, 1, 1, 2, 1, 1, 2, 1, 4, 10 },
  { 2, 1, 1, 1, 1, 1, 0, 4, 0, 16 },
  { 1, 1, 4, 1, 1, 2, 0, 1, 2, 1 },
  { 2, 1, 1, 2, 1, 1, 1, 1, 4, 10 },
  { 1, 1, 2, 1, 1, 2, 4, 1, 8, 1 },
  { 2, 1, 4, 1, 1, 1, 4, 4, 1, 16 },
  { 2, 1, 1, 1, 1, 1, 1, 1, 1, 16 },
  { 1, 1, 2, 1, 1, 1, 1, 1, 1, 15 },
  { 1, 1, 1, 1, 1, 1, 2, 1, 1, 14 },
  { 4, 1, 1, 1, 1, 1, 2, 1, 1, 14 },
  { 1, 1, 1, 1, 1, 1, 1, 1, 2, 8 },
  { 0, 0, 0, 0, 255, 0, 0, 0, 0, 0 },
  { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },
  { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },
  { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },
  { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },
  { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },
  { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },
  { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },
  { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },
  { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },
  { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },
  { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },
  { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },
  { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },
  { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },
  { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },
  { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },
  { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },
  { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },
  { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },
  { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },
  { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },
  { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },
  { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },
  { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },
  { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },
  { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },
  { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },
  { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 }
};

void __cdecl InitDungeon()
{
	signed int v0; // edx
	signed int v1; // eax
	signed int v2; // ecx

	v0 = 0;
	do
	{
		v1 = v0;
		v2 = 40;
		do
		{
			dflags[0][v1] = 0;
			predungeon[0][v1] = 32;
			v1 += 40;
			--v2;
		}
		while ( v2 );
		++v0;
	}
	while ( v0 < 40 );
}

void __cdecl L2LockoutFix()
{
	signed int v0; // ecx
	char *v1; // eax
	signed int v2; // edx
	signed int v3; // ecx
	signed int v4; // edi
	signed int v5; // eax
	char *v6; // esi
	signed int v7; // edx
	char v8; // al
	unsigned int v9; // ecx
	signed int v10; // eax
	char v11; // dl
	signed int v12; // esi
	char v13; // bl
	char *v14; // edx

	v0 = 0;
	do
	{
		v1 = (char *)dungeon + v0;
		v2 = 40;
		do
		{
			if ( *v1 == 4 && *(v1 - 40) != 3 )
				*v1 = 1;
			if ( *v1 == 5 && *(v1 - 1) != 3 )
				*v1 = 2;
			v1 += 40;
			--v2;
		}
		while ( v2 );
		++v0;
	}
	while ( v0 < 40 );
	v3 = 1;
	do
	{
		v4 = 1;
		do
		{
			v5 = v4;
			if ( dflags[v4][v3] >= 0 )
			{
				v6 = &dungeon[v5][v3];
				if ( (*v6 == 2 || *v6 == 5) && *(v6 - 1) == 3 && dungeon[v5][v3 + 1] == 3 )
				{
					v7 = 0;
					while ( 1 )
					{
						v8 = *v6;
						if ( *v6 != 2 && v8 != 5 )
							break;
						if ( *(v6 - 1) != 3 || v6[1] != 3 )
							break;
						if ( v8 == 5 )
							v7 = 1;
						++v4;
						v6 += 40;
					}
					if ( !v7 && dflags[v4 - 1][v3] >= 0 ) // dflags[-1][]
						dungeon[v4 - 1][v3] = 5; // dungeon[-1][]
				}
			}
			++v4;
		}
		while ( v4 < 39 );
		++v3;
	}
	while ( v3 < 39 );
	v9 = 1;
	do
	{
		v10 = 1;
		do
		{
			if ( dflags[v9][v10] >= 0 )
			{
				v11 = dungeon[v9][v10];
				if ( (v11 == 1 || v11 == 4)
				  && dungeon[v9 - 1][v10] == 3 // dungeon[-1][]
				  && dungeon[v9 + 1][v10] == 3 )
				{
					v12 = 0;
					while ( 1 )
					{
						v13 = dungeon[v9][v10];
						if ( v13 != 1 && v13 != 4 )
							break;
						v14 = &dungeon[v9 + 1][v10];
						if ( *(v14 - 80) != 3 || *v14 != 3 )
							break;
						if ( v13 == 4 )
							v12 = 1;
						++v10;
					}
					if ( !v12 && dflags[v9][v10 - 1] >= 0 ) // dflags[][-1]
						dungeon[v9][v10 - 1] = 4; // dungeon[][-1]
				}
			}
			++v10;
		}
		while ( v10 < 39 );
		++v9;
	}
	while ( v9 < 39 );
}

void __cdecl L2DoorFix()
{
	signed int v0; // ecx
	char *v1; // eax
	signed int v2; // edx

	v0 = 1;
	do
	{
		v1 = &dungeon[1][v0];
		v2 = 39;
		do
		{
			if ( *v1 == 4 && *(v1 - 1) == 3 )
				*v1 = 7;
			if ( *v1 == 5 && *(v1 - 40) == 3 )
				*v1 = 9;
			v1 += 40;
			--v2;
		}
		while ( v2 );
		++v0;
	}
	while ( v0 < 40 );
}

void __fastcall LoadL2Dungeon(char *sFileName, int vx, int vy)
{
	char *v3; // esi
	unsigned char *v4; // edi
	signed int v5; // edx
	signed int v6; // eax
	signed int v7; // ecx
	int v8; // esi
	int v9; // eax
	int v10; // ebx
	int v11; // edi
	char *v12; // eax
	int v13; // ecx
	char v14; // dl
	signed int v15; // ecx
	_BYTE *v16; // eax
	signed int v17; // edx
	int v18; // ebx
	int (*v19)[112]; // esi
	char *v20; // ecx
	signed int v21; // edi
	int v22; // edx
	char v23; // al
	int v24; // ecx
	int (*v25)[112]; // edi
	char *v26; // eax
	int (*v27)[112]; // edx
	signed int v28; // ebx
	int v29; // esi
	int v30; // [esp+Ch] [ebp-Ch]
	char *ptr; // [esp+10h] [ebp-8h]
	int v32; // [esp+14h] [ebp-4h]
	int (*v33)[112]; // [esp+14h] [ebp-4h]

	v30 = vx;
	v3 = sFileName;
	InitDungeon();
	DRLG_InitTrans();
	v4 = LoadFileInMem(v3, 0);
	v5 = 0;
	ptr = (char *)v4;
	do
	{
		v6 = v5;
		v7 = 40;
		do
		{
			dflags[0][v6] = 0;
			dungeon[0][v6] = 12;
			v6 += 40;
			--v7;
		}
		while ( v7 );
		++v5;
	}
	while ( v5 < 40 );
	v8 = *v4;
	v9 = (int)(v4 + 2);
	v10 = 0;
	v11 = v4[2];
	v12 = (char *)(v9 + 2);
	if ( v11 > 0 )
	{
		do
		{
			if ( v8 > 0 )
			{
				v13 = v10;
				v32 = v8;
				do
				{
					v14 = *v12;
					if ( *v12 )
					{
						dflags[0][v13] |= DFLAG_EXPLORED;
						dungeon[0][v13] = v14;
					}
					else
					{
						dungeon[0][v13] = 3;
					}
					v13 += 40;
					v12 += 2;
					--v32;
				}
				while ( v32 );
			}
			++v10;
		}
		while ( v10 < v11 );
	}
	v15 = 0;
	do
	{
		v16 = (unsigned char *)dungeon + v15;
		v17 = 40;
		do
		{
			if ( !*v16 )
				*v16 = 12;
			v16 += 40;
			--v17;
		}
		while ( v17 );
		++v15;
	}
	while ( v15 < 40 );
	DRLG_L2Pass3();
	DRLG_Init_Globals();
	v18 = 0;
	v33 = dPiece;
	do
	{
		v19 = v33;
		v20 = (char *)dArch + v18;
		v21 = 112;
		do
		{
			v22 = (*v19)[0];
			v23 = 0;
			if ( (*v19)[0] == 541 )
				v23 = 5;
			if ( v22 == 178 )
				v23 = 5;
			if ( v22 == 551 )
				v23 = 5;
			if ( v22 == 542 )
				v23 = 6;
			if ( v22 == 553 )
				v23 = 6;
			if ( v22 == 13 )
				v23 = 5;
			if ( v22 == 17 )
				v23 = 6;
			*v20 = v23;
			++v19;
			v20 += 112;
			--v21;
		}
		while ( v21 );
		v33 = (int (*)[112])((char *)v33 + 4);
		++v18;
	}
	while ( (signed int)v33 < (signed int)dPiece[1] );
	v24 = 0;
	v25 = dPiece;
	do
	{
		v26 = &dArch[0][v24 + 2];
		v27 = v25;
		v28 = 112;
		do
		{
			v29 = (*v27)[0];
			if ( (*v27)[0] == 132 )
			{
				*(v26 - 1) = 2;
				*v26 = 1;
			}
			else if ( v29 == 135 || v29 == 139 )
			{
				v26[110] = 3;
				v26[222] = 4;
			}
			++v27;
			v26 += 112;
			--v28;
		}
		while ( v28 );
		v25 = (int (*)[112])((char *)v25 + 4);
		++v24;
	}
	while ( (signed int)v25 < (signed int)dPiece[1] );
	ViewX = v30;
	ViewY = vy;
	SetMapMonsters((unsigned char *)ptr, 0, 0);
	SetMapObjects((unsigned char *)ptr, 0, 0);
	mem_free_dbg(ptr);
}

void __cdecl DRLG_L2Pass3()
{
	int v0; // eax
	int *v1; // edx
	int *v2; // eax
	signed int v3; // ecx
	signed int v4; // ebx
	int *v5; // ecx
	unsigned char *v6; // edx
	unsigned short *v7; // esi
	unsigned short v8; // ax
	int v9; // eax
	int v10; // ST24_4
	int v11; // ST20_4
	int v12; // ST1C_4
	signed int v13; // [esp+Ch] [ebp-1Ch]
	int *v14; // [esp+10h] [ebp-18h]
	int v15; // [esp+14h] [ebp-14h]
	int v16; // [esp+18h] [ebp-10h]
	int v17; // [esp+1Ch] [ebp-Ch]
	int v18; // [esp+20h] [ebp-8h]

	v0 = *((unsigned short *)pMegaTiles + 44) + 1;
	v18 = *((unsigned short *)pMegaTiles + 44) + 1;
	_LOWORD(v0) = *((_WORD *)pMegaTiles + 45);
	v17 = ++v0;
	_LOWORD(v0) = *((_WORD *)pMegaTiles + 46);
	v16 = ++v0;
	_LOWORD(v0) = *((_WORD *)pMegaTiles + 47);
	v15 = v0 + 1;
	v1 = dPiece[1];
	do
	{
		v2 = v1;
		v3 = 56;
		do
		{
			*(v2 - 112) = v18;
			*v2 = v17;
			*(v2 - 111) = v16;
			v2[1] = v15;
			v2 += 224;
			--v3;
		}
		while ( v3 );
		v1 += 2;
	}
	while ( (signed int)v1 < (signed int)dPiece[2] );
	v4 = 0;
	v14 = &dPiece[17][16];
	do
	{
		v5 = v14;
		v6 = (unsigned char *)dungeon + v4;
		v13 = 40;
		do
		{
			v7 = (unsigned short *)((char *)pMegaTiles + 8 * (*v6 - 1));
			v8 = *v7;
			++v7;
			v9 = v8 + 1;
			v10 = v9;
			_LOWORD(v9) = *v7;
			++v7;
			v11 = ++v9;
			_LOWORD(v9) = *v7;
			v12 = ++v9;
			_LOWORD(v9) = v7[1];
			v6 += 40;
			*(v5 - 112) = v10;
			*v5 = v11;
			*(v5 - 111) = v12;
			v5[1] = v9 + 1;
			v5 += 224;
			--v13;
		}
		while ( v13 );
		v14 += 2;
		++v4;
	}
	while ( v4 < 40 );
}

void __fastcall LoadPreL2Dungeon(char *sFileName, int vx, int vy)
{
	char *v3; // esi
	unsigned char *v4; // ebx
	signed int v5; // esi
	signed int v6; // edx
	signed int v7; // eax
	int v8; // eax
	int v9; // edi
	char *v10; // edx
	int v11; // esi
	char v12; // bl
	signed int v13; // eax
	_BYTE *v14; // edx
	signed int v15; // esi
	signed int v16; // eax
	signed int v17; // edx
	signed int v18; // esi
	unsigned char *ptr; // [esp+8h] [ebp-Ch]
	int v20; // [esp+Ch] [ebp-8h]
	int v21; // [esp+10h] [ebp-4h]

	v3 = sFileName;
	InitDungeon();
	DRLG_InitTrans();
	v4 = LoadFileInMem(v3, 0);
	v5 = 0;
	ptr = v4;
	do
	{
		v6 = v5;
		v7 = 40;
		do
		{
			dflags[0][v6] = 0;
			dungeon[0][v6] = 12;
			v6 += 40;
			--v7;
		}
		while ( v7 );
		++v5;
	}
	while ( v5 < 40 );
	v21 = 0;
	v8 = v4[2];
	v9 = *v4;
	v10 = (char *)(v4 + 4);
	if ( v8 > 0 )
	{
		do
		{
			if ( v9 > 0 )
			{
				v11 = v21;
				v20 = v9;
				do
				{
					v12 = *v10;
					if ( *v10 )
					{
						dflags[0][v11] |= DFLAG_EXPLORED;
						dungeon[0][v11] = v12;
					}
					else
					{
						dungeon[0][v11] = 3;
					}
					v11 += 40;
					v10 += 2;
					--v20;
				}
				while ( v20 );
			}
			++v21;
		}
		while ( v21 < v8 );
	}
	v13 = 0;
	do
	{
		v14 = (unsigned char *)dungeon + v13;
		v15 = 40;
		do
		{
			if ( !*v14 )
				*v14 = 12;
			v14 += 40;
			--v15;
		}
		while ( v15 );
		++v13;
	}
	while ( v13 < 40 );
	v16 = 0;
	do
	{
		v17 = v16;
		v18 = 40;
		do
		{
			pdungeon[0][v17] = dungeon[0][v17];
			v17 += 40;
			--v18;
		}
		while ( v18 );
		++v16;
	}
	while ( v16 < 40 );
	mem_free_dbg(ptr);
}

void __fastcall CreateL2Dungeon(int rseed, int entry)
{
	int v2; // esi
	int v3; // edi
	int v4; // ecx

	v2 = entry;
	v3 = rseed;
	if ( gbMaxPlayers == 1 )
	{
		if ( currlevel == 7 )
		{
			if ( quests[QTYPE_BLIND]._qactive )
				goto LABEL_10;
			currlevel = 6;
			CreateL2Dungeon(glSeedTbl[6], 4);
			currlevel = 7;
		}
		if ( currlevel == 8 )
		{
			if ( quests[QTYPE_BLIND]._qactive )
			{
				v4 = glSeedTbl[7];
				currlevel = 7;
			}
			else
			{
				v4 = glSeedTbl[6];
				currlevel = 6;
			}
			CreateL2Dungeon(v4, 4);
			currlevel = 8;
		}
	}
LABEL_10:
	SetRndSeed(v3);
	dminx = 16;
	dminy = 16;
	dmaxx = 96;
	dmaxy = 96;
	DRLG_InitTrans();
	DRLG_InitSetPC();
	DRLG_LoadL2SP();
	DRLG_L2(v2);
	DRLG_L2Pass3();
	DRLG_FreeL2SP();
	DRLG_InitL2Vals();
	DRLG_SetPC();
}
// 5CF328: using guessed type int dmaxx;
// 5CF32C: using guessed type int dmaxy;
// 5D2458: using guessed type int dminx;
// 5D245C: using guessed type int dminy;
// 679660: using guessed type char gbMaxPlayers;

void __cdecl DRLG_LoadL2SP()
{
	char *v1; // ecx

	setloadflag_2 = 0;
	if ( QuestStatus(QTYPE_BLIND) )
	{
		v1 = "Levels\\L2Data\\Blind2.DUN";
	}
	else
	{
		if ( QuestStatus(QTYPE_BLOOD) )
		{
			v1 = "Levels\\L2Data\\Blood1.DUN";
		}
		else
		{
			if ( !QuestStatus(QTYPE_BONE) )
				return;
			v1 = "Levels\\L2Data\\Bonestr2.DUN";
		}
	}
	pSetPiece_2 = (char *)LoadFileInMem(v1, 0);
	setloadflag_2 = 1;
}
// 5B50D8: using guessed type int setloadflag_2;

void __cdecl DRLG_FreeL2SP()
{
	char *v0; // ecx

	v0 = pSetPiece_2;
	pSetPiece_2 = 0;
	mem_free_dbg(v0);
}

void __fastcall DRLG_L2(int entry)
{
	int v1; // esi
	int v2; // eax
	int v3; // eax
	int v4; // eax
	int v5; // eax
	int v6; // eax
	int v7; // eax
	int v8; // eax
	int v9; // eax
	signed int v10; // ecx
	signed int v11; // eax
	signed int v12; // esi
	int v13; // [esp+10h] [ebp-4h]

	v1 = 0;
	v13 = entry;
	do
	{
		nRoomCnt = 0;
		InitDungeon();
		DRLG_InitTrans();
		v2 = CreateDungeon();
		if ( !v2 )
			continue;
		L2TileFix();
		if ( setloadflag_2 )
			DRLG_L2SetRoom(nSx1, nSy1);
		DRLG_L2FloodTVal();
		DRLG_L2TransFix();
		if ( !v13 )
		{
			v3 = DRLG_L2PlaceMiniSet(USTAIRS, 1, 1, -1, -1, 1, 0);
			v1 = v3;
			if ( !v3 )
				goto LABEL_21;
			v4 = DRLG_L2PlaceMiniSet(DSTAIRS, 1, 1, -1, -1, 0, 1);
			v1 = v4;
			if ( !v4 || currlevel != 5 )
				goto LABEL_21;
			v5 = DRLG_L2PlaceMiniSet(WARPSTAIRS, 1, 1, -1, -1, 0, 6);
LABEL_20:
			v1 = v5;
LABEL_21:
			ViewY -= 2;
			continue;
		}
		v6 = DRLG_L2PlaceMiniSet(USTAIRS, 1, 1, -1, -1, 0, 0);
		v1 = v6;
		if ( v13 != 1 )
		{
			if ( !v6 )
				goto LABEL_21;
			v9 = DRLG_L2PlaceMiniSet(DSTAIRS, 1, 1, -1, -1, 0, 1);
			v1 = v9;
			if ( !v9 || currlevel != 5 )
				goto LABEL_21;
			v5 = DRLG_L2PlaceMiniSet(WARPSTAIRS, 1, 1, -1, -1, 1, 6);
			goto LABEL_20;
		}
		if ( v6 )
		{
			v7 = DRLG_L2PlaceMiniSet(DSTAIRS, 1, 1, -1, -1, 1, 1);
			v1 = v7;
			if ( v7 )
			{
				if ( currlevel == 5 )
				{
					v8 = DRLG_L2PlaceMiniSet(WARPSTAIRS, 1, 1, -1, -1, 0, 6);
					v1 = v8;
				}
			}
		}
		--ViewX;
	}
	while ( !v1 );
	L2LockoutFix();
	L2DoorFix();
	L2DirtFix();
	DRLG_PlaceThemeRooms(6, 10, 3, 0, 0);
	DRLG_L2PlaceRndSet(CTRDOOR1, 100);
	DRLG_L2PlaceRndSet(CTRDOOR2, 100);
	DRLG_L2PlaceRndSet(CTRDOOR3, 100);
	DRLG_L2PlaceRndSet(CTRDOOR4, 100);
	DRLG_L2PlaceRndSet(CTRDOOR5, 100);
	DRLG_L2PlaceRndSet(CTRDOOR6, 100);
	DRLG_L2PlaceRndSet(CTRDOOR7, 100);
	DRLG_L2PlaceRndSet(CTRDOOR8, 100);
	DRLG_L2PlaceRndSet(VARCH33, 100);
	DRLG_L2PlaceRndSet(VARCH34, 100);
	DRLG_L2PlaceRndSet(VARCH35, 100);
	DRLG_L2PlaceRndSet(VARCH36, 100);
	DRLG_L2PlaceRndSet(VARCH37, 100);
	DRLG_L2PlaceRndSet(VARCH38, 100);
	DRLG_L2PlaceRndSet(VARCH39, 100);
	DRLG_L2PlaceRndSet(VARCH40, 100);
	DRLG_L2PlaceRndSet(VARCH1, 100);
	DRLG_L2PlaceRndSet(VARCH2, 100);
	DRLG_L2PlaceRndSet(VARCH3, 100);
	DRLG_L2PlaceRndSet(VARCH4, 100);
	DRLG_L2PlaceRndSet(VARCH5, 100);
	DRLG_L2PlaceRndSet(VARCH6, 100);
	DRLG_L2PlaceRndSet(VARCH7, 100);
	DRLG_L2PlaceRndSet(VARCH8, 100);
	DRLG_L2PlaceRndSet(VARCH9, 100);
	DRLG_L2PlaceRndSet(VARCH10, 100);
	DRLG_L2PlaceRndSet(VARCH11, 100);
	DRLG_L2PlaceRndSet(VARCH12, 100);
	DRLG_L2PlaceRndSet(VARCH13, 100);
	DRLG_L2PlaceRndSet(VARCH14, 100);
	DRLG_L2PlaceRndSet(VARCH15, 100);
	DRLG_L2PlaceRndSet(VARCH16, 100);
	DRLG_L2PlaceRndSet(VARCH17, 100);
	DRLG_L2PlaceRndSet(VARCH18, 100);
	DRLG_L2PlaceRndSet(VARCH19, 100);
	DRLG_L2PlaceRndSet(VARCH20, 100);
	DRLG_L2PlaceRndSet(VARCH21, 100);
	DRLG_L2PlaceRndSet(VARCH22, 100);
	DRLG_L2PlaceRndSet(VARCH23, 100);
	DRLG_L2PlaceRndSet(VARCH24, 100);
	DRLG_L2PlaceRndSet(VARCH25, 100);
	DRLG_L2PlaceRndSet(VARCH26, 100);
	DRLG_L2PlaceRndSet(VARCH27, 100);
	DRLG_L2PlaceRndSet(VARCH28, 100);
	DRLG_L2PlaceRndSet(VARCH29, 100);
	DRLG_L2PlaceRndSet(VARCH30, 100);
	DRLG_L2PlaceRndSet(VARCH31, 100);
	DRLG_L2PlaceRndSet(VARCH32, 100);
	DRLG_L2PlaceRndSet(HARCH1, 100);
	DRLG_L2PlaceRndSet(HARCH2, 100);
	DRLG_L2PlaceRndSet(HARCH3, 100);
	DRLG_L2PlaceRndSet(HARCH4, 100);
	DRLG_L2PlaceRndSet(HARCH5, 100);
	DRLG_L2PlaceRndSet(HARCH6, 100);
	DRLG_L2PlaceRndSet(HARCH7, 100);
	DRLG_L2PlaceRndSet(HARCH8, 100);
	DRLG_L2PlaceRndSet(HARCH9, 100);
	DRLG_L2PlaceRndSet(HARCH10, 100);
	DRLG_L2PlaceRndSet(HARCH11, 100);
	DRLG_L2PlaceRndSet(HARCH12, 100);
	DRLG_L2PlaceRndSet(HARCH13, 100);
	DRLG_L2PlaceRndSet(HARCH14, 100);
	DRLG_L2PlaceRndSet(HARCH15, 100);
	DRLG_L2PlaceRndSet(HARCH16, 100);
	DRLG_L2PlaceRndSet(HARCH17, 100);
	DRLG_L2PlaceRndSet(HARCH18, 100);
	DRLG_L2PlaceRndSet(HARCH19, 100);
	DRLG_L2PlaceRndSet(HARCH20, 100);
	DRLG_L2PlaceRndSet(HARCH21, 100);
	DRLG_L2PlaceRndSet(HARCH22, 100);
	DRLG_L2PlaceRndSet(HARCH23, 100);
	DRLG_L2PlaceRndSet(HARCH24, 100);
	DRLG_L2PlaceRndSet(HARCH25, 100);
	DRLG_L2PlaceRndSet(HARCH26, 100);
	DRLG_L2PlaceRndSet(HARCH27, 100);
	DRLG_L2PlaceRndSet(HARCH28, 100);
	DRLG_L2PlaceRndSet(HARCH29, 100);
	DRLG_L2PlaceRndSet(HARCH30, 100);
	DRLG_L2PlaceRndSet(HARCH31, 100);
	DRLG_L2PlaceRndSet(HARCH32, 100);
	DRLG_L2PlaceRndSet(HARCH33, 100);
	DRLG_L2PlaceRndSet(HARCH34, 100);
	DRLG_L2PlaceRndSet(HARCH35, 100);
	DRLG_L2PlaceRndSet(HARCH36, 100);
	DRLG_L2PlaceRndSet(HARCH37, 100);
	DRLG_L2PlaceRndSet(HARCH38, 100);
	DRLG_L2PlaceRndSet(HARCH39, 100);
	DRLG_L2PlaceRndSet(HARCH40, 100);
	DRLG_L2PlaceRndSet(CRUSHCOL, 99);
	DRLG_L2PlaceRndSet(RUINS1, 10);
	DRLG_L2PlaceRndSet(RUINS2, 10);
	DRLG_L2PlaceRndSet(RUINS3, 10);
	DRLG_L2PlaceRndSet(RUINS4, 10);
	DRLG_L2PlaceRndSet(RUINS5, 10);
	DRLG_L2PlaceRndSet(RUINS6, 10);
	DRLG_L2PlaceRndSet(RUINS7, 50);
	DRLG_L2PlaceRndSet(PANCREAS1, 1);
	DRLG_L2PlaceRndSet(PANCREAS2, 1);
	DRLG_L2PlaceRndSet(BIG1, 3);
	DRLG_L2PlaceRndSet(BIG2, 3);
	DRLG_L2PlaceRndSet(BIG3, 3);
	DRLG_L2PlaceRndSet(BIG4, 3);
	DRLG_L2PlaceRndSet(BIG5, 3);
	DRLG_L2PlaceRndSet(BIG6, 20);
	DRLG_L2PlaceRndSet(BIG7, 20);
	DRLG_L2PlaceRndSet(BIG8, 3);
	DRLG_L2PlaceRndSet(BIG9, 20);
	DRLG_L2PlaceRndSet(BIG10, 20);
	DRLG_L2Subs();
	DRLG_L2Shadows();
	v10 = 0;
	do
	{
		v11 = v10;
		v12 = 40;
		do
		{
			pdungeon[0][v11] = dungeon[0][v11];
			v11 += 40;
			--v12;
		}
		while ( v12 );
		++v10;
	}
	while ( v10 < 40 );
	DRLG_Init_Globals();
	DRLG_CheckQuests(nSx1, nSy1);
}
// 5B50D8: using guessed type int setloadflag_2;

bool __fastcall DRLG_L2PlaceMiniSet(unsigned char *miniset, int tmin, int tmax, int cx, int cy, bool setview, int ldir)
{
	int v7; // ebx
	int v8; // esi
	int v9; // edi
	int v10; // edx
	int v11; // eax
	int v13; // esi
	int v14; // ebx
	int v15; // ecx
	int v16; // eax
	int v18; // eax
	int v20; // edi
	signed int i; // eax
	int v22; // ecx
	unsigned char v23; // dl
	int v24; // eax
	int v25; // edi
	char *v26; // edx
	unsigned char v27; // bl
	bool result; // al
	unsigned char *v29; // [esp+Ch] [ebp-28h]
	int v30; // [esp+10h] [ebp-24h]
	int v31; // [esp+14h] [ebp-20h]
	int v32; // [esp+18h] [ebp-1Ch]
	signed int v33; // [esp+1Ch] [ebp-18h]
	int v34; // [esp+20h] [ebp-14h]
	int v35; // [esp+24h] [ebp-10h]
	int v36; // [esp+28h] [ebp-Ch]
	int max; // [esp+2Ch] [ebp-8h]
	//int v38; // [esp+30h] [ebp-4h]
	int v39; // [esp+30h] [ebp-4h]
	int tmaxa; // [esp+3Ch] [ebp+8h]

	v7 = miniset[1];
	v8 = tmin;
	v9 = *miniset;
	v29 = miniset;
	v10 = tmax - tmin;
	v34 = *miniset;
	v35 = miniset[1];
	if ( v10 )
	{
		v30 = v8 + random(0, v10);
	}
	else
	{
		v30 = 1;
	}
	v31 = 0;
	if ( v30 <= 0 )
	{
		v13 = ldir;
		v14 = 0; /* v38; check */
	}
	else
	{
		max = 40 - v9;
		v36 = 40 - v7;
		do
		{
			v11 = random(0, max);
			v13 = v11;
			v33 = 0;
			v14 = random(0, v36);
			v39 = v14;
			do
			{
				if ( v33 >= 200 )
					return 0;
				tmaxa = 1;
				if ( v13 >= nSx1 && v13 <= nSx2 && v14 >= nSy1 && v14 <= nSy2 )
					tmaxa = 0;
				if ( cx != -1 )
				{
					v15 = cx - v34;
					if ( v13 >= cx - v34 && v13 <= cx + 12 )
					{
						v16 = random(0, max);
						v13 = v16;
						tmaxa = 0;
						v39 = random(0, v36);
						v14 = v39;
					}
				}
				if ( cy != -1 && v14 >= cy - v35 && v14 <= cy + 12 )
				{
					v18 = random(0, max); /* cy - v35 */
					v13 = v18;
					tmaxa = 0;
					v39 = random(0, v36);
					v14 = v39;
				}
				v20 = 0;
				for ( i = 2; v20 < v35; ++v20 )
				{
					if ( tmaxa != 1 )
						break;
					v32 = 0;
					if ( v34 > 0 )
					{
						v22 = v14 + v20 + 40 * v13;
						do
						{
							if ( tmaxa != 1 )
								break;
							v23 = v29[i];
							if ( v23 && dungeon[0][v22] != v23 )
								tmaxa = 0;
							if ( dflags[0][v22] )
								tmaxa = 0;
							++i;
							++v32;
							v22 += 40;
						}
						while ( v32 < v34 );
					}
				}
				if ( !tmaxa && ++v13 == max )
				{
					v13 = 0;
					v39 = ++v14;
					if ( v14 == v36 )
					{
						v39 = 0;
						v14 = 0;
					}
				}
				++v33;
			}
			while ( !tmaxa );
			if ( v33 >= 200 )
				return 0;
			v24 = 0;
			for ( miniset = (unsigned char *)(v34 * v35 + 2); v24 < v35; ++v24 )
			{
				v25 = v34;
				if ( v34 > 0 )
				{
					v26 = &dungeon[v13][v24 + v14];
					do
					{
						v27 = v29[(_DWORD)miniset];
						if ( v27 )
							*v26 = v27;
						++miniset;
						v26 += 40;
						--v25;
					}
					while ( v25 );
					v14 = v39;
				}
			}
			++v31;
		}
		while ( v31 < v30 );
	}
	result = 1;
	if ( setview == 1 )
	{
		ViewX = 2 * v13 + 21;
		ViewY = 2 * v14 + 22;
	}
	if ( !ldir )
	{
		LvlViewX = 2 * v13 + 21;
		LvlViewY = 2 * v14 + 22;
	}
	if ( ldir == 6 )
	{
		LvlViewX = 2 * v13 + 21;
		LvlViewY = 2 * v14 + 22;
	}
	return result;
}
// 5276CC: using guessed type int nSx2;
// 5276D4: using guessed type int nSy2;
// 5CF320: using guessed type int LvlViewY;
// 5CF324: using guessed type int LvlViewX;

void __fastcall DRLG_L2PlaceRndSet(unsigned char *miniset, int rndper)
{
	unsigned char *v2; // ebx
	signed int v3; // esi
	signed int v4; // ecx
	int v5; // edx
	signed int v6; // edi
	signed int i; // edx
	signed int v8; // esi
	int v9; // eax
	unsigned char v10; // cl
	int v11; // edi
	_BYTE *v12; // ecx
	int v13; // esi
	int v14; // eax
	int v15; // eax
	signed int j; // edx
	signed int v17; // esi
	unsigned char *v18; // eax
	unsigned char v19; // cl
	int v20; // [esp+8h] [ebp-3Ch]
	unsigned char *v21; // [esp+10h] [ebp-34h]
	int v22; // [esp+14h] [ebp-30h]
	int v23; // [esp+18h] [ebp-2Ch]
	int v24; // [esp+1Ch] [ebp-28h]
	int v25; // [esp+20h] [ebp-24h]
	int v26; // [esp+24h] [ebp-20h]
	int v27; // [esp+28h] [ebp-1Ch]
	int v28; // [esp+2Ch] [ebp-18h]
	int v29; // [esp+30h] [ebp-14h]
	signed int v30; // [esp+34h] [ebp-10h]
	signed int v31; // [esp+38h] [ebp-Ch]
	int v32; // [esp+3Ch] [ebp-8h]
	signed int v33; // [esp+40h] [ebp-4h]

	v2 = miniset;
	v32 = 0;
	v20 = rndper;
	v3 = miniset[1];
	v4 = *miniset;
	v21 = v2;
	v30 = v4;
	v26 = 40 - v3;
	v31 = v3;
	if ( 40 - v3 > 0 )
	{
		v27 = 40 - v4;
		v23 = -v3;
		while ( 1 )
		{
			v5 = 0;
			v25 = 0;
			if ( v27 > 0 )
			{
				v29 = -v4;
				v22 = v4 * v3 + 2;
				v28 = 0;
				v24 = -40 * v4;
				do
				{
					v33 = 1;
					v6 = 2;
					if ( v5 >= nSx1 && v5 <= nSx2 && v32 >= nSy1 && v32 <= nSy2 )
						v33 = 0;
					for ( i = 0; i < v31; ++i )
					{
						if ( v33 != 1 )
							break;
						v8 = 0;
						if ( v30 > 0 )
						{
							v9 = v32 + i + v28;
							do
							{
								if ( v33 != 1 )
									break;
								v10 = v2[v6];
								if ( v10 && dungeon[0][v9] != v10 )
									v33 = 0;
								if ( dflags[0][v9] )
									v33 = 0;
								++v6;
								++v8;
								v9 += 40;
							}
							while ( v8 < v30 );
						}
					}
					v11 = v22;
					if ( v33 == 1 )
					{
						v12 = (_BYTE *)v31;
						v13 = v23;
						if ( v23 >= v32 + 2 * v31 )
						{
LABEL_34:
							if ( random(0, 100) < v20 )
							{
								for ( j = 0; j < v31; ++j )
								{
									v17 = v30;
									if ( v30 > 0 )
									{
										v18 = (unsigned char *)dungeon + j + v28 + v32;
										do
										{
											v19 = v2[v11];
											if ( v19 )
												*v18 = v19;
											++v11;
											v18 += 40;
											--v17;
										}
										while ( v17 );
									}
								}
							}
						}
						else
						{
							while ( v33 == 1 )
							{
								v12 = (_BYTE *)v30;
								v14 = v25 + 2 * v30;
								if ( v29 < v14 )
								{
									v15 = v14 - v29;
									v12 = (unsigned char *)dungeon + v24 + v13;
									do
									{
										if ( *v12 == v2[v22] )
											v33 = 0;
										v12 += 40;
										--v15;
									}
									while ( v15 );
									v2 = v21;
								}
								if ( ++v13 >= v32 + 2 * v31 )
								{
									if ( v33 != 1 )
										break;
									goto LABEL_34;
								}
							}
						}
					}
					v24 += 40;
					v28 += 40;
					v5 = v25 + 1;
					++v29;
					++v25;
				}
				while ( v25 < v27 );
			}
			++v32;
			++v23;
			if ( v32 >= v26 )
				break;
			v4 = v30;
			v3 = v31;
		}
	}
}
// 5276CC: using guessed type int nSx2;
// 5276D4: using guessed type int nSy2;

void __cdecl DRLG_L2Subs()
{
	signed int v0; // edi
	unsigned char v1; // bl
	int v2; // eax
	signed int v3; // edx
	int v4; // esi
	int i; // ebx
	int j; // eax
	signed int v7; // [esp+Ch] [ebp-10h]
	char *v8; // [esp+10h] [ebp-Ch]
	signed int v9; // [esp+14h] [ebp-8h]
	int v10; // [esp+18h] [ebp-4h]

	v0 = 3;
	v9 = -2;
	v7 = 3;
	do
	{
		v10 = 0;
		v8 = &dungeon[0][v9 + 2];
		do
		{
			if ( (v10 < nSx1 || v10 > nSx2) && (v0 - 3 < nSy1 || v0 - 3 > nSy2) && !random(0, 4) )
			{
				v1 = BTYPESL2[(unsigned char)*v8];
				if ( v1 )
				{
					v2 = random(0, 16);
					v3 = -1;
					while ( v2 >= 0 )
					{
						if ( ++v3 == 161 )
							v3 = 0;
						if ( v1 == BTYPESL2[v3] )
							--v2;
					}
					v4 = v9;
					for ( i = v0 - 1; v4 < i; ++v4 )
					{
						for ( j = v10 - 2; j < v10 + 2; ++j )
						{
							v0 = v7;
							if ( (unsigned char)dungeon[j][v4] == v3 )
							{
								v4 = v7;
								j = v10 + 2;
							}
						}
					}
					if ( v4 < v0 )
						*v8 = v3;
				}
			}
			++v10;
			v8 += 40;
		}
		while ( v10 < 40 );
		++v9;
		v7 = ++v0;
	}
	while ( v0 - 3 < 40 );
}
// 5276CC: using guessed type int nSx2;
// 5276D4: using guessed type int nSy2;

void __cdecl DRLG_L2Shadows()
{
	char *v0; // eax
	unsigned char *v1; // esi
	unsigned char v2; // dl
	signed int v3; // edi
	char v4; // cl
	char v5; // cl
	char v6; // cl
	char v7; // cl
	char v8; // cl
	signed int v9; // [esp+8h] [ebp-Ch]
	signed int v10; // [esp+Ch] [ebp-8h]
	unsigned char v11; // [esp+11h] [ebp-3h]
	unsigned char v12; // [esp+12h] [ebp-2h]
	unsigned char v13; // [esp+13h] [ebp-1h]

	v10 = 1;
	do
	{
		v9 = 39;
		v0 = &dungeon[0][v10 + 39];
		do
		{
			v1 = &SPATSL2[0].s1;
			v2 = BSTYPESL2[(unsigned char)v0[1]];
			v12 = BSTYPESL2[(unsigned char)*(v0 - 39)];
			v11 = BSTYPESL2[(unsigned char)*v0];
			v13 = BSTYPESL2[(unsigned char)*(v0 - 40)];
			do
			{
				if ( *(v1 - 1) == v2 )
				{
					v3 = 1;
					if ( *v1 && *v1 != v13 )
						v3 = 0;
					v4 = v1[1];
					if ( v4 && v4 != v11 )
						v3 = 0;
					v5 = v1[2];
					if ( v5 && v5 != v12 )
						v3 = 0;
					if ( v3 == 1 )
					{
						v6 = v1[3];
						if ( v6 )
							*(v0 - 40) = v6;
						v7 = v1[4];
						if ( v7 )
							*v0 = v7;
						v8 = v1[5];
						if ( v8 )
							*(v0 - 39) = v8;
					}
				}
				v1 += 7;
			}
			while ( (signed int)v1 < (signed int)&SPATSL2[2].s1 );
			v0 += 40;
			--v9;
		}
		while ( v9 );
		++v10;
	}
	while ( v10 < 40 );
}
// 48489A: using guessed type short word_48489A;

void __fastcall DRLG_L2SetRoom(int rx1, int ry1)
{
	int v2; // edi
	int v3; // esi
	int v4; // eax
	char v5; // bl
	int v6; // [esp+8h] [ebp-Ch]
	char *v7; // [esp+Ch] [ebp-8h]
	int v8; // [esp+10h] [ebp-4h]

	v8 = 0;
	v2 = (unsigned char)pSetPiece_2[2];
	v3 = (unsigned char)*pSetPiece_2;
	setpc_x = rx1;
	setpc_y = ry1;
	setpc_w = v3;
	setpc_h = v2;
	v7 = pSetPiece_2 + 4;
	if ( v2 > 0 )
	{
		do
		{
			if ( v3 > 0 )
			{
				v6 = v3;
				v4 = ry1 + v8 + 40 * rx1;
				do
				{
					v5 = *v7;
					if ( *v7 )
					{
						dflags[0][v4] |= DFLAG_EXPLORED;
						dungeon[0][v4] = v5;
					}
					else
					{
						dungeon[0][v4] = 3;
					}
					v7 += 2;
					v4 += 40;
					--v6;
				}
				while ( v6 );
			}
			++v8;
		}
		while ( v8 < v2 );
	}
}
// 5CF330: using guessed type int setpc_h;
// 5CF334: using guessed type int setpc_w;

void __cdecl L2TileFix()
{
	signed int v0; // edx
	char *v1; // eax
	signed int v2; // esi
	char v3; // cl

	v0 = 0;
	do
	{
		v1 = (char *)dungeon + v0;
		v2 = 40;
		do
		{
			v3 = *v1;
			if ( *v1 == 1 && v1[1] == 3 )
				v1[1] = 1;
			if ( v3 == 3 )
			{
				if ( v1[1] == 1 )
					v1[1] = 3;
				if ( v1[40] == 7 )
					v1[40] = 3;
			}
			if ( v3 == 2 && v1[40] == 3 )
				v1[40] = 2;
			if ( v3 == 11 && v1[40] == 14 )
				v1[40] = 16;
			v1 += 40;
			--v2;
		}
		while ( v2 );
		++v0;
	}
	while ( v0 < 40 );
}

bool __cdecl CreateDungeon()
{
	int v0; // esi
	int v1; // edx
	int v2; // ecx
	signed int v3; // esi
	char *v4; // eax
	signed int v5; // ebx
	_BYTE *v6; // ecx
	bool v7; // zf
	bool v8; // eax
	int v9; // edi
	int v10; // esi
	signed int v12; // [esp-4h] [ebp-20h]
	int nX1; // [esp+8h] [ebp-14h]
	int nY1; // [esp+Ch] [ebp-10h]
	int nX2; // [esp+10h] [ebp-Ch]
	int nY2; // [esp+14h] [ebp-8h]
	int nHd; // [esp+18h] [ebp-4h]

	v0 = 0;
	v1 = 0;
	v2 = 0;
	if ( currlevel == 5 )
	{
		if ( !quests[QTYPE_BLOOD]._qactive )
			goto LABEL_12;
		v1 = 20;
		v0 = 14;
	}
	else
	{
		if ( currlevel == 6 )
		{
			if ( !quests[QTYPE_BONE]._qactive )
				goto LABEL_12;
			v12 = 10;
		}
		else
		{
			if ( currlevel != 7 || !quests[QTYPE_BLIND]._qactive )
				goto LABEL_12;
			v12 = 15;
		}
		v0 = v12;
		v1 = v12;
	}
	v2 = 1;
LABEL_12:
	CreateRoom(2, 2, 39, 39, 0, 0, v2, v1, v0);
	while ( pHallList )
	{
		GetHall(&nX1, &nY1, &nX2, &nY2, &nHd);
		ConnectHall(nX1, nY1, nX2, nY2, nHd);
	}
	v3 = 0;
	do
	{
		v4 = &predungeon[-1][v3];
		v5 = 41;
		do
		{
			v6 = (unsigned char *)v4 + 40;
			if ( v4[40] == 67 )
				*v6 = 35;
			if ( *v6 == 66 )
				*v6 = 35;
			if ( *v6 == 69 )
				*v6 = 35;
			if ( *v6 == 65 )
				*v6 = 35;
			if ( *v6 == 44 )
			{
				v7 = *(v4 - 1) == 32;
				*v6 = 46;
				if ( v7 )
					*(v4 - 1) = 35;
				if ( *v4 == 32 )
					*v4 = 35;
				if ( v4[1] == 32 )
					v4[1] = 35;
				if ( v4[79] == 32 )
					v4[79] = 35;
				if ( v4[80] == 32 )
					v4[80] = 35;
				if ( v4[81] == 32 )
					v4[81] = 35;
				if ( v4[39] == 32 )
					v4[39] = 35;
				if ( v4[41] == 32 )
					v4[41] = 35;
			}
			--v5;
			v4 += 40;
		}
		while ( v5 );
		++v3;
	}
	while ( v3 <= 40 );
	v8 = DL2_FillVoids();
	if ( v8 )
	{
		v9 = 0;
		do
		{
			v10 = 0;
			do
				DoPatternCheck(v10++, v9);
			while ( v10 < 40 );
			++v9;
		}
		while ( v9 < 40 );
		v8 = 1;
	}
	return v8;
}

void __fastcall CreateRoom(int nX1, int nY1, int nX2, int nY2, int nRDest, int nHDir, int ForceHW, int nH, int nW)
{
	int v9; // esi
	int v10; // ebx
	int v11; // edx
	int v12; // eax
	int v13; // edx
	int v14; // edx
	int v15; // edi
	int v17; // esi
	int v18; // ebx
	int v19; // edx
	int v20; // ecx
	int v21; // eax
	int v23; // eax
	int v24; // eax
	int v26; // eax
	int *v27; // ecx
	int v28; // eax
	int v29; // eax
	int *v30; // ecx
	int v31; // eax
	int nX1a; // [esp+Ch] [ebp-30h]
	int v33; // [esp+10h] [ebp-2Ch]
	int v34; // [esp+14h] [ebp-28h]
	int v35; // [esp+18h] [ebp-24h]
	int v36; // [esp+1Ch] [ebp-20h]
	int v37; // [esp+20h] [ebp-1Ch]
	int nY1a; // [esp+24h] [ebp-18h]
	int v39; // [esp+28h] [ebp-14h]
	int v40; // [esp+2Ch] [ebp-10h]
	int v41; // [esp+30h] [ebp-Ch]
	int v42; // [esp+34h] [ebp-8h]
	int v43; // [esp+38h] [ebp-4h]
	int *ForceHWa; // [esp+54h] [ebp+18h]
	int *ForceHWb; // [esp+54h] [ebp+18h]

	v39 = nY1;
	v37 = nX1;
	if ( nRoomCnt < 80 )
	{
		v40 = nX2 - 2;
		nY1a = nY1 + 2;
		while ( 1 )
		{
			v9 = nX2 - v37;
			v10 = nY2 - v39;
			if ( nX2 - v37 < Area_Min || v10 < Area_Min )
				return;
			if ( v9 > Room_Max )
				break;
			nX1 = Room_Min;
			if ( v9 > Room_Min )
			{
				v11 = v9 - Room_Min;
				goto LABEL_7;
			}
			v41 = nX2 - v37;
LABEL_11:
			v13 = Room_Max;
			if ( v10 <= Room_Max )
			{
				if ( v10 <= nX1 )
				{
					v36 = nY2 - v39;
					goto LABEL_16;
				}
				v13 = nY2 - v39;
			}
			v14 = v13 - nX1;
			v36 = Room_Min + random(0, v14);
LABEL_16:
			if ( ForceHW == 1 )
			{
				v41 = nW;
				v36 = nH;
			}
			v15 = v37 + random(0, v9);
			v17 = v39 + random(0, v10);
			v18 = v15 + v41;
			v43 = v17 + v36;
			if ( v15 + v41 > nX2 )
			{
				v18 = nX2;
				v15 = nX2 - v41;
			}
			if ( v17 + v36 > nY2 )
			{
				v43 = nY2;
				v17 = nY2 - v36;
			}
			if ( v15 >= 38 )
				v15 = 38;
			if ( v17 >= 38 )
				v17 = 38;
			if ( v15 <= 1 )
				v15 = 1;
			if ( v17 <= 1 )
				v17 = 1;
			if ( v18 >= 38 )
				v18 = 38;
			if ( v43 >= 38 )
				v43 = 38;
			if ( v18 <= 1 )
				v18 = 1;
			if ( v43 <= 1 )
				v43 = 1;
			DefineRoom(v15, v17, v18, v43, ForceHW);
			if ( ForceHW == 1 )
			{
				nSx2 = v18;
				nSx1 = v15 + 2;
				nSy1 = v17 + 2;
				nSy2 = v43;
			}
			v19 = nRoomCnt;
			v20 = nRDest;
			v42 = nRoomCnt;
			RoomList[nRoomCnt].nRoomDest = nRDest;
			if ( nRDest )
			{
				if ( nHDir == 1 )
				{
					v21 = random(0, v18 - v15 - 2);
					nX1a = v21 + v15 + 1;
					v33 = v17;
					v23 = random(0, RoomList[nRDest].nRoomx2 - RoomList[nRDest].nRoomx1 - 2);
					v20 = 20 * nRDest;
					v34 = v23 + RoomList[nRDest].nRoomx1 + 1;
					v35 = RoomList[nRDest].nRoomy2;
				}
				if ( nHDir == 3 )
				{
					v24 = random(0, v18 - v15 - 2);
					nX1a = v24 + v15 + 1;
					v33 = v43;
					v26 = random(0, RoomList[nRDest].nRoomx2 - RoomList[nRDest].nRoomx1 - 2);
					v20 = 20 * nRDest;
					v34 = v26 + RoomList[nRDest].nRoomx1 + 1;
					v35 = RoomList[nRDest].nRoomy1;
				}
				if ( nHDir == 2 )
				{
					nX1a = v18;
					v33 = random(0, v43 - v17 - 2) + v17 + 1;
					v34 = RoomList[nRDest].nRoomx1;
					v27 = &RoomList[nRDest].nRoomy1;
					ForceHWa = v27;
					v28 = RoomList[nRDest].nRoomy2 - *v27;
					v29 = random(0, v28 - 2);
					v20 = *ForceHWa;
					v35 = v29 + *ForceHWa + 1;
				}
				if ( nHDir == 4 )
				{
					nX1a = v15;
					v33 = random(0, v43 - v17 - 2) + v17 + 1;
					v34 = RoomList[nRDest].nRoomx2;
					v30 = &RoomList[nRDest].nRoomy1;
					ForceHWb = v30;
					v31 = RoomList[nRDest].nRoomy2 - *v30;
					v35 = random(0, v31 - 2) + *ForceHWb + 1;
				}
				AddHall(nX1a, v33, v34, v35, nHDir);
				v19 = v42;
			}
			if ( v36 <= v41 )
			{
				CreateRoom(v37 + 2, nY1a, v18 - 2, v17 - 2, v19, 3, 0, 0, 0);
				CreateRoom(v15 + 2, v43 + 2, v40, nY2 - 2, v42, 1, 0, 0, 0);
				CreateRoom(v37 + 2, v17 + 2, v15 - 2, nY2 - 2, v42, 2, 0, 0, 0);
				nHDir = 4;
				nW = 0;
				nH = 0;
				ForceHW = 0;
				nRDest = v42;
				nY2 = v43 - 2;
				nX2 -= 2;
				v40 -= 2;
				v39 += 2;
				nY1a += 2;
				v37 = v18 + 2;
			}
			else
			{
				CreateRoom(v37 + 2, nY1a, v15 - 2, v43 - 2, v19, 2, 0, 0, 0);
				CreateRoom(v18 + 2, v17 + 2, v40, nY2 - 2, v42, 4, 0, 0, 0);
				CreateRoom(v37 + 2, v43 + 2, v18 - 2, nY2 - 2, v42, 1, 0, 0, 0);
				nW = 0;
				nH = 0;
				ForceHW = 0;
				nRDest = v42;
				nHDir = 3;
				nX2 -= 2;
				v40 -= 2;
				v39 += 2;
				nY1a += 2;
				nY2 = v17 - 2;
				v37 = v15 + 2;
			}
			if ( nRoomCnt >= 80 )
				return;
		}
		v11 = Room_Max - Room_Min;
LABEL_7:
		v12 = random(0, v11);
		nX1 = Room_Min;
		v41 = Room_Min + v12;
		goto LABEL_11;
	}
}
// 484858: using guessed type int Area_Min;
// 48485C: using guessed type int Room_Max;
// 484860: using guessed type int Room_Min;
// 5276CC: using guessed type int nSx2;
// 5276D4: using guessed type int nSy2;

void __fastcall DefineRoom(int nX1, int nY1, int nX2, int nY2, int ForceHW)
{
	int v5; // esi
	int v6; // edi
	int v7; // eax
	int i; // eax
	bool v9; // zf
	int v10; // ecx
	char *v11; // eax
	char *v12; // ebx
	int v13; // eax
	int v14; // [esp+10h] [ebp-4h]
	int v15; // [esp+10h] [ebp-4h]
	int nY2a; // [esp+20h] [ebp+Ch]
	char *ForceHWa; // [esp+24h] [ebp+10h]

	v5 = nX1;
	v6 = nX2;
	predungeon[v5][nY1] = 67;
	predungeon[v5][nY2] = 69;
	predungeon[v6][nY1] = 66;
	predungeon[v6][nY2] = 65;
	v7 = nRoomCnt + 1;
	nRoomCnt = v7;
	v7 *= 20;
	*(int *)((char *)&RoomList[0].nRoomx1 + v7) = nX1;
	*(int *)((char *)&RoomList[0].nRoomx2 + v7) = nX2;
	*(int *)((char *)&RoomList[0].nRoomy1 + v7) = nY1;
	*(int *)((char *)&RoomList[0].nRoomy2 + v7) = nY2;
	if ( ForceHW == 1 )
	{
		for ( i = nX1; i < nX2; ++i )
		{
			if ( i < nY2 )
			{
				ForceHWa = &dflags[i][nY1];
				v14 = nY2 - i;
				i = nY2;
				do
				{
					*ForceHWa |= DFLAG_EXPLORED;
					v9 = v14-- == 1;
					ForceHWa += 40;
				}
				while ( !v9 );
			}
		}
	}
	v10 = nX1 + 1;
	if ( v10 <= nX2 - 1 )
	{
		v15 = nX2 - v10;
		v11 = &predungeon[v10][nY2];
		do
		{
			v11[nY1 - nY2] = 35;
			*v11 = 35;
			v11 += 40;
			--v15;
		}
		while ( v15 );
	}
	nY2a = nY2 - 1;
	while ( ++nY1 <= nY2a )
	{
		predungeon[v5][nY1] = 35;
		predungeon[v6][nY1] = 35;
		if ( v10 < nX2 )
		{
			v12 = &predungeon[v10][nY1];
			v13 = nX2 - v10;
			do
			{
				*v12 = 46;
				v12 += 40;
				--v13;
			}
			while ( v13 );
		}
	}
}

void __fastcall AddHall(int nX1, int nY1, int nX2, int nY2, int nHd)
{
	int v5; // edi
	int v6; // esi
	HALLNODE *v7; // eax
	HALLNODE *i; // ecx

	v5 = nX1;
	v6 = nY1;
	if ( pHallList )
	{
		v7 = (HALLNODE *)DiabloAllocPtr(24);
		v7->pNext = 0;
		v7->nHallx2 = nX2;
		v7->nHally2 = nY2;
		v7->nHallx1 = v5;
		v7->nHally1 = v6;
		v7->nHalldir = nHd;
		for ( i = pHallList; i->pNext; i = i->pNext )
			;
		i->pNext = v7;
	}
	else
	{
		pHallList = (HALLNODE *)DiabloAllocPtr(24);
		pHallList->nHallx1 = v5;
		pHallList->nHally1 = v6;
		pHallList->nHallx2 = nX2;
		pHallList->nHally2 = nY2;
		pHallList->nHalldir = nHd;
		pHallList->pNext = 0;
	}
}

void __fastcall GetHall(int *nX1, int *nY1, int *nX2, int *nY2, int *nHd)
{
	HALLNODE *v5; // esi
	HALLNODE *v6; // ecx

	v5 = pHallList->pNext;
	*nX1 = pHallList->nHallx1;
	*nY1 = pHallList->nHally1;
	*nX2 = pHallList->nHallx2;
	*nY2 = pHallList->nHally2;
	*nHd = pHallList->nHalldir;
	v6 = pHallList;
	pHallList = 0;
	mem_free_dbg(v6);
	pHallList = v5;
}

void __fastcall ConnectHall(int nX1, int nY1, int nX2, int nY2, int nHd)
{
	int v5; // edi
	signed int v6; // esi
	int v7; // eax
	int v9; // edi
	int v10; // ebx
	int v11; // ecx
	char v12; // al
	int v13; // eax
	int v14; // ecx
	char *v15; // ebx
	int v16; // ecx
	int v17; // edx
	int v18; // ecx
	int v19; // edx
	int v20; // eax
	//int v21; // ST04_4
	int v23; // ebx
	int v24; // ebx
	bool v25; // zf
	signed int v26; // [esp-4h] [ebp-34h]
	signed int v27; // [esp-4h] [ebp-34h]
	signed int v28; // [esp-4h] [ebp-34h]
	signed int v29; // [esp-4h] [ebp-34h]
	int v30; // [esp+Ch] [ebp-24h]
	int v31; // [esp+10h] [ebp-20h]
	int v32; // [esp+14h] [ebp-1Ch]
	signed int v33; // [esp+18h] [ebp-18h]
	signed int v34; // [esp+1Ch] [ebp-14h]
	signed int v35; // [esp+20h] [ebp-10h]
	int v36; // [esp+24h] [ebp-Ch]
	char *v37; // [esp+28h] [ebp-8h]
	signed int nY; // [esp+2Ch] [ebp-4h]
	int nX2a; // [esp+38h] [ebp+8h]
	int nY2a; // [esp+3Ch] [ebp+Ch]
	int nHda; // [esp+40h] [ebp+10h]

	v34 = 0;
	v5 = nY1;
	v6 = nX1;
	nY = nY1;
	v7 = random(0, 100);
	v33 = v7;
	v32 = random(0, 100);
	v31 = v6;
	v30 = v5;
	CreateDoorType(v6, v5);
	CreateDoorType(nX2, nY2);
	abs(nX2 - v6);
	abs(nY2 - v5);
	v9 = nHd;
	v10 = nX2 - Dir_Xadd[nHd];
	v11 = nY2 - Dir_Yadd[nHd];
	nHda = 0;
	nY2a = v11;
	nX2a = v10;
	predungeon[v10][v11] = 44;
	v37 = &predungeon[v6][nY];
	do
	{
		if ( v6 >= 38 && v9 == 2 )
			v9 = 4;
		if ( nY >= 38 && v9 == 3 )
			v9 = 1;
		if ( v6 <= 1 && v9 == 4 )
			v9 = 2;
		if ( nY <= 1 && v9 == 1 )
			v9 = 3;
		v12 = *v37;
		if ( *v37 == 67 && (v9 == 1 || v9 == 4) )
			v9 = 2;
		if ( v12 == 66 && (v9 == 1 || v9 == 2) )
			v9 = 3;
		if ( v12 == 69 && (v9 == 4 || v9 == 3) )
			v9 = 1;
		if ( v12 == 65 && (v9 == 2 || v9 == 3) )
			v9 = 4;
		v13 = Dir_Xadd[v9];
		v14 = Dir_Yadd[v9];
		nY += v14;
		v6 += v13;
		v15 = &predungeon[v6][nY];
		v37 = v15;
		if ( *v15 == 32 )
		{
			if ( nHda )
			{
				CreateDoorType(v6 - v13, nY - v14);
			}
			else
			{
				if ( v33 < 50 )
				{
					if ( v9 == 1 || v9 == 3 )
					{
						v17 = nY;
						v16 = v6 - 1;
					}
					else
					{
						v16 = v6;
						v17 = nY - 1;
					}
					PlaceHallExt(v16, v17);
				}
				if ( v32 < 50 )
				{
					if ( v9 == 1 || v9 == 3 )
					{
						v19 = nY;
						v18 = v6 + 1;
					}
					else
					{
						v18 = v6;
						v19 = nY + 1;
					}
					PlaceHallExt(v18, v19);
				}
			}
			nHda = 0;
			*v15 = 44;
		}
		else
		{
			if ( !nHda && *v15 == 35 )
				CreateDoorType(v6, nY);
			if ( *v15 != 44 )
				nHda = 1;
		}
		v36 = abs(nX2a - v6);
		v20 = abs(nY2a - nY);
		//v22 = v21;
		v35 = v20;
		if ( v36 <= v20 )
		{
			v24 = 5 * v20;
			if ( 5 * v20 > 80 )
				v24 = 80;
			if ( random(0, 100) < v24 )
			{
				if ( nY2a <= nY || nY >= 40 )
				{
					v9 = 1;
					goto LABEL_67;
				}
				v26 = 3;
				goto LABEL_58;
			}
		}
		else
		{
			v23 = 2 * v36;
			if ( 2 * v36 > 30 )
				v23 = 30;
			if ( random(0, 100) < v23 )
			{
				if ( nX2a <= v6 || v6 >= 40 )
					v26 = 4;
				else
					v26 = 2;
LABEL_58:
				v9 = v26;
				goto LABEL_67;
			}
		}
LABEL_67:
		if ( v35 < 10 && v6 == nX2a && (v9 == 2 || v9 == 4) )
		{
			if ( nY2a <= nY || nY >= 40 )
				v9 = 1;
			else
				v9 = 3;
		}
		if ( v36 < 10 && nY == nY2a && (v9 == 1 || v9 == 3) )
		{
			if ( nX2a <= v6 || v6 >= 40 )
				v27 = 4;
			else
				v27 = 2;
			v9 = v27;
		}
		if ( v35 == 1 )
		{
			v25 = v36 == 1;
			if ( v36 <= 1 )
				goto LABEL_94;
			if ( v9 == 1 || v9 == 3 )
			{
				if ( nX2a <= v6 || v6 >= 40 )
					v28 = 4;
				else
					v28 = 2;
				v9 = v28;
			}
		}
		v25 = v36 == 1;
LABEL_94:
		if ( v25 )
		{
			if ( v35 <= 1 || v9 != 2 && v9 != 4 )
				goto LABEL_109;
			if ( nY2a > nY && v6 < 40 )
				goto LABEL_100;
			v9 = 1;
		}
		if ( !v36 && *v37 != 32 && (v9 == 2 || v9 == 4) )
		{
			if ( nX2a <= v31 || v6 >= 40 )
			{
				v9 = 1;
				goto LABEL_109;
			}
LABEL_100:
			v9 = 3;
		}
LABEL_109:
		if ( !v35 && *v37 != 32 && (v9 == 1 || v9 == 3) )
		{
			if ( nY2a <= v30 || nY >= 40 )
				v29 = 4;
			else
				v29 = 2;
			v9 = v29;
		}
		if ( v6 == nX2a && nY == nY2a )
			v34 = 1;
	}
	while ( !v34 );
}

void __fastcall CreateDoorType(int nX, int nY)
{
	int v2; // eax
	signed int v3; // esi
	char *v4; // ecx
	char v5; // al

	v2 = nX;
	v3 = 0;
	v4 = &predungeon[nX][nY];
	if ( *(v4 - 40) == 68 )
		v3 = 1;
	if ( predungeon[v2 + 1][nY] == 68 )
		v3 = 1;
	if ( *(v4 - 1) == 68 )
		v3 = 1;
	if ( predungeon[v2][nY + 1] == 68 )
		v3 = 1;
	v5 = *v4;
	if ( *v4 == 66 || v5 == 67 || v5 == 65 || v5 == 69 )
		v3 = 1;
	if ( !v3 )
		*v4 = 68;
}

void __fastcall PlaceHallExt(int nX, int nY)
{
	char *v2; // eax

	v2 = &predungeon[nX][nY];
	if ( *v2 == 32 )
		*v2 = 44;
}

void __fastcall DoPatternCheck(int i, int j)
{
	int v2; // edx
	signed int v3; // eax
	signed int v4; // ebp
	int v5; // esi
	int v6; // ecx
	bool v7; // zf
	char v8; // bl
	bool v9; // zf
	char v10; // bl
	int *v11; // [esp+0h] [ebp-10h]
	int v12; // [esp+4h] [ebp-Ch]
	int v13; // [esp+8h] [ebp-8h]
	int v14; // [esp+Ch] [ebp-4h]

	v13 = j;
	v14 = i;
	if ( Patterns[0][4] != 255 )
	{
		v12 = 0;
		v2 = i - 1;
		v11 = &Patterns[0][4];
		do
		{
			v3 = v2;
			v4 = 254;
			v5 = v13 - 1;
			v6 = 0;
			while ( v4 == 254 )
			{
				v4 = 255;
				if ( v6 == 3 || v6 == 6 )
				{
					++v5;
					v3 = v2;
				}
				if ( v3 < 0 || v3 >= 40 || v5 < 0 || v5 >= 40 )
				{
LABEL_26:
					v4 = 254;
				}
				else
				{
					switch ( Patterns[0][v6 + v12] )
					{
						case 0:
							goto LABEL_26;
						case 1:
							v7 = predungeon[v3][v5] == 35;
							goto LABEL_25;
						case 2:
							v7 = predungeon[v3][v5] == 46;
							goto LABEL_25;
						case 3:
							v7 = predungeon[v3][v5] == 68;
							goto LABEL_25;
						case 4:
							v7 = predungeon[v3][v5] == 32;
							goto LABEL_25;
						case 5:
							v8 = predungeon[v3][v5];
							v9 = v8 == 68;
							goto LABEL_23;
						case 6:
							v10 = predungeon[v3][v5];
							if ( v10 == 68 )
								goto LABEL_26;
							v7 = v10 == 35;
							goto LABEL_25;
						case 7:
							v8 = predungeon[v3][v5];
							v9 = v8 == 32;
							goto LABEL_23;
						case 8:
							v8 = predungeon[v3][v5];
							if ( v8 == 68 )
								goto LABEL_26;
							v9 = v8 == 35;
LABEL_23:
							if ( v9 )
								goto LABEL_26;
							v7 = v8 == 46;
LABEL_25:
							if ( v7 )
								goto LABEL_26;
							break;
						default:
							break;
					}
				}
				++v3;
				if ( ++v6 >= 9 )
				{
					if ( v4 == 254 )
						dungeon[v14][v13] = *((_BYTE *)v11 + 20);
					break;
				}
			}
			v11 += 10;
			v12 += 10;
		}
		while ( *v11 != 255 );
	}
}

bool __cdecl DL2_FillVoids()
{
	int i; // eax
	int v2; // eax
	int v4; // edi
	int v5; // eax
	int v6; // ebx
	int v7; // eax
	int v8; // ecx
	char v9; // dl
	bool v10; // eax
	int v11; // esi
	signed int v12; // ecx
	signed int v13; // edi
	signed int v14; // edx
	signed int v15; // eax
	int v16; // ebx
	char *v17; // eax
	signed int v18; // edx
	int k; // eax
	int v20; // ebx
	int v21; // ebx
	char *v22; // eax
	int v23; // ebx
	signed int v24; // edx
	int v25; // eax
	int v26; // esi
	int v27; // edx
	int v28; // esi
	int v29; // edx
	int v30; // edx
	signed int v31; // ebx
	int v32; // edi
	int v33; // ecx
	char *v34; // eax
	int v35; // edi
	int v36; // edx
	signed int v37; // ecx
	signed int v38; // eax
	int v39; // edx
	int v40; // edx
	int v41; // edx
	signed int v42; // ebx
	int j; // edi
	int v44; // ecx
	char *v45; // eax
	int v46; // edi
	int v47; // [esp-4h] [ebp-30h]
	signed int v48; // [esp+Ch] [ebp-20h]
	signed int y1f; // [esp+10h] [ebp-1Ch]
	signed int y2f; // [esp+14h] [ebp-18h]
	signed int x2f; // [esp+18h] [ebp-14h]
	signed int x1f; // [esp+1Ch] [ebp-10h]
	int x2; // [esp+20h] [ebp-Ch]
	int x2a; // [esp+20h] [ebp-Ch]
	int y1; // [esp+24h] [ebp-8h]
	int y1a; // [esp+24h] [ebp-8h]
	int y1b; // [esp+24h] [ebp-8h]
	int y2; // [esp+28h] [ebp-4h]
	int y2a; // [esp+28h] [ebp-4h]
	int y2b; // [esp+28h] [ebp-4h]

	v48 = 0;
	for ( i = DL2_NumNoChar(); i > 700 && v48 < 100; i = DL2_NumNoChar() )
	{
		v2 = random(0, 38);
		v4 = v2 + 1;
		v5 = random(0, 38);
		v6 = v5 + 1;
		v7 = v5 + 1 + 40 * v4;
		if ( predungeon[0][v7] != 35 )
			continue;
		y2f = 0;
		y1f = 0;
		x2f = 0;
		x1f = 0;
		v8 = predungeon[-1][v7]; // *((unsigned char *)&VR1 + v7);
		if ( (_BYTE)v8 == 32 && predungeon[1][v7] == 46 )
		{
			if ( predungeon[0][v7 + 39] != 46
			  || predungeon[1][v7 + 1] != 46
			  || predungeon[-1][v7 - 1] != 32 // *((_BYTE *)&HR3 + v7 + 3) != 32
			  || predungeon[-1][v7 + 1] != 32 ) // *((_BYTE *)&VR1 + v7 + 1) != 32 )
			{
				goto LABEL_34;
			}
			y1f = 1;
LABEL_32:
			x1f = 1;
LABEL_33:
			y2f = 1;
			goto LABEL_34;
		}
		if ( predungeon[1][v7] == 32 && (_BYTE)v8 == 46 )
		{
			if ( predungeon[-1][v7 - 1] != 46 // *((_BYTE *)&HR3 + v7 + 3) != 46
			  || predungeon[-1][v7 + 1] != 46 // *((_BYTE *)&VR1 + v7 + 1) != 46
			  || predungeon[0][v7 + 39] != 32
			  || predungeon[1][v7 + 1] != 32 )
			{
				goto LABEL_34;
			}
			y1f = 1;
			x2f = 1;
			goto LABEL_33;
		}
		v9 = predungeon[0][v7 - 1]; /* *((_BYTE *)&nRoomCnt + v7 + 3); */
		if ( v9 != 32 || predungeon[0][v7 + 1] != 46 )
		{
			if ( predungeon[0][v7 + 1] != 32
			  || v9 != 46
			  || predungeon[-1][v7 - 1] != 46 // *((_BYTE *)&HR3 + v7 + 3) != 46
			  || predungeon[0][v7 + 39] != 46
			  || predungeon[-1][v7 + 1] != 32
			  || predungeon[1][v7 + 1] != 32 )
			{
				goto LABEL_34;
			}
			x2f = 1;
			goto LABEL_32;
		}
		if ( predungeon[-1][v7 + 1] == 46
		  && predungeon[1][v7 + 1] == 46
		  && predungeon[-1][v7 - 1] == 32 // *((_BYTE *)&HR3 + v7 + 3) == 32
		  && predungeon[0][v7 + 39] == 32 )
		{
			x2f = 1;
			x1f = 1;
			y1f = 1;
			v10 = DL2_Cont(1, 1, 1, 0);
			goto LABEL_35;
		}
LABEL_34:
		v10 = DL2_Cont(x1f, y1f, x2f, y2f);
LABEL_35:
		if ( v10 )
		{
			v11 = v4 - 1;
			if ( !x1f )
				v11 = v4;
			v12 = x2f;
			if ( x2f )
				++v4;
			x2 = v4;
			v13 = y1f;
			if ( y1f )
				y1 = v6 - 1;
			else
				y1 = v6;
			v14 = y2f;
			if ( y2f )
				++v6;
			v15 = x1f;
			y2 = v6;
			if ( x1f )
			{
				if ( x2f )
				{
					if ( y1f )
					{
						if ( y2f )
							goto LABEL_177;
						v37 = x1f;
						v38 = x2f;
						v39 = x2;
						while ( v37 || v38 )
						{
							if ( !v11 )
								v37 = 0;
							if ( v39 == 39 )
								v38 = 0;
							if ( v39 - v11 >= 14 )
							{
								v37 = 0;
								v38 = 0;
							}
							if ( v37 )
								--v11;
							if ( v38 )
								++v39;
							if ( predungeon[v11][y1] != 32 )
								v37 = 0;
							if ( predungeon[v39][y1] != 32 )
								v38 = 0;
						}
						v28 = v11 + 2;
						v40 = v39 - 2;
						x2a = v40;
						v41 = v40 - v28;
						if ( v41 <= 5 )
							goto LABEL_177;
						v42 = y1f;
						for ( j = y1; ; --j )
						{
							if ( !j )
								v42 = 0;
							if ( y2 - j >= 12 )
								v42 = 0;
							if ( v28 <= x2a )
							{
								v44 = v41 + 1;
								v45 = &predungeon[v28][j];
								do
								{
									if ( *v45 != 32 )
										v42 = 0;
									v45 += 40;
									--v44;
								}
								while ( v44 );
							}
							if ( !v42 )
								break;
						}
						v46 = j + 2;
						if ( y2 - v46 <= 5 )
							goto LABEL_177;
						DL2_DrawRoom(v28, v46, x2a, y2);
						v36 = v46;
						v47 = y2;
					}
					else
					{
						v27 = x2;
						while ( v15 || v12 )
						{
							if ( !v11 )
								v15 = 0;
							if ( v27 == 39 )
								v12 = 0;
							if ( v27 - v11 >= 14 )
							{
								v15 = 0;
								v12 = 0;
							}
							if ( v15 )
								--v11;
							if ( v12 )
								++v27;
							if ( predungeon[v11][v6] != 32 )
								v15 = 0;
							if ( predungeon[v27][v6] != 32 )
								v12 = 0;
						}
						v28 = v11 + 2;
						v29 = v27 - 2;
						x2a = v29;
						v30 = v29 - v28;
						if ( v30 <= 5 )
							goto LABEL_177;
						v31 = y2f;
						v32 = y2;
						if ( y2f )
						{
							while ( 1 )
							{
								if ( v32 == 39 )
									v31 = 0;
								if ( v32 - y1 >= 12 )
									v31 = 0;
								if ( v28 <= x2a )
								{
									v33 = v30 + 1;
									v34 = &predungeon[v28][v32];
									do
									{
										if ( *v34 != 32 )
											v31 = 0;
										v34 += 40;
										--v33;
									}
									while ( v33 );
								}
								if ( !v31 )
									break;
								++v32;
							}
						}
						v35 = v32 - 2;
						if ( v35 - y1 <= 5 )
							goto LABEL_177;
						DL2_DrawRoom(v28, y1, x2a, v35);
						v36 = y1;
						v47 = v35;
					}
					DL2_KnockWalls(v28, v36, x2a, v47);
				}
				else
				{
					v21 = y1;
					while ( v13 || v14 )
					{
						if ( !v21 )
							v13 = 0;
						if ( y2 == 39 )
							v14 = 0;
						if ( y2 - v21 >= 14 )
						{
							v13 = 0;
							v14 = 0;
						}
						if ( v13 )
							--v21;
						if ( v14 )
							++y2;
						v22 = predungeon[v11];
						if ( v22[v21] != 32 )
							v13 = 0;
						if ( v22[y2] != 32 )
							v14 = 0;
					}
					y2b = y2 - 2;
					v23 = v21 + 2;
					y1b = v23;
					if ( y2b - v23 > 5 )
					{
						v24 = x1f;
						while ( 1 )
						{
							if ( !v11 )
								v24 = 0;
							if ( x2 - v11 >= 12 )
								v24 = 0;
							v25 = v23;
							if ( v23 <= y2b )
							{
								do
								{
									if ( predungeon[v11][v25] != 32 )
										v24 = 0;
									++v25;
								}
								while ( v25 <= y2b );
								v23 = y1b;
							}
							if ( !v24 )
								break;
							--v11;
						}
						v26 = v11 + 2;
						if ( x2 - v26 > 5 )
						{
							DL2_DrawRoom(v26, v23, x2, y2b);
							DL2_KnockWalls(v26, v23, x2, y2b);
						}
					}
				}
			}
			else
			{
				v16 = x2;
				while ( v13 || v14 )
				{
					if ( !y1 )
						v13 = 0;
					if ( y2 == 39 )
						v14 = 0;
					if ( y2 - y1 >= 14 )
					{
						v13 = 0;
						v14 = 0;
					}
					if ( v13 )
						--y1;
					if ( v14 )
						++y2;
					v17 = predungeon[x2];
					if ( v17[y1] != 32 )
						v13 = 0;
					if ( v17[y2] != 32 )
						v14 = 0;
				}
				y2a = y2 - 2;
				y1a = y1 + 2;
				if ( y2a - y1a > 5 )
				{
					v18 = x2f;
					if ( x2f )
					{
						while ( 1 )
						{
							if ( v16 == 39 )
								v18 = 0;
							if ( v16 - v11 >= 12 )
								v18 = 0;
							for ( k = y1a; k <= y2a; ++k )
							{
								if ( predungeon[v16][k] != 32 )
									v18 = 0;
							}
							if ( !v18 )
								break;
							++v16;
						}
					}
					v20 = v16 - 2;
					if ( v20 - v11 > 5 )
					{
						DL2_DrawRoom(v11, y1a, v20, y2a);
						DL2_KnockWalls(v11, y1a, v20, y2a);
					}
				}
			}
		}
LABEL_177:
		++v48;
	}
	return DL2_NumNoChar() <= 700;
}

bool __fastcall DL2_Cont(bool x1f, bool y1f, bool x2f, bool y2f)
{
	bool v4; // zf

	if ( x1f && x2f )
	{
		if ( !y1f )
			goto LABEL_16;
		if ( y2f )
			return 0;
		if ( !y1f )
		{
LABEL_16:
			v4 = y2f == 0;
			goto LABEL_11;
		}
		return 1;
	}
	if ( !y1f || !y2f )
		return 0;
	if ( x1f )
		return 1;
	v4 = x2f == 0;
LABEL_11:
	if ( !v4 )
		return 1;
	return 0;
}

int __cdecl DL2_NumNoChar()
{
	int result; // eax
	signed int v1; // edx
	_BYTE *v2; // ecx
	signed int v3; // esi

	result = 0;
	v1 = 0;
	do
	{
		v2 = (unsigned char *)predungeon + v1;
		v3 = 40;
		do
		{
			if ( *v2 == 32 )
				++result;
			v2 += 40;
			--v3;
		}
		while ( v3 );
		++v1;
	}
	while ( v1 < 40 );
	return result;
}

void __fastcall DL2_DrawRoom(int x1, int y1, int x2, int y2)
{
	int v4; // ebx
	char *v5; // edx
	int v6; // esi
	int i; // esi
	char *v8; // esi
	int v9; // eax
	int v10; // [esp+Ch] [ebp-4h]

	v4 = y1;
	v10 = y1;
	while ( v4 <= y2 )
	{
		if ( x1 <= x2 )
		{
			v5 = &predungeon[x1][v4];
			v6 = x2 - x1 + 1;
			do
			{
				*v5 = 46;
				v5 += 40;
				--v6;
			}
			while ( v6 );
		}
		++v4;
	}
	for ( i = v10; i <= y2; ++i )
	{
		predungeon[x1][i] = 35;
		predungeon[x2][i] = 35;
	}
	if ( x1 <= x2 )
	{
		v8 = &predungeon[x1][y2];
		v9 = x2 - x1 + 1;
		do
		{
			v8[v10 - y2] = 35;
			*v8 = 35;
			v8 += 40;
			--v9;
		}
		while ( v9 );
	}
}

void __fastcall DL2_KnockWalls(int x1, int y1, int x2, int y2)
{
	int v4; // esi
	char *v5; // ebx
	char *v6; // eax
	int v7; // edi
	int v8; // eax
	int v9; // ecx
	char *v10; // edx
	char *v11; // esi

	v4 = x1 + 1;
	if ( x1 + 1 < x2 )
	{
		v5 = &predungeon[v4][y2 + 1];
		v6 = &predungeon[v4][y1 - 1]; // (char *)&nRoomCnt + 40 * v4 + y1 + 3; /* check */
		v7 = x2 - v4;
		do
		{
			if ( *v6 == 46 && v6[2] == 46 )
				v6[1] = 46;
			if ( v6[y2 - y1] == 46 && *v5 == 46 )
				*(v5 - 1) = 46;
			if ( *v6 == 68 )
				*v6 = 46;
			if ( *v5 == 68 )
				*v5 = 46;
			v6 += 40;
			v5 += 40;
			--v7;
		}
		while ( v7 );
	}
	v8 = y1 + 1;
	if ( y1 + 1 < y2 )
	{
		v9 = x1;
		v10 = predungeon[x2 + 1];
		do
		{
			v11 = &predungeon[v9 - 1][v8]; // (char *)&VR1 + v9 * 40 + v8;
			if ( *v11 == 46 && predungeon[v9 + 1][v8] == 46 )
				predungeon[v9][v8] = 46;
			if ( v10[v8 - 80] == 46 && v10[v8] == 46 )
				v10[v8 - 40] = 46;
			if ( *v11 == 68 )
				*v11 = 46;
			if ( v10[v8] == 68 )
				v10[v8] = 46;
			++v8;
		}
		while ( v8 < y2 );
	}
}

void __cdecl DRLG_L2FloodTVal()
{
	int v0; // ebx
	int v1; // esi
	char *v2; // edi
	_BYTE *v3; // [esp+Ch] [ebp-Ch]
	signed int x; // [esp+10h] [ebp-8h]
	signed int i; // [esp+14h] [ebp-4h]

	v0 = 16;
	v1 = 0;
	do
	{
		i = 0;
		x = 16;
		v2 = &dung_map[16][v0];
		v3 = (unsigned char *)dungeon + v1;
		do
		{
			if ( *v3 == 3 && !*v2 )
			{
				DRLG_L2FTVR(i, v1, x, v0, 0);
				++TransVal;
			}
			x += 2;
			v3 += 40;
			v2 += 224;
			++i;
		}
		while ( i < 40 );
		v0 += 2;
		++v1;
	}
	while ( v1 < 40 );
}
// 5A5590: using guessed type char TransVal;

void __fastcall DRLG_L2FTVR(int i, int j, int x, int y, int d)
{
	int v5; // ebx
	int v6; // esi
	int v7; // edi
	int v8; // edx
	int v9; // ecx
	int v10; // ebx
	int v11; // eax
	int v12; // edi
	char v13; // al
	char v14; // al
	int v15; // ecx
	int v16; // ecx
	int v17; // ecx
	int v18; // ecx
	int v19; // [esp+Ch] [ebp-14h]
	int k; // [esp+10h] [ebp-10h]
	int v21; // [esp+14h] [ebp-Ch]
	int ja; // [esp+18h] [ebp-8h]
	int ia; // [esp+1Ch] [ebp-4h]
	int ya; // [esp+2Ch] [ebp+Ch]

	v5 = x;
	v6 = y;
	v7 = j;
	v8 = i;
	v9 = 112 * x + y;
	ja = v7;
	v21 = v8;
	if ( !dung_map[0][v9] )
	{
		v19 = x;
		ia = v8 - 1;
		v10 = x - 2;
		v11 = 40 * v8;
		ya = v7 - 1;
		v12 = v6 - 2;
		for ( k = 40 * v8; dungeon[0][v11 + ja] == 3; v11 = k )
		{
			v13 = TransVal;
			dung_map[0][v9] = TransVal;
			dung_map[1][v9] = v13;
			dung_map[0][v9 + 1] = v13;
			dung_map[1][v9 + 1] = v13;
			DRLG_L2FTVR(ia + 2, ja, v10 + 4, v6, 1);
			DRLG_L2FTVR(ia, ja, v10, v6, 2);
			DRLG_L2FTVR(v21, ya + 2, x, v12 + 4, 3);
			DRLG_L2FTVR(v21, ya, x, v12, 4);
			DRLG_L2FTVR(ia, ya, v10, v12, 5);
			DRLG_L2FTVR(ia + 2, ya, v10 + 4, v12, 6);
			DRLG_L2FTVR(ia, ya + 2, v10, v12 + 4, 7);
			v19 += 2;
			k += 40;
			d = 8;
			x += 2;
			v6 += 2;
			v12 += 2;
			v10 += 2;
			++ja;
			++ya;
			++v21;
			++ia;
			v9 = v19 * 112 + v6;
			if ( dung_map[v19][v6] )
				break;
		}
		v5 = x;
	}
	v14 = TransVal;
	if ( d == 1 )
	{
		v15 = v6 + 112 * v5;
		dung_map[0][v15] = TransVal;
		dung_map[0][v15 + 1] = v14;
	}
	if ( d == 2 )
	{
		v16 = v6 + 112 * v5;
		dung_map[1][v16] = v14;
		dung_map[1][v16 + 1] = v14;
	}
	if ( d == 3 )
	{
		v17 = v6 + 112 * v5;
		dung_map[0][v17] = v14;
		dung_map[1][v17] = v14;
	}
	if ( d == 4 )
	{
		v18 = v6 + 112 * v5;
		dung_map[0][v18 + 1] = v14;
		dung_map[1][v18 + 1] = v14;
	}
	if ( d == 5 )
		dung_map[v5 + 1][v6 + 1] = v14;
	if ( d == 6 )
		dung_map[v5][v6 + 1] = v14;
	if ( d == 7 )
		dung_map[v5 + 1][v6] = v14;
	if ( d == 8 )
		dung_map[v5][v6] = v14;
}
// 5A5590: using guessed type char TransVal;

void __cdecl DRLG_L2TransFix()
{
	signed int v0; // esi
	char *v1; // eax
	char *v2; // ecx
	signed int v3; // edi
	char v4; // bl
	char v5; // dl
	char v6; // dl
	char v7; // dl
	char v8; // dl
	char v9; // dl
	char *v10; // [esp+Ch] [ebp-4h]

	v0 = 0;
	v10 = &dung_map[16][16];
	do
	{
		v1 = v10;
		v2 = (char *)dungeon + v0;
		v3 = 40;
		do
		{
			v4 = *v2;
			if ( *v2 == 14 && *(v2 - 1) == 10 )
			{
				v5 = *v1;
				v1[112] = *v1;
				v1[113] = v5;
			}
			if ( v4 == 15 && v2[40] == 11 )
			{
				v6 = *v1;
				v1[1] = *v1;
				v1[113] = v6;
			}
			if ( v4 == 10 )
			{
				v7 = *v1;
				v1[112] = *v1;
				v1[113] = v7;
			}
			if ( v4 == 11 )
			{
				v8 = *v1;
				v1[1] = *v1;
				v1[113] = v8;
			}
			if ( v4 == 16 )
			{
				v9 = *v1;
				v1[112] = *v1;
				v1[1] = v9;
				v1[113] = v9;
			}
			v1 += 224;
			v2 += 40;
			--v3;
		}
		while ( v3 );
		v10 += 2;
		++v0;
	}
	while ( v0 < 40 );
}

void __cdecl L2DirtFix()
{
	signed int v0; // ecx
	char *v1; // eax
	signed int v2; // edx

	v0 = 0;
	do
	{
		v1 = (char *)dungeon + v0;
		v2 = 40;
		do
		{
			if ( *v1 == 13 && v1[40] != 11 )
				*v1 = -110;
			if ( *v1 == 11 && v1[40] != 11 )
				*v1 = -112;
			if ( *v1 == 15 && v1[40] != 11 )
				*v1 = -108;
			if ( *v1 == 10 && v1[1] != 10 )
				*v1 = -113;
			if ( *v1 == 13 && v1[1] != 10 )
				*v1 = -110;
			if ( *v1 == 14 && v1[1] != 15 )
				*v1 = -109;
			v1 += 40;
			--v2;
		}
		while ( v2 );
		++v0;
	}
	while ( v0 < 40 );
}

void __cdecl DRLG_InitL2Vals()
{
	int v0; // edi
	int (*v1)[112]; // ebx
	char *v2; // ecx
	int (*v3)[112]; // edx
	signed int v4; // esi
	int v5; // eax
	int v6; // ecx
	int (*v7)[112]; // esi
	char *v8; // eax
	int (*v9)[112]; // edx
	signed int v10; // ebx
	int v11; // edi
	char v12; // [esp-4h] [ebp-14h]

	v0 = 0;
	v1 = dPiece;
	do
	{
		v2 = (char *)dArch + v0;
		v3 = v1;
		v4 = 112;
		do
		{
			v5 = (*v3)[0];
			if ( (*v3)[0] != 541 && v5 != 178 && v5 != 551 )
			{
				if ( v5 == 542 || v5 == 553 )
					goto LABEL_11;
				if ( v5 != 13 )
				{
					if ( v5 != 17 )
						goto LABEL_13;
LABEL_11:
					v12 = 6;
					goto LABEL_12;
				}
			}
			v12 = 5;
LABEL_12:
			*v2 = v12;
LABEL_13:
			++v3;
			v2 += 112;
			--v4;
		}
		while ( v4 );
		v1 = (int (*)[112])((char *)v1 + 4);
		++v0;
	}
	while ( (signed int)v1 < (signed int)dPiece[1] );
	v6 = 0;
	v7 = dPiece;
	do
	{
		v8 = &dArch[0][v6 + 2];
		v9 = v7;
		v10 = 112;
		do
		{
			v11 = (*v9)[0];
			if ( (*v9)[0] == 132 )
			{
				*(v8 - 1) = 2;
				*v8 = 1;
			}
			else if ( v11 == 135 || v11 == 139 )
			{
				v8[110] = 3;
				v8[222] = 4;
			}
			++v9;
			v8 += 112;
			--v10;
		}
		while ( v10 );
		v7 = (int (*)[112])((char *)v7 + 4);
		++v6;
	}
	while ( (signed int)v7 < (signed int)dPiece[1] );
}

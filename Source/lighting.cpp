//HEADER_GOES_HERE

#include "../types.h"

#ifndef NO_GLOBALS
LightListStruct VisionList[32];
char lightactive[32];
LightListStruct LightList[32];
int numlights;
char dung_map_radius[2048]; /* char [16][128] */
int dovision; // weak
int numvision;
char lightmax; // weak
int dolighting; // weak
char dung_map_rgba[16384]; /* int [64][64] short [64][128] char [64][256] */
int visionid;
char *pLightTbl; /* todo: struct? */
BOOL lightflag;
#endif

char CrawlTable[2749] =
{
	1,
		0, 0,
	4,
		0, 1, 0, -1, -1, 0, 1, 0,
	16,
		0, 2, 0, -2, -1, 2, 1, 2,
		-1, -2, 1, -2, -1, 1, 1, 1,
		-1, -1, 1, -1, -2, 1, 2, 1,
		-2, -1, 2, -1, -2, 0, 2, 0,
	24,
		0, 3, 0, -3, -1, 3, 1, 3,
		-1, -3, 1, -3, -2, 3, 2, 3,
		-2, -3, 2, -3, -2, 2, 2, 2,
		-2, -2, 2, -2, -3, 2, 3, 2,
		-3, -2, 3, -2, -3, 1, 3, 1,
		-3, -1, 3, -1, -3, 0, 3, 0,
	32,
		0, 4, 0, -4, -1, 4, 1, 4,
		-1, -4, 1, -4, -2, 4, 2, 4,
		-2, -4, 2, -4, -3, 4, 3, 4,
		-3, -4, 3, -4, -3, 3, 3, 3,
		-3, -3, 3, -3, -4, 3, 4, 3,
		-4, -3, 4, -3, -4, 2, 4, 2,
		-4, -2, 4, -2, -4, 1, 4, 1,
		-4, -1, 4, -1, -4, 0, 4, 0,
	40,
		0, 5, 0, -5, -1, 5, 1, 5,
		-1, -5, 1, -5, -2, 5, 2, 5,
		-2, -5, 2, -5, -3, 5, 3, 5,
		-3, -5, 3, -5, -4, 5, 4, 5,
		-4, -5, 4, -5, -4, 4, 4, 4,
		-4, -4, 4, -4, -5, 4, 5, 4,
		-5, -4, 5, -4, -5, 3, 5, 3,
		-5, -3, 5, -3, -5, 2, 5, 2,
		-5, -2, 5, -2, -5, 1, 5, 1,
		-5, -1, 5, -1, -5, 0, 5, 0,
	48,
		0, 6, 0, -6, -1, 6, 1, 6,
		-1, -6, 1, -6, -2, 6, 2, 6,
		-2, -6, 2, -6, -3, 6, 3, 6,
		-3, -6, 3, -6, -4, 6, 4, 6,
		-4, -6, 4, -6, -5, 6, 5, 6,
		-5, -6, 5, -6, -5, 5, 5, 5,
		-5, -5, 5, -5, -6, 5, 6, 5,
		-6, -5, 6, -5, -6, 4, 6, 4,
		-6, -4, 6, -4, -6, 3, 6, 3,
		-6, -3, 6, -3, -6, 2, 6, 2,
		-6, -2, 6, -2, -6, 1, 6, 1,
		-6, -1, 6, -1, -6, 0, 6, 0,
	56,
		0, 7, 0, -7, -1, 7, 1, 7,
		-1, -7, 1, -7, -2, 7, 2, 7,
		-2, -7, 2, -7, -3, 7, 3, 7,
		-3, -7, 3, -7, -4, 7, 4, 7,
		-4, -7, 4, -7, -5, 7, 5, 7,
		-5, -7, 5, -7, -6, 7, 6, 7,
		-6, -7, 6, -7, -6, 6, 6, 6,
		-6, -6, 6, -6, -7, 6, 7, 6,
		-7, -6, 7, -6, -7, 5, 7, 5,
		-7, -5, 7, -5, -7, 4, 7, 4,
		-7, -4, 7, -4, -7, 3, 7, 3,
		-7, -3, 7, -3, -7, 2, 7, 2,
		-7, -2, 7, -2, -7, 1, 7, 1,
		-7, -1, 7, -1, -7, 0, 7, 0,
	64,
		0, 8, 0, -8, -1, 8, 1, 8,
		-1, -8, 1, -8, -2, 8, 2, 8,
		-2, -8, 2, -8, -3, 8, 3, 8,
		-3, -8, 3, -8, -4, 8, 4, 8,
		-4, -8, 4, -8, -5, 8, 5, 8,
		-5, -8, 5, -8, -6, 8, 6, 8,
		-6, -8, 6, -8, -7, 8, 7, 8,
		-7, -8, 7, -8, -7, 7, 7, 7,
		-7, -7, 7, -7, -8, 7, 8, 7,
		-8, -7, 8, -7, -8, 6, 8, 6,
		-8, -6, 8, -6, -8, 5, 8, 5,
		-8, -5, 8, -5, -8, 4, 8, 4,
		-8, -4, 8, -4, -8, 3, 8, 3,
		-8, -3, 8, -3, -8, 2, 8, 2,
		-8, -2, 8, -2, -8, 1, 8, 1,
		-8, -1, 8, -1, -8, 0, 8, 0,
	72,
		0, 9, 0, -9, -1, 9, 1, 9,
		-1, -9, 1, -9, -2, 9, 2, 9,
		-2, -9, 2, -9, -3, 9, 3, 9,
		-3, -9, 3, -9, -4, 9, 4, 9,
		-4, -9, 4, -9, -5, 9, 5, 9,
		-5, -9, 5, -9, -6, 9, 6, 9,
		-6, -9, 6, -9, -7, 9, 7, 9,
		-7, -9, 7, -9, -8, 9, 8, 9,
		-8, -9, 8, -9, -8, 8, 8, 8,
		-8, -8, 8, -8, -9, 8, 9, 8,
		-9, -8, 9, -8, -9, 7, 9, 7,
		-9, -7, 9, -7, -9, 6, 9, 6,
		-9, -6, 9, -6, -9, 5, 9, 5,
		-9, -5, 9, -5, -9, 4, 9, 4,
		-9, -4, 9, -4, -9, 3, 9, 3,
		-9, -3, 9, -3, -9, 2, 9, 2,
		-9, -2, 9, -2, -9, 1, 9, 1,
		-9, -1, 9, -1, -9, 0, 9, 0,
	80,
		0, 10, 0, -10, -1, 10, 1, 10,
		-1, -10, 1, -10, -2, 10, 2, 10,
		-2, -10, 2, -10, -3, 10, 3, 10,
		-3, -10, 3, -10, -4, 10, 4, 10,
		-4, -10, 4, -10, -5, 10, 5, 10,
		-5, -10, 5, -10, -6, 10, 6, 10,
		-6, -10, 6, -10, -7, 10, 7, 10,
		-7, -10, 7, -10, -8, 10, 8, 10,
		-8, -10, 8, -10, -9, 10, 9, 10,
		-9, -10, 9, -10, -9, 9, 9, 9,
		-9, -9, 9, -9, -10, 9, 10, 9,
		-10, -9, 10, -9, -10, 8, 10, 8,
		-10, -8, 10, -8, -10, 7, 10, 7,
		-10, -7, 10, -7, -10, 6, 10, 6,
		-10, -6, 10, -6, -10, 5, 10, 5,
		-10, -5, 10, -5, -10, 4, 10, 4,
		-10, -4, 10, -4, -10, 3, 10, 3,
		-10, -3, 10, -3, -10, 2, 10, 2,
		-10, -2, 10, -2, -10, 1, 10, 1,
		-10, -1, 10, -1, -10, 0, 10, 0,
	88,
		0, 11, 0, -11, -1, 11, 1, 11,
		-1, -11, 1, -11, -2, 11, 2, 11,
		-2, -11, 2, -11, -3, 11, 3, 11,
		-3, -11, 3, -11, -4, 11, 4, 11,
		-4, -11, 4, -11, -5, 11, 5, 11,
		-5, -11, 5, -11, -6, 11, 6, 11,
		-6, -11, 6, -11, -7, 11, 7, 11,
		-7, -11, 7, -11, -8, 11, 8, 11,
		-8, -11, 8, -11, -9, 11, 9, 11,
		-9, -11, 9, -11, -10, 11, 10, 11,
		-10, -11, 10, -11, -10, 10, 10, 10,
		-10, -10, 10, -10, -11, 10, 11, 10,
		-11, -10, 11, -10, -11, 9, 11, 9,
		-11, -9, 11, -9, -11, 8, 11, 8,
		-11, -8, 11, -8, -11, 7, 11, 7,
		-11, -7, 11, -7, -11, 6, 11, 6,
		-11, -6, 11, -6, -11, 5, 11, 5,
		-11, -5, 11, -5, -11, 4, 11, 4,
		-11, -4, 11, -4, -11, 3, 11, 3,
		-11, -3, 11, -3, -11, 2, 11, 2,
		-11, -2, 11, -2, -11, 1, 11, 1,
		-11, -1, 11, -1, -11, 0, 11, 0,
	96,
		0, 12, 0, -12, -1, 12, 1, 12,
		-1, -12, 1, -12, -2, 12, 2, 12,
		-2, -12, 2, -12, -3, 12, 3, 12,
		-3, -12, 3, -12, -4, 12, 4, 12,
		-4, -12, 4, -12, -5, 12, 5, 12,
		-5, -12, 5, -12, -6, 12, 6, 12,
		-6, -12, 6, -12, -7, 12, 7, 12,
		-7, -12, 7, -12, -8, 12, 8, 12,
		-8, -12, 8, -12, -9, 12, 9, 12,
		-9, -12, 9, -12, -10, 12, 10, 12,
		-10, -12, 10, -12, -11, 12, 11, 12,
		-11, -12, 11, -12, -11, 11, 11, 11,
		-11, -11, 11, -11, -12, 11, 12, 11,
		-12, -11, 12, -11, -12, 10, 12, 10,
		-12, -10, 12, -10, -12, 9, 12, 9,
		-12, -9, 12, -9, -12, 8, 12, 8,
		-12, -8, 12, -8, -12, 7, 12, 7,
		-12, -7, 12, -7, -12, 6, 12, 6,
		-12, -6, 12, -6, -12, 5, 12, 5,
		-12, -5, 12, -5, -12, 4, 12, 4,
		-12, -4, 12, -4, -12, 3, 12, 3,
		-12, -3, 12, -3, -12, 2, 12, 2,
		-12, -2, 12, -2, -12, 1, 12, 1,
		-12, -1, 12, -1, -12, 0, 12, 0,
	104,
		0, 13, 0, -13, -1, 13, 1, 13,
		-1, -13, 1, -13, -2, 13, 2, 13,
		-2, -13, 2, -13, -3, 13, 3, 13,
		-3, -13, 3, -13, -4, 13, 4, 13,
		-4, -13, 4, -13, -5, 13, 5, 13,
		-5, -13, 5, -13, -6, 13, 6, 13,
		-6, -13, 6, -13, -7, 13, 7, 13,
		-7, -13, 7, -13, -8, 13, 8, 13,
		-8, -13, 8, -13, -9, 13, 9, 13,
		-9, -13, 9, -13, -10, 13, 10, 13,
		-10, -13, 10, -13, -11, 13, 11, 13,
		-11, -13, 11, -13, -12, 13, 12, 13,
		-12, -13, 12, -13, -12, 12, 12, 12,
		-12, -12, 12, -12, -13, 12, 13, 12,
		-13, -12, 13, -12, -13, 11, 13, 11,
		-13, -11, 13, -11, -13, 10, 13, 10,
		-13, -10, 13, -10, -13, 9, 13, 9,
		-13, -9, 13, -9, -13, 8, 13, 8,
		-13, -8, 13, -8, -13, 7, 13, 7,
		-13, -7, 13, -7, -13, 6, 13, 6,
		-13, -6, 13, -6, -13, 5, 13, 5,
		-13, -5, 13, -5, -13, 4, 13, 4,
		-13, -4, 13, -4, -13, 3, 13, 3,
		-13, -3, 13, -3, -13, 2, 13, 2,
		-13, -2, 13, -2, -13, 1, 13, 1,
		-13, -1, 13, -1, -13, 0, 13, 0,
	112,
		0, 14, 0, -14, -1, 14, 1, 14,
		-1, -14, 1, -14, -2, 14, 2, 14,
		-2, -14, 2, -14, -3, 14, 3, 14,
		-3, -14, 3, -14, -4, 14, 4, 14,
		-4, -14, 4, -14, -5, 14, 5, 14,
		-5, -14, 5, -14, -6, 14, 6, 14,
		-6, -14, 6, -14, -7, 14, 7, 14,
		-7, -14, 7, -14, -8, 14, 8, 14,
		-8, -14, 8, -14, -9, 14, 9, 14,
		-9, -14, 9, -14, -10, 14, 10, 14,
		-10, -14, 10, -14, -11, 14, 11, 14,
		-11, -14, 11, -14, -12, 14, 12, 14,
		-12, -14, 12, -14, -13, 14, 13, 14,
		-13, -14, 13, -14, -13, 13, 13, 13,
		-13, -13, 13, -13, -14, 13, 14, 13,
		-14, -13, 14, -13, -14, 12, 14, 12,
		-14, -12, 14, -12, -14, 11, 14, 11,
		-14, -11, 14, -11, -14, 10, 14, 10,
		-14, -10, 14, -10, -14, 9, 14, 9,
		-14, -9, 14, -9, -14, 8, 14, 8,
		-14, -8, 14, -8, -14, 7, 14, 7,
		-14, -7, 14, -7, -14, 6, 14, 6,
		-14, -6, 14, -6, -14, 5, 14, 5,
		-14, -5, 14, -5, -14, 4, 14, 4,
		-14, -4, 14, -4, -14, 3, 14, 3,
		-14, -3, 14, -3, -14, 2, 14, 2,
		-14, -2, 14, -2, -14, 1, 14, 1,
		-14, -1, 14, -1, -14, 0, 14, 0,
	120,
		0, 15, 0, -15, -1, 15, 1, 15,
		-1, -15, 1, -15, -2, 15, 2, 15,
		-2, -15, 2, -15, -3, 15, 3, 15,
		-3, -15, 3, -15, -4, 15, 4, 15,
		-4, -15, 4, -15, -5, 15, 5, 15,
		-5, -15, 5, -15, -6, 15, 6, 15,
		-6, -15, 6, -15, -7, 15, 7, 15,
		-7, -15, 7, -15, -8, 15, 8, 15,
		-8, -15, 8, -15, -9, 15, 9, 15,
		-9, -15, 9, -15, -10, 15, 10, 15,
		-10, -15, 10, -15, -11, 15, 11, 15,
		-11, -15, 11, -15, -12, 15, 12, 15,
		-12, -15, 12, -15, -13, 15, 13, 15,
		-13, -15, 13, -15, -14, 15, 14, 15,
		-14, -15, 14, -15, -14, 14, 14, 14,
		-14, -14, 14, -14, -15, 14, 15, 14,
		-15, -14, 15, -14, -15, 13, 15, 13,
		-15, -13, 15, -13, -15, 12, 15, 12,
		-15, -12, 15, -12, -15, 11, 15, 11,
		-15, -11, 15, -11, -15, 10, 15, 10,
		-15, -10, 15, -10, -15, 9, 15, 9,
		-15, -9, 15, -9, -15, 8, 15, 8,
		-15, -8, 15, -8, -15, 7, 15, 7,
		-15, -7, 15, -7, -15, 6, 15, 6,
		-15, -6, 15, -6, -15, 5, 15, 5,
		-15, -5, 15, -5, -15, 4, 15, 4,
		-15, -4, 15, -4, -15, 3, 15, 3,
		-15, -3, 15, -3, -15, 2, 15, 2,
		-15, -2, 15, -2, -15, 1, 15, 1,
		-15, -1, 15, -1, -15, 0, 15, 0,
	(char)128,
		0, 16, 0, -16, -1, 16, 1, 16,
		-1, -16, 1, -16, -2, 16, 2, 16,
		-2, -16, 2, -16, -3, 16, 3, 16,
		-3, -16, 3, -16, -4, 16, 4, 16,
		-4, -16, 4, -16, -5, 16, 5, 16,
		-5, -16, 5, -16, -6, 16, 6, 16,
		-6, -16, 6, -16, -7, 16, 7, 16,
		-7, -16, 7, -16, -8, 16, 8, 16,
		-8, -16, 8, -16, -9, 16, 9, 16,
		-9, -16, 9, -16, -10, 16, 10, 16,
		-10, -16, 10, -16, -11, 16, 11, 16,
		-11, -16, 11, -16, -12, 16, 12, 16,
		-12, -16, 12, -16, -13, 16, 13, 16,
		-13, -16, 13, -16, -14, 16, 14, 16,
		-14, -16, 14, -16, -15, 16, 15, 16,
		-15, -16, 15, -16, -15, 15, 15, 15,
		-15, -15, 15, -15, -16, 15, 16, 15,
		-16, -15, 16, -15, -16, 14, 16, 14,
		-16, -14, 16, -14, -16, 13, 16, 13,
		-16, -13, 16, -13, -16, 12, 16, 12,
		-16, -12, 16, -12, -16, 11, 16, 11,
		-16, -11, 16, -11, -16, 10, 16, 10,
		-16, -10, 16, -10, -16, 9, 16, 9,
		-16, -9, 16, -9, -16, 8, 16, 8,
		-16, -8, 16, -8, -16, 7, 16, 7,
		-16, -7, 16, -7, -16, 6, 16, 6,
		-16, -6, 16, -6, -16, 5, 16, 5,
		-16, -5, 16, -5, -16, 4, 16, 4,
		-16, -4, 16, -4, -16, 3, 16, 3,
		-16, -3, 16, -3, -16, 2, 16, 2,
		-16, -2, 16, -2, -16, 1, 16, 1,
		-16, -1, 16, -1, -16, 0, 16, 0,
	(char)136,
		0, 17, 0, -17, -1, 17, 1, 17,
		-1, -17, 1, -17, -2, 17, 2, 17,
		-2, -17, 2, -17, -3, 17, 3, 17,
		-3, -17, 3, -17, -4, 17, 4, 17,
		-4, -17, 4, -17, -5, 17, 5, 17,
		-5, -17, 5, -17, -6, 17, 6, 17,
		-6, -17, 6, -17, -7, 17, 7, 17,
		-7, -17, 7, -17, -8, 17, 8, 17,
		-8, -17, 8, -17, -9, 17, 9, 17,
		-9, -17, 9, -17, -10, 17, 10, 17,
		-10, -17, 10, -17, -11, 17, 11, 17,
		-11, -17, 11, -17, -12, 17, 12, 17,
		-12, -17, 12, -17, -13, 17, 13, 17,
		-13, -17, 13, -17, -14, 17, 14, 17,
		-14, -17, 14, -17, -15, 17, 15, 17,
		-15, -17, 15, -17, -16, 17, 16, 17,
		-16, -17, 16, -17, -16, 16, 16, 16,
		-16, -16, 16, -16, -17, 16, 17, 16,
		-17, -16, 17, -16, -17, 15, 17, 15,
		-17, -15, 17, -15, -17, 14, 17, 14,
		-17, -14, 17, -14, -17, 13, 17, 13,
		-17, -13, 17, -13, -17, 12, 17, 12,
		-17, -12, 17, -12, -17, 11, 17, 11,
		-17, -11, 17, -11, -17, 10, 17, 10,
		-17, -10, 17, -10, -17, 9, 17, 9,
		-17, -9, 17, -9, -17, 8, 17, 8,
		-17, -8, 17, -8, -17, 7, 17, 7,
		-17, -7, 17, -7, -17, 6, 17, 6,
		-17, -6, 17, -6, -17, 5, 17, 5,
		-17, -5, 17, -5, -17, 4, 17, 4,
		-17, -4, 17, -4, -17, 3, 17, 3,
		-17, -3, 17, -3, -17, 2, 17, 2,
		-17, -2, 17, -2, -17, 1, 17, 1,
		-17, -1, 17, -1, -17, 0, 17, 0,
	(char)144,
		0, 18, 0, -18, -1, 18, 1, 18,
		-1, -18, 1, -18, -2, 18, 2, 18,
		-2, -18, 2, -18, -3, 18, 3, 18,
		-3, -18, 3, -18, -4, 18, 4, 18,
		-4, -18, 4, -18, -5, 18, 5, 18,
		-5, -18, 5, -18, -6, 18, 6, 18,
		-6, -18, 6, -18, -7, 18, 7, 18,
		-7, -18, 7, -18, -8, 18, 8, 18,
		-8, -18, 8, -18, -9, 18, 9, 18,
		-9, -18, 9, -18, -10, 18, 10, 18,
		-10, -18, 10, -18, -11, 18, 11, 18,
		-11, -18, 11, -18, -12, 18, 12, 18,
		-12, -18, 12, -18, -13, 18, 13, 18,
		-13, -18, 13, -18, -14, 18, 14, 18,
		-14, -18, 14, -18, -15, 18, 15, 18,
		-15, -18, 15, -18, -16, 18, 16, 18,
		-16, -18, 16, -18, -17, 18, 17, 18,
		-17, -18, 17, -18, -17, 17, 17, 17,
		-17, -17, 17, -17, -18, 17, 18, 17,
		-18, -17, 18, -17, -18, 16, 18, 16,
		-18, -16, 18, -16, -18, 15, 18, 15,
		-18, -15, 18, -15, -18, 14, 18, 14,
		-18, -14, 18, -14, -18, 13, 18, 13,
		-18, -13, 18, -13, -18, 12, 18, 12,
		-18, -12, 18, -12, -18, 11, 18, 11,
		-18, -11, 18, -11, -18, 10, 18, 10,
		-18, -10, 18, -10, -18, 9, 18, 9,
		-18, -9, 18, -9, -18, 8, 18, 8,
		-18, -8, 18, -8, -18, 7, 18, 7,
		-18, -7, 18, -7, -18, 6, 18, 6,
		-18, -6, 18, -6, -18, 5, 18, 5,
		-18, -5, 18, -5, -18, 4, 18, 4,
		-18, -4, 18, -4, -18, 3, 18, 3,
		-18, -3, 18, -3, -18, 2, 18, 2,
		-18, -2, 18, -2, -18, 1, 18, 1,
		-18, -1, 18, -1, -18, 0, 18, 0
};

char *pCrawlTable[19] = /* figure out what this is for */
{
	CrawlTable,
	CrawlTable+3,
	CrawlTable+12,
	CrawlTable+45,
	CrawlTable+94,
	CrawlTable+159,
	CrawlTable+240,
	CrawlTable+337,
	CrawlTable+450,
	CrawlTable+579,
	CrawlTable+724,
	CrawlTable+885,
	CrawlTable+1062,
	CrawlTable+1255,
	CrawlTable+1464,
	CrawlTable+1689,
	CrawlTable+1930,
	CrawlTable+2187,
	CrawlTable+2460
};
unsigned char vCrawlTable[23][30] =
{
  { 1, 0, 2, 0, 3, 0, 4, 0, 5, 0, 6, 0, 7, 0, 8, 0, 9, 0, 10, 0, 11, 0, 12, 0, 13, 0, 14, 0, 15, 0 },
  { 1, 0, 2, 0, 3, 0, 4, 0, 5, 0, 6, 0, 7, 0, 8, 1, 9, 1, 10, 1, 11, 1, 12, 1, 13, 1, 14, 1, 15, 1 },
  { 1, 0, 2, 0, 3, 0, 4, 1, 5, 1, 6, 1, 7, 1, 8, 1, 9, 1, 10, 1, 11, 1, 12, 2, 13, 2, 14, 2, 15, 2 },
  { 1, 0, 2, 0, 3, 1, 4, 1, 5, 1, 6, 1, 7, 1, 8, 2, 9, 2, 10, 2, 11, 2, 12, 2, 13, 3, 14, 3, 15, 3 },
  { 1, 0, 2, 1, 3, 1, 4, 1, 5, 1, 6, 2, 7, 2, 8, 2, 9, 3, 10, 3, 11, 3, 12, 3, 13, 4, 14, 4, 0, 0 },
  { 1, 0, 2, 1, 3, 1, 4, 1, 5, 2, 6, 2, 7, 3, 8, 3, 9, 3, 10, 4, 11, 4, 12, 4, 13, 5, 14, 5, 0, 0 },
  { 1, 0, 2, 1, 3, 1, 4, 2, 5, 2, 6, 3, 7, 3, 8, 3, 9, 4, 10, 4, 11, 5, 12, 5, 13, 6, 14, 6, 0, 0 },
  { 1, 1, 2, 1, 3, 2, 4, 2, 5, 3, 6, 3, 7, 4, 8, 4, 9, 5, 10, 5, 11, 6, 12, 6, 13, 7, 0, 0, 0, 0 },
  { 1, 1, 2, 1, 3, 2, 4, 2, 5, 3, 6, 4, 7, 4, 8, 5, 9, 6, 10, 6, 11, 7, 12, 7, 12, 8, 13, 8, 0, 0 },
  { 1, 1, 2, 2, 3, 2, 4, 3, 5, 4, 6, 5, 7, 5, 8, 6, 9, 7, 10, 7, 10, 8, 11, 8, 12, 9, 0, 0, 0, 0 },
  { 1, 1, 2, 2, 3, 3, 4, 4, 5, 5, 6, 5, 7, 6, 8, 7, 9, 8, 10, 9, 11, 9, 11, 10, 0, 0, 0, 0, 0, 0 },
  { 1, 1, 2, 2, 3, 3, 4, 4, 5, 5, 6, 6, 7, 7, 8, 8, 9, 9, 10, 10, 11, 11, 0, 0, 0, 0, 0, 0, 0, 0 },
  { 1, 1, 2, 2, 3, 3, 4, 4, 5, 5, 5, 6, 6, 7, 7, 8, 8, 9, 9, 10, 9, 11, 10, 11, 0, 0, 0, 0, 0, 0 },
  { 1, 1, 2, 2, 2, 3, 3, 4, 4, 5, 5, 6, 5, 7, 6, 8, 7, 9, 7, 10, 8, 10, 8, 11, 9, 12, 0, 0, 0, 0 },
  { 1, 1, 1, 2, 2, 3, 2, 4, 3, 5, 4, 6, 4, 7, 5, 8, 6, 9, 6, 10, 7, 11, 7, 12, 8, 12, 8, 13, 0, 0 },
  { 1, 1, 1, 2, 2, 3, 2, 4, 3, 5, 3, 6, 4, 7, 4, 8, 5, 9, 5, 10, 6, 11, 6, 12, 7, 13, 0, 0, 0, 0 },
  { 0, 1, 1, 2, 1, 3, 2, 4, 2, 5, 3, 6, 3, 7, 3, 8, 4, 9, 4, 10, 5, 11, 5, 12, 6, 13, 6, 14, 0, 0 },
  { 0, 1, 1, 2, 1, 3, 1, 4, 2, 5, 2, 6, 3, 7, 3, 8, 3, 9, 4, 10, 4, 11, 4, 12, 5, 13, 5, 14, 0, 0 },
  { 0, 1, 1, 2, 1, 3, 1, 4, 1, 5, 2, 6, 2, 7, 2, 8, 3, 9, 3, 10, 3, 11, 3, 12, 4, 13, 4, 14, 0, 0 },
  { 0, 1, 0, 2, 1, 3, 1, 4, 1, 5, 1, 6, 1, 7, 2, 8, 2, 9, 2, 10, 2, 11, 2, 12, 3, 13, 3, 14, 3, 15 },
  { 0, 1, 0, 2, 0, 3, 1, 4, 1, 5, 1, 6, 1, 7, 1, 8, 1, 9, 1, 10, 1, 11, 2, 12, 2, 13, 2, 14, 2, 15 },
  { 0, 1, 0, 2, 0, 3, 0, 4, 0, 5, 0, 6, 0, 7, 1, 8, 1, 9, 1, 10, 1, 11, 1, 12, 1, 13, 1, 14, 1, 15 },
  { 0, 1, 0, 2, 0, 3, 0, 4, 0, 5, 0, 6, 0, 7, 0, 8, 0, 9, 0, 10, 0, 11, 0, 12, 0, 13, 0, 14, 0, 15 }
};
unsigned char byte_49463C[18][18] = /* unused */
{
	{ 0, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3 },
	{ 0, 1, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3 },
	{ 0, 1, 2, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3 },
	{ 0, 0, 1, 2, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3 },
	{ 0, 0, 1, 1, 2, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3 },
	{ 0, 0, 1, 1, 2, 2, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3 },
	{ 0, 0, 0, 1, 1, 2, 2, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3 },
	{ 0, 0, 0, 1, 1, 1, 2, 2, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3 },
	{ 0, 0, 0, 0, 1, 1, 2, 2, 2, 3, 3, 3, 3, 3, 3, 3, 3, 3 },
	{ 0, 0, 0, 0, 1, 1, 1, 2, 2, 2, 3, 3, 3, 3, 3, 3, 3, 3 },
	{ 0, 0, 0, 0, 0, 1, 1, 1, 2, 2, 2, 3, 3, 3, 3, 3, 3, 3 },
	{ 0, 0, 0, 0, 0, 1, 1, 1, 2, 2, 2, 2, 3, 3, 3, 3, 3, 3 },
	{ 0, 0, 0, 0, 0, 1, 1, 1, 1, 2, 2, 2, 2, 3, 3, 3, 3, 3 },
	{ 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 1, 2, 2, 2, 2, 3, 3, 3 },
	{ 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 1, 2, 2, 2, 2, 2, 3, 3 },
	{ 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 1, 2, 2, 2, 2, 2, 3, 3 },
	{ 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 1, 2, 2, 2, 2, 2, 3 },
	{ 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 1, 2, 2, 2, 2, 2 }
};

unsigned char RadiusAdj[23] = { 0, 0, 0, 0, 1, 1, 1, 2, 2, 2, 3, 4, 3, 2, 2, 2, 1, 1, 1, 0, 0, 0, 0 };

void __fastcall SetLightFX(int *x, int *y, short *s_r, short *s_g, int *s_b, int *d_r, int *d_g, int *d_b)
{
	short *v8; // eax
	int v9; // edx
	int *v10; // [esp+Ch] [ebp-4h]
	short *s_ra; // [esp+18h] [ebp+8h]

	*d_g = 0;
	*d_b = 0;
	v8 = s_r;
	v10 = y;
	v9 = *(_DWORD *)s_r;
	*(_DWORD *)s_r = 7 - *(_DWORD *)s_g;
	*(_DWORD *)s_g = v9;
	s_ra = (short *)*s_b;
	*s_b = 7 - *d_r;
	*d_r = (int)s_ra;
	*x = *(_DWORD *)v8 - *s_b;
	*v10 = *(_DWORD *)s_g - *d_r;
	if ( *x < 0 )
	{
		*x += 8;
		*d_g = 1;
	}
	if ( *v10 < 0 )
	{
		*v10 += 8;
		*d_b = 1;
	}
}

void __fastcall DoLighting(int nXPos, int nYPos, int nRadius, int Lnum)
{
	int v4; // edi
	int v5; // ebx
	int v6; // ecx
	int v7; // eax
	int v8; // edx
	int v9; // esi
	int v10; // eax
	char *v11; // edi
	signed int v12; // ecx
	int v13; // edx
	_BYTE *v14; // ecx
	int v15; // ebx
	bool v16; // sf
	unsigned char v17; // of
	int v18; // esi
	int v19; // ecx
	char *v20; // edi
	signed int v21; // eax
	int v22; // edx
	_BYTE *v23; // eax
	int v24; // ebx
	int v25; // eax
	int v26; // esi
	char *v27; // edi
	signed int v28; // ecx
	int v29; // edx
	_BYTE *v30; // ecx
	int v31; // ebx
	signed int v32; // ebx
	int v33; // ecx
	char *v34; // esi
	signed int v35; // eax
	int v36; // edx
	_BYTE *v37; // eax
	int v38; // edi
	short s_r[2]; // [esp+Ch] [ebp-44h]
	short s_g[2]; // [esp+10h] [ebp-40h]
	int s_b; // [esp+14h] [ebp-3Ch]
	int d_r; // [esp+18h] [ebp-38h]
	int v43; // [esp+1Ch] [ebp-34h]
	int v44; // [esp+20h] [ebp-30h]
	int v45; // [esp+24h] [ebp-2Ch]
	int v46; // [esp+28h] [ebp-28h]
	int v47; // [esp+2Ch] [ebp-24h]
	int v48; // [esp+30h] [ebp-20h]
	int d_g; // [esp+34h] [ebp-1Ch]
	int d_b; // [esp+38h] [ebp-18h]
	int v51; // [esp+3Ch] [ebp-14h]
	int v52; // [esp+40h] [ebp-10h]
	int y; // [esp+44h] [ebp-Ch]
	int x; // [esp+48h] [ebp-8h]
	int v55; // [esp+4Ch] [ebp-4h]
	int Lnuma; // [esp+5Ch] [ebp+Ch]
	int Lnumb; // [esp+5Ch] [ebp+Ch]
	int Lnumc; // [esp+5Ch] [ebp+Ch]
	int Lnumd; // [esp+5Ch] [ebp+Ch]

	v4 = nYPos;
	v5 = nXPos;
	v6 = 0;
	v7 = 0;
	v48 = nYPos;
	v52 = v5;
	x = 0;
	y = 0;
	s_b = 0;
	d_r = 0;
	d_g = 0;
	d_b = 0;
	if ( Lnum >= 0 )
	{
		v7 = LightList[Lnum]._yoff;
		x = LightList[Lnum]._xoff;
		v6 = x;
		y = v7;
		if ( x < 0 )
		{
			v6 = x + 8;
			--v5;
			x += 8;
			v52 = v5;
		}
		if ( v7 < 0 )
		{
			v7 += 8;
			v4 = nYPos - 1;
			y = v7;
			v48 = nYPos - 1;
		}
	}
	*(_DWORD *)s_r = v6;
	*(_DWORD *)s_g = v7;
	v8 = 15;
	if ( v5 - 15 >= 0 )
		v44 = 15;
	else
		v44 = v5 + 1;
	if ( v5 + 15 <= 112 )
		v46 = 15;
	else
		v46 = 112 - v5;
	if ( v4 - 15 >= 0 )
		v43 = 15;
	else
		v43 = v4 + 1;
	if ( v4 + 15 > 112 )
		v8 = 112 - v4;
	v45 = v8;
	if ( v5 >= 0 && v5 < 112 && v4 >= 0 && v4 < 112 )
		dTransVal[v5][v4] = 0;
	v55 = 0;
	v51 = v6 + 8 * v7;
	if ( v43 > 0 )
	{
		v47 = v4;
		do
		{
			Lnuma = 1;
			if ( v46 > 1 )
			{
				v9 = v5 + 1;
				v10 = 112 * (v5 + 1);
				v11 = &dung_map_rgba[16 * (v55 + 16 * v51)];
				do
				{
					v12 = (unsigned char)v11[Lnuma];
					if ( v12 < 128 )
					{
						v13 = (unsigned char)dung_map_radius[128 * nRadius + v12];
						if ( v9 >= 0 && v9 < 112 && v47 >= 0 && v47 < 112 )
						{
							v14 = (unsigned char *)dTransVal + v47 + v10;
							v15 = (char)*v14;
							v17 = __OFSUB__(v13, v15);
							v16 = v13 - v15 < 0;
							v5 = v52;
							if ( v16 ^ v17 )
								*v14 = v13;
						}
					}
					++Lnuma;
					v10 += 112;
					++v9;
				}
				while ( Lnuma < v46 );
				v4 = v48;
			}
			++v55;
			++v47;
		}
		while ( v55 < v43 );
	}
	SetLightFX(&x, &y, s_r, s_g, &s_b, &d_r, &d_g, &d_b);
	v18 = 0;
	v51 = x + 8 * y;
	if ( v45 > 0 )
	{
		v47 = 112 * v5;
		do
		{
			Lnumb = 1;
			if ( v46 > 1 )
			{
				v19 = v4 - 1;
				v20 = &dung_map_rgba[16 * (d_b + v18 + 16 * v51) + d_g];
				do
				{
					v21 = (unsigned char)v20[Lnumb];
					if ( v21 < 128 )
					{
						v22 = (unsigned char)dung_map_radius[128 * nRadius + v21];
						if ( v18 + v5 >= 0 && v18 + v5 < 112 && v19 >= 0 && v19 < 112 )
						{
							v23 = (unsigned char *)dTransVal + v47 + v19;
							v24 = (char)*v23;
							v17 = __OFSUB__(v22, v24);
							v16 = v22 - v24 < 0;
							v5 = v52;
							if ( v16 ^ v17 )
								*v23 = v22;
						}
					}
					++Lnumb;
					--v19;
				}
				while ( Lnumb < v46 );
				v4 = v48;
			}
			v47 += 112;
			++v18;
		}
		while ( v18 < v45 );
	}
	SetLightFX(&x, &y, s_r, s_g, &s_b, &d_r, &d_g, &d_b);
	v55 = 0;
	v51 = x + 8 * y;
	if ( v45 > 0 )
	{
		v46 = v4;
		do
		{
			Lnumc = 1;
			if ( v44 > 1 )
			{
				v25 = 112 * v5 - 112;
				v26 = v5 - 1;
				v27 = &dung_map_rgba[16 * (d_b + v55 + 16 * v51) + d_g];
				do
				{
					v28 = (unsigned char)v27[Lnumc];
					if ( v28 < 128 )
					{
						v29 = (unsigned char)dung_map_radius[128 * nRadius + v28];
						if ( v26 >= 0 && v26 < 112 && v46 >= 0 && v46 < 112 )
						{
							v30 = (unsigned char *)dTransVal + v46 + v25;
							v31 = (char)*v30;
							v17 = __OFSUB__(v29, v31);
							v16 = v29 - v31 < 0;
							v5 = v52;
							if ( v16 ^ v17 )
								*v30 = v29;
						}
					}
					++Lnumc;
					v25 -= 112;
					--v26;
				}
				while ( Lnumc < v44 );
				v4 = v48;
			}
			++v55;
			--v46;
		}
		while ( v55 < v45 );
	}
	SetLightFX(&x, &y, s_r, s_g, &s_b, &d_r, &d_g, &d_b);
	v55 = 0;
	v51 = x + 8 * y;
	if ( v43 > 0 )
	{
		Lnumd = v5;
		*(_DWORD *)s_r = 112 * v5;
		do
		{
			v32 = 1;
			if ( v44 > 1 )
			{
				v33 = v4 + 1;
				v34 = &dung_map_rgba[16 * (d_b + v55 + 16 * v51) + d_g];
				do
				{
					v35 = (unsigned char)v34[v32];
					if ( v35 < 128 )
					{
						v36 = (unsigned char)dung_map_radius[128 * nRadius + v35];
						if ( Lnumd >= 0 && Lnumd < 112 && v33 >= 0 && v33 < 112 )
						{
							v37 = (unsigned char *)dTransVal + v33 + *(_DWORD *)s_r;
							v38 = (char)*v37;
							v17 = __OFSUB__(v36, v38);
							v16 = v36 - v38 < 0;
							v4 = v48;
							if ( v16 ^ v17 )
								*v37 = v36;
						}
					}
					++v32;
					++v33;
				}
				while ( v32 < v44 );
			}
			++v55;
			--Lnumd;
			*(_DWORD *)s_r -= 112;
		}
		while ( v55 < v43 );
	}
}

void __fastcall DoUnLight(int nXPos, int nYPos, int nRadius)
{
	int max_y; // ebx
	int y; // esi
	int x; // edx
	int max_x; // edi
	signed int v7; // esi
	int v8; // eax
	int radius_block; // [esp+14h] [ebp+8h]

	max_y = nYPos + nRadius + 1;
	y = nYPos - (nRadius + 1);
	x = nXPos - (nRadius + 1);
	max_x = nXPos + nRadius + 1;
	if ( y < 0 )
		y = 0;
	if ( max_y > 112 )
		max_y = 112;
	if ( x < 0 )
		x = 0;
	if ( max_x > 112 )
		max_x = 112;
	for ( radius_block = y; radius_block < max_y; ++radius_block )
	{
		v7 = x;
		if ( x < max_x )
		{
			v8 = radius_block + 112 * x;
			do
			{
				if ( v7 >= 0 && v7 < 112 && radius_block >= 0 && radius_block < 112 )
					dTransVal[0][v8] = dTransVal2[0][v8];
				++v7;
				v8 += 112;
			}
			while ( v7 < max_x );
		}
	}
}

void __fastcall DoUnVision(int nXPos, int nYPos, int nRadius)
{
	int y2; // edi
	int y1; // esi
	int x1; // edx
	int x2; // ecx
	char *v7; // eax
	int i; // ecx
	int j; // edx

	y2 = nYPos + nRadius + 1;
	y1 = nYPos - (nRadius + 1);
	x1 = nXPos - (nRadius + 1);
	x2 = nRadius + 1 + nXPos;
	if ( y1 < 0 )
		y1 = 0;
	if ( y2 > 112 )
		y2 = 112;
	if ( x1 < 0 )
		x1 = 0;
	if ( x2 > 112 )
		x2 = 112;
	if ( x1 < x2 )
	{
		v7 = dFlags[x1];
		i = x2 - x1;
		do
		{
			for ( j = y1; j < y2; ++j )
				v7[j] &= ~(DFLAG_VISIBLE | DFLAG_LIT);
			v7 += 112;
			--i;
		}
		while ( i );
	}
}

void __fastcall DoVision(int nXPos, int nYPos, int nRadius, unsigned char doautomap, int visible)
{
	char *v5; // esi
	int v6; // esi
	int v7; // edi
	unsigned char *v8; // eax
	int v9; // ebx
	int v10; // ecx
	unsigned char v11; // dl
	int v12; // ecx
	int v13; // ecx
	unsigned char v14; // cl
	unsigned char v15; // dl
	int v16; // ecx
	int v17; // ecx
	int i; // [esp+Ch] [ebp-34h]
	unsigned char *v19; // [esp+10h] [ebp-30h]
	int v20; // [esp+14h] [ebp-2Ch]
	int v21; // [esp+18h] [ebp-28h]
	int v22; // [esp+1Ch] [ebp-24h]
	signed int v23; // [esp+20h] [ebp-20h]
	signed int v24; // [esp+24h] [ebp-1Ch]
	signed int v25; // [esp+28h] [ebp-18h]
	signed int v26; // [esp+2Ch] [ebp-14h]
	signed int v27; // [esp+30h] [ebp-10h]
	int v28; // [esp+34h] [ebp-Ch]
	int v29; // [esp+38h] [ebp-8h]
	unsigned char v30; // [esp+3Fh] [ebp-1h]
	unsigned char v31; // [esp+3Fh] [ebp-1h]

	v28 = nYPos;
	v29 = nXPos;
	if ( nXPos >= 0 && nXPos <= 112 && nYPos >= 0 && nYPos <= 112 )
	{
		if ( doautomap )
		{
			v5 = &dFlags[nXPos][nYPos];
			if ( *v5 >= 0 )
			{
				SetAutomapView(nXPos, nXPos);
				nYPos = v28;
				nXPos = v29;
			}
			*v5 |= DFLAG_EXPLORED;
		}
		if ( visible )
			dFlags[nXPos][nYPos] |= DFLAG_LIT;
		dFlags[nXPos][nYPos] |= DFLAG_VISIBLE;
	}
	v27 = 0;
	v6 = doautomap;
	v7 = doautomap;
	do
	{
		v20 = 0;
		v8 = &vCrawlTable[0][1];
		v19 = &vCrawlTable[0][1];
		do
		{
			v9 = 0;
			v21 = 0;
			for ( i = 2 * (nRadius - RadiusAdj[v20]); v9 < i; v9 += 2 )
			{
				if ( v21 )
					break;
				v26 = 0;
				v24 = 0;
				v25 = 0;
				v23 = 0;
				if ( v27 )
				{
					switch ( v27 )
					{
						case 1:
							v13 = v8[v9 - 1];
							v6 = v29 - (unsigned char)v13;
							v31 = v8[v9];
							v7 = v28 - v31;
							if ( (_BYTE)v13 && v31 )
							{
								v25 = 1;
								v24 = 1;
							}
							break;
						case 2:
							v12 = v8[v9 - 1];
							v30 = v8[v9];
							v6 = v29 + (unsigned char)v12;
							v7 = v28 - v30;
							if ( (_BYTE)v12 && v30 )
							{
								v26 = -1;
								v23 = 1;
							}
							break;
						case 3:
							v10 = v8[v9 - 1];
							v6 = v29 - (unsigned char)v10;
							v11 = v8[v9];
							v7 = v28 + v11;
							if ( (_BYTE)v10 )
							{
								if ( v11 )
								{
									v25 = -1;
									v24 = 1;
								}
							}
							break;
					}
				}
				else
				{
					v14 = v8[v9 - 1];
					v15 = v8[v9];
					v6 = v29 + v14;
					v7 = v28 + v15;
					if ( v14 && v15 )
					{
						v26 = -1;
						v23 = -1;
					}
				}
				if ( v6 >= 0 && v6 <= 112 && v7 >= 0 && v7 <= 112 )
				{
					v22 = v7 + 112 * v6;
					v21 = (unsigned char)nBlockTable[dPiece[0][v22]];
					if ( !nBlockTable[dPiece[0][v25 + v7 + 112 * (v6 + v26)]]
					  || !nBlockTable[dPiece[0][v23 + v7 + 112 * (v6 + v24)]] )
					{
						v16 = v7 + 112 * v6;
						if ( doautomap )
						{
							if ( dFlags[0][v22] >= 0 )
							{
								SetAutomapView(v6, v7);
								v16 = v7 + 112 * v6;
								v8 = v19;
							}
							dFlags[0][v16] |= DFLAG_EXPLORED;
						}
						if ( visible )
							dFlags[0][v16] |= DFLAG_LIT;
						dFlags[0][v16] |= DFLAG_VISIBLE;
						if ( !v21 )
						{
							v17 = dung_map[0][v16];
							if ( v17 )
								TransList[v17] = 1;
						}
					}
				}
			}
			++v20;
			v8 += 30;
			v19 = v8;
		}
		while ( (signed int)v8 < (signed int)&vCrawlTable[23][1] );
		++v27;
	}
	while ( v27 < 4 );
}

void __cdecl FreeLightTable()
{
	char *v0; // ecx

	v0 = pLightTbl;
	pLightTbl = 0;
	mem_free_dbg(v0);
}

void __cdecl InitLightTable()
{
	pLightTbl = (char *)DiabloAllocPtr(LIGHTSIZE);
}

void __cdecl MakeLightTable()
{
	char *v0; // ebx
	signed int v1; // esi
	unsigned char v2; // al
	unsigned char v3; // cl
	signed int v4; // edi
	int v5; // edx
	signed int v6; // edi
	unsigned char v7; // cl
	unsigned char v8; // al
	signed int v9; // edx
	unsigned char v10; // cl
	unsigned char v11; // al
	char *v12; // ebx
	char *v13; // ebx
	int v14; // ecx
	signed int v15; // esi
	char v16; // al
	int v17; // edx
	int v18; // ebx
	signed int v19; // esi
	_BYTE *v20; // ebx
	char *v21; // ebx
	int v22; // edi
	unsigned char *v23; // esi
	signed int v24; // edx
	unsigned char *v25; // esi
	signed int v26; // edx
	signed int v27; // ecx
	char v28; // al
	_BYTE *v29; // ebx
	signed int v30; // edx
	char v31; // al
	signed int v32; // ecx
	signed int v33; // ecx
	char v34; // al
	int v35; // eax
	signed int v36; // esi
	char *v37; // eax
	signed int v38; // ebx
	int v39; // esi
	double v40; // st7
	double v41; // st6
	int v42; // ecx
	char *v43; // ecx
	bool v44; // zf
	char v45[16]; // [esp+14h] [ebp-2Ch] /* check */
	int v46; // [esp+24h] [ebp-1Ch]
	int v47; // [esp+28h] [ebp-18h]
	char *v48; // [esp+2Ch] [ebp-14h]
	int v49; // [esp+30h] [ebp-10h]
	int v50; // [esp+34h] [ebp-Ch]
	int v51; // [esp+38h] [ebp-8h]
	int v52; // [esp+3Ch] [ebp-4h]

	v51 = 0;
	v0 = pLightTbl;
	v1 = light4flag != 0 ? 3 : 15;
	v50 = light4flag != 0 ? 3 : 15;
	if ( v1 > 0 )
	{
		v49 = light4flag != 0 ? 3 : 15;
		do
		{
			*v0++ = 0;
			v52 = 0;
			do
			{
				v2 = 16 * v52 + 15;
				v3 = v51 + 16 * v52;
				v4 = 0;
				do
				{
					if ( v4 || v52 )
						*v0++ = v3;
					if ( v3 >= v2 )
					{
						v2 = 0;
						v3 = 0;
					}
					else
					{
						++v3;
					}
					++v4;
				}
				while ( v4 < 16 );
				++v52;
			}
			while ( v52 < 8 );
			v52 = 16;
			v5 = v51 >> 1;
			do
			{
				v6 = 8;
				v7 = v5 + 8 * v52;
				v8 = 8 * v52 + 7;
				do
				{
					*v0++ = v7;
					if ( v7 >= v8 )
					{
						v8 = 0;
						v7 = 0;
					}
					else
					{
						++v7;
					}
					--v6;
				}
				while ( v6 );
				++v52;
			}
			while ( v52 < 20 );
			v52 = 10;
			do
			{
				v9 = 16;
				v10 = v51 + 16 * v52;
				v11 = 16 * v52 + 15;
				do
				{
					*v0++ = v10;
					if ( v10 >= v11 )
					{
						v11 = 0;
						v10 = 0;
					}
					else
					{
						++v10;
					}
					if ( v10 == LOBYTE(-1) )
					{
						v11 = 0;
						v10 = 0;
					}
					--v9;
				}
				while ( v9 );
				++v52;
			}
			while ( v52 < 16 );
			if ( light4flag )
				v51 += 5;
			else
				++v51;
			--v49;
		}
		while ( v49 );
	}
	memset(v0, 0, 0x100u);
	v12 = v0 + 256;
	if ( leveltype == DTYPE_HELL )
	{
		v13 = pLightTbl;
		if ( v1 > 0 )
		{
			v14 = v50;
			v49 = v50;
			do
			{
				v52 = 0;
				v45[0] = 0;
				v51 = v14;
				v15 = 1;
				v48 = (char *)(v50 / v14);
				v47 = v50 % v14;
				v16 = 1;
				do
				{
					v17 = v51;
					v45[v15] = v16;
					v51 = v47 + v17;
					if ( v47 + v17 > v14 && v15 < 15 )
					{
						++v15;
						v51 -= v14;
						v45[v15] = v16;
					}
					if ( (char *)++v52 == v48 )
					{
						++v16;
						v52 = 0;
					}
					++v15;
				}
				while ( v15 < 16 );
				*v13 = 0;
				v18 = (int)(v13 + 1);
				*(_DWORD *)v18 = *(_DWORD *)&v45[1];
				*(_DWORD *)(v18 + 4) = *(_DWORD *)&v45[5];
				*(_DWORD *)(v18 + 8) = *(_DWORD *)&v45[9];
				*(_WORD *)(v18 + 12) = *(_WORD *)&v45[13];
				*(_BYTE *)(v18 + 14) = v45[15];
				v19 = 15;
				v20 = (_BYTE *)(v18 + 15);
				do
					*v20++ = v45[v19--];
				while ( v19 > 0 );
				*v20 = 1;
				v13 = (char *)v20 + 225;
				--v14;
				--v49;
			}
			while ( v49 );
		}
		*v13 = 0;
		v21 = v13 + 1;
		memset(v21, 1u, 0x1Cu);
		v22 = (int)(v21 + 28);
		*(_WORD *)v22 = 257;
		*(_BYTE *)(v22 + 2) = 1;
		v12 = v21 + 255;
	}
	v23 = LoadFileInMem("PlrGFX\\Infra.TRN", 0);
	v24 = 0;
	do
		*v12++ = v23[v24++];
	while ( v24 < 256 );
	mem_free_dbg(v23);
	v25 = LoadFileInMem("PlrGFX\\Stone.TRN", 0);
	v26 = 0;
	do
		*v12++ = v25[v26++];
	while ( v26 < 256 );
	mem_free_dbg(v25);
	v27 = 0;
	do
	{
		v28 = -30;
		do
		{
			if ( v27 || v28 != -30 )
				*v12 = v28;
			else
				*v12 = 0;
			++v12;
			++v28;
		}
		while ( (unsigned char)v28 < 0xEFu );
		*v12 = 0;
		v29 = (unsigned char *)v12 + 1;
		*v29++ = 0;
		*v29 = 0;
		v12 = (char *)v29 + 1;
		++v27;
	}
	while ( v27 < 8 );
	v30 = 4;
	do
	{
		v31 = -32;
		v32 = 8;
		do
		{
			*v12++ = v31;
			v31 += 2;
			--v32;
		}
		while ( v32 );
		--v30;
	}
	while ( v30 );
	v33 = 6;
	do
	{
		v34 = -32;
		do
			*v12++ = v34++;
		while ( (unsigned char)v34 < 0xEFu );
		*v12++ = 0;
		--v33;
	}
	while ( v33 );
	v35 = 0;
	v51 = (int)dung_map_radius;
	v52 = 8;
	do
	{
		v36 = 0;
		v49 = 0;
		v50 = v35 + 1;
		do
		{
			if ( v36 <= v52 )
				*(_BYTE *)(v51 + v36) = (signed __int64)((double)v49 * 15.0 / ((double)v50 * 8.0) + 0.5);
			else
				*(_BYTE *)(v51 + v36) = 15;
			v49 = ++v36;
		}
		while ( v36 < 128 );
		v52 += 8;
		v51 += 128;
		v35 = v50;
	}
	while ( v52 < 136 );
	v49 = 0;
	v37 = dung_map_rgba;
	do
	{
		v52 = 0;
		do
		{
			v48 = v37;
			v50 = v49;
			v47 = 16;
			do
			{
				v38 = 0;
				v39 = v50 * v50;
				v51 = v52;
				do
				{
					v46 = v39 + v51 * v51;
					v46 = (unsigned char)(signed __int64)sqrt((double)v46);
					v40 = (double)v46;
					if ( v40 >= 0.0 )
						v41 = 0.5;
					else
						v41 = -0.5;
					v42 = (int)v48;
					v51 += 8;
					v48[v38++] = (signed __int64)(v41 + v40);
				}
				while ( v38 < 16 );
				v50 += 8;
				v43 = (char *)(v42 + 16);
				v44 = v47-- == 1;
				v48 = v43;
			}
			while ( !v44 );
			--v52;
			v37 = v43;
		}
		while ( v52 > -8 );
		--v49;
	}
	while ( (signed int)v43 < (signed int)&dung_map_rgba[16384] );
}
// 525728: using guessed type int light4flag;
// 5BB1ED: using guessed type char leveltype;

#ifdef _DEBUG
void __cdecl ToggleLighting()
{
	int i;

	lightflag ^= 1;
	if ( lightflag )
	{
		memset(dTransVal, 0, sizeof(dTransVal));
	}
	else
	{
		memcpy(dTransVal, dTransVal2, sizeof(dTransVal));
		for(i = 0; i < 4; i++)
		{
			if ( plr[i].plractive )
			{
				if ( currlevel == plr[i].plrlevel )
					DoLighting(plr[i].WorldX, plr[i].WorldY, plr[i]._pLightRad, -1);
			}
		}
	}
}
#endif

void __cdecl InitLightMax()
{
	lightmax = light4flag == 0 ? 15 : 3;
}
// 525728: using guessed type int light4flag;
// 642A14: using guessed type char lightmax;

void __cdecl InitLighting()
{
	signed int v0; // eax

	v0 = 0;
	numlights = 0;
	dolighting = 0;
	lightflag = 0;
	do
	{
		lightactive[v0] = v0;
		++v0;
	}
	while ( v0 < 32 );
}
// 642A18: using guessed type int dolighting;
// 646A28: using guessed type int lightflag;

int __fastcall AddLight(int x, int y, int r)
{
	int lid; // eax

	lid = -1;
	if ( !lightflag && numlights < 32 )
	{
		lid = lightactive[numlights++];
		dolighting = 1;
		LightList[lid]._lx = x;
		LightList[lid]._ly = y;
		LightList[lid]._lradius = r;
		LightList[lid]._xoff = 0;
		LightList[lid]._yoff = 0;
		LightList[lid]._ldel = 0;
		LightList[lid]._lunflag = 0;
	}
	return lid;
}
// 642A18: using guessed type int dolighting;
// 646A28: using guessed type int lightflag;

void __fastcall AddUnLight(int i)
{
	if ( !lightflag && i != -1 )
	{
		LightList[i]._ldel = 1;
		dolighting = 1;
	}
}
// 642A18: using guessed type int dolighting;
// 646A28: using guessed type int lightflag;

void __fastcall ChangeLightRadius(int i, int r)
{
	if ( !lightflag && i != -1 )
	{
		LightList[i]._lunx = LightList[i]._lx;
		LightList[i]._luny = LightList[i]._ly;
		LightList[i]._lunflag = 1;
		LightList[i]._lunr = LightList[i]._lradius;
		dolighting = 1;
		LightList[i]._lradius = r;
	}
}
// 642A18: using guessed type int dolighting;
// 646A28: using guessed type int lightflag;

void __fastcall ChangeLightXY(int i, int x, int y)
{
	if ( !lightflag && i != -1 )
	{
		LightList[i]._lunx = LightList[i]._lx;
		LightList[i]._lunflag = 1;
		dolighting = 1;
		LightList[i]._luny = LightList[i]._ly;
		LightList[i]._lunr = LightList[i]._lradius;
		LightList[i]._ly = y;
		LightList[i]._lx = x;
	}
}
// 642A18: using guessed type int dolighting;
// 646A28: using guessed type int lightflag;

void __fastcall ChangeLightOff(int i, int x, int y)
{
	if ( !lightflag && i != -1 )
	{
		LightList[i]._xoff = x;
		LightList[i]._lunx = LightList[i]._lx;
		LightList[i]._luny = LightList[i]._ly;
		LightList[i]._lunr = LightList[i]._lradius;
		LightList[i]._lunflag = 1;
		LightList[i]._yoff = y;
		dolighting = 1;
	}
}
// 642A18: using guessed type int dolighting;
// 646A28: using guessed type int lightflag;

void __fastcall ChangeLight(int i, int x, int y, int r)
{
	if ( !lightflag && i != -1 )
	{
		LightList[i]._lunx = LightList[i]._lx;
		LightList[i]._luny = LightList[i]._ly;
		LightList[i]._lunr = LightList[i]._lradius;
		LightList[i]._lunflag = 1;
		LightList[i]._lx = x;
		LightList[i]._ly = y;
		LightList[i]._lradius = r;
		dolighting = 1;
	}
}
// 642A18: using guessed type int dolighting;
// 646A28: using guessed type int lightflag;

void __cdecl ProcessLightList()
{
	int v0; // ebp
	int v1; // edi
	int v2; // esi
	int i; // esi
	int v4; // eax
	int v5; // ecx
	unsigned char v6; // bl
	char v7; // dl

	v0 = 0;
	if ( !lightflag )
	{
		if ( dolighting )
		{
			v1 = numlights;
			if ( numlights > 0 )
			{
				do
				{
					v2 = (unsigned char)lightactive[v0];
					if ( LightList[v2]._ldel )
						DoUnLight(LightList[v2]._lx, LightList[v2]._ly, LightList[v2]._lradius);
					if ( LightList[v2]._lunflag )
					{
						DoUnLight(LightList[v2]._lunx, LightList[v2]._luny, LightList[v2]._lunr);
						LightList[v2]._lunflag = 0;
					}
					++v0;
				}
				while ( v0 < v1 );
			}
			for ( i = 0; i < v1; ++i )
			{
				v4 = (unsigned char)lightactive[i];
				if ( !LightList[v4]._ldel )
					DoLighting(
						LightList[v4]._lx,
						LightList[v4]._ly,
						LightList[v4]._lradius,
						(unsigned char)lightactive[i]);
			}
			v5 = 0;
			if ( v1 > 0 )
			{
				do
				{
					v6 = lightactive[v5];
					if ( LightList[v6]._ldel )
					{
						v7 = lightactive[--v1];
						lightactive[v1] = v6;
						lightactive[v5] = v7;
					}
					else
					{
						++v5;
					}
				}
				while ( v5 < v1 );
				numlights = v1;
			}
		}
		dolighting = 0;
	}
}
// 642A18: using guessed type int dolighting;
// 646A28: using guessed type int lightflag;

void __cdecl SavePreLighting()
{
	memcpy(dTransVal2, dTransVal, 0x3100u);
}

void __cdecl InitVision()
{
	numvision = 0;
	dovision = 0;
	visionid = 1;
	if ( TransVal > 0 )
		memset(TransList, 0, TransVal);
}
// 5A5590: using guessed type char TransVal;
// 642A0C: using guessed type int dovision;

int __fastcall AddVision(int x, int y, int r, BOOL mine)
{
	int vid; // eax
	int v5; // esi

	vid = r;
	if ( numvision < 32 )
	{
		v5 = numvision;
		dovision = 1;
		VisionList[v5]._lx = x;
		VisionList[v5]._ly = y;
		VisionList[v5]._lradius = r;
		vid = visionid++;
		VisionList[v5]._lid = vid;
		VisionList[v5]._ldel = 0;
		++numvision;
		VisionList[v5]._lunflag = 0;
		VisionList[v5]._lflags = mine != 0;
	}
	return vid;
}
// 642A0C: using guessed type int dovision;

void __fastcall ChangeVisionRadius(int id, int r)
{
	int i; // esi

	if ( numvision > 0 )
	{
		for(i = 0; i < numvision; i++)
		{
			if ( VisionList[i]._lid == id )
			{
				VisionList[i]._lunflag = 1;
				VisionList[i]._lunx = VisionList[i]._lx;
				VisionList[i]._luny = VisionList[i]._ly;
				VisionList[i]._lunr = VisionList[i]._lradius;
				VisionList[i]._lradius = r;
				dovision = 1;
			}
		}
	}
}
// 642A0C: using guessed type int dovision;

void __fastcall ChangeVisionXY(int id, int x, int y)
{
	int i; // esi

	if ( numvision > 0 )
	{
		for(i = 0; i < numvision; i++)
		{
			if ( VisionList[i]._lid == id )
			{
				VisionList[i]._lunflag = 1;
				VisionList[i]._lunx = VisionList[i]._lx;
				VisionList[i]._luny = VisionList[i]._ly;
				VisionList[i]._lunr = VisionList[i]._lradius;
				VisionList[i]._lx = x;
				VisionList[i]._ly = y;
				dovision = 1;
			}
		}
	}
}
// 642A0C: using guessed type int dovision;

void __cdecl ProcessVisionList()
{
	bool delflag; // ecx
	int i;

	if ( dovision )
	{
		for(i = 0; i < numvision; i++)
		{
			if ( VisionList[i]._ldel )
				DoUnVision(VisionList[i]._lx, VisionList[i]._ly, VisionList[i]._lradius);
			if ( VisionList[i]._lunflag )
			{
				DoUnVision(VisionList[i]._lunx, VisionList[i]._luny, VisionList[i]._lunr);
				VisionList[i]._lunflag = 0;
			}
		}

		if ( TransVal > 0 )
			memset(TransList, 0, TransVal);

		for(i = 0; i < numvision; i++)
		{
			if ( !VisionList[i]._ldel )
				DoVision(VisionList[i]._lx, VisionList[i]._ly, VisionList[i]._lradius, VisionList[i]._lflags & 1, VisionList[i]._lflags & 1);
		}

		do
		{
			delflag = 0;
			if ( numvision <= 0 )
				break;
			for(i = 0; i < numvision; i++)
			{
				if ( VisionList[i]._ldel )
				{
					--numvision;
					if ( numvision > 0 && i != numvision )
						qmemcpy(&VisionList[i], &VisionList[numvision], sizeof(LightListStruct));
					delflag = 1;
				}
			}
		}
		while ( delflag );
	}

	dovision = 0;
}
// 5A5590: using guessed type char TransVal;
// 642A0C: using guessed type int dovision;

void __cdecl lighting_color_cycling()
{
	char *v0; // eax
	signed int v1; // ebx
	char *v2; // eax
	char *v3; // edi
	char v4; // dl
	const void *v5; // esi

	if ( leveltype == DTYPE_HELL )
	{
		v0 = pLightTbl;
		if ( (light4flag != 0 ? 4 : 16) > 0 )
		{
			v1 = light4flag != 0 ? 4 : 16;
			do
			{
				v2 = v0 + 1;
				v3 = v2;
				v4 = *v2;
				v5 = v2 + 1;
				v2 += 30;
				qmemcpy(v3, v5, 0x1Eu);
				*v2 = v4;
				v0 = v2 + 225;
				--v1;
			}
			while ( v1 );
		}
	}
}
// 525728: using guessed type int light4flag;
// 5BB1ED: using guessed type char leveltype;

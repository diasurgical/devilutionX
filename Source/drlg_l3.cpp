/**
 * @file drlg_l3.cpp
 *
 * Implementation of the caves level generation algorithms.
 */

#include <algorithm>

#include "engine/load_file.hpp"
#include "engine/random.hpp"
#include "gendung.h"
#include "lighting.h"
#include "monster.h"
#include "objdat.h"
#include "objects.h"
#include "quests.h"
#include "setmaps.h"

namespace devilution {

namespace {

/** This will be true if a lava pool has been generated for the level */
uint8_t lavapool;
int lockoutcnt;
bool lockout[DMAXX][DMAXY];

/**
 * A lookup table for the 16 possible patterns of a 2x2 area,
 * where each cell either contains a SW wall or it doesn't.
 */
const BYTE L3ConvTbl[16] = { 8, 11, 3, 10, 1, 9, 12, 12, 6, 13, 4, 13, 2, 14, 5, 7 };
/** Miniset: Stairs up. */
const BYTE L3UP[] = {
	// clang-format off
	3, 3, // width, height

	 8,  8, 0, // search
	10, 10, 0,
	 7,  7, 0,

	51, 50, 0, // replace
	48, 49, 0,
	 0,  0, 0,
	// clang-format on
};
const BYTE L6UP[] = {
	// clang-format off
	3, 3, // width, height

	 8,  8, 0, // search
	10, 10, 0,
	 7,  7, 0,

	20, 19, 0, // replace
	17, 18, 0,
	 0,  0, 0,
	// clang-format on
};
/** Miniset: Stairs down. */
const BYTE L3DOWN[] = {
	// clang-format off
	3, 3, // width, height

	8, 9, 7, // search
	8, 9, 7,
	0, 0, 0,

	0, 47, 0, // replace
	0, 46, 0,
	0,  0, 0,
	// clang-format on
};
const BYTE L6DOWN[] = {
	// clang-format off
	3, 3, // width, height

	8, 9, 7, // search
	8, 9, 7,
	0, 0, 0,

	0, 16, 0, // replace
	0, 15, 0,
	0,  0, 0,
	// clang-format on
};
/** Miniset: Stairs up to town. */
const BYTE L3HOLDWARP[] = {
	// clang-format off
	3, 3, // width, height

	 8,  8, 0, // search
	10, 10, 0,
	 7,  7, 0,

	125, 125, 0, // replace
	125, 125, 0,
	  0,   0, 0,
	// clang-format on
};
const BYTE L6HOLDWARP[] = {
	// clang-format off
	3, 3, // width, height

	 8,  8, 0, // search
	10, 10, 0,
	 7,  7, 0,

	24, 23, 0, // replace
	21, 22, 0,
	 0,  0, 0,
	// clang-format on
};
/** Miniset: Stalagmite white stalactite 1. */
const BYTE L3TITE1[] = {
	// clang-format off
	4, 4, // width, height

	7, 7, 7, 7, // search
	7, 7, 7, 7,
	7, 7, 7, 7,
	7, 7, 7, 7,

	0,  0,  0, 0, // replace
	0, 57, 58, 0,
	0, 56, 55, 0,
	0,  0,  0, 0,
	// clang-format on
};
/** Miniset: Stalagmite white stalactite 2. */
const BYTE L3TITE2[] = {
	// clang-format off
	4, 4, // width, height

	7, 7, 7, 7, // search
	7, 7, 7, 7,
	7, 7, 7, 7,
	7, 7, 7, 7,

	0,  0,  0, 0, // replace
	0, 61, 62, 0,
	0, 60, 59, 0,
	0,  0,  0, 0,
	// clang-format on
};
/** Miniset: Stalagmite white stalactite 3. */
const BYTE L3TITE3[] = {
	// clang-format off
	4, 4, // width, height

	7, 7, 7, 7, // search
	7, 7, 7, 7,
	7, 7, 7, 7,
	7, 7, 7, 7,

	0,  0,  0, 0, // replace
	0, 65, 66, 0,
	0, 64, 63, 0,
	0,  0,  0, 0,
	// clang-format on
};
/** Miniset: Stalagmite white stalactite horizontal. */
const BYTE L3TITE6[] = {
	// clang-format off
	5, 4, // width, height

	7, 7, 7, 7, 7, // search
	7, 7, 7, 0, 7,
	7, 7, 7, 0, 7,
	7, 7, 7, 7, 7,

	0,  0,  0,  0, 0, // replace
	0, 77, 78,  0, 0,
	0, 76, 74, 75, 0,
	0,  0,  0,  0, 0,
	// clang-format on
};
/** Miniset: Stalagmite white stalactite vertical. */
const BYTE L3TITE7[] = {
	// clang-format off
	4, 5, // width, height

	7, 7, 7, 7, // search
	7, 7, 0, 7,
	7, 7, 7, 7,
	7, 7, 7, 7,
	7, 7, 7, 7,

	0,  0,  0, 0, // replace
	0, 83,  0, 0,
	0, 82, 80, 0,
	0, 81, 79, 0,
	0,  0,  0, 0,
	// clang-format on
};
/** Miniset: Stalagmite 1. */
const BYTE L3TITE8[] = {
	// clang-format off
	3, 3, // width, height

	7, 7, 7, // search
	7, 7, 7,
	7, 7, 7,

	0,  0, 0, // replace
	0, 52, 0,
	0,  0, 0,
	// clang-format on
};
/** Miniset: Stalagmite 2. */
const BYTE L3TITE9[] = {
	// clang-format off
	3, 3, // width, height

	7, 7, 7, // search
	7, 7, 7,
	7, 7, 7,

	0,  0, 0, // replace
	0, 53, 0,
	0,  0, 0,
	// clang-format on
};
/** Miniset: Stalagmite 3. */
const BYTE L3TITE10[] = {
	// clang-format off
	3, 3, // width, height

	7, 7, 7, // search
	7, 7, 7,
	7, 7, 7,

	0,  0, 0, // replace
	0, 54, 0,
	0,  0, 0,
	// clang-format on
};
/** Miniset: Stalagmite 4. */
const BYTE L3TITE11[] = {
	// clang-format off
	3, 3, // width, height

	7, 7, 7, // search
	7, 7, 7,
	7, 7, 7,

	0,  0, 0, // replace
	0, 67, 0,
	0,  0, 0,
	// clang-format on
};
/** Miniset: Stalagmite on vertical wall. */
const BYTE L3TITE12[] = {
	// clang-format off
	2, 1, // width, height

	9, 7, // search

	68, 0, // replace
	// clang-format on
};
/** Miniset: Stalagmite on horizontal wall. */
const BYTE L3TITE13[] = {
	// clang-format off
	1, 2, // width, height

	10, // search
	 7,

	69, // replace
	 0,
	// clang-format on
};
/** Miniset: Cracked vertical wall 1. */
const BYTE L3CREV1[] = {
	// clang-format off
	2, 1, // width, height

	8, 7, // search

	84, 85, // replace
	// clang-format on
};
/** Miniset: Cracked vertical wall - north corner. */
const BYTE L3CREV2[] = {
	// clang-format off
	2, 1, // width, height

	8, 11, // search

	86, 87, // replace
	// clang-format on
};
/** Miniset: Cracked horizontal wall 1. */
const BYTE L3CREV3[] = {
	// clang-format off
	1, 2, // width, height

	 8, // search
	10,

	89, // replace
	88,
	// clang-format on
};
/** Miniset: Cracked vertical wall 2. */
const BYTE L3CREV4[] = {
	// clang-format off
	2, 1, // width, height

	8, 7, // search

	90, 91, // replace
	// clang-format on
};
/** Miniset: Cracked horizontal wall - north corner. */
const BYTE L3CREV5[] = {
	// clang-format off
	1, 2, // width, height

	 8, // search
	11,

	92, // replace
	93,
	// clang-format on
};
/** Miniset: Cracked horizontal wall 2. */
const BYTE L3CREV6[] = {
	// clang-format off
	1, 2, // width, height

	 8, // search
	10,

	95, // replace
	94,
	// clang-format on
};
/** Miniset: Cracked vertical wall - west corner. */
const BYTE L3CREV7[] = {
	// clang-format off
	2, 1, // width, height

	8, 7, // search

	96, 101, // replace
	// clang-format on
};
/** Miniset: Cracked horizontal wall - north. */
const BYTE L3CREV8[] = {
	// clang-format off
	1, 2, // width, height

	2, // search
	8,

	102, // replace
	 97,
	// clang-format on
};
/** Miniset: Cracked vertical wall - east corner. */
const BYTE L3CREV9[] = {
	// clang-format off
	2, 1, // width, height

	3, 8, // search

	103, 98, // replace
	// clang-format on
};
/** Miniset: Cracked vertical wall - west. */
const BYTE L3CREV10[] = {
	// clang-format off
	2, 1, // width, height

	4, 8, // search

	104, 99, // replace
	// clang-format on
};
/** Miniset: Cracked horizontal wall - south corner. */
const BYTE L3CREV11[] = {
	// clang-format off
	1, 2, // width, height

	6, // search
	8,

	105, // replace
	100,
	// clang-format on
};
/** Miniset: Replace broken wall with floor 1. */
const BYTE L3ISLE1[] = {
	// clang-format off
	2, 3, // width, height

	5, 14, // search
	4,  9,
	13, 12,

	7, 7, // replace
	7, 7,
	7, 7,
	// clang-format on
};
/** Miniset: Replace small wall with floor 2. */
const BYTE L3ISLE2[] = {
	// clang-format off
	3, 2, // width, height

	 5,  2, 14, // search
	13, 10, 12,

	7, 7, 7, // replace
	7, 7, 7,
	// clang-format on
};
/** Miniset: Replace small wall with lava 1. */
const BYTE L3ISLE3[] = {
	// clang-format off
	2, 3, // width, height

	 5, 14, // search
	 4,  9,
	13, 12,

	29, 30, // replace
	25, 28,
	31, 32,
	// clang-format on
};
/** Miniset: Replace small wall with lava 2. */
const BYTE L3ISLE4[] = {
	// clang-format off
	3, 2, // width, height

	 5,  2, 14, // search
	13, 10, 12,

	29, 26, 30, // replace
	31, 27, 32,
	// clang-format on
};
/** Miniset: Replace small wall with floor 3. */
const BYTE L3ISLE5[] = {
	// clang-format off
	2, 2, // width, height

	 5, 14, // search
	13, 12,

	7, 7, // replace
	7, 7,
	// clang-format on
};
/** Miniset: Use random floor tile 1. */
const BYTE L3XTRA1[] = {
	// clang-format off
	1, 1, // width, height

	7, // search

	106, // replace
	// clang-format on
};
/** Miniset: Use random floor tile 2. */
const BYTE L3XTRA2[] = {
	// clang-format off
	1, 1, // width, height

	7, // search

	107, // replace
	// clang-format on
};
/** Miniset: Use random floor tile 3. */
const BYTE L3XTRA3[] = {
	// clang-format off
	1, 1, // width, height

	7, // search

	108, // replace
	// clang-format on
};
/** Miniset: Use random horizontal wall tile. */
const BYTE L3XTRA4[] = {
	// clang-format off
	1, 1, // width, height

	9, // search

	109, // replace
	// clang-format on
};
/** Miniset: Use random vertical wall tile. */
const BYTE L3XTRA5[] = {
	// clang-format off
	1, 1, // width, height

	10, // search

	110, // replace
	// clang-format on
};

/** Miniset: Anvil of Fury island. */
const BYTE L3ANVIL[] = {
	// clang-format off
	11, 11, // width, height

	7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, // search
	7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7,
	7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7,
	7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7,
	7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7,
	7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7,
	7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7,
	7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7,
	7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7,
	7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7,
	7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7,

	0,  0,  0,  0,  0,  0,  0,  0,  0,  0, 0, // replace
	0,  0, 29, 26, 26, 26, 26, 26, 30,  0, 0,
	0, 29, 34, 33, 33, 37, 36, 33, 35, 30, 0,
	0, 25, 33, 37, 27, 32, 31, 36, 33, 28, 0,
	0, 25, 37, 32,  7,  7,  7, 31, 27, 32, 0,
	0, 25, 28,  7,  7,  7,  7,  2,  2,  2, 0,
	0, 25, 35, 30,  7,  7,  7, 29, 26, 30, 0,
	0, 25, 33, 35, 26, 30, 29, 34, 33, 28, 0,
	0, 31, 36, 33, 33, 35, 34, 33, 37, 32, 0,
	0,  0, 31, 27, 27, 27, 27, 27, 32,  0, 0,
	0,  0,  0,  0,  0,  0,  0,  0,  0,  0, 0,
	// clang-format on
};
const BYTE HivePattern1[] = { 1, 1, 8, 25 };
const BYTE HivePattern2[] = { 1, 1, 8, 26 };
const BYTE HivePattern3[] = { 1, 1, 8, 27 };
const BYTE HivePattern4[] = { 1, 1, 8, 28 };
const BYTE HivePattern5[] = { 1, 1, 7, 29 };
const BYTE HivePattern6[] = { 1, 1, 7, 30 };
const BYTE HivePattern7[] = { 1, 1, 7, 31 };
const BYTE HivePattern8[] = { 1, 1, 7, 32 };
const BYTE HivePattern9[] = {
	// clang-format off
	3, 3, // width, height

	7, 7, 7, // search
	7, 7, 7,
	7, 7, 7,

	0,   0, 0, // replace
	0, 126, 0,
	0,   0, 0,
	// clang-format on
};
const BYTE HivePattern10[] = {
	// clang-format off
	3, 3, // width, height

	7, 7, 7, // search
	7, 7, 7,
	7, 7, 7,

	0,   0, 0, // replace
	0, 124, 0,
	0,   0, 0,
	// clang-format on
};
const BYTE HivePattern11[] = { 1, 1, 9, 33 };
const BYTE HivePattern12[] = { 1, 1, 9, 34 };
const BYTE HivePattern13[] = { 1, 1, 9, 35 };
const BYTE HivePattern14[] = { 1, 1, 9, 36 };
const BYTE HivePattern15[] = { 1, 1, 9, 37 };
const BYTE HivePattern16[] = { 1, 1, 11, 38 };
const BYTE HivePattern17[] = { 1, 1, 10, 39 };
const BYTE HivePattern18[] = { 1, 1, 10, 40 };
const BYTE HivePattern19[] = { 1, 1, 10, 41 };
const BYTE HivePattern20[] = { 1, 1, 10, 42 };
const BYTE HivePattern21[] = { 1, 1, 10, 43 };
const BYTE HivePattern22[] = { 1, 1, 11, 44 };
const BYTE HivePattern23[] = { 1, 1, 9, 45 };
const BYTE HivePattern24[] = { 1, 1, 9, 46 };
const BYTE HivePattern25[] = { 1, 1, 10, 47 };
const BYTE HivePattern26[] = { 1, 1, 10, 48 };
const BYTE HivePattern27[] = { 1, 1, 11, 49 };
const BYTE HivePattern28[] = { 1, 1, 11, 50 };
const BYTE HivePattern29[] = {
	// clang-format off
	3, 3, // width, height

	7, 7, 7, // search
	7, 7, 7,
	7, 7, 7,

	67,  0, 0, // replace
	66, 51, 0,
	 0,  0, 0,
	// clang-format on
};
const BYTE HivePattern30[] = {
	// clang-format off
	3, 3, // width, height

	7, 7, 7, // search
	7, 7, 7,
	7, 7, 7,

	69,  0, 0, // replace
	68, 52, 0,
	 0,  0, 0,
	// clang-format on
};
const BYTE HivePattern31[] = {
	// clang-format off
	3, 3, // width, height

	7, 7, 7, // search
	7, 7, 7,
	7, 7, 7,

	70,  0, 0, // replace
	71, 53, 0,
	 0,  0, 0,
	// clang-format on
};
const BYTE HivePattern32[] = {
	// clang-format off
	3, 3, // width, height

	7, 7, 7, // search
	7, 7, 7,
	7, 7, 7,

	73,  0, 0, // replace
	72, 54, 0,
	 0,  0, 0,
	// clang-format on
};
const BYTE HivePattern33[] = {
	// clang-format off
	3, 3, // width, height

	7, 7, 7, // search
	7, 7, 7,
	7, 7, 7,

	75,  0, 0, // replace
	74, 55, 0,
	 0,  0, 0,
	// clang-format on
};
const BYTE HivePattern34[] = {
	// clang-format off
	3, 3, // width, height

	7, 7, 7, // search
	7, 7, 7,
	7, 7, 7,

	77,  0, 0, // replace
	76, 56, 0,
	 0,  0, 0,
	// clang-format on
};
const BYTE HivePattern35[] = {
	// clang-format off
	3, 3, // width, height

	7, 7, 7, // search
	7, 7, 7,
	7, 7, 7,

	79,  0, 0, // replace
	78, 57, 0,
	 0,  0, 0,
	// clang-format on
};
const BYTE HivePattern36[] = {
	// clang-format off
	3, 3, // width, height

	7, 7, 7, // search
	7, 7, 7,
	7, 7, 7,

	81,  0, 0, // replace
	80, 58, 0,
	 0,  0, 0,
	// clang-format on
};
const BYTE HivePattern37[] = {
	// clang-format off
	3, 3, // width, height

	7, 7, 7, // search
	7, 7, 7,
	7, 7, 7,

	83,  0, 0, // replace
	82, 59, 0,
	 0,  0, 0,
	// clang-format on
};
const BYTE HivePattern38[] = {
	// clang-format off
	3, 3, // width, height

	7, 7, 7, // search
	7, 7, 7,
	7, 7, 7,

	84,  0, 0, // replace
	85, 60, 0,
	 0,  0, 0,
	// clang-format on
};
const BYTE L6ISLE1[] = {
	// clang-format off
	2, 3, // width, height

	 5, 14, // search
	 4,  9,
	13, 12,

	7, 7, // replace
	7, 7,
	7, 7,
	// clang-format on
};
const BYTE L6ISLE2[] = {
	// clang-format off
	3, 2, // width, height

	 5,  2, 14, // search
	13, 10, 12,

	7, 7, 7, // replace
	7, 7, 7,
	// clang-format on
};
const BYTE L6ISLE3[] = {
	// clang-format off
	2, 3, // width, height

	 5, 14, // search
	 4,  9,
	13, 12,

	107, 115, // replace
	119, 122,
	131, 123,
	// clang-format on
};
const BYTE L6ISLE4[] = {
	// clang-format off
	3, 2, // width, height

	 5,  2, 14, // search
	13, 10, 12,

	107, 120, 115, // replace
	131, 121, 123,
	// clang-format on
};
const BYTE L6ISLE5[] = {
	// clang-format off
	2, 2, // width, height

	 5, 14, // search
	13, 12,

	7, 7, // replace
	7, 7,
	// clang-format on
};
const BYTE HivePattern39[] = {
	// clang-format off
	4, 4, // width, height

	7, 7, 7, 7, // search
	7, 7, 7, 7,
	7, 7, 7, 7,
	7, 7, 7, 7,

	7,   7,   7, 7, // replace
	7, 107, 115, 7,
	7, 131, 123, 7,
	7,   7,   7, 7,
	// clang-format on
};
const BYTE HivePattern40[] = {
	// clang-format off
	4, 4, // width, height

	7, 7, 7, 7, // search
	7, 7, 7, 7,
	7, 7, 7, 7,
	7, 7, 7, 7,

	7,   7,   7, 7, // replace
	7,   7, 108, 7,
	7, 109, 112, 7,
	7,   7,   7, 7,
	// clang-format on
};
const BYTE HivePattern41[] = {
	// clang-format off
	4, 5, // width, height

	7, 7, 7, 7, // search
	7, 7, 7, 7,
	7, 7, 7, 7,
	7, 7, 7, 7,
	7, 7, 7, 7,

	7,   7,   7, 7, // replace
	7, 107, 115, 7,
	7, 119, 122, 7,
	7, 131, 123, 7,
	7,   7,   7, 7,
	// clang-format on
};
const BYTE HivePattern42[] = {
	// clang-format off
	4, 5, // width, height

	7, 7, 7, 7, // search
	7, 7, 7, 7,
	7, 7, 7, 7,
	7, 7, 7, 7,
	7, 7, 7, 7,

	7,   7,   7, 7, // replace
	7, 126, 108, 7,
	7,   7, 117, 7,
	7, 109, 112, 7,
	7,   7,   7, 7,
	// clang-format on
};

void InitDungeonFlags()
{
	memset(dungeon, 0, sizeof(dungeon));

	for (int j = 0; j < DMAXY; j++) {
		for (int i = 0; i < DMAXX; i++) {
			dungeon[i][j] = 0;
			dflags[i][j] = 0;
		}
	}
}

bool FillRoom(int x1, int y1, int x2, int y2)
{
	if (x1 <= 1 || x2 >= 34 || y1 <= 1 || y2 >= 38) {
		return false;
	}

	int v = 0;
	for (int j = y1; j <= y2; j++) {
		for (int i = x1; i <= x2; i++) {
			v += dungeon[i][j];
		}
	}

	if (v != 0) {
		return false;
	}

	for (int j = y1 + 1; j < y2; j++) {
		for (int i = x1 + 1; i < x2; i++) {
			dungeon[i][j] = 1;
		}
	}
	for (int j = y1; j <= y2; j++) {
		if (GenerateRnd(2) != 0) {
			dungeon[x1][j] = 1;
		}
		if (GenerateRnd(2) != 0) {
			dungeon[x2][j] = 1;
		}
	}
	for (int i = x1; i <= x2; i++) {
		if (GenerateRnd(2) != 0) {
			dungeon[i][y1] = 1;
		}
		if (GenerateRnd(2) != 0) {
			dungeon[i][y2] = 1;
		}
	}

	return true;
}

void CreateBlock(int x, int y, int obs, int dir)
{
	int x1;
	int y1;
	int x2;
	int y2;

	int blksizex = GenerateRnd(2) + 3;
	int blksizey = GenerateRnd(2) + 3;

	if (dir == 0) {
		y2 = y - 1;
		y1 = y2 - blksizey;
		if (blksizex < obs) {
			x1 = GenerateRnd(blksizex) + x;
		}
		if (blksizex == obs) {
			x1 = x;
		}
		if (blksizex > obs) {
			x1 = x - GenerateRnd(blksizex);
		}
		x2 = blksizex + x1;
	}
	if (dir == 1) {
		x1 = x + 1;
		x2 = x1 + blksizex;
		if (blksizey < obs) {
			y1 = GenerateRnd(blksizey) + y;
		}
		if (blksizey == obs) {
			y1 = y;
		}
		if (blksizey > obs) {
			y1 = y - GenerateRnd(blksizey);
		}
		y2 = y1 + blksizey;
	}
	if (dir == 2) {
		y1 = y + 1;
		y2 = y1 + blksizey;
		if (blksizex < obs) {
			x1 = GenerateRnd(blksizex) + x;
		}
		if (blksizex == obs) {
			x1 = x;
		}
		if (blksizex > obs) {
			x1 = x - GenerateRnd(blksizex);
		}
		x2 = blksizex + x1;
	}
	if (dir == 3) {
		x2 = x - 1;
		x1 = x2 - blksizex;
		if (blksizey < obs) {
			y1 = GenerateRnd(blksizey) + y;
		}
		if (blksizey == obs) {
			y1 = y;
		}
		if (blksizey > obs) {
			y1 = y - GenerateRnd(blksizey);
		}
		y2 = y1 + blksizey;
	}

	if (FillRoom(x1, y1, x2, y2)) {
		int contflag = GenerateRnd(4);
		if (contflag != 0 && dir != 2) {
			CreateBlock(x1, y1, blksizey, 0);
		}
		if (contflag != 0 && dir != 3) {
			CreateBlock(x2, y1, blksizex, 1);
		}
		if (contflag != 0 && dir != 0) {
			CreateBlock(x1, y2, blksizey, 2);
		}
		if (contflag != 0 && dir != 1) {
			CreateBlock(x1, y1, blksizex, 3);
		}
	}
}

void FloorArea(int x1, int y1, int x2, int y2)
{
	for (int j = y1; j <= y2; j++) {
		for (int i = x1; i <= x2; i++) {
			dungeon[i][j] = 1;
		}
	}
}

void FillDiagonals()
{
	for (int j = 0; j < DMAXY - 1; j++) {
		for (int i = 0; i < DMAXX - 1; i++) {
			int v = dungeon[i + 1][j + 1] + 2 * dungeon[i][j + 1] + 4 * dungeon[i + 1][j] + 8 * dungeon[i][j];
			if (v == 6) {
				if (GenerateRnd(2) == 0) {
					dungeon[i][j] = 1;
				} else {
					dungeon[i + 1][j + 1] = 1;
				}
			}
			if (v == 9) {
				if (GenerateRnd(2) == 0) {
					dungeon[i + 1][j] = 1;
				} else {
					dungeon[i][j + 1] = 1;
				}
			}
		}
	}
}

void FillSingles()
{
	for (int j = 1; j < DMAXY - 1; j++) {
		for (int i = 1; i < DMAXX - 1; i++) {
			if (dungeon[i][j] == 0
			    && dungeon[i][j - 1] + dungeon[i - 1][j - 1] + dungeon[i + 1][j - 1] == 3
			    && dungeon[i + 1][j] + dungeon[i - 1][j] == 2
			    && dungeon[i][j + 1] + dungeon[i - 1][j + 1] + dungeon[i + 1][j + 1] == 3) {
				dungeon[i][j] = 1;
			}
		}
	}
}

void FillStraights()
{
	int xc;
	int yc;

	for (int j = 0; j < DMAXY - 1; j++) {
		int xs = 0;
		for (int i = 0; i < 37; i++) {
			if (dungeon[i][j] == 0 && dungeon[i][j + 1] == 1) {
				if (xs == 0) {
					xc = i;
				}
				xs++;
			} else {
				if (xs > 3 && GenerateRnd(2) != 0) {
					for (int k = xc; k < i; k++) {
						int rv = GenerateRnd(2);
						dungeon[k][j] = rv;
					}
				}
				xs = 0;
			}
		}
	}
	for (int j = 0; j < DMAXY - 1; j++) {
		int xs = 0;
		for (int i = 0; i < 37; i++) {
			if (dungeon[i][j] == 1 && dungeon[i][j + 1] == 0) {
				if (xs == 0) {
					xc = i;
				}
				xs++;
			} else {
				if (xs > 3 && GenerateRnd(2) != 0) {
					for (int k = xc; k < i; k++) {
						int rv = GenerateRnd(2);
						dungeon[k][j + 1] = rv;
					}
				}
				xs = 0;
			}
		}
	}
	for (int i = 0; i < DMAXX - 1; i++) {
		int ys = 0;
		for (int j = 0; j < 37; j++) {
			if (dungeon[i][j] == 0 && dungeon[i + 1][j] == 1) {
				if (ys == 0) {
					yc = j;
				}
				ys++;
			} else {
				if (ys > 3 && GenerateRnd(2) != 0) {
					for (int k = yc; k < j; k++) {
						int rv = GenerateRnd(2);
						dungeon[i][k] = rv;
					}
				}
				ys = 0;
			}
		}
	}
	for (int i = 0; i < DMAXX - 1; i++) {
		int ys = 0;
		for (int j = 0; j < 37; j++) {
			if (dungeon[i][j] == 1 && dungeon[i + 1][j] == 0) {
				if (ys == 0) {
					yc = j;
				}
				ys++;
			} else {
				if (ys > 3 && GenerateRnd(2) != 0) {
					for (int k = yc; k < j; k++) {
						int rv = GenerateRnd(2);
						dungeon[i + 1][k] = rv;
					}
				}
				ys = 0;
			}
		}
	}
}

void Edges()
{
	for (int j = 0; j < DMAXY; j++) {
		dungeon[DMAXX - 1][j] = 0;
	}
	for (int i = 0; i < DMAXX; i++) { // NOLINT(modernize-loop-convert)
		dungeon[i][DMAXY - 1] = 0;
	}
}

int GetFloorArea()
{
	int gfa = 0;

	for (int j = 0; j < DMAXY; j++) {
		for (int i = 0; i < DMAXX; i++) { // NOLINT(modernize-loop-convert)
			gfa += dungeon[i][j];
		}
	}

	return gfa;
}

void MakeMegas()
{
	for (int j = 0; j < DMAXY - 1; j++) {
		for (int i = 0; i < DMAXX - 1; i++) {
			int v = dungeon[i + 1][j + 1] + 2 * dungeon[i][j + 1] + 4 * dungeon[i + 1][j] + 8 * dungeon[i][j];
			if (v == 6) {
				int rv = GenerateRnd(2);
				if (rv == 0) {
					v = 12;
				} else {
					v = 5;
				}
			}
			if (v == 9) {
				int rv = GenerateRnd(2);
				if (rv == 0) {
					v = 13;
				} else {
					v = 14;
				}
			}
			dungeon[i][j] = L3ConvTbl[v];
		}
		dungeon[DMAXX - 1][j] = 8;
	}
	for (int i = 0; i < DMAXX; i++) { // NOLINT(modernize-loop-convert)
		dungeon[i][DMAXY - 1] = 8;
	}
}

void River()
{
	int dir;
	int nodir;
	int river[3][100];
	int riveramt;

	int rivercnt = 0;
	int trys = 0;
	/// BUGFIX: pdir is uninitialized, add code `pdir = -1;`(fixed)
	int pdir = -1;

	while (trys < 200 && rivercnt < 4) {
		bool bail = false;
		while (!bail && trys < 200) {
			trys++;
			int rx = 0;
			int ry = 0;
			int i = 0;
			// BUGFIX: Replace with `(ry >= DMAXY || dungeon[rx][ry] < 25 || dungeon[rx][ry] > 28) && i < 100` (fixed)
			while ((ry >= DMAXY || dungeon[rx][ry] < 25 || dungeon[rx][ry] > 28) && i < 100) {
				rx = GenerateRnd(DMAXX);
				ry = GenerateRnd(DMAXY);
				i++;
				// BUGFIX: Move `ry < DMAXY` check before dungeon checks (fixed)
				while (ry < DMAXY && (dungeon[rx][ry] < 25 || dungeon[rx][ry] > 28)) {
					rx++;
					if (rx >= DMAXX) {
						rx = 0;
						ry++;
					}
				}
			}
			// BUGFIX: Continue if `ry >= DMAXY` (fixed)
			if (ry >= DMAXY)
				continue;
			if (i >= 100) {
				return;
			}
			switch (dungeon[rx][ry]) {
			case 25:
				dir = 3;
				nodir = 2;
				river[2][0] = 40;
				break;
			case 26:
				dir = 0;
				nodir = 1;
				river[2][0] = 38;
				break;
			case 27:
				dir = 1;
				nodir = 0;
				river[2][0] = 41;
				break;
			case 28:
				dir = 2;
				nodir = 3;
				river[2][0] = 39;
				break;
			}
			river[0][0] = rx;
			river[1][0] = ry;
			riveramt = 1;
			int nodir2 = 4;
			int dircheck = 0;
			while (dircheck < 4 && riveramt < 100) {
				int px = rx;
				int py = ry;
				if (dircheck == 0) {
					dir = GenerateRnd(4);
				} else {
					dir = (dir + 1) & 3;
				}
				dircheck++;
				while (dir == nodir || dir == nodir2) {
					dir = (dir + 1) & 3;
					dircheck++;
				}
				if (dir == 0 && ry > 0) {
					ry--;
				}
				if (dir == 1 && ry < DMAXY) {
					ry++;
				}
				if (dir == 2 && rx < DMAXX) {
					rx++;
				}
				if (dir == 3 && rx > 0) {
					rx--;
				}
				if (dungeon[rx][ry] == 7) {
					dircheck = 0;
					if (dir < 2) {
						river[2][riveramt] = (BYTE)GenerateRnd(2) + 17;
					}
					if (dir > 1) {
						river[2][riveramt] = (BYTE)GenerateRnd(2) + 15;
					}
					river[0][riveramt] = rx;
					river[1][riveramt] = ry;
					riveramt++;
					if ((dir == 0 && pdir == 2) || (dir == 3 && pdir == 1)) {
						if (riveramt > 2) {
							river[2][riveramt - 2] = 22;
						}
						if (dir == 0) {
							nodir2 = 1;
						} else {
							nodir2 = 2;
						}
					}
					if ((dir == 0 && pdir == 3) || (dir == 2 && pdir == 1)) {
						if (riveramt > 2) {
							river[2][riveramt - 2] = 21;
						}
						if (dir == 0) {
							nodir2 = 1;
						} else {
							nodir2 = 3;
						}
					}
					if ((dir == 1 && pdir == 2) || (dir == 3 && pdir == 0)) {
						if (riveramt > 2) {
							river[2][riveramt - 2] = 20;
						}
						if (dir == 1) {
							nodir2 = 0;
						} else {
							nodir2 = 2;
						}
					}
					if ((dir == 1 && pdir == 3) || (dir == 2 && pdir == 0)) {
						if (riveramt > 2) {
							river[2][riveramt - 2] = 19;
						}
						if (dir == 1) {
							nodir2 = 0;
						} else {
							nodir2 = 3;
						}
					}
					pdir = dir;
				} else {
					rx = px;
					ry = py;
				}
			}
			// BUGFIX: Check `ry >= 2` (fixed)
			if (dir == 0 && ry >= 2 && dungeon[rx][ry - 1] == 10 && dungeon[rx][ry - 2] == 8) {
				river[0][riveramt] = rx;
				river[1][riveramt] = ry - 1;
				river[2][riveramt] = 24;
				if (pdir == 2) {
					river[2][riveramt - 1] = 22;
				}
				if (pdir == 3) {
					river[2][riveramt - 1] = 21;
				}
				bail = true;
			}
			// BUGFIX: Check `ry + 2 < DMAXY` (fixed)
			if (dir == 1 && ry + 2 < DMAXY && dungeon[rx][ry + 1] == 2 && dungeon[rx][ry + 2] == 8) {
				river[0][riveramt] = rx;
				river[1][riveramt] = ry + 1;
				river[2][riveramt] = 42;
				if (pdir == 2) {
					river[2][riveramt - 1] = 20;
				}
				if (pdir == 3) {
					river[2][riveramt - 1] = 19;
				}
				bail = true;
			}
			// BUGFIX: Check `rx + 2 < DMAXX` (fixed)
			if (dir == 2 && rx + 2 < DMAXX && dungeon[rx + 1][ry] == 4 && dungeon[rx + 2][ry] == 8) {
				river[0][riveramt] = rx + 1;
				river[1][riveramt] = ry;
				river[2][riveramt] = 43;
				if (pdir == 0) {
					river[2][riveramt - 1] = 19;
				}
				if (pdir == 1) {
					river[2][riveramt - 1] = 21;
				}
				bail = true;
			}
			// BUGFIX: Check `rx >= 2` (fixed)
			if (dir == 3 && rx >= 2 && dungeon[rx - 1][ry] == 9 && dungeon[rx - 2][ry] == 8) {
				river[0][riveramt] = rx - 1;
				river[1][riveramt] = ry;
				river[2][riveramt] = 23;
				if (pdir == 0) {
					river[2][riveramt - 1] = 20;
				}
				if (pdir == 1) {
					river[2][riveramt - 1] = 22;
				}
				bail = true;
			}
		}
		if (bail && riveramt < 7) {
			bail = false;
		}
		if (bail) {
			int found = 0;
			int lpcnt = 0;
			int bridge;
			while (found == 0 && lpcnt < 30) {
				lpcnt++;
				bridge = GenerateRnd(riveramt);
				if ((river[2][bridge] == 15 || river[2][bridge] == 16)
				    && dungeon[river[0][bridge]][river[1][bridge] - 1] == 7
				    && dungeon[river[0][bridge]][river[1][bridge] + 1] == 7) {
					found = 1;
				}
				if ((river[2][bridge] == 17 || river[2][bridge] == 18)
				    && dungeon[river[0][bridge] - 1][river[1][bridge]] == 7
				    && dungeon[river[0][bridge] + 1][river[1][bridge]] == 7) {
					found = 2;
				}
				for (int i = 0; i < riveramt && found != 0; i++) {
					if (found == 1
					    && (river[1][bridge] - 1 == river[1][i] || river[1][bridge] + 1 == river[1][i])
					    && river[0][bridge] == river[0][i]) {
						found = 0;
					}
					if (found == 2
					    && (river[0][bridge] - 1 == river[0][i] || river[0][bridge] + 1 == river[0][i])
					    && river[1][bridge] == river[1][i]) {
						found = 0;
					}
				}
			}
			if (found != 0) {
				if (found == 1) {
					river[2][bridge] = 44;
				} else {
					river[2][bridge] = 45;
				}
				rivercnt++;
				for (bridge = 0; bridge <= riveramt; bridge++) {
					dungeon[river[0][bridge]][river[1][bridge]] = river[2][bridge];
				}
			} else {
				bail = false;
			}
		}
	}
}

bool Spawn(int x, int y, int *totarea);

bool SpawnEdge(int x, int y, int *totarea)
{
	BYTE i;
	static BYTE spawntable[15] = { 0x00, 0x0A, 0x43, 0x05, 0x2c, 0x06, 0x09, 0x00, 0x00, 0x1c, 0x83, 0x06, 0x09, 0x0A, 0x05 };

	if (*totarea > 40) {
		return true;
	}
	if (x < 0 || y < 0 || x >= DMAXX || y >= DMAXY) {
		return true;
	}
	if ((dungeon[x][y] & 0x80) != 0) {
		return false;
	}
	if (dungeon[x][y] > 15) {
		return true;
	}

	i = dungeon[x][y];
	dungeon[x][y] |= 0x80;
	*totarea += 1;

	if ((spawntable[i] & 8) != 0 && SpawnEdge(x, y - 1, totarea)) {
		return true;
	}
	if ((spawntable[i] & 4) != 0 && SpawnEdge(x, y + 1, totarea)) {
		return true;
	}
	if ((spawntable[i] & 2) != 0 && SpawnEdge(x + 1, y, totarea)) {
		return true;
	}
	if ((spawntable[i] & 1) != 0 && SpawnEdge(x - 1, y, totarea)) {
		return true;
	}
	if ((spawntable[i] & 0x80) != 0 && Spawn(x, y - 1, totarea)) {
		return true;
	}
	if ((spawntable[i] & 0x40) != 0 && Spawn(x, y + 1, totarea)) {
		return true;
	}
	if ((spawntable[i] & 0x20) != 0 && Spawn(x + 1, y, totarea)) {
		return true;
	}
	if ((spawntable[i] & 0x10) != 0 && Spawn(x - 1, y, totarea)) {
		return true;
	}

	return false;
}

bool Spawn(int x, int y, int *totarea)
{
	BYTE i;
	static BYTE spawntable[15] = { 0x00, 0x0A, 0x03, 0x05, 0x0C, 0x06, 0x09, 0x00, 0x00, 0x0C, 0x03, 0x06, 0x09, 0x0A, 0x05 };

	if (*totarea > 40) {
		return true;
	}
	if (x < 0 || y < 0 || x >= DMAXX || y >= DMAXY) {
		return true;
	}
	if ((dungeon[x][y] & 0x80) != 0) {
		return false;
	}
	if (dungeon[x][y] > 15) {
		return true;
	}

	i = dungeon[x][y];
	dungeon[x][y] |= 0x80;
	*totarea += 1;

	if (i != 8) {
		if ((spawntable[i] & 8) != 0 && SpawnEdge(x, y - 1, totarea)) {
			return true;
		}
		if ((spawntable[i] & 4) != 0 && SpawnEdge(x, y + 1, totarea)) {
			return true;
		}
		if ((spawntable[i] & 2) != 0 && SpawnEdge(x + 1, y, totarea)) {
			return true;
		}
		if ((spawntable[i] & 1) != 0 && SpawnEdge(x - 1, y, totarea)) {
			return true;
		}
	} else {
		if (Spawn(x + 1, y, totarea)) {
			return true;
		}
		if (Spawn(x - 1, y, totarea)) {
			return true;
		}
		if (Spawn(x, y + 1, totarea)) {
			return true;
		}
		if (Spawn(x, y - 1, totarea)) {
			return true;
		}
	}

	return false;
}

/**
 * Flood fills dirt and wall tiles looking for
 * an area of at most 40 tiles and disconnected from the map edge.
 * If it finds one, converts it to lava tiles and sets lavapool to true.
 */
void Pool()
{
	constexpr uint8_t Poolsub[15] = { 0, 35, 26, 36, 25, 29, 34, 7, 33, 28, 27, 37, 32, 31, 30 };

	for (int duny = 0; duny < DMAXY; duny++) {
		for (int dunx = 0; dunx < DMAXY; dunx++) {
			if (dungeon[dunx][duny] != 8) {
				continue;
			}
			dungeon[dunx][duny] |= 0x80;
			int totarea = 1;
			bool found = true;
			if (dunx + 1 < DMAXX) {
				found = Spawn(dunx + 1, duny, &totarea);
			}
			if (dunx - 1 > 0 && !found) {
				found = Spawn(dunx - 1, duny, &totarea);
			} else {
				found = true;
			}
			if (duny + 1 < DMAXY && !found) {
				found = Spawn(dunx, duny + 1, &totarea);
			} else {
				found = true;
			}
			if (duny - 1 > 0 && !found) {
				found = Spawn(dunx, duny - 1, &totarea);
			} else {
				found = true;
			}
			int poolchance = GenerateRnd(100);
			for (int j = std::max(duny - totarea, 0); j < std::min(duny + totarea, DMAXY); j++) {
				for (int i = std::max(dunx - totarea, 0); i < std::min(dunx + totarea, DMAXX); i++) {
					// BUGFIX: In the following swap the order to first do the
					// index checks and only then access dungeon[i][j] (fixed)
					if ((dungeon[i][j] & 0x80) != 0) {
						dungeon[i][j] &= ~0x80;
						if (totarea > 4 && poolchance < 25 && !found) {
							uint8_t k = Poolsub[dungeon[i][j]];
							if (k != 0 && k <= 37) {
								dungeon[i][j] = k;
							}
							lavapool = 1;
						}
					}
				}
			}
		}
	}
}

void PoolFix()
{
	for (int duny = 1; duny < DMAXY - 1; duny++) {     // BUGFIX: Change '0' to '1' and 'DMAXY' to 'DMAXY - 1' (fixed)
		for (int dunx = 1; dunx < DMAXX - 1; dunx++) { // BUGFIX: Change '0' to '1' and 'DMAXX' to 'DMAXX - 1' (fixed)
			if (dungeon[dunx][duny] == 8) {
				if (dungeon[dunx - 1][duny - 1] >= 25 && dungeon[dunx - 1][duny - 1] <= 41
				    && dungeon[dunx - 1][duny] >= 25 && dungeon[dunx - 1][duny] <= 41
				    && dungeon[dunx - 1][duny + 1] >= 25 && dungeon[dunx - 1][duny + 1] <= 41
				    && dungeon[dunx][duny - 1] >= 25 && dungeon[dunx][duny - 1] <= 41
				    && dungeon[dunx][duny + 1] >= 25 && dungeon[dunx][duny + 1] <= 41
				    && dungeon[dunx + 1][duny - 1] >= 25 && dungeon[dunx + 1][duny - 1] <= 41
				    && dungeon[dunx + 1][duny] >= 25 && dungeon[dunx + 1][duny] <= 41
				    && dungeon[dunx + 1][duny + 1] >= 25 && dungeon[dunx + 1][duny + 1] <= 41) {
					dungeon[dunx][duny] = 33;
				} else if (dungeon[dunx + 1][duny] == 35 || dungeon[dunx + 1][duny] == 37) {
					dungeon[dunx][duny] = 33;
				}
			}
		}
	}
}

bool PlaceMiniSet(const BYTE *miniset, int tmin, int tmax, int cx, int cy, bool setview)
{
	int sw = miniset[0];
	int sh = miniset[1];

	int numt = 1;
	if (tmax - tmin != 0) {
		numt = GenerateRnd(tmax - tmin) + tmin;
	}

	int sx = 0;
	int sy = 0;
	for (int i = 0; i < numt; i++) {
		sx = GenerateRnd(DMAXX - sw);
		sy = GenerateRnd(DMAXY - sh);
		bool abort = false;
		int bailcnt;

		for (bailcnt = 0; !abort && bailcnt < 200;) {
			bailcnt++;
			abort = true;
			if (cx != -1 && sx >= cx - sw && sx <= cx + 12) {
				sx = GenerateRnd(DMAXX - sw);
				sy = GenerateRnd(DMAXY - sh);
				abort = false;
			}
			if (cy != -1 && sy >= cy - sh && sy <= cy + 12) {
				sx = GenerateRnd(DMAXX - sw);
				sy = GenerateRnd(DMAXY - sh);
				abort = false;
			}
			int ii = 2;

			for (int yy = 0; yy < sh && abort; yy++) {
				for (int xx = 0; xx < sw && abort; xx++) {
					if (miniset[ii] != 0 && dungeon[xx + sx][yy + sy] != miniset[ii])
						abort = false;
					if (dflags[xx + sx][yy + sy] != 0)
						abort = false;
					ii++;
				}
			}

			if (!abort) {
				sx++;
				if (sx == DMAXX - sw) {
					sx = 0;
					sy++;
					if (sy == DMAXY - sh) {
						sy = 0;
					}
				}
			}
		}
		if (bailcnt >= 200) {
			return true;
		}
		int ii = sw * sh + 2;

		for (int yy = 0; yy < sh; yy++) {
			for (int xx = 0; xx < sw; xx++) {
				if (miniset[ii] != 0) {
					dungeon[xx + sx][yy + sy] = miniset[ii];
				}
				ii++;
			}
		}
	}

	if (setview) {
		ViewX = 2 * sx + 17;
		ViewY = 2 * sy + 19;
	}

	return false;
}

void PlaceMiniSetRandom(const BYTE *miniset, int rndper)
{
	int sw = miniset[0];
	int sh = miniset[1];

	for (int sy = 0; sy < DMAXX - sh; sy++) {
		for (int sx = 0; sx < DMAXY - sw; sx++) {
			bool found = true;
			int ii = 2;
			for (int yy = 0; yy < sh && found; yy++) {
				for (int xx = 0; xx < sw && found; xx++) {
					if (miniset[ii] != 0 && dungeon[xx + sx][yy + sy] != miniset[ii]) {
						found = false;
					}
					if (dflags[xx + sx][yy + sy] != 0) {
						found = false;
					}
					ii++;
				}
			}
			int kk = sw * sh + 2;
			if (found) {
				if (miniset[kk] >= 84 && miniset[kk] <= 100) {
					// BUGFIX: accesses to dungeon can go out of bounds (fixed)
					// BUGFIX: Comparisons vs 100 should use same tile as comparisons vs 84.
					if (sx - 1 >= 0 && dungeon[sx - 1][sy] >= 84 && dungeon[sx - 1][sy] <= 100) {
						found = false;
					}
					if (sx + 1 < 40 && sx - 1 >= 0 && dungeon[sx + 1][sy] >= 84 && dungeon[sx - 1][sy] <= 100) {
						found = false;
					}
					if (sy + 1 < 40 && sx - 1 >= 0 && dungeon[sx][sy + 1] >= 84 && dungeon[sx - 1][sy] <= 100) {
						found = false;
					}
					if (sy - 1 >= 0 && sx - 1 >= 0 && dungeon[sx][sy - 1] >= 84 && dungeon[sx - 1][sy] <= 100) {
						found = false;
					}
				}
			}
			if (found && GenerateRnd(100) < rndper) {
				for (int yy = 0; yy < sh; yy++) {
					for (int xx = 0; xx < sw; xx++) {
						if (miniset[kk] != 0) {
							dungeon[xx + sx][yy + sy] = miniset[kk];
						}
						kk++;
					}
				}
			}
		}
	}
}

bool HivePlaceSetRandom(const BYTE *miniset, int rndper)
{
	bool placed = false;
	int sw = miniset[0];
	int sh = miniset[1];

	for (int sy = 0; sy < DMAXX - sh; sy++) {
		for (int sx = 0; sx < DMAXY - sw; sx++) {
			bool found = true;
			int ii = 2;
			for (int yy = 0; yy < sh && found; yy++) {
				for (int xx = 0; xx < sw && found; xx++) {
					if (miniset[ii] != 0 && dungeon[xx + sx][yy + sy] != miniset[ii]) {
						found = false;
					}
					if (dflags[xx + sx][yy + sy] != 0) {
						found = false;
					}
					ii++;
				}
			}
			int kk = sw * sh + 2;
			if (found) {
				if (miniset[kk] >= 84 && miniset[kk] <= 100) {
					// BUGFIX: accesses to dungeon can go out of bounds
					// BUGFIX: Comparisons vs 100 should use same tile as comparisons vs 84.
					if (dungeon[sx - 1][sy] >= 84 && dungeon[sx - 1][sy] <= 100) {
						found = false;
					}
					if (dungeon[sx + 1][sy] >= 84 && dungeon[sx - 1][sy] <= 100) {
						found = false;
					}
					if (dungeon[sx][sy + 1] >= 84 && dungeon[sx - 1][sy] <= 100) {
						found = false;
					}
					if (dungeon[sx][sy - 1] >= 84 && dungeon[sx - 1][sy] <= 100) {
						found = false;
					}
				}
			}
			if (found && GenerateRnd(100) < rndper) {
				placed = true;
				for (int yy = 0; yy < sh; yy++) {
					for (int xx = 0; xx < sw; xx++) {
						if (miniset[kk] != 0) {
							dungeon[xx + sx][yy + sy] = miniset[kk];
						}
						kk++;
					}
				}
			}
		}
	}

	return placed;
}

bool FenceVerticalUp(int i, int y)
{
	if ((dungeon[i + 1][y] > 152 || dungeon[i + 1][y] < 130)
	    && (dungeon[i - 1][y] > 152 || dungeon[i - 1][y] < 130)) {
		if (IsAnyOf(dungeon[i][y], 7, 10, 126, 129, 134, 136)) {
			return true;
		}
	}

	return false;
}

bool FenceVerticalDown(int i, int y)
{
	if ((dungeon[i + 1][y] > 152 || dungeon[i + 1][y] < 130)
	    && (dungeon[i - 1][y] > 152 || dungeon[i - 1][y] < 130)) {
		if (IsAnyOf(dungeon[i][y], 2, 7, 134, 136)) {
			return true;
		}
	}

	return false;
}

bool FenceHorizontalLeft(int x, int j)
{
	if ((dungeon[x][j + 1] > 152 || dungeon[x][j + 1] < 130)
	    && (dungeon[x][j - 1] > 152 || dungeon[x][j - 1] < 130)) {
		if (IsAnyOf(dungeon[x][j], 7, 9, 121, 124, 135, 137)) {
			return true;
		}
	}

	return false;
}

bool FenceHorizontalRight(int x, int j)
{
	if ((dungeon[x][j + 1] > 152 || dungeon[x][j + 1] < 130)
	    && (dungeon[x][j - 1] > 152 || dungeon[x][j - 1] < 130)) {
		if (IsAnyOf(dungeon[x][j], 4, 7, 135, 137)) {
			return true;
		}
	}

	return false;
}

void AddFenceDoors()
{
	for (int j = 0; j < DMAXY; j++) {
		for (int i = 0; i < DMAXX; i++) {
			if (dungeon[i][j] == 7) {
				if (dungeon[i - 1][j] <= 152 && dungeon[i - 1][j] >= 130
				    && dungeon[i + 1][j] <= 152 && dungeon[i + 1][j] >= 130) {
					dungeon[i][j] = 146;
					continue;
				}
			}
			if (dungeon[i][j] == 7) {
				if (dungeon[i][j - 1] <= 152 && dungeon[i][j - 1] >= 130
				    && dungeon[i][j + 1] <= 152 && dungeon[i][j + 1] >= 130) {
					dungeon[i][j] = 147;
					continue;
				}
			}
		}
	}
}

void FenceDoorFix()
{
	for (int j = 0; j < DMAXY; j++) {
		for (int i = 0; i < DMAXX; i++) {
			if (dungeon[i][j] == 146) {
				if (dungeon[i + 1][j] > 152 || dungeon[i + 1][j] < 130
				    || dungeon[i - 1][j] > 152 || dungeon[i - 1][j] < 130) {
					dungeon[i][j] = 7;
					continue;
				}
			}
			if (dungeon[i][j] == 146) {
				if (IsNoneOf(dungeon[i + 1][j], 130, 132, 133, 134, 136, 138, 140) && IsNoneOf(dungeon[i - 1][j], 130, 132, 133, 134, 136, 138, 140)) {
					dungeon[i][j] = 7;
					continue;
				}
			}
			if (dungeon[i][j] == 147) {
				if (dungeon[i][j + 1] > 152 || dungeon[i][j + 1] < 130
				    || dungeon[i][j - 1] > 152 || dungeon[i][j - 1] < 130) {
					dungeon[i][j] = 7;
					continue;
				}
			}
			if (dungeon[i][j] == 147) {
				if (IsNoneOf(dungeon[i][j + 1], 131, 132, 133, 135, 137, 138, 139) && IsNoneOf(dungeon[i][j - 1], 131, 132, 133, 135, 137, 138, 139)) {
					dungeon[i][j] = 7;
					continue;
				}
			}
		}
	}
}

void Fence()
{
	for (int j = 1; j < DMAXY - 1; j++) {     // BUGFIX: Change '0' to '1' (fixed)
		for (int i = 1; i < DMAXX - 1; i++) { // BUGFIX: Change '0' to '1' (fixed)
			if (dungeon[i][j] == 10 && GenerateRnd(2) != 0) {
				int x = i;
				while (dungeon[x][j] == 10) {
					x++;
				}
				x--;
				if (x - i > 0) {
					dungeon[i][j] = 127;
					for (int xx = i + 1; xx < x; xx++) {
						if (GenerateRnd(2) != 0) {
							dungeon[xx][j] = 126;
						} else {
							dungeon[xx][j] = 129;
						}
					}
					dungeon[x][j] = 128;
				}
			}
			if (dungeon[i][j] == 9 && GenerateRnd(2) != 0) {
				int y = j;
				while (dungeon[i][y] == 9) {
					y++;
				}
				y--;
				if (y - j > 0) {
					dungeon[i][j] = 123;
					for (int yy = j + 1; yy < y; yy++) {
						if (GenerateRnd(2) != 0) {
							dungeon[i][yy] = 121;
						} else {
							dungeon[i][yy] = 124;
						}
					}
					dungeon[i][y] = 122;
				}
			}
			if (dungeon[i][j] == 11 && dungeon[i + 1][j] == 10 && dungeon[i][j + 1] == 9 && GenerateRnd(2) != 0) {
				dungeon[i][j] = 125;
				int x = i + 1;
				while (dungeon[x][j] == 10) {
					x++;
				}
				x--;
				for (int xx = i + 1; xx < x; xx++) {
					if (GenerateRnd(2) != 0) {
						dungeon[xx][j] = 126;
					} else {
						dungeon[xx][j] = 129;
					}
				}
				dungeon[x][j] = 128;
				int y = j + 1;
				while (dungeon[i][y] == 9) {
					y++;
				}
				y--;
				for (int yy = j + 1; yy < y; yy++) {
					if (GenerateRnd(2) != 0) {
						dungeon[i][yy] = 121;
					} else {
						dungeon[i][yy] = 124;
					}
				}
				dungeon[i][y] = 122;
			}
		}
	}

	for (int j = 1; j < DMAXY; j++) {     // BUGFIX: Change '0' to '1' (fixed)
		for (int i = 1; i < DMAXX; i++) { // BUGFIX: Change '0' to '1' (fixed)
			if (dungeon[i][j] == 7 && GenerateRnd(1) == 0 && SkipThemeRoom(i, j)) {
				int rt = GenerateRnd(2);
				if (rt == 0) {
					int y1 = j;
					// BUGFIX: Check `y1 >= 0` first (fixed)
					while (y1 >= 0 && FenceVerticalUp(i, y1)) {
						y1--;
					}
					y1++;
					int y2 = j;
					// BUGFIX: Check `y2 < DMAXY` first (fixed)
					while (y2 < DMAXY && FenceVerticalDown(i, y2)) {
						y2++;
					}
					y2--;
					bool skip = true;
					if (dungeon[i][y1] == 7) {
						skip = false;
					}
					if (dungeon[i][y2] == 7) {
						skip = false;
					}
					if (y2 - y1 > 1 && skip) {
						int rp = GenerateRnd(y2 - y1 - 1) + y1 + 1;
						for (int y = y1; y <= y2; y++) {
							if (y == rp) {
								continue;
							}
							if (dungeon[i][y] == 7) {
								if (GenerateRnd(2) != 0) {
									dungeon[i][y] = 135;
								} else {
									dungeon[i][y] = 137;
								}
							}
							if (dungeon[i][y] == 10) {
								dungeon[i][y] = 131;
							}
							if (dungeon[i][y] == 126) {
								dungeon[i][y] = 133;
							}
							if (dungeon[i][y] == 129) {
								dungeon[i][y] = 133;
							}
							if (dungeon[i][y] == 2) {
								dungeon[i][y] = 139;
							}
							if (dungeon[i][y] == 134) {
								dungeon[i][y] = 138;
							}
							if (dungeon[i][y] == 136) {
								dungeon[i][y] = 138;
							}
						}
					}
				}
				if (rt == 1) {
					int x1 = i;
					// BUGFIX: Check `x1 >= 0` first (fixed)
					while (x1 >= 0 && FenceHorizontalLeft(x1, j)) {
						x1--;
					}
					x1++;
					int x2 = i;
					// BUGFIX: Check `x2 < DMAXX` first (fixed)
					while (x2 < DMAXX && FenceHorizontalRight(x2, j)) {
						x2++;
					}
					x2--;
					bool skip = true;
					if (dungeon[x1][j] == 7) {
						skip = false;
					}
					if (dungeon[x2][j] == 7) {
						skip = false;
					}
					if (x2 - x1 > 1 && skip) {
						int rp = GenerateRnd(x2 - x1 - 1) + x1 + 1;
						for (int x = x1; x <= x2; x++) {
							if (x == rp) {
								continue;
							}
							if (dungeon[x][j] == 7) {
								if (GenerateRnd(2) != 0) {
									dungeon[x][j] = 134;
								} else {
									dungeon[x][j] = 136;
								}
							}
							if (dungeon[x][j] == 9) {
								dungeon[x][j] = 130;
							}
							if (dungeon[x][j] == 121) {
								dungeon[x][j] = 132;
							}
							if (dungeon[x][j] == 124) {
								dungeon[x][j] = 132;
							}
							if (dungeon[x][j] == 4) {
								dungeon[x][j] = 140;
							}
							if (dungeon[x][j] == 135) {
								dungeon[x][j] = 138;
							}
							if (dungeon[x][j] == 137) {
								dungeon[x][j] = 138;
							}
						}
					}
				}
			}
		}
	}

	AddFenceDoors();
	FenceDoorFix();
}

bool Anvil()
{
	int sw = L3ANVIL[0];
	int sh = L3ANVIL[1];
	int sx = GenerateRnd(DMAXX - sw);
	int sy = GenerateRnd(DMAXY - sh);

	bool found = false;
	int trys = 0;
	while (!found && trys < 200) {
		trys++;
		found = true;
		int ii = 2;
		for (int yy = 0; yy < sh && found; yy++) {
			for (int xx = 0; xx < sw && found; xx++) {
				if (L3ANVIL[ii] != 0 && dungeon[xx + sx][yy + sy] != L3ANVIL[ii]) {
					found = false;
				}
				if (dflags[xx + sx][yy + sy] != 0) {
					found = false;
				}
				ii++;
			}
		}
		if (!found) {
			sx++;
			if (sx == DMAXX - sw) {
				sx = 0;
				sy++;
				if (sy == DMAXY - sh) {
					sy = 0;
				}
			}
		}
	}
	if (trys >= 200) {
		return true;
	}

	int ii = sw * sh + 2;
	for (int yy = 0; yy < sh; yy++) {
		for (int xx = 0; xx < sw; xx++) {
			if (L3ANVIL[ii] != 0) {
				dungeon[xx + sx][yy + sy] = L3ANVIL[ii];
			}
			dflags[xx + sx][yy + sy] |= DLRG_PROTECTED;
			ii++;
		}
	}

	setpc_x = sx;
	setpc_y = sy;
	setpc_w = sw;
	setpc_h = sh;

	return false;
}

void Warp()
{
	for (int j = 0; j < DMAXY; j++) {
		for (int i = 0; i < DMAXX; i++) {
			if (dungeon[i][j] == 125 && dungeon[i + 1][j] == 125 && dungeon[i][j + 1] == 125 && dungeon[i + 1][j + 1] == 125) {
				dungeon[i][j] = 156;
				dungeon[i + 1][j] = 155;
				dungeon[i][j + 1] = 153;
				dungeon[i + 1][j + 1] = 154;
				return;
			}
			if (dungeon[i][j] == 5 && dungeon[i + 1][j + 1] == 7) {
				dungeon[i][j] = 7;
			}
		}
	}
}

void HallOfHeroes()
{
	for (int j = 0; j < DMAXY; j++) {
		for (int i = 0; i < DMAXX; i++) {
			if (dungeon[i][j] == 5 && dungeon[i + 1][j + 1] == 7) {
				dungeon[i][j] = 7;
			}
		}
	}
	for (int j = 0; j < DMAXY; j++) {
		for (int i = 0; i < DMAXX; i++) {
			if (dungeon[i][j] == 5 && dungeon[i + 1][j + 1] == 12 && dungeon[i + 1][j] == 7) {
				dungeon[i][j] = 7;
				dungeon[i][j + 1] = 7;
				dungeon[i + 1][j + 1] = 7;
			}
			if (dungeon[i][j] == 5 && dungeon[i + 1][j + 1] == 12 && dungeon[i][j + 1] == 7) {
				dungeon[i][j] = 7;
				dungeon[i + 1][j] = 7;
				dungeon[i + 1][j + 1] = 7;
			}
		}
	}
}

void LockRectangle(int x, int y)
{
	if (!lockout[x][y]) {
		return;
	}

	lockout[x][y] = false;
	lockoutcnt++;
	LockRectangle(x, y - 1);
	LockRectangle(x, y + 1);
	LockRectangle(x - 1, y);
	LockRectangle(x + 1, y);
}

bool Lockout()
{
	int fx;
	int fy;

	int t = 0;
	for (int j = 0; j < DMAXY; j++) {
		for (int i = 0; i < DMAXX; i++) {
			if (dungeon[i][j] != 0) {
				lockout[i][j] = true;
				fx = i;
				fy = j;
				t++;
			} else {
				lockout[i][j] = false;
			}
		}
	}

	lockoutcnt = 0;
	LockRectangle(fx, fy);

	return t == lockoutcnt;
}

void GenerateLevel(lvl_entry entry)
{
	bool found;
	bool genok;

	lavapool = 0;

	do {
		do {
			do {
				InitDungeonFlags();
				int x1 = GenerateRnd(20) + 10;
				int y1 = GenerateRnd(20) + 10;
				int x2 = x1 + 2;
				int y2 = y1 + 2;
				FillRoom(x1, y1, x2, y2);
				CreateBlock(x1, y1, 2, 0);
				CreateBlock(x2, y1, 2, 1);
				CreateBlock(x1, y2, 2, 2);
				CreateBlock(x1, y1, 2, 3);
				if (QuestStatus(Quests[Q_ANVIL], QuestData[Q_ANVIL])) {
					x1 = GenerateRnd(10) + 10;
					y1 = GenerateRnd(10) + 10;
					x2 = x1 + 12;
					y2 = y1 + 12;
					FloorArea(x1, y1, x2, y2);
				}
				FillDiagonals();
				FillSingles();
				FillStraights();
				FillDiagonals();
				Edges();
				if (GetFloorArea() >= 600) {
					found = Lockout();
				} else {
					found = false;
				}
			} while (!found);
			MakeMegas();
			if (entry == ENTRY_MAIN) {
				if (currlevel < 17) {
					genok = PlaceMiniSet(L3UP, 1, 1, -1, -1, true);
				} else {
					if (currlevel != 17)
						genok = PlaceMiniSet(L6UP, 1, 1, -1, -1, true);
					else
						genok = PlaceMiniSet(L6HOLDWARP, 1, 1, -1, -1, true);
				}
				if (!genok) {
					if (currlevel < 17) {
						genok = PlaceMiniSet(L3DOWN, 1, 1, -1, -1, false);
					} else {
						if (currlevel != 20)
							genok = PlaceMiniSet(L6DOWN, 1, 1, -1, -1, false);
					}
					if (!genok && currlevel == 9) {
						genok = PlaceMiniSet(L3HOLDWARP, 1, 1, -1, -1, false);
					}
				}
			} else if (entry == ENTRY_PREV) {
				if (currlevel < 17) {
					genok = PlaceMiniSet(L3UP, 1, 1, -1, -1, false);
				} else {
					if (currlevel != 17)
						genok = PlaceMiniSet(L6UP, 1, 1, -1, -1, false);
					else
						genok = PlaceMiniSet(L6HOLDWARP, 1, 1, -1, -1, false);
				}
				if (!genok) {
					if (currlevel < 17) {
						genok = PlaceMiniSet(L3DOWN, 1, 1, -1, -1, true);
						ViewX += 2;
						ViewY -= 2;
					} else {
						if (currlevel != 20) {
							genok = PlaceMiniSet(L6DOWN, 1, 1, -1, -1, true);
							ViewX += 2;
							ViewY -= 2;
						}
					}
					if (!genok && currlevel == 9) {
						genok = PlaceMiniSet(L3HOLDWARP, 1, 1, -1, -1, false);
					}
				}
			} else {
				if (currlevel < 17) {
					genok = PlaceMiniSet(L3UP, 1, 1, -1, -1, false);
				} else {
					if (currlevel != 17)
						genok = PlaceMiniSet(L6UP, 1, 1, -1, -1, false);
					else
						genok = PlaceMiniSet(L6HOLDWARP, 1, 1, -1, -1, true);
				}
				if (!genok) {
					if (currlevel < 17) {
						genok = PlaceMiniSet(L3DOWN, 1, 1, -1, -1, false);
					} else {
						if (currlevel != 20)
							genok = PlaceMiniSet(L6DOWN, 1, 1, -1, -1, false);
					}
					if (!genok && currlevel == 9) {
						genok = PlaceMiniSet(L3HOLDWARP, 1, 1, -1, -1, true);
					}
				}
			}
			if (!genok && QuestStatus(Quests[Q_ANVIL], QuestData[Q_ANVIL])) {
				genok = Anvil();
			}
		} while (genok);
		if (currlevel < 17) {
			Pool();
		} else {
			if (HivePlaceSetRandom(HivePattern41, 30))
				lavapool++;
			if (HivePlaceSetRandom(HivePattern42, 40))
				lavapool++;
			if (HivePlaceSetRandom(HivePattern39, 50))
				lavapool++;
			if (HivePlaceSetRandom(HivePattern40, 60))
				lavapool++;
			if (lavapool < 3)
				lavapool = 0;
		}
	} while (lavapool == 0);

	if (currlevel < 17)
		PoolFix();
	if (currlevel < 17)
		Warp();

	if (currlevel < 17) {
		PlaceMiniSetRandom(L3ISLE1, 70);
		PlaceMiniSetRandom(L3ISLE2, 70);
		PlaceMiniSetRandom(L3ISLE3, 30);
		PlaceMiniSetRandom(L3ISLE4, 30);
		PlaceMiniSetRandom(L3ISLE1, 100);
		PlaceMiniSetRandom(L3ISLE2, 100);
		PlaceMiniSetRandom(L3ISLE5, 90);
	} else {
		PlaceMiniSetRandom(L6ISLE1, 70);
		PlaceMiniSetRandom(L6ISLE2, 70);
		PlaceMiniSetRandom(L6ISLE3, 30);
		PlaceMiniSetRandom(L6ISLE4, 30);
		PlaceMiniSetRandom(L6ISLE1, 100);
		PlaceMiniSetRandom(L6ISLE2, 100);
		PlaceMiniSetRandom(L6ISLE5, 90);
	}

	if (currlevel < 17)
		HallOfHeroes();
	if (currlevel < 17)
		River();

	if (QuestStatus(Quests[Q_ANVIL], QuestData[Q_ANVIL])) {
		dungeon[setpc_x + 7][setpc_y + 5] = 7;
		dungeon[setpc_x + 8][setpc_y + 5] = 7;
		dungeon[setpc_x + 9][setpc_y + 5] = 7;
		if (dungeon[setpc_x + 10][setpc_y + 5] == 17 || dungeon[setpc_x + 10][setpc_y + 5] == 18) {
			dungeon[setpc_x + 10][setpc_y + 5] = 45;
		}
	}

	if (currlevel < 17)
		DRLG_PlaceThemeRooms(5, 10, 7, 0, false);

	if (currlevel < 17) {
		Fence();
		PlaceMiniSetRandom(L3TITE1, 10);
		PlaceMiniSetRandom(L3TITE2, 10);
		PlaceMiniSetRandom(L3TITE3, 10);
		PlaceMiniSetRandom(L3TITE6, 20);
		PlaceMiniSetRandom(L3TITE7, 20);
		PlaceMiniSetRandom(L3TITE8, 20);
		PlaceMiniSetRandom(L3TITE9, 20);
		PlaceMiniSetRandom(L3TITE10, 20);
		PlaceMiniSetRandom(L3TITE11, 30);
		PlaceMiniSetRandom(L3TITE12, 20);
		PlaceMiniSetRandom(L3TITE13, 20);
		PlaceMiniSetRandom(L3CREV1, 30);
		PlaceMiniSetRandom(L3CREV2, 30);
		PlaceMiniSetRandom(L3CREV3, 30);
		PlaceMiniSetRandom(L3CREV4, 30);
		PlaceMiniSetRandom(L3CREV5, 30);
		PlaceMiniSetRandom(L3CREV6, 30);
		PlaceMiniSetRandom(L3CREV7, 30);
		PlaceMiniSetRandom(L3CREV8, 30);
		PlaceMiniSetRandom(L3CREV9, 30);
		PlaceMiniSetRandom(L3CREV10, 30);
		PlaceMiniSetRandom(L3CREV11, 30);
		PlaceMiniSetRandom(L3XTRA1, 25);
		PlaceMiniSetRandom(L3XTRA2, 25);
		PlaceMiniSetRandom(L3XTRA3, 25);
		PlaceMiniSetRandom(L3XTRA4, 25);
		PlaceMiniSetRandom(L3XTRA5, 25);
	} else {
		PlaceMiniSetRandom(HivePattern1, 20);
		PlaceMiniSetRandom(HivePattern2, 20);
		PlaceMiniSetRandom(HivePattern3, 20);
		PlaceMiniSetRandom(HivePattern4, 20);
		PlaceMiniSetRandom(HivePattern29, 10);
		PlaceMiniSetRandom(HivePattern30, 15);
		PlaceMiniSetRandom(HivePattern31, 20);
		PlaceMiniSetRandom(HivePattern32, 25);
		PlaceMiniSetRandom(HivePattern33, 30);
		PlaceMiniSetRandom(HivePattern34, 35);
		PlaceMiniSetRandom(HivePattern35, 40);
		PlaceMiniSetRandom(HivePattern36, 45);
		PlaceMiniSetRandom(HivePattern37, 50);
		PlaceMiniSetRandom(HivePattern38, 55);
		PlaceMiniSetRandom(HivePattern38, 10);
		PlaceMiniSetRandom(HivePattern37, 15);
		PlaceMiniSetRandom(HivePattern36, 20);
		PlaceMiniSetRandom(HivePattern35, 25);
		PlaceMiniSetRandom(HivePattern34, 30);
		PlaceMiniSetRandom(HivePattern33, 35);
		PlaceMiniSetRandom(HivePattern32, 40);
		PlaceMiniSetRandom(HivePattern31, 45);
		PlaceMiniSetRandom(HivePattern30, 50);
		PlaceMiniSetRandom(HivePattern29, 55);
		PlaceMiniSetRandom(HivePattern9, 40);
		PlaceMiniSetRandom(HivePattern10, 45);
		PlaceMiniSetRandom(HivePattern5, 25);
		PlaceMiniSetRandom(HivePattern6, 25);
		PlaceMiniSetRandom(HivePattern7, 25);
		PlaceMiniSetRandom(HivePattern8, 25);
		PlaceMiniSetRandom(HivePattern11, 25);
		PlaceMiniSetRandom(HivePattern12, 25);
		PlaceMiniSetRandom(HivePattern13, 25);
		PlaceMiniSetRandom(HivePattern14, 25);
		PlaceMiniSetRandom(HivePattern15, 25);
		PlaceMiniSetRandom(HivePattern17, 25);
		PlaceMiniSetRandom(HivePattern18, 25);
		PlaceMiniSetRandom(HivePattern19, 25);
		PlaceMiniSetRandom(HivePattern20, 25);
		PlaceMiniSetRandom(HivePattern21, 25);
		PlaceMiniSetRandom(HivePattern23, 25);
		PlaceMiniSetRandom(HivePattern24, 25);
		PlaceMiniSetRandom(HivePattern25, 25);
		PlaceMiniSetRandom(HivePattern26, 25);
		PlaceMiniSetRandom(HivePattern16, 25);
		PlaceMiniSetRandom(HivePattern22, 25);
		PlaceMiniSetRandom(HivePattern27, 25);
		PlaceMiniSetRandom(HivePattern28, 25);
	}

	for (int j = 0; j < DMAXY; j++) {
		for (int i = 0; i < DMAXX; i++) {
			pdungeon[i][j] = dungeon[i][j];
		}
	}

	DRLG_Init_Globals();
}

void Pass3()
{
	DRLG_LPass3(8 - 1);
}

} // namespace

void CreateL3Dungeon(uint32_t rseed, lvl_entry entry)
{
	SetRndSeed(rseed);

	dminx = 16;
	dminy = 16;
	dmaxx = 96;
	dmaxy = 96;

	DRLG_InitTrans();
	DRLG_InitSetPC();
	GenerateLevel(entry);
	Pass3();

	if (currlevel < 17) {
		for (int j = 0; j < MAXDUNY; j++) {
			for (int i = 0; i < MAXDUNX; i++) {
				if (dPiece[i][j] >= 56 && dPiece[i][j] <= 147) {
					DoLighting({ i, j }, 7, -1);
				} else if (dPiece[i][j] >= 154 && dPiece[i][j] <= 161) {
					DoLighting({ i, j }, 7, -1);
				} else if (IsAnyOf(dPiece[i][j], 150, 152)) {
					DoLighting({ i, j }, 7, -1);
				}
			}
		}
	} else {
		for (int j = 0; j < MAXDUNY; j++) {
			for (int i = 0; i < MAXDUNX; i++) {
				if (dPiece[i][j] >= 382 && dPiece[i][j] <= 457) {
					DoLighting({ i, j }, 9, -1);
				}
			}
		}
	}

	DRLG_SetPC();
}

void LoadL3Dungeon(const char *path, int vx, int vy)
{
	dminx = 16;
	dminy = 16;
	dmaxx = 96;
	dmaxy = 96;

	InitDungeonFlags();
	DRLG_InitTrans();

	auto dunData = LoadFileInMem<uint16_t>(path);

	int width = SDL_SwapLE16(dunData[0]);
	int height = SDL_SwapLE16(dunData[1]);

	const uint16_t *tileLayer = &dunData[2];

	for (int j = 0; j < height; j++) {
		for (int i = 0; i < width; i++) {
			uint8_t tileId = SDL_SwapLE16(tileLayer[j * width + i]);
			dungeon[i][j] = (tileId != 0) ? tileId : 7;
		}
	}

	for (int j = 0; j < DMAXY; j++) {
		for (int i = 0; i < DMAXX; i++) { // NOLINT(modernize-loop-convert)
			if (dungeon[i][j] == 0) {
				dungeon[i][j] = 8;
			}
		}
	}

	Pass3();
	DRLG_Init_Globals();

	ViewX = vx;
	ViewY = vy;

	SetMapMonsters(dunData.get(), { 0, 0 });
	SetMapObjects(dunData.get(), 0, 0);

	for (int j = 0; j < MAXDUNY; j++) {
		for (int i = 0; i < MAXDUNX; i++) {
			if (dPiece[i][j] >= 56 && dPiece[i][j] <= 147) {
				DoLighting({ i, j }, 7, -1);
			} else if (dPiece[i][j] >= 154 && dPiece[i][j] <= 161) {
				DoLighting({ i, j }, 7, -1);
			} else if (IsAnyOf(dPiece[i][j], 150, 152)) {
				DoLighting({ i, j }, 7, -1);
			}
		}
	}
}

void LoadPreL3Dungeon(const char *path)
{
	InitDungeonFlags();
	DRLG_InitTrans();

	auto dunData = LoadFileInMem<uint16_t>(path);

	int width = SDL_SwapLE16(dunData[0]);
	int height = SDL_SwapLE16(dunData[1]);

	const uint16_t *tileLayer = &dunData[2];

	for (int j = 0; j < height; j++) {
		for (int i = 0; i < width; i++) {
			uint8_t tileId = SDL_SwapLE16(tileLayer[j * width + i]);
			dungeon[i][j] = (tileId != 0) ? tileId : 7;
		}
	}

	for (int j = 0; j < DMAXY; j++) {
		for (int i = 0; i < DMAXX; i++) { // NOLINT(modernize-loop-convert)
			if (dungeon[i][j] == 0) {
				dungeon[i][j] = 8;
			}
		}
	}

	memcpy(pdungeon, dungeon, sizeof(pdungeon));
}

} // namespace devilution

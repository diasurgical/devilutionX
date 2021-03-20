/**
 * @file drlg_l3.cpp
 *
 * Implementation of the caves level generation algorithms.
 */

#include <algorithm>

#include "all.h"

DEVILUTION_BEGIN_NAMESPACE

namespace {

/** This will be true if a lava pool has been generated for the level */
Uint8 lavapool;
int lockoutcnt;
BOOLEAN lockout[DMAXX][DMAXY];

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
const BYTE byte_48A76C[] = { 1, 1, 8, 25 };
const BYTE byte_48A770[] = { 1, 1, 8, 26 };
const BYTE byte_48A774[] = { 1, 1, 8, 27 };
const BYTE byte_48A778[] = { 1, 1, 8, 28 };
const BYTE byte_48A77C[] = { 1, 1, 7, 29 };
const BYTE byte_48A780[] = { 1, 1, 7, 30 };
const BYTE byte_48A784[] = { 1, 1, 7, 31 };
const BYTE byte_48A788[] = { 1, 1, 7, 32 };
const BYTE byte_48A790[] = {
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
const BYTE byte_48A7A8[] = {
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
const BYTE byte_48A7BC[] = { 1, 1, 9, 33 };
const BYTE byte_48A7C0[] = { 1, 1, 9, 34 };
const BYTE byte_48A7C4[] = { 1, 1, 9, 35 };
const BYTE byte_48A7C8[] = { 1, 1, 9, 36 };
const BYTE byte_48A7CC[] = { 1, 1, 9, 37 };
const BYTE byte_48A7D0[] = { 1, 1, 11, 38 };
const BYTE byte_48A7D4[] = { 1, 1, 10, 39 };
const BYTE byte_48A7D8[] = { 1, 1, 10, 40 };
const BYTE byte_48A7DC[] = { 1, 1, 10, 41 };
const BYTE byte_48A7E0[] = { 1, 1, 10, 42 };
const BYTE byte_48A7E4[] = { 1, 1, 10, 43 };
const BYTE byte_48A7E8[] = { 1, 1, 11, 44 };
const BYTE byte_48A7EC[] = { 1, 1, 9, 45 };
const BYTE byte_48A7F0[] = { 1, 1, 9, 46 };
const BYTE byte_48A7F4[] = { 1, 1, 10, 47 };
const BYTE byte_48A7F8[] = { 1, 1, 10, 48 };
const BYTE byte_48A7FC[] = { 1, 1, 11, 49 };
const BYTE byte_48A800[] = { 1, 1, 11, 50 };
const BYTE byte_48A808[] = {
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
const BYTE byte_48A820[] = {
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
const BYTE byte_48A838[] = {
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
const BYTE byte_48A850[] = {
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
const BYTE byte_48A868[] = {
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
const BYTE byte_48A880[] = {
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
const BYTE byte_48A898[] = {
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
const BYTE byte_48A8B0[] = {
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
const BYTE byte_48A8C8[] = {
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
const BYTE byte_48A8E0[] = {
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
const BYTE byte_48A948[] = {
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
const BYTE byte_48A970[] = {
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
const BYTE byte_48A998[] = {
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
const BYTE byte_48A9C8[] = {
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

} // namespace

static void InitL3Dungeon()
{
	int i, j;

	memset(dungeon, 0, sizeof(dungeon));

	for (j = 0; j < DMAXY; j++) {
		for (i = 0; i < DMAXX; i++) {
			dungeon[i][j] = 0;
			dflags[i][j] = 0;
		}
	}
}

static BOOL DRLG_L3FillRoom(int x1, int y1, int x2, int y2)
{
	int i, j, v;

	if (x1 <= 1 || x2 >= 34 || y1 <= 1 || y2 >= 38) {
		return FALSE;
	}

	v = 0;
	for (j = y1; j <= y2; j++) {
		for (i = x1; i <= x2; i++) {
			v += dungeon[i][j];
		}
	}

	if (v != 0) {
		return FALSE;
	}

	for (j = y1 + 1; j < y2; j++) {
		for (i = x1 + 1; i < x2; i++) {
			dungeon[i][j] = 1;
		}
	}
	for (j = y1; j <= y2; j++) {
		if (random_(0, 2) != 0) {
			dungeon[x1][j] = 1;
		}
		if (random_(0, 2) != 0) {
			dungeon[x2][j] = 1;
		}
	}
	for (i = x1; i <= x2; i++) {
		if (random_(0, 2) != 0) {
			dungeon[i][y1] = 1;
		}
		if (random_(0, 2) != 0) {
			dungeon[i][y2] = 1;
		}
	}

	return TRUE;
}

static void DRLG_L3CreateBlock(int x, int y, int obs, int dir)
{
	int blksizex, blksizey, x1, y1, x2, y2;
	int contflag;

	blksizex = random_(0, 2) + 3;
	blksizey = random_(0, 2) + 3;

	if (dir == 0) {
		y2 = y - 1;
		y1 = y2 - blksizey;
		if (blksizex < obs) {
			x1 = random_(0, blksizex) + x;
		}
		if (blksizex == obs) {
			x1 = x;
		}
		if (blksizex > obs) {
			x1 = x - random_(0, blksizex);
		}
		x2 = blksizex + x1;
	}
	if (dir == 3) {
		x2 = x - 1;
		x1 = x2 - blksizex;
		if (blksizey < obs) {
			y1 = random_(0, blksizey) + y;
		}
		if (blksizey == obs) {
			y1 = y;
		}
		if (blksizey > obs) {
			y1 = y - random_(0, blksizey);
		}
		y2 = y1 + blksizey;
	}
	if (dir == 2) {
		y1 = y + 1;
		y2 = y1 + blksizey;
		if (blksizex < obs) {
			x1 = random_(0, blksizex) + x;
		}
		if (blksizex == obs) {
			x1 = x;
		}
		if (blksizex > obs) {
			x1 = x - random_(0, blksizex);
		}
		x2 = blksizex + x1;
	}
	if (dir == 1) {
		x1 = x + 1;
		x2 = x1 + blksizex;
		if (blksizey < obs) {
			y1 = random_(0, blksizey) + y;
		}
		if (blksizey == obs) {
			y1 = y;
		}
		if (blksizey > obs) {
			y1 = y - random_(0, blksizey);
		}
		y2 = y1 + blksizey;
	}

	if (DRLG_L3FillRoom(x1, y1, x2, y2) == TRUE) {
		contflag = random_(0, 4);
		if (contflag != 0 && dir != 2) {
			DRLG_L3CreateBlock(x1, y1, blksizey, 0);
		}
		if (contflag != 0 && dir != 3) {
			DRLG_L3CreateBlock(x2, y1, blksizex, 1);
		}
		if (contflag != 0 && dir != 0) {
			DRLG_L3CreateBlock(x1, y2, blksizey, 2);
		}
		if (contflag != 0 && dir != 1) {
			DRLG_L3CreateBlock(x1, y1, blksizex, 3);
		}
	}
}

static void DRLG_L3FloorArea(int x1, int y1, int x2, int y2)
{
	int i, j;

	for (j = y1; j <= y2; j++) {
		for (i = x1; i <= x2; i++) {
			dungeon[i][j] = 1;
		}
	}
}

static void DRLG_L3FillDiags()
{
	int i, j, v;

	for (j = 0; j < DMAXY - 1; j++) {
		for (i = 0; i < DMAXX - 1; i++) {
			v = dungeon[i + 1][j + 1] + 2 * dungeon[i][j + 1] + 4 * dungeon[i + 1][j] + 8 * dungeon[i][j];
			if (v == 6) {
				if (random_(0, 2) == 0) {
					dungeon[i][j] = 1;
				} else {
					dungeon[i + 1][j + 1] = 1;
				}
			}
			if (v == 9) {
				if (random_(0, 2) == 0) {
					dungeon[i + 1][j] = 1;
				} else {
					dungeon[i][j + 1] = 1;
				}
			}
		}
	}
}

static void DRLG_L3FillSingles()
{
	int i, j;

	for (j = 1; j < DMAXY - 1; j++) {
		for (i = 1; i < DMAXX - 1; i++) {
			if (dungeon[i][j] == 0
			    && dungeon[i][j - 1] + dungeon[i - 1][j - 1] + dungeon[i + 1][j - 1] == 3
			    && dungeon[i + 1][j] + dungeon[i - 1][j] == 2
			    && dungeon[i][j + 1] + dungeon[i - 1][j + 1] + dungeon[i + 1][j + 1] == 3) {
				dungeon[i][j] = 1;
			}
		}
	}
}

static void DRLG_L3FillStraights()
{
	int i, j, xc, xs, yc, ys, k, rv;

	for (j = 0; j < DMAXY - 1; j++) {
		xs = 0;
		for (i = 0; i < 37; i++) {
			if (dungeon[i][j] == 0 && dungeon[i][j + 1] == 1) {
				if (xs == 0) {
					xc = i;
				}
				xs++;
			} else {
				if (xs > 3 && random_(0, 2) != 0) {
					for (k = xc; k < i; k++) {
						rv = random_(0, 2);
						dungeon[k][j] = rv;
					}
				}
				xs = 0;
			}
		}
	}
	for (j = 0; j < DMAXY - 1; j++) {
		xs = 0;
		for (i = 0; i < 37; i++) {
			if (dungeon[i][j] == 1 && dungeon[i][j + 1] == 0) {
				if (xs == 0) {
					xc = i;
				}
				xs++;
			} else {
				if (xs > 3 && random_(0, 2) != 0) {
					for (k = xc; k < i; k++) {
						rv = random_(0, 2);
						dungeon[k][j + 1] = rv;
					}
				}
				xs = 0;
			}
		}
	}
	for (i = 0; i < DMAXX - 1; i++) {
		ys = 0;
		for (j = 0; j < 37; j++) {
			if (dungeon[i][j] == 0 && dungeon[i + 1][j] == 1) {
				if (ys == 0) {
					yc = j;
				}
				ys++;
			} else {
				if (ys > 3 && random_(0, 2) != 0) {
					for (k = yc; k < j; k++) {
						rv = random_(0, 2);
						dungeon[i][k] = rv;
					}
				}
				ys = 0;
			}
		}
	}
	for (i = 0; i < DMAXX - 1; i++) {
		ys = 0;
		for (j = 0; j < 37; j++) {
			if (dungeon[i][j] == 1 && dungeon[i + 1][j] == 0) {
				if (ys == 0) {
					yc = j;
				}
				ys++;
			} else {
				if (ys > 3 && random_(0, 2) != 0) {
					for (k = yc; k < j; k++) {
						rv = random_(0, 2);
						dungeon[i + 1][k] = rv;
					}
				}
				ys = 0;
			}
		}
	}
}

static void DRLG_L3Edges()
{
	int i, j;

	for (j = 0; j < DMAXY; j++) {
		dungeon[DMAXX - 1][j] = 0;
	}
	for (i = 0; i < DMAXX; i++) {
		dungeon[i][DMAXY - 1] = 0;
	}
}

static int DRLG_L3GetFloorArea()
{
	int i, j, gfa;

	gfa = 0;

	for (j = 0; j < DMAXY; j++) {
		for (i = 0; i < DMAXX; i++) {
			gfa += dungeon[i][j];
		}
	}

	return gfa;
}

static void DRLG_L3MakeMegas()
{
	int i, j, v, rv;

	for (j = 0; j < DMAXY - 1; j++) {
		for (i = 0; i < DMAXX - 1; i++) {
			v = dungeon[i + 1][j + 1] + 2 * dungeon[i][j + 1] + 4 * dungeon[i + 1][j] + 8 * dungeon[i][j];
			if (v == 6) {
				rv = random_(0, 2);
				if (rv == 0) {
					v = 12;
				} else {
					v = 5;
				}
			}
			if (v == 9) {
				rv = random_(0, 2);
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
	for (i = 0; i < DMAXX; i++) {
		dungeon[i][DMAXY - 1] = 8;
	}
}

static void DRLG_L3River()
{
	int rx, ry, px, py, dir, pdir, nodir, nodir2, dircheck;
	int river[3][100];
	int rivercnt, riveramt;
	int i, trys, found, bridge, lpcnt;
	BOOL bail;

	rivercnt = 0;
	bail = FALSE;
	trys = 0;
	/// BUGFIX: pdir is uninitialized, add code `pdir = -1;`(fixed)
	pdir = -1;

	while (trys < 200 && rivercnt < 4) {
		bail = FALSE;
		while (!bail && trys < 200) {
			trys++;
			rx = 0;
			ry = 0;
			i = 0;
			// BUGFIX: Replace with `(ry >= DMAXY || dungeon[rx][ry] < 25 || dungeon[rx][ry] > 28) && i < 100` (fixed)
			while ((ry >= DMAXY || dungeon[rx][ry] < 25 || dungeon[rx][ry] > 28) && i < 100) {
				rx = random_(0, DMAXX);
				ry = random_(0, DMAXY);
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
			nodir2 = 4;
			dircheck = 0;
			while (dircheck < 4 && riveramt < 100) {
				px = rx;
				py = ry;
				if (dircheck == 0) {
					dir = random_(0, 4);
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
						river[2][riveramt] = (BYTE)random_(0, 2) + 17;
					}
					if (dir > 1) {
						river[2][riveramt] = (BYTE)random_(0, 2) + 15;
					}
					river[0][riveramt] = rx;
					river[1][riveramt] = ry;
					riveramt++;
					if (dir == 0 && pdir == 2 || dir == 3 && pdir == 1) {
						if (riveramt > 2) {
							river[2][riveramt - 2] = 22;
						}
						if (dir == 0) {
							nodir2 = 1;
						} else {
							nodir2 = 2;
						}
					}
					if (dir == 0 && pdir == 3 || dir == 2 && pdir == 1) {
						if (riveramt > 2) {
							river[2][riveramt - 2] = 21;
						}
						if (dir == 0) {
							nodir2 = 1;
						} else {
							nodir2 = 3;
						}
					}
					if (dir == 1 && pdir == 2 || dir == 3 && pdir == 0) {
						if (riveramt > 2) {
							river[2][riveramt - 2] = 20;
						}
						if (dir == 1) {
							nodir2 = 0;
						} else {
							nodir2 = 2;
						}
					}
					if (dir == 1 && pdir == 3 || dir == 2 && pdir == 0) {
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
				bail = TRUE;
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
				bail = TRUE;
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
				bail = TRUE;
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
				bail = TRUE;
			}
		}
		if (bail == TRUE && riveramt < 7) {
			bail = FALSE;
		}
		if (bail == TRUE) {
			found = 0;
			lpcnt = 0;
			while (found == 0 && lpcnt < 30) {
				lpcnt++;
				bridge = random_(0, riveramt);
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
				for (i = 0; i < riveramt && found != 0; i++) {
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
				bail = FALSE;
			}
		}
	}
}

static BOOL DRLG_L3Spawn(int x, int y, int *totarea);

static BOOL DRLG_L3SpawnEdge(int x, int y, int *totarea)
{
	BYTE i;
	static BYTE spawntable[15] = { 0x00, 0x0A, 0x43, 0x05, 0x2c, 0x06, 0x09, 0x00, 0x00, 0x1c, 0x83, 0x06, 0x09, 0x0A, 0x05 };

	if (*totarea > 40) {
		return TRUE;
	}
	if (x < 0 || y < 0 || x >= DMAXX || y >= DMAXY) {
		return TRUE;
	}
	if (dungeon[x][y] & 0x80) {
		return FALSE;
	}
	if (dungeon[x][y] > 15) {
		return TRUE;
	}

	i = dungeon[x][y];
	dungeon[x][y] |= 0x80;
	*totarea += 1;

	if (spawntable[i] & 8 && DRLG_L3SpawnEdge(x, y - 1, totarea) == TRUE) {
		return TRUE;
	}
	if (spawntable[i] & 4 && DRLG_L3SpawnEdge(x, y + 1, totarea) == TRUE) {
		return TRUE;
	}
	if (spawntable[i] & 2 && DRLG_L3SpawnEdge(x + 1, y, totarea) == TRUE) {
		return TRUE;
	}
	if (spawntable[i] & 1 && DRLG_L3SpawnEdge(x - 1, y, totarea) == TRUE) {
		return TRUE;
	}
	if (spawntable[i] & 0x80 && DRLG_L3Spawn(x, y - 1, totarea) == TRUE) {
		return TRUE;
	}
	if (spawntable[i] & 0x40 && DRLG_L3Spawn(x, y + 1, totarea) == TRUE) {
		return TRUE;
	}
	if (spawntable[i] & 0x20 && DRLG_L3Spawn(x + 1, y, totarea) == TRUE) {
		return TRUE;
	}
	if (spawntable[i] & 0x10 && DRLG_L3Spawn(x - 1, y, totarea) == TRUE) {
		return TRUE;
	}

	return FALSE;
}

static BOOL DRLG_L3Spawn(int x, int y, int *totarea)
{
	BYTE i;
	static BYTE spawntable[15] = { 0x00, 0x0A, 0x03, 0x05, 0x0C, 0x06, 0x09, 0x00, 0x00, 0x0C, 0x03, 0x06, 0x09, 0x0A, 0x05 };

	if (*totarea > 40) {
		return TRUE;
	}
	if (x < 0 || y < 0 || x >= DMAXX || y >= DMAXY) {
		return TRUE;
	}
	if (dungeon[x][y] & 0x80) {
		return FALSE;
	}
	if (dungeon[x][y] > 15) {
		return TRUE;
	}

	i = dungeon[x][y];
	dungeon[x][y] |= 0x80;
	*totarea += 1;

	if (i != 8) {
		if (spawntable[i] & 8 && DRLG_L3SpawnEdge(x, y - 1, totarea) == TRUE) {
			return TRUE;
		}
		if (spawntable[i] & 4 && DRLG_L3SpawnEdge(x, y + 1, totarea) == TRUE) {
			return TRUE;
		}
		if (spawntable[i] & 2 && DRLG_L3SpawnEdge(x + 1, y, totarea) == TRUE) {
			return TRUE;
		}
		if (spawntable[i] & 1 && DRLG_L3SpawnEdge(x - 1, y, totarea) == TRUE) {
			return TRUE;
		}
	} else {
		if (DRLG_L3Spawn(x + 1, y, totarea) == TRUE) {
			return TRUE;
		}
		if (DRLG_L3Spawn(x - 1, y, totarea) == TRUE) {
			return TRUE;
		}
		if (DRLG_L3Spawn(x, y + 1, totarea) == TRUE) {
			return TRUE;
		}
		if (DRLG_L3Spawn(x, y - 1, totarea) == TRUE) {
			return TRUE;
		}
	}

	return FALSE;
}

/**
 * Flood fills dirt and wall tiles looking for
 * an area of at most 40 tiles and disconnected from the map edge.
 * If it finds one, converts it to lava tiles and sets lavapool to TRUE.
 */
static void DRLG_L3Pool()
{
	int i, j, dunx, duny, totarea, poolchance;
	BOOL found;
	BYTE k;
	static BYTE poolsub[15] = { 0, 35, 26, 36, 25, 29, 34, 7, 33, 28, 27, 37, 32, 31, 30 };

	for (duny = 0; duny < DMAXY; duny++) {
		for (dunx = 0; dunx < DMAXY; dunx++) {
			if (dungeon[dunx][duny] != 8) {
				continue;
			}
			dungeon[dunx][duny] |= 0x80;
			totarea = 1;
			if (dunx + 1 < DMAXX) {
				found = DRLG_L3Spawn(dunx + 1, duny, &totarea);
			} else {
				found = TRUE;
			}
			if (dunx - 1 > 0 && !found) {
				found = DRLG_L3Spawn(dunx - 1, duny, &totarea);
			} else {
				found = TRUE;
			}
			if (duny + 1 < DMAXY && !found) {
				found = DRLG_L3Spawn(dunx, duny + 1, &totarea);
			} else {
				found = TRUE;
			}
			if (duny - 1 > 0 && !found) {
				found = DRLG_L3Spawn(dunx, duny - 1, &totarea);
			} else {
				found = TRUE;
			}
			poolchance = random_(0, 100);
			for (j = std::max(duny - totarea, 0); j < std::min(duny + totarea, DMAXY); j++) {
				for (i = std::max(dunx - totarea, 0); i < std::min(dunx + totarea, DMAXX); i++) {
					// BUGFIX: In the following swap the order to first do the
					// index checks and only then access dungeon[i][j] (fixed)
					if (dungeon[i][j] & 0x80) {
						dungeon[i][j] &= ~0x80;
						if (totarea > 4 && poolchance < 25 && !found) {
							k = poolsub[dungeon[i][j]];
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

static void DRLG_L3PoolFix()
{
	int dunx, duny;

	for (duny = 1; duny < DMAXY - 1; duny++) {     // BUGFIX: Change '0' to '1' and 'DMAXY' to 'DMAXY - 1' (fixed)
		for (dunx = 1; dunx < DMAXX - 1; dunx++) { // BUGFIX: Change '0' to '1' and 'DMAXX' to 'DMAXX - 1' (fixed)
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

static BOOL DRLG_L3PlaceMiniSet(const BYTE *miniset, int tmin, int tmax, int cx, int cy, BOOL setview, int ldir)
{
	int sx, sy, sw, sh, xx, yy, i, ii, numt, trys;
	BOOL found;

	sw = miniset[0];
	sh = miniset[1];

	if (tmax - tmin == 0) {
		numt = 1;
	} else {
		numt = random_(0, tmax - tmin) + tmin;
	}

	for (i = 0; i < numt; i++) {
		sx = random_(0, DMAXX - sw);
		sy = random_(0, DMAXY - sh);
		found = FALSE;
		trys = 0;
		while (!found && trys < 200) {
			trys++;
			found = TRUE;
			if (cx != -1 && sx >= cx - sw && sx <= cx + 12) {
				sx = random_(0, DMAXX - sw);
				sy = random_(0, DMAXY - sh);
				found = FALSE;
			}
			if (cy != -1 && sy >= cy - sh && sy <= cy + 12) {
				sx = random_(0, DMAXX - sw);
				sy = random_(0, DMAXY - sh);
				found = FALSE;
			}
			ii = 2;
			for (yy = 0; yy < sh && found == TRUE; yy++) {
				for (xx = 0; xx < sw && found == TRUE; xx++) {
					if (miniset[ii] != 0 && dungeon[xx + sx][yy + sy] != miniset[ii]) {
						found = FALSE;
					}
					if (dflags[xx + sx][yy + sy] != 0) {
						found = FALSE;
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
			return TRUE;
		}
		ii = sw * sh + 2;
		for (yy = 0; yy < sh; yy++) {
			for (xx = 0; xx < sw; xx++) {
				if (miniset[ii] != 0) {
					dungeon[xx + sx][yy + sy] = miniset[ii];
				}
				ii++;
			}
		}
	}

	if (setview == TRUE) {
		ViewX = 2 * sx + 17;
		ViewY = 2 * sy + 19;
	}
	if (ldir == 0) {
		LvlViewX = 2 * sx + 17;
		LvlViewY = 2 * sy + 19;
	}

	return FALSE;
}

static void DRLG_L3PlaceRndSet(const BYTE *miniset, int rndper)
{
	int sx, sy, sw, sh, xx, yy, ii, kk;
	BOOL found;

	sw = miniset[0];
	sh = miniset[1];

	for (sy = 0; sy < DMAXX - sh; sy++) {
		for (sx = 0; sx < DMAXY - sw; sx++) {
			found = TRUE;
			ii = 2;
			for (yy = 0; yy < sh && found == TRUE; yy++) {
				for (xx = 0; xx < sw && found == TRUE; xx++) {
					if (miniset[ii] != 0 && dungeon[xx + sx][yy + sy] != miniset[ii]) {
						found = FALSE;
					}
					if (dflags[xx + sx][yy + sy] != 0) {
						found = FALSE;
					}
					ii++;
				}
			}
			kk = sw * sh + 2;
			if (miniset[kk] >= 84 && miniset[kk] <= 100 && found == TRUE) {
				// BUGFIX: accesses to dungeon can go out of bounds (fixed)
				// BUGFIX: Comparisons vs 100 should use same tile as comparisons vs 84.
				if (sx - 1 >= 0 && dungeon[sx - 1][sy] >= 84 && dungeon[sx - 1][sy] <= 100) {
					found = FALSE;
				}
				if (sx + 1 < 40 && sx - 1 >= 0 && dungeon[sx + 1][sy] >= 84 && dungeon[sx - 1][sy] <= 100) {
					found = FALSE;
				}
				if (sy + 1 < 40 && sx - 1 >= 0 && dungeon[sx][sy + 1] >= 84 && dungeon[sx - 1][sy] <= 100) {
					found = FALSE;
				}
				if (sy - 1 >= 0 && sx - 1 >= 0 && dungeon[sx][sy - 1] >= 84 && dungeon[sx - 1][sy] <= 100) {
					found = FALSE;
				}
			}
			if (found == TRUE && random_(0, 100) < rndper) {
				for (yy = 0; yy < sh; yy++) {
					for (xx = 0; xx < sw; xx++) {
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

BOOLEAN drlg_l3_hive_rnd_piece(const BYTE *miniset, int rndper)
{
	int sx, sy, sw, sh, xx, yy, ii, kk;
	BOOL found;
	BOOLEAN placed;

	placed = FALSE;
	sw = miniset[0];
	sh = miniset[1];

	for (sy = 0; sy < DMAXX - sh; sy++) {
		for (sx = 0; sx < DMAXY - sw; sx++) {
			found = TRUE;
			ii = 2;
			for (yy = 0; yy < sh && found == TRUE; yy++) {
				for (xx = 0; xx < sw && found == TRUE; xx++) {
					if (miniset[ii] != 0 && dungeon[xx + sx][yy + sy] != miniset[ii]) {
						found = FALSE;
					}
					if (dflags[xx + sx][yy + sy] != 0) {
						found = FALSE;
					}
					ii++;
				}
			}
			kk = sw * sh + 2;
			if (miniset[kk] >= 84 && miniset[kk] <= 100 && found == TRUE) {
				// BUGFIX: accesses to dungeon can go out of bounds
				// BUGFIX: Comparisons vs 100 should use same tile as comparisons vs 84.
				if (dungeon[sx - 1][sy] >= 84 && dungeon[sx - 1][sy] <= 100) {
					found = FALSE;
				}
				if (dungeon[sx + 1][sy] >= 84 && dungeon[sx - 1][sy] <= 100) {
					found = FALSE;
				}
				if (dungeon[sx][sy + 1] >= 84 && dungeon[sx - 1][sy] <= 100) {
					found = FALSE;
				}
				if (dungeon[sx][sy - 1] >= 84 && dungeon[sx - 1][sy] <= 100) {
					found = FALSE;
				}
			}
			if (found == TRUE && random_(0, 100) < rndper) {
				placed = TRUE;
				for (yy = 0; yy < sh; yy++) {
					for (xx = 0; xx < sw; xx++) {
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

static BOOL WoodVertU(int i, int y)
{
	if ((dungeon[i + 1][y] > 152 || dungeon[i + 1][y] < 130)
	    && (dungeon[i - 1][y] > 152 || dungeon[i - 1][y] < 130)) {
		if (dungeon[i][y] == 7) {
			return TRUE;
		}
		if (dungeon[i][y] == 10) {
			return TRUE;
		}
		if (dungeon[i][y] == 126) {
			return TRUE;
		}
		if (dungeon[i][y] == 129) {
			return TRUE;
		}
		if (dungeon[i][y] == 134) {
			return TRUE;
		}
		if (dungeon[i][y] == 136) {
			return TRUE;
		}
	}

	return FALSE;
}

static BOOL WoodVertD(int i, int y)
{
	if ((dungeon[i + 1][y] > 152 || dungeon[i + 1][y] < 130)
	    && (dungeon[i - 1][y] > 152 || dungeon[i - 1][y] < 130)) {
		if (dungeon[i][y] == 7) {
			return TRUE;
		}
		if (dungeon[i][y] == 2) {
			return TRUE;
		}
		if (dungeon[i][y] == 134) {
			return TRUE;
		}
		if (dungeon[i][y] == 136) {
			return TRUE;
		}
	}

	return FALSE;
}

static BOOL WoodHorizL(int x, int j)
{
	if ((dungeon[x][j + 1] > 152 || dungeon[x][j + 1] < 130)
	    && (dungeon[x][j - 1] > 152 || dungeon[x][j - 1] < 130)) {
		if (dungeon[x][j] == 7) {
			return TRUE;
		}
		if (dungeon[x][j] == 9) {
			return TRUE;
		}
		if (dungeon[x][j] == 121) {
			return TRUE;
		}
		if (dungeon[x][j] == 124) {
			return TRUE;
		}
		if (dungeon[x][j] == 135) {
			return TRUE;
		}
		if (dungeon[x][j] == 137) {
			return TRUE;
		}
	}

	return FALSE;
}

static BOOL WoodHorizR(int x, int j)
{
	if ((dungeon[x][j + 1] > 152 || dungeon[x][j + 1] < 130)
	    && (dungeon[x][j - 1] > 152 || dungeon[x][j - 1] < 130)) {
		if (dungeon[x][j] == 7) {
			return TRUE;
		}
		if (dungeon[x][j] == 4) {
			return TRUE;
		}
		if (dungeon[x][j] == 135) {
			return TRUE;
		}
		if (dungeon[x][j] == 137) {
			return TRUE;
		}
	}

	return FALSE;
}

void AddFenceDoors()
{
	int i, j;

	for (j = 0; j < DMAXY; j++) {
		for (i = 0; i < DMAXX; i++) {
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
	int i, j;

	for (j = 0; j < DMAXY; j++) {
		for (i = 0; i < DMAXX; i++) {
			if (dungeon[i][j] == 146) {
				if (dungeon[i + 1][j] > 152 || dungeon[i + 1][j] < 130
				    || dungeon[i - 1][j] > 152 || dungeon[i - 1][j] < 130) {
					dungeon[i][j] = 7;
					continue;
				}
			}
			if (dungeon[i][j] == 146) {
				if (dungeon[i + 1][j] != 130 && dungeon[i - 1][j] != 130
				    && dungeon[i + 1][j] != 132 && dungeon[i - 1][j] != 132
				    && dungeon[i + 1][j] != 133 && dungeon[i - 1][j] != 133
				    && dungeon[i + 1][j] != 134 && dungeon[i - 1][j] != 134
				    && dungeon[i + 1][j] != 136 && dungeon[i - 1][j] != 136
				    && dungeon[i + 1][j] != 138 && dungeon[i - 1][j] != 138
				    && dungeon[i + 1][j] != 140 && dungeon[i - 1][j] != 140) {
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
				if (dungeon[i][j + 1] != 131 && dungeon[i][j - 1] != 131
				    && dungeon[i][j + 1] != 132 && dungeon[i][j - 1] != 132
				    && dungeon[i][j + 1] != 133 && dungeon[i][j - 1] != 133
				    && dungeon[i][j + 1] != 135 && dungeon[i][j - 1] != 135
				    && dungeon[i][j + 1] != 137 && dungeon[i][j - 1] != 137
				    && dungeon[i][j + 1] != 138 && dungeon[i][j - 1] != 138
				    && dungeon[i][j + 1] != 139 && dungeon[i][j - 1] != 139) {
					dungeon[i][j] = 7;
					continue;
				}
			}
		}
	}
}

static void DRLG_L3Wood()
{
#if (_MSC_VER >= 1920)
	volatile // visual studio 2019 throws internal compiler error without it, see #708

#endif
	int i, j, x, y, xx, yy, rt, rp, x1, y1, x2, y2;
	BOOL skip;

	for (j = 1; j < DMAXY - 1; j++) {     // BUGFIX: Change '0' to '1' (fixed)
		for (i = 1; i < DMAXX - 1; i++) { // BUGFIX: Change '0' to '1' (fixed)
			if (dungeon[i][j] == 10 && random_(0, 2) != 0) {
				x = i;
				while (dungeon[x][j] == 10) {
					x++;
				}
				x--;
				if (x - i > 0) {
					dungeon[i][j] = 127;
					for (xx = i + 1; xx < x; xx++) {
						if (random_(0, 2) != 0) {
							dungeon[xx][j] = 126;
						} else {
							dungeon[xx][j] = 129;
						}
					}
					dungeon[x][j] = 128;
				}
			}
			if (dungeon[i][j] == 9 && random_(0, 2) != 0) {
				y = j;
				while (dungeon[i][y] == 9) {
					y++;
				}
				y--;
				if (y - j > 0) {
					dungeon[i][j] = 123;
					for (yy = j + 1; yy < y; yy++) {
						if (random_(0, 2) != 0) {
							dungeon[i][yy] = 121;
						} else {
							dungeon[i][yy] = 124;
						}
					}
					dungeon[i][y] = 122;
				}
			}
			if (dungeon[i][j] == 11 && dungeon[i + 1][j] == 10 && dungeon[i][j + 1] == 9 && random_(0, 2) != 0) {
				dungeon[i][j] = 125;
				x = i + 1;
				while (dungeon[x][j] == 10) {
					x++;
				}
				x--;
				for (xx = i + 1; xx < x; xx++) {
					if (random_(0, 2) != 0) {
						dungeon[xx][j] = 126;
					} else {
						dungeon[xx][j] = 129;
					}
				}
				dungeon[x][j] = 128;
				y = j + 1;
				while (dungeon[i][y] == 9) {
					y++;
				}
				y--;
				for (yy = j + 1; yy < y; yy++) {
					if (random_(0, 2) != 0) {
						dungeon[i][yy] = 121;
					} else {
						dungeon[i][yy] = 124;
					}
				}
				dungeon[i][y] = 122;
			}
		}
	}

	for (j = 1; j < DMAXY; j++) {     // BUGFIX: Change '0' to '1' (fixed)
		for (i = 1; i < DMAXX; i++) { // BUGFIX: Change '0' to '1' (fixed)
			if (dungeon[i][j] == 7 && random_(0, 1) == 0 && SkipThemeRoom(i, j)) {
				rt = random_(0, 2);
				if (rt == 0) {
					y1 = j;
					// BUGFIX: Check `y1 >= 0` first (fixed)
					while (y1 >= 0 && WoodVertU(i, y1)) {
						y1--;
					}
					y1++;
					y2 = j;
					// BUGFIX: Check `y2 < DMAXY` first (fixed)
					while (y2 < DMAXY && WoodVertD(i, y2)) {
						y2++;
					}
					y2--;
					skip = TRUE;
					if (dungeon[i][y1] == 7) {
						skip = FALSE;
					}
					if (dungeon[i][y2] == 7) {
						skip = FALSE;
					}
					if (y2 - y1 > 1 && skip) {
						rp = random_(0, y2 - y1 - 1) + y1 + 1;
						for (y = y1; y <= y2; y++) {
							if (y == rp) {
								continue;
							}
							if (dungeon[i][y] == 7) {
								if (random_(0, 2) != 0) {
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
					x1 = i;
					// BUGFIX: Check `x1 >= 0` first (fixed)
					while (x1 >= 0 && WoodHorizL(x1, j)) {
						x1--;
					}
					x1++;
					x2 = i;
					// BUGFIX: Check `x2 < DMAXX` first (fixed)
					while (x2 < DMAXX && WoodHorizR(x2, j)) {
						x2++;
					}
					x2--;
					skip = TRUE;
					if (dungeon[x1][j] == 7) {
						skip = FALSE;
					}
					if (dungeon[x2][j] == 7) {
						skip = FALSE;
					}
					if (x2 - x1 > 1 && skip) {
						rp = random_(0, x2 - x1 - 1) + x1 + 1;
						for (x = x1; x <= x2; x++) {
							if (x == rp) {
								continue;
							}
							if (dungeon[x][j] == 7) {
								if (random_(0, 2) != 0) {
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

static BOOL DRLG_L3Anvil()
{
	int sx, sy, sw, sh, xx, yy, ii, trys;
	BOOL found;

	sw = L3ANVIL[0];
	sh = L3ANVIL[1];
	sx = random_(0, DMAXX - sw);
	sy = random_(0, DMAXY - sh);

	found = FALSE;
	trys = 0;
	while (!found && trys < 200) {
		trys++;
		found = TRUE;
		ii = 2;
		for (yy = 0; yy < sh && found == TRUE; yy++) {
			for (xx = 0; xx < sw && found == TRUE; xx++) {
				if (L3ANVIL[ii] != 0 && dungeon[xx + sx][yy + sy] != L3ANVIL[ii]) {
					found = FALSE;
				}
				if (dflags[xx + sx][yy + sy] != 0) {
					found = FALSE;
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
		return TRUE;
	}

	ii = sw * sh + 2;
	for (yy = 0; yy < sh; yy++) {
		for (xx = 0; xx < sw; xx++) {
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

	return FALSE;
}

static void FixL3Warp()
{
	int i, j;

	for (j = 0; j < DMAXY; j++) {
		for (i = 0; i < DMAXX; i++) {
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

static void FixL3HallofHeroes()
{
	int i, j;

	for (j = 0; j < DMAXY; j++) {
		for (i = 0; i < DMAXX; i++) {
			if (dungeon[i][j] == 5 && dungeon[i + 1][j + 1] == 7) {
				dungeon[i][j] = 7;
			}
		}
	}
	for (j = 0; j < DMAXY; j++) {
		for (i = 0; i < DMAXX; i++) {
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

static void DRLG_L3LockRec(int x, int y)
{
	if (!lockout[x][y]) {
		return;
	}

	lockout[x][y] = FALSE;
	lockoutcnt++;
	DRLG_L3LockRec(x, y - 1);
	DRLG_L3LockRec(x, y + 1);
	DRLG_L3LockRec(x - 1, y);
	DRLG_L3LockRec(x + 1, y);
}

BOOL DRLG_L3Lockout()
{
	int i, j, t, fx, fy;

	t = 0;
	for (j = 0; j < DMAXY; j++) {
		for (i = 0; i < DMAXX; i++) {
			if (dungeon[i][j] != 0) {
				lockout[i][j] = TRUE;
				fx = i;
				fy = j;
				t++;
			} else {
				lockout[i][j] = FALSE;
			}
		}
	}

	lockoutcnt = 0;
	DRLG_L3LockRec(fx, fy);

	return t == lockoutcnt;
}

static void DRLG_L3(int entry)
{
	int x1, y1, x2, y2, i, j;
	BOOL found, genok;

	lavapool = 0;

	do {
		do {
			do {
				InitL3Dungeon();
				x1 = random_(0, 20) + 10;
				y1 = random_(0, 20) + 10;
				x2 = x1 + 2;
				y2 = y1 + 2;
				DRLG_L3FillRoom(x1, y1, x2, y2);
				DRLG_L3CreateBlock(x1, y1, 2, 0);
				DRLG_L3CreateBlock(x2, y1, 2, 1);
				DRLG_L3CreateBlock(x1, y2, 2, 2);
				DRLG_L3CreateBlock(x1, y1, 2, 3);
				if (QuestStatus(Q_ANVIL)) {
					x1 = random_(0, 10) + 10;
					y1 = random_(0, 10) + 10;
					x2 = x1 + 12;
					y2 = y1 + 12;
					DRLG_L3FloorArea(x1, y1, x2, y2);
				}
				DRLG_L3FillDiags();
				DRLG_L3FillSingles();
				DRLG_L3FillStraights();
				DRLG_L3FillDiags();
				DRLG_L3Edges();
				if (DRLG_L3GetFloorArea() >= 600) {
					found = DRLG_L3Lockout();
				} else {
					found = FALSE;
				}
			} while (!found);
			DRLG_L3MakeMegas();
			if (entry == ENTRY_MAIN) {
				if (currlevel < 17) {
					genok = DRLG_L3PlaceMiniSet(L3UP, 1, 1, -1, -1, TRUE, 0);
				} else {
					if (currlevel != 17)
						genok = DRLG_L3PlaceMiniSet(L6UP, 1, 1, -1, -1, TRUE, 0);
					else
						genok = DRLG_L3PlaceMiniSet(L6HOLDWARP, 1, 1, -1, -1, TRUE, 6);
				}
				if (!genok) {
					if (currlevel < 17) {
						genok = DRLG_L3PlaceMiniSet(L3DOWN, 1, 1, -1, -1, FALSE, 1);
					} else {
						if (currlevel != 20)
							genok = DRLG_L3PlaceMiniSet(L6DOWN, 1, 1, -1, -1, FALSE, 1);
					}
					if (!genok && currlevel == 9) {
						genok = DRLG_L3PlaceMiniSet(L3HOLDWARP, 1, 1, -1, -1, FALSE, 6);
					}
				}
			} else if (entry == ENTRY_PREV) {
				if (currlevel < 17) {
					genok = DRLG_L3PlaceMiniSet(L3UP, 1, 1, -1, -1, FALSE, 0);
				} else {
					if (currlevel != 17)
						genok = DRLG_L3PlaceMiniSet(L6UP, 1, 1, -1, -1, FALSE, 0);
					else
						genok = DRLG_L3PlaceMiniSet(L6HOLDWARP, 1, 1, -1, -1, FALSE, 6);
				}
				if (!genok) {
					if (currlevel < 17) {
						genok = DRLG_L3PlaceMiniSet(L3DOWN, 1, 1, -1, -1, TRUE, 1);
						ViewX += 2;
						ViewY -= 2;
					} else {
						if (currlevel != 20) {
							genok = DRLG_L3PlaceMiniSet(L6DOWN, 1, 1, -1, -1, TRUE, 1);
							ViewX += 2;
							ViewY -= 2;
						}
					}
					if (!genok && currlevel == 9) {
						genok = DRLG_L3PlaceMiniSet(L3HOLDWARP, 1, 1, -1, -1, FALSE, 6);
					}
				}
			} else {
				if (currlevel < 17) {
					genok = DRLG_L3PlaceMiniSet(L3UP, 1, 1, -1, -1, FALSE, 0);
				} else {
					if (currlevel != 17)
						genok = DRLG_L3PlaceMiniSet(L6UP, 1, 1, -1, -1, FALSE, 0);
					else
						genok = DRLG_L3PlaceMiniSet(L6HOLDWARP, 1, 1, -1, -1, TRUE, 6);
				}
				if (!genok) {
					if (currlevel < 17) {
						genok = DRLG_L3PlaceMiniSet(L3DOWN, 1, 1, -1, -1, FALSE, 1);
					} else {
						if (currlevel != 20)
							genok = DRLG_L3PlaceMiniSet(L6DOWN, 1, 1, -1, -1, FALSE, 1);
					}
					if (!genok && currlevel == 9) {
						genok = DRLG_L3PlaceMiniSet(L3HOLDWARP, 1, 1, -1, -1, TRUE, 6);
					}
				}
			}
			if (!genok && QuestStatus(Q_ANVIL)) {
				genok = DRLG_L3Anvil();
			}
		} while (genok == TRUE);
		if (currlevel < 17) {
			DRLG_L3Pool();
		} else {
			lavapool += drlg_l3_hive_rnd_piece(byte_48A998, 30);
			lavapool += drlg_l3_hive_rnd_piece(byte_48A9C8, 40);
			lavapool += drlg_l3_hive_rnd_piece(byte_48A948, 50);
			lavapool += drlg_l3_hive_rnd_piece(byte_48A970, 60);
			if (lavapool < 3)
				lavapool = 0;
		}
	} while (lavapool == 0);

	if (currlevel < 17)
		DRLG_L3PoolFix();
	if (currlevel < 17)
		FixL3Warp();

	if (currlevel < 17) {
		DRLG_L3PlaceRndSet(L3ISLE1, 70);
		DRLG_L3PlaceRndSet(L3ISLE2, 70);
		DRLG_L3PlaceRndSet(L3ISLE3, 30);
		DRLG_L3PlaceRndSet(L3ISLE4, 30);
		DRLG_L3PlaceRndSet(L3ISLE1, 100);
		DRLG_L3PlaceRndSet(L3ISLE2, 100);
		DRLG_L3PlaceRndSet(L3ISLE5, 90);
	} else {
		DRLG_L3PlaceRndSet(L6ISLE1, 70);
		DRLG_L3PlaceRndSet(L6ISLE2, 70);
		DRLG_L3PlaceRndSet(L6ISLE3, 30);
		DRLG_L3PlaceRndSet(L6ISLE4, 30);
		DRLG_L3PlaceRndSet(L6ISLE1, 100);
		DRLG_L3PlaceRndSet(L6ISLE2, 100);
		DRLG_L3PlaceRndSet(L6ISLE5, 90);
	}

	if (currlevel < 17)
		FixL3HallofHeroes();
	if (currlevel < 17)
		DRLG_L3River();

	if (QuestStatus(Q_ANVIL)) {
		dungeon[setpc_x + 7][setpc_y + 5] = 7;
		dungeon[setpc_x + 8][setpc_y + 5] = 7;
		dungeon[setpc_x + 9][setpc_y + 5] = 7;
		if (dungeon[setpc_x + 10][setpc_y + 5] == 17 || dungeon[setpc_x + 10][setpc_y + 5] == 18) {
			dungeon[setpc_x + 10][setpc_y + 5] = 45;
		}
	}

	if (currlevel < 17)
		DRLG_PlaceThemeRooms(5, 10, 7, 0, 0);

	if (currlevel < 17) {
		DRLG_L3Wood();
		DRLG_L3PlaceRndSet(L3TITE1, 10);
		DRLG_L3PlaceRndSet(L3TITE2, 10);
		DRLG_L3PlaceRndSet(L3TITE3, 10);
		DRLG_L3PlaceRndSet(L3TITE6, 20);
		DRLG_L3PlaceRndSet(L3TITE7, 20);
		DRLG_L3PlaceRndSet(L3TITE8, 20);
		DRLG_L3PlaceRndSet(L3TITE9, 20);
		DRLG_L3PlaceRndSet(L3TITE10, 20);
		DRLG_L3PlaceRndSet(L3TITE11, 30);
		DRLG_L3PlaceRndSet(L3TITE12, 20);
		DRLG_L3PlaceRndSet(L3TITE13, 20);
		DRLG_L3PlaceRndSet(L3CREV1, 30);
		DRLG_L3PlaceRndSet(L3CREV2, 30);
		DRLG_L3PlaceRndSet(L3CREV3, 30);
		DRLG_L3PlaceRndSet(L3CREV4, 30);
		DRLG_L3PlaceRndSet(L3CREV5, 30);
		DRLG_L3PlaceRndSet(L3CREV6, 30);
		DRLG_L3PlaceRndSet(L3CREV7, 30);
		DRLG_L3PlaceRndSet(L3CREV8, 30);
		DRLG_L3PlaceRndSet(L3CREV9, 30);
		DRLG_L3PlaceRndSet(L3CREV10, 30);
		DRLG_L3PlaceRndSet(L3CREV11, 30);
		DRLG_L3PlaceRndSet(L3XTRA1, 25);
		DRLG_L3PlaceRndSet(L3XTRA2, 25);
		DRLG_L3PlaceRndSet(L3XTRA3, 25);
		DRLG_L3PlaceRndSet(L3XTRA4, 25);
		DRLG_L3PlaceRndSet(L3XTRA5, 25);
	} else {
		DRLG_L3PlaceRndSet(byte_48A76C, 20);
		DRLG_L3PlaceRndSet(byte_48A770, 20);
		DRLG_L3PlaceRndSet(byte_48A774, 20);
		DRLG_L3PlaceRndSet(byte_48A778, 20);
		DRLG_L3PlaceRndSet(byte_48A808, 10);
		DRLG_L3PlaceRndSet(byte_48A820, 15);
		DRLG_L3PlaceRndSet(byte_48A838, 20);
		DRLG_L3PlaceRndSet(byte_48A850, 25);
		DRLG_L3PlaceRndSet(byte_48A868, 30);
		DRLG_L3PlaceRndSet(byte_48A880, 35);
		DRLG_L3PlaceRndSet(byte_48A898, 40);
		DRLG_L3PlaceRndSet(byte_48A8B0, 45);
		DRLG_L3PlaceRndSet(byte_48A8C8, 50);
		DRLG_L3PlaceRndSet(byte_48A8E0, 55);
		DRLG_L3PlaceRndSet(byte_48A8E0, 10);
		DRLG_L3PlaceRndSet(byte_48A8C8, 15);
		DRLG_L3PlaceRndSet(byte_48A8B0, 20);
		DRLG_L3PlaceRndSet(byte_48A898, 25);
		DRLG_L3PlaceRndSet(byte_48A880, 30);
		DRLG_L3PlaceRndSet(byte_48A868, 35);
		DRLG_L3PlaceRndSet(byte_48A850, 40);
		DRLG_L3PlaceRndSet(byte_48A838, 45);
		DRLG_L3PlaceRndSet(byte_48A820, 50);
		DRLG_L3PlaceRndSet(byte_48A808, 55);
		DRLG_L3PlaceRndSet(byte_48A790, 40);
		DRLG_L3PlaceRndSet(byte_48A7A8, 45);
		DRLG_L3PlaceRndSet(byte_48A77C, 25);
		DRLG_L3PlaceRndSet(byte_48A780, 25);
		DRLG_L3PlaceRndSet(byte_48A784, 25);
		DRLG_L3PlaceRndSet(byte_48A788, 25);
		DRLG_L3PlaceRndSet(byte_48A7BC, 25);
		DRLG_L3PlaceRndSet(byte_48A7C0, 25);
		DRLG_L3PlaceRndSet(byte_48A7C4, 25);
		DRLG_L3PlaceRndSet(byte_48A7C8, 25);
		DRLG_L3PlaceRndSet(byte_48A7CC, 25);
		DRLG_L3PlaceRndSet(byte_48A7D4, 25);
		DRLG_L3PlaceRndSet(byte_48A7D8, 25);
		DRLG_L3PlaceRndSet(byte_48A7DC, 25);
		DRLG_L3PlaceRndSet(byte_48A7E0, 25);
		DRLG_L3PlaceRndSet(byte_48A7E4, 25);
		DRLG_L3PlaceRndSet(byte_48A7EC, 25);
		DRLG_L3PlaceRndSet(byte_48A7F0, 25);
		DRLG_L3PlaceRndSet(byte_48A7F4, 25);
		DRLG_L3PlaceRndSet(byte_48A7F8, 25);
		DRLG_L3PlaceRndSet(byte_48A7D0, 25);
		DRLG_L3PlaceRndSet(byte_48A7E8, 25);
		DRLG_L3PlaceRndSet(byte_48A7FC, 25);
		DRLG_L3PlaceRndSet(byte_48A800, 25);
	}

	for (j = 0; j < DMAXY; j++) {
		for (i = 0; i < DMAXX; i++) {
			pdungeon[i][j] = dungeon[i][j];
		}
	}

	DRLG_Init_Globals();
}

static void DRLG_L3Pass3()
{
	int i, j, xx, yy;
	long v1, v2, v3, v4, lv;
	WORD *MegaTiles;

	lv = 8 - 1;

	MegaTiles = (WORD *)&pMegaTiles[lv * 8];
	v1 = SDL_SwapLE16(*(MegaTiles + 0)) + 1;
	v2 = SDL_SwapLE16(*(MegaTiles + 1)) + 1;
	v3 = SDL_SwapLE16(*(MegaTiles + 2)) + 1;
	v4 = SDL_SwapLE16(*(MegaTiles + 3)) + 1;

	for (j = 0; j < MAXDUNY; j += 2) {
		for (i = 0; i < MAXDUNX; i += 2) {
			dPiece[i][j] = v1;
			dPiece[i + 1][j] = v2;
			dPiece[i][j + 1] = v3;
			dPiece[i + 1][j + 1] = v4;
		}
	}

	yy = 16;
	for (j = 0; j < DMAXY; j++) {
		xx = 16;
		for (i = 0; i < DMAXX; i++) {
			lv = dungeon[i][j] - 1;
			if (lv >= 0) {
				MegaTiles = (WORD *)&pMegaTiles[lv * 8];
				v1 = SDL_SwapLE16(*(MegaTiles + 0)) + 1;
				v2 = SDL_SwapLE16(*(MegaTiles + 1)) + 1;
				v3 = SDL_SwapLE16(*(MegaTiles + 2)) + 1;
				v4 = SDL_SwapLE16(*(MegaTiles + 3)) + 1;
			} else {
				v1 = 0;
				v2 = 0;
				v3 = 0;
				v4 = 0;
			}
			dPiece[xx][yy] = v1;
			dPiece[xx + 1][yy] = v2;
			dPiece[xx][yy + 1] = v3;
			dPiece[xx + 1][yy + 1] = v4;
			xx += 2;
		}
		yy += 2;
	}
}

void CreateL3Dungeon(DWORD rseed, int entry)
{
	int i, j;

	SetRndSeed(rseed);
	dminx = 16;
	dminy = 16;
	dmaxx = 96;
	dmaxy = 96;
	DRLG_InitTrans();
	DRLG_InitSetPC();
	DRLG_L3(entry);
	DRLG_L3Pass3();

	if (currlevel < 17) {
		for (j = 0; j < MAXDUNY; j++) {
			for (i = 0; i < MAXDUNX; i++) {
				if (dPiece[i][j] >= 56 && dPiece[i][j] <= 147) {
					DoLighting(i, j, 7, -1);
				} else if (dPiece[i][j] >= 154 && dPiece[i][j] <= 161) {
					DoLighting(i, j, 7, -1);
				} else if (dPiece[i][j] == 150) {
					DoLighting(i, j, 7, -1);
				} else if (dPiece[i][j] == 152) {
					DoLighting(i, j, 7, -1);
				}
			}
		}
	} else {
		for (j = 0; j < MAXDUNY; j++) {
			for (i = 0; i < MAXDUNX; i++) {
				if (dPiece[i][j] >= 382 && dPiece[i][j] <= 457) {
					DoLighting(i, j, 9, -1);
				}
			}
		}
	}

	DRLG_SetPC();
}

void LoadL3Dungeon(const char *sFileName, int vx, int vy)
{
	int i, j, rw, rh;
	BYTE *pLevelMap, *lm;

	InitL3Dungeon();
	dminx = 16;
	dminy = 16;
	dmaxx = 96;
	dmaxy = 96;
	DRLG_InitTrans();
	pLevelMap = LoadFileInMem(sFileName, NULL);

	lm = pLevelMap;
	rw = *lm;
	lm += 2;
	rh = *lm;
	lm += 2;

	for (j = 0; j < rh; j++) {
		for (i = 0; i < rw; i++) {
			if (*lm != 0) {
				dungeon[i][j] = *lm;
			} else {
				dungeon[i][j] = 7;
			}
			lm += 2;
		}
	}
	for (j = 0; j < DMAXY; j++) {
		for (i = 0; i < DMAXX; i++) {
			if (dungeon[i][j] == 0) {
				dungeon[i][j] = 8;
			}
		}
	}

	DRLG_L3Pass3();
	DRLG_Init_Globals();
	ViewX = vx;
	ViewY = vy;
	SetMapMonsters(pLevelMap, 0, 0);
	SetMapObjects(pLevelMap, 0, 0);

	for (j = 0; j < MAXDUNY; j++) {
		for (i = 0; i < MAXDUNX; i++) {
			if (dPiece[i][j] >= 56 && dPiece[i][j] <= 147) {
				DoLighting(i, j, 7, -1);
			} else if (dPiece[i][j] >= 154 && dPiece[i][j] <= 161) {
				DoLighting(i, j, 7, -1);
			} else if (dPiece[i][j] == 150) {
				DoLighting(i, j, 7, -1);
			} else if (dPiece[i][j] == 152) {
				DoLighting(i, j, 7, -1);
			}
		}
	}

	mem_free_dbg(pLevelMap);
}

void LoadPreL3Dungeon(const char *sFileName, int vx, int vy)
{
	int i, j, rw, rh;
	BYTE *pLevelMap, *lm;

	InitL3Dungeon();
	DRLG_InitTrans();
	pLevelMap = LoadFileInMem(sFileName, NULL);

	lm = pLevelMap;
	rw = *lm;
	lm += 2;
	rh = *lm;
	lm += 2;

	for (j = 0; j < rh; j++) {
		for (i = 0; i < rw; i++) {
			if (*lm != 0) {
				dungeon[i][j] = *lm;
			} else {
				dungeon[i][j] = 7;
			}
			lm += 2;
		}
	}
	for (j = 0; j < DMAXY; j++) {
		for (i = 0; i < DMAXX; i++) {
			if (dungeon[i][j] == 0) {
				dungeon[i][j] = 8;
			}
		}
	}

	memcpy(pdungeon, dungeon, sizeof(pdungeon));
	mem_free_dbg(pLevelMap);
}

DEVILUTION_END_NAMESPACE

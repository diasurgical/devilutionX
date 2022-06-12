/**
 * @file drlg_l1.cpp
 *
 * Implementation of the cathedral level generation algorithms.
 */
#include "drlg_l1.h"

#include "engine/load_file.hpp"
#include "engine/point.hpp"
#include "engine/random.hpp"
#include "gendung.h"
#include "player.h"
#include "quests.h"

namespace devilution {

int UberRow;
int UberCol;
bool IsUberRoomOpened;
bool IsUberLeverActivated;
int UberDiabloMonsterIndex;

namespace {

/** Represents a tile ID map of twice the size, repeating each tile of the original map in blocks of 4. */
BYTE L5dungeon[80][80];
/** Marks where walls may not be added to the level */
bool Chamber[DMAXX][DMAXX];
/** Specifies whether a single player quest DUN has been loaded. */
bool L5setloadflag;
/** Specifies whether to generate a horizontal room at position 1 in the Cathedral. */
bool HR1;
/** Specifies whether to generate a horizontal room at position 2 in the Cathedral. */
bool HR2;
/** Specifies whether to generate a horizontal room at position 3 in the Cathedral. */
bool HR3;

/** Specifies whether to generate a vertical room at position 1 in the Cathedral. */
bool VR1;
/** Specifies whether to generate a vertical room at position 2 in the Cathedral. */
bool VR2;
/** Specifies whether to generate a vertical room at position 3 in the Cathedral. */
bool VR3;
/** Contains the contents of the single player quest DUN file. */
std::unique_ptr<uint16_t[]> L5pSetPiece;

/** Contains shadows for 2x2 blocks of base tile IDs in the Cathedral. */
const ShadowStruct SPATS[37] = {
	// clang-format off
	// strig, s1, s2, s3, nv1, nv2, nv3
	{      7, 13,  0, 13, 144,   0, 142 },
	{     16, 13,  0, 13, 144,   0, 142 },
	{     15, 13,  0, 13, 145,   0, 142 },
	{      5, 13, 13, 13, 152, 140, 139 },
	{      5, 13,  1, 13, 143, 146, 139 },
	{      5, 13, 13,  2, 143, 140, 148 },
	{      5,  0,  1,  2,   0, 146, 148 },
	{      5, 13, 11, 13, 143, 147, 139 },
	{      5, 13, 13, 12, 143, 140, 149 },
	{      5, 13, 11, 12, 150, 147, 149 },
	{      5, 13,  1, 12, 143, 146, 149 },
	{      5, 13, 11,  2, 143, 147, 148 },
	{      9, 13, 13, 13, 144, 140, 142 },
	{      9, 13,  1, 13, 144, 146, 142 },
	{      9, 13, 11, 13, 151, 147, 142 },
	{      8, 13,  0, 13, 144,   0, 139 },
	{      8, 13,  0, 12, 143,   0, 149 },
	{      8,  0,  0,  2,   0,   0, 148 },
	{     11,  0,  0, 13,   0,   0, 139 },
	{     11, 13,  0, 13, 139,   0, 139 },
	{     11,  2,  0, 13, 148,   0, 139 },
	{     11, 12,  0, 13, 149,   0, 139 },
	{     11, 13, 11, 12, 139,   0, 149 },
	{     14,  0,  0, 13,   0,   0, 139 },
	{     14, 13,  0, 13, 139,   0, 139 },
	{     14,  2,  0, 13, 148,   0, 139 },
	{     14, 12,  0, 13, 149,   0, 139 },
	{     14, 13, 11, 12, 139,   0, 149 },
	{     10,  0, 13,  0,   0, 140,   0 },
	{     10, 13, 13,  0, 140, 140,   0 },
	{     10,  0,  1,  0,   0, 146,   0 },
	{     10, 13, 11,  0, 140, 147,   0 },
	{     12,  0, 13,  0,   0, 140,   0 },
	{     12, 13, 13,  0, 140, 140,   0 },
	{     12,  0,  1,  0,   0, 146,   0 },
	{     12, 13, 11,  0, 140, 147,   0 },
	{      3, 13, 11, 12, 150,   0,   0 }
	// clang-format on
};

// BUGFIX: This array should contain an additional 0 (207 elements).
/** Maps tile IDs to their corresponding base tile ID. */
const BYTE BSTYPES[] = {
	0, 1, 2, 3, 4, 5, 6, 7, 8, 9,
	10, 11, 12, 13, 14, 15, 16, 17, 0, 0,
	0, 0, 0, 0, 0, 1, 2, 10, 4, 5,
	6, 7, 8, 9, 10, 11, 12, 14, 5, 14,
	10, 4, 14, 4, 5, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0, 0, 1,
	2, 3, 4, 1, 6, 7, 16, 17, 2, 1,
	1, 2, 2, 1, 1, 2, 2, 2, 2, 2,
	1, 1, 11, 1, 13, 13, 13, 1, 2, 1,
	2, 1, 2, 1, 2, 2, 2, 2, 12, 0,
	0, 11, 1, 11, 1, 13, 0, 0, 0, 0,
	0, 0, 0, 13, 13, 13, 13, 13, 13, 13,
	13, 13, 13, 13, 13, 13, 1, 11, 2, 12,
	13, 13, 13, 12, 2, 1, 2, 2, 4, 14,
	4, 10, 13, 13, 4, 4, 1, 1, 4, 2,
	2, 13, 13, 13, 13, 25, 26, 28, 30, 31,
	41, 43, 40, 41, 42, 43, 25, 41, 43, 28,
	28, 1, 2, 25, 26, 22, 22, 25, 26, 0,
	0, 0, 0, 0, 0, 0, 0
};

// BUGFIX: This array should contain an additional 0 (207 elements) (fixed).
/** Maps tile IDs to their corresponding undecorated tile ID. */
const BYTE L5BTYPES[] = {
	0, 1, 2, 3, 4, 5, 6, 7, 8, 9,
	10, 11, 12, 13, 14, 15, 16, 17, 0, 0,
	0, 0, 0, 0, 0, 25, 26, 0, 28, 0,
	30, 31, 0, 0, 0, 0, 0, 0, 0, 0,
	40, 41, 42, 43, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0, 0, 79,
	80, 0, 82, 0, 0, 0, 0, 0, 0, 79,
	0, 80, 0, 0, 79, 80, 0, 2, 2, 2,
	1, 1, 11, 25, 13, 13, 13, 1, 2, 1,
	2, 1, 2, 1, 2, 2, 2, 2, 12, 0,
	0, 11, 1, 11, 1, 13, 0, 0, 0, 0,
	0, 0, 0, 13, 13, 13, 13, 13, 13, 0,
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0
};
/** Miniset: stairs up on a corner wall. */
const Miniset STAIRSUP {
	{ 4, 4 },
	{
	    { 13, 13, 13, 13 },
	    { 2, 2, 2, 2 },
	    { 13, 13, 13, 13 },
	    { 13, 13, 13, 13 },
	},
	{
	    { 0, 66, 6, 0 },
	    { 63, 64, 65, 0 },
	    { 0, 67, 68, 0 },
	    { 0, 0, 0, 0 },
	}
};
const Miniset L5STAIRSUPHF {
	{ 4, 5 },
	{
	    { 22, 22, 22, 22 },
	    { 22, 22, 22, 22 },
	    { 2, 2, 2, 2 },
	    { 13, 13, 13, 13 },
	    { 13, 13, 13, 13 },
	},
	{
	    { 0, 54, 23, 0 },
	    { 0, 53, 18, 0 },
	    { 55, 56, 57, 0 },
	    { 58, 59, 60, 0 },
	    { 0, 0, 0, 0 },
	}
};
/** Miniset: stairs up. */
const Miniset L5STAIRSUP {
	{ 4, 4 },
	{
	    { 22, 22, 22, 22 },
	    { 2, 2, 2, 2 },
	    { 13, 13, 13, 13 },
	    { 13, 13, 13, 13 },
	},
	{
	    { 0, 66, 23, 0 },
	    { 63, 64, 65, 0 },
	    { 0, 67, 68, 0 },
	    { 0, 0, 0, 0 },
	}
};
/** Miniset: stairs down. */
const Miniset STAIRSDOWN {
	{ 4, 3 },
	{
	    { 13, 13, 13, 13 },
	    { 13, 13, 13, 13 },
	    { 13, 13, 13, 13 },
	},
	{
	    { 62, 57, 58, 0 },
	    { 61, 59, 60, 0 },
	    { 0, 0, 0, 0 },
	}
};
const Miniset L5STAIRSDOWN {
	{ 4, 5 },
	{
	    { 13, 13, 13, 13 },
	    { 13, 13, 13, 13 },
	    { 13, 13, 13, 13 },
	    { 13, 13, 13, 13 },
	    { 13, 13, 13, 13 },
	},
	{
	    { 0, 0, 52, 0 },
	    { 0, 48, 51, 0 },
	    { 0, 47, 50, 0 },
	    { 45, 46, 49, 0 },
	    { 0, 0, 0, 0 },
	}
};
const Miniset L5STAIRSTOWN {
	{ 4, 5 },
	{
	    { 22, 22, 22, 22 },
	    { 22, 22, 22, 22 },
	    { 2, 2, 2, 2 },
	    { 13, 13, 13, 13 },
	    { 13, 13, 13, 13 },
	},
	{
	    { 0, 62, 23, 0 },
	    { 0, 61, 18, 0 },
	    { 63, 64, 65, 0 },
	    { 66, 67, 68, 0 },
	    { 0, 0, 0, 0 },
	}
};
/** Miniset: candlestick. */
const Miniset LAMPS {
	{ 2, 2 },
	{
	    { 13, 0 },
	    { 13, 13 },
	},
	{
	    { 129, 0 },
	    { 130, 128 },
	}
};
/** Miniset: Poisoned Water Supply entrance. */
const Miniset PWATERIN {
	{ 6, 6 },
	{
	    { 13, 13, 13, 13, 13, 13 },
	    { 13, 13, 13, 13, 13, 13 },
	    { 13, 13, 13, 13, 13, 13 },
	    { 13, 13, 13, 13, 13, 13 },
	    { 13, 13, 13, 13, 13, 13 },
	    { 13, 13, 13, 13, 13, 13 },
	},
	{
	    { 0, 0, 0, 0, 0, 0 },
	    { 0, 202, 200, 200, 84, 0 },
	    { 0, 199, 203, 203, 83, 0 },
	    { 0, 85, 206, 80, 81, 0 },
	    { 0, 0, 134, 135, 0, 0 },
	    { 0, 0, 0, 0, 0, 0 },
	}
};
const Miniset VWallSection {
	{ 1, 3 },
	{
	    { 1 },
	    { 1 },
	    { 1 },
	},
	{
	    { 91 },
	    { 90 },
	    { 89 },
	}
};
const Miniset HWallSection {
	{ 3, 1 },
	{ { 2, 2, 2 } },
	{ { 94, 93, 92 } }
};
const Miniset CryptFloorLave {
	{ 3, 3 },
	{
	    { 13, 13, 13 },
	    { 13, 13, 13 },
	    { 13, 13, 13 },
	},
	{
	    { 0, 0, 0 },
	    { 0, 101, 0 },
	    { 0, 0, 0 },
	}
};
const Miniset CryptPillar1 {
	{ 3, 3 },
	{
	    { 13, 13, 13 },
	    { 13, 13, 13 },
	    { 13, 13, 13 },
	},
	{
	    { 0, 0, 0 },
	    { 0, 167, 0 },
	    { 0, 0, 0 },
	}
};
const Miniset CryptPillar2 {
	{ 3, 3 },
	{
	    { 13, 13, 13 },
	    { 13, 13, 13 },
	    { 13, 13, 13 },
	},
	{
	    { 0, 0, 0 },
	    { 0, 168, 0 },
	    { 0, 0, 0 },
	}
};
const Miniset CryptPillar3 {
	{ 3, 3 },
	{
	    { 13, 13, 13 },
	    { 13, 13, 13 },
	    { 13, 13, 13 },
	},
	{
	    { 0, 0, 0 },
	    { 0, 169, 0 },
	    { 0, 0, 0 },
	}
};
const Miniset CryptPillar4 {
	{ 3, 3 },
	{
	    { 13, 13, 13 },
	    { 13, 13, 13 },
	    { 13, 13, 13 },
	},
	{
	    { 0, 0, 0 },
	    { 0, 170, 0 },
	    { 0, 0, 0 },
	}
};
const Miniset CryptPillar5 {
	{ 3, 3 },
	{
	    { 13, 13, 13 },
	    { 13, 13, 13 },
	    { 13, 13, 13 },
	},
	{
	    { 0, 0, 0 },
	    { 0, 171, 0 },
	    { 0, 0, 0 },
	}
};
const Miniset CryptStar {
	{ 3, 3 },
	{
	    { 13, 13, 13 },
	    { 13, 13, 13 },
	    { 13, 13, 13 },
	},
	{
	    { 0, 0, 0 },
	    { 0, 172, 0 },
	    { 0, 0, 0 },
	}
};

const Miniset UberRoomPattern {
	{ 4, 6 },
	{},
	{
	    { 115, 130, 6, 13 },
	    { 129, 108, 1, 13 },
	    { 1, 107, 103, 13 },
	    { 146, 106, 102, 13 },
	    { 129, 168, 1, 13 },
	    { 7, 2, 3, 13 },
	}
};
const Miniset CornerstoneRoomPattern {
	{ 5, 5 },
	{},
	{
	    { 4, 2, 2, 2, 6 },
	    { 1, 111, 172, 13, 1 },
	    { 1, 172, 13, 13, 25 },
	    { 1, 13, 13, 13, 1 },
	    { 7, 2, 2, 2, 3 },
	}
};
/**
 * A lookup table for the 16 possible patterns of a 2x2 area,
 * where each cell either contains a SW wall or it doesn't.
 */
BYTE L5ConvTbl[16] = { 22, 13, 1, 13, 2, 13, 13, 13, 4, 13, 1, 13, 2, 13, 16, 13 };

enum Tile : uint8_t {
	// clang-format off
	VWall          =  1,
	HWall          =  2,
	Corner         =  3,
	DWall          =  4,
	DArch          =  5,
	VWallEnd       =  6,
	HWallEnd       =  7,
	HArchEnd       =  8,
	VArchEnd       =  9,
	HArchVWall     = 10,
	VArch          = 11,
	HArch          = 12,
	Floor          = 13,
	HWallVArch     = 14,
	Pillar         = 15,
	Pillar1        = 16,
	Pillar2        = 17,
	DirtCorner     = 21,
	VDoor          = 25,
	HDoor          = 26,
	HFenceVWall    = 27,
	HDoorVDoor     = 28,
	VDoorEnd       = 30,
	HDoorEnd       = 31,
	VFence         = 35,
	HFence         = 36,
	HWallVFence    = 37,
	EntranceStairs = 64,
	// clang-format on
};

enum CryptTile : uint8_t {
	// clang-format off
	VWall5      =  89,
	VWall6      =  90,
	VWall7      =  91,
	HWall5      =  92,
	HWall6      =  93,
	HWall7      =  94,
	VArch5      =  95,
	HArch5      =  96,
	Floor6      =  97,
	Floor7      =  98,
	Floor8      =  99,
	Floor9      = 100,
	Floor10     = 101,
	VWall2      = 112,
	HWall2      = 113,
	Corner2     = 114,
	DWall2      = 115,
	DArch2      = 116,
	VWallEnd2   = 117,
	HWallEnd2   = 118,
	HArchEnd2   = 119,
	VArchEnd2   = 120,
	HArchVWall2 = 121,
	VArch2      = 122,
	HArch2      = 123,
	Floor2      = 124,
	HWallVArch2 = 125,
	Pillar3     = 126,
	Pillar4     = 127,
	Pillar5     = 128,
	VWall3      = 129,
	HWall3      = 130,
	Corner3     = 131,
	DWall3      = 132,
	DArch3      = 133,
	VWallEnd3   = 134,
	HWallEnd3   = 135,
	HArchEnd3   = 136,
	VArchEnd3   = 137,
	HArchVWall3 = 138,
	VArch3      = 139,
	HArch3      = 140,
	Floor3      = 141,
	HWallVArch3 = 142,
	Pillar6     = 143,
	Pillar7     = 144,
	Pillar8     = 145,
	VWall4      = 146,
	HWall4      = 147,
	Corner4     = 148,
	DWall4      = 149,
	DArch4      = 150,
	VWallEnd4   = 151,
	HWallEnd4   = 152,
	HArchEnd4   = 153,
	VArchEnd4   = 154,
	HArchVWall4 = 155,
	VArch4      = 156,
	HArch4      = 157,
	Floor4      = 158,
	HWallVArch4 = 159,
	Pillar9     = 160,
	Pillar10    = 161,
	Pillar11    = 162,
	Floor11     = 163,
	Floor12     = 164,
	Floor13     = 165,
	Floor14     = 166,
	VWall8      = 173,
	VWall9      = 174,
	VWall10     = 175,
	VWall11     = 176,
	VWall12     = 177,
	VWall13     = 178,
	HWall8      = 179,
	HWall9      = 180,
	HWall10     = 181,
	HWall11     = 182,
	HWall12     = 183,
	HWall13     = 184,
	VArch6      = 185,
	VArch7      = 186,
	HArch6      = 187,
	HArch7      = 188,
	Floor15     = 189,
	Floor16     = 190,
	Floor17     = 191,
	Pillar12    = 192,
	Floor18     = 193,
	Floor19     = 194,
	Floor20     = 195,
	Floor21     = 196,
	Floor22     = 197,
	Floor23     = 198,
	VDemon      = 199,
	HDemon      = 200,
	VSuccubus   = 201,
	HSuccubus   = 202,
	// clang-format on
};

void InitCryptPieces()
{
	for (int j = 0; j < MAXDUNY; j++) {
		for (int i = 0; i < MAXDUNX; i++) {
			if (dPiece[i][j] == 77) {
				dSpecial[i][j] = 1;
			} else if (dPiece[i][j] == 80) {
				dSpecial[i][j] = 2;
			}
		}
	}
}

void CryptLavafloor()
{
	for (int j = 1; j < 40; j++) {
		for (int i = 1; i < 40; i++) {
			switch (dungeon[i][j]) {
			case 5:
			case 116:
			case 133:
				if (dungeon[i - 1][j] == 13)
					dungeon[i - 1][j] = 203;
				if (dungeon[i - 1][j - 1] == 13)
					dungeon[i - 1][j - 1] = 204;
				if (dungeon[i][j - 1] == 13)
					dungeon[i][j - 1] = 205;
				break;
			case 7:
			case 15:
			case 17:
			case 118:
			case 126:
			case 128:
			case 135:
			case 152:
			case 160:
				if (dungeon[i - 1][j] == 13)
					dungeon[i - 1][j] = 206;
				if (dungeon[i - 1][j - 1] == 13)
					dungeon[i - 1][j - 1] = 207;
				break;
			case 8:
			case 11:
			case 14:
			case 95:
			case 119:
			case 125:
			case 136:
			case 142:
			case 153:
			case 156:
			case 159:
			case 185:
			case 186:
				if (dungeon[i - 1][j] == 13)
					dungeon[i - 1][j] = 203;
				if (dungeon[i - 1][j - 1] == 13)
					dungeon[i - 1][j - 1] = 204;
				break;
			case 9:
			case 120:
			case 154:
				if (dungeon[i - 1][j] == 13)
					dungeon[i - 1][j] = 206;
				if (dungeon[i - 1][j - 1] == 13)
					dungeon[i - 1][j - 1] = 207;
				if (dungeon[i][j - 1] == 13)
					dungeon[i][j - 1] = 205;
				break;
			case 10:
			case 12:
			case 121:
			case 123:
			case 138:
			case 155:
				if (dungeon[i][j - 1] == 13)
					dungeon[i][j - 1] = 205;
				break;
			case 96:
			case 187:
				if (dungeon[i][j - 1] == 13)
					dungeon[i][j - 1] = 208;
				break;
			case 122:
				if (dungeon[i - 1][j] == 13)
					dungeon[i - 1][j] = 211;
				if (dungeon[i - 1][j - 1] == 13)
					dungeon[i - 1][j - 1] = 212;
				break;
			case 137:
				if (dungeon[i - 1][j] == 13)
					dungeon[i - 1][j] = 213;
				if (dungeon[i - 1][j - 1] == 13)
					dungeon[i - 1][j - 1] = 214;
				if (dungeon[i][j - 1] == 13)
					dungeon[i][j - 1] = 205;
				break;
			case 139:
				if (dungeon[i - 1][j] == 13)
					dungeon[i - 1][j] = 215;
				if (dungeon[i - 1][j - 1] == 13)
					dungeon[i - 1][j - 1] = 216;
				break;
			case 140:
			case 157:
				if (dungeon[i][j - 1] == 13)
					dungeon[i][j - 1] = 217;
				break;
			case 143:
			case 145:
				if (dungeon[i - 1][j] == 13)
					dungeon[i - 1][j] = 213;
				if (dungeon[i - 1][j - 1] == 13)
					dungeon[i - 1][j - 1] = 214;
				break;
			case 150:
				if (dungeon[i - 1][j] == 13)
					dungeon[i - 1][j] = 203;
				if (dungeon[i - 1][j - 1] == 13)
					dungeon[i - 1][j - 1] = 204;
				if (dungeon[i][j - 1] == 13)
					dungeon[i][j - 1] = 217;
				break;
			case 162:
			case 167:
			case 192:
				if (dungeon[i - 1][j] == 13)
					dungeon[i - 1][j] = 209;
				if (dungeon[i - 1][j - 1] == 13)
					dungeon[i - 1][j - 1] = 210;
				break;
			}
		}
	}
}

void ApplyShadowsPatterns()
{
	uint8_t sd[2][2];

	for (int y = 1; y < DMAXY; y++) {
		for (int x = 1; x < DMAXX; x++) {
			sd[0][0] = BSTYPES[dungeon[x][y]];
			sd[1][0] = BSTYPES[dungeon[x - 1][y]];
			sd[0][1] = BSTYPES[dungeon[x][y - 1]];
			sd[1][1] = BSTYPES[dungeon[x - 1][y - 1]];

			for (const auto &shadow : SPATS) {
				if (shadow.strig != sd[0][0])
					continue;
				if (shadow.s1 != 0 && shadow.s1 != sd[1][1])
					continue;
				if (shadow.s2 != 0 && shadow.s2 != sd[0][1])
					continue;
				if (shadow.s3 != 0 && shadow.s3 != sd[1][0])
					continue;

				if (shadow.nv1 != 0 && !Protected[x - 1][y - 1]) {
					dungeon[x - 1][y - 1] = shadow.nv1;
				}
				if (shadow.nv2 != 0 && !Protected[x][y - 1]) {
					dungeon[x][y - 1] = shadow.nv2;
				}
				if (shadow.nv3 != 0 && !Protected[x - 1][y]) {
					dungeon[x - 1][y] = shadow.nv3;
				}
			}
		}
	}

	for (int y = 1; y < DMAXY; y++) {
		for (int x = 1; x < DMAXX; x++) {
			if (dungeon[x - 1][y] == 139 && !Protected[x - 1][y]) {
				uint8_t tnv3 = 139;
				if (IsAnyOf(dungeon[x][y], 29, 32, 35, 37, 38, 39)) {
					tnv3 = 141;
				}
				dungeon[x - 1][y] = tnv3;
			}
			if (dungeon[x - 1][y] == 149 && !Protected[x - 1][y]) {
				uint8_t tnv3 = 149;
				if (IsAnyOf(dungeon[x][y], 29, 32, 35, 37, 38, 39)) {
					tnv3 = 153;
				}
				dungeon[x - 1][y] = tnv3;
			}
			if (dungeon[x - 1][y] == 148 && !Protected[x - 1][y]) {
				uint8_t tnv3 = 148;
				if (IsAnyOf(dungeon[x][y], 29, 32, 35, 37, 38, 39)) {
					tnv3 = 154;
				}
				dungeon[x - 1][y] = tnv3;
			}
		}
	}
}

bool CanReplaceTile(uint8_t replace, Point tile)
{
	if (replace < 84 || replace > 100) {
		return true;
	}

	// BUGFIX: p2 is a workaround for a bug, only p1 should have been used (fixing this breaks compatability)
	constexpr auto ComparisonWithBoundsCheck = [](Point p1, Point p2) {
		return (p1.x >= 0 && p1.x < DMAXX && p1.y >= 0 && p1.y < DMAXY)
		    && (p2.x >= 0 && p2.x < DMAXX && p2.y >= 0 && p2.y < DMAXY)
		    && (dungeon[p1.x][p1.y] >= 84 && dungeon[p2.x][p2.y] <= 100);
	};
	if (ComparisonWithBoundsCheck(tile + Direction::NorthWest, tile + Direction::NorthWest)
	    || ComparisonWithBoundsCheck(tile + Direction::SouthEast, tile + Direction::NorthWest)
	    || ComparisonWithBoundsCheck(tile + Direction::SouthWest, tile + Direction::NorthWest)
	    || ComparisonWithBoundsCheck(tile + Direction::NorthEast, tile + Direction::NorthWest)) {
		return false;
	}

	return true;
}

void PlaceMiniSetRandom(const Miniset &miniset, int rndper)
{
	int sw = miniset.size.width;
	int sh = miniset.size.height;

	for (int sy = 0; sy < DMAXY - sh; sy++) {
		for (int sx = 0; sx < DMAXX - sw; sx++) {
			if (!miniset.matches({ sx, sy }, false))
				continue;
			if (!CanReplaceTile(miniset.replace[0][0], { sx, sy }))
				continue;
			if (GenerateRnd(100) >= rndper)
				continue;
			miniset.place({ sx, sy });
		}
	}
}

void PlaceMiniSetRandom1x1(uint8_t search, uint8_t replace, int rndper)
{
	PlaceMiniSetRandom({ { 1, 1 }, { search }, { replace } }, rndper);
}

void FillFloor()
{
	for (int j = 0; j < DMAXY; j++) {
		for (int i = 0; i < DMAXX; i++) {
			if (!Protected[i][j] && dungeon[i][j] == Tile::Floor) {
				int rv = GenerateRnd(3);

				if (rv == 1)
					dungeon[i][j] = 162;
				if (rv == 2)
					dungeon[i][j] = 163;
			}
		}
	}
}

void LoadQuestSetPieces()
{
	L5setloadflag = false;

	if (Quests[Q_BUTCHER].IsAvailable()) {
		L5pSetPiece = LoadFileInMem<uint16_t>("Levels\\L1Data\\rnd6.DUN");
		L5setloadflag = true;
	} else if (Quests[Q_SKELKING].IsAvailable() && !gbIsMultiplayer) {
		L5pSetPiece = LoadFileInMem<uint16_t>("Levels\\L1Data\\SKngDO.DUN");
		L5setloadflag = true;
	} else if (Quests[Q_LTBANNER].IsAvailable()) {
		L5pSetPiece = LoadFileInMem<uint16_t>("Levels\\L1Data\\Banner2.DUN");
		L5setloadflag = true;
	}
}

void FreeQuestSetPieces()
{
	L5pSetPiece = nullptr;
}

void InitDungeonPieces()
{
	for (int j = 0; j < MAXDUNY; j++) {
		for (int i = 0; i < MAXDUNX; i++) {
			int8_t pc;
			if (IsAnyOf(dPiece[i][j], 12, 71, 321, 211, 341, 418)) {
				pc = 1;
			} else if (IsAnyOf(dPiece[i][j], 11, 249, 325, 344, 331, 421)) {
				pc = 2;
			} else if (dPiece[i][j] == 253) {
				pc = 3;
			} else if (dPiece[i][j] == 255) {
				pc = 4;
			} else if (dPiece[i][j] == 259) {
				pc = 5;
			} else if (dPiece[i][j] == 267) {
				pc = 6;
			} else {
				continue;
			}
			dSpecial[i][j] = pc;
		}
	}
}

void InitDungeonFlags()
{
	for (int j = 0; j < DMAXY; j++) {
		for (int i = 0; i < DMAXX; i++) {
			dungeon[i][j] = 0;
			Protected[i][j] = false;
			Chamber[i][j] = false;
		}
	}
}

void MapRoom(int x, int y, int width, int height)
{
	for (int j = 0; j < height; j++) {
		for (int i = 0; i < width; i++) {
			dungeon[x + i][y + j] = Tile::VWall;
		}
	}
}

bool CheckRoom(int x, int y, int width, int height)
{
	for (int j = 0; j < height; j++) {
		for (int i = 0; i < width; i++) {
			if (i + x < 0 || i + x >= DMAXX || j + y < 0 || j + y >= DMAXY) {
				return false;
			}
			if (dungeon[i + x][j + y] != 0) {
				return false;
			}
		}
	}

	return true;
}

void GenerateRoom(int x, int y, int w, int h, int dir)
{
	int dirProb = GenerateRnd(4);
	int num = 0;

	bool ran;
	if ((dir == 1 && dirProb == 0) || (dir != 1 && dirProb != 0)) {
		int cw;
		int ch;
		int cx1;
		int cy1;
		do {
			cw = (GenerateRnd(5) + 2) & ~1;
			ch = (GenerateRnd(5) + 2) & ~1;
			cx1 = x - cw;
			cy1 = h / 2 + y - ch / 2;
			ran = CheckRoom(cx1 - 1, cy1 - 1, ch + 2, cw + 1); /// BUGFIX: swap args 3 and 4 ("ch+2" and "cw+1") (workaround applied below)
			num++;
		} while (!ran && num < 20);

		if (ran)
			MapRoom(cx1, cy1, std::min(DMAXX - cx1, cw), std::min(DMAXX - cy1, ch));
		int cx2 = x + w;
		bool ran2 = CheckRoom(cx2, cy1 - 1, cw + 1, ch + 2);
		if (ran2)
			MapRoom(cx2, cy1, cw, ch);
		if (ran)
			GenerateRoom(cx1, cy1, cw, ch, 1);
		if (ran2)
			GenerateRoom(cx2, cy1, cw, ch, 1);
		return;
	}

	int width;
	int height;
	int rx;
	int ry;
	do {
		width = (GenerateRnd(5) + 2) & ~1;
		height = (GenerateRnd(5) + 2) & ~1;
		rx = w / 2 + x - width / 2;
		ry = y - height;
		ran = CheckRoom(rx - 1, ry - 1, width + 2, height + 1);
		num++;
	} while (!ran && num < 20);

	if (ran)
		MapRoom(rx, ry, width, height);
	int ry2 = y + h;
	bool ran2 = CheckRoom(rx - 1, ry2, width + 2, height + 1);
	if (ran2)
		MapRoom(rx, ry2, width, height);
	if (ran)
		GenerateRoom(rx, ry, width, height, 0);
	if (ran2)
		GenerateRoom(rx, ry2, width, height, 0);
}

void FirstRoom()
{
	if (GenerateRnd(2) == 0) {
		int ys = 1;
		int ye = DMAXY - 1;

		VR1 = (GenerateRnd(2) != 0);
		VR2 = (GenerateRnd(2) != 0);
		VR3 = (GenerateRnd(2) != 0);

		if (!VR1 || !VR3)
			VR2 = true;
		if (VR1)
			MapRoom(15, 1, 10, 10);
		else
			ys = 18;

		if (VR2)
			MapRoom(15, 15, 10, 10);
		if (VR3)
			MapRoom(15, 29, 10, 10);
		else
			ye = 22;

		for (int y = ys; y < ye; y++) {
			dungeon[17][y] = Tile::VWall;
			dungeon[18][y] = Tile::VWall;
			dungeon[19][y] = Tile::VWall;
			dungeon[20][y] = Tile::VWall;
			dungeon[21][y] = Tile::VWall;
			dungeon[22][y] = Tile::VWall;
		}

		if (VR1)
			GenerateRoom(15, 1, 10, 10, 0);
		if (VR2)
			GenerateRoom(15, 15, 10, 10, 0);
		if (VR3)
			GenerateRoom(15, 29, 10, 10, 0);

		HR3 = false;
		HR2 = false;
		HR1 = false;
	} else {
		int xs = 1;
		int xe = DMAXX - 1;

		HR1 = GenerateRnd(2) != 0;
		HR2 = GenerateRnd(2) != 0;
		HR3 = GenerateRnd(2) != 0;

		if (!HR1 || !HR3)
			HR2 = true;
		if (HR1)
			MapRoom(1, 15, 10, 10);
		else
			xs = 18;

		if (HR2)
			MapRoom(15, 15, 10, 10);
		if (HR3)
			MapRoom(29, 15, 10, 10);
		else
			xe = 22;

		for (int x = xs; x < xe; x++) {
			dungeon[x][17] = Tile::VWall;
			dungeon[x][18] = Tile::VWall;
			dungeon[x][19] = Tile::VWall;
			dungeon[x][20] = Tile::VWall;
			dungeon[x][21] = Tile::VWall;
			dungeon[x][22] = Tile::VWall;
		}

		if (HR1)
			GenerateRoom(1, 15, 10, 10, 1);
		if (HR2)
			GenerateRoom(15, 15, 10, 10, 1);
		if (HR3)
			GenerateRoom(29, 15, 10, 10, 1);

		VR3 = false;
		VR2 = false;
		VR1 = false;
	}
}

int FindArea()
{
	int rv = 0;

	for (int j = 0; j < DMAXY; j++) {
		for (int i = 0; i < DMAXX; i++) { // NOLINT(modernize-loop-convert)
			if (dungeon[i][j] == Tile::VWall)
				rv++;
		}
	}

	return rv;
}

void MakeDungeon()
{
	for (int j = 0; j < DMAXY; j++) {
		for (int i = 0; i < DMAXX; i++) {
			int i2 = i * 2;
			int j2 = j * 2;
			L5dungeon[i2][j2] = dungeon[i][j];
			L5dungeon[i2][j2 + 1] = dungeon[i][j];
			L5dungeon[i2 + 1][j2] = dungeon[i][j];
			L5dungeon[i2 + 1][j2 + 1] = dungeon[i][j];
		}
	}
}

void MakeDmt()
{
	for (int j = 0; j < DMAXY; j++) {
		for (int i = 0; i < DMAXX; i++) { // NOLINT(modernize-loop-convert)
			dungeon[i][j] = 22;
		}
	}

	int dmty = 1;
	for (int j = 0; dmty <= 77; j++, dmty += 2) {
		int dmtx = 1;
		for (int i = 0; dmtx <= 77; i++, dmtx += 2) {
			int val = 8 * L5dungeon[dmtx + 1][dmty + 1]
			    + 4 * L5dungeon[dmtx][dmty + 1]
			    + 2 * L5dungeon[dmtx + 1][dmty]
			    + L5dungeon[dmtx][dmty];
			dungeon[i][j] = L5ConvTbl[val];
		}
	}
}

int HorizontalWallOk(int i, int j)
{
	int x;
	for (x = 1; dungeon[i + x][j] == 13; x++) {
		if (dungeon[i + x][j - 1] != 13 || dungeon[i + x][j + 1] != 13 || Protected[i + x][j] || Chamber[i + x][j])
			break;
	}

	bool wallok = false;
	if (dungeon[i + x][j] >= 3 && dungeon[i + x][j] <= 7)
		wallok = true;
	if (dungeon[i + x][j] >= 16 && dungeon[i + x][j] <= 24)
		wallok = true;
	if (dungeon[i + x][j] == 22)
		wallok = false;
	if (x == 1)
		wallok = false;

	if (wallok)
		return x;

	return -1;
}

int VerticalWallOk(int i, int j)
{
	int y;
	for (y = 1; dungeon[i][j + y] == 13; y++) {
		if (dungeon[i - 1][j + y] != 13 || dungeon[i + 1][j + y] != 13 || Protected[i][j + y] || Chamber[i][j + y])
			break;
	}

	bool wallok = false;
	if (dungeon[i][j + y] >= 3 && dungeon[i][j + y] <= 7)
		wallok = true;
	if (dungeon[i][j + y] >= 16 && dungeon[i][j + y] <= 24)
		wallok = true;
	if (dungeon[i][j + y] == 22)
		wallok = false;
	if (y == 1)
		wallok = false;

	if (wallok)
		return y;

	return -1;
}

void HorizontalWall(int i, int j, Tile p, int dx)
{
	Tile dt = Tile::HWall;
	Tile wt = Tile::HDoor;

	switch (GenerateRnd(4)) {
	case 2: // Add arch
		dt = Tile::HArch;
		wt = Tile::HArch;
		if (p == Tile::HWall)
			p = Tile::HArch;
		else if (p == Tile::DWall)
			p = Tile::HArchVWall;
		break;
	case 3: // Add Fence
		dt = Tile::HFence;
		if (p == Tile::HWall)
			p = Tile::HFence;
		else if (p == Tile::DWall)
			p = Tile::HFenceVWall;
		break;
	default:
		break;
	}

	if (GenerateRnd(6) == 5)
		wt = Tile::HArch;

	dungeon[i][j] = p;

	for (int xx = 1; xx < dx; xx++) {
		dungeon[i + xx][j] = dt;
	}

	int xx = GenerateRnd(dx - 1) + 1;

	dungeon[i + xx][j] = wt;
	if (wt == Tile::HDoor) {
		Protected[i + xx][j] = true;
	}
}

void VerticalWall(int i, int j, Tile p, int dy)
{
	Tile dt = Tile::VWall;
	Tile wt = Tile::VDoor;

	switch (GenerateRnd(4)) {
	case 2: // Add arch
		dt = Tile::VArch;
		wt = Tile::VArch;
		if (p == Tile::VWall)
			p = Tile::VArch;
		else if (p == Tile::DWall)
			p = Tile::HWallVArch;
		break;
	case 3: // Add Fence
		dt = Tile::VFence;
		if (p == Tile::VWall)
			p = Tile::VFence;
		else if (p == Tile::DWall)
			p = Tile::HWallVFence;
		break;
	default:
		break;
	}

	if (GenerateRnd(6) == 5)
		wt = Tile::VArch;

	dungeon[i][j] = p;

	for (int yy = 1; yy < dy; yy++) {
		dungeon[i][j + yy] = dt;
	}

	int yy = GenerateRnd(dy - 1) + 1;

	dungeon[i][j + yy] = wt;
	if (wt == Tile::VDoor) {
		Protected[i][j + yy] = true;
	}
}

void AddWall()
{
	for (int j = 0; j < DMAXY; j++) {
		for (int i = 0; i < DMAXX; i++) {
			if (!Protected[i][j] && !Chamber[i][j]) {
				if (dungeon[i][j] == Tile::Corner) {
					AdvanceRndSeed();
					int x = HorizontalWallOk(i, j);
					if (x != -1) {
						HorizontalWall(i, j, Tile::HWall, x);
					}
				}
				if (dungeon[i][j] == Tile::Corner) {
					AdvanceRndSeed();
					int y = VerticalWallOk(i, j);
					if (y != -1) {
						VerticalWall(i, j, Tile::VWall, y);
					}
				}
				if (dungeon[i][j] == Tile::VWallEnd) {
					AdvanceRndSeed();
					int x = HorizontalWallOk(i, j);
					if (x != -1) {
						HorizontalWall(i, j, Tile::DWall, x);
					}
				}
				if (dungeon[i][j] == Tile::HWallEnd) {
					AdvanceRndSeed();
					int y = VerticalWallOk(i, j);
					if (y != -1) {
						VerticalWall(i, j, Tile::DWall, y);
					}
				}
				if (dungeon[i][j] == Tile::HWall) {
					AdvanceRndSeed();
					int x = HorizontalWallOk(i, j);
					if (x != -1) {
						HorizontalWall(i, j, Tile::HWall, x);
					}
				}
				if (dungeon[i][j] == Tile::VWall) {
					AdvanceRndSeed();
					int y = VerticalWallOk(i, j);
					if (y != -1) {
						VerticalWall(i, j, Tile::VWall, y);
					}
				}
			}
		}
	}
}

void GenerateChamber(int sx, int sy, bool topflag, bool bottomflag, bool leftflag, bool rightflag)
{
	if (topflag) {
		dungeon[sx + 2][sy] = Tile::HArch;
		dungeon[sx + 3][sy] = Tile::HArch;
		dungeon[sx + 4][sy] = Tile::Corner;
		dungeon[sx + 7][sy] = Tile::VArchEnd;
		dungeon[sx + 8][sy] = Tile::HArch;
		dungeon[sx + 9][sy] = Tile::HWall;
	}
	if (bottomflag) {
		sy += 11;
		dungeon[sx + 2][sy] = Tile::HArchVWall;
		dungeon[sx + 3][sy] = Tile::HArch;
		dungeon[sx + 4][sy] = Tile::HArchEnd;
		dungeon[sx + 7][sy] = Tile::DArch;
		dungeon[sx + 8][sy] = Tile::HArch;
		if (dungeon[sx + 9][sy] != Tile::DWall) {
			dungeon[sx + 9][sy] = Tile::DirtCorner;
		}
		sy -= 11;
	}
	if (leftflag) {
		dungeon[sx][sy + 2] = Tile::VArch;
		dungeon[sx][sy + 3] = Tile::VArch;
		dungeon[sx][sy + 4] = Tile::Corner;
		dungeon[sx][sy + 7] = Tile::HArchEnd;
		dungeon[sx][sy + 8] = Tile::VArch;
		dungeon[sx][sy + 9] = Tile::VWall;
	}
	if (rightflag) {
		sx += 11;
		dungeon[sx][sy + 2] = Tile::HWallVArch;
		dungeon[sx][sy + 3] = Tile::VArch;
		dungeon[sx][sy + 4] = Tile::VArchEnd;
		dungeon[sx][sy + 7] = Tile::DArch;
		dungeon[sx][sy + 8] = Tile::VArch;
		if (dungeon[sx][sy + 9] != Tile::DWall) {
			dungeon[sx][sy + 9] = Tile::DirtCorner;
		}
		sx -= 11;
	}

	for (int j = 1; j < 11; j++) {
		for (int i = 1; i < 11; i++) {
			dungeon[i + sx][j + sy] = Tile::Floor;
			Chamber[i + sx][j + sy] = true;
		}
	}

	dungeon[sx + 4][sy + 4] = Tile::Pillar;
	dungeon[sx + 7][sy + 4] = Tile::Pillar;
	dungeon[sx + 4][sy + 7] = Tile::Pillar;
	dungeon[sx + 7][sy + 7] = Tile::Pillar;
}

void GenerateHall(int x1, int y1, int x2, int y2)
{
	if (y1 == y2) {
		for (int i = x1; i < x2; i++) {
			dungeon[i][y1] = Tile::HArch;
			dungeon[i][y1 + 3] = Tile::HArch;
		}
		return;
	}

	for (int i = y1; i < y2; i++) {
		dungeon[x1][i] = Tile::VArch;
		dungeon[x1 + 3][i] = Tile::VArch;
	}
}

void FixTilesPatterns()
{
	// BUGFIX: Bounds checks are required in all loop bodies.
	// See https://github.com/diasurgical/devilutionX/pull/401

	for (int j = 0; j < DMAXY; j++) {
		for (int i = 0; i < DMAXX; i++) {
			if (i + 1 < DMAXX) {
				if (dungeon[i][j] == 2 && dungeon[i + 1][j] == 22)
					dungeon[i + 1][j] = 23;
				if (dungeon[i][j] == 13 && dungeon[i + 1][j] == 22)
					dungeon[i + 1][j] = 18;
				if (dungeon[i][j] == 13 && dungeon[i + 1][j] == 2)
					dungeon[i + 1][j] = 7;
				if (dungeon[i][j] == 6 && dungeon[i + 1][j] == 22)
					dungeon[i + 1][j] = 24;
			}
			if (j + 1 < DMAXY) {
				if (dungeon[i][j] == 1 && dungeon[i][j + 1] == 22)
					dungeon[i][j + 1] = 24;
				if (dungeon[i][j] == 13 && dungeon[i][j + 1] == 1)
					dungeon[i][j + 1] = 6;
				if (dungeon[i][j] == 13 && dungeon[i][j + 1] == 22)
					dungeon[i][j + 1] = 19;
			}
		}
	}

	for (int j = 0; j < DMAXY; j++) {
		for (int i = 0; i < DMAXX; i++) {
			if (i + 1 < DMAXX) {
				if (dungeon[i][j] == 13 && dungeon[i + 1][j] == 19)
					dungeon[i + 1][j] = 21;
				if (dungeon[i][j] == 13 && dungeon[i + 1][j] == 22)
					dungeon[i + 1][j] = 20;
				if (dungeon[i][j] == 7 && dungeon[i + 1][j] == 22)
					dungeon[i + 1][j] = 23;
				if (dungeon[i][j] == 13 && dungeon[i + 1][j] == 24)
					dungeon[i + 1][j] = 21;
				if (dungeon[i][j] == 19 && dungeon[i + 1][j] == 22)
					dungeon[i + 1][j] = 20;
				if (dungeon[i][j] == 2 && dungeon[i + 1][j] == 19)
					dungeon[i + 1][j] = 21;
				if (dungeon[i][j] == 19 && dungeon[i + 1][j] == 1)
					dungeon[i + 1][j] = 6;
				if (dungeon[i][j] == 7 && dungeon[i + 1][j] == 19)
					dungeon[i + 1][j] = 21;
				if (dungeon[i][j] == 2 && dungeon[i + 1][j] == 1)
					dungeon[i + 1][j] = 6;
				if (dungeon[i][j] == 3 && dungeon[i + 1][j] == 22)
					dungeon[i + 1][j] = 24;
				if (dungeon[i][j] == 21 && dungeon[i + 1][j] == 1)
					dungeon[i + 1][j] = 6;
				if (dungeon[i][j] == 7 && dungeon[i + 1][j] == 1)
					dungeon[i + 1][j] = 6;
				if (dungeon[i][j] == 7 && dungeon[i + 1][j] == 24)
					dungeon[i + 1][j] = 21;
				if (dungeon[i][j] == 4 && dungeon[i + 1][j] == 16)
					dungeon[i + 1][j] = 17;
				if (dungeon[i][j] == 7 && dungeon[i + 1][j] == 13)
					dungeon[i + 1][j] = 17;
				if (dungeon[i][j] == 2 && dungeon[i + 1][j] == 24)
					dungeon[i + 1][j] = 21;
				if (dungeon[i][j] == 2 && dungeon[i + 1][j] == 13)
					dungeon[i + 1][j] = 17;
			}
			if (i > 0) {
				if (dungeon[i][j] == 23 && dungeon[i - 1][j] == 22)
					dungeon[i - 1][j] = 19;
				if (dungeon[i][j] == 19 && dungeon[i - 1][j] == 23)
					dungeon[i - 1][j] = 21;
				if (dungeon[i][j] == 6 && dungeon[i - 1][j] == 22)
					dungeon[i - 1][j] = 24;
				if (dungeon[i][j] == 6 && dungeon[i - 1][j] == 23)
					dungeon[i - 1][j] = 21;
			}
			if (j + 1 < DMAXY) {
				if (dungeon[i][j] == 1 && dungeon[i][j + 1] == 2)
					dungeon[i][j + 1] = 7;
				if (dungeon[i][j] == 6 && dungeon[i][j + 1] == 18)
					dungeon[i][j + 1] = 21;
				if (dungeon[i][j] == 18 && dungeon[i][j + 1] == 2)
					dungeon[i][j + 1] = 7;
				if (dungeon[i][j] == 6 && dungeon[i][j + 1] == 2)
					dungeon[i][j + 1] = 7;
				if (dungeon[i][j] == 21 && dungeon[i][j + 1] == 2)
					dungeon[i][j + 1] = 7;
				if (dungeon[i][j] == 6 && dungeon[i][j + 1] == 22)
					dungeon[i][j + 1] = 24;
				if (dungeon[i][j] == 6 && dungeon[i][j + 1] == 13)
					dungeon[i][j + 1] = 16;
				if (dungeon[i][j] == 1 && dungeon[i][j + 1] == 13)
					dungeon[i][j + 1] = 16;
				if (dungeon[i][j] == 13 && dungeon[i][j + 1] == 16)
					dungeon[i][j + 1] = 17;
			}
			if (j > 0) {
				if (dungeon[i][j] == 6 && dungeon[i][j - 1] == 22)
					dungeon[i][j - 1] = 7;
				if (dungeon[i][j] == 6 && dungeon[i][j - 1] == 22)
					dungeon[i][j - 1] = 24;
				if (dungeon[i][j] == 7 && dungeon[i][j - 1] == 24)
					dungeon[i][j - 1] = 21;
				if (dungeon[i][j] == 18 && dungeon[i][j - 1] == 24)
					dungeon[i][j - 1] = 21;
			}
		}
	}

	for (int j = 0; j < DMAXY; j++) {
		for (int i = 0; i < DMAXX; i++) {
			if (j + 1 < DMAXY && dungeon[i][j] == 4 && dungeon[i][j + 1] == 2)
				dungeon[i][j + 1] = 7;
			if (i + 1 < DMAXX && dungeon[i][j] == 2 && dungeon[i + 1][j] == 19)
				dungeon[i + 1][j] = 21;
			if (j + 1 < DMAXY && dungeon[i][j] == 18 && dungeon[i][j + 1] == 22)
				dungeon[i][j + 1] = 20;
		}
	}
}

void SetCornerRoom(int rx1, int ry1)
{
	SetPiece = { { rx1, ry1 }, CornerstoneRoomPattern.size };

	CornerstoneRoomPattern.place({ rx1, ry1 }, true);
}

void Substitution()
{
	for (int y = 0; y < DMAXY; y++) {
		for (int x = 0; x < DMAXX; x++) {
			if (GenerateRnd(4) == 0) {
				uint8_t c = L5BTYPES[dungeon[x][y]];
				if (c != 0 && !Protected[x][y]) {
					int rv = GenerateRnd(16);
					int i = -1;
					while (rv >= 0) {
						i++;
						if (i == sizeof(L5BTYPES)) {
							i = 0;
						}
						if (c == L5BTYPES[i]) {
							rv--;
						}
					}

					// BUGFIX: Add `&& y > 0` to the if statement. (fixed)
					if (i == 89 && y > 0) {
						if (L5BTYPES[dungeon[x][y - 1]] != 79 || Protected[x][y - 1])
							i = 79;
						else
							dungeon[x][y - 1] = 90;
					}
					// BUGFIX: Add `&& x + 1 < DMAXX` to the if statement. (fixed)
					if (i == 91 && x + 1 < DMAXX) {
						if (L5BTYPES[dungeon[x + 1][y]] != 80 || Protected[x + 1][y])
							i = 80;
						else
							dungeon[x + 1][y] = 92;
					}
					dungeon[x][y] = i;
				}
			}
		}
	}
}

void SetCryptRoom(int rx1, int ry1)
{
	UberRow = 2 * rx1 + 6;
	UberCol = 2 * ry1 + 8;
	SetPiece = { { rx1, ry1 }, UberRoomPattern.size };
	IsUberRoomOpened = false;
	IsUberLeverActivated = false;

	UberRoomPattern.place({ rx1, ry1 }, true);
}

void FillChambers()
{
	if (HR1)
		GenerateChamber(0, 14, false, false, false, true);

	if (HR2) {
		if (HR1 && !HR3)
			GenerateChamber(14, 14, false, false, true, false);
		if (!HR1 && HR3)
			GenerateChamber(14, 14, false, false, false, true);
		if (HR1 && HR3)
			GenerateChamber(14, 14, false, false, true, true);
		if (!HR1 && !HR3)
			GenerateChamber(14, 14, false, false, false, false);
	}

	if (HR3)
		GenerateChamber(28, 14, false, false, true, false);
	if (HR1 && HR2)
		GenerateHall(12, 18, 14, 18);
	if (HR2 && HR3)
		GenerateHall(26, 18, 28, 18);
	if (HR1 && !HR2 && HR3)
		GenerateHall(12, 18, 28, 18);
	if (VR1)
		GenerateChamber(14, 0, false, true, false, false);

	if (VR2) {
		if (VR1 && !VR3)
			GenerateChamber(14, 14, true, false, false, false);
		if (!VR1 && VR3)
			GenerateChamber(14, 14, false, true, false, false);
		if (VR1 && VR3)
			GenerateChamber(14, 14, true, true, false, false);
		if (!VR1 && !VR3)
			GenerateChamber(14, 14, false, false, false, false);
	}

	if (VR3)
		GenerateChamber(14, 28, true, false, false, false);
	if (VR1 && VR2)
		GenerateHall(18, 12, 18, 14);
	if (VR2 && VR3)
		GenerateHall(18, 26, 18, 28);
	if (VR1 && !VR2 && VR3)
		GenerateHall(18, 12, 18, 28);

	if (currlevel == 24) {
		if (VR1 || VR2 || VR3) {
			int c = 1;
			if (!VR1 && VR2 && VR3 && GenerateRnd(2) != 0)
				c = 2;
			if (VR1 && VR2 && !VR3 && GenerateRnd(2) != 0)
				c = 0;

			if (VR1 && !VR2 && VR3) {
				c = (GenerateRnd(2) != 0) ? 0 : 2;
			}

			if (VR1 && VR2 && VR3)
				c = GenerateRnd(3);

			switch (c) {
			case 0:
				SetCryptRoom(16, 2);
				break;
			case 1:
				SetCryptRoom(16, 16);
				break;
			case 2:
				SetCryptRoom(16, 30);
				break;
			}
		} else {
			int c = 1;
			if (!HR1 && HR2 && HR3 && GenerateRnd(2) != 0)
				c = 2;
			if (HR1 && HR2 && !HR3 && GenerateRnd(2) != 0)
				c = 0;

			if (HR1 && !HR2 && HR3) {
				c = (GenerateRnd(2) != 0) ? 0 : 2;
			}

			if (HR1 && HR2 && HR3)
				c = GenerateRnd(3);

			switch (c) {
			case 0:
				SetCryptRoom(2, 16);
				break;
			case 1:
				SetCryptRoom(16, 16);
				break;
			case 2:
				SetCryptRoom(30, 16);
				break;
			}
		}
	}
	if (currlevel == 21) {
		if (VR1 || VR2 || VR3) {
			int c = 1;
			if (!VR1 && VR2 && VR3 && GenerateRnd(2) != 0)
				c = 2;
			if (VR1 && VR2 && !VR3 && GenerateRnd(2) != 0)
				c = 0;

			if (VR1 && !VR2 && VR3) {
				if (GenerateRnd(2) != 0)
					c = 0;
				else
					c = 2;
			}

			if (VR1 && VR2 && VR3)
				c = GenerateRnd(3);

			switch (c) {
			case 0:
				SetCornerRoom(16, 2);
				break;
			case 1:
				SetCornerRoom(16, 16);
				break;
			case 2:
				SetCornerRoom(16, 30);
				break;
			}
		} else {
			int c = 1;
			if (!HR1 && HR2 && HR3 && GenerateRnd(2) != 0)
				c = 2;
			if (HR1 && HR2 && !HR3 && GenerateRnd(2) != 0)
				c = 0;

			if (HR1 && !HR2 && HR3) {
				if (GenerateRnd(2) != 0)
					c = 0;
				else
					c = 2;
			}

			if (HR1 && HR2 && HR3)
				c = GenerateRnd(3);

			switch (c) {
			case 0:
				SetCornerRoom(2, 16);
				break;
			case 1:
				SetCornerRoom(16, 16);
				break;
			case 2:
				SetCornerRoom(30, 16);
				break;
			}
		}
	}
	if (L5setloadflag) {
		Point position;
		if (VR1 || VR2 || VR3) {
			int c = 1;
			if (!VR1 && VR2 && VR3 && GenerateRnd(2) != 0)
				c = 2;
			if (VR1 && VR2 && !VR3 && GenerateRnd(2) != 0)
				c = 0;

			if (VR1 && !VR2 && VR3) {
				if (GenerateRnd(2) != 0)
					c = 0;
				else
					c = 2;
			}

			if (VR1 && VR2 && VR3)
				c = GenerateRnd(3);

			switch (c) {
			case 0:
				position = { 16, 2 };
				break;
			case 1:
				position = { 16, 16 };
				break;
			case 2:
				position = { 16, 30 };
				break;
			}
		} else {
			int c = 1;
			if (!HR1 && HR2 && HR3 && GenerateRnd(2) != 0)
				c = 2;
			if (HR1 && HR2 && !HR3 && GenerateRnd(2) != 0)
				c = 0;

			if (HR1 && !HR2 && HR3) {
				if (GenerateRnd(2) != 0)
					c = 0;
				else
					c = 2;
			}

			if (HR1 && HR2 && HR3)
				c = GenerateRnd(3);

			switch (c) {
			case 0:
				position = { 2, 16 };
				break;
			case 1:
				position = { 16, 16 };
				break;
			case 2:
				position = { 30, 16 };
				break;
			}
		}
		PlaceDunTiles(L5pSetPiece.get(), position, Tile::Floor);
		SetPiece = { position, { SDL_SwapLE16(L5pSetPiece[0]), SDL_SwapLE16(L5pSetPiece[1]) } };
	}
}

void FixTransparency()
{
	int yy = 16;
	for (int j = 0; j < DMAXY; j++) {
		int xx = 16;
		for (int i = 0; i < DMAXX; i++) {
			// BUGFIX: Should check for `j > 0` first. (fixed)
			if (dungeon[i][j] == 23 && j > 0 && dungeon[i][j - 1] == 18) {
				dTransVal[xx + 1][yy] = dTransVal[xx][yy];
				dTransVal[xx + 1][yy + 1] = dTransVal[xx][yy];
			}
			// BUGFIX: Should check for `i + 1 < DMAXY` first. (fixed)
			if (dungeon[i][j] == 24 && i + 1 < DMAXY && dungeon[i + 1][j] == 19) {
				dTransVal[xx][yy + 1] = dTransVal[xx][yy];
				dTransVal[xx + 1][yy + 1] = dTransVal[xx][yy];
			}
			if (dungeon[i][j] == 18) {
				dTransVal[xx + 1][yy] = dTransVal[xx][yy];
				dTransVal[xx + 1][yy + 1] = dTransVal[xx][yy];
			}
			if (dungeon[i][j] == 19) {
				dTransVal[xx][yy + 1] = dTransVal[xx][yy];
				dTransVal[xx + 1][yy + 1] = dTransVal[xx][yy];
			}
			if (dungeon[i][j] == 20) {
				dTransVal[xx + 1][yy] = dTransVal[xx][yy];
				dTransVal[xx][yy + 1] = dTransVal[xx][yy];
				dTransVal[xx + 1][yy + 1] = dTransVal[xx][yy];
			}
			xx += 2;
		}
		yy += 2;
	}
}

void FixDirtTiles()
{
	for (int j = 0; j < DMAXY - 1; j++) {
		for (int i = 0; i < DMAXX - 1; i++) {
			if (dungeon[i][j] == 21 && dungeon[i + 1][j] != 19) {
				dungeon[i][j] = 202;
			}
			if (dungeon[i][j] == 19 && dungeon[i + 1][j] != 19) {
				dungeon[i][j] = 200;
			}
			if (dungeon[i][j] == 24 && dungeon[i + 1][j] != 19) {
				dungeon[i][j] = 205;
			}
			if (dungeon[i][j] == 18 && dungeon[i][j + 1] != 18) {
				dungeon[i][j] = 199;
			}
			if (dungeon[i][j] == 21 && dungeon[i][j + 1] != 18) {
				dungeon[i][j] = 202;
			}
			if (dungeon[i][j] == 23 && dungeon[i][j + 1] != 18) {
				dungeon[i][j] = 204;
			}
		}
	}
}

void FixCryptDirtTiles()
{
	for (int j = 0; j < DMAXY - 1; j++) {
		for (int i = 0; i < DMAXX - 1; i++) {
			if (dungeon[i][j] == 19)
				dungeon[i][j] = 83;
			if (dungeon[i][j] == 21)
				dungeon[i][j] = 85;
			if (dungeon[i][j] == 23)
				dungeon[i][j] = 87;
			if (dungeon[i][j] == 24)
				dungeon[i][j] = 88;
			if (dungeon[i][j] == 18)
				dungeon[i][j] = 82;
		}
	}
}

void FixCornerTiles()
{
	for (int j = 1; j < DMAXY - 1; j++) {
		for (int i = 1; i < DMAXX - 1; i++) {
			if (!Protected[i][j] && dungeon[i][j] == 17 && dungeon[i - 1][j] == Tile::Floor && dungeon[i][j - 1] == Tile::VWall) {
				dungeon[i][j] = 16;
				// BUGFIX: Set tile as Protected
			}
			if (dungeon[i][j] == 202 && dungeon[i + 1][j] == Tile::Floor && dungeon[i][j + 1] == Tile::VWall) {
				dungeon[i][j] = 8;
			}
		}
	}
}

void CryptStatues(int rndper)
{
	PlaceMiniSetRandom1x1(Tile::VWall, CryptTile::VDemon, rndper);
	PlaceMiniSetRandom1x1(Tile::VWall, CryptTile::VSuccubus, rndper);
	PlaceMiniSetRandom1x1(Tile::HWall, CryptTile::HDemon, rndper);
	PlaceMiniSetRandom1x1(Tile::HWall, CryptTile::HSuccubus, rndper);
}

void CryptCracked(int rndper)
{
	// clang-format off
	PlaceMiniSetRandom1x1(Tile::VWall,      CryptTile::VWall2,      rndper);
	PlaceMiniSetRandom1x1(Tile::HWall,      CryptTile::HWall2,      rndper);
	PlaceMiniSetRandom1x1(Tile::Corner,     CryptTile::Corner2,     rndper);
	PlaceMiniSetRandom1x1(Tile::DWall,      CryptTile::DWall2,      rndper);
	PlaceMiniSetRandom1x1(Tile::DArch,      CryptTile::DArch2,      rndper);
	PlaceMiniSetRandom1x1(Tile::VWallEnd,   CryptTile::VWallEnd2,   rndper);
	PlaceMiniSetRandom1x1(Tile::HWallEnd,   CryptTile::HWallEnd2,   rndper);
	PlaceMiniSetRandom1x1(Tile::HArchEnd,   CryptTile::HArchEnd2,   rndper);
	PlaceMiniSetRandom1x1(Tile::VArchEnd,   CryptTile::VArchEnd2,   rndper);
	PlaceMiniSetRandom1x1(Tile::HArchVWall, CryptTile::HArchVWall2, rndper);
	PlaceMiniSetRandom1x1(Tile::VArch,      CryptTile::VArch2,      rndper);
	PlaceMiniSetRandom1x1(Tile::HArch,      CryptTile::HArch2,      rndper);
	PlaceMiniSetRandom1x1(Tile::Floor,      CryptTile::Floor2,      rndper);
	PlaceMiniSetRandom1x1(Tile::HWallVArch, CryptTile::HWallVArch2, rndper);
	PlaceMiniSetRandom1x1(Tile::Pillar,      CryptTile::Pillar3,      rndper);
	PlaceMiniSetRandom1x1(Tile::Pillar1,     CryptTile::Pillar4,      rndper);
	PlaceMiniSetRandom1x1(Tile::Pillar2,     CryptTile::Pillar5,      rndper);
	// clang-format on
}

void CryptBroken(int rndper)
{
	// clang-format off
	PlaceMiniSetRandom1x1(Tile::VWall,      CryptTile::VWall3,      rndper);
	PlaceMiniSetRandom1x1(Tile::HWall,      CryptTile::HWall3,      rndper);
	PlaceMiniSetRandom1x1(Tile::Corner,     CryptTile::Corner3,     rndper);
	PlaceMiniSetRandom1x1(Tile::DWall,      CryptTile::DWall3,      rndper);
	PlaceMiniSetRandom1x1(Tile::DArch,      CryptTile::DArch3,      rndper);
	PlaceMiniSetRandom1x1(Tile::VWallEnd,   CryptTile::VWallEnd3,   rndper);
	PlaceMiniSetRandom1x1(Tile::HWallEnd,   CryptTile::HWallEnd3,   rndper);
	PlaceMiniSetRandom1x1(Tile::HArchEnd,   CryptTile::HArchEnd3,   rndper);
	PlaceMiniSetRandom1x1(Tile::VArchEnd,   CryptTile::VArchEnd3,   rndper);
	PlaceMiniSetRandom1x1(Tile::HArchVWall, CryptTile::HArchVWall3, rndper);
	PlaceMiniSetRandom1x1(Tile::VArch,      CryptTile::VArch3,      rndper);
	PlaceMiniSetRandom1x1(Tile::HArch,      CryptTile::HArch3,      rndper);
	PlaceMiniSetRandom1x1(Tile::Floor,      CryptTile::Floor3,      rndper);
	PlaceMiniSetRandom1x1(Tile::HWallVArch, CryptTile::HWallVArch3, rndper);
	PlaceMiniSetRandom1x1(Tile::Pillar,      CryptTile::Pillar6,      rndper);
	PlaceMiniSetRandom1x1(Tile::Pillar1,     CryptTile::Pillar7,      rndper);
	PlaceMiniSetRandom1x1(Tile::Pillar2,     CryptTile::Pillar8,      rndper);
	// clang-format on
}

void CryptLeaking(int rndper)
{
	// clang-format off
	PlaceMiniSetRandom1x1(Tile::VWall,      CryptTile::VWall4,      rndper);
	PlaceMiniSetRandom1x1(Tile::HWall,      CryptTile::HWall4,      rndper);
	PlaceMiniSetRandom1x1(Tile::Corner,     CryptTile::Corner4,     rndper);
	PlaceMiniSetRandom1x1(Tile::DWall,      CryptTile::DWall4,      rndper);
	PlaceMiniSetRandom1x1(Tile::DArch,      CryptTile::DArch4,      rndper);
	PlaceMiniSetRandom1x1(Tile::VWallEnd,   CryptTile::VWallEnd4,   rndper);
	PlaceMiniSetRandom1x1(Tile::HWallEnd,   CryptTile::HWallEnd4,   rndper);
	PlaceMiniSetRandom1x1(Tile::HArchEnd,   CryptTile::HArchEnd4,   rndper);
	PlaceMiniSetRandom1x1(Tile::VArchEnd,   CryptTile::VArchEnd4,   rndper);
	PlaceMiniSetRandom1x1(Tile::HArchVWall, CryptTile::HArchVWall4, rndper);
	PlaceMiniSetRandom1x1(Tile::VArch,      CryptTile::VArch4,      rndper);
	PlaceMiniSetRandom1x1(Tile::HArch,      CryptTile::HArch4,      rndper);
	PlaceMiniSetRandom1x1(Tile::Floor,      CryptTile::Floor4,      rndper);
	PlaceMiniSetRandom1x1(Tile::HWallVArch, CryptTile::HWallVArch4, rndper);
	PlaceMiniSetRandom1x1(Tile::Pillar,      CryptTile::Pillar9,      rndper);
	PlaceMiniSetRandom1x1(Tile::Pillar1,     CryptTile::Pillar10,     rndper);
	PlaceMiniSetRandom1x1(Tile::Pillar2,     CryptTile::Pillar11,     rndper);
	// clang-format on
}

void CryptSubstitions1(int rndper)
{
	PlaceMiniSetRandom1x1(Tile::VArch, CryptTile::VArch6, rndper);
	PlaceMiniSetRandom1x1(Tile::HArch, CryptTile::HArch6, rndper);
	PlaceMiniSetRandom1x1(Tile::VArch, CryptTile::VArch7, rndper);
	PlaceMiniSetRandom1x1(Tile::HArch, CryptTile::HArch7, rndper);
	PlaceMiniSetRandom1x1(CryptTile::VWall5, CryptTile::VWall8, rndper);
	PlaceMiniSetRandom1x1(CryptTile::VWall5, CryptTile::VWall9, rndper);
	PlaceMiniSetRandom1x1(CryptTile::VWall6, CryptTile::VWall10, rndper);
	PlaceMiniSetRandom1x1(CryptTile::VWall6, CryptTile::VWall11, rndper);
	PlaceMiniSetRandom1x1(CryptTile::VWall7, CryptTile::VWall12, rndper);
	PlaceMiniSetRandom1x1(CryptTile::VWall7, CryptTile::VWall13, rndper);
	PlaceMiniSetRandom1x1(CryptTile::HWall5, CryptTile::HWall8, rndper);
	PlaceMiniSetRandom1x1(CryptTile::HWall5, CryptTile::HWall9, rndper);
	PlaceMiniSetRandom1x1(CryptTile::HWall5, CryptTile::HWall10, rndper);
	PlaceMiniSetRandom1x1(CryptTile::HWall5, CryptTile::HWall11, rndper);
	PlaceMiniSetRandom1x1(CryptTile::HWall5, CryptTile::HWall12, rndper);
	PlaceMiniSetRandom1x1(CryptTile::HWall5, CryptTile::HWall13, rndper);
	PlaceMiniSetRandom1x1(CryptTile::Floor7, CryptTile::Floor15, rndper);
	PlaceMiniSetRandom1x1(CryptTile::Floor7, CryptTile::Floor16, rndper);
	PlaceMiniSetRandom1x1(CryptTile::Floor6, CryptTile::Floor17, rndper);
	PlaceMiniSetRandom1x1(Tile::Pillar, CryptTile::Pillar12, rndper);
	PlaceMiniSetRandom1x1(CryptTile::Floor8, CryptTile::Floor18, rndper);
	PlaceMiniSetRandom1x1(CryptTile::Floor8, CryptTile::Floor19, rndper);
	PlaceMiniSetRandom1x1(CryptTile::Floor9, CryptTile::Floor20, rndper);
	PlaceMiniSetRandom1x1(CryptTile::Floor10, CryptTile::Floor21, rndper);
	PlaceMiniSetRandom1x1(CryptTile::Floor10, CryptTile::Floor22, rndper);
	PlaceMiniSetRandom1x1(CryptTile::Floor10, CryptTile::Floor23, rndper);
}

void CryptSubstitions2(int rndper)
{
	PlaceMiniSetRandom(CryptPillar1, rndper);
	PlaceMiniSetRandom(CryptPillar2, rndper);
	PlaceMiniSetRandom(CryptPillar3, rndper);
	PlaceMiniSetRandom(CryptPillar4, rndper);
	PlaceMiniSetRandom(CryptPillar5, rndper);
	PlaceMiniSetRandom(CryptStar, rndper);
	PlaceMiniSetRandom1x1(Tile::Floor, CryptTile::Floor11, rndper);
	PlaceMiniSetRandom1x1(Tile::Floor, CryptTile::Floor12, rndper);
	PlaceMiniSetRandom1x1(Tile::Floor, CryptTile::Floor13, rndper);
	PlaceMiniSetRandom1x1(Tile::Floor, CryptTile::Floor14, rndper);
}

void CryptFloor(int rndper)
{
	PlaceMiniSetRandom1x1(Tile::Floor, CryptTile::Floor6, rndper);
	PlaceMiniSetRandom1x1(Tile::Floor, CryptTile::Floor7, rndper);
	PlaceMiniSetRandom1x1(Tile::Floor, CryptTile::Floor8, rndper);
	PlaceMiniSetRandom1x1(Tile::Floor, CryptTile::Floor9, rndper);
}

bool PlaceCathedralStairs(lvl_entry entry)
{
	bool success = true;
	std::optional<Point> position;

	// Place poison water entrance
	if (Quests[Q_PWATER].IsAvailable()) {
		position = PlaceMiniSet(PWATERIN, DMAXX * DMAXY, true);
		if (!position) {
			success = false;
		} else {
			int8_t t = TransVal;
			TransVal = 0;
			Point miniPosition = *position;
			DRLG_MRectTrans({ miniPosition + Displacement { 0, 2 }, { 5, 2 } });
			TransVal = t;
			Quests[Q_PWATER].position = miniPosition.megaToWorld() + Displacement { 5, 7 };
			if (entry == ENTRY_RTNLVL)
				ViewPosition = Quests[Q_PWATER].position;
		}
	}

	// Place stairs up
	position = PlaceMiniSet(MyPlayer->pOriginalCathedral ? L5STAIRSUP : STAIRSUP, DMAXX * DMAXY, true);
	if (!position) {
		if (MyPlayer->pOriginalCathedral)
			return false;
		success = false;
	} else if (entry == ENTRY_MAIN) {
		ViewPosition = position->megaToWorld() + Displacement { 3, 4 };
	}

	// Place stairs down
	if (Quests[Q_LTBANNER].IsAvailable()) {
		if (entry == ENTRY_PREV)
			ViewPosition = SetPiece.position.megaToWorld() + Displacement { 4, 12 };
	} else {
		position = PlaceMiniSet(STAIRSDOWN, DMAXX * DMAXY, true);
		if (!position) {
			success = false;
		} else if (entry == ENTRY_PREV) {
			ViewPosition = position->megaToWorld() + Displacement { 3, 3 };
		}
	}

	return success;
}

bool PlaceCryptStairs(lvl_entry entry)
{
	bool success = true;
	std::optional<Point> position;

	// Place stairs up
	position = PlaceMiniSet(currlevel != 21 ? L5STAIRSUPHF : L5STAIRSTOWN, DMAXX * DMAXY, true);
	if (!position) {
		success = false;
	} else if (entry == ENTRY_MAIN || entry == ENTRY_TWARPDN) {
		ViewPosition = position->megaToWorld() + Displacement { 3, 5 };
	}

	// Place stairs down
	if (currlevel != 24) {
		position = PlaceMiniSet(L5STAIRSDOWN, DMAXX * DMAXY, true);
		if (!position)
			success = false;
		else if (entry == ENTRY_PREV)
			ViewPosition = position->megaToWorld() + Displacement { 3, 7 };
	}

	return success;
}

bool PlaceStairs(lvl_entry entry)
{
	if (leveltype == DTYPE_CRYPT) {
		return PlaceCryptStairs(entry);
	}

	return PlaceCathedralStairs(entry);
}

void GenerateLevel(lvl_entry entry)
{
	int minarea = 761;
	switch (currlevel) {
	case 1:
		minarea = 533;
		break;
	case 2:
		minarea = 693;
		break;
	default:
		break;
	}

	while (true) {
		DRLG_InitTrans();

		do {
			InitDungeonFlags();
			FirstRoom();
		} while (FindArea() < minarea);

		MakeDungeon();
		MakeDmt();
		FillChambers();
		FixTilesPatterns();
		AddWall();
		FloodTransparencyValues(13);
		if (PlaceStairs(entry))
			break;
	}

	for (int j = 0; j < DMAXY; j++) {
		for (int i = 0; i < DMAXX; i++) {
			if (dungeon[i][j] == Tile::EntranceStairs) {
				int xx = 2 * i + 16; /* todo: fix loop */
				int yy = 2 * j + 16;
				DRLG_CopyTrans(xx, yy + 1, xx, yy);
				DRLG_CopyTrans(xx + 1, yy + 1, xx + 1, yy);
			}
		}
	}

	FixTransparency();
	if (leveltype == DTYPE_CRYPT) {
		FixCryptDirtTiles();
	} else {
		FixDirtTiles();
	}
	FixCornerTiles();

	if (leveltype == DTYPE_CRYPT) {
		CryptStatues(10);
		PlaceMiniSetRandom1x1(Tile::VArch, CryptTile::VArch5, 95);
		PlaceMiniSetRandom1x1(Tile::HArch, CryptTile::HArch5, 95);
		PlaceMiniSetRandom(VWallSection, 100);
		PlaceMiniSetRandom(HWallSection, 100);
		PlaceMiniSetRandom(CryptFloorLave, 60);
		CryptLavafloor();
		switch (currlevel) {
		case 21:
			CryptCracked(30);
			CryptBroken(15);
			CryptLeaking(5);
			CryptLavafloor();
			CryptFloor(10);
			CryptSubstitions1(5);
			CryptSubstitions2(20);
			break;
		case 22:
			CryptFloor(10);
			CryptSubstitions1(10);
			CryptSubstitions2(20);
			CryptCracked(30);
			CryptBroken(20);
			CryptLeaking(10);
			CryptLavafloor();
			break;
		case 23:
			CryptFloor(10);
			CryptSubstitions1(15);
			CryptSubstitions2(30);
			CryptCracked(30);
			CryptBroken(20);
			CryptLeaking(15);
			CryptLavafloor();
			break;
		default:
			CryptFloor(10);
			CryptSubstitions1(20);
			CryptSubstitions2(30);
			CryptCracked(30);
			CryptBroken(20);
			CryptLeaking(20);
			CryptLavafloor();
			break;
		}
	} else {
		Substitution();
		ApplyShadowsPatterns();

		int numt = GenerateRnd(5) + 5;
		for (int i = 0; i < numt; i++) {
			PlaceMiniSet(LAMPS, DMAXX * DMAXY, true);
		}

		FillFloor();
	}

	for (int j = 0; j < DMAXY; j++) {
		for (int i = 0; i < DMAXX; i++) {
			pdungeon[i][j] = dungeon[i][j];
		}
	}

	DRLG_Init_Globals();
	DRLG_CheckQuests(SetPiece.position);
}

void Pass3()
{
	DRLG_LPass3(22 - 1);
}

} // namespace

void LoadL1Dungeon(const char *path, int vx, int vy)
{
	dminPosition = { 16, 16 };
	dmaxPosition = { 96, 96 };

	DRLG_InitTrans();

	for (int j = 0; j < DMAXY; j++) {
		for (int i = 0; i < DMAXX; i++) {
			dungeon[i][j] = 22;
			Protected[i][j] = false;
		}
	}

	auto dunData = LoadFileInMem<uint16_t>(path);
	PlaceDunTiles(dunData.get(), { 0, 0 }, Tile::Floor);

	FillFloor();

	ViewPosition = { vx, vy };

	Pass3();
	DRLG_Init_Globals();

	if (leveltype != DTYPE_CRYPT)
		InitDungeonPieces();

	SetMapMonsters(dunData.get(), Point(0, 0).megaToWorld());
	SetMapObjects(dunData.get(), 0, 0);
}

void LoadPreL1Dungeon(const char *path)
{
	for (int j = 0; j < DMAXY; j++) {
		for (int i = 0; i < DMAXX; i++) {
			dungeon[i][j] = 22;
			Protected[i][j] = false;
		}
	}

	dminPosition = { 16, 16 };
	dmaxPosition = { 96, 96 };

	auto dunData = LoadFileInMem<uint16_t>(path);
	PlaceDunTiles(dunData.get(), { 0, 0 }, Tile::Floor);

	FillFloor();

	for (int j = 0; j < DMAXY; j++) {
		for (int i = 0; i < DMAXX; i++) {
			pdungeon[i][j] = dungeon[i][j];
		}
	}
}

void CreateL5Dungeon(uint32_t rseed, lvl_entry entry)
{
	SetRndSeed(rseed);

	dminPosition = { 16, 16 };
	dmaxPosition = { 96, 96 };

	UberRow = 0;
	UberCol = 0;
	IsUberRoomOpened = false;
	IsUberLeverActivated = false;
	UberDiabloMonsterIndex = 0;

	DRLG_InitTrans();
	DRLG_InitSetPC();
	LoadQuestSetPieces();
	GenerateLevel(entry);
	Pass3();
	FreeQuestSetPieces();

	if (leveltype == DTYPE_CRYPT) {
		InitCryptPieces();
	} else {
		InitDungeonPieces();
	}

	DRLG_SetPC();

	for (int j = dminPosition.y; j < dmaxPosition.y; j++) {
		for (int i = dminPosition.x; i < dmaxPosition.x; i++) {
			if (dPiece[i][j] == 290) {
				UberRow = i;
				UberCol = j;
			}
			if (dPiece[i][j] == 317) {
				CornerStone.position = { i, j };
			}
		}
	}
}

} // namespace devilution

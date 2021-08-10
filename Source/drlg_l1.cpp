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
BYTE L5dflags[DMAXX][DMAXY];
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
const BYTE STAIRSUP[] = {
	// clang-format off
	4, 4, // width, height

	13, 13, 13, 13, // search
	 2,  2,  2,  2,
	13, 13, 13, 13,
	13, 13, 13, 13,

	 0, 66,  6,  0, // replace
	63, 64, 65,  0,
	 0, 67, 68,  0,
	 0,  0,  0,  0,
	// clang-format on
};
const BYTE L5STAIRSUPHF[] = {
	// clang-format off
	4, 5, // width, height

	22, 22, 22, 22, // search
	22, 22, 22, 22,
	 2,  2,  2,  2,
	13, 13, 13, 13,
	13, 13, 13, 13,

	 0, 54, 23,  0, // replace
	 0, 53, 18,  0,
	55, 56, 57,  0,
	58, 59, 60,  0,
	 0,  0,  0,  0
	// clang-format on
};
/** Miniset: stairs up. */
const BYTE L5STAIRSUP[] = {
	// clang-format off
	4, 4, // width, height

	22, 22, 22, 22, // search
	 2,  2,  2,  2,
	13, 13, 13, 13,
	13, 13, 13, 13,

	 0, 66, 23,  0, // replace
	63, 64, 65,  0,
	 0, 67, 68,  0,
	 0,  0,  0,  0,
	// clang-format on
};
/** Miniset: stairs down. */
const BYTE STAIRSDOWN[] = {
	// clang-format off
	4, 3, // width, height

	13, 13, 13, 13, // search
	13, 13, 13, 13,
	13, 13, 13, 13,

	62, 57, 58,  0, // replace
	61, 59, 60,  0,
	 0,  0,  0,  0,
	// clang-format on
};
const BYTE L5STAIRSDOWN[] = {
	// clang-format off
	4, 5, // width, height

	13, 13, 13, 13, // search
	13, 13, 13, 13,
	13, 13, 13, 13,
	13, 13, 13, 13,
	13, 13, 13, 13,

	 0,  0, 52,  0, // replace
	 0, 48, 51,  0,
	 0, 47, 50,  0,
	45, 46, 49,  0,
	 0,  0,  0,  0,
	// clang-format on
};
const BYTE L5STAIRSTOWN[] = {
	// clang-format off
	4, 5, // width, height

	22, 22, 22, 22, // search
	22, 22, 22, 22,
	 2,  2,  2,  2,
	13, 13, 13, 13,
	13, 13, 13, 13,

	 0, 62, 23,  0, // replace
	 0, 61, 18,  0,
	63, 64, 65,  0,
	66, 67, 68,  0,
	 0,  0,  0,  0,
	// clang-format on
};
/** Miniset: candlestick. */
const BYTE LAMPS[] = {
	// clang-format off
	2, 2, // width, height

	13,  0, // search
	13, 13,

	129,   0, // replace
	130, 128,
	// clang-format on
};
/** Miniset: Poisoned Water Supply entrance. */
const BYTE PWATERIN[] = {
	// clang-format off
	6, 6, // width, height

	13, 13, 13, 13, 13, 13, // search
	13, 13, 13, 13, 13, 13,
	13, 13, 13, 13, 13, 13,
	13, 13, 13, 13, 13, 13,
	13, 13, 13, 13, 13, 13,
	13, 13, 13, 13, 13, 13,

	 0,   0,   0,   0,   0, 0, // replace
	 0, 202, 200, 200,  84, 0,
	 0, 199, 203, 203,  83, 0,
	 0,  85, 206,  80,  81, 0,
	 0,   0, 134, 135,   0, 0,
	 0,   0,   0,   0,   0, 0,
	// clang-format on
};
const BYTE CryptPattern1[4] = { 1, 1, 11, 95 };
const BYTE CryptPattern2[8] = { 1, 1, 12, 96 };
const BYTE CryptPattern3[8] = {
	// clang-format off
	1, 3, // width, height

	1, // search
	1,
	1,

	91, // replace
	90,
	89,
	// clang-format on
};
const BYTE CryptPattern4[8] = {
	// clang-format off
	3, 1, // width, height

	 2,  2,  2, // search

	94, 93, 92, // replace
	// clang-format on
};
const BYTE CryptPattern5[4] = { 1, 1, 13, 97 };
const BYTE CryptPattern6[4] = { 1, 1, 13, 98 };
const BYTE CryptPattern7[4] = { 1, 1, 13, 99 };
const BYTE CryptPattern8[4] = { 1, 1, 13, 100 };
const BYTE CryptPattern9[20] = {
	// clang-format off
	3, 3, // width, height

	13, 13, 13, // search
	13, 13, 13,
	13, 13, 13,

	0,   0, 0, // replace
	0, 101, 0,
	0,   0, 0,
	// clang-format on
};
const BYTE CryptPattern10[4] = { 1, 1, 11, 185 };
const BYTE CryptPattern11[4] = { 1, 1, 11, 186 };
const BYTE CryptPattern12[4] = { 1, 1, 12, 187 };
const BYTE CryptPattern13[4] = { 1, 1, 12, 188 };
const BYTE CryptPattern14[4] = { 1, 1, 89, 173 };
const BYTE CryptPattern15[4] = { 1, 1, 89, 174 };
const BYTE CryptPattern16[4] = { 1, 1, 90, 175 };
const BYTE CryptPattern17[4] = { 1, 1, 90, 176 };
const BYTE CryptPattern18[4] = { 1, 1, 91, 177 };
const BYTE CryptPattern19[4] = { 1, 1, 91, 178 };
const BYTE CryptPattern20[4] = { 1, 1, 92, 179 };
const BYTE CryptPattern21[4] = { 1, 1, 92, 180 };
const BYTE CryptPattern22[4] = { 1, 1, 92, 181 };
const BYTE CryptPattern23[4] = { 1, 1, 92, 182 };
const BYTE CryptPattern24[4] = { 1, 1, 92, 183 };
const BYTE CryptPattern25[4] = { 1, 1, 92, 184 };
const BYTE CryptPattern26[4] = { 1, 1, 98, 189 };
const BYTE CryptPattern27[4] = { 1, 1, 98, 190 };
const BYTE CryptPattern28[4] = { 1, 1, 97, 191 };
const BYTE CryptPattern29[4] = { 1, 1, 15, 192 };
const BYTE CryptPattern30[4] = { 1, 1, 99, 193 };
const BYTE CryptPattern31[4] = { 1, 1, 99, 194 };
const BYTE CryptPattern32[4] = { 1, 1, 100, 195 };
const BYTE CryptPattern33[4] = { 1, 1, 101, 196 };
const BYTE CryptPattern34[4] = { 1, 1, 101, 197 };
const BYTE CryptPattern35[8] = { 1, 1, 101, 198 };
const BYTE CryptPattern36[24] = {
	// clang-format off
	3, 3, // width, height

	13, 13, 13, // search
	13, 13, 13,
	13, 13, 13,

	0,   0, 0, // replace
	0, 167, 0,
	0,   0, 0,
	// clang-format on
};
const BYTE CryptPattern37[24] = {
	// clang-format off
	3, 3, // width, height

	13, 13, 13, // search
	13, 13, 13,
	13, 13, 13,

	0,   0, 0, // replace
	0, 168, 0,
	0,   0, 0,
	// clang-format on
};
const BYTE CryptPattern38[24] = {
	// clang-format off
	3, 3, // width, height

	13, 13, 13, // search
	13, 13, 13,
	13, 13, 13,

	0,   0, 0, // replace
	0, 169, 0,
	0,   0, 0,
};
const BYTE CryptPattern39[24] = {
	// clang-format off
	3, 3, // width, height

	13, 13, 13, // search
	13, 13, 13,
	13, 13, 13,

	0,   0, 0, // replace
	0, 170, 0,
	0,   0, 0,
	// clang-format on
};
const BYTE CryptPattern40[24] = {
	// clang-format off
	3, 3, // width, height

	13, 13, 13, // search
	13, 13, 13,
	13, 13, 13,

	0,   0, 0, // replace
	0, 171, 0,
	0,   0, 0,
	// clang-format on
};
const BYTE CryptPattern41[20] = {
	// clang-format off
	3, 3, // width, height

	13, 13, 13, // search
	13, 13, 13,
	13, 13, 13,

	 0,   0, 0, // replace
	 0, 172, 0,
	 0,   0, 0,
	// clang-format on
};
const BYTE CryptPattern42[4] = { 1, 1, 13, 163 };
const BYTE CryptPattern43[4] = { 1, 1, 13, 164 };
const BYTE CryptPattern44[4] = { 1, 1, 13, 165 };
const BYTE CryptPattern45[4] = { 1, 1, 13, 166 };
const BYTE CryptPattern46[4] = { 1, 1, 1, 112 };
const BYTE CryptPattern47[4] = { 1, 1, 2, 113 };
const BYTE CryptPattern48[4] = { 1, 1, 3, 114 };
const BYTE CryptPattern49[4] = { 1, 1, 4, 115 };
const BYTE CryptPattern50[4] = { 1, 1, 5, 116 };
const BYTE CryptPattern51[4] = { 1, 1, 6, 117 };
const BYTE CryptPattern52[4] = { 1, 1, 7, 118 };
const BYTE CryptPattern53[4] = { 1, 1, 8, 119 };
const BYTE CryptPattern54[4] = { 1, 1, 9, 120 };
const BYTE CryptPattern55[4] = { 1, 1, 10, 121 };
const BYTE CryptPattern56[4] = { 1, 1, 11, 122 };
const BYTE CryptPattern57[4] = { 1, 1, 12, 123 };
const BYTE CryptPattern58[4] = { 1, 1, 13, 124 };
const BYTE CryptPattern59[4] = { 1, 1, 14, 125 };
const BYTE CryptPattern60[4] = { 1, 1, 15, 126 };
const BYTE CryptPattern61[4] = { 1, 1, 16, 127 };
const BYTE CryptPattern62[4] = { 1, 1, 17, 128 };
const BYTE CryptPattern63[4] = { 1, 1, 1, 129 };
const BYTE CryptPattern64[4] = { 1, 1, 2, 130 };
const BYTE CryptPattern65[4] = { 1, 1, 3, 131 };
const BYTE CryptPattern66[4] = { 1, 1, 4, 132 };
const BYTE CryptPattern67[4] = { 1, 1, 5, 133 };
const BYTE CryptPattern68[4] = { 1, 1, 6, 134 };
const BYTE CryptPattern69[4] = { 1, 1, 7, 135 };
const BYTE CryptPattern70[4] = { 1, 1, 8, 136 };
const BYTE CryptPattern71[4] = { 1, 1, 9, 137 };
const BYTE CryptPattern72[4] = { 1, 1, 10, 138 };
const BYTE CryptPattern73[4] = { 1, 1, 11, 139 };
const BYTE CryptPattern74[4] = { 1, 1, 12, 140 };
const BYTE CryptPattern75[4] = { 1, 1, 13, 141 };
const BYTE CryptPattern76[4] = { 1, 1, 14, 142 };
const BYTE CryptPattern77[4] = { 1, 1, 15, 143 };
const BYTE CryptPattern78[4] = { 1, 1, 16, 144 };
const BYTE CryptPattern79[4] = { 1, 1, 17, 145 };
const BYTE CryptPattern80[4] = { 1, 1, 1, 146 };
const BYTE CryptPattern81[4] = { 1, 1, 2, 147 };
const BYTE CryptPattern82[4] = { 1, 1, 3, 148 };
const BYTE CryptPattern83[4] = { 1, 1, 4, 149 };
const BYTE CryptPattern84[4] = { 1, 1, 5, 150 };
const BYTE CryptPattern85[4] = { 1, 1, 6, 151 };
const BYTE CryptPattern86[4] = { 1, 1, 7, 152 };
const BYTE CryptPattern87[4] = { 1, 1, 8, 153 };
const BYTE CryptPattern88[4] = { 1, 1, 9, 154 };
const BYTE CryptPattern89[4] = { 1, 1, 10, 155 };
const BYTE CryptPattern90[4] = { 1, 1, 11, 156 };
const BYTE CryptPattern91[4] = { 1, 1, 12, 157 };
const BYTE CryptPattern92[4] = { 1, 1, 13, 158 };
const BYTE CryptPattern93[4] = { 1, 1, 14, 159 };
const BYTE CryptPattern94[4] = { 1, 1, 15, 160 };
const BYTE CryptPattern95[4] = { 1, 1, 16, 161 };
const BYTE CryptPattern96[4] = { 1, 1, 17, 162 };
const BYTE CryptPattern97[4] = { 1, 1, 1, 199 };
const BYTE CryptPattern98[4] = { 1, 1, 1, 201 };
const BYTE CryptPattern99[4] = { 1, 1, 2, 200 };
const BYTE CryptPattern100[4] = { 1, 1, 2, 202 };

/* data */

BYTE UberRoomPattern[26] = {
	// clang-format off
	4, 6, // width, height

	115, 130,   6, 13, // pattern
	129, 108,   1, 13,
	  1, 107, 103, 13,
	146, 106, 102, 13,
	129, 168,   1, 13,
	  7,   2,   3, 13,
	// clang-format on
};
BYTE CornerstoneRoomPattern[27] = {
	// clang-format off
	5, 5, // width, height

	4,   2,   2, 2,  6, // pattern
	1, 111, 172, 0,  1,
	1, 172,   0, 0, 25,
	1,   0,   0, 0,  1,
	7,   2,   2, 2,  3,
	// clang-format on
};
/**
 * A lookup table for the 16 possible patterns of a 2x2 area,
 * where each cell either contains a SW wall or it doesn't.
 */
BYTE L5ConvTbl[16] = { 22, 13, 1, 13, 2, 13, 13, 13, 4, 13, 1, 13, 2, 13, 16, 13 };

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

void PlaceDoor(int x, int y)
{
	if ((L5dflags[x][y] & DLRG_PROTECTED) == 0) {
		BYTE df = L5dflags[x][y] & 0x7F;
		BYTE c = dungeon[x][y];

		if (df == 1) {
			if (y != 1 && c == 2)
				dungeon[x][y] = 26;
			if (y != 1 && c == 7)
				dungeon[x][y] = 31;
			if (y != 1 && c == 14)
				dungeon[x][y] = 42;
			if (y != 1 && c == 4)
				dungeon[x][y] = 43;
			if (x != 1 && c == 1)
				dungeon[x][y] = 25;
			if (x != 1 && c == 10)
				dungeon[x][y] = 40;
			if (x != 1 && c == 6)
				dungeon[x][y] = 30;
		}
		if (df == 2) {
			if (x != 1 && c == 1)
				dungeon[x][y] = 25;
			if (x != 1 && c == 6)
				dungeon[x][y] = 30;
			if (x != 1 && c == 10)
				dungeon[x][y] = 40;
			if (x != 1 && c == 4)
				dungeon[x][y] = 41;
			if (y != 1 && c == 2)
				dungeon[x][y] = 26;
			if (y != 1 && c == 14)
				dungeon[x][y] = 42;
			if (y != 1 && c == 7)
				dungeon[x][y] = 31;
		}
		if (df == 3) {
			if (x != 1 && y != 1 && c == 4)
				dungeon[x][y] = 28;
			if (x != 1 && c == 10)
				dungeon[x][y] = 40;
			if (y != 1 && c == 14)
				dungeon[x][y] = 42;
			if (y != 1 && c == 2)
				dungeon[x][y] = 26;
			if (x != 1 && c == 1)
				dungeon[x][y] = 25;
			if (y != 1 && c == 7)
				dungeon[x][y] = 31;
			if (x != 1 && c == 6)
				dungeon[x][y] = 30;
		}
	}

	L5dflags[x][y] = DLRG_PROTECTED;
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

				if (shadow.nv1 != 0 && L5dflags[x - 1][y - 1] == 0) {
					dungeon[x - 1][y - 1] = shadow.nv1;
				}
				if (shadow.nv2 != 0 && L5dflags[x][y - 1] == 0) {
					dungeon[x][y - 1] = shadow.nv2;
				}
				if (shadow.nv3 != 0 && L5dflags[x - 1][y] == 0) {
					dungeon[x - 1][y] = shadow.nv3;
				}
			}
		}
	}

	for (int y = 1; y < DMAXY; y++) {
		for (int x = 1; x < DMAXX; x++) {
			if (dungeon[x - 1][y] == 139 && L5dflags[x - 1][y] == 0) {
				uint8_t tnv3 = 139;
				if (IsAnyOf(dungeon[x][y], 29, 32, 35, 37, 38, 39)) {
					tnv3 = 141;
				}
				dungeon[x - 1][y] = tnv3;
			}
			if (dungeon[x - 1][y] == 149 && L5dflags[x - 1][y] == 0) {
				uint8_t tnv3 = 149;
				if (IsAnyOf(dungeon[x][y], 29, 32, 35, 37, 38, 39)) {
					tnv3 = 153;
				}
				dungeon[x - 1][y] = tnv3;
			}
			if (dungeon[x - 1][y] == 148 && L5dflags[x - 1][y] == 0) {
				uint8_t tnv3 = 148;
				if (IsAnyOf(dungeon[x][y], 29, 32, 35, 37, 38, 39)) {
					tnv3 = 154;
				}
				dungeon[x - 1][y] = tnv3;
			}
		}
	}
}

int PlaceMiniSet(const BYTE *miniset, int tmin, int tmax, int cx, int cy, bool setview, int noquad)
{
	int sx;
	int sy;

	int sw = miniset[0];
	int sh = miniset[1];

	int numt = 1;
	if (tmax - tmin != 0) {
		numt = GenerateRnd(tmax - tmin) + tmin;
	}

	for (int i = 0; i < numt; i++) {
		sx = GenerateRnd(DMAXX - sw);
		sy = GenerateRnd(DMAXY - sh);
		bool abort = false;
		int found = 0;

		while (!abort) {
			abort = true;
			if (cx != -1 && sx >= cx - sw && sx <= cx + 12) {
				sx++;
				abort = false;
			}
			if (cy != -1 && sy >= cy - sh && sy <= cy + 12) {
				sy++;
				abort = false;
			}

			switch (noquad) {
			case 0:
				if (sx < cx && sy < cy)
					abort = false;
				break;
			case 1:
				if (sx > cx && sy < cy)
					abort = false;
				break;
			case 2:
				if (sx < cx && sy > cy)
					abort = false;
				break;
			case 3:
				if (sx > cx && sy > cy)
					abort = false;
				break;
			}

			int ii = 2;

			for (int yy = 0; yy < sh && abort; yy++) {
				for (int xx = 0; xx < sw && abort; xx++) {
					if (miniset[ii] != 0 && dungeon[xx + sx][sy + yy] != miniset[ii])
						abort = false;
					if (L5dflags[xx + sx][sy + yy] != 0)
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
				if (++found > 4000)
					return -1;
			}
		}

		int ii = sw * sh + 2;

		for (int yy = 0; yy < sh; yy++) {
			for (int xx = 0; xx < sw; xx++) {
				if (miniset[ii] != 0) {
					dungeon[xx + sx][sy + yy] = miniset[ii];
				}
				ii++;
			}
		}
	}

	if (miniset == PWATERIN) {
		int8_t t = TransVal;
		TransVal = 0;
		DRLG_MRectTrans(sx, sy + 2, sx + 5, sy + 4);
		TransVal = t;

		Quests[Q_PWATER].position = { 2 * sx + 21, 2 * sy + 22 };
	}

	if (setview) {
		ViewX = 2 * sx + 19;
		ViewY = 2 * sy + 20;
	}

	if (sx < cx && sy < cy)
		return 0;
	if (sx > cx && sy < cy)
		return 1;
	if (sx < cx && sy > cy)
		return 2;

	return 3;
}

void PlaceMiniSetRandom(const BYTE *miniset, int rndper)
{
	int sw = miniset[0];
	int sh = miniset[1];

	for (int sy = 0; sy < DMAXY - sh; sy++) {
		for (int sx = 0; sx < DMAXX - sw; sx++) {
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
			if (miniset[kk] >= 84 && miniset[kk] <= 100 && found) {
				// BUGFIX: accesses to dungeon can go out of bounds (fixed)
				// BUGFIX: Comparisons vs 100 should use same tile as comparisons vs 84 - NOT A BUG - "fixing" this breaks crypt

				constexpr auto ComparisonWithBoundsCheck = [](Point p1, Point p2) {
					return (p1.x >= 0 && p1.x < DMAXX && p1.y >= 0 && p1.y < DMAXY) && (p2.x >= 0 && p2.x < DMAXX && p2.y >= 0 && p2.y < DMAXY) && (dungeon[p1.x][p1.y] >= 84 && dungeon[p2.x][p2.y] <= 100);
				};
				if (ComparisonWithBoundsCheck({ sx - 1, sy }, { sx - 1, sy })) {
					found = false;
				}
				if (ComparisonWithBoundsCheck({ sx + 1, sy }, { sx - 1, sy })) {
					found = false;
				}
				if (ComparisonWithBoundsCheck({ sx, sy + 1 }, { sx - 1, sy })) {
					found = false;
				}
				if (ComparisonWithBoundsCheck({ sx, sy - 1 }, { sx - 1, sy })) {
					found = false;
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

void FillFloor()
{
	for (int j = 0; j < DMAXY; j++) {
		for (int i = 0; i < DMAXX; i++) {
			if (L5dflags[i][j] == 0 && dungeon[i][j] == 13) {
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

	if (QuestStatus(Q_BUTCHER)) {
		L5pSetPiece = LoadFileInMem<uint16_t>("Levels\\L1Data\\rnd6.DUN");
		L5setloadflag = true;
	} else if (QuestStatus(Q_SKELKING) && !gbIsMultiplayer) {
		L5pSetPiece = LoadFileInMem<uint16_t>("Levels\\L1Data\\SKngDO.DUN");
		L5setloadflag = true;
	} else if (QuestStatus(Q_LTBANNER)) {
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
			L5dflags[i][j] = 0;
		}
	}
}

void ClearFlags()
{
	for (int j = 0; j < DMAXY; j++) {
		for (int i = 0; i < DMAXX; i++) { // NOLINT(modernize-loop-convert)
			L5dflags[i][j] &= ~DLRG_CHAMBER;
		}
	}
}

void MapRoom(int x, int y, int width, int height)
{
	for (int j = 0; j < height; j++) {
		for (int i = 0; i < width; i++) {
			dungeon[x + i][y + j] = 1;
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
			ran = CheckRoom(cx1 - 1, cy1 - 1, ch + 2, cw + 1); /// BUGFIX: swap args 3 and 4 ("ch+2" and "cw+1")
			num++;
		} while (!ran && num < 20);

		if (ran)
			MapRoom(cx1, cy1, cw, ch);
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
			dungeon[17][y] = 1;
			dungeon[18][y] = 1;
			dungeon[19][y] = 1;
			dungeon[20][y] = 1;
			dungeon[21][y] = 1;
			dungeon[22][y] = 1;
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
			dungeon[x][17] = 1;
			dungeon[x][18] = 1;
			dungeon[x][19] = 1;
			dungeon[x][20] = 1;
			dungeon[x][21] = 1;
			dungeon[x][22] = 1;
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
			if (dungeon[i][j] == 1)
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
		if (dungeon[i + x][j - 1] != 13 || dungeon[i + x][j + 1] != 13 || L5dflags[i + x][j] != 0)
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
		if (dungeon[i - 1][j + y] != 13 || dungeon[i + 1][j + y] != 13 || L5dflags[i][j + y] != 0)
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

void HorizontalWall(int i, int j, char p, int dx)
{
	int8_t dt;

	switch (GenerateRnd(4)) {
	case 0:
	case 1:
		dt = 2;
		break;
	case 2:
		dt = 12;
		if (p == 2)
			p = 12;
		if (p == 4)
			p = 10;
		break;
	case 3:
		dt = 36;
		if (p == 2)
			p = 36;
		if (p == 4)
			p = 27;
		break;
	}

	int8_t wt = 26;
	if (GenerateRnd(6) == 5)
		wt = 12;

	if (dt == 12)
		wt = 12;

	dungeon[i][j] = p;

	for (int xx = 1; xx < dx; xx++) {
		dungeon[i + xx][j] = dt;
	}

	int xx = GenerateRnd(dx - 1) + 1;

	if (wt == 12) {
		dungeon[i + xx][j] = wt;
	} else {
		dungeon[i + xx][j] = 2;
		L5dflags[i + xx][j] |= DLRG_HDOOR;
	}
}

void VerticalWall(int i, int j, char p, int dy)
{
	int8_t dt;

	switch (GenerateRnd(4)) {
	case 0:
	case 1:
		dt = 1;
		break;
	case 2:
		dt = 11;
		if (p == 1)
			p = 11;
		if (p == 4)
			p = 14;
		break;
	case 3:
		dt = 35;
		if (p == 1)
			p = 35;
		if (p == 4)
			p = 37;
		break;
	}

	int8_t wt = 25;
	if (GenerateRnd(6) == 5)
		wt = 11;

	if (dt == 11)
		wt = 11;

	dungeon[i][j] = p;

	for (int yy = 1; yy < dy; yy++) {
		dungeon[i][j + yy] = dt;
	}

	int yy = GenerateRnd(dy - 1) + 1;

	if (wt == 11) {
		dungeon[i][j + yy] = wt;
	} else {
		dungeon[i][j + yy] = 1;
		L5dflags[i][j + yy] |= DLRG_VDOOR;
	}
}

void AddWall()
{
	for (int j = 0; j < DMAXY; j++) {
		for (int i = 0; i < DMAXX; i++) {
			if (L5dflags[i][j] == 0) {
				if (dungeon[i][j] == 3) {
					AdvanceRndSeed();
					int x = HorizontalWallOk(i, j);
					if (x != -1) {
						HorizontalWall(i, j, 2, x);
					}
				}
				if (dungeon[i][j] == 3) {
					AdvanceRndSeed();
					int y = VerticalWallOk(i, j);
					if (y != -1) {
						VerticalWall(i, j, 1, y);
					}
				}
				if (dungeon[i][j] == 6) {
					AdvanceRndSeed();
					int x = HorizontalWallOk(i, j);
					if (x != -1) {
						HorizontalWall(i, j, 4, x);
					}
				}
				if (dungeon[i][j] == 7) {
					AdvanceRndSeed();
					int y = VerticalWallOk(i, j);
					if (y != -1) {
						VerticalWall(i, j, 4, y);
					}
				}
				if (dungeon[i][j] == 2) {
					AdvanceRndSeed();
					int x = HorizontalWallOk(i, j);
					if (x != -1) {
						HorizontalWall(i, j, 2, x);
					}
				}
				if (dungeon[i][j] == 1) {
					AdvanceRndSeed();
					int y = VerticalWallOk(i, j);
					if (y != -1) {
						VerticalWall(i, j, 1, y);
					}
				}
			}
		}
	}
}

void GenerateChamber(int sx, int sy, bool topflag, bool bottomflag, bool leftflag, bool rightflag)
{
	if (topflag) {
		dungeon[sx + 2][sy] = 12;
		dungeon[sx + 3][sy] = 12;
		dungeon[sx + 4][sy] = 3;
		dungeon[sx + 7][sy] = 9;
		dungeon[sx + 8][sy] = 12;
		dungeon[sx + 9][sy] = 2;
	}
	if (bottomflag) {
		sy += 11;
		dungeon[sx + 2][sy] = 10;
		dungeon[sx + 3][sy] = 12;
		dungeon[sx + 4][sy] = 8;
		dungeon[sx + 7][sy] = 5;
		dungeon[sx + 8][sy] = 12;
		if (dungeon[sx + 9][sy] != 4) {
			dungeon[sx + 9][sy] = 21;
		}
		sy -= 11;
	}
	if (leftflag) {
		dungeon[sx][sy + 2] = 11;
		dungeon[sx][sy + 3] = 11;
		dungeon[sx][sy + 4] = 3;
		dungeon[sx][sy + 7] = 8;
		dungeon[sx][sy + 8] = 11;
		dungeon[sx][sy + 9] = 1;
	}
	if (rightflag) {
		sx += 11;
		dungeon[sx][sy + 2] = 14;
		dungeon[sx][sy + 3] = 11;
		dungeon[sx][sy + 4] = 9;
		dungeon[sx][sy + 7] = 5;
		dungeon[sx][sy + 8] = 11;
		if (dungeon[sx][sy + 9] != 4) {
			dungeon[sx][sy + 9] = 21;
		}
		sx -= 11;
	}

	for (int j = 1; j < 11; j++) {
		for (int i = 1; i < 11; i++) {
			dungeon[i + sx][j + sy] = 13;
			L5dflags[i + sx][j + sy] |= DLRG_CHAMBER;
		}
	}

	dungeon[sx + 4][sy + 4] = 15;
	dungeon[sx + 7][sy + 4] = 15;
	dungeon[sx + 4][sy + 7] = 15;
	dungeon[sx + 7][sy + 7] = 15;
}

void GenerateHall(int x1, int y1, int x2, int y2)
{
	if (y1 == y2) {
		for (int i = x1; i < x2; i++) {
			dungeon[i][y1] = 12;
			dungeon[i][y1 + 3] = 12;
		}
		return;
	}

	for (int i = y1; i < y2; i++) {
		dungeon[x1][i] = 11;
		dungeon[x1 + 3][i] = 11;
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
	int rw = CornerstoneRoomPattern[0];
	int rh = CornerstoneRoomPattern[1];

	setpc_x = rx1;
	setpc_y = ry1;
	setpc_w = rw;
	setpc_h = rh;

	int sp = 2;

	for (int j = 0; j < rh; j++) {
		for (int i = 0; i < rw; i++) {
			if (CornerstoneRoomPattern[sp] != 0) {
				dungeon[rx1 + i][ry1 + j] = CornerstoneRoomPattern[sp];
				L5dflags[rx1 + i][ry1 + j] |= DLRG_PROTECTED;
			} else {
				dungeon[rx1 + i][ry1 + j] = 13;
			}
			sp++;
		}
	}
}
void Substitution()
{
	for (int y = 0; y < DMAXY; y++) {
		for (int x = 0; x < DMAXX; x++) {
			if (GenerateRnd(4) == 0) {
				uint8_t c = L5BTYPES[dungeon[x][y]];
				if (c != 0 && L5dflags[x][y] == 0) {
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
						if (L5BTYPES[dungeon[x][y - 1]] != 79 || L5dflags[x][y - 1] != 0)
							i = 79;
						else
							dungeon[x][y - 1] = 90;
					}
					// BUGFIX: Add `&& x + 1 < DMAXX` to the if statement. (fixed)
					if (i == 91 && x + 1 < DMAXX) {
						if (L5BTYPES[dungeon[x + 1][y]] != 80 || L5dflags[x + 1][y] != 0)
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

void SetRoom(int rx1, int ry1)
{
	int width = SDL_SwapLE16(L5pSetPiece[0]);
	int height = SDL_SwapLE16(L5pSetPiece[1]);

	setpc_x = rx1;
	setpc_y = ry1;
	setpc_w = width;
	setpc_h = height;

	uint16_t *tileLayer = &L5pSetPiece[2];

	for (int j = 0; j < height; j++) {
		for (int i = 0; i < width; i++) {
			uint8_t tileId = SDL_SwapLE16(tileLayer[j * width + i]);
			if (tileId != 0) {
				dungeon[rx1 + i][ry1 + j] = tileId;
				L5dflags[rx1 + i][ry1 + j] |= DLRG_PROTECTED;
			} else {
				dungeon[rx1 + i][ry1 + j] = 13;
			}
		}
	}
}

void SetCryptRoom(int rx1, int ry1)
{
	int rw = UberRoomPattern[0];
	int rh = UberRoomPattern[1];

	UberRow = 2 * rx1 + 6;
	UberCol = 2 * ry1 + 8;
	setpc_x = rx1;
	setpc_y = ry1;
	setpc_w = rw;
	setpc_h = rh;
	IsUberRoomOpened = false;
	IsUberLeverActivated = false;

	int sp = 2;

	for (int j = 0; j < rh; j++) {
		for (int i = 0; i < rw; i++) {
			if (UberRoomPattern[sp] != 0) {
				dungeon[rx1 + i][ry1 + j] = UberRoomPattern[sp];
				L5dflags[rx1 + i][ry1 + j] |= DLRG_PROTECTED;
			} else {
				dungeon[rx1 + i][ry1 + j] = 13;
			}
			sp++;
		}
	}
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
				SetRoom(16, 2);
				break;
			case 1:
				SetRoom(16, 16);
				break;
			case 2:
				SetRoom(16, 30);
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
				SetRoom(2, 16);
				break;
			case 1:
				SetRoom(16, 16);
				break;
			case 2:
				SetRoom(30, 16);
				break;
			}
		}
	}
}

void FloodTransparancyValues()
{
	int yy = 16;
	uint8_t floorID = 13;
	for (int j = 0; j < DMAXY; j++) {
		int xx = 16;
		for (int i = 0; i < DMAXX; i++) {
			if (dungeon[i][j] == floorID && dTransVal[xx][yy] == 0) {
				FindTransparencyValues(i, j, xx, yy, 0, floorID);
				TransVal++;
			}
			xx += 2;
		}
		yy += 2;
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
	if (currlevel < 21) {
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
		return;
	}

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
			if ((L5dflags[i][j] & DLRG_PROTECTED) == 0 && dungeon[i][j] == 17 && dungeon[i - 1][j] == 13 && dungeon[i][j - 1] == 1) {
				dungeon[i][j] = 16;
				L5dflags[i][j - 1] &= DLRG_PROTECTED;
			}
			if (dungeon[i][j] == 202 && dungeon[i + 1][j] == 13 && dungeon[i][j + 1] == 1) {
				dungeon[i][j] = 8;
			}
		}
	}
}

void CryptPatternGroup1(int rndper)
{
	PlaceMiniSetRandom(CryptPattern97, rndper);
	PlaceMiniSetRandom(CryptPattern98, rndper);
	PlaceMiniSetRandom(CryptPattern99, rndper);
	PlaceMiniSetRandom(CryptPattern100, rndper);
}

void CryptPatternGroup2(int rndper)
{
	PlaceMiniSetRandom(CryptPattern46, rndper);
	PlaceMiniSetRandom(CryptPattern47, rndper);
	PlaceMiniSetRandom(CryptPattern48, rndper);
	PlaceMiniSetRandom(CryptPattern49, rndper);
	PlaceMiniSetRandom(CryptPattern50, rndper);
	PlaceMiniSetRandom(CryptPattern51, rndper);
	PlaceMiniSetRandom(CryptPattern52, rndper);
	PlaceMiniSetRandom(CryptPattern53, rndper);
	PlaceMiniSetRandom(CryptPattern54, rndper);
	PlaceMiniSetRandom(CryptPattern55, rndper);
	PlaceMiniSetRandom(CryptPattern56, rndper);
	PlaceMiniSetRandom(CryptPattern57, rndper);
	PlaceMiniSetRandom(CryptPattern58, rndper);
	PlaceMiniSetRandom(CryptPattern59, rndper);
	PlaceMiniSetRandom(CryptPattern60, rndper);
	PlaceMiniSetRandom(CryptPattern61, rndper);
	PlaceMiniSetRandom(CryptPattern62, rndper);
}

void CryptPatternGroup3(int rndper)
{
	PlaceMiniSetRandom(CryptPattern63, rndper);
	PlaceMiniSetRandom(CryptPattern64, rndper);
	PlaceMiniSetRandom(CryptPattern65, rndper);
	PlaceMiniSetRandom(CryptPattern66, rndper);
	PlaceMiniSetRandom(CryptPattern67, rndper);
	PlaceMiniSetRandom(CryptPattern68, rndper);
	PlaceMiniSetRandom(CryptPattern69, rndper);
	PlaceMiniSetRandom(CryptPattern70, rndper);
	PlaceMiniSetRandom(CryptPattern71, rndper);
	PlaceMiniSetRandom(CryptPattern72, rndper);
	PlaceMiniSetRandom(CryptPattern73, rndper);
	PlaceMiniSetRandom(CryptPattern74, rndper);
	PlaceMiniSetRandom(CryptPattern75, rndper);
	PlaceMiniSetRandom(CryptPattern76, rndper);
	PlaceMiniSetRandom(CryptPattern77, rndper);
	PlaceMiniSetRandom(CryptPattern78, rndper);
	PlaceMiniSetRandom(CryptPattern79, rndper);
}

void CryptPatternGroup4(int rndper)
{
	PlaceMiniSetRandom(CryptPattern80, rndper);
	PlaceMiniSetRandom(CryptPattern81, rndper);
	PlaceMiniSetRandom(CryptPattern82, rndper);
	PlaceMiniSetRandom(CryptPattern83, rndper);
	PlaceMiniSetRandom(CryptPattern84, rndper);
	PlaceMiniSetRandom(CryptPattern85, rndper);
	PlaceMiniSetRandom(CryptPattern86, rndper);
	PlaceMiniSetRandom(CryptPattern87, rndper);
	PlaceMiniSetRandom(CryptPattern88, rndper);
	PlaceMiniSetRandom(CryptPattern89, rndper);
	PlaceMiniSetRandom(CryptPattern90, rndper);
	PlaceMiniSetRandom(CryptPattern91, rndper);
	PlaceMiniSetRandom(CryptPattern92, rndper);
	PlaceMiniSetRandom(CryptPattern93, rndper);
	PlaceMiniSetRandom(CryptPattern94, rndper);
	PlaceMiniSetRandom(CryptPattern95, rndper);
	PlaceMiniSetRandom(CryptPattern96, rndper);
}

void CryptPatternGroup5(int rndper)
{
	PlaceMiniSetRandom(CryptPattern36, rndper);
	PlaceMiniSetRandom(CryptPattern37, rndper);
	PlaceMiniSetRandom(CryptPattern38, rndper);
	PlaceMiniSetRandom(CryptPattern39, rndper);
	PlaceMiniSetRandom(CryptPattern40, rndper);
	PlaceMiniSetRandom(CryptPattern41, rndper);
	PlaceMiniSetRandom(CryptPattern42, rndper);
	PlaceMiniSetRandom(CryptPattern43, rndper);
	PlaceMiniSetRandom(CryptPattern44, rndper);
	PlaceMiniSetRandom(CryptPattern45, rndper);
}

void CryptPatternGroup6(int rndper)
{
	PlaceMiniSetRandom(CryptPattern10, rndper);
	PlaceMiniSetRandom(CryptPattern12, rndper);
	PlaceMiniSetRandom(CryptPattern11, rndper);
	PlaceMiniSetRandom(CryptPattern13, rndper);
	PlaceMiniSetRandom(CryptPattern14, rndper);
	PlaceMiniSetRandom(CryptPattern15, rndper);
	PlaceMiniSetRandom(CryptPattern16, rndper);
	PlaceMiniSetRandom(CryptPattern17, rndper);
	PlaceMiniSetRandom(CryptPattern18, rndper);
	PlaceMiniSetRandom(CryptPattern19, rndper);
	PlaceMiniSetRandom(CryptPattern20, rndper);
	PlaceMiniSetRandom(CryptPattern21, rndper);
	PlaceMiniSetRandom(CryptPattern22, rndper);
	PlaceMiniSetRandom(CryptPattern23, rndper);
	PlaceMiniSetRandom(CryptPattern24, rndper);
	PlaceMiniSetRandom(CryptPattern25, rndper);
	PlaceMiniSetRandom(CryptPattern26, rndper);
	PlaceMiniSetRandom(CryptPattern27, rndper);
	PlaceMiniSetRandom(CryptPattern28, rndper);
	PlaceMiniSetRandom(CryptPattern29, rndper);
	PlaceMiniSetRandom(CryptPattern30, rndper);
	PlaceMiniSetRandom(CryptPattern31, rndper);
	PlaceMiniSetRandom(CryptPattern32, rndper);
	PlaceMiniSetRandom(CryptPattern33, rndper);
	PlaceMiniSetRandom(CryptPattern34, rndper);
	PlaceMiniSetRandom(CryptPattern35, rndper);
}

void CryptPatternGroup7(int rndper)
{
	PlaceMiniSetRandom(CryptPattern5, rndper);
	PlaceMiniSetRandom(CryptPattern6, rndper);
	PlaceMiniSetRandom(CryptPattern7, rndper);
	PlaceMiniSetRandom(CryptPattern8, rndper);
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

	bool doneflag;
	do {
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
		ClearFlags();
		FloodTransparancyValues();

		doneflag = true;

		if (QuestStatus(Q_PWATER)) {
			if (entry == ENTRY_MAIN) {
				if (PlaceMiniSet(PWATERIN, 1, 1, 0, 0, true, -1) < 0)
					doneflag = false;
			} else {
				if (PlaceMiniSet(PWATERIN, 1, 1, 0, 0, false, -1) < 0)
					doneflag = false;
				ViewY--;
			}
		}
		if (QuestStatus(Q_LTBANNER)) {
			if (entry == ENTRY_MAIN) {
				if (PlaceMiniSet(STAIRSUP, 1, 1, 0, 0, true, -1) < 0)
					doneflag = false;
			} else {
				if (PlaceMiniSet(STAIRSUP, 1, 1, 0, 0, false, -1) < 0)
					doneflag = false;
				if (entry == ENTRY_PREV) {
					ViewX = 2 * setpc_x + 20;
					ViewY = 2 * setpc_y + 28;
				} else {
					ViewY--;
				}
			}
		} else if (entry == ENTRY_MAIN) {
			if (currlevel < 21) {
				if (!Players[MyPlayerId].pOriginalCathedral) {
					if (PlaceMiniSet(STAIRSUP, 1, 1, 0, 0, true, -1) < 0)
						doneflag = false;
					if (PlaceMiniSet(STAIRSDOWN, 1, 1, 0, 0, false, -1) < 0)
						doneflag = false;
				} else {
					if (PlaceMiniSet(L5STAIRSUP, 1, 1, 0, 0, true, -1) < 0)
						doneflag = false;
					else if (PlaceMiniSet(STAIRSDOWN, 1, 1, 0, 0, false, -1) < 0)
						doneflag = false;
				}
			} else if (currlevel == 21) {
				if (PlaceMiniSet(L5STAIRSTOWN, 1, 1, 0, 0, false, -1) < 0)
					doneflag = false;
				if (PlaceMiniSet(L5STAIRSDOWN, 1, 1, 0, 0, false, -1) < 0)
					doneflag = false;
				ViewY++;
			} else {
				if (PlaceMiniSet(L5STAIRSUPHF, 1, 1, 0, 0, true, -1) < 0)
					doneflag = false;
				if (currlevel != 24) {
					if (PlaceMiniSet(L5STAIRSDOWN, 1, 1, 0, 0, false, -1) < 0)
						doneflag = false;
				}
				ViewY++;
			}
		} else if (!Players[MyPlayerId].pOriginalCathedral && entry == ENTRY_PREV) {
			if (currlevel < 21) {
				if (PlaceMiniSet(STAIRSUP, 1, 1, 0, 0, false, -1) < 0)
					doneflag = false;
				if (PlaceMiniSet(STAIRSDOWN, 1, 1, 0, 0, true, -1) < 0)
					doneflag = false;
				ViewY--;
			} else if (currlevel == 21) {
				if (PlaceMiniSet(L5STAIRSTOWN, 1, 1, 0, 0, false, -1) < 0)
					doneflag = false;
				if (PlaceMiniSet(L5STAIRSDOWN, 1, 1, 0, 0, true, -1) < 0)
					doneflag = false;
				ViewY += 3;
			} else {
				if (PlaceMiniSet(L5STAIRSUPHF, 1, 1, 0, 0, true, -1) < 0)
					doneflag = false;
				if (currlevel != 24) {
					if (PlaceMiniSet(L5STAIRSDOWN, 1, 1, 0, 0, true, -1) < 0)
						doneflag = false;
				}
				ViewY += 3;
			}
		} else {
			if (currlevel < 21) {
				if (!Players[MyPlayerId].pOriginalCathedral) {
					if (PlaceMiniSet(STAIRSUP, 1, 1, 0, 0, false, -1) < 0)
						doneflag = false;
					if (PlaceMiniSet(STAIRSDOWN, 1, 1, 0, 0, false, -1) < 0)
						doneflag = false;
				} else {
					if (PlaceMiniSet(L5STAIRSUP, 1, 1, 0, 0, false, -1) < 0)
						doneflag = false;
					else if (PlaceMiniSet(STAIRSDOWN, 1, 1, 0, 0, true, -1) < 0)
						doneflag = false;
					ViewY--;
				}
			} else if (currlevel == 21) {
				if (PlaceMiniSet(L5STAIRSTOWN, 1, 1, 0, 0, true, -1) < 0)
					doneflag = false;
				if (PlaceMiniSet(L5STAIRSDOWN, 1, 1, 0, 0, false, -1) < 0)
					doneflag = false;
			} else {
				if (PlaceMiniSet(L5STAIRSUPHF, 1, 1, 0, 0, true, -1) < 0)
					doneflag = false;
				if (currlevel != 24) {
					if (PlaceMiniSet(L5STAIRSDOWN, 1, 1, 0, 0, false, -1) < 0)
						doneflag = false;
				}
			}
		}
	} while (!doneflag);

	for (int j = 0; j < DMAXY; j++) {
		for (int i = 0; i < DMAXX; i++) {
			if (dungeon[i][j] == 64) {
				int xx = 2 * i + 16; /* todo: fix loop */
				int yy = 2 * j + 16;
				DRLG_CopyTrans(xx, yy + 1, xx, yy);
				DRLG_CopyTrans(xx + 1, yy + 1, xx + 1, yy);
			}
		}
	}

	FixTransparency();
	FixDirtTiles();
	FixCornerTiles();

	for (int j = 0; j < DMAXY; j++) {
		for (int i = 0; i < DMAXX; i++) {
			if ((L5dflags[i][j] & ~DLRG_PROTECTED) != 0)
				PlaceDoor(i, j);
		}
	}

	if (currlevel < 21) {
		Substitution();
	} else {
		CryptPatternGroup1(10);
		PlaceMiniSetRandom(CryptPattern1, 95);
		PlaceMiniSetRandom(CryptPattern2, 95);
		PlaceMiniSetRandom(CryptPattern3, 100);
		PlaceMiniSetRandom(CryptPattern4, 100);
		PlaceMiniSetRandom(CryptPattern9, 60);
		CryptLavafloor();
		switch (currlevel) {
		case 21:
			CryptPatternGroup2(30);
			CryptPatternGroup3(15);
			CryptPatternGroup4(5);
			CryptLavafloor();
			CryptPatternGroup7(10);
			CryptPatternGroup6(5);
			CryptPatternGroup5(20);
			break;
		case 22:
			CryptPatternGroup7(10);
			CryptPatternGroup6(10);
			CryptPatternGroup5(20);
			CryptPatternGroup2(30);
			CryptPatternGroup3(20);
			CryptPatternGroup4(10);
			CryptLavafloor();
			break;
		case 23:
			CryptPatternGroup7(10);
			CryptPatternGroup6(15);
			CryptPatternGroup5(30);
			CryptPatternGroup2(30);
			CryptPatternGroup3(20);
			CryptPatternGroup4(15);
			CryptLavafloor();
			break;
		default:
			CryptPatternGroup7(10);
			CryptPatternGroup6(20);
			CryptPatternGroup5(30);
			CryptPatternGroup2(30);
			CryptPatternGroup3(20);
			CryptPatternGroup4(20);
			CryptLavafloor();
			break;
		}
	}

	if (currlevel < 21) {
		ApplyShadowsPatterns();
		PlaceMiniSet(LAMPS, 5, 10, 0, 0, false, -1);
		FillFloor();
	}

	for (int j = 0; j < DMAXY; j++) {
		for (int i = 0; i < DMAXX; i++) {
			pdungeon[i][j] = dungeon[i][j];
		}
	}

	DRLG_Init_Globals();
	DRLG_CheckQuests(setpc_x, setpc_y);
}

void Pass3()
{
	DRLG_LPass3(22 - 1);
}

} // namespace

void LoadL1Dungeon(const char *path, int vx, int vy)
{
	dminx = 16;
	dminy = 16;
	dmaxx = 96;
	dmaxy = 96;

	DRLG_InitTrans();

	for (int j = 0; j < DMAXY; j++) {
		for (int i = 0; i < DMAXX; i++) {
			dungeon[i][j] = 22;
			L5dflags[i][j] = 0;
		}
	}

	auto dunData = LoadFileInMem<uint16_t>(path);

	int width = SDL_SwapLE16(dunData[0]);
	int height = SDL_SwapLE16(dunData[1]);

	const uint16_t *tileLayer = &dunData[2];

	for (int j = 0; j < height; j++) {
		for (int i = 0; i < width; i++) {
			uint8_t tileId = SDL_SwapLE16(*tileLayer);
			tileLayer++;
			if (tileId != 0) {
				dungeon[i][j] = tileId;
				L5dflags[i][j] |= DLRG_PROTECTED;
			} else {
				dungeon[i][j] = 13;
			}
		}
	}

	FillFloor();

	ViewX = vx;
	ViewY = vy;

	Pass3();
	DRLG_Init_Globals();

	if (currlevel < 17)
		InitDungeonPieces();

	SetMapMonsters(dunData.get(), { 0, 0 });
	SetMapObjects(dunData.get(), 0, 0);
}

void LoadPreL1Dungeon(const char *path)
{
	for (int j = 0; j < DMAXY; j++) {
		for (int i = 0; i < DMAXX; i++) {
			dungeon[i][j] = 22;
			L5dflags[i][j] = 0;
		}
	}

	dminx = 16;
	dminy = 16;
	dmaxx = 96;
	dmaxy = 96;

	auto dunData = LoadFileInMem<uint16_t>(path);

	int width = SDL_SwapLE16(dunData[0]);
	int height = SDL_SwapLE16(dunData[1]);

	const uint16_t *tileLayer = &dunData[2];

	for (int j = 0; j < height; j++) {
		for (int i = 0; i < width; i++) {
			uint8_t tileId = SDL_SwapLE16(*tileLayer);
			tileLayer++;
			if (tileId != 0) {
				dungeon[i][j] = tileId;
				L5dflags[i][j] |= DLRG_PROTECTED;
			} else {
				dungeon[i][j] = 13;
			}
		}
	}

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

	dminx = 16;
	dminy = 16;
	dmaxx = 96;
	dmaxy = 96;

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

	if (currlevel < 17) {
		InitDungeonPieces();
	} else {
		InitCryptPieces();
	}

	DRLG_SetPC();

	for (int j = dminy; j < dmaxy; j++) {
		for (int i = dminx; i < dmaxx; i++) {
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

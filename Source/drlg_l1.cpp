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
#include "lighting.h"
#include "player.h"
#include "quests.h"

namespace devilution {

int UberRow;
int UberCol;
bool IsUberRoomOpened;
int UberLeverRow;
int UberLeverCol;
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

} // namespace

void DRLG_InitL5Vals()
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

static void DrlgPlaceDoor(int x, int y)
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

void drlg_l1_crypt_lavafloor()
{
	for (int j = 1; j < 40; j++) {
		for (int i = 1; i < 40; i++) {
			switch (dungeon[i][j]) {
			case 5:
				if (dungeon[i - 1][j] == 13)
					dungeon[i - 1][j] = 203;
				if (dungeon[i - 1][j - 1] == 13)
					dungeon[i - 1][j - 1] = 204;
				if (dungeon[i][j - 1] == 13)
					dungeon[i][j - 1] = 205;
				break;
			case 7:
				if (dungeon[i - 1][j] == 13)
					dungeon[i - 1][j] = 206;
				if (dungeon[i - 1][j - 1] == 13)
					dungeon[i - 1][j - 1] = 207;
				break;
			case 8:
				if (dungeon[i - 1][j] == 13)
					dungeon[i - 1][j] = 203;
				if (dungeon[i - 1][j - 1] == 13)
					dungeon[i - 1][j - 1] = 204;
				break;
			case 9:
				if (dungeon[i - 1][j] == 13)
					dungeon[i - 1][j] = 206;
				if (dungeon[i - 1][j - 1] == 13)
					dungeon[i - 1][j - 1] = 207;
				if (dungeon[i][j - 1] == 13)
					dungeon[i][j - 1] = 205;
				break;
			case 10:
				if (dungeon[i][j - 1] == 13)
					dungeon[i][j - 1] = 205;
				break;
			case 11:
				if (dungeon[i - 1][j] == 13)
					dungeon[i - 1][j] = 203;
				if (dungeon[i - 1][j - 1] == 13)
					dungeon[i - 1][j - 1] = 204;
				break;
			case 12:
				if (dungeon[i][j - 1] == 13)
					dungeon[i][j - 1] = 205;
				break;
			case 14:
				if (dungeon[i - 1][j] == 13)
					dungeon[i - 1][j] = 203;
				if (dungeon[i - 1][j - 1] == 13)
					dungeon[i - 1][j - 1] = 204;
				break;
			case 15:
				if (dungeon[i - 1][j] == 13)
					dungeon[i - 1][j] = 206;
				if (dungeon[i - 1][j - 1] == 13)
					dungeon[i - 1][j - 1] = 207;
				break;
			case 17:
				if (dungeon[i - 1][j] == 13)
					dungeon[i - 1][j] = 206;
				if (dungeon[i - 1][j - 1] == 13)
					dungeon[i - 1][j - 1] = 207;
				break;
			case 95:
				if (dungeon[i - 1][j] == 13)
					dungeon[i - 1][j] = 203;
				if (dungeon[i - 1][j - 1] == 13)
					dungeon[i - 1][j - 1] = 204;
				break;
			case 96:
				if (dungeon[i][j - 1] == 13)
					dungeon[i][j - 1] = 208;
				break;
			case 116:
				if (dungeon[i - 1][j] == 13)
					dungeon[i - 1][j] = 203;
				if (dungeon[i - 1][j - 1] == 13)
					dungeon[i - 1][j - 1] = 204;
				if (dungeon[i][j - 1] == 13)
					dungeon[i][j - 1] = 205;
				break;
			case 118:
				if (dungeon[i - 1][j] == 13)
					dungeon[i - 1][j] = 206;
				if (dungeon[i - 1][j - 1] == 13)
					dungeon[i - 1][j - 1] = 207;
				break;
			case 119:
				if (dungeon[i - 1][j] == 13)
					dungeon[i - 1][j] = 203;
				if (dungeon[i - 1][j - 1] == 13)
					dungeon[i - 1][j - 1] = 204;
				break;
			case 120:
				if (dungeon[i - 1][j] == 13)
					dungeon[i - 1][j] = 206;
				if (dungeon[i - 1][j - 1] == 13)
					dungeon[i - 1][j - 1] = 207;
				if (dungeon[i][j - 1] == 13)
					dungeon[i][j - 1] = 205;
				break;
			case 121:
				if (dungeon[i][j - 1] == 13)
					dungeon[i][j - 1] = 205;
				break;
			case 122:
				if (dungeon[i - 1][j] == 13)
					dungeon[i - 1][j] = 211;
				if (dungeon[i - 1][j - 1] == 13)
					dungeon[i - 1][j - 1] = 212;
				break;
			case 123:
				if (dungeon[i][j - 1] == 13)
					dungeon[i][j - 1] = 205;
				break;
			case 125:
				if (dungeon[i - 1][j] == 13)
					dungeon[i - 1][j] = 203;
				if (dungeon[i - 1][j - 1] == 13)
					dungeon[i - 1][j - 1] = 204;
				break;
			case 126:
				if (dungeon[i - 1][j] == 13)
					dungeon[i - 1][j] = 206;
				if (dungeon[i - 1][j - 1] == 13)
					dungeon[i - 1][j - 1] = 207;
				break;
			case 128:
				if (dungeon[i - 1][j] == 13)
					dungeon[i - 1][j] = 206;
				if (dungeon[i - 1][j - 1] == 13)
					dungeon[i - 1][j - 1] = 207;
				break;
			case 133:
				if (dungeon[i - 1][j] == 13)
					dungeon[i - 1][j] = 203;
				if (dungeon[i - 1][j - 1] == 13)
					dungeon[i - 1][j - 1] = 204;
				if (dungeon[i][j - 1] == 13)
					dungeon[i][j - 1] = 205;
				break;
			case 135:
				if (dungeon[i - 1][j] == 13)
					dungeon[i - 1][j] = 206;
				if (dungeon[i - 1][j - 1] == 13)
					dungeon[i - 1][j - 1] = 207;
				break;
			case 136:
				if (dungeon[i - 1][j] == 13)
					dungeon[i - 1][j] = 203;
				if (dungeon[i - 1][j - 1] == 13)
					dungeon[i - 1][j - 1] = 204;
				break;
			case 137:
				if (dungeon[i - 1][j] == 13)
					dungeon[i - 1][j] = 213;
				if (dungeon[i - 1][j - 1] == 13)
					dungeon[i - 1][j - 1] = 214;
				if (dungeon[i][j - 1] == 13)
					dungeon[i][j - 1] = 205;
				break;
			case 138:
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
				if (dungeon[i][j - 1] == 13)
					dungeon[i][j - 1] = 217;
				break;
			case 142:
				if (dungeon[i - 1][j] == 13)
					dungeon[i - 1][j] = 203;
				if (dungeon[i - 1][j - 1] == 13)
					dungeon[i - 1][j - 1] = 204;
				break;
			case 143:
				if (dungeon[i - 1][j] == 13)
					dungeon[i - 1][j] = 213;
				if (dungeon[i - 1][j - 1] == 13)
					dungeon[i - 1][j - 1] = 214;
				break;
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
			case 152:
				if (dungeon[i - 1][j] == 13)
					dungeon[i - 1][j] = 206;
				if (dungeon[i - 1][j - 1] == 13)
					dungeon[i - 1][j - 1] = 207;
				break;
			case 153:
				if (dungeon[i - 1][j] == 13)
					dungeon[i - 1][j] = 203;
				if (dungeon[i - 1][j - 1] == 13)
					dungeon[i - 1][j - 1] = 204;
				break;
			case 154:
				if (dungeon[i - 1][j] == 13)
					dungeon[i - 1][j] = 206;
				if (dungeon[i - 1][j - 1] == 13)
					dungeon[i - 1][j - 1] = 207;
				if (dungeon[i][j - 1] == 13)
					dungeon[i][j - 1] = 205;
				break;
			case 155:
				if (dungeon[i][j - 1] == 13)
					dungeon[i][j - 1] = 205;
				break;
			case 156:
				if (dungeon[i - 1][j] == 13)
					dungeon[i - 1][j] = 203;
				if (dungeon[i - 1][j - 1] == 13)
					dungeon[i - 1][j - 1] = 204;
				break;
			case 157:
				if (dungeon[i][j - 1] == 13)
					dungeon[i][j - 1] = 217;
				break;
			case 159:
				if (dungeon[i - 1][j] == 13)
					dungeon[i - 1][j] = 203;
				if (dungeon[i - 1][j - 1] == 13)
					dungeon[i - 1][j - 1] = 204;
				break;
			case 160:
				if (dungeon[i - 1][j] == 13)
					dungeon[i - 1][j] = 206;
				if (dungeon[i - 1][j - 1] == 13)
					dungeon[i - 1][j - 1] = 207;
				break;
			case 162:
				if (dungeon[i - 1][j] == 13)
					dungeon[i - 1][j] = 209;
				if (dungeon[i - 1][j - 1] == 13)
					dungeon[i - 1][j - 1] = 210;
				break;
			case 167:
				if (dungeon[i - 1][j] == 13)
					dungeon[i - 1][j] = 209;
				if (dungeon[i - 1][j - 1] == 13)
					dungeon[i - 1][j - 1] = 210;
				break;
			case 187:
				if (dungeon[i][j - 1] == 13)
					dungeon[i][j - 1] = 208;
				break;
			case 185:
				if (dungeon[i - 1][j] == 13)
					dungeon[i - 1][j] = 203;
				if (dungeon[i - 1][j - 1] == 13)
					dungeon[i - 1][j - 1] = 204;
				break;
			case 186:
				if (dungeon[i - 1][j] == 13)
					dungeon[i - 1][j] = 203;
				if (dungeon[i - 1][j - 1] == 13)
					dungeon[i - 1][j - 1] = 204;
				break;
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

static void DrlgL1Shadows()
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

				if (shadow.nv1 != 0 && L5dflags[x - 1][y - 1] == 0)
					dungeon[x - 1][y - 1] = shadow.nv1;
				if (shadow.nv2 != 0 && L5dflags[x][y - 1] == 0)
					dungeon[x][y - 1] = shadow.nv2;
				if (shadow.nv3 != 0 && L5dflags[x - 1][y] == 0)
					dungeon[x - 1][y] = shadow.nv3;
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

static int DrlgPlaceMiniSet(const BYTE *miniset, int tmin, int tmax, int cx, int cy, bool setview, int noquad, int ldir)
{
	int sx;
	int sy;

	int sw = miniset[0];
	int sh = miniset[1];

	int numt = 1;
	if (tmax - tmin != 0)
		numt = GenerateRnd(tmax - tmin) + tmin;

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
				if (++sx == DMAXX - sw) {
					sx = 0;
					if (++sy == DMAXY - sh)
						sy = 0;
				}
				if (++found > 4000)
					return -1;
			}
		}

		int ii = sw * sh + 2;

		for (int yy = 0; yy < sh; yy++) {
			for (int xx = 0; xx < sw; xx++) {
				if (miniset[ii] != 0)
					dungeon[xx + sx][sy + yy] = miniset[ii];
				ii++;
			}
		}
	}

	if (miniset == PWATERIN) {
		int t = TransVal;
		TransVal = 0;
		DRLG_MRectTrans(sx, sy + 2, sx + 5, sy + 4);
		TransVal = t;

		quests[Q_PWATER].position = { 2 * sx + 21, 2 * sy + 22 };
	}

	if (setview) {
		ViewX = 2 * sx + 19;
		ViewY = 2 * sy + 20;
	}

	if (ldir == 0) {
		LvlViewX = 2 * sx + 19;
		LvlViewY = 2 * sy + 20;
	}

	if (sx < cx && sy < cy)
		return 0;
	if (sx > cx && sy < cy)
		return 1;
	if (sx < cx && sy > cy)
		return 2;

	return 3;
}

static void DrlgL1Floor()
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

void DRLG_LPass3(int lv)
{
	MegaTile mega = pMegaTiles[lv];
	int v1 = SDL_SwapLE16(mega.micro1) + 1;
	int v2 = SDL_SwapLE16(mega.micro2) + 1;
	int v3 = SDL_SwapLE16(mega.micro3) + 1;
	int v4 = SDL_SwapLE16(mega.micro4) + 1;

	for (int j = 0; j < MAXDUNY; j += 2) {
		for (int i = 0; i < MAXDUNX; i += 2) {
			dPiece[i + 0][j + 0] = v1;
			dPiece[i + 1][j + 0] = v2;
			dPiece[i + 0][j + 1] = v3;
			dPiece[i + 1][j + 1] = v4;
		}
	}

	int yy = 16;
	for (int j = 0; j < DMAXY; j++) {
		int xx = 16;
		for (int i = 0; i < DMAXX; i++) { // NOLINT(modernize-loop-convert)
			v1 = 0;
			v2 = 0;
			v3 = 0;
			v4 = 0;

			int tileId = dungeon[i][j] - 1;
			if (tileId >= 0) {
				MegaTile mega = pMegaTiles[tileId];
				v1 = SDL_SwapLE16(mega.micro1) + 1;
				v2 = SDL_SwapLE16(mega.micro2) + 1;
				v3 = SDL_SwapLE16(mega.micro3) + 1;
				v4 = SDL_SwapLE16(mega.micro4) + 1;
			}
			dPiece[xx + 0][yy + 0] = v1;
			dPiece[xx + 1][yy + 0] = v2;
			dPiece[xx + 0][yy + 1] = v3;
			dPiece[xx + 1][yy + 1] = v4;
			xx += 2;
		}
		yy += 2;
	}
}

static void DrlgL1Pass3()
{
	DRLG_LPass3(22 - 1);
}

static void DrlgLoadL1Sp()
{
	L5setloadflag = false;
	if (QuestStatus(Q_BUTCHER)) {
		L5pSetPiece = LoadFileInMem<uint16_t>("Levels\\L1Data\\rnd6.DUN");
		L5setloadflag = true;
	}
	if (QuestStatus(Q_SKELKING) && !gbIsMultiplayer) {
		L5pSetPiece = LoadFileInMem<uint16_t>("Levels\\L1Data\\SKngDO.DUN");
		L5setloadflag = true;
	}
	if (QuestStatus(Q_LTBANNER)) {
		L5pSetPiece = LoadFileInMem<uint16_t>("Levels\\L1Data\\Banner2.DUN");
		L5setloadflag = true;
	}
}

static void DrlgFreeL1Sp()
{
	L5pSetPiece = nullptr;
}

void DRLG_Init_Globals()
{
	memset(dFlags, 0, sizeof(dFlags));
	memset(dPlayer, 0, sizeof(dPlayer));
	memset(dMonster, 0, sizeof(dMonster));
	memset(dDead, 0, sizeof(dDead));
	memset(dObject, 0, sizeof(dObject));
	memset(dItem, 0, sizeof(dItem));
	memset(dMissile, 0, sizeof(dMissile));
	memset(dSpecial, 0, sizeof(dSpecial));
	int8_t c = lightflag ? 0 : 15;
	memset(dLight, c, sizeof(dLight));
}

static void DrlgInitL1Vals()
{
	int pc;

	for (int j = 0; j < MAXDUNY; j++) {
		for (int i = 0; i < MAXDUNX; i++) {
			if (dPiece[i][j] == 12) {
				pc = 1;
			} else if (dPiece[i][j] == 11) {
				pc = 2;
			} else if (dPiece[i][j] == 71) {
				pc = 1;
			} else if (dPiece[i][j] == 253) {
				pc = 3;
			} else if (dPiece[i][j] == 267) {
				pc = 6;
			} else if (dPiece[i][j] == 259) {
				pc = 5;
			} else if (dPiece[i][j] == 249) {
				pc = 2;
			} else if (dPiece[i][j] == 325) {
				pc = 2;
			} else if (dPiece[i][j] == 321) {
				pc = 1;
			} else if (dPiece[i][j] == 255) {
				pc = 4;
			} else if (dPiece[i][j] == 211) {
				pc = 1;
			} else if (dPiece[i][j] == 344) {
				pc = 2;
			} else if (dPiece[i][j] == 341) {
				pc = 1;
			} else if (dPiece[i][j] == 331) {
				pc = 2;
			} else if (dPiece[i][j] == 418) {
				pc = 1;
			} else if (dPiece[i][j] == 421) {
				pc = 2;
			} else {
				continue;
			}
			dSpecial[i][j] = pc;
		}
	}
}

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

	DrlgL1Floor();

	ViewX = vx;
	ViewY = vy;

	DrlgL1Pass3();
	DRLG_Init_Globals();

	if (currlevel < 17)
		DrlgInitL1Vals();

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

	DrlgL1Floor();

	for (int j = 0; j < DMAXY; j++) {
		for (int i = 0; i < DMAXX; i++) {
			pdungeon[i][j] = dungeon[i][j];
		}
	}
}

static void InitL5Dungeon()
{
	for (int j = 0; j < DMAXY; j++) {
		for (int i = 0; i < DMAXX; i++) {
			dungeon[i][j] = 0;
			L5dflags[i][j] = 0;
		}
	}
}

static void L5ClearFlags()
{
	for (int j = 0; j < DMAXY; j++) {
		for (int i = 0; i < DMAXX; i++) { // NOLINT(modernize-loop-convert)
			L5dflags[i][j] &= ~DLRG_CHAMBER;
		}
	}
}

static void L5drawRoom(int x, int y, int w, int h)
{
	for (int j = 0; j < h; j++) {
		for (int i = 0; i < w; i++) {
			dungeon[x + i][y + j] = 1;
		}
	}
}

static bool L5checkRoom(int x, int y, int width, int height)
{
	for (int j = 0; j < height; j++) {
		for (int i = 0; i < width; i++) {
			if (i + x < 0 || i + x >= DMAXX || j + y < 0 || j + y >= DMAXY)
				return false;
			if (dungeon[i + x][j + y] != 0)
				return false;
		}
	}

	return true;
}

static void L5roomGen(int x, int y, int w, int h, int dir)
{
	int dirProb = GenerateRnd(4);
	int num = 0;

	bool ran;
	bool ran2;
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
			ran = L5checkRoom(cx1 - 1, cy1 - 1, ch + 2, cw + 1); /// BUGFIX: swap args 3 and 4 ("ch+2" and "cw+1")
			num++;
		} while (!ran && num < 20);

		if (ran)
			L5drawRoom(cx1, cy1, cw, ch);
		int cx2 = x + w;
		ran2 = L5checkRoom(cx2, cy1 - 1, cw + 1, ch + 2);
		if (ran2)
			L5drawRoom(cx2, cy1, cw, ch);
		if (ran)
			L5roomGen(cx1, cy1, cw, ch, 1);
		if (ran2)
			L5roomGen(cx2, cy1, cw, ch, 1);
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
		ran = L5checkRoom(rx - 1, ry - 1, width + 2, height + 1);
		num++;
	} while (!ran && num < 20);

	if (ran)
		L5drawRoom(rx, ry, width, height);
	int ry2 = y + h;
	ran2 = L5checkRoom(rx - 1, ry2, width + 2, height + 1);
	if (ran2)
		L5drawRoom(rx, ry2, width, height);
	if (ran)
		L5roomGen(rx, ry, width, height, 0);
	if (ran2)
		L5roomGen(rx, ry2, width, height, 0);
}

static void L5firstRoom()
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
			L5drawRoom(15, 1, 10, 10);
		else
			ys = 18;

		if (VR2)
			L5drawRoom(15, 15, 10, 10);
		if (VR3)
			L5drawRoom(15, 29, 10, 10);
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
			L5roomGen(15, 1, 10, 10, 0);
		if (VR2)
			L5roomGen(15, 15, 10, 10, 0);
		if (VR3)
			L5roomGen(15, 29, 10, 10, 0);

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
			L5drawRoom(1, 15, 10, 10);
		else
			xs = 18;

		if (HR2)
			L5drawRoom(15, 15, 10, 10);
		if (HR3)
			L5drawRoom(29, 15, 10, 10);
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
			L5roomGen(1, 15, 10, 10, 1);
		if (HR2)
			L5roomGen(15, 15, 10, 10, 1);
		if (HR3)
			L5roomGen(29, 15, 10, 10, 1);

		VR3 = false;
		VR2 = false;
		VR1 = false;
	}
}

static int L5GetArea()
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

static void L5makeDungeon()
{
	for (int j = 0; j < DMAXY; j++) {
		for (int i = 0; i < DMAXX; i++) {
			int i_2 = i * 2;
			int j_2 = j * 2;
			L5dungeon[i_2][j_2] = dungeon[i][j];
			L5dungeon[i_2][j_2 + 1] = dungeon[i][j];
			L5dungeon[i_2 + 1][j_2] = dungeon[i][j];
			L5dungeon[i_2 + 1][j_2 + 1] = dungeon[i][j];
		}
	}
}

static void L5makeDmt()
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

static int L5HWallOk(int i, int j)
{
	int x;
	bool wallok;

	for (x = 1; dungeon[i + x][j] == 13; x++) {
		if (dungeon[i + x][j - 1] != 13 || dungeon[i + x][j + 1] != 13 || L5dflags[i + x][j] != 0)
			break;
	}

	wallok = false;
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

static int L5VWallOk(int i, int j)
{
	int y;
	bool wallok;

	for (y = 1; dungeon[i][j + y] == 13; y++) {
		if (dungeon[i - 1][j + y] != 13 || dungeon[i + 1][j + y] != 13 || L5dflags[i][j + y] != 0)
			break;
	}

	wallok = false;
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

static void L5HorizWall(int i, int j, char p, int dx)
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

static void L5VertWall(int i, int j, char p, int dy)
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

static void L5AddWall()
{
	for (int j = 0; j < DMAXY; j++) {
		for (int i = 0; i < DMAXX; i++) {
			if (L5dflags[i][j] == 0) {
				if (dungeon[i][j] == 3 && GenerateRnd(100) < WALL_CHANCE) {
					int x = L5HWallOk(i, j);
					if (x != -1)
						L5HorizWall(i, j, 2, x);
				}
				if (dungeon[i][j] == 3 && GenerateRnd(100) < WALL_CHANCE) {
					int y = L5VWallOk(i, j);
					if (y != -1)
						L5VertWall(i, j, 1, y);
				}
				if (dungeon[i][j] == 6 && GenerateRnd(100) < WALL_CHANCE) {
					int x = L5HWallOk(i, j);
					if (x != -1)
						L5HorizWall(i, j, 4, x);
				}
				if (dungeon[i][j] == 7 && GenerateRnd(100) < WALL_CHANCE) {
					int y = L5VWallOk(i, j);
					if (y != -1)
						L5VertWall(i, j, 4, y);
				}
				if (dungeon[i][j] == 2 && GenerateRnd(100) < WALL_CHANCE) {
					int x = L5HWallOk(i, j);
					if (x != -1)
						L5HorizWall(i, j, 2, x);
				}
				if (dungeon[i][j] == 1 && GenerateRnd(100) < WALL_CHANCE) {
					int y = L5VWallOk(i, j);
					if (y != -1)
						L5VertWall(i, j, 1, y);
				}
			}
		}
	}
}

static void DrlgL5GChamber(int sx, int sy, bool topflag, bool bottomflag, bool leftflag, bool rightflag)
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

static void DrlgL5GHall(int x1, int y1, int x2, int y2)
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

static void L5tileFix()
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

void drlg_l1_crypt_rndset(const BYTE *miniset, int rndper)
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

static void DrlgL5Subs()
{
	for (int y = 0; y < DMAXY; y++) {
		for (int x = 0; x < DMAXX; x++) {
			if (GenerateRnd(4) == 0) {
				BYTE c = L5BTYPES[dungeon[x][y]];

				if (c != 0 && L5dflags[x][y] == 0) {
					int rv = GenerateRnd(16);
					int i = -1;

					while (rv >= 0) {
						if (++i == sizeof(L5BTYPES))
							i = 0;
						if (c == L5BTYPES[i])
							rv--;
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

static void DrlgL5SetRoom(int rx1, int ry1)
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

static void L5FillChambers()
{
	if (HR1)
		DrlgL5GChamber(0, 14, false, false, false, true);

	if (HR2) {
		if (HR1 && !HR3)
			DrlgL5GChamber(14, 14, false, false, true, false);
		if (!HR1 && HR3)
			DrlgL5GChamber(14, 14, false, false, false, true);
		if (HR1 && HR3)
			DrlgL5GChamber(14, 14, false, false, true, true);
		if (!HR1 && !HR3)
			DrlgL5GChamber(14, 14, false, false, false, false);
	}

	if (HR3)
		DrlgL5GChamber(28, 14, false, false, true, false);
	if (HR1 && HR2)
		DrlgL5GHall(12, 18, 14, 18);
	if (HR2 && HR3)
		DrlgL5GHall(26, 18, 28, 18);
	if (HR1 && !HR2 && HR3)
		DrlgL5GHall(12, 18, 28, 18);
	if (VR1)
		DrlgL5GChamber(14, 0, false, true, false, false);

	if (VR2) {
		if (VR1 && !VR3)
			DrlgL5GChamber(14, 14, true, false, false, false);
		if (!VR1 && VR3)
			DrlgL5GChamber(14, 14, false, true, false, false);
		if (VR1 && VR3)
			DrlgL5GChamber(14, 14, true, true, false, false);
		if (!VR1 && !VR3)
			DrlgL5GChamber(14, 14, false, false, false, false);
	}

	if (VR3)
		DrlgL5GChamber(14, 28, true, false, false, false);
	if (VR1 && VR2)
		DrlgL5GHall(18, 12, 18, 14);
	if (VR2 && VR3)
		DrlgL5GHall(18, 26, 18, 28);
	if (VR1 && !VR2 && VR3)
		DrlgL5GHall(18, 12, 18, 28);

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
				drlg_l1_set_crypt_room(16, 2);
				break;
			case 1:
				drlg_l1_set_crypt_room(16, 16);
				break;
			case 2:
				drlg_l1_set_crypt_room(16, 30);
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
				drlg_l1_set_crypt_room(2, 16);
				break;
			case 1:
				drlg_l1_set_crypt_room(16, 16);
				break;
			case 2:
				drlg_l1_set_crypt_room(30, 16);
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
				drlg_l1_set_corner_room(16, 2);
				break;
			case 1:
				drlg_l1_set_corner_room(16, 16);
				break;
			case 2:
				drlg_l1_set_corner_room(16, 30);
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
				drlg_l1_set_corner_room(2, 16);
				break;
			case 1:
				drlg_l1_set_corner_room(16, 16);
				break;
			case 2:
				drlg_l1_set_corner_room(30, 16);
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
				DrlgL5SetRoom(16, 2);
				break;
			case 1:
				DrlgL5SetRoom(16, 16);
				break;
			case 2:
				DrlgL5SetRoom(16, 30);
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
				DrlgL5SetRoom(2, 16);
				break;
			case 1:
				DrlgL5SetRoom(16, 16);
				break;
			case 2:
				DrlgL5SetRoom(30, 16);
				break;
			}
		}
	}
}

void drlg_l1_set_crypt_room(int rx1, int ry1)
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

void drlg_l1_set_corner_room(int rx1, int ry1)
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

static void DrlgL5FTransparencyValueR(int i, int j, int x, int y, int d)
{
	if (dTransVal[x][y] != 0 || dungeon[i][j] != 13) {
		if (d == 1) {
			dTransVal[x][y] = TransVal;
			dTransVal[x][y + 1] = TransVal;
		}
		if (d == 2) {
			dTransVal[x + 1][y] = TransVal;
			dTransVal[x + 1][y + 1] = TransVal;
		}
		if (d == 3) {
			dTransVal[x][y] = TransVal;
			dTransVal[x + 1][y] = TransVal;
		}
		if (d == 4) {
			dTransVal[x][y + 1] = TransVal;
			dTransVal[x + 1][y + 1] = TransVal;
		}
		if (d == 5)
			dTransVal[x + 1][y + 1] = TransVal;
		if (d == 6)
			dTransVal[x][y + 1] = TransVal;
		if (d == 7)
			dTransVal[x + 1][y] = TransVal;
		if (d == 8)
			dTransVal[x][y] = TransVal;
		return;
	}

	dTransVal[x][y] = TransVal;
	dTransVal[x + 1][y] = TransVal;
	dTransVal[x][y + 1] = TransVal;
	dTransVal[x + 1][y + 1] = TransVal;
	DrlgL5FTransparencyValueR(i + 1, j, x + 2, y, 1);
	DrlgL5FTransparencyValueR(i - 1, j, x - 2, y, 2);
	DrlgL5FTransparencyValueR(i, j + 1, x, y + 2, 3);
	DrlgL5FTransparencyValueR(i, j - 1, x, y - 2, 4);
	DrlgL5FTransparencyValueR(i - 1, j - 1, x - 2, y - 2, 5);
	DrlgL5FTransparencyValueR(i + 1, j - 1, x + 2, y - 2, 6);
	DrlgL5FTransparencyValueR(i - 1, j + 1, x - 2, y + 2, 7);
	DrlgL5FTransparencyValueR(i + 1, j + 1, x + 2, y + 2, 8);
}

static void DrlgL5FloodTVal()
{
	int yy = 16;

	for (int j = 0; j < DMAXY; j++) {
		int xx = 16;

		for (int i = 0; i < DMAXX; i++) {
			if (dungeon[i][j] == 13 && dTransVal[xx][yy] == 0) {
				DrlgL5FTransparencyValueR(i, j, xx, yy, 0);
				TransVal++;
			}
			xx += 2;
		}
		yy += 2;
	}
}

static void DrlgL5TransFix()
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

static void DrlgL5DirtFix()
{
	if (currlevel < 21) {
		for (int j = 0; j < DMAXY - 1; j++) {
			for (int i = 0; i < DMAXX - 1; i++) {
				if (dungeon[i][j] == 21 && dungeon[i + 1][j] != 19)
					dungeon[i][j] = 202;
				if (dungeon[i][j] == 19 && dungeon[i + 1][j] != 19)
					dungeon[i][j] = 200;
				if (dungeon[i][j] == 24 && dungeon[i + 1][j] != 19)
					dungeon[i][j] = 205;
				if (dungeon[i][j] == 18 && dungeon[i][j + 1] != 18)
					dungeon[i][j] = 199;
				if (dungeon[i][j] == 21 && dungeon[i][j + 1] != 18)
					dungeon[i][j] = 202;
				if (dungeon[i][j] == 23 && dungeon[i][j + 1] != 18)
					dungeon[i][j] = 204;
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

static void DrlgL5CornerFix()
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

static void DrlgL5(lvl_entry entry)
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
			InitL5Dungeon();
			L5firstRoom();
		} while (L5GetArea() < minarea);

		L5makeDungeon();
		L5makeDmt();
		L5FillChambers();
		L5tileFix();
		L5AddWall();
		L5ClearFlags();
		DrlgL5FloodTVal();

		doneflag = true;

		if (QuestStatus(Q_PWATER)) {
			if (entry == ENTRY_MAIN) {
				if (DrlgPlaceMiniSet(PWATERIN, 1, 1, 0, 0, true, -1, 0) < 0)
					doneflag = false;
			} else {
				if (DrlgPlaceMiniSet(PWATERIN, 1, 1, 0, 0, false, -1, 0) < 0)
					doneflag = false;
				ViewY--;
			}
		}
		if (QuestStatus(Q_LTBANNER)) {
			if (entry == ENTRY_MAIN) {
				if (DrlgPlaceMiniSet(STAIRSUP, 1, 1, 0, 0, true, -1, 0) < 0)
					doneflag = false;
			} else {
				if (DrlgPlaceMiniSet(STAIRSUP, 1, 1, 0, 0, false, -1, 0) < 0)
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
				if (!plr[myplr].pOriginalCathedral) {
					if (DrlgPlaceMiniSet(STAIRSUP, 1, 1, 0, 0, true, -1, 0) < 0)
						doneflag = false;
					if (DrlgPlaceMiniSet(STAIRSDOWN, 1, 1, 0, 0, false, -1, 1) < 0)
						doneflag = false;
				} else {
					if (DrlgPlaceMiniSet(L5STAIRSUP, 1, 1, 0, 0, true, -1, 0) < 0)
						doneflag = false;
					else if (DrlgPlaceMiniSet(STAIRSDOWN, 1, 1, 0, 0, false, -1, 1) < 0)
						doneflag = false;
				}
			} else if (currlevel == 21) {
				if (DrlgPlaceMiniSet(L5STAIRSTOWN, 1, 1, 0, 0, false, -1, 6) < 0)
					doneflag = false;
				if (DrlgPlaceMiniSet(L5STAIRSDOWN, 1, 1, 0, 0, false, -1, 1) < 0)
					doneflag = false;
				ViewY++;
			} else {
				if (DrlgPlaceMiniSet(L5STAIRSUPHF, 1, 1, 0, 0, true, -1, 0) < 0)
					doneflag = false;
				if (currlevel != 24) {
					if (DrlgPlaceMiniSet(L5STAIRSDOWN, 1, 1, 0, 0, false, -1, 1) < 0)
						doneflag = false;
				}
				ViewY++;
			}
		} else if (!plr[myplr].pOriginalCathedral && entry == ENTRY_PREV) {
			if (currlevel < 21) {
				if (DrlgPlaceMiniSet(STAIRSUP, 1, 1, 0, 0, false, -1, 0) < 0)
					doneflag = false;
				if (DrlgPlaceMiniSet(STAIRSDOWN, 1, 1, 0, 0, true, -1, 1) < 0)
					doneflag = false;
				ViewY--;
			} else if (currlevel == 21) {
				if (DrlgPlaceMiniSet(L5STAIRSTOWN, 1, 1, 0, 0, false, -1, 6) < 0)
					doneflag = false;
				if (DrlgPlaceMiniSet(L5STAIRSDOWN, 1, 1, 0, 0, true, -1, 1) < 0)
					doneflag = false;
				ViewY += 3;
			} else {
				if (DrlgPlaceMiniSet(L5STAIRSUPHF, 1, 1, 0, 0, true, -1, 0) < 0)
					doneflag = false;
				if (currlevel != 24) {
					if (DrlgPlaceMiniSet(L5STAIRSDOWN, 1, 1, 0, 0, true, -1, 1) < 0)
						doneflag = false;
				}
				ViewY += 3;
			}
		} else {
			if (currlevel < 21) {
				if (!plr[myplr].pOriginalCathedral) {
					if (DrlgPlaceMiniSet(STAIRSUP, 1, 1, 0, 0, false, -1, 0) < 0)
						doneflag = false;
					if (DrlgPlaceMiniSet(STAIRSDOWN, 1, 1, 0, 0, false, -1, 1) < 0)
						doneflag = false;
				} else {
					if (DrlgPlaceMiniSet(L5STAIRSUP, 1, 1, 0, 0, false, -1, 0) < 0)
						doneflag = false;
					else if (DrlgPlaceMiniSet(STAIRSDOWN, 1, 1, 0, 0, true, -1, 1) < 0)
						doneflag = false;
					ViewY--;
				}
			} else if (currlevel == 21) {
				if (DrlgPlaceMiniSet(L5STAIRSTOWN, 1, 1, 0, 0, true, -1, 6) < 0)
					doneflag = false;
				if (DrlgPlaceMiniSet(L5STAIRSDOWN, 1, 1, 0, 0, false, -1, 1) < 0)
					doneflag = false;
			} else {
				if (DrlgPlaceMiniSet(L5STAIRSUPHF, 1, 1, 0, 0, true, -1, 0) < 0)
					doneflag = false;
				if (currlevel != 24) {
					if (DrlgPlaceMiniSet(L5STAIRSDOWN, 1, 1, 0, 0, false, -1, 1) < 0)
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

	DrlgL5TransFix();
	DrlgL5DirtFix();
	DrlgL5CornerFix();

	for (int j = 0; j < DMAXY; j++) {
		for (int i = 0; i < DMAXX; i++) {
			if ((L5dflags[i][j] & ~DLRG_PROTECTED) != 0)
				DrlgPlaceDoor(i, j);
		}
	}

	if (currlevel < 21) {
		DrlgL5Subs();
	} else {
		drlg_l1_crypt_pattern1(10);
		drlg_l1_crypt_rndset(CryptPattern1, 95);
		drlg_l1_crypt_rndset(CryptPattern2, 95);
		drlg_l1_crypt_rndset(CryptPattern3, 100);
		drlg_l1_crypt_rndset(CryptPattern4, 100);
		drlg_l1_crypt_rndset(CryptPattern9, 60);
		drlg_l1_crypt_lavafloor();
		switch (currlevel) {
		case 21:
			drlg_l1_crypt_pattern2(30);
			drlg_l1_crypt_pattern3(15);
			drlg_l1_crypt_pattern4(5);
			drlg_l1_crypt_lavafloor();
			drlg_l1_crypt_pattern7(10);
			drlg_l1_crypt_pattern6(5);
			drlg_l1_crypt_pattern5(20);
			break;
		case 22:
			drlg_l1_crypt_pattern7(10);
			drlg_l1_crypt_pattern6(10);
			drlg_l1_crypt_pattern5(20);
			drlg_l1_crypt_pattern2(30);
			drlg_l1_crypt_pattern3(20);
			drlg_l1_crypt_pattern4(10);
			drlg_l1_crypt_lavafloor();
			break;
		case 23:
			drlg_l1_crypt_pattern7(10);
			drlg_l1_crypt_pattern6(15);
			drlg_l1_crypt_pattern5(30);
			drlg_l1_crypt_pattern2(30);
			drlg_l1_crypt_pattern3(20);
			drlg_l1_crypt_pattern4(15);
			drlg_l1_crypt_lavafloor();
			break;
		default:
			drlg_l1_crypt_pattern7(10);
			drlg_l1_crypt_pattern6(20);
			drlg_l1_crypt_pattern5(30);
			drlg_l1_crypt_pattern2(30);
			drlg_l1_crypt_pattern3(20);
			drlg_l1_crypt_pattern4(20);
			drlg_l1_crypt_lavafloor();
			break;
		}
	}

	if (currlevel < 21) {
		DrlgL1Shadows();
		DrlgPlaceMiniSet(LAMPS, 5, 10, 0, 0, false, -1, 4);
		DrlgL1Floor();
	}

	for (int j = 0; j < DMAXY; j++) {
		for (int i = 0; i < DMAXX; i++) {
			pdungeon[i][j] = dungeon[i][j];
		}
	}

	DRLG_Init_Globals();
	DRLG_CheckQuests(setpc_x, setpc_y);
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
	UberLeverRow = 0;
	UberLeverCol = 0;
	IsUberLeverActivated = false;
	UberDiabloMonsterIndex = 0;

	DRLG_InitTrans();
	DRLG_InitSetPC();
	DrlgLoadL1Sp();
	DrlgL5(entry);
	DrlgL1Pass3();
	DrlgFreeL1Sp();

	if (currlevel < 17)
		DrlgInitL1Vals();
	else
		DRLG_InitL5Vals();

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

void drlg_l1_crypt_pattern1(int rndper)
{
	drlg_l1_crypt_rndset(CryptPattern97, rndper);
	drlg_l1_crypt_rndset(CryptPattern98, rndper);
	drlg_l1_crypt_rndset(CryptPattern99, rndper);
	drlg_l1_crypt_rndset(CryptPattern100, rndper);
}

void drlg_l1_crypt_pattern2(int rndper)
{
	drlg_l1_crypt_rndset(CryptPattern46, rndper);
	drlg_l1_crypt_rndset(CryptPattern47, rndper);
	drlg_l1_crypt_rndset(CryptPattern48, rndper);
	drlg_l1_crypt_rndset(CryptPattern49, rndper);
	drlg_l1_crypt_rndset(CryptPattern50, rndper);
	drlg_l1_crypt_rndset(CryptPattern51, rndper);
	drlg_l1_crypt_rndset(CryptPattern52, rndper);
	drlg_l1_crypt_rndset(CryptPattern53, rndper);
	drlg_l1_crypt_rndset(CryptPattern54, rndper);
	drlg_l1_crypt_rndset(CryptPattern55, rndper);
	drlg_l1_crypt_rndset(CryptPattern56, rndper);
	drlg_l1_crypt_rndset(CryptPattern57, rndper);
	drlg_l1_crypt_rndset(CryptPattern58, rndper);
	drlg_l1_crypt_rndset(CryptPattern59, rndper);
	drlg_l1_crypt_rndset(CryptPattern60, rndper);
	drlg_l1_crypt_rndset(CryptPattern61, rndper);
	drlg_l1_crypt_rndset(CryptPattern62, rndper);
}

void drlg_l1_crypt_pattern3(int rndper)
{
	drlg_l1_crypt_rndset(CryptPattern63, rndper);
	drlg_l1_crypt_rndset(CryptPattern64, rndper);
	drlg_l1_crypt_rndset(CryptPattern65, rndper);
	drlg_l1_crypt_rndset(CryptPattern66, rndper);
	drlg_l1_crypt_rndset(CryptPattern67, rndper);
	drlg_l1_crypt_rndset(CryptPattern68, rndper);
	drlg_l1_crypt_rndset(CryptPattern69, rndper);
	drlg_l1_crypt_rndset(CryptPattern70, rndper);
	drlg_l1_crypt_rndset(CryptPattern71, rndper);
	drlg_l1_crypt_rndset(CryptPattern72, rndper);
	drlg_l1_crypt_rndset(CryptPattern73, rndper);
	drlg_l1_crypt_rndset(CryptPattern74, rndper);
	drlg_l1_crypt_rndset(CryptPattern75, rndper);
	drlg_l1_crypt_rndset(CryptPattern76, rndper);
	drlg_l1_crypt_rndset(CryptPattern77, rndper);
	drlg_l1_crypt_rndset(CryptPattern78, rndper);
	drlg_l1_crypt_rndset(CryptPattern79, rndper);
}

void drlg_l1_crypt_pattern4(int rndper)
{
	drlg_l1_crypt_rndset(CryptPattern80, rndper);
	drlg_l1_crypt_rndset(CryptPattern81, rndper);
	drlg_l1_crypt_rndset(CryptPattern82, rndper);
	drlg_l1_crypt_rndset(CryptPattern83, rndper);
	drlg_l1_crypt_rndset(CryptPattern84, rndper);
	drlg_l1_crypt_rndset(CryptPattern85, rndper);
	drlg_l1_crypt_rndset(CryptPattern86, rndper);
	drlg_l1_crypt_rndset(CryptPattern87, rndper);
	drlg_l1_crypt_rndset(CryptPattern88, rndper);
	drlg_l1_crypt_rndset(CryptPattern89, rndper);
	drlg_l1_crypt_rndset(CryptPattern90, rndper);
	drlg_l1_crypt_rndset(CryptPattern91, rndper);
	drlg_l1_crypt_rndset(CryptPattern92, rndper);
	drlg_l1_crypt_rndset(CryptPattern93, rndper);
	drlg_l1_crypt_rndset(CryptPattern94, rndper);
	drlg_l1_crypt_rndset(CryptPattern95, rndper);
	drlg_l1_crypt_rndset(CryptPattern96, rndper);
}

void drlg_l1_crypt_pattern5(int rndper)
{
	drlg_l1_crypt_rndset(CryptPattern36, rndper);
	drlg_l1_crypt_rndset(CryptPattern37, rndper);
	drlg_l1_crypt_rndset(CryptPattern38, rndper);
	drlg_l1_crypt_rndset(CryptPattern39, rndper);
	drlg_l1_crypt_rndset(CryptPattern40, rndper);
	drlg_l1_crypt_rndset(CryptPattern41, rndper);
	drlg_l1_crypt_rndset(CryptPattern42, rndper);
	drlg_l1_crypt_rndset(CryptPattern43, rndper);
	drlg_l1_crypt_rndset(CryptPattern44, rndper);
	drlg_l1_crypt_rndset(CryptPattern45, rndper);
}

void drlg_l1_crypt_pattern6(int rndper)
{
	drlg_l1_crypt_rndset(CryptPattern10, rndper);
	drlg_l1_crypt_rndset(CryptPattern12, rndper);
	drlg_l1_crypt_rndset(CryptPattern11, rndper);
	drlg_l1_crypt_rndset(CryptPattern13, rndper);
	drlg_l1_crypt_rndset(CryptPattern14, rndper);
	drlg_l1_crypt_rndset(CryptPattern15, rndper);
	drlg_l1_crypt_rndset(CryptPattern16, rndper);
	drlg_l1_crypt_rndset(CryptPattern17, rndper);
	drlg_l1_crypt_rndset(CryptPattern18, rndper);
	drlg_l1_crypt_rndset(CryptPattern19, rndper);
	drlg_l1_crypt_rndset(CryptPattern20, rndper);
	drlg_l1_crypt_rndset(CryptPattern21, rndper);
	drlg_l1_crypt_rndset(CryptPattern22, rndper);
	drlg_l1_crypt_rndset(CryptPattern23, rndper);
	drlg_l1_crypt_rndset(CryptPattern24, rndper);
	drlg_l1_crypt_rndset(CryptPattern25, rndper);
	drlg_l1_crypt_rndset(CryptPattern26, rndper);
	drlg_l1_crypt_rndset(CryptPattern27, rndper);
	drlg_l1_crypt_rndset(CryptPattern28, rndper);
	drlg_l1_crypt_rndset(CryptPattern29, rndper);
	drlg_l1_crypt_rndset(CryptPattern30, rndper);
	drlg_l1_crypt_rndset(CryptPattern31, rndper);
	drlg_l1_crypt_rndset(CryptPattern32, rndper);
	drlg_l1_crypt_rndset(CryptPattern33, rndper);
	drlg_l1_crypt_rndset(CryptPattern34, rndper);
	drlg_l1_crypt_rndset(CryptPattern35, rndper);
}

void drlg_l1_crypt_pattern7(int rndper)
{
	drlg_l1_crypt_rndset(CryptPattern5, rndper);
	drlg_l1_crypt_rndset(CryptPattern6, rndper);
	drlg_l1_crypt_rndset(CryptPattern7, rndper);
	drlg_l1_crypt_rndset(CryptPattern8, rndper);
}

} // namespace devilution

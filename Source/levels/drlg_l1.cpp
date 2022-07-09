#include "levels/drlg_l1.h"

#include "engine/load_file.hpp"
#include "engine/point.hpp"
#include "engine/random.hpp"
#include "engine/rectangle.hpp"
#include "levels/crypt.h"
#include "levels/gendung.h"
#include "player.h"
#include "quests.h"
#include "utils/bitset2d.hpp"

namespace devilution {

namespace {

/** Marks where walls may not be added to the level */
Bitset2d<DMAXX, DMAXY> Chamber;
/** Specifies whether to generate a horizontal or vertical layout. */
bool VerticalLayout;
/** Specifies whether to generate a room at position 1 in the Cathedral. */
bool HasChamber1;
/** Specifies whether to generate a room at position 2 in the Cathedral. */
bool HasChamber2;
/** Specifies whether to generate a room at position 3 in the Cathedral. */
bool HasChamber3;

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
/**
 * A lookup table for the 16 possible patterns of a 2x2 area,
 * where each cell either contains a SW wall or it doesn't.
 */
BYTE L5ConvTbl[16] = { 22, 13, 1, 13, 2, 13, 13, 13, 4, 13, 1, 13, 2, 13, 16, 13 };

enum Tile : uint8_t {
	// clang-format off
	VWall          =   1,
	HWall          =   2,
	Corner         =   3,
	DWall          =   4,
	DArch          =   5,
	VWallEnd       =   6,
	HWallEnd       =   7,
	HArchEnd       =   8,
	VArchEnd       =   9,
	HArchVWall     =  10,
	VArch          =  11,
	HArch          =  12,
	Floor          =  13,
	HWallVArch     =  14,
	Pillar         =  15,
	Pillar1        =  16,
	Pillar2        =  17,
	DirtCorner     =  21,
	VDoor          =  25,
	HDoor          =  26,
	HFenceVWall    =  27,
	HDoorVDoor     =  28,
	DFence         =  29,
	VDoorEnd       =  30,
	HDoorEnd       =  31,
	VFenceEnd      =  32,
	VFence         =  35,
	HFence         =  36,
	HWallVFence    =  37,
	HArchVFence    =  38,
	HArchVDoor     =  39,
	EntranceStairs =  64,
	HWallShadow    = 148,
	HArchShadow    = 149,
	HArchShadow2   = 153,
	HWallShadow2   = 154,
	// clang-format on
};

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

				if (shadow.nv1 != 0 && !Protected.test(x - 1, y - 1)) {
					dungeon[x - 1][y - 1] = shadow.nv1;
				}
				if (shadow.nv2 != 0 && !Protected.test(x, y - 1)) {
					dungeon[x][y - 1] = shadow.nv2;
				}
				if (shadow.nv3 != 0 && !Protected.test(x - 1, y)) {
					dungeon[x - 1][y] = shadow.nv3;
				}
			}
		}
	}

	for (int y = 1; y < DMAXY; y++) {
		for (int x = 1; x < DMAXX; x++) {
			if (Protected.test(x - 1, y))
				continue;

			if (dungeon[x - 1][y] == 139) {
				uint8_t tnv3 = 139;
				if (IsAnyOf(dungeon[x][y], DFence, VFenceEnd, VFence, HWallVFence, HArchVFence, HArchVDoor)) {
					tnv3 = 141;
				}
				dungeon[x - 1][y] = tnv3;
			}
			if (dungeon[x - 1][y] == HArchShadow) {
				uint8_t tnv3 = HArchShadow;
				if (IsAnyOf(dungeon[x][y], DFence, VFenceEnd, VFence, HWallVFence, HArchVFence, HArchVDoor)) {
					tnv3 = HArchShadow2;
				}
				dungeon[x - 1][y] = tnv3;
			}
			if (dungeon[x - 1][y] == HWallShadow) {
				uint8_t tnv3 = HWallShadow;
				if (IsAnyOf(dungeon[x][y], DFence, VFenceEnd, VFence, HWallVFence, HArchVFence, HArchVDoor)) {
					tnv3 = HWallShadow2;
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

void FillFloor()
{
	for (int j = 0; j < DMAXY; j++) {
		for (int i = 0; i < DMAXX; i++) {
			if (!Protected.test(i, j) && dungeon[i][j] == Tile::Floor) {
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
	if (Quests[Q_BUTCHER].IsAvailable()) {
		pSetPiece = LoadFileInMem<uint16_t>("Levels\\L1Data\\rnd6.DUN");
	} else if (Quests[Q_SKELKING].IsAvailable() && !gbIsMultiplayer) {
		pSetPiece = LoadFileInMem<uint16_t>("Levels\\L1Data\\SKngDO.DUN");
	} else if (Quests[Q_LTBANNER].IsAvailable()) {
		pSetPiece = LoadFileInMem<uint16_t>("Levels\\L1Data\\Banner2.DUN");
	}
}

void InitDungeonPieces()
{
	for (int j = 0; j < MAXDUNY; j++) {
		for (int i = 0; i < MAXDUNX; i++) {
			int8_t pc;
			if (IsAnyOf(dPiece[i][j], 11, 70, 320, 210, 340, 417)) {
				pc = 1;
			} else if (IsAnyOf(dPiece[i][j], 10, 248, 324, 343, 330, 420)) {
				pc = 2;
			} else if (dPiece[i][j] == 252) {
				pc = 3;
			} else if (dPiece[i][j] == 254) {
				pc = 4;
			} else if (dPiece[i][j] == 258) {
				pc = 5;
			} else if (dPiece[i][j] == 266) {
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
	memset(dungeon, 22, sizeof(dungeon));
	Protected.reset();
	Chamber.reset();
}

void MapRoom(Rectangle room)
{
	for (int y = 0; y < room.size.height; y++) {
		for (int x = 0; x < room.size.width; x++) {
			DungeonMask.set(room.position.x + x, room.position.y + y);
		}
	}
}

bool CheckRoom(Rectangle room)
{
	for (int j = 0; j < room.size.height; j++) {
		for (int i = 0; i < room.size.width; i++) {
			if (i + room.position.x < 0 || i + room.position.x >= DMAXX || j + room.position.y < 0 || j + room.position.y >= DMAXY) {
				return false;
			}
			if (DungeonMask.test(i + room.position.x, j + room.position.y)) {
				return false;
			}
		}
	}

	return true;
}

void GenerateRoom(Rectangle area, bool verticalLayout)
{
	bool rotate = GenerateRnd(4) == 0;
	verticalLayout = (!verticalLayout && rotate) || (verticalLayout && !rotate);

	bool placeRoom1;
	Rectangle room1;

	for (int num = 0; num < 20; num++) {
		room1.size = { (GenerateRnd(5) + 2) & ~1, (GenerateRnd(5) + 2) & ~1 };
		room1.position = area.position;
		if (verticalLayout) {
			room1.position += Displacement { -room1.size.width, area.size.height / 2 - room1.size.height / 2 };
			placeRoom1 = CheckRoom({ room1.position + Displacement { -1, -1 }, { room1.size.height + 2, room1.size.width + 1 } }); /// BUGFIX: swap height and width ({ room1.size.width + 1, room1.size.height + 2 }) (workaround applied below)
		} else {
			room1.position += Displacement { area.size.width / 2 - room1.size.width / 2, -room1.size.height };
			placeRoom1 = CheckRoom({ room1.position + Displacement { -1, -1 }, { room1.size.width + 2, room1.size.height + 1 } });
		}
		if (placeRoom1)
			break;
	}

	if (placeRoom1)
		MapRoom({ room1.position, { std::min(DMAXX - room1.position.x, room1.size.width), std::min(DMAXX - room1.position.y, room1.size.height) } });

	bool placeRoom2;
	Rectangle room2 = room1;
	if (verticalLayout) {
		room2.position.x = area.position.x + area.size.width;
		placeRoom2 = CheckRoom({ room2.position + Displacement { 0, -1 }, { room2.size.width + 1, room2.size.height + 2 } });
	} else {
		room2.position.y = area.position.y + area.size.height;
		placeRoom2 = CheckRoom({ room2.position + Displacement { -1, 0 }, { room2.size.width + 2, room2.size.height + 1 } });
	}

	if (placeRoom2)
		MapRoom(room2);
	if (placeRoom1)
		GenerateRoom(room1, !verticalLayout);
	if (placeRoom2)
		GenerateRoom(room2, !verticalLayout);
}

/**
 * @brief Generate a boolean dungoen room layout
 */
void FirstRoom()
{
	DungeonMask.reset();

	VerticalLayout = !FlipCoin();
	HasChamber1 = FlipCoin();
	HasChamber2 = FlipCoin();
	HasChamber3 = FlipCoin();

	if (!HasChamber1 || !HasChamber3)
		HasChamber2 = true;

	Rectangle chamber1 { { 15, 15 }, { 10, 10 } };
	Rectangle chamber2 { { 15, 15 }, { 10, 10 } };
	Rectangle chamber3 { { 15, 15 }, { 10, 10 } };
	Rectangle hallway { { 1, 1 }, { DMAXX - 2, DMAXY - 2 } };

	if (VerticalLayout) {
		chamber1.position.y = 1;
		chamber3.position.y = 29;
		hallway.position.x = 17;
		hallway.size.width = 6;

		if (!HasChamber1) {
			hallway.position.y += 17;
			hallway.size.height -= 17;
		}

		if (!HasChamber3)
			hallway.size.height -= 16;
	} else {
		chamber1.position.x = 1;
		chamber3.position.x = 29;
		hallway.position.y = 17;
		hallway.size.height = 6;

		if (!HasChamber1) {
			hallway.position.x += 17;
			hallway.size.width -= 17;
		}

		if (!HasChamber3)
			hallway.size.width -= 16;
	}

	if (HasChamber1)
		MapRoom(chamber1);
	if (HasChamber2)
		MapRoom(chamber2);
	if (HasChamber3)
		MapRoom(chamber3);

	MapRoom(hallway);

	if (HasChamber1)
		GenerateRoom(chamber1, VerticalLayout);
	if (HasChamber2)
		GenerateRoom(chamber2, VerticalLayout);
	if (HasChamber3)
		GenerateRoom(chamber3, VerticalLayout);
}

/**
 * @brief Find the number of mega tiles used by layout
 */
int FindArea()
{
	int rv = 0;

	for (int j = 0; j < DMAXY; j++) {
		for (int i = 0; i < DMAXX; i++) { // NOLINT(modernize-loop-convert)
			if (DungeonMask.test(i, j))
				rv++;
		}
	}

	return rv;
}

void MakeDmt()
{
	for (int j = 0; j < DMAXY - 1; j++) {
		for (int i = 0; i < DMAXX - 1; i++) {
			int val = (DungeonMask.test(i + 1, j + 1) << 3) | (DungeonMask.test(i, j + 1) << 2) | (DungeonMask.test(i + 1, j) << 1) | DungeonMask.test(i, j);
			dungeon[i][j] = L5ConvTbl[val];
		}
	}
}

int HorizontalWallOk(int i, int j)
{
	int x;
	for (x = 1; dungeon[i + x][j] == 13; x++) {
		if (dungeon[i + x][j - 1] != 13 || dungeon[i + x][j + 1] != 13 || Protected.test(i + x, j) || Chamber.test(i + x, j))
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
		if (dungeon[i - 1][j + y] != 13 || dungeon[i + 1][j + y] != 13 || Protected.test(i, j + y) || Chamber.test(i, j + y))
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
		Protected.set(i + xx, j);
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
		Protected.set(i, j + yy);
	}
}

void AddWall()
{
	for (int j = 0; j < DMAXY; j++) {
		for (int i = 0; i < DMAXX; i++) {
			if (!Protected.test(i, j) && !Chamber.test(i, j)) {
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
			Chamber.set(i + sx, j + sy);
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

void Substitution()
{
	for (int y = 0; y < DMAXY; y++) {
		for (int x = 0; x < DMAXX; x++) {
			if (GenerateRnd(4) == 0) {
				uint8_t c = L5BTYPES[dungeon[x][y]];
				if (c != 0 && !Protected.test(x, y)) {
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
						if (L5BTYPES[dungeon[x][y - 1]] != 79 || Protected.test(x, y - 1))
							i = 79;
						else
							dungeon[x][y - 1] = 90;
					}
					// BUGFIX: Add `&& x + 1 < DMAXX` to the if statement. (fixed)
					if (i == 91 && x + 1 < DMAXX) {
						if (L5BTYPES[dungeon[x + 1][y]] != 80 || Protected.test(x + 1, y))
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

void FillChambers()
{
	if (!VerticalLayout) {
		if (HasChamber1)
			GenerateChamber(0, 14, false, false, false, true);

		if (!HasChamber3)
			GenerateChamber(14, 14, false, false, true, false);
		else if (!HasChamber1)
			GenerateChamber(14, 14, false, false, false, true);
		else if (HasChamber1 && HasChamber2 && HasChamber3)
			GenerateChamber(14, 14, false, false, true, true);

		if (HasChamber3)
			GenerateChamber(28, 14, false, false, true, false);
		if (HasChamber1 && HasChamber2)
			GenerateHall(12, 18, 14, 18);
		if (HasChamber2 && HasChamber3)
			GenerateHall(26, 18, 28, 18);
		if (!HasChamber2)
			GenerateHall(12, 18, 28, 18);
	} else {
		if (HasChamber1)
			GenerateChamber(14, 0, false, true, false, false);

		if (!HasChamber3)
			GenerateChamber(14, 14, true, false, false, false);
		else if (!HasChamber1)
			GenerateChamber(14, 14, false, true, false, false);
		else if (HasChamber1 && HasChamber2 && HasChamber3)
			GenerateChamber(14, 14, true, true, false, false);

		if (HasChamber3)
			GenerateChamber(14, 28, true, false, false, false);
		if (HasChamber1 && HasChamber2)
			GenerateHall(18, 12, 18, 14);
		if (HasChamber2 && HasChamber3)
			GenerateHall(18, 26, 18, 28);
		if (!HasChamber2)
			GenerateHall(18, 12, 18, 28);
	}

	if (leveltype == DTYPE_CRYPT) {
		if (currlevel == 24) {
			SetCryptRoom();
		} else if (currlevel == 21) {
			SetCornerRoom();
		}
	} else if (pSetPiece != nullptr) {
		SetSetPieceRoom(SelectChamber(), Tile::Floor);
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

void FixCornerTiles()
{
	for (int j = 1; j < DMAXY - 1; j++) {
		for (int i = 1; i < DMAXX - 1; i++) {
			if (!Protected.test(i, j) && dungeon[i][j] == 17 && dungeon[i - 1][j] == Tile::Floor && dungeon[i][j - 1] == Tile::VWall) {
				dungeon[i][j] = 16;
				// BUGFIX: Set tile as Protected
			}
			if (dungeon[i][j] == 202 && dungeon[i + 1][j] == Tile::Floor && dungeon[i][j + 1] == Tile::VWall) {
				dungeon[i][j] = 8;
			}
		}
	}
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

	LoadQuestSetPieces();

	while (true) {
		DRLG_InitTrans();

		do {
			FirstRoom();
		} while (FindArea() < minarea);

		InitDungeonFlags();
		MakeDmt();
		FillChambers();
		FixTilesPatterns();
		AddWall();
		FloodTransparencyValues(13);
		if (PlaceStairs(entry))
			break;
	}

	FreeQuestSetPieces();

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
		CryptSubstitution();
	} else {
		Substitution();
		ApplyShadowsPatterns();

		int numt = GenerateRnd(5) + 5;
		for (int i = 0; i < numt; i++) {
			PlaceMiniSet(LAMPS, DMAXX * DMAXY, true);
		}

		FillFloor();
	}

	memcpy(pdungeon, dungeon, sizeof(pdungeon));

	DRLG_CheckQuests(SetPiece.position);
}

void Pass3()
{
	DRLG_LPass3(22 - 1);

	if (leveltype == DTYPE_CRYPT)
		InitCryptPieces();
	else
		InitDungeonPieces();
}

} // namespace

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

Point SelectChamber()
{
	int chamber;
	if (!HasChamber1)
		chamber = FlipCoin() ? 3 : 2;
	else if (!HasChamber2)
		chamber = FlipCoin() ? 1 : 3;
	else if (!HasChamber3)
		chamber = FlipCoin() ? 1 : 2;
	else
		chamber = GenerateRnd(3) + 1;

	switch (chamber) {
	case 1:
		return VerticalLayout ? Point { 16, 2 } : Point { 2, 16 };
	case 3:
		return VerticalLayout ? Point { 16, 30 } : Point { 30, 16 };
	default:
		return { 16, 16 };
	}
}

void CreateL5Dungeon(uint32_t rseed, lvl_entry entry)
{
	SetRndSeed(rseed);

	UberRow = 0;
	UberCol = 0;

	GenerateLevel(entry);

	Pass3();

	if (leveltype == DTYPE_CRYPT) {
		for (int j = dminPosition.y; j < dmaxPosition.y; j++) {
			for (int i = dminPosition.x; i < dmaxPosition.x; i++) {
				if (dPiece[i][j] == 289) {
					UberRow = i;
					UberCol = j;
				}
				if (dPiece[i][j] == 316) {
					CornerStone.position = { i, j };
				}
			}
		}
	}
}

void LoadPreL1Dungeon(const char *path)
{
	memset(dungeon, 22, sizeof(dungeon));

	auto dunData = LoadFileInMem<uint16_t>(path);
	PlaceDunTiles(dunData.get(), { 0, 0 }, Tile::Floor);

	if (leveltype == DTYPE_CATHEDRAL)
		FillFloor();

	memcpy(pdungeon, dungeon, sizeof(pdungeon));
}

void LoadL1Dungeon(const char *path, Point spawn)
{
	LoadDungeonBase(path, spawn, Tile::Floor, 22);

	if (leveltype == DTYPE_CATHEDRAL)
		FillFloor();

	Pass3();

	if (leveltype == DTYPE_CRYPT)
		AddCryptObjects(0, 0, MAXDUNX, MAXDUNY);
	else
		AddL1Objs(0, 0, MAXDUNX, MAXDUNY);
}

} // namespace devilution

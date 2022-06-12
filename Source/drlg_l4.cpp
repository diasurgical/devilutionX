/**
 * @file drlg_l4.cpp
 *
 * Implementation of the hell level generation algorithms.
 */
#include "drlg_l4.h"

#include "engine/load_file.hpp"
#include "engine/random.hpp"
#include "gendung.h"
#include "monster.h"
#include "multi.h"
#include "objdat.h"

namespace devilution {

Point DiabloQuad1;
Point DiabloQuad2;
Point DiabloQuad3;
Point DiabloQuad4;

namespace {

bool hallok[20];
Point L4Hold;
BYTE L4dungeon[80][80];
BYTE dung[20][20];
// int dword_52A4DC;

/**
 * A lookup table for the 16 possible patterns of a 2x2 area,
 * where each cell either contains a SW wall or it doesn't.
 */
const BYTE L4ConvTbl[16] = { 30, 6, 1, 6, 2, 6, 6, 6, 9, 6, 1, 6, 2, 6, 3, 6 };

/** Miniset: Stairs up. */
const Miniset L4USTAIRS {
	{ 4, 5 },
	{
	    { 6, 6, 6, 6 },
	    { 6, 6, 6, 6 },
	    { 6, 6, 6, 6 },
	    { 6, 6, 6, 6 },
	    { 6, 6, 6, 6 },
	},
	{
	    { 0, 0, 0, 0 },
	    { 36, 38, 35, 0 },
	    { 37, 34, 33, 32 },
	    { 0, 0, 31, 0 },
	    { 0, 0, 0, 0 },
	}
};
/** Miniset: Stairs up to town. */
const Miniset L4TWARP {
	{ 4, 5 },
	{
	    { 6, 6, 6, 6 },
	    { 6, 6, 6, 6 },
	    { 6, 6, 6, 6 },
	    { 6, 6, 6, 6 },
	    { 6, 6, 6, 6 },
	},
	{
	    { 0, 0, 0, 0 },
	    { 134, 136, 133, 0 },
	    { 135, 132, 131, 130 },
	    { 0, 0, 129, 0 },
	    { 0, 0, 0, 0 },
	}
};
/** Miniset: Stairs down. */
const Miniset L4DSTAIRS {
	{ 5, 5 },
	{
	    { 6, 6, 6, 6, 6 },
	    { 6, 6, 6, 6, 6 },
	    { 6, 6, 6, 6, 6 },
	    { 6, 6, 6, 6, 6 },
	    { 6, 6, 6, 6, 6 },
	},
	{
	    { 0, 0, 0, 0, 0 },
	    { 0, 0, 45, 41, 0 },
	    { 0, 44, 43, 40, 0 },
	    { 0, 46, 42, 39, 0 },
	    { 0, 0, 0, 0, 0 },
	}
};
/** Miniset: Pentagram. */
const Miniset L4PENTA {
	{ 5, 5 },
	{
	    { 6, 6, 6, 6, 6 },
	    { 6, 6, 6, 6, 6 },
	    { 6, 6, 6, 6, 6 },
	    { 6, 6, 6, 6, 6 },
	    { 6, 6, 6, 6, 6 },
	},
	{
	    { 0, 0, 0, 0, 0 },
	    { 0, 98, 100, 103, 0 },
	    { 0, 99, 102, 105, 0 },
	    { 0, 101, 104, 106, 0 },
	    { 0, 0, 0, 0, 0 },
	}
};
/** Miniset: Pentagram portal. */
const Miniset L4PENTA2 {
	{ 5, 5 },
	{
	    { 6, 6, 6, 6, 6 },
	    { 6, 6, 6, 6, 6 },
	    { 6, 6, 6, 6, 6 },
	    { 6, 6, 6, 6, 6 },
	    { 6, 6, 6, 6, 6 },
	},
	{
	    { 0, 0, 0, 0, 0 },
	    { 0, 107, 109, 112, 0 },
	    { 0, 108, 111, 114, 0 },
	    { 0, 110, 113, 115, 0 },
	    { 0, 0, 0, 0, 0 },
	}
};

/** Maps tile IDs to their corresponding undecorated tile ID. */
const BYTE L4BTYPES[140] = {
	0, 1, 2, 3, 4, 5, 6, 7, 8, 9,
	10, 11, 12, 13, 14, 15, 16, 17, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0, 0, 6,
	6, 6, 0, 0, 0, 0, 0, 0, 0, 0,
	0, 1, 2, 1, 2, 1, 2, 1, 1, 2,
	2, 0, 0, 0, 0, 0, 0, 15, 16, 9,
	12, 4, 5, 7, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0
};

void ApplyShadowsPatterns()
{
	for (int y = 1; y < DMAXY; y++) {
		for (int x = 1; x < DMAXY; x++) {
			if (IsNoneOf(dungeon[x][y], 3, 4, 8, 15)) {
				continue;
			}
			if (dungeon[x - 1][y] == 6) {
				dungeon[x - 1][y] = 47;
			}
			if (dungeon[x - 1][y - 1] == 6) {
				dungeon[x - 1][y - 1] = 48;
			}
		}
	}
}

void LoadQuestSetPieces()
{
	setloadflag = false;
	if (Quests[Q_WARLORD].IsAvailable()) {
		pSetPiece = LoadFileInMem<uint16_t>("Levels\\L4Data\\Warlord.DUN");
		setloadflag = true;
	}
	if (currlevel == 15 && gbIsMultiplayer) {
		pSetPiece = LoadFileInMem<uint16_t>("Levels\\L4Data\\Vile1.DUN");
		setloadflag = true;
	}
}

void FreeQuestSetPieces()
{
	pSetPiece = nullptr;
}

void InitDungeonFlags()
{
	memset(dung, 0, sizeof(dung));
	memset(L4dungeon, 0, sizeof(L4dungeon));

	for (int j = 0; j < DMAXY; j++) {
		for (int i = 0; i < DMAXX; i++) {
			dungeon[i][j] = 30;
			Protected[i][j] = false;
		}
	}
}

void MapRoom(int x, int y, int width, int height)
{
	for (int j = 0; j < height && j + y < 20; j++) {
		for (int i = 0; i < width && i + x < 20; i++) {
			dung[i + x][j + y] = 1;
		}
	}
}

bool CheckRoom(int x, int y, int width, int height)
{
	if (x <= 0 || y <= 0) {
		return false;
	}

	for (int j = 0; j < height; j++) {
		for (int i = 0; i < width; i++) {
			if (i + x < 0 || i + x >= 20 || j + y < 0 || j + y >= 20) {
				return false;
			}
			if (dung[i + x][j + y] != 0) {
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
	int w = 14;
	int h = 14;
	if (currlevel != 16) {
		if (currlevel == Quests[Q_WARLORD]._qlevel && Quests[Q_WARLORD]._qactive != QUEST_NOTAVAIL) {
			assert(!gbIsMultiplayer);
			w = 11;
			h = 11;
		} else if (currlevel == Quests[Q_BETRAYER]._qlevel && gbIsMultiplayer) {
			w = 11;
			h = 11;
		} else {
			w = GenerateRnd(5) + 2;
			h = GenerateRnd(5) + 2;
		}
	}

	int xmin = (20 - w) / 2;
	int xmax = 19 - w;
	int x = GenerateRnd(xmax - xmin + 1) + xmin;

	int ymin = (20 - h) / 2;
	int ymax = 19 - h;
	int y = GenerateRnd(ymax - ymin + 1) + ymin;

	if (currlevel == 16) {
		L4Hold = { x, y };
	}
	if (Quests[Q_WARLORD].IsAvailable() || (currlevel == Quests[Q_BETRAYER]._qlevel && gbIsMultiplayer)) {
		SetPieceRoom = { { x + 1, y + 1 }, { w + 1, h + 1 } };
	} else {
		SetPieceRoom = {};
	}

	MapRoom(x, y, w, h);
	GenerateRoom(x, y, w, h, GenerateRnd(2));
}

void SetSetPieceRoom(Point position)
{
	SetPiece = { position, { SDL_SwapLE16(pSetPiece[0]), SDL_SwapLE16(pSetPiece[1]) } };

	PlaceDunTiles(pSetPiece.get(), position, 6);
}

void MakeDungeon()
{
	for (int j = 0; j < 20; j++) {
		for (int i = 0; i < 20; i++) {
			int k = i * 2;
			int l = j * 2;
			L4dungeon[k][l] = dung[i][j];
			L4dungeon[k][l + 1] = dung[i][j];
			L4dungeon[k + 1][l] = dung[i][j];
			L4dungeon[k + 1][l + 1] = dung[i][j];
		}
	}
	for (int j = 0; j < 20; j++) {
		for (int i = 0; i < 20; i++) {
			int k = i * 2;
			int l = j * 2;
			L4dungeon[k][l + 40] = dung[i][19 - j];
			L4dungeon[k][l + 41] = dung[i][19 - j];
			L4dungeon[k + 1][l + 40] = dung[i][19 - j];
			L4dungeon[k + 1][l + 41] = dung[i][19 - j];
		}
	}
	for (int j = 0; j < 20; j++) {
		for (int i = 0; i < 20; i++) {
			int k = i * 2;
			int l = j * 2;
			L4dungeon[k + 40][l] = dung[19 - i][j];
			L4dungeon[k + 40][l + 1] = dung[19 - i][j];
			L4dungeon[k + 41][l] = dung[19 - i][j];
			L4dungeon[k + 41][l + 1] = dung[19 - i][j];
		}
	}
	for (int j = 0; j < 20; j++) {
		for (int i = 0; i < 20; i++) {
			int k = i * 2;
			int l = j * 2;
			L4dungeon[k + 40][l + 40] = dung[19 - i][19 - j];
			L4dungeon[k + 40][l + 41] = dung[19 - i][19 - j];
			L4dungeon[k + 41][l + 40] = dung[19 - i][19 - j];
			L4dungeon[k + 41][l + 41] = dung[19 - i][19 - j];
		}
	}
}

void MakeDmt()
{
	int dmty = 1;
	for (int j = 0; dmty <= 77; j++, dmty += 2) {
		int dmtx = 1;
		for (int i = 0; dmtx <= 77; i++, dmtx += 2) {
			int val = 8 * L4dungeon[dmtx + 1][dmty + 1]
			    + 4 * L4dungeon[dmtx][dmty + 1]
			    + 2 * L4dungeon[dmtx + 1][dmty]
			    + L4dungeon[dmtx][dmty];
			dungeon[i][j] = L4ConvTbl[val];
		}
	}
}

int HorizontalWallOk(int i, int j)
{
	int x;
	for (x = 1; dungeon[i + x][j] == 6; x++) {
		if (Protected[i + x][j]) {
			break;
		}
		if (dungeon[i + x][j - 1] != 6) {
			break;
		}
		if (dungeon[i + x][j + 1] != 6) {
			break;
		}
	}

	if (IsAnyOf(dungeon[i + x][j], 10, 12, 13, 15, 16, 21, 22) && x > 3)
		return x;

	return -1;
}

int VerticalWallOk(int i, int j)
{
	int y;
	for (y = 1; dungeon[i][j + y] == 6; y++) {
		if (Protected[i][j + y]) {
			break;
		}
		if (dungeon[i - 1][j + y] != 6) {
			break;
		}
		if (dungeon[i + 1][j + y] != 6) {
			break;
		}
	}

	if (IsAnyOf(dungeon[i][j + y], 8, 9, 11, 14, 15, 16, 21, 23) && y > 3)
		return y;

	return -1;
}

void HorizontalWall(int i, int j, int dx)
{
	if (dungeon[i][j] == 13) {
		dungeon[i][j] = 17;
	}
	if (dungeon[i][j] == 16) {
		dungeon[i][j] = 11;
	}
	if (dungeon[i][j] == 12) {
		dungeon[i][j] = 14;
	}

	for (int xx = 1; xx < dx; xx++) {
		dungeon[i + xx][j] = 2;
	}

	if (dungeon[i + dx][j] == 15) {
		dungeon[i + dx][j] = 14;
	}
	if (dungeon[i + dx][j] == 10) {
		dungeon[i + dx][j] = 17;
	}
	if (dungeon[i + dx][j] == 21) {
		dungeon[i + dx][j] = 23;
	}
	if (dungeon[i + dx][j] == 22) {
		dungeon[i + dx][j] = 29;
	}

	int xx = GenerateRnd(dx - 3) + 1;
	dungeon[i + xx][j] = 57;
	dungeon[i + xx + 2][j] = 56;
	dungeon[i + xx + 1][j] = 60;

	if (dungeon[i + xx][j - 1] == 6) {
		dungeon[i + xx][j - 1] = 58;
	}
	if (dungeon[i + xx + 1][j - 1] == 6) {
		dungeon[i + xx + 1][j - 1] = 59;
	}
}

void VerticalWall(int i, int j, int dy)
{
	if (dungeon[i][j] == 14) {
		dungeon[i][j] = 17;
	}
	if (dungeon[i][j] == 8) {
		dungeon[i][j] = 9;
	}
	if (dungeon[i][j] == 15) {
		dungeon[i][j] = 10;
	}

	for (int yy = 1; yy < dy; yy++) {
		dungeon[i][j + yy] = 1;
	}

	if (dungeon[i][j + dy] == 11) {
		dungeon[i][j + dy] = 17;
	}
	if (dungeon[i][j + dy] == 9) {
		dungeon[i][j + dy] = 10;
	}
	if (dungeon[i][j + dy] == 16) {
		dungeon[i][j + dy] = 13;
	}
	if (dungeon[i][j + dy] == 21) {
		dungeon[i][j + dy] = 22;
	}
	if (dungeon[i][j + dy] == 23) {
		dungeon[i][j + dy] = 29;
	}

	int yy = GenerateRnd(dy - 3) + 1;
	dungeon[i][j + yy] = 53;
	dungeon[i][j + yy + 2] = 52;
	dungeon[i][j + yy + 1] = 6;

	if (dungeon[i - 1][j + yy] == 6) {
		dungeon[i - 1][j + yy] = 54;
	}
	if (dungeon[i - 1][j + yy - 1] == 6) {
		dungeon[i - 1][j + yy - 1] = 55;
	}
}

void AddWall()
{
	for (int j = 0; j < DMAXY; j++) {
		for (int i = 0; i < DMAXX; i++) {
			if (Protected[i][j]) {
				continue;
			}
			for (auto d : { 10, 12, 13, 15, 16, 21, 22 }) {
				if (d == dungeon[i][j]) {
					AdvanceRndSeed();
					int x = HorizontalWallOk(i, j);
					if (x != -1) {
						HorizontalWall(i, j, x);
					}
				}
			}
			for (auto d : { 8, 9, 11, 14, 15, 16, 21, 23 }) {
				if (d == dungeon[i][j]) {
					AdvanceRndSeed();
					int y = VerticalWallOk(i, j);
					if (y != -1) {
						VerticalWall(i, j, y);
					}
				}
			}
		}
	}
}

void FixTilesPatterns()
{
	for (int j = 0; j < DMAXY; j++) {
		for (int i = 0; i < DMAXX; i++) {
			if (dungeon[i][j] == 2 && dungeon[i + 1][j] == 6)
				dungeon[i + 1][j] = 5;
			if (dungeon[i][j] == 2 && dungeon[i + 1][j] == 1)
				dungeon[i + 1][j] = 13;
			if (dungeon[i][j] == 1 && dungeon[i][j + 1] == 2)
				dungeon[i][j + 1] = 14;
		}
	}

	for (int j = 0; j < DMAXY; j++) {
		for (int i = 0; i < DMAXX; i++) {
			if (dungeon[i][j] == 2 && dungeon[i + 1][j] == 6)
				dungeon[i + 1][j] = 2;
			if (dungeon[i][j] == 2 && dungeon[i + 1][j] == 9)
				dungeon[i + 1][j] = 11;
			if (dungeon[i][j] == 9 && dungeon[i + 1][j] == 6)
				dungeon[i + 1][j] = 12;
			if (dungeon[i][j] == 14 && dungeon[i + 1][j] == 1)
				dungeon[i + 1][j] = 13;
			if (dungeon[i][j] == 6 && dungeon[i + 1][j] == 14)
				dungeon[i + 1][j] = 15;
			if (dungeon[i][j] == 6 && dungeon[i][j + 1] == 13)
				dungeon[i][j + 1] = 16;
			if (dungeon[i][j] == 1 && dungeon[i][j + 1] == 9)
				dungeon[i][j + 1] = 10;
			if (dungeon[i][j] == 6 && dungeon[i][j - 1] == 1)
				dungeon[i][j - 1] = 1;
		}
	}

	for (int j = 0; j < DMAXY; j++) {
		for (int i = 0; i < DMAXX; i++) {
			if (dungeon[i][j] == 13 && dungeon[i][j + 1] == 30)
				dungeon[i][j + 1] = 27;
			if (dungeon[i][j] == 27 && dungeon[i + 1][j] == 30)
				dungeon[i + 1][j] = 19;
			if (dungeon[i][j] == 1 && dungeon[i][j + 1] == 30)
				dungeon[i][j + 1] = 27;
			if (dungeon[i][j] == 27 && dungeon[i + 1][j] == 1)
				dungeon[i + 1][j] = 16;
			if (dungeon[i][j] == 19 && dungeon[i + 1][j] == 27)
				dungeon[i + 1][j] = 26;
			if (dungeon[i][j] == 27 && dungeon[i + 1][j] == 30)
				dungeon[i + 1][j] = 19;
			if (dungeon[i][j] == 2 && dungeon[i + 1][j] == 15)
				dungeon[i + 1][j] = 14;
			if (dungeon[i][j] == 14 && dungeon[i + 1][j] == 15)
				dungeon[i + 1][j] = 14;
			if (dungeon[i][j] == 22 && dungeon[i + 1][j] == 1)
				dungeon[i + 1][j] = 16;
			if (dungeon[i][j] == 27 && dungeon[i + 1][j] == 1)
				dungeon[i + 1][j] = 16;
			if (dungeon[i][j] == 6 && dungeon[i + 1][j] == 27 && dungeon[i + 1][j + 1] != 0) /* check */
				dungeon[i + 1][j] = 22;
			if (dungeon[i][j] == 22 && dungeon[i + 1][j] == 30)
				dungeon[i + 1][j] = 19;
			if (dungeon[i][j] == 21 && dungeon[i + 1][j] == 1 && dungeon[i + 1][j - 1] == 1)
				dungeon[i + 1][j] = 13;
			if (dungeon[i][j] == 14 && dungeon[i + 1][j] == 30 && dungeon[i][j + 1] == 6)
				dungeon[i + 1][j] = 28;
			if (dungeon[i][j] == 16 && dungeon[i + 1][j] == 6 && dungeon[i][j + 1] == 30)
				dungeon[i][j + 1] = 27;
			if (dungeon[i][j] == 16 && dungeon[i][j + 1] == 30 && dungeon[i + 1][j + 1] == 30)
				dungeon[i][j + 1] = 27;
			if (dungeon[i][j] == 6 && dungeon[i + 1][j] == 30 && dungeon[i + 1][j - 1] == 6)
				dungeon[i + 1][j] = 21;
			if (dungeon[i][j] == 2 && dungeon[i + 1][j] == 27 && dungeon[i + 1][j + 1] == 9)
				dungeon[i + 1][j] = 29;
			if (dungeon[i][j] == 9 && dungeon[i + 1][j] == 15)
				dungeon[i + 1][j] = 14;
			if (dungeon[i][j] == 15 && dungeon[i + 1][j] == 27 && dungeon[i + 1][j + 1] == 2)
				dungeon[i + 1][j] = 29;
			if (dungeon[i][j] == 19 && dungeon[i + 1][j] == 18)
				dungeon[i + 1][j] = 24;
			if (dungeon[i][j] == 9 && dungeon[i + 1][j] == 15)
				dungeon[i + 1][j] = 14;
			if (dungeon[i][j] == 19 && dungeon[i + 1][j] == 19 && dungeon[i + 1][j - 1] == 30)
				dungeon[i + 1][j] = 24;
			if (dungeon[i][j] == 24 && dungeon[i][j - 1] == 30 && dungeon[i][j - 2] == 6)
				dungeon[i][j - 1] = 21;
			if (dungeon[i][j] == 2 && dungeon[i + 1][j] == 30)
				dungeon[i + 1][j] = 28;
			if (dungeon[i][j] == 15 && dungeon[i + 1][j] == 30)
				dungeon[i + 1][j] = 28;
			if (dungeon[i][j] == 28 && dungeon[i][j + 1] == 30)
				dungeon[i][j + 1] = 18;
			if (dungeon[i][j] == 28 && dungeon[i][j + 1] == 2)
				dungeon[i][j + 1] = 15;
			if (dungeon[i][j] == 19 && dungeon[i + 2][j] == 2 && dungeon[i + 1][j - 1] == 18 && dungeon[i + 1][j + 1] == 1)
				dungeon[i + 1][j] = 17;
			if (dungeon[i][j] == 19 && dungeon[i + 2][j] == 2 && dungeon[i + 1][j - 1] == 22 && dungeon[i + 1][j + 1] == 1)
				dungeon[i + 1][j] = 17;
			if (dungeon[i][j] == 19 && dungeon[i + 2][j] == 2 && dungeon[i + 1][j - 1] == 18 && dungeon[i + 1][j + 1] == 13)
				dungeon[i + 1][j] = 17;
			if (dungeon[i][j] == 21 && dungeon[i + 2][j] == 2 && dungeon[i + 1][j - 1] == 18 && dungeon[i + 1][j + 1] == 1)
				dungeon[i + 1][j] = 17;
			if (dungeon[i][j] == 21 && dungeon[i + 1][j + 1] == 1 && dungeon[i + 1][j - 1] == 22 && dungeon[i + 2][j] == 3)
				dungeon[i + 1][j] = 17;
			if (dungeon[i][j] == 15 && dungeon[i + 1][j] == 28 && dungeon[i + 2][j] == 30 && dungeon[i + 1][j - 1] == 6)
				dungeon[i + 1][j] = 23;
			if (dungeon[i][j] == 14 && dungeon[i + 1][j] == 28 && dungeon[i + 2][j] == 1)
				dungeon[i + 1][j] = 23;
			if (dungeon[i][j] == 15 && dungeon[i + 1][j] == 27 && dungeon[i + 1][j + 1] == 30)
				dungeon[i + 1][j] = 29;
			if (dungeon[i][j] == 28 && dungeon[i][j + 1] == 9)
				dungeon[i][j + 1] = 15;
			if (dungeon[i][j] == 21 && dungeon[i + 1][j - 1] == 21)
				dungeon[i + 1][j] = 24;
			if (dungeon[i][j] == 2 && dungeon[i + 1][j] == 27 && dungeon[i + 1][j + 1] == 30)
				dungeon[i + 1][j] = 29;
			if (dungeon[i][j] == 2 && dungeon[i + 1][j] == 18)
				dungeon[i + 1][j] = 25;
			if (dungeon[i][j] == 21 && dungeon[i + 1][j] == 9 && dungeon[i + 2][j] == 2)
				dungeon[i + 1][j] = 11;
			if (dungeon[i][j] == 19 && dungeon[i + 1][j] == 10)
				dungeon[i + 1][j] = 17;
			if (dungeon[i][j] == 15 && dungeon[i][j + 1] == 3)
				dungeon[i][j + 1] = 4;
			if (dungeon[i][j] == 22 && dungeon[i][j + 1] == 9)
				dungeon[i][j + 1] = 15;
			if (dungeon[i][j] == 18 && dungeon[i][j + 1] == 30)
				dungeon[i][j + 1] = 18;
			if (dungeon[i][j] == 24 && dungeon[i - 1][j] == 30)
				dungeon[i - 1][j] = 19;
			if (dungeon[i][j] == 21 && dungeon[i][j + 1] == 2)
				dungeon[i][j + 1] = 15;
			if (dungeon[i][j] == 21 && dungeon[i][j + 1] == 9)
				dungeon[i][j + 1] = 10;
			if (dungeon[i][j] == 22 && dungeon[i][j + 1] == 30)
				dungeon[i][j + 1] = 18;
			if (dungeon[i][j] == 21 && dungeon[i][j + 1] == 30)
				dungeon[i][j + 1] = 18;
			if (dungeon[i][j] == 16 && dungeon[i][j + 1] == 2)
				dungeon[i][j + 1] = 15;
			if (dungeon[i][j] == 13 && dungeon[i][j + 1] == 2)
				dungeon[i][j + 1] = 15;
			if (dungeon[i][j] == 22 && dungeon[i][j + 1] == 2)
				dungeon[i][j + 1] = 15;
			if (dungeon[i][j] == 21 && dungeon[i + 1][j] == 18 && dungeon[i + 2][j] == 30)
				dungeon[i + 1][j] = 24;
			if (dungeon[i][j] == 21 && dungeon[i + 1][j] == 9 && dungeon[i + 1][j + 1] == 1)
				dungeon[i + 1][j] = 16;
			if (dungeon[i][j] == 2 && dungeon[i + 1][j] == 27 && dungeon[i + 1][j + 1] == 2)
				dungeon[i + 1][j] = 29;
			if (dungeon[i][j] == 23 && dungeon[i][j + 1] == 2)
				dungeon[i][j + 1] = 15;
			if (dungeon[i][j] == 23 && dungeon[i][j + 1] == 9)
				dungeon[i][j + 1] = 15;
			if (dungeon[i][j] == 25 && dungeon[i][j + 1] == 2)
				dungeon[i][j + 1] = 15;
			if (dungeon[i][j] == 22 && dungeon[i + 1][j] == 9)
				dungeon[i + 1][j] = 11;
			if (dungeon[i][j] == 23 && dungeon[i + 1][j] == 9)
				dungeon[i + 1][j] = 11;
			if (dungeon[i][j] == 15 && dungeon[i + 1][j] == 1)
				dungeon[i + 1][j] = 16;
			if (dungeon[i][j] == 11 && dungeon[i + 1][j] == 15)
				dungeon[i + 1][j] = 14;
			if (dungeon[i][j] == 23 && dungeon[i + 1][j] == 1)
				dungeon[i + 1][j] = 16;
			if (dungeon[i][j] == 21 && dungeon[i + 1][j] == 27)
				dungeon[i + 1][j] = 26;
			if (dungeon[i][j] == 21 && dungeon[i + 1][j] == 18)
				dungeon[i + 1][j] = 24;
			if (dungeon[i][j] == 26 && dungeon[i + 1][j] == 1)
				dungeon[i + 1][j] = 16;
			if (dungeon[i][j] == 29 && dungeon[i + 1][j] == 1)
				dungeon[i + 1][j] = 16;
			if (dungeon[i][j] == 29 && dungeon[i][j + 1] == 2)
				dungeon[i][j + 1] = 15;
			if (dungeon[i][j] == 1 && dungeon[i][j - 1] == 15)
				dungeon[i][j - 1] = 10;
			if (dungeon[i][j] == 18 && dungeon[i][j + 1] == 2)
				dungeon[i][j + 1] = 15;
			if (dungeon[i][j] == 23 && dungeon[i][j + 1] == 30)
				dungeon[i][j + 1] = 18;
			if (dungeon[i][j] == 18 && dungeon[i][j + 1] == 9)
				dungeon[i][j + 1] = 10;
			if (dungeon[i][j] == 14 && dungeon[i + 1][j] == 30 && dungeon[i + 1][j + 1] == 30)
				dungeon[i + 1][j] = 23;
			if (dungeon[i][j] == 2 && dungeon[i + 1][j] == 28 && dungeon[i + 1][j - 1] == 6)
				dungeon[i + 1][j] = 23;
			if (dungeon[i][j] == 23 && dungeon[i + 1][j] == 18 && dungeon[i][j - 1] == 6)
				dungeon[i + 1][j] = 24;
			if (dungeon[i][j] == 14 && dungeon[i + 1][j] == 23 && dungeon[i + 2][j] == 30)
				dungeon[i + 1][j] = 28;
			if (dungeon[i][j] == 14 && dungeon[i + 1][j] == 28 && dungeon[i + 2][j] == 30 && dungeon[i + 1][j - 1] == 6)
				dungeon[i + 1][j] = 23;
			if (dungeon[i][j] == 23 && dungeon[i + 1][j] == 30)
				dungeon[i + 1][j] = 19;
			if (dungeon[i][j] == 29 && dungeon[i + 1][j] == 30)
				dungeon[i + 1][j] = 19;
			if (dungeon[i][j] == 29 && dungeon[i][j + 1] == 30)
				dungeon[i][j + 1] = 18;
			if (dungeon[i][j] == 19 && dungeon[i + 1][j] == 30)
				dungeon[i + 1][j] = 19;
			if (dungeon[i][j] == 21 && dungeon[i + 1][j] == 30)
				dungeon[i + 1][j] = 19;
			if (dungeon[i][j] == 26 && dungeon[i + 1][j] == 30)
				dungeon[i + 1][j] = 19;
			if (dungeon[i][j] == 16 && dungeon[i][j + 1] == 30)
				dungeon[i][j + 1] = 18;
			if (dungeon[i][j] == 13 && dungeon[i][j + 1] == 9)
				dungeon[i][j + 1] = 10;
			if (dungeon[i][j] == 25 && dungeon[i][j + 1] == 30)
				dungeon[i][j + 1] = 18;
			if (dungeon[i][j] == 18 && dungeon[i][j + 1] == 2)
				dungeon[i][j + 1] = 15;
			if (dungeon[i][j] == 11 && dungeon[i + 1][j] == 3)
				dungeon[i + 1][j] = 5;
			if (dungeon[i][j] == 19 && dungeon[i + 1][j] == 9)
				dungeon[i + 1][j] = 11;
			if (dungeon[i][j] == 19 && dungeon[i + 1][j] == 1)
				dungeon[i + 1][j] = 13;
			if (dungeon[i][j] == 19 && dungeon[i + 1][j] == 13 && dungeon[i + 1][j - 1] == 6)
				dungeon[i + 1][j] = 16;
		}
	}

	for (int j = 0; j < DMAXY; j++) {
		for (int i = 0; i < DMAXX; i++) {
			if (dungeon[i][j] == 21 && dungeon[i][j + 1] == 24 && dungeon[i][j + 2] == 1)
				dungeon[i][j + 1] = 17;
			if (dungeon[i][j] == 15 && dungeon[i + 1][j + 1] == 9 && dungeon[i + 1][j - 1] == 1 && dungeon[i + 2][j] == 16)
				dungeon[i + 1][j] = 29;
			if (dungeon[i][j] == 2 && dungeon[i - 1][j] == 6)
				dungeon[i - 1][j] = 8;
			if (dungeon[i][j] == 1 && dungeon[i][j - 1] == 6)
				dungeon[i][j - 1] = 7;
			if (dungeon[i][j] == 6 && dungeon[i + 1][j] == 15 && dungeon[i + 1][j + 1] == 4)
				dungeon[i + 1][j] = 10;
			if (dungeon[i][j] == 1 && dungeon[i][j + 1] == 3)
				dungeon[i][j + 1] = 4;
			if (dungeon[i][j] == 1 && dungeon[i][j + 1] == 6)
				dungeon[i][j + 1] = 4;
			if (dungeon[i][j] == 9 && dungeon[i][j + 1] == 3)
				dungeon[i][j + 1] = 4;
			if (dungeon[i][j] == 10 && dungeon[i][j + 1] == 3)
				dungeon[i][j + 1] = 4;
			if (dungeon[i][j] == 13 && dungeon[i][j + 1] == 3)
				dungeon[i][j + 1] = 4;
			if (dungeon[i][j] == 1 && dungeon[i][j + 1] == 5)
				dungeon[i][j + 1] = 12;
			if (dungeon[i][j] == 1 && dungeon[i][j + 1] == 16)
				dungeon[i][j + 1] = 13;
			if (dungeon[i][j] == 6 && dungeon[i][j + 1] == 13)
				dungeon[i][j + 1] = 16;
			if (dungeon[i][j] == 25 && dungeon[i][j + 1] == 9)
				dungeon[i][j + 1] = 10;
			if (dungeon[i][j] == 13 && dungeon[i][j + 1] == 5)
				dungeon[i][j + 1] = 12;
			if (dungeon[i][j] == 28 && dungeon[i][j - 1] == 6 && dungeon[i + 1][j] == 1)
				dungeon[i + 1][j] = 23;
			if (dungeon[i][j] == 19 && dungeon[i + 1][j] == 10)
				dungeon[i + 1][j] = 17;
			if (dungeon[i][j] == 21 && dungeon[i + 1][j] == 9)
				dungeon[i + 1][j] = 11;
			if (dungeon[i][j] == 11 && dungeon[i + 1][j] == 3)
				dungeon[i + 1][j] = 5;
			if (dungeon[i][j] == 10 && dungeon[i + 1][j] == 4)
				dungeon[i + 1][j] = 12;
			if (dungeon[i][j] == 14 && dungeon[i + 1][j] == 4)
				dungeon[i + 1][j] = 12;
			if (dungeon[i][j] == 27 && dungeon[i + 1][j] == 9)
				dungeon[i + 1][j] = 11;
			if (dungeon[i][j] == 15 && dungeon[i + 1][j] == 4)
				dungeon[i + 1][j] = 12;
			if (dungeon[i][j] == 21 && dungeon[i + 1][j] == 1)
				dungeon[i + 1][j] = 16;
			if (dungeon[i][j] == 11 && dungeon[i + 1][j] == 4)
				dungeon[i + 1][j] = 12;
			if (dungeon[i][j] == 2 && dungeon[i + 1][j] == 3)
				dungeon[i + 1][j] = 5;
			if (dungeon[i][j] == 9 && dungeon[i + 1][j] == 3)
				dungeon[i + 1][j] = 5;
			if (dungeon[i][j] == 14 && dungeon[i + 1][j] == 3)
				dungeon[i + 1][j] = 5;
			if (dungeon[i][j] == 15 && dungeon[i + 1][j] == 3)
				dungeon[i + 1][j] = 5;
			if (dungeon[i][j] == 2 && dungeon[i + 1][j] == 5 && dungeon[i + 1][j - 1] == 16)
				dungeon[i + 1][j] = 12;
			if (dungeon[i][j] == 2 && dungeon[i + 1][j] == 4)
				dungeon[i + 1][j] = 12;
			if (dungeon[i][j] == 9 && dungeon[i + 1][j] == 4)
				dungeon[i + 1][j] = 12;
			if (dungeon[i][j] == 1 && dungeon[i][j - 1] == 8)
				dungeon[i][j - 1] = 9;
			if (dungeon[i][j] == 28 && dungeon[i + 1][j] == 23 && dungeon[i + 1][j + 1] == 3)
				dungeon[i + 1][j] = 16;
		}
	}

	for (int j = 0; j < DMAXY; j++) {
		for (int i = 0; i < DMAXX; i++) {
			if (dungeon[i][j] == 21 && dungeon[i + 1][j] == 10)
				dungeon[i + 1][j] = 17;
			if (dungeon[i][j] == 17 && dungeon[i + 1][j] == 4)
				dungeon[i + 1][j] = 12;
			if (dungeon[i][j] == 10 && dungeon[i + 1][j] == 4)
				dungeon[i + 1][j] = 12;
			if (dungeon[i][j] == 17 && dungeon[i][j + 1] == 5)
				dungeon[i][j + 1] = 12;
			if (dungeon[i][j] == 29 && dungeon[i][j + 1] == 9)
				dungeon[i][j + 1] = 10;
			if (dungeon[i][j] == 13 && dungeon[i][j + 1] == 5)
				dungeon[i][j + 1] = 12;
			if (dungeon[i][j] == 9 && dungeon[i][j + 1] == 16)
				dungeon[i][j + 1] = 13;
			if (dungeon[i][j] == 10 && dungeon[i][j + 1] == 16)
				dungeon[i][j + 1] = 13;
			if (dungeon[i][j] == 16 && dungeon[i][j + 1] == 3)
				dungeon[i][j + 1] = 4;
			if (dungeon[i][j] == 11 && dungeon[i][j + 1] == 5)
				dungeon[i][j + 1] = 12;
			if (dungeon[i][j] == 10 && dungeon[i + 1][j] == 3 && dungeon[i + 1][j - 1] == 16)
				dungeon[i + 1][j] = 12;
			if (dungeon[i][j] == 16 && dungeon[i][j + 1] == 5)
				dungeon[i][j + 1] = 12;
			if (dungeon[i][j] == 1 && dungeon[i][j + 1] == 6)
				dungeon[i][j + 1] = 4;
			if (dungeon[i][j] == 21 && dungeon[i + 1][j] == 13 && dungeon[i][j + 1] == 10)
				dungeon[i + 1][j + 1] = 12;
			if (dungeon[i][j] == 15 && dungeon[i + 1][j] == 10)
				dungeon[i + 1][j] = 17;
			if (dungeon[i][j] == 22 && dungeon[i][j + 1] == 11)
				dungeon[i][j + 1] = 17;
			if (dungeon[i][j] == 15 && dungeon[i + 1][j] == 28 && dungeon[i + 2][j] == 16)
				dungeon[i + 1][j] = 23;
			if (dungeon[i][j] == 28 && dungeon[i + 1][j] == 23 && dungeon[i + 1][j + 1] == 1 && dungeon[i + 2][j] == 6)
				dungeon[i + 1][j] = 16;
		}
	}

	for (int j = 0; j < DMAXY; j++) {
		for (int i = 0; i < DMAXX; i++) {
			if (dungeon[i][j] == 15 && dungeon[i + 1][j] == 28 && dungeon[i + 2][j] == 16)
				dungeon[i + 1][j] = 23;
			if (dungeon[i][j] == 21 && dungeon[i + 1][j - 1] == 21 && dungeon[i + 1][j + 1] == 13 && dungeon[i + 2][j] == 2)
				dungeon[i + 1][j] = 17;
			if (dungeon[i][j] == 19 && dungeon[i + 1][j] == 15 && dungeon[i + 1][j + 1] == 12)
				dungeon[i + 1][j] = 17;
		}
	}
}

void Substitution()
{
	for (int y = 0; y < DMAXY; y++) {
		for (int x = 0; x < DMAXX; x++) {
			if (GenerateRnd(3) == 0) {
				uint8_t c = L4BTYPES[dungeon[x][y]];
				if (c != 0 && !Protected[x][y]) {
					int rv = GenerateRnd(16);
					int i = -1;
					while (rv >= 0) {
						i++;
						if (i == sizeof(L4BTYPES)) {
							i = 0;
						}
						if (c == L4BTYPES[i]) {
							rv--;
						}
					}

					dungeon[x][y] = i;
				}
			}
		}
	}
	for (int y = 0; y < DMAXY; y++) {
		for (int x = 0; x < DMAXX; x++) {
			int rv = GenerateRnd(10);
			if (rv == 0) {
				uint8_t c = dungeon[x][y];
				if (L4BTYPES[c] == 6 && !Protected[x][y]) {
					dungeon[x][y] = GenerateRnd(3) + 95;
				}
			}
		}
	}
}

void UShape()
{
	for (int j = 19; j >= 0; j--) {
		for (int i = 19; i >= 0; i--) {
			if (dung[i][j] != 1) {
				hallok[j] = false;
			}
			if (dung[i][j] == 1) {
				// BUGFIX: check that i + 1 < 20 and j + 1 < 20 (fixed)
				if (i + 1 < 20 && j + 1 < 20
				    && dung[i][j + 1] == 1 && dung[i + 1][j + 1] == 0) {
					hallok[j] = true;
				} else {
					hallok[j] = false;
				}
				i = 0;
			}
		}
	}

	int rv = GenerateRnd(19) + 1;
	do {
		if (hallok[rv]) {
			for (int i = 19; i >= 0; i--) {
				if (dung[i][rv] == 1) {
					i = -1;
					rv = 0;
				} else {
					dung[i][rv] = 1;
					dung[i][rv + 1] = 1;
				}
			}
		} else {
			rv++;
			if (rv == 20) {
				rv = 1;
			}
		}
	} while (rv != 0);

	for (int i = 19; i >= 0; i--) {
		for (int j = 19; j >= 0; j--) {
			if (dung[i][j] != 1) {
				hallok[i] = false;
			}
			if (dung[i][j] == 1) {
				// BUGFIX: check that i + 1 < 20 and j + 1 < 20 (fixed)
				if (i + 1 < 20 && j + 1 < 20
				    && dung[i + 1][j] == 1 && dung[i + 1][j + 1] == 0) {
					hallok[i] = true;
				} else {
					hallok[i] = false;
				}
				j = 0;
			}
		}
	}

	rv = GenerateRnd(19) + 1;
	do {
		if (hallok[rv]) {
			for (int j = 19; j >= 0; j--) {
				if (dung[rv][j] == 1) {
					j = -1;
					rv = 0;
				} else {
					dung[rv][j] = 1;
					dung[rv + 1][j] = 1;
				}
			}
		} else {
			rv++;
			if (rv == 20) {
				rv = 1;
			}
		}
	} while (rv != 0);
}

int GetArea()
{
	int rv = 0;

	for (int j = 0; j < 20; j++) {
		for (int i = 0; i < 20; i++) { // NOLINT(modernize-loop-convert)
			if (dung[i][j] == 1) {
				rv++;
			}
		}
	}

	return rv;
}

void ProtectQuads()
{
	for (int y = 0; y < 14; y++) {
		for (int x = 0; x < 14; x++) {
			Protected[L4Hold.x + x][L4Hold.y + y] = true;
			Protected[DMAXX - 1 - x - L4Hold.x][L4Hold.y + y] = true;
			Protected[L4Hold.x + x][DMAXY - 1 - y - L4Hold.y] = true;
			Protected[DMAXX - 1 - x - L4Hold.x][DMAXY - 1 - y - L4Hold.y] = true;
		}
	}
}

void LoadDiabQuads(bool preflag)
{
	{
		auto dunData = LoadFileInMem<uint16_t>("Levels\\L4Data\\diab1.DUN");
		DiabloQuad1 = L4Hold + Displacement { 4, 4 };
		PlaceDunTiles(dunData.get(), DiabloQuad1, 6);
	}
	{
		auto dunData = LoadFileInMem<uint16_t>(preflag ? "Levels\\L4Data\\diab2b.DUN" : "Levels\\L4Data\\diab2a.DUN");
		DiabloQuad2 = { 27 - L4Hold.x, 1 + L4Hold.y };
		PlaceDunTiles(dunData.get(), DiabloQuad2, 6);
	}
	{
		auto dunData = LoadFileInMem<uint16_t>(preflag ? "Levels\\L4Data\\diab3b.DUN" : "Levels\\L4Data\\diab3a.DUN");
		DiabloQuad3 = { 1 + L4Hold.x, 27 - L4Hold.y };
		PlaceDunTiles(dunData.get(), DiabloQuad3, 6);
	}
	{
		auto dunData = LoadFileInMem<uint16_t>(preflag ? "Levels\\L4Data\\diab4b.DUN" : "Levels\\L4Data\\diab4a.DUN");
		DiabloQuad4 = { 28 - L4Hold.x, 28 - L4Hold.y };
		PlaceDunTiles(dunData.get(), DiabloQuad4, 6);
	}
}

bool IsDURightWall(char d)
{
	if (d == 25) {
		return true;
	}
	if (d == 28) {
		return true;
	}
	if (d == 23) {
		return true;
	}

	return false;
}

bool IsDLLeftWall(char dd)
{
	if (dd == 27) {
		return true;
	}
	if (dd == 26) {
		return true;
	}
	if (dd == 22) {
		return true;
	}

	return false;
}

void FixTransparency()
{
	int yy = 16;
	for (int j = 0; j < DMAXY; j++) {
		int xx = 16;
		for (int i = 0; i < DMAXX; i++) {
			// BUGFIX: Should check for `j > 0` first.
			if (IsDURightWall(dungeon[i][j]) && dungeon[i][j - 1] == 18) {
				dTransVal[xx + 1][yy] = dTransVal[xx][yy];
				dTransVal[xx + 1][yy + 1] = dTransVal[xx][yy];
			}
			// BUGFIX: Should check for `i + 1 < DMAXY` first.
			if (IsDLLeftWall(dungeon[i][j]) && dungeon[i + 1][j] == 19) {
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
			if (dungeon[i][j] == 24) {
				dTransVal[xx + 1][yy] = dTransVal[xx][yy];
				dTransVal[xx][yy + 1] = dTransVal[xx][yy];
				dTransVal[xx + 1][yy + 1] = dTransVal[xx][yy];
			}
			if (dungeon[i][j] == 57) {
				dTransVal[xx - 1][yy] = dTransVal[xx][yy + 1];
				dTransVal[xx][yy] = dTransVal[xx][yy + 1];
			}
			if (dungeon[i][j] == 53) {
				dTransVal[xx][yy - 1] = dTransVal[xx + 1][yy];
				dTransVal[xx][yy] = dTransVal[xx + 1][yy];
			}
			xx += 2;
		}
		yy += 2;
	}
}

void FixCornerTiles()
{
	for (int j = 1; j < DMAXY - 1; j++) {
		for (int i = 1; i < DMAXX - 1; i++) {
			if (dungeon[i][j] >= 18 && dungeon[i][j] <= 30) {
				if (dungeon[i + 1][j] < 18 || dungeon[i][j + 1] < 18) {
					dungeon[i][j] += 98;
				}
			}
		}
	}
}

void FixRim()
{
	for (int i = 0; i < 20; i++) { // NOLINT(modernize-loop-convert)
		dung[i][0] = 0;
	}
	for (int j = 0; j < 20; j++) {
		dung[0][j] = 0;
	}
}

void GeneralFix()
{
	for (int j = 0; j < DMAXY - 1; j++) {
		for (int i = 0; i < DMAXX - 1; i++) {
			if ((dungeon[i][j] == 24 || dungeon[i][j] == 122) && dungeon[i + 1][j] == 2 && dungeon[i][j + 1] == 5) {
				dungeon[i][j] = 17;
			}
		}
	}
}

bool PlaceStairs(lvl_entry entry)
{
	std::optional<Point> position;

	// Place stairs up
	position = PlaceMiniSet(L4USTAIRS);
	if (!position)
		return false;
	if (entry == ENTRY_MAIN)
		ViewPosition = position->megaToWorld() + Displacement { 6, 6 };

	if (currlevel != 15) {
		// Place stairs down
		if (currlevel != 16) {
			if (Quests[Q_WARLORD].IsAvailable()) {
				if (entry == ENTRY_PREV)
					ViewPosition = SetPiece.position.megaToWorld() + Displacement { 6, 6 };
			} else {
				position = PlaceMiniSet(L4DSTAIRS);
				if (!position)
					return false;
				if (entry == ENTRY_PREV)
					ViewPosition = position->megaToWorld() + Displacement { 5, 7 };
			}
		}

		// Place town warp stairs
		if (currlevel == 13) {
			position = PlaceMiniSet(L4TWARP);
			if (!position)
				return false;
			if (entry == ENTRY_TWARPDN)
				ViewPosition = position->megaToWorld() + Displacement { 6, 6 };
		}
	} else {
		// Place hell gate
		bool isGateOpen = gbIsMultiplayer || Quests[Q_DIABLO]._qactive == QUEST_ACTIVE;
		position = PlaceMiniSet(isGateOpen ? L4PENTA2 : L4PENTA);
		if (!position)
			return false;
		if (entry == ENTRY_PREV)
			ViewPosition = position->megaToWorld() + Displacement { 5, 7 };
	}

	return true;
}

void GenerateLevel(lvl_entry entry)
{
	while (true) {
		DRLG_InitTrans();

		constexpr int Minarea = 173;
		do {
			InitDungeonFlags();
			FirstRoom();
			FixRim();
		} while (GetArea() < Minarea);
		UShape();

		MakeDungeon();
		MakeDmt();
		FixTilesPatterns();
		if (currlevel == 16) {
			ProtectQuads();
		}
		if (Quests[Q_WARLORD].IsAvailable() || (currlevel == Quests[Q_BETRAYER]._qlevel && gbIsMultiplayer)) {
			for (int spi = SetPieceRoom.position.x; spi < SetPieceRoom.position.x + SetPieceRoom.size.width - 1; spi++) {
				for (int spj = SetPieceRoom.position.y; spj < SetPieceRoom.position.y + SetPieceRoom.size.height - 1; spj++) {
					Protected[spi][spj] = true;
				}
			}
		}
		AddWall();
		FloodTransparencyValues(6);
		FixTransparency();
		if (setloadflag) {
			SetSetPieceRoom(SetPieceRoom.position);
		}
		if (currlevel == 16) {
			LoadDiabQuads(true);
		}
		if (PlaceStairs(entry))
			break;
	}

	GeneralFix();

	if (currlevel != 16) {
		DRLG_PlaceThemeRooms(7, 10, 6, 8, true);
	}

	ApplyShadowsPatterns();
	FixCornerTiles();
	Substitution();
	DRLG_Init_Globals();

	if (Quests[Q_WARLORD].IsAvailable()) {
		for (int j = 0; j < DMAXY; j++) {
			for (int i = 0; i < DMAXX; i++) {
				pdungeon[i][j] = dungeon[i][j];
			}
		}
	}

	DRLG_CheckQuests(SetPieceRoom.position);

	if (currlevel == 15) {
		for (int j = 0; j < DMAXY; j++) {
			for (int i = 0; i < DMAXX; i++) {
				if (IsAnyOf(dungeon[i][j], 98, 107)) {
					Make_SetPC({ { i - 1, j - 1 }, { 5, 5 } });
					if (Quests[Q_BETRAYER]._qactive >= QUEST_ACTIVE) { /// Lazarus staff skip bug fixed
						// Set the portal position to the location of the northmost pentagram tile.
						Quests[Q_BETRAYER].position = { i, j };
					}
				}
			}
		}
	}
	if (currlevel == 16) {
		for (int j = 0; j < DMAXY; j++) {
			for (int i = 0; i < DMAXX; i++) {
				pdungeon[i][j] = dungeon[i][j];
			}
		}
		LoadDiabQuads(false);
	}
}

void Pass3()
{
	DRLG_LPass3(30 - 1);
}

} // namespace

void CreateL4Dungeon(uint32_t rseed, lvl_entry entry)
{
	SetRndSeed(rseed);

	dminPosition = Point(0, 0).megaToWorld();
	dmaxPosition = Point(40, 40).megaToWorld();

	ViewPosition = { 40, 40 };

	DRLG_InitSetPC();
	LoadQuestSetPieces();
	GenerateLevel(entry);
	Pass3();
	FreeQuestSetPieces();
	DRLG_SetPC();
}

void LoadL4Dungeon(const char *path, int vx, int vy)
{
	dminPosition = Point(0, 0).megaToWorld();
	dmaxPosition = Point(40, 40).megaToWorld();

	DRLG_InitTrans();
	InitDungeonFlags();

	auto dunData = LoadFileInMem<uint16_t>(path);
	PlaceDunTiles(dunData.get(), { 0, 0 }, 6);

	ViewPosition = { vx, vy };

	Pass3();
	DRLG_Init_Globals();

	SetMapMonsters(dunData.get(), Point(0, 0).megaToWorld());
	SetMapObjects(dunData.get(), 0, 0);
}

void LoadPreL4Dungeon(const char *path)
{
	dminPosition = Point(0, 0).megaToWorld();
	dmaxPosition = Point(40, 40).megaToWorld();

	InitDungeonFlags();

	auto dunData = LoadFileInMem<uint16_t>(path);
	PlaceDunTiles(dunData.get(), { 0, 0 }, 6);
}

} // namespace devilution

/**
 * @file gendung.cpp
 *
 * Implementation of general dungeon generation code.
 */
#include "all.h"

DEVILUTION_BEGIN_NAMESPACE

/** Contains the tile IDs of the map. */
BYTE dungeon[DMAXX][DMAXY];
/** Contains a backup of the tile IDs of the map. */
BYTE pdungeon[DMAXX][DMAXY];
char dflags[DMAXX][DMAXY];
/** Specifies the active set level X-coordinate of the map. */
int setpc_x;
/** Specifies the active set level Y-coordinate of the map. */
int setpc_y;
/** Specifies the width of the active set level of the map. */
int setpc_w;
/** Specifies the height of the active set level of the map. */
int setpc_h;
/** Contains the contents of the single player quest DUN file. */
BYTE *pSetPiece;
/** Specifies whether a single player quest DUN has been loaded. */
BOOL setloadflag;
BYTE *pSpecialCels;
/** Specifies the tile definitions of the active dungeon type; (e.g. levels/l1data/l1.til). */
BYTE *pMegaTiles;
BYTE *pLevelPieces;
BYTE *pDungeonCels;
/**
 * List of transparancy masks to use for dPieces
 */
char block_lvid[MAXTILES + 1];
/**
 * List of light blocking dPieces
 */
BOOLEAN nBlockTable[MAXTILES + 1];
/**
 * List of path blocking dPieces
 */
BOOLEAN nSolidTable[MAXTILES + 1];
/**
 * List of transparent dPieces
 */
BOOLEAN nTransTable[MAXTILES + 1];
/**
 * List of missile blocking dPieces
 */
BOOLEAN nMissileTable[MAXTILES + 1];
BOOLEAN nTrapTable[MAXTILES + 1];
/** Specifies the minimum X-coordinate of the map. */
int dminx;
/** Specifies the minimum Y-coordinate of the map. */
int dminy;
/** Specifies the maximum X-coordinate of the map. */
int dmaxx;
/** Specifies the maximum Y-coordinate of the map. */
int dmaxy;
int gnDifficulty;
/** Specifies the active dungeon type of the current game. */
BYTE leveltype;
/** Specifies the active dungeon level of the current game. */
BYTE currlevel;
BOOLEAN setlevel;
/** Specifies the active quest level of the current game. */
BYTE setlvlnum;
char setlvltype;
/** Specifies the player viewpoint X-coordinate of the map. */
int ViewX;
/** Specifies the player viewpoint Y-coordinate of the map. */
int ViewY;
int ViewBX;
int ViewBY;
int ViewDX;
int ViewDY;
ScrollStruct ScrollInfo;
/** Specifies the level viewpoint X-coordinate of the map. */
int LvlViewX;
/** Specifies the level viewpoint Y-coordinate of the map. */
int LvlViewY;
int MicroTileLen;
char TransVal;
/** Specifies the active transparency indices. */
BOOLEAN TransList[256];
/** Contains the piece IDs of each tile on the map. */
int dPiece[MAXDUNX][MAXDUNY];
/** Specifies the dungeon piece information for a given coordinate and block number. */
MICROS dpiece_defs_map_2[MAXDUNX][MAXDUNY];
/** Specifies the transparency at each coordinate of the map. */
char dTransVal[MAXDUNX][MAXDUNY];
char dLight[MAXDUNX][MAXDUNY];
char dPreLight[MAXDUNX][MAXDUNY];
char dFlags[MAXDUNX][MAXDUNY];
/** Contains the player numbers (players array indices) of the map. */
char dPlayer[MAXDUNX][MAXDUNY];
/**
 * Contains the NPC numbers of the map. The NPC number represents a
 * towner number (towners array index) in Tristram and a monster number
 * (monsters array index) in the dungeon.
 */
int dMonster[MAXDUNX][MAXDUNY];
/**
 * Contains the dead numbers (deads array indices) and dead direction of
 * the map, encoded as specified by the pseudo-code below.
 * dDead[x][y] & 0x1F - index of dead
 * dDead[x][y] >> 0x5 - direction
 */
char dDead[MAXDUNX][MAXDUNY];
/** Contains the object numbers (objects array indices) of the map. */
char dObject[MAXDUNX][MAXDUNY];
/** Contains the item numbers (items array indices) of the map. */
char dItem[MAXDUNX][MAXDUNY];
/** Contains the missile numbers (missiles array indices) of the map. */
char dMissile[MAXDUNX][MAXDUNY];
/**
 * Contains the arch frame numbers of the map from the special tileset
 * (e.g. "levels/l1data/l1s.cel"). Note, the special tileset of Tristram (i.e.
 * "levels/towndata/towns.cel") contains trees rather than arches.
 */
char dSpecial[MAXDUNX][MAXDUNY];
int themeCount;
THEME_LOC themeLoc[MAXTHEMES];

void FillSolidBlockTbls()
{
	BYTE bv;
	DWORD i, dwTiles;
	BYTE *pSBFile, *pTmp;

	memset(nBlockTable, 0, sizeof(nBlockTable));
	memset(nSolidTable, 0, sizeof(nSolidTable));
	memset(nTransTable, 0, sizeof(nTransTable));
	memset(nMissileTable, 0, sizeof(nMissileTable));
	memset(nTrapTable, 0, sizeof(nTrapTable));

	switch (leveltype) {
	case DTYPE_TOWN:
		if (gbIsHellfire)
			pSBFile = LoadFileInMem("NLevels\\TownData\\Town.SOL", &dwTiles);
		else
			pSBFile = LoadFileInMem("Levels\\TownData\\Town.SOL", &dwTiles);
		break;
	case DTYPE_CATHEDRAL:
		pSBFile = LoadFileInMem("Levels\\L1Data\\L1.SOL", &dwTiles);
		break;
	case DTYPE_CATACOMBS:
		pSBFile = LoadFileInMem("Levels\\L2Data\\L2.SOL", &dwTiles);
		break;
	case DTYPE_CAVES:
		pSBFile = LoadFileInMem("Levels\\L3Data\\L3.SOL", &dwTiles);
		break;
	case DTYPE_HELL:
		pSBFile = LoadFileInMem("Levels\\L4Data\\L4.SOL", &dwTiles);
		break;
	case DTYPE_NEST:
		pSBFile = LoadFileInMem("NLevels\\L6Data\\L6.SOL", &dwTiles);
		break;
	case DTYPE_CRYPT:
		pSBFile = LoadFileInMem("NLevels\\L5Data\\L5.SOL", &dwTiles);
		break;
	default:
		app_fatal("FillSolidBlockTbls");
	}

	pTmp = pSBFile;

	for (i = 1; i <= dwTiles; i++) {
		bv = *pTmp++;
		if (bv & 1)
			nSolidTable[i] = TRUE;
		if (bv & 2)
			nBlockTable[i] = TRUE;
		if (bv & 4)
			nMissileTable[i] = TRUE;
		if (bv & 8)
			nTransTable[i] = TRUE;
		if (bv & 0x80)
			nTrapTable[i] = TRUE;
		block_lvid[i] = (bv & 0x70) >> 4; /* beta: (bv >> 4) & 7 */
	}

	mem_free_dbg(pSBFile);
}

void SetDungeonMicros()
{
	int i, x, y, lv, blocks;
	WORD *pPiece;
	MICROS *pMap;

	if (leveltype == DTYPE_TOWN) {
		MicroTileLen = 16;
		blocks = 16;
	} else if (leveltype != DTYPE_HELL) {
		MicroTileLen = 10;
		blocks = 10;
	} else {
		MicroTileLen = 12;
		blocks = 16;
	}

	for (y = 0; y < MAXDUNY; y++) {
		for (x = 0; x < MAXDUNX; x++) {
			lv = dPiece[x][y];
			pMap = &dpiece_defs_map_2[x][y];
			if (lv != 0) {
				lv--;
				if (leveltype != DTYPE_HELL && leveltype != DTYPE_TOWN)
					pPiece = (WORD *)&pLevelPieces[20 * lv];
				else
					pPiece = (WORD *)&pLevelPieces[32 * lv];
				for (i = 0; i < blocks; i++)
					pMap->mt[i] = SDL_SwapLE16(pPiece[(i & 1) + blocks - 2 - (i & 0xE)]);
			} else {
				for (i = 0; i < blocks; i++)
					pMap->mt[i] = 0;
			}
		}
	}
}

void DRLG_InitTrans()
{
	memset(dTransVal, 0, sizeof(dTransVal));
	memset(TransList, 0, sizeof(TransList));
	TransVal = 1;
}

void DRLG_MRectTrans(int x1, int y1, int x2, int y2)
{
	int i, j;

	x1 = 2 * x1 + 17;
	y1 = 2 * y1 + 17;
	x2 = 2 * x2 + 16;
	y2 = 2 * y2 + 16;

	for (j = y1; j <= y2; j++) {
		for (i = x1; i <= x2; i++) {
			dTransVal[i][j] = TransVal;
		}
	}

	TransVal++;
}

void DRLG_RectTrans(int x1, int y1, int x2, int y2)
{
	int i, j;

	for (j = y1; j <= y2; j++) {
		for (i = x1; i <= x2; i++) {
			dTransVal[i][j] = TransVal;
		}
	}
	TransVal++;
}

void DRLG_CopyTrans(int sx, int sy, int dx, int dy)
{
	dTransVal[dx][dy] = dTransVal[sx][sy];
}

void DRLG_ListTrans(int num, BYTE *List)
{
	int i;
	BYTE x1, y1, x2, y2;

	for (i = 0; i < num; i++) {
		x1 = *List++;
		y1 = *List++;
		x2 = *List++;
		y2 = *List++;
		DRLG_RectTrans(x1, y1, x2, y2);
	}
}

void DRLG_AreaTrans(int num, BYTE *List)
{
	int i;
	BYTE x1, y1, x2, y2;

	for (i = 0; i < num; i++) {
		x1 = *List++;
		y1 = *List++;
		x2 = *List++;
		y2 = *List++;
		DRLG_RectTrans(x1, y1, x2, y2);
		TransVal--;
	}
	TransVal++;
}

void DRLG_InitSetPC()
{
	setpc_x = 0;
	setpc_y = 0;
	setpc_w = 0;
	setpc_h = 0;
}

void DRLG_SetPC()
{
	int i, j, x, y, w, h;

	w = 2 * setpc_w;
	h = 2 * setpc_h;
	x = 2 * setpc_x + 16;
	y = 2 * setpc_y + 16;

	for (j = 0; j < h; j++) {
		for (i = 0; i < w; i++) {
			dFlags[i + x][j + y] |= BFLAG_POPULATED;
		}
	}
}

void Make_SetPC(int x, int y, int w, int h)
{
	int i, j, dx, dy, dh, dw;

	dw = 2 * w;
	dh = 2 * h;
	dx = 2 * x + 16;
	dy = 2 * y + 16;

	for (j = 0; j < dh; j++) {
		for (i = 0; i < dw; i++) {
			dFlags[i + dx][j + dy] |= BFLAG_POPULATED;
		}
	}
}

BOOL DRLG_WillThemeRoomFit(int floor, int x, int y, int minSize, int maxSize, int *width, int *height)
{
	int ii, xx, yy;
	int xSmallest, ySmallest;
	int xArray[20], yArray[20];
	int xCount, yCount;
	BOOL yFlag, xFlag;

	yFlag = TRUE;
	xFlag = TRUE;
	xCount = 0;
	yCount = 0;

	// BUGFIX: change '&&' to '||' (fixed)
	if (x > DMAXX - maxSize || y > DMAXY - maxSize) {
		return FALSE;
	}
	if (!SkipThemeRoom(x, y)) {
		return FALSE;
	}

	memset(xArray, 0, sizeof(xArray));
	memset(yArray, 0, sizeof(yArray));

	for (ii = 0; ii < maxSize; ii++) {
		if (xFlag) {
			for (xx = x; xx < x + maxSize; xx++) {
				if (dungeon[xx][y + ii] != floor) {
					if (xx >= minSize) {
						break;
					}
					xFlag = FALSE;
				} else {
					xCount++;
				}
			}
			if (xFlag) {
				xArray[ii] = xCount;
				xCount = 0;
			}
		}
		if (yFlag) {
			for (yy = y; yy < y + maxSize; yy++) {
				if (dungeon[x + ii][yy] != floor) {
					if (yy >= minSize) {
						break;
					}
					yFlag = FALSE;
				} else {
					yCount++;
				}
			}
			if (yFlag) {
				yArray[ii] = yCount;
				yCount = 0;
			}
		}
	}

	for (ii = 0; ii < minSize; ii++) {
		if (xArray[ii] < minSize || yArray[ii] < minSize) {
			return FALSE;
		}
	}

	xSmallest = xArray[0];
	ySmallest = yArray[0];

	for (ii = 0; ii < maxSize; ii++) {
		if (xArray[ii] < minSize || yArray[ii] < minSize) {
			break;
		}
		if (xArray[ii] < xSmallest) {
			xSmallest = xArray[ii];
		}
		if (yArray[ii] < ySmallest) {
			ySmallest = yArray[ii];
		}
	}

	*width = xSmallest - 2;
	*height = ySmallest - 2;
	return TRUE;
}

void DRLG_CreateThemeRoom(int themeIndex)
{
	int xx, yy;
	const int lx = themeLoc[themeIndex].x;
	const int ly = themeLoc[themeIndex].y;
	const int hx = lx + themeLoc[themeIndex].width;
	const int hy = ly + themeLoc[themeIndex].height;

	for (yy = ly; yy < hy; yy++) {
		for (xx = lx; xx < hx; xx++) {
			if (leveltype == DTYPE_CATACOMBS) {
				if (yy == ly || yy == hy - 1) {
					dungeon[xx][yy] = 2;
				} else if (xx == lx || xx == hx - 1) {
					dungeon[xx][yy] = 1;
				} else {
					dungeon[xx][yy] = 3;
				}
			}
			if (leveltype == DTYPE_CAVES || leveltype == DTYPE_NEST) {
				if (yy == ly || yy == hy - 1) {
					dungeon[xx][yy] = 134;
				} else if (xx == lx || xx == hx - 1) {
					dungeon[xx][yy] = 137;
				} else {
					dungeon[xx][yy] = 7;
				}
			}
			if (leveltype == DTYPE_HELL) {
				if (yy == ly || yy == hy - 1) {
					dungeon[xx][yy] = 2;
				} else if (xx == lx || xx == hx - 1) {
					dungeon[xx][yy] = 1;
				} else {
					dungeon[xx][yy] = 6;
				}
			}
		}
	}

	if (leveltype == DTYPE_CATACOMBS) {
		dungeon[lx][ly] = 8;
		dungeon[hx - 1][ly] = 7;
		dungeon[lx][hy - 1] = 9;
		dungeon[hx - 1][hy - 1] = 6;
	}
	if (leveltype == DTYPE_CAVES || leveltype == DTYPE_NEST) {
		dungeon[lx][ly] = 150;
		dungeon[hx - 1][ly] = 151;
		dungeon[lx][hy - 1] = 152;
		dungeon[hx - 1][hy - 1] = 138;
	}
	if (leveltype == DTYPE_HELL) {
		dungeon[lx][ly] = 9;
		dungeon[hx - 1][ly] = 16;
		dungeon[lx][hy - 1] = 15;
		dungeon[hx - 1][hy - 1] = 12;
	}

	if (leveltype == DTYPE_CATACOMBS) {
		switch (random_(0, 2)) {
		case 0:
			dungeon[hx - 1][(ly + hy) / 2] = 4;
			break;
		case 1:
			dungeon[(lx + hx) / 2][hy - 1] = 5;
			break;
		}
	}
	if (leveltype == DTYPE_CAVES || leveltype == DTYPE_NEST) {
		switch (random_(0, 2)) {
		case 0:
			dungeon[hx - 1][(ly + hy) / 2] = 147;
			break;
		case 1:
			dungeon[(lx + hx) / 2][hy - 1] = 146;
			break;
		}
	}
	if (leveltype == DTYPE_HELL) {
		switch (random_(0, 2)) {
		case 0:
			yy = (ly + hy) / 2;
			dungeon[hx - 1][yy - 1] = 53;
			dungeon[hx - 1][yy] = 6;
			dungeon[hx - 1][yy + 1] = 52;
			dungeon[hx - 2][yy - 1] = 54;
			break;
		case 1:
			xx = (lx + hx) / 2;
			dungeon[xx - 1][hy - 1] = 57;
			dungeon[xx][hy - 1] = 6;
			dungeon[xx + 1][hy - 1] = 56;
			dungeon[xx][hy - 2] = 59;
			dungeon[xx - 1][hy - 2] = 58;
			break;
		}
	}
}

void DRLG_PlaceThemeRooms(int minSize, int maxSize, int floor, int freq, int rndSize)
{
	int i, j;
	int themeW, themeH;
	int rv2, min, max;

	themeCount = 0;
	memset(themeLoc, 0, sizeof(*themeLoc));
	for (j = 0; j < DMAXY; j++) {
		for (i = 0; i < DMAXX; i++) {
			if (dungeon[i][j] == floor && !random_(0, freq) && DRLG_WillThemeRoomFit(floor, i, j, minSize, maxSize, &themeW, &themeH)) {
				if (rndSize) {
					min = minSize - 2;
					max = maxSize - 2;
					rv2 = min + random_(0, random_(0, themeW - min + 1));
					if (rv2 >= min && rv2 <= max)
						themeW = rv2;
					else
						themeW = min;
					rv2 = min + random_(0, random_(0, themeH - min + 1));
					if (rv2 >= min && rv2 <= max)
						themeH = rv2;
					else
						themeH = min;
				}
				themeLoc[themeCount].x = i + 1;
				themeLoc[themeCount].y = j + 1;
				themeLoc[themeCount].width = themeW;
				themeLoc[themeCount].height = themeH;
				if (leveltype == DTYPE_CAVES || leveltype == DTYPE_NEST)
					DRLG_RectTrans(2 * i + 20, 2 * j + 20, 2 * (i + themeW) + 15, 2 * (j + themeH) + 15);
				else
					DRLG_MRectTrans(i + 1, j + 1, i + themeW, j + themeH);
				themeLoc[themeCount].ttval = TransVal - 1;
				DRLG_CreateThemeRoom(themeCount);
				themeCount++;
			}
		}
	}
}

void DRLG_HoldThemeRooms()
{
	int i, x, y, xx, yy;

	for (i = 0; i < themeCount; i++) {
		for (y = themeLoc[i].y; y < themeLoc[i].y + themeLoc[i].height - 1; y++) {
			for (x = themeLoc[i].x; x < themeLoc[i].x + themeLoc[i].width - 1; x++) {
				xx = 2 * x + 16;
				yy = 2 * y + 16;
				dFlags[xx][yy] |= BFLAG_POPULATED;
				dFlags[xx + 1][yy] |= BFLAG_POPULATED;
				dFlags[xx][yy + 1] |= BFLAG_POPULATED;
				dFlags[xx + 1][yy + 1] |= BFLAG_POPULATED;
			}
		}
	}
}

BOOL SkipThemeRoom(int x, int y)
{
	int i;

	for (i = 0; i < themeCount; i++) {
		if (x >= themeLoc[i].x - 2 && x <= themeLoc[i].x + themeLoc[i].width + 2
		    && y >= themeLoc[i].y - 2 && y <= themeLoc[i].y + themeLoc[i].height + 2)
			return FALSE;
	}

	return TRUE;
}

void InitLevels()
{
#ifdef _DEBUG
	if (leveldebug)
		return;
#endif

	currlevel = 0;
	leveltype = DTYPE_TOWN;
	setlevel = FALSE;
}

DEVILUTION_END_NAMESPACE

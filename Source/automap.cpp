#include "diablo.h"

DEVILUTION_BEGIN_NAMESPACE

// BUGFIX: only the first 256 elements are ever read
WORD automaptype[512];
static int MapX;
static int MapY;
BOOL automapflag;
char AmShiftTab[32]; // [31]?
BOOLEAN automapview[DMAXX][DMAXY];
int AutoMapScale;
int AutoMapXOfs;
int AutoMapYOfs;
int AutoMapPosBits;
int AutoMapXPos;
int AutoMapYPos;
int AMPlayerX;
int AMPlayerY;

// color used to draw the player's arrow
#define COLOR_PLAYER (PAL8_ORANGE + 1)
// color for bright map lines (doors, stairs etc.)
#define COLOR_BRIGHT PAL8_YELLOW
// color for dim map lines/dots
#define COLOR_DIM (PAL16_YELLOW + 8)

#define MAPFLAG_TYPE 0x000F
// these are in the second byte
#define MAPFLAG_VERTDOOR 0x01
#define MAPFLAG_HORZDOOR 0x02
#define MAPFLAG_VERTARCH 0x04
#define MAPFLAG_HORZARCH 0x08
#define MAPFLAG_VERTGRATE 0x10
#define MAPFLAG_HORZGRATE 0x20
#define MAPFLAG_SQUARE 0x40
#define MAPFLAG_STAIRS 0x80

void InitAutomapOnce()
{
	automapflag = FALSE;
	AutoMapScale = 50;
	AutoMapPosBits = 32;
	AutoMapXPos = 16;
	AutoMapYPos = 8;
	AMPlayerX = 4;
	AMPlayerY = 2;
}

void InitAutomap()
{
	BYTE b1, b2;
	DWORD dwTiles;
	int x, y;
	BYTE *pAFile, *pTmp;
	int i, j;
	int d;

	j = 50;

	for (i = 0; i < 31; i++) {
		d = (j << 6) / 100;
		AmShiftTab[i] = 2 * (320 / d) + 1;
		if (320 % d)
			AmShiftTab[i]++;
		if (320 % d >= (j << 5) / 100)
			AmShiftTab[i]++;
		j += 5;
	}

	memset(automaptype, 0, sizeof(automaptype));

	switch (leveltype) {
	case DTYPE_CATHEDRAL:
		pAFile = LoadFileInMem("Levels\\L1Data\\L1.AMP", &dwTiles);
		dwTiles >>= 1;
		break;
	case DTYPE_CATACOMBS:
		pAFile = LoadFileInMem("Levels\\L2Data\\L2.AMP", &dwTiles);
		dwTiles >>= 1;
		break;
	case DTYPE_CAVES:
		pAFile = LoadFileInMem("Levels\\L3Data\\L3.AMP", &dwTiles);
		dwTiles >>= 1;
		break;
	case DTYPE_HELL:
		pAFile = LoadFileInMem("Levels\\L4Data\\L4.AMP", &dwTiles);
		dwTiles >>= 1;
		break;
	default:
		return;
	}

	pTmp = pAFile;

	for (i = 1; i <= dwTiles; i++) {
		b1 = *pTmp++;
		b2 = *pTmp++;
		automaptype[i] = b1 + (b2 << 8);
	}

	mem_free_dbg(pAFile);
	memset(automapview, 0, sizeof(automapview));

	for (y = 0; y < MAXDUNY; y++) {
		for (x = 0; x < MAXDUNX; x++)
			dFlags[x][y] &= ~BFLAG_EXPLORED;
	}
}

void StartAutomap()
{
	AutoMapXOfs = 0;
	AutoMapYOfs = 0;
	automapflag = TRUE;
}

void AutomapUp()
{
	AutoMapXOfs--;
	AutoMapYOfs--;
}

void AutomapDown()
{
	AutoMapXOfs++;
	AutoMapYOfs++;
}

void AutomapLeft()
{
	AutoMapXOfs--;
	AutoMapYOfs++;
}

void AutomapRight()
{
	AutoMapXOfs++;
	AutoMapYOfs--;
}

void AutomapZoomIn()
{
	if (AutoMapScale < 200) {
		AutoMapScale += 5;
		AutoMapPosBits = (AutoMapScale << 6) / 100;
		AutoMapXPos = AutoMapPosBits >> 1;
		AutoMapYPos = AutoMapXPos >> 1;
		AMPlayerX = AutoMapYPos >> 1;
		AMPlayerY = AMPlayerX >> 1;
	}
}

void AutomapZoomOut()
{
	if (AutoMapScale > 50) {
		AutoMapScale -= 5;
		AutoMapPosBits = (AutoMapScale << 6) / 100;
		AutoMapXPos = AutoMapPosBits >> 1;
		AutoMapYPos = AutoMapXPos >> 1;
		AMPlayerX = AutoMapYPos >> 1;
		AMPlayerY = AMPlayerX >> 1;
	}
}

void DrawAutomap()
{
	int cells;
	int sx, sy;
	int i, j;
	int mapx, mapy;

	if (leveltype == DTYPE_TOWN) {
		DrawAutomapGame();
		return;
	}

	gpBufEnd = &gpBuffer[(PANEL_Y) * BUFFER_WIDTH];

	MapX = (ViewX - 16) >> 1;
	while (MapX + AutoMapXOfs < 0)
		AutoMapXOfs++;
	while (MapX + AutoMapXOfs >= DMAXX)
		AutoMapXOfs--;
	MapX += AutoMapXOfs;

	MapY = (ViewY - 16) >> 1;
	while (MapY + AutoMapYOfs < 0)
		AutoMapYOfs++;
	while (MapY + AutoMapYOfs >= DMAXY)
		AutoMapYOfs--;
	MapY += AutoMapYOfs;

	cells = AmShiftTab[(AutoMapScale - 50) / 5];
	if (ScrollInfo._sxoff + ScrollInfo._syoff)
		cells++;
	mapx = MapX - cells;
	mapy = MapY - 1;

	if (cells & 1) {
		sx = 384 - AutoMapPosBits * ((cells - 1) >> 1);
		sy = 336 - AutoMapXPos * ((cells + 1) >> 1);
	} else {
		sx = 384 - AutoMapPosBits * (cells >> 1) + AutoMapXPos;
		sy = 336 - AutoMapXPos * (cells >> 1) - AutoMapYPos;
	}
	if (ViewX & 1) {
		sx -= AutoMapYPos;
		sy -= AMPlayerX;
	}
	if (ViewY & 1) {
		sx += AutoMapYPos;
		sy -= AMPlayerX;
	}

	sx += AutoMapScale * ScrollInfo._sxoff / 100 >> 1;
	sy += AutoMapScale * ScrollInfo._syoff / 100 >> 1;
	if (invflag || sbookflag) {
		sx -= 160;
	}
	if (chrflag || questlog) {
		sx += 160;
	}

	for (i = 0; i <= cells + 1; i++) {
		int x = sx;
		int y;

		for (j = 0; j < cells; j++) {
			WORD maptype = GetAutomapType(mapx + j, mapy - j, TRUE);
			if (maptype)
				DrawAutomapType(x, sy, maptype);
			x += AutoMapPosBits;
		}
		mapy++;
		x = sx - AutoMapXPos;
		y = sy + AutoMapYPos;
		for (j = 0; j <= cells; j++) {
			WORD maptype = GetAutomapType(mapx + j, mapy - j, TRUE);
			if (maptype)
				DrawAutomapType(x, y, maptype);
			x += AutoMapPosBits;
		}
		mapx++;
		sy += AutoMapXPos;
	}
	DrawAutomapPlr();
	DrawAutomapGame();
}

void DrawAutomapType(int sx, int sy, WORD automap_type)
{
	BOOL do_vert;
	BOOL do_horz;
	BOOL do_cave_horz;
	BOOL do_cave_vert;
	int x1, y1, x2, y2;

	BYTE flags = automap_type >> 8;

	if (flags & MAPFLAG_SQUARE) {
		ENG_set_pixel(sx, sy, COLOR_DIM);
		ENG_set_pixel(sx - AMPlayerX, sy - AMPlayerY, COLOR_DIM);
		ENG_set_pixel(sx - AMPlayerX, sy + AMPlayerY, COLOR_DIM);
		ENG_set_pixel(sx + AMPlayerX, sy - AMPlayerY, COLOR_DIM);
		ENG_set_pixel(sx + AMPlayerX, sy + AMPlayerY, COLOR_DIM);
		ENG_set_pixel(sx - AutoMapYPos, sy, COLOR_DIM);
		ENG_set_pixel(sx + AutoMapYPos, sy, COLOR_DIM);
		ENG_set_pixel(sx, sy - AMPlayerX, COLOR_DIM);
		ENG_set_pixel(sx, sy + AMPlayerX, COLOR_DIM);
		ENG_set_pixel(sx + AMPlayerX - AutoMapXPos, sy + AMPlayerY, COLOR_DIM);
		ENG_set_pixel(sx - AMPlayerX + AutoMapXPos, sy + AMPlayerY, COLOR_DIM);
		ENG_set_pixel(sx - AutoMapYPos, sy + AMPlayerX, COLOR_DIM);
		ENG_set_pixel(sx + AutoMapYPos, sy + AMPlayerX, COLOR_DIM);
		ENG_set_pixel(sx - AMPlayerX, sy + AutoMapYPos - AMPlayerY, COLOR_DIM);
		ENG_set_pixel(sx + AMPlayerX, sy + AutoMapYPos - AMPlayerY, COLOR_DIM);
		ENG_set_pixel(sx, sy + AutoMapYPos, COLOR_DIM);
	}

	if (flags & MAPFLAG_STAIRS) {
		DrawLine(sx - AMPlayerX, sy - AMPlayerX - AMPlayerY, sx + AMPlayerX + AutoMapYPos, sy + AMPlayerY, COLOR_BRIGHT);
		DrawLine(sx - AutoMapYPos, sy - AMPlayerX, sx + AutoMapYPos, sy + AMPlayerX, COLOR_BRIGHT);
		DrawLine(sx - AutoMapYPos - AMPlayerX, sy - AMPlayerY, sx + AMPlayerX, sy + AMPlayerX + AMPlayerY, COLOR_BRIGHT);
		DrawLine(sx - AutoMapXPos, sy, sx, sy + AutoMapYPos, COLOR_BRIGHT);
	}

	do_vert = FALSE;
	do_horz = FALSE;
	do_cave_horz = FALSE;
	do_cave_vert = FALSE;
	switch (automap_type & MAPFLAG_TYPE) {
	case 1: // stand-alone column or other unpassable object
		x1 = sx - AutoMapYPos;
		y1 = sy - AutoMapYPos;
		x2 = x1 + AutoMapXPos;
		y2 = sy - AMPlayerX;
		DrawLine(sx, y1, x1, y2, COLOR_DIM);
		DrawLine(sx, y1, x2, y2, COLOR_DIM);
		DrawLine(sx, sy, x1, y2, COLOR_DIM);
		DrawLine(sx, sy, x2, y2, COLOR_DIM);
		return;
	case 2:
	case 5:
		do_vert = TRUE;
		break;
	case 4:
		do_vert = TRUE;
		do_horz = TRUE;
		break;
	case 3:
	case 6:
		do_horz = TRUE;
		break;
	case 8:
		do_vert = TRUE;
		do_cave_horz = TRUE;
		break;
	case 9:
		do_horz = TRUE;
		do_cave_vert = TRUE;
		break;
	case 10:
		do_cave_horz = TRUE;
		break;
	case 11:
		do_cave_vert = TRUE;
		break;
	case 12:
		do_cave_horz = TRUE;
		do_cave_vert = TRUE;
		break;
	}

	if (do_vert) {                      // right-facing obstacle
		if (flags & MAPFLAG_VERTDOOR) { // two wall segments with a door in the middle
			x1 = sx - AutoMapXPos;
			x2 = sx - AutoMapYPos;
			y1 = sy - AutoMapYPos;
			y2 = sy - AMPlayerX;

			DrawLine(sx, y1, sx - AMPlayerX, y1 + AMPlayerY, COLOR_DIM);
			DrawLine(x1, sy, x1 + AMPlayerX, sy - AMPlayerY, COLOR_DIM);
			DrawLine(x2, y1, x1, y2, COLOR_BRIGHT);
			DrawLine(x2, y1, sx, y2, COLOR_BRIGHT);
			DrawLine(x2, sy, x1, y2, COLOR_BRIGHT);
			DrawLine(x2, sy, sx, y2, COLOR_BRIGHT);
		}
		if (flags & MAPFLAG_VERTGRATE) { // right-facing half-wall
			DrawLine(sx - AutoMapYPos, sy - AMPlayerX, sx - AutoMapXPos, sy, COLOR_DIM);
			flags |= MAPFLAG_VERTARCH;
		}
		if (flags & MAPFLAG_VERTARCH) { // window or passable column
			x1 = sx - AutoMapYPos;
			y1 = sy - AutoMapYPos;
			x2 = x1 + AutoMapXPos;
			y2 = sy - AMPlayerX;

			DrawLine(sx, y1, x1, y2, COLOR_DIM);
			DrawLine(sx, y1, x2, y2, COLOR_DIM);
			DrawLine(sx, sy, x1, y2, COLOR_DIM);
			DrawLine(sx, sy, x2, y2, COLOR_DIM);
		}
		if (!(flags & (MAPFLAG_VERTDOOR | MAPFLAG_VERTGRATE | MAPFLAG_VERTARCH)))
			DrawLine(sx, sy - AutoMapYPos, sx - AutoMapXPos, sy, COLOR_DIM);
	}

	if (do_horz) { // left-facing obstacle
		if (flags & MAPFLAG_HORZDOOR) {
			x1 = sx + AutoMapYPos;
			x2 = sx + AutoMapXPos;
			y1 = sy - AutoMapYPos;
			y2 = sy - AMPlayerX;

			DrawLine(sx, y1, sx + AMPlayerX, y1 + AMPlayerY, COLOR_DIM);
			DrawLine(x2, sy, x2 - AMPlayerX, sy - AMPlayerY, COLOR_DIM);
			DrawLine(x1, y1, sx, y2, COLOR_BRIGHT);
			DrawLine(x1, y1, x2, y2, COLOR_BRIGHT);
			DrawLine(x1, sy, sx, y2, COLOR_BRIGHT);
			DrawLine(x1, sy, x2, y2, COLOR_BRIGHT);
		}
		if (flags & MAPFLAG_HORZGRATE) {
			DrawLine(sx + AutoMapYPos, sy - AMPlayerX, sx + AutoMapXPos, sy, COLOR_DIM);
			flags |= MAPFLAG_HORZARCH;
		}
		if (flags & MAPFLAG_HORZARCH) {
			x1 = sx - AutoMapYPos;
			y1 = sy - AutoMapYPos;
			x2 = x1 + AutoMapXPos;
			y2 = sy - AMPlayerX;

			DrawLine(sx, y1, x1, y2, COLOR_DIM);
			DrawLine(sx, y1, x2, y2, COLOR_DIM);
			DrawLine(sx, sy, x1, y2, COLOR_DIM);
			DrawLine(sx, sy, x2, y2, COLOR_DIM);
		}
		if (!(flags & (MAPFLAG_HORZDOOR | MAPFLAG_HORZGRATE | MAPFLAG_HORZARCH)))
			DrawLine(sx, sy - AutoMapYPos, sx + AutoMapXPos, sy, COLOR_DIM);
	}

	// for caves the horz/vert flags are switched
	if (do_cave_horz) {
		if (flags & MAPFLAG_VERTDOOR) {
			x1 = sx - AutoMapXPos;
			x2 = sx - AutoMapYPos;
			y1 = sy + AutoMapYPos;
			y2 = sy + AMPlayerX;

			DrawLine(sx, y1, sx - AMPlayerX, y1 - AMPlayerY, COLOR_DIM);
			DrawLine(x1, sy, x1 + AMPlayerX, sy + AMPlayerY, COLOR_DIM);
			DrawLine(x2, y1, x1, y2, COLOR_BRIGHT);
			DrawLine(x2, y1, sx, y2, COLOR_BRIGHT);
			DrawLine(x2, sy, x1, y2, COLOR_BRIGHT);
			DrawLine(x2, sy, sx, y2, COLOR_BRIGHT);
		} else
			DrawLine(sx, sy + AutoMapYPos, sx - AutoMapXPos, sy, COLOR_DIM);
	}

	if (do_cave_vert) {
		if (flags & MAPFLAG_HORZDOOR) {
			x1 = sx + AutoMapYPos;
			x2 = sx + AutoMapXPos;
			y1 = sy + AutoMapYPos;
			y2 = sy + AMPlayerX;

			DrawLine(sx, y1, sx + AMPlayerX, y1 - AMPlayerY, COLOR_DIM);
			DrawLine(x2, sy, x2 - AMPlayerX, sy + AMPlayerY, COLOR_DIM);
			DrawLine(x1, y1, sx, y2, COLOR_BRIGHT);
			DrawLine(x1, y1, x2, y2, COLOR_BRIGHT);
			DrawLine(x1, sy, sx, y2, COLOR_BRIGHT);
			DrawLine(x1, sy, x2, y2, COLOR_BRIGHT);
		} else
			DrawLine(sx, sy + AutoMapYPos, sx + AutoMapXPos, sy, COLOR_DIM);
	}
}

void DrawAutomapPlr()
{
	int px, py;
	int x, y;

	if (plr[myplr]._pmode == PM_WALK3) {
		x = plr[myplr]._px;
		y = plr[myplr]._py;
		if (plr[myplr]._pdir == DIR_W)
			x++;
		else
			y++;
	} else {
		x = plr[myplr].WorldX;
		y = plr[myplr].WorldY;
	}
	px = x - 2 * AutoMapXOfs - ViewX;
	py = y - 2 * AutoMapYOfs - ViewY;

	x = (plr[myplr]._pxoff * AutoMapScale / 100 >> 1) + (ScrollInfo._sxoff * AutoMapScale / 100 >> 1) + (px - py) * AutoMapYPos + 384;
	y = (plr[myplr]._pyoff * AutoMapScale / 100 >> 1) + (ScrollInfo._syoff * AutoMapScale / 100 >> 1) + (px + py) * AMPlayerX + 336;

	if (invflag || sbookflag)
		x -= 160;
	if (chrflag || questlog)
		x += 160;
	y -= AMPlayerX;

	switch (plr[myplr]._pdir) {
	case DIR_N:
		DrawLine(x, y, x, y - AutoMapYPos, COLOR_PLAYER);
		DrawLine(x, y - AutoMapYPos, x - AMPlayerY, y - AMPlayerX, COLOR_PLAYER);
		DrawLine(x, y - AutoMapYPos, x + AMPlayerY, y - AMPlayerX, COLOR_PLAYER);
		break;
	case DIR_NE:
		DrawLine(x, y, x + AutoMapYPos, y - AMPlayerX, COLOR_PLAYER);
		DrawLine(x + AutoMapYPos, y - AMPlayerX, x + AMPlayerX, y - AMPlayerX, COLOR_PLAYER);
		DrawLine(x + AutoMapYPos, y - AMPlayerX, x + AMPlayerX + AMPlayerY, y, COLOR_PLAYER);
		break;
	case DIR_E:
		DrawLine(x, y, x + AutoMapYPos, y, COLOR_PLAYER);
		DrawLine(x + AutoMapYPos, y, x + AMPlayerX, y - AMPlayerY, COLOR_PLAYER);
		DrawLine(x + AutoMapYPos, y, x + AMPlayerX, y + AMPlayerY, COLOR_PLAYER);
		break;
	case DIR_SE:
		DrawLine(x, y, x + AutoMapYPos, y + AMPlayerX, COLOR_PLAYER);
		DrawLine(x + AutoMapYPos, y + AMPlayerX, x + AMPlayerX + AMPlayerY, y, COLOR_PLAYER);
		DrawLine(x + AutoMapYPos, y + AMPlayerX, x + AMPlayerX, y + AMPlayerX, COLOR_PLAYER);
		break;
	case DIR_S:
		DrawLine(x, y, x, y + AutoMapYPos, COLOR_PLAYER);
		DrawLine(x, y + AutoMapYPos, x + AMPlayerY, y + AMPlayerX, COLOR_PLAYER);
		DrawLine(x, y + AutoMapYPos, x - AMPlayerY, y + AMPlayerX, COLOR_PLAYER);
		break;
	case DIR_SW:
		DrawLine(x, y, x - AutoMapYPos, y + AMPlayerX, COLOR_PLAYER);
		DrawLine(x - AutoMapYPos, y + AMPlayerX, x - AMPlayerY - AMPlayerX, y, COLOR_PLAYER);
		DrawLine(x - AutoMapYPos, y + AMPlayerX, x - AMPlayerX, y + AMPlayerX, COLOR_PLAYER);
		break;
	case DIR_W:
		DrawLine(x, y, x - AutoMapYPos, y, COLOR_PLAYER);
		DrawLine(x - AutoMapYPos, y, x - AMPlayerX, y - AMPlayerY, COLOR_PLAYER);
		DrawLine(x - AutoMapYPos, y, x - AMPlayerX, y + AMPlayerY, COLOR_PLAYER);
		break;
	case DIR_NW:
		DrawLine(x, y, x - AutoMapYPos, y - AMPlayerX, COLOR_PLAYER);
		DrawLine(x - AutoMapYPos, y - AMPlayerX, x - AMPlayerX, y - AMPlayerX, COLOR_PLAYER);
		DrawLine(x - AutoMapYPos, y - AMPlayerX, x - AMPlayerY - AMPlayerX, y, COLOR_PLAYER);
		break;
	}
}

WORD GetAutomapType(int x, int y, BOOL view)
{
	WORD rv;

	if (view && x == -1 && y >= 0 && y < DMAXY && automapview[0][y]) {
		if (GetAutomapType(0, y, FALSE) & (MAPFLAG_SQUARE << 8)) {
			return 0;
		} else {
			return MAPFLAG_SQUARE << 8;
		}
	}

	if (view && y == -1 && x >= 0 && x < DMAXY && automapview[x][0]) {
		if (GetAutomapType(x, 0, FALSE) & (MAPFLAG_SQUARE << 8)) {
			return 0;
		} else {
			return MAPFLAG_SQUARE << 8;
		}
	}

	if (x < 0 || x >= DMAXX) {
		return 0;
	}
	if (y < 0 || y >= DMAXX) {
		return 0;
	}
	if (!automapview[x][y] && view) {
		return 0;
	}

	rv = automaptype[(BYTE)dungeon[x][y]];
	if (rv == 7
	    && GetAutomapType(x - 1, y, FALSE) & (MAPFLAG_HORZARCH << 8)
	    && GetAutomapType(x, y - 1, FALSE) & (MAPFLAG_VERTARCH << 8)) {
		rv = 1;
	}
	return rv;
}

void DrawAutomapGame()
{
	char desc[256];
	int nextline = 20;

	if (gbMaxPlayers > 1) {
		strcat(strcpy(desc, "game: "), szPlayerName);
		PrintGameStr(8, 20, desc, COL_GOLD);
		nextline = 35;
		if (szPlayerDescript[0]) {
			strcat(strcpy(desc, "password: "), szPlayerDescript);
			PrintGameStr(8, 35, desc, COL_GOLD);
			nextline = 50;
		}
	}
	if (setlevel)
		PrintGameStr(8, nextline, quest_level_names[(BYTE)setlvlnum], COL_GOLD);
	else if (currlevel) {
		sprintf(desc, "Level: %i", currlevel);
		PrintGameStr(8, nextline, desc, COL_GOLD);
	}
}

void SetAutomapView(int x, int y)
{
	WORD maptype, solid;
	int xx, yy;

	xx = (x - 16) >> 1;
	yy = (y - 16) >> 1;

	if (xx < 0 || xx >= DMAXX || yy < 0 || yy >= DMAXY) {
		return;
	}

	automapview[xx][yy] = 1;

	maptype = GetAutomapType(xx, yy, FALSE);
	solid = maptype & 0x4000;

	switch (maptype & 0xF) {
	case 2:
		if (solid) {
			if (GetAutomapType(xx, yy + 1, FALSE) == 0x4007)
				automapview[xx][yy + 1] = 1;
		} else if (GetAutomapType(xx - 1, yy, FALSE) & 0x4000) {
			automapview[xx - 1][yy] = 1;
		}
		break;
	case 3:
		if (solid) {
			if (GetAutomapType(xx + 1, yy, FALSE) == 0x4007)
				automapview[xx + 1][yy] = 1;
		} else if (GetAutomapType(xx, yy - 1, FALSE) & 0x4000) {
			automapview[xx][yy - 1] = 1;
		}
		break;
	case 4:
		if (solid) {
			if (GetAutomapType(xx, yy + 1, FALSE) == 0x4007)
				automapview[xx][yy + 1] = 1;
			if (GetAutomapType(xx + 1, yy, FALSE) == 0x4007)
				automapview[xx + 1][yy] = 1;
		} else {
			if (GetAutomapType(xx - 1, yy, FALSE) & 0x4000)
				automapview[xx - 1][yy] = 1;
			if (GetAutomapType(xx, yy - 1, FALSE) & 0x4000)
				automapview[xx][yy - 1] = 1;
			if (GetAutomapType(xx - 1, yy - 1, FALSE) & 0x4000)
				automapview[xx - 1][yy - 1] = 1;
		}
		break;
	case 5:
		if (solid) {
			if (GetAutomapType(xx, yy - 1, FALSE) & 0x4000)
				automapview[xx][yy - 1] = 1;
			if (GetAutomapType(xx, yy + 1, FALSE) == 0x4007)
				automapview[xx][yy + 1] = 1;
		} else if (GetAutomapType(xx - 1, yy, FALSE) & 0x4000) {
			automapview[xx - 1][yy] = 1;
		}
		break;
	case 6:
		if (solid) {
			if (GetAutomapType(xx - 1, yy, FALSE) & 0x4000)
				automapview[xx - 1][yy] = 1;
			if (GetAutomapType(xx + 1, yy, FALSE) == 0x4007)
				automapview[xx + 1][yy] = 1;
		} else if (GetAutomapType(xx, yy - 1, FALSE) & 0x4000) {
			automapview[xx][yy - 1] = 1;
		}
		break;
	}
}

void AutomapZoomReset()
{
	AutoMapXOfs = 0;
	AutoMapYOfs = 0;
	AutoMapPosBits = (AutoMapScale << 6) / 100;
	AutoMapXPos = AutoMapPosBits >> 1;
	AutoMapYPos = AutoMapXPos >> 1;
	AMPlayerX = AutoMapYPos >> 1;
	AMPlayerY = AMPlayerX >> 1;
}

DEVILUTION_END_NAMESPACE

/**
 * @file automap.cpp
 *
 * Implementation of the in-game map overlay.
 */
#include "automap.h"

#include "control.h"
#include "inv.h"
#include "miniwin/miniwin.h"
#include "monster.h"
#include "palette.h"
#include "player.h"
#include "setmaps.h"
#include "utils/ui_fwd.h"

namespace devilution {

namespace {
/**
 * Maps from tile_id to automap type.
 * BUGFIX: only the first 256 elements are ever read
 */
uint16_t AutomapTypes[512];

int AutoMapX;
int AutoMapY;

/** color used to draw the player's arrow */
#define COLOR_PLAYER (PAL8_ORANGE + 1)
/** color for bright map lines (doors, stairs etc.) */
#define COLOR_BRIGHT PAL8_YELLOW
/** color for dim map lines/dots */
#define COLOR_DIM (PAL16_YELLOW + 8)
// color for items on automap
#define COLOR_ITEM (PAL8_BLUE + 1)

#define MAPFLAG_TYPE 0x000F
/** these are in the second byte */
#define MAPFLAG_VERTDOOR 0x01
#define MAPFLAG_HORZDOOR 0x02
#define MAPFLAG_VERTARCH 0x04
#define MAPFLAG_HORZARCH 0x08
#define MAPFLAG_VERTGRATE 0x10
#define MAPFLAG_HORZGRATE 0x20
#define MAPFLAG_DIRT 0x40
#define MAPFLAG_STAIRS 0x80

/**
 * @brief Renders the given automap shape at the specified screen coordinates.
 */
void DrawAutomapTile(const CelOutputBuffer &out, int sx, int sy, uint16_t automapType)
{
	uint8_t flags = automapType >> 8;

	if ((flags & MAPFLAG_DIRT) != 0) {
		SetPixel(out, sx, sy, COLOR_DIM);
		SetPixel(out, sx - AmLine8, sy - AmLine4, COLOR_DIM);
		SetPixel(out, sx - AmLine8, sy + AmLine4, COLOR_DIM);
		SetPixel(out, sx + AmLine8, sy - AmLine4, COLOR_DIM);
		SetPixel(out, sx + AmLine8, sy + AmLine4, COLOR_DIM);
		SetPixel(out, sx - AmLine16, sy, COLOR_DIM);
		SetPixel(out, sx + AmLine16, sy, COLOR_DIM);
		SetPixel(out, sx, sy - AmLine8, COLOR_DIM);
		SetPixel(out, sx, sy + AmLine8, COLOR_DIM);
		SetPixel(out, sx + AmLine8 - AmLine32, sy + AmLine4, COLOR_DIM);
		SetPixel(out, sx - AmLine8 + AmLine32, sy + AmLine4, COLOR_DIM);
		SetPixel(out, sx - AmLine16, sy + AmLine8, COLOR_DIM);
		SetPixel(out, sx + AmLine16, sy + AmLine8, COLOR_DIM);
		SetPixel(out, sx - AmLine8, sy + AmLine16 - AmLine4, COLOR_DIM);
		SetPixel(out, sx + AmLine8, sy + AmLine16 - AmLine4, COLOR_DIM);
		SetPixel(out, sx, sy + AmLine16, COLOR_DIM);
	}

	if ((flags & MAPFLAG_STAIRS) != 0) {
		DrawLineTo(out, sx - AmLine8, sy - AmLine8 - AmLine4, sx + AmLine8 + AmLine16, sy + AmLine4, COLOR_BRIGHT);
		DrawLineTo(out, sx - AmLine16, sy - AmLine8, sx + AmLine16, sy + AmLine8, COLOR_BRIGHT);
		DrawLineTo(out, sx - AmLine16 - AmLine8, sy - AmLine4, sx + AmLine8, sy + AmLine8 + AmLine4, COLOR_BRIGHT);
		DrawLineTo(out, sx - AmLine32, sy, sx, sy + AmLine16, COLOR_BRIGHT);
	}

	bool doVert = false;
	bool doHorz = false;
	bool doCaveHorz = false;
	bool doCaveVert = false;
	switch (automapType & MAPFLAG_TYPE) {
	case 1: { // stand-alone column or other unpassable object
		int x1 = sx - AmLine16;
		int y1 = sy - AmLine16;
		int x2 = x1 + AmLine32;
		int y2 = sy - AmLine8;
		DrawLineTo(out, sx, y1, x1, y2, COLOR_DIM);
		DrawLineTo(out, sx, y1, x2, y2, COLOR_DIM);
		DrawLineTo(out, sx, sy, x1, y2, COLOR_DIM);
		DrawLineTo(out, sx, sy, x2, y2, COLOR_DIM);

	} break;
	case 2:
	case 5:
		doVert = true;
		break;
	case 3:
	case 6:
		doHorz = true;
		break;
	case 4:
		doVert = true;
		doHorz = true;
		break;
	case 8:
		doVert = true;
		doCaveHorz = true;
		break;
	case 9:
		doHorz = true;
		doCaveVert = true;
		break;
	case 10:
		doCaveHorz = true;
		break;
	case 11:
		doCaveVert = true;
		break;
	case 12:
		doCaveHorz = true;
		doCaveVert = true;
		break;
	}

	if (doVert) {                              // right-facing obstacle
		if ((flags & MAPFLAG_VERTDOOR) != 0) { // two wall segments with a door in the middle
			int x1 = sx - AmLine32;
			int x2 = sx - AmLine16;
			int y1 = sy - AmLine16;
			int y2 = sy - AmLine8;

			DrawLineTo(out, sx, y1, sx - AmLine8, y1 + AmLine4, COLOR_DIM);
			DrawLineTo(out, x1, sy, x1 + AmLine8, sy - AmLine4, COLOR_DIM);
			DrawLineTo(out, x2, y1, x1, y2, COLOR_BRIGHT);
			DrawLineTo(out, x2, y1, sx, y2, COLOR_BRIGHT);
			DrawLineTo(out, x2, sy, x1, y2, COLOR_BRIGHT);
			DrawLineTo(out, x2, sy, sx, y2, COLOR_BRIGHT);
		}
		if ((flags & MAPFLAG_VERTGRATE) != 0) { // right-facing half-wall
			DrawLineTo(out, sx - AmLine16, sy - AmLine8, sx - AmLine32, sy, COLOR_DIM);
			flags |= MAPFLAG_VERTARCH;
		}
		if ((flags & MAPFLAG_VERTARCH) != 0) { // window or passable column
			int x1 = sx - AmLine16;
			int y1 = sy - AmLine16;
			int x2 = x1 + AmLine32;
			int y2 = sy - AmLine8;

			DrawLineTo(out, sx, y1, x1, y2, COLOR_DIM);
			DrawLineTo(out, sx, y1, x2, y2, COLOR_DIM);
			DrawLineTo(out, sx, sy, x1, y2, COLOR_DIM);
			DrawLineTo(out, sx, sy, x2, y2, COLOR_DIM);
		}
		if ((flags & (MAPFLAG_VERTDOOR | MAPFLAG_VERTGRATE | MAPFLAG_VERTARCH)) == 0)
			DrawLineTo(out, sx, sy - AmLine16, sx - AmLine32, sy, COLOR_DIM);
	}

	if (doHorz) { // left-facing obstacle
		if ((flags & MAPFLAG_HORZDOOR) != 0) {
			int x1 = sx + AmLine16;
			int x2 = sx + AmLine32;
			int y1 = sy - AmLine16;
			int y2 = sy - AmLine8;

			DrawLineTo(out, sx, y1, sx + AmLine8, y1 + AmLine4, COLOR_DIM);
			DrawLineTo(out, x2, sy, x2 - AmLine8, sy - AmLine4, COLOR_DIM);
			DrawLineTo(out, x1, y1, sx, y2, COLOR_BRIGHT);
			DrawLineTo(out, x1, y1, x2, y2, COLOR_BRIGHT);
			DrawLineTo(out, x1, sy, sx, y2, COLOR_BRIGHT);
			DrawLineTo(out, x1, sy, x2, y2, COLOR_BRIGHT);
		}
		if ((flags & MAPFLAG_HORZGRATE) != 0) {
			DrawLineTo(out, sx + AmLine16, sy - AmLine8, sx + AmLine32, sy, COLOR_DIM);
			flags |= MAPFLAG_HORZARCH;
		}
		if ((flags & MAPFLAG_HORZARCH) != 0) {
			int x1 = sx - AmLine16;
			int y1 = sy - AmLine16;
			int x2 = x1 + AmLine32;
			int y2 = sy - AmLine8;

			DrawLineTo(out, sx, y1, x1, y2, COLOR_DIM);
			DrawLineTo(out, sx, y1, x2, y2, COLOR_DIM);
			DrawLineTo(out, sx, sy, x1, y2, COLOR_DIM);
			DrawLineTo(out, sx, sy, x2, y2, COLOR_DIM);
		}
		if ((flags & (MAPFLAG_HORZDOOR | MAPFLAG_HORZGRATE | MAPFLAG_HORZARCH)) == 0)
			DrawLineTo(out, sx, sy - AmLine16, sx + AmLine32, sy, COLOR_DIM);
	}

	// for caves the horz/vert flags are switched
	if (doCaveHorz) {
		if ((flags & MAPFLAG_VERTDOOR) != 0) {
			int x1 = sx - AmLine32;
			int x2 = sx - AmLine16;
			int y1 = sy + AmLine16;
			int y2 = sy + AmLine8;

			DrawLineTo(out, sx, y1, sx - AmLine8, y1 - AmLine4, COLOR_DIM);
			DrawLineTo(out, x1, sy, x1 + AmLine8, sy + AmLine4, COLOR_DIM);
			DrawLineTo(out, x2, y1, x1, y2, COLOR_BRIGHT);
			DrawLineTo(out, x2, y1, sx, y2, COLOR_BRIGHT);
			DrawLineTo(out, x2, sy, x1, y2, COLOR_BRIGHT);
			DrawLineTo(out, x2, sy, sx, y2, COLOR_BRIGHT);
		} else
			DrawLineTo(out, sx, sy + AmLine16, sx - AmLine32, sy, COLOR_DIM);
	}

	if (doCaveVert) {
		if ((flags & MAPFLAG_HORZDOOR) != 0) {
			int x1 = sx + AmLine16;
			int x2 = sx + AmLine32;
			int y1 = sy + AmLine16;
			int y2 = sy + AmLine8;

			DrawLineTo(out, sx, y1, sx + AmLine8, y1 - AmLine4, COLOR_DIM);
			DrawLineTo(out, x2, sy, x2 - AmLine8, sy + AmLine4, COLOR_DIM);
			DrawLineTo(out, x1, y1, sx, y2, COLOR_BRIGHT);
			DrawLineTo(out, x1, y1, x2, y2, COLOR_BRIGHT);
			DrawLineTo(out, x1, sy, sx, y2, COLOR_BRIGHT);
			DrawLineTo(out, x1, sy, x2, y2, COLOR_BRIGHT);
		} else {
			DrawLineTo(out, sx, sy + AmLine16, sx + AmLine32, sy, COLOR_DIM);
		}
	}
}

void DrawAutomapItem(const CelOutputBuffer &out, int x, int y, uint8_t color)
{
	int x1 = x - AmLine32 / 2;
	int y1 = y - AmLine16 / 2;
	int x2 = x1 + AmLine64 / 2;
	int y2 = y1 + AmLine32 / 2;
	DrawLineTo(out, x, y1, x1, y, color);
	DrawLineTo(out, x, y1, x2, y, color);
	DrawLineTo(out, x, y2, x1, y, color);
	DrawLineTo(out, x, y2, x2, y, color);
}

void SearchAutomapItem(const CelOutputBuffer &out)
{
	int x = plr[myplr]._px;
	int y = plr[myplr]._py;
	if (plr[myplr]._pmode == PM_WALK3) {
		x = plr[myplr]._pfutx;
		y = plr[myplr]._pfuty;
		if (plr[myplr]._pdir == DIR_W)
			x++;
		else
			y++;
	}

	int x1 = x - 8;
	if (x1 < 0)
		x1 = 0;
	else if (x1 > MAXDUNX)
		x1 = MAXDUNX;

	int y1 = y - 8;
	if (y1 < 0)
		y1 = 0;
	else if (y1 > MAXDUNY)
		y1 = MAXDUNY;

	int x2 = x + 8;
	if (x2 < 0)
		x2 = 0;
	else if (x2 > MAXDUNX)
		x2 = MAXDUNX;

	int y2 = y + 8;
	if (y2 < 0)
		y2 = 0;
	else if (y2 > MAXDUNY)
		y2 = MAXDUNY;

	for (int i = x1; i < x2; i++) {
		for (int j = y1; j < y2; j++) {
			if (dItem[i][j] != 0) {
				int px = i - 2 * AutoMapXOfs - ViewX;
				int py = j - 2 * AutoMapYOfs - ViewY;

				x = (ScrollInfo._sxoff * AutoMapScale / 100 >> 1) + (px - py) * AmLine16 + gnScreenWidth / 2;
				y = (ScrollInfo._syoff * AutoMapScale / 100 >> 1) + (px + py) * AmLine8 + (gnScreenHeight - PANEL_HEIGHT) / 2;

				if (PANELS_COVER) {
					if (invflag || sbookflag)
						x -= 160;
					if (chrflag || questlog)
						x += 160;
				}
				y -= AmLine8;
				DrawAutomapItem(out, x, y, COLOR_ITEM);
			}
		}
	}
}

/**
 * @brief Renders an arrow on the automap, centered on and facing the direction of the player.
 */
void DrawAutomapPlr(const CelOutputBuffer &out, int pnum)
{
	int x;
	int y;

	int playerColor = COLOR_PLAYER + (8 * pnum) % 128;

	if (plr[pnum]._pmode == PM_WALK3) {
		x = plr[pnum]._pfutx;
		y = plr[pnum]._pfuty;
		if (plr[pnum]._pdir == DIR_W)
			x++;
		else
			y++;
	} else {
		x = plr[pnum]._px;
		y = plr[pnum]._py;
	}
	int px = x - 2 * AutoMapXOfs - ViewX;
	int py = y - 2 * AutoMapYOfs - ViewY;

	x = (plr[pnum]._pxoff * AutoMapScale / 100 >> 1) + (ScrollInfo._sxoff * AutoMapScale / 100 >> 1) + (px - py) * AmLine16 + gnScreenWidth / 2;
	y = (plr[pnum]._pyoff * AutoMapScale / 100 >> 1) + (ScrollInfo._syoff * AutoMapScale / 100 >> 1) + (px + py) * AmLine8 + (gnScreenHeight - PANEL_HEIGHT) / 2;

	if (PANELS_COVER) {
		if (invflag || sbookflag)
			x -= gnScreenWidth / 4;
		if (chrflag || questlog)
			x += gnScreenWidth / 4;
	}
	y -= AmLine8;

	switch (plr[pnum]._pdir) {
	case DIR_N:
		DrawLineTo(out, x, y, x, y - AmLine16, playerColor);
		DrawLineTo(out, x, y - AmLine16, x - AmLine4, y - AmLine8, playerColor);
		DrawLineTo(out, x, y - AmLine16, x + AmLine4, y - AmLine8, playerColor);
		break;
	case DIR_NE:
		DrawLineTo(out, x, y, x + AmLine16, y - AmLine8, playerColor);
		DrawLineTo(out, x + AmLine16, y - AmLine8, x + AmLine8, y - AmLine8, playerColor);
		DrawLineTo(out, x + AmLine16, y - AmLine8, x + AmLine8 + AmLine4, y, playerColor);
		break;
	case DIR_E:
		DrawLineTo(out, x, y, x + AmLine16, y, playerColor);
		DrawLineTo(out, x + AmLine16, y, x + AmLine8, y - AmLine4, playerColor);
		DrawLineTo(out, x + AmLine16, y, x + AmLine8, y + AmLine4, playerColor);
		break;
	case DIR_SE:
		DrawLineTo(out, x, y, x + AmLine16, y + AmLine8, playerColor);
		DrawLineTo(out, x + AmLine16, y + AmLine8, x + AmLine8 + AmLine4, y, playerColor);
		DrawLineTo(out, x + AmLine16, y + AmLine8, x + AmLine8, y + AmLine8, playerColor);
		break;
	case DIR_S:
	case DIR_OMNI:
		DrawLineTo(out, x, y, x, y + AmLine16, playerColor);
		DrawLineTo(out, x, y + AmLine16, x + AmLine4, y + AmLine8, playerColor);
		DrawLineTo(out, x, y + AmLine16, x - AmLine4, y + AmLine8, playerColor);
		break;
	case DIR_SW:
		DrawLineTo(out, x, y, x - AmLine16, y + AmLine8, playerColor);
		DrawLineTo(out, x - AmLine16, y + AmLine8, x - AmLine4 - AmLine8, y, playerColor);
		DrawLineTo(out, x - AmLine16, y + AmLine8, x - AmLine8, y + AmLine8, playerColor);
		break;
	case DIR_W:
		DrawLineTo(out, x, y, x - AmLine16, y, playerColor);
		DrawLineTo(out, x - AmLine16, y, x - AmLine8, y - AmLine4, playerColor);
		DrawLineTo(out, x - AmLine16, y, x - AmLine8, y + AmLine4, playerColor);
		break;
	case DIR_NW:
		DrawLineTo(out, x, y, x - AmLine16, y - AmLine8, playerColor);
		DrawLineTo(out, x - AmLine16, y - AmLine8, x - AmLine8, y - AmLine8, playerColor);
		DrawLineTo(out, x - AmLine16, y - AmLine8, x - AmLine4 - AmLine8, y, playerColor);
		break;
	}
}

/**
 * @brief Returns the automap shape at the given coordinate.
 */
uint16_t GetAutomapType(int x, int y, bool view)
{
	if (view && x == -1 && y >= 0 && y < DMAXY && automapview[0][y]) {
		if ((GetAutomapType(0, y, false) & (MAPFLAG_DIRT << 8)) != 0) {
			return 0;
		}
		return MAPFLAG_DIRT << 8;
	}

	if (view && y == -1 && x >= 0 && x < DMAXY && automapview[x][0]) {
		if ((GetAutomapType(x, 0, false) & (MAPFLAG_DIRT << 8)) != 0) {
			return 0;
		}
		return MAPFLAG_DIRT << 8;
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

	uint16_t rv = AutomapTypes[(BYTE)dungeon[x][y]];
	if (rv == 7) {
		if (((GetAutomapType(x - 1, y, false) >> 8) & MAPFLAG_HORZARCH) != 0) {
			if (((GetAutomapType(x, y - 1, false) >> 8) & MAPFLAG_VERTARCH) != 0) {
				rv = 1;
			}
		}
	}
	return rv;
}

/**
 * @brief Renders game info, such as the name of the current level, and in multi player the name of the game and the game password.
 */
void DrawAutomapText(const CelOutputBuffer &out)
{
	// TODO: Use the `out` buffer instead of the global one.

	char desc[256];
	int nextline = 20;

	if (gbIsMultiplayer) {
		strcat(strcpy(desc, "game: "), szPlayerName);
		PrintGameStr(out, 8, 20, desc, COL_GOLD);
		nextline = 35;
		if (szPlayerDescript[0] != 0) {
			strcat(strcpy(desc, "password: "), szPlayerDescript);
			PrintGameStr(out, 8, 35, desc, COL_GOLD);
			nextline = 50;
		}
	}
	if (setlevel) {
		PrintGameStr(out, 8, nextline, quest_level_names[(BYTE)setlvlnum], COL_GOLD);
	} else if (currlevel != 0) {
		if (currlevel < 17 || currlevel > 20) {
			if (currlevel < 21 || currlevel > 24)
				sprintf(desc, "Level: %i", currlevel);
			else
				sprintf(desc, "Level: Crypt %i", currlevel - 20);
		} else {
			sprintf(desc, "Level: Nest %i", currlevel - 16);
		}
		PrintGameStr(out, 8, nextline, desc, COL_GOLD);
	}
}

} // namespace

bool automapflag;
bool automapview[DMAXX][DMAXY];
int AutoMapScale;
int AutoMapXOfs;
int AutoMapYOfs;
int AmLine64;
int AmLine32;
int AmLine16;
int AmLine8;
int AmLine4;

void InitAutomapOnce()
{
	automapflag = false;
	AutoMapScale = 50;
	AmLine64 = 32;
	AmLine32 = 16;
	AmLine16 = 8;
	AmLine8 = 4;
	AmLine4 = 2;
}

void InitAutomap()
{
	DWORD dwTiles;
	BYTE *pAFile;

	memset(AutomapTypes, 0, sizeof(AutomapTypes));

	switch (leveltype) {
	case DTYPE_CATHEDRAL:
		if (currlevel < 21)
			pAFile = LoadFileInMem("Levels\\L1Data\\L1.AMP", &dwTiles);
		else
			pAFile = LoadFileInMem("NLevels\\L5Data\\L5.AMP", &dwTiles);
		break;
	case DTYPE_CATACOMBS:
		pAFile = LoadFileInMem("Levels\\L2Data\\L2.AMP", &dwTiles);
		break;
	case DTYPE_CAVES:
		if (currlevel < 17)
			pAFile = LoadFileInMem("Levels\\L3Data\\L3.AMP", &dwTiles);
		else
			pAFile = LoadFileInMem("NLevels\\L6Data\\L6.AMP", &dwTiles);
		break;
	case DTYPE_HELL:
		pAFile = LoadFileInMem("Levels\\L4Data\\L4.AMP", &dwTiles);
		break;
	default:
		return;
	}

	dwTiles /= 2;
	BYTE *pTmp = pAFile;

	for (unsigned i = 1; i <= dwTiles; i++) {
		uint8_t b1 = *pTmp++;
		uint8_t b2 = *pTmp++;
		AutomapTypes[i] = b1 + (b2 << 8);
	}

	mem_free_dbg(pAFile);
	memset(automapview, 0, sizeof(automapview));

	for (auto &row : dFlags)
		for (auto &dFlag : row)
			dFlag &= ~BFLAG_EXPLORED;
}

void StartAutomap()
{
	AutoMapXOfs = 0;
	AutoMapYOfs = 0;
	automapflag = true;
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
		AmLine64 = (AutoMapScale << 6) / 100;
		AmLine32 = AmLine64 >> 1;
		AmLine16 = AmLine32 >> 1;
		AmLine8 = AmLine16 >> 1;
		AmLine4 = AmLine8 >> 1;
	}
}

void AutomapZoomOut()
{
	if (AutoMapScale > 50) {
		AutoMapScale -= 5;
		AmLine64 = (AutoMapScale << 6) / 100;
		AmLine32 = AmLine64 >> 1;
		AmLine16 = AmLine32 >> 1;
		AmLine8 = AmLine16 >> 1;
		AmLine4 = AmLine8 >> 1;
	}
}

void DrawAutomap(const CelOutputBuffer &out)
{
	int sx;
	int sy;

	if (leveltype == DTYPE_TOWN) {
		DrawAutomapText(out);
		return;
	}

	AutoMapX = (ViewX - 16) >> 1;
	while (AutoMapX + AutoMapXOfs < 0)
		AutoMapXOfs++;
	while (AutoMapX + AutoMapXOfs >= DMAXX)
		AutoMapXOfs--;
	AutoMapX += AutoMapXOfs;

	AutoMapY = (ViewY - 16) >> 1;
	while (AutoMapY + AutoMapYOfs < 0)
		AutoMapYOfs++;
	while (AutoMapY + AutoMapYOfs >= DMAXY)
		AutoMapYOfs--;
	AutoMapY += AutoMapYOfs;

	int d = (AutoMapScale << 6) / 100;
	int cells = 2 * (gnScreenWidth / 2 / d) + 1;
	if (((gnScreenWidth / 2) % d) != 0)
		cells++;
	if ((gnScreenWidth / 2) % d >= (AutoMapScale << 5) / 100)
		cells++;

	if ((ScrollInfo._sxoff + ScrollInfo._syoff) != 0)
		cells++;
	int mapx = AutoMapX - cells;
	int mapy = AutoMapY - 1;

	if ((cells & 1) != 0) {
		sx = gnScreenWidth / 2 - AmLine64 * ((cells - 1) >> 1);
		sy = (gnScreenHeight - PANEL_HEIGHT) / 2 - AmLine32 * ((cells + 1) >> 1);
	} else {
		sx = gnScreenWidth / 2 - AmLine64 * (cells >> 1) + AmLine32;
		sy = (gnScreenHeight - PANEL_HEIGHT) / 2 - AmLine32 * (cells >> 1) - AmLine16;
	}
	if ((ViewX & 1) != 0) {
		sx -= AmLine16;
		sy -= AmLine8;
	}
	if ((ViewY & 1) != 0) {
		sx += AmLine16;
		sy -= AmLine8;
	}

	sx += AutoMapScale * ScrollInfo._sxoff / 100 >> 1;
	sy += AutoMapScale * ScrollInfo._syoff / 100 >> 1;
	if (PANELS_COVER) {
		if (invflag || sbookflag) {
			sx -= gnScreenWidth / 4;
		}
		if (chrflag || questlog) {
			sx += gnScreenWidth / 4;
		}
	}

	for (int i = 0; i <= cells + 1; i++) {
		int x = sx;
		int y;

		for (int j = 0; j < cells; j++) {
			uint16_t maptype = GetAutomapType(mapx + j, mapy - j, true);
			if (maptype != 0)
				DrawAutomapTile(out, x, sy, maptype);
			x += AmLine64;
		}
		mapy++;
		x = sx - AmLine32;
		y = sy + AmLine16;
		for (int j = 0; j <= cells; j++) {
			uint16_t maptype = GetAutomapType(mapx + j, mapy - j, true);
			if (maptype != 0)
				DrawAutomapTile(out, x, y, maptype);
			x += AmLine64;
		}
		mapx++;
		sy += AmLine32;
	}

	for (int pnum = 0; pnum < MAX_PLRS; pnum++) {
		if (plr[pnum].plrlevel == plr[myplr].plrlevel && plr[pnum].plractive && !plr[pnum]._pLvlChanging) {
			DrawAutomapPlr(out, pnum);
		}
	}
	if (AutoMapShowItems)
		SearchAutomapItem(out);
	DrawAutomapText(out);
}

void SetAutomapView(int x, int y)
{
	int xx = (x - 16) >> 1;
	int yy = (y - 16) >> 1;

	if (xx < 0 || xx >= DMAXX || yy < 0 || yy >= DMAXY) {
		return;
	}

	automapview[xx][yy] = true;

	uint16_t maptype = GetAutomapType(xx, yy, false);
	uint16_t solid = maptype & 0x4000;

	switch (maptype & MAPFLAG_TYPE) {
	case 2:
		if (solid != 0) {
			if (GetAutomapType(xx, yy + 1, false) == 0x4007)
				automapview[xx][yy + 1] = true;
		} else if ((GetAutomapType(xx - 1, yy, false) & 0x4000) != 0) {
			automapview[xx - 1][yy] = true;
		}
		break;
	case 3:
		if (solid != 0) {
			if (GetAutomapType(xx + 1, yy, false) == 0x4007)
				automapview[xx + 1][yy] = true;
		} else if ((GetAutomapType(xx, yy - 1, false) & 0x4000) != 0) {
			automapview[xx][yy - 1] = true;
		}
		break;
	case 4:
		if (solid != 0) {
			if (GetAutomapType(xx, yy + 1, false) == 0x4007)
				automapview[xx][yy + 1] = true;
			if (GetAutomapType(xx + 1, yy, false) == 0x4007)
				automapview[xx + 1][yy] = true;
		} else {
			if ((GetAutomapType(xx - 1, yy, false) & 0x4000) != 0)
				automapview[xx - 1][yy] = true;
			if ((GetAutomapType(xx, yy - 1, false) & 0x4000) != 0)
				automapview[xx][yy - 1] = true;
			if ((GetAutomapType(xx - 1, yy - 1, false) & 0x4000) != 0)
				automapview[xx - 1][yy - 1] = true;
		}
		break;
	case 5:
		if (solid != 0) {
			if ((GetAutomapType(xx, yy - 1, false) & 0x4000) != 0)
				automapview[xx][yy - 1] = true;
			if (GetAutomapType(xx, yy + 1, false) == 0x4007)
				automapview[xx][yy + 1] = true;
		} else if ((GetAutomapType(xx - 1, yy, false) & 0x4000) != 0) {
			automapview[xx - 1][yy] = true;
		}
		break;
	case 6:
		if (solid != 0) {
			if ((GetAutomapType(xx - 1, yy, false) & 0x4000) != 0)
				automapview[xx - 1][yy] = true;
			if (GetAutomapType(xx + 1, yy, false) == 0x4007)
				automapview[xx + 1][yy] = true;
		} else if ((GetAutomapType(xx, yy - 1, false) & 0x4000) != 0) {
			automapview[xx][yy - 1] = true;
		}
		break;
	}
}

void AutomapZoomReset()
{
	AutoMapXOfs = 0;
	AutoMapYOfs = 0;
	AmLine64 = (AutoMapScale << 6) / 100;
	AmLine32 = AmLine64 >> 1;
	AmLine16 = AmLine32 >> 1;
	AmLine8 = AmLine16 >> 1;
	AmLine4 = AmLine8 >> 1;
}

} // namespace devilution

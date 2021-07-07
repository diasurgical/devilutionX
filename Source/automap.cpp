/**
 * @file automap.cpp
 *
 * Implementation of the in-game map overlay.
 */
#include "automap.h"

#include <fmt/format.h>

#include "control.h"
#include "engine/load_file.hpp"
#include "engine/render/automap_render.hpp"
#include "inv.h"
#include "monster.h"
#include "palette.h"
#include "player.h"
#include "setmaps.h"
#include "utils/language.h"
#include "utils/stdcompat/algorithm.hpp"
#include "utils/ui_fwd.h"

namespace devilution {

namespace {
/**
 * Maps from tile_id to automap type.
 */
std::array<uint16_t, 256> AutomapTypes;

Point Automap;

enum MapColors : uint8_t {
	/** color used to draw the player's arrow */
	MapColorsPlayer = (PAL8_ORANGE + 1),
	/** color for bright map lines (doors, stairs etc.) */
	MapColorsBright = PAL8_YELLOW,
	/** color for dim map lines/dots */
	MapColorsDim = (PAL16_YELLOW + 8),
	/** color for items on automap */
	MapColorsItem = (PAL8_BLUE + 1),
};

constexpr uint16_t MapFlagsType = 0x000F;
/** these are in the second byte */
enum MapFlags : uint8_t {
	// clang-format off
	MapFlagsMapVerticalDoor   = 1 << 0,
	MapFlagsMapHorizontalDoor = 1 << 1,
	MapFlagsVerticalArch      = 1 << 2,
	MapFlagsHorizontalArch    = 1 << 3,
	MapFlagsVerticalGrate     = 1 << 4,
	MapFlagsHorizontalGrate   = 1 << 5,
	MapFlagsDirt              = 1 << 6,
	MapFlagsStairs            = 1 << 7,
	// clang-format on
};

void DrawDiamond(const Surface &out, Point center, uint8_t color)
{
	const Point left { center.x - AmLine16, center.y };
	const Point top { center.x, center.y - AmLine8 };
	const Point bottom { center.x, center.y + AmLine8 };

	DrawMapLineNE(out, left, AmLine8, color);
	DrawMapLineSE(out, left, AmLine8, color);
	DrawMapLineSE(out, top, AmLine8, color);
	DrawMapLineNE(out, bottom, AmLine8, color);
}

void DrawMapVerticalDoor(const Surface &out, Point center)
{
	DrawMapLineNE(out, { center.x + AmLine8, center.y - AmLine4 }, AmLine4, MapColorsDim);
	DrawMapLineNE(out, { center.x - AmLine16, center.y + AmLine8 }, AmLine4, MapColorsDim);
	DrawDiamond(out, center, MapColorsBright);
}

void DrawMapHorizontalDoor(const Surface &out, Point center)
{
	DrawMapLineSE(out, { center.x - AmLine16, center.y - AmLine8 }, AmLine4, MapColorsDim);
	DrawMapLineSE(out, { center.x + AmLine8, center.y + AmLine4 }, AmLine4, MapColorsDim);
	DrawDiamond(out, center, MapColorsBright);
}

/**
 * @brief Renders the given automap shape at the specified screen coordinates.
 */
void DrawAutomapTile(const Surface &out, Point center, uint16_t automapType)
{
	uint8_t flags = automapType >> 8;

	if ((flags & MapFlagsDirt) != 0) {
		out.SetPixel(center, MapColorsDim);
		out.SetPixel({ center.x - AmLine8, center.y - AmLine4 }, MapColorsDim);
		out.SetPixel({ center.x - AmLine8, center.y + AmLine4 }, MapColorsDim);
		out.SetPixel({ center.x + AmLine8, center.y - AmLine4 }, MapColorsDim);
		out.SetPixel({ center.x + AmLine8, center.y + AmLine4 }, MapColorsDim);
		out.SetPixel({ center.x - AmLine16, center.y }, MapColorsDim);
		out.SetPixel({ center.x + AmLine16, center.y }, MapColorsDim);
		out.SetPixel({ center.x, center.y - AmLine8 }, MapColorsDim);
		out.SetPixel({ center.x, center.y + AmLine8 }, MapColorsDim);
		out.SetPixel({ center.x + AmLine8 - AmLine32, center.y + AmLine4 }, MapColorsDim);
		out.SetPixel({ center.x - AmLine8 + AmLine32, center.y + AmLine4 }, MapColorsDim);
		out.SetPixel({ center.x - AmLine16, center.y + AmLine8 }, MapColorsDim);
		out.SetPixel({ center.x + AmLine16, center.y + AmLine8 }, MapColorsDim);
		out.SetPixel({ center.x - AmLine8, center.y + AmLine16 - AmLine4 }, MapColorsDim);
		out.SetPixel({ center.x + AmLine8, center.y + AmLine16 - AmLine4 }, MapColorsDim);
		out.SetPixel({ center.x, center.y + AmLine16 }, MapColorsDim);
	}

	if ((flags & MapFlagsStairs) != 0) {
		constexpr int NumStairSteps = 4;
		const Displacement offset = { -AmLine8, AmLine4 };
		Point p = { center.x - AmLine8, center.y - AmLine8 - AmLine4 };
		for (int i = 0; i < NumStairSteps; ++i) {
			DrawMapLineSE(out, p, AmLine16, MapColorsBright);
			p += offset;
		}
	}

	bool drawVertical = false;
	bool drawHorizontal = false;
	bool drawCaveHorizontal = false;
	bool drawCaveVertical = false;
	switch (automapType & MapFlagsType) {
	case 1: // stand-alone column or other unpassable object
		DrawDiamond(out, { center.x, center.y - AmLine8 }, MapColorsDim);
		break;
	case 2:
	case 5:
		drawVertical = true;
		break;
	case 3:
	case 6:
		drawHorizontal = true;
		break;
	case 4:
		drawVertical = true;
		drawHorizontal = true;
		break;
	case 8:
		drawVertical = true;
		drawCaveHorizontal = true;
		break;
	case 9:
		drawHorizontal = true;
		drawCaveVertical = true;
		break;
	case 10:
		drawCaveHorizontal = true;
		break;
	case 11:
		drawCaveVertical = true;
		break;
	case 12:
		drawCaveHorizontal = true;
		drawCaveVertical = true;
		break;
	}

	if (drawVertical) {                               // right-facing obstacle
		if ((flags & MapFlagsMapVerticalDoor) != 0) { // two wall segments with a door in the middle
			DrawMapVerticalDoor(out, { center.x - AmLine16, center.y - AmLine8 });
		}
		if ((flags & MapFlagsVerticalGrate) != 0) { // right-facing half-wall
			DrawMapLineNE(out, { center.x - AmLine32, center.y }, AmLine8, MapColorsDim);
			flags |= MapFlagsVerticalArch;
		}
		if ((flags & MapFlagsVerticalArch) != 0) { // window or passable column
			DrawDiamond(out, { center.x, center.y - AmLine8 }, MapColorsDim);
		}
		if ((flags & (MapFlagsMapVerticalDoor | MapFlagsVerticalGrate | MapFlagsVerticalArch)) == 0) {
			DrawMapLineNE(out, { center.x - AmLine32, center.y }, AmLine16, MapColorsDim);
		}
	}

	if (drawHorizontal) { // left-facing obstacle
		if ((flags & MapFlagsMapHorizontalDoor) != 0) {
			DrawMapHorizontalDoor(out, { center.x + AmLine16, center.y - AmLine8 });
		}
		if ((flags & MapFlagsHorizontalGrate) != 0) {
			DrawMapLineSE(out, { center.x + AmLine16, center.y - AmLine8 }, AmLine8, MapColorsDim);
			flags |= MapFlagsHorizontalArch;
		}
		if ((flags & MapFlagsHorizontalArch) != 0) {
			DrawDiamond(out, { center.x, center.y - AmLine8 }, MapColorsDim);
		}
		if ((flags & (MapFlagsMapHorizontalDoor | MapFlagsHorizontalGrate | MapFlagsHorizontalArch)) == 0) {
			DrawMapLineSE(out, { center.x, center.y - AmLine16 }, AmLine16, MapColorsDim);
		}
	}

	// For caves the horizontal/vertical flags are swapped
	if (drawCaveHorizontal) {
		if ((flags & MapFlagsMapVerticalDoor) != 0) {
			DrawMapHorizontalDoor(out, { center.x - AmLine16, center.y + AmLine8 });
		} else {
			DrawMapLineSE(out, { center.x - AmLine32, center.y }, AmLine16, MapColorsDim);
		}
	}

	if (drawCaveVertical) {
		if ((flags & MapFlagsMapHorizontalDoor) != 0) {
			DrawMapVerticalDoor(out, { center.x + AmLine16, center.y + AmLine8 });
		} else {
			DrawMapLineNE(out, { center.x, center.y + AmLine16 }, AmLine16, MapColorsDim);
		}
	}
}

void SearchAutomapItem(const Surface &out)
{
	auto &myPlayer = Players[MyPlayerId];
	Point tile = myPlayer.position.tile;
	if (myPlayer._pmode == PM_WALK3) {
		tile = myPlayer.position.future;
		if (myPlayer._pdir == DIR_W)
			tile.x++;
		else
			tile.y++;
	}

	const int startX = clamp(tile.x - 8, 0, MAXDUNX);
	const int startY = clamp(tile.y - 8, 0, MAXDUNY);

	const int endX = clamp(tile.x + 8, 0, MAXDUNX);
	const int endY = clamp(tile.y + 8, 0, MAXDUNY);

	for (int i = startX; i < endX; i++) {
		for (int j = startY; j < endY; j++) {
			if (dItem[i][j] == 0)
				continue;

			int px = i - 2 * AutomapOffset.deltaX - ViewX;
			int py = j - 2 * AutomapOffset.deltaY - ViewY;

			Point screen = {
				(ScrollInfo.offset.deltaX * AutoMapScale / 100 / 2) + (px - py) * AmLine16 + gnScreenWidth / 2,
				(ScrollInfo.offset.deltaY * AutoMapScale / 100 / 2) + (px + py) * AmLine8 + (gnScreenHeight - PANEL_HEIGHT) / 2
			};

			if (CanPanelsCoverView()) {
				if (invflag || sbookflag)
					screen.x -= 160;
				if (chrflag || questlog)
					screen.x += 160;
			}
			screen.y -= AmLine8;
			DrawDiamond(out, screen, MapColorsItem);
		}
	}
}

/**
 * @brief Renders an arrow on the automap, centered on and facing the direction of the player.
 */
void DrawAutomapPlr(const Surface &out, int playerId)
{
	int playerColor = MapColorsPlayer + (8 * playerId) % 128;

	auto &player = Players[playerId];
	Point tile = player.position.tile;
	if (player._pmode == PM_WALK3) {
		tile = player.position.future;
		if (player._pdir == DIR_W)
			tile.x++;
		else
			tile.y++;
	}

	int px = tile.x - 2 * AutomapOffset.deltaX - ViewX;
	int py = tile.y - 2 * AutomapOffset.deltaY - ViewY;

	Point base = {
		(player.position.offset.deltaX * AutoMapScale / 100 / 2) + (ScrollInfo.offset.deltaX * AutoMapScale / 100 / 2) + (px - py) * AmLine16 + gnScreenWidth / 2,
		(player.position.offset.deltaY * AutoMapScale / 100 / 2) + (ScrollInfo.offset.deltaY * AutoMapScale / 100 / 2) + (px + py) * AmLine8 + (gnScreenHeight - PANEL_HEIGHT) / 2
	};

	if (CanPanelsCoverView()) {
		if (invflag || sbookflag)
			base.x -= gnScreenWidth / 4;
		if (chrflag || questlog)
			base.x += gnScreenWidth / 4;
	}
	base.y -= AmLine8;

	switch (player._pdir) {
	case DIR_N: {
		const Point point { base.x, base.y - AmLine16 };
		DrawVerticalLine(out, point, AmLine16, playerColor);
		DrawMapLineSteepNE(out, { point.x - AmLine4, point.y + 2 * AmLine4 }, AmLine4, playerColor);
		DrawMapLineSteepNW(out, { point.x + AmLine4, point.y + 2 * AmLine4 }, AmLine4, playerColor);
	} break;
	case DIR_NE: {
		const Point point { base.x + AmLine16, base.y - AmLine8 };
		DrawHorizontalLine(out, { point.x - AmLine8, point.y }, AmLine8, playerColor);
		DrawMapLineNE(out, { point.x - 2 * AmLine8, point.y + AmLine8 }, AmLine8, playerColor);
		DrawMapLineSteepSW(out, point, AmLine4, playerColor);
	} break;
	case DIR_E: {
		const Point point { base.x + AmLine16, base.y };
		DrawMapLineNW(out, point, AmLine4, playerColor);
		DrawHorizontalLine(out, { point.x - AmLine16, point.y }, AmLine16, playerColor);
		DrawMapLineSW(out, point, AmLine4, playerColor);
	} break;
	case DIR_SE: {
		const Point point { base.x + AmLine16, base.y + AmLine8 };
		DrawMapLineSE(out, { point.x - 2 * AmLine8, point.y - AmLine8 }, AmLine8, playerColor);
		DrawHorizontalLine(out, { point.x - (AmLine8 + 1), point.y }, AmLine8 + 1, playerColor);
		DrawMapLineSteepNW(out, point, AmLine4, playerColor);
	} break;
	case DIR_S:
	case DIR_OMNI: {
		const Point point { base.x, base.y + AmLine16 };
		DrawVerticalLine(out, { point.x, point.y - AmLine16 }, AmLine16, playerColor);
		DrawMapLineSteepSW(out, { point.x + AmLine4, point.y - 2 * AmLine4 }, AmLine4, playerColor);
		DrawMapLineSteepSE(out, { point.x - AmLine4, point.y - 2 * AmLine4 }, AmLine4, playerColor);
	} break;
	case DIR_SW: {
		const Point point { base.x - AmLine16, base.y + AmLine8 };
		DrawMapLineSteepNE(out, point, AmLine4, playerColor);
		DrawMapLineSW(out, { point.x + 2 * AmLine8, point.y - AmLine8 }, AmLine8, playerColor);
		DrawHorizontalLine(out, point, AmLine8 + 1, playerColor);
	} break;
	case DIR_W: {
		const Point point { base.x - AmLine16, base.y };
		DrawMapLineNE(out, point, AmLine4, playerColor);
		DrawHorizontalLine(out, point, AmLine16 + 1, playerColor);
		DrawMapLineSE(out, point, AmLine4, playerColor);
	} break;
	case DIR_NW: {
		const Point point { base.x - AmLine16, base.y - AmLine8 };
		DrawMapLineNW(out, { point.x + 2 * AmLine8, point.y + AmLine8 }, AmLine8, playerColor);
		DrawHorizontalLine(out, point, AmLine8 + 1, playerColor);
		DrawMapLineSteepSE(out, point, AmLine4, playerColor);
	} break;
	}
}

/**
 * @brief Returns the automap shape at the given coordinate.
 */
uint16_t GetAutomapType(Point map, bool view)
{
	if (view && map.x == -1 && map.y >= 0 && map.y < DMAXY && AutomapView[0][map.y]) {
		if ((GetAutomapType({ 0, map.y }, false) & (MapFlagsDirt << 8)) != 0) {
			return 0;
		}
		return MapFlagsDirt << 8;
	}

	if (view && map.y == -1 && map.x >= 0 && map.x < DMAXY && AutomapView[map.x][0]) {
		if ((GetAutomapType({ map.x, 0 }, false) & (MapFlagsDirt << 8)) != 0) {
			return 0;
		}
		return MapFlagsDirt << 8;
	}

	if (map.x < 0 || map.x >= DMAXX) {
		return 0;
	}
	if (map.y < 0 || map.y >= DMAXX) {
		return 0;
	}
	if (!AutomapView[map.x][map.y] && view) {
		return 0;
	}

	uint16_t rv = AutomapTypes[dungeon[map.x][map.y]];
	if (rv == 7) {
		if (((GetAutomapType({ map.x - 1, map.y }, false) >> 8) & MapFlagsHorizontalArch) != 0) {
			if (((GetAutomapType({ map.x, map.y - 1 }, false) >> 8) & MapFlagsVerticalArch) != 0) {
				rv = 1;
			}
		}
	}

	return rv;
}

/**
 * @brief Renders game info, such as the name of the current level, and in multi player the name of the game and the game password.
 */
void DrawAutomapText(const Surface &out)
{
	char desc[256];
	Point linePosition { 8, 20 };

	if (gbIsMultiplayer) {
		if (strcasecmp("0.0.0.0", szPlayerName) != 0) {
			strcat(strcpy(desc, _("game: ")), szPlayerName);
			DrawString(out, desc, linePosition);
			linePosition.y += 15;
		}

		if (szPlayerDescript[0] != '\0') {
			strcat(strcpy(desc, _("password: ")), szPlayerDescript);
			DrawString(out, desc, linePosition);
			linePosition.y += 15;
		}
	}

	if (setlevel) {
		DrawString(out, _(quest_level_names[setlvlnum]), linePosition);
		return;
	}

	if (currlevel != 0) {
		if (currlevel >= 17 && currlevel <= 20) {
			strcpy(desc, fmt::format(_("Level: Nest {:d}"), currlevel - 16).c_str());
		} else if (currlevel >= 21 && currlevel <= 24) {
			strcpy(desc, fmt::format(_("Level: Crypt {:d}"), currlevel - 20).c_str());
		} else {
			strcpy(desc, fmt::format(_("Level: {:d}"), currlevel).c_str());
		}

		DrawString(out, desc, linePosition);
	}
}

std::unique_ptr<uint16_t[]> LoadAutomapData(size_t &tileCount)
{
	switch (leveltype) {
	case DTYPE_CATHEDRAL:
		if (currlevel < 21)
			return LoadFileInMem<uint16_t>("Levels\\L1Data\\L1.AMP", &tileCount);
		return LoadFileInMem<uint16_t>("NLevels\\L5Data\\L5.AMP", &tileCount);
	case DTYPE_CATACOMBS:
		return LoadFileInMem<uint16_t>("Levels\\L2Data\\L2.AMP", &tileCount);
	case DTYPE_CAVES:
		if (currlevel < 17)
			return LoadFileInMem<uint16_t>("Levels\\L3Data\\L3.AMP", &tileCount);
		return LoadFileInMem<uint16_t>("NLevels\\L6Data\\L6.AMP", &tileCount);
	case DTYPE_HELL:
		return LoadFileInMem<uint16_t>("Levels\\L4Data\\L4.AMP", &tileCount);
	default:
		return nullptr;
	}
}

} // namespace

bool AutomapActive;
bool AutomapView[DMAXX][DMAXY];
int AutoMapScale;
Displacement AutomapOffset;
int AmLine64;
int AmLine32;
int AmLine16;
int AmLine8;
int AmLine4;

void InitAutomapOnce()
{
	AutomapActive = false;
	AutoMapScale = 50;
	AmLine64 = 32;
	AmLine32 = 16;
	AmLine16 = 8;
	AmLine8 = 4;
	AmLine4 = 2;
}

void InitAutomap()
{
	size_t tileCount = 0;
	std::unique_ptr<uint16_t[]> tileTypes = LoadAutomapData(tileCount);
	for (unsigned i = 0; i < tileCount; i++) {
		AutomapTypes[i + 1] = tileTypes[i];
	}
	tileTypes = nullptr;

	memset(AutomapView, 0, sizeof(AutomapView));

	for (auto &column : dFlags)
		for (auto &dFlag : column)
			dFlag &= ~BFLAG_EXPLORED;
}

void StartAutomap()
{
	AutomapOffset = { 0, 0 };
	AutomapActive = true;
}

void AutomapUp()
{
	AutomapOffset.deltaX--;
	AutomapOffset.deltaY--;
}

void AutomapDown()
{
	AutomapOffset.deltaX++;
	AutomapOffset.deltaY++;
}

void AutomapLeft()
{
	AutomapOffset.deltaX--;
	AutomapOffset.deltaY++;
}

void AutomapRight()
{
	AutomapOffset.deltaX++;
	AutomapOffset.deltaY--;
}

void AutomapZoomIn()
{
	if (AutoMapScale >= 200)
		return;

	AutoMapScale += 5;
	AmLine64 = (AutoMapScale * 64) / 100;
	AmLine32 = AmLine64 / 2;
	AmLine16 = AmLine32 / 2;
	AmLine8 = AmLine16 / 2;
	AmLine4 = AmLine8 / 2;
}

void AutomapZoomOut()
{
	if (AutoMapScale <= 50)
		return;

	AutoMapScale -= 5;
	AmLine64 = (AutoMapScale * 64) / 100;
	AmLine32 = AmLine64 / 2;
	AmLine16 = AmLine32 / 2;
	AmLine8 = AmLine16 / 2;
	AmLine4 = AmLine8 / 2;
}

void DrawAutomap(const Surface &out)
{
	if (leveltype == DTYPE_TOWN) {
		DrawAutomapText(out);
		return;
	}

	Automap = { (ViewX - 16) / 2, (ViewY - 16) / 2 };
	while (Automap.x + AutomapOffset.deltaX < 0)
		AutomapOffset.deltaX++;
	while (Automap.x + AutomapOffset.deltaX >= DMAXX)
		AutomapOffset.deltaX--;

	while (Automap.y + AutomapOffset.deltaY < 0)
		AutomapOffset.deltaY++;
	while (Automap.y + AutomapOffset.deltaY >= DMAXY)
		AutomapOffset.deltaY--;

	Automap += AutomapOffset;

	int d = (AutoMapScale * 64) / 100;
	int cells = 2 * (gnScreenWidth / 2 / d) + 1;
	if (((gnScreenWidth / 2) % d) != 0)
		cells++;
	if (((gnScreenWidth / 2) % d) >= (AutoMapScale * 32) / 100)
		cells++;
	if ((ScrollInfo.offset.deltaX + ScrollInfo.offset.deltaY) != 0)
		cells++;

	Point screen {
		gnScreenWidth / 2,
		(gnScreenHeight - PANEL_HEIGHT) / 2
	};
	if ((cells & 1) != 0) {
		screen.x -= AmLine64 * ((cells - 1) / 2);
		screen.y -= AmLine32 * ((cells + 1) / 2);
	} else {
		screen.x -= AmLine64 * (cells / 2) - AmLine32;
		screen.y -= AmLine32 * (cells / 2) + AmLine16;
	}
	if ((ViewX & 1) != 0) {
		screen.x -= AmLine16;
		screen.y -= AmLine8;
	}
	if ((ViewY & 1) != 0) {
		screen.x += AmLine16;
		screen.y -= AmLine8;
	}

	screen.x += AutoMapScale * ScrollInfo.offset.deltaX / 100 / 2;
	screen.y += AutoMapScale * ScrollInfo.offset.deltaY / 100 / 2;

	if (CanPanelsCoverView()) {
		if (invflag || sbookflag) {
			screen.x -= gnScreenWidth / 4;
		}
		if (chrflag || questlog) {
			screen.x += gnScreenWidth / 4;
		}
	}

	Point map = { Automap.x - cells, Automap.y - 1 };

	for (int i = 0; i <= cells + 1; i++) {
		Point tile1 = screen;
		for (int j = 0; j < cells; j++) {
			uint16_t mapType = GetAutomapType({ map.x + j, map.y - j }, true);
			if (mapType != 0)
				DrawAutomapTile(out, tile1, mapType);
			tile1.x += AmLine64;
		}
		map.y++;

		Point tile2 { screen.x - AmLine32, screen.y + AmLine16 };
		for (int j = 0; j <= cells; j++) {
			uint16_t mapType = GetAutomapType({ map.x + j, map.y - j }, true);
			if (mapType != 0)
				DrawAutomapTile(out, tile2, mapType);
			tile2.x += AmLine64;
		}
		map.x++;
		screen.y += AmLine32;
	}

	for (int playerId = 0; playerId < MAX_PLRS; playerId++) {
		auto &player = Players[playerId];
		if (player.plrlevel == Players[MyPlayerId].plrlevel && player.plractive && !player._pLvlChanging) {
			DrawAutomapPlr(out, playerId);
		}
	}

	if (AutoMapShowItems)
		SearchAutomapItem(out);

	DrawAutomapText(out);
}

void SetAutomapView(Point tile)
{
	const Point map { (tile.x - 16) / 2, (tile.y - 16) / 2 };

	if (map.x < 0 || map.x >= DMAXX || map.y < 0 || map.y >= DMAXY) {
		return;
	}

	AutomapView[map.x][map.y] = true;

	uint16_t mapType = GetAutomapType(map, false);
	uint16_t solid = mapType & 0x4000;

	switch (mapType & MapFlagsType) {
	case 2:
		if (solid != 0) {
			if (GetAutomapType({ map.x, map.y + 1 }, false) == 0x4007)
				AutomapView[map.x][map.y + 1] = true;
		} else if ((GetAutomapType({ map.x - 1, map.y }, false) & 0x4000) != 0) {
			AutomapView[map.x - 1][map.y] = true;
		}
		break;
	case 3:
		if (solid != 0) {
			if (GetAutomapType({ map.x + 1, map.y }, false) == 0x4007)
				AutomapView[map.x + 1][map.y] = true;
		} else if ((GetAutomapType({ map.x, map.y - 1 }, false) & 0x4000) != 0) {
			AutomapView[map.x][map.y - 1] = true;
		}
		break;
	case 4:
		if (solid != 0) {
			if (GetAutomapType({ map.x, map.y + 1 }, false) == 0x4007)
				AutomapView[map.x][map.y + 1] = true;
			if (GetAutomapType({ map.x + 1, map.y }, false) == 0x4007)
				AutomapView[map.x + 1][map.y] = true;
		} else {
			if ((GetAutomapType({ map.x - 1, map.y }, false) & 0x4000) != 0)
				AutomapView[map.x - 1][map.y] = true;
			if ((GetAutomapType({ map.x, map.y - 1 }, false) & 0x4000) != 0)
				AutomapView[map.x][map.y - 1] = true;
			if ((GetAutomapType({ map.x - 1, map.y - 1 }, false) & 0x4000) != 0)
				AutomapView[map.x - 1][map.y - 1] = true;
		}
		break;
	case 5:
		if (solid != 0) {
			if ((GetAutomapType({ map.x, map.y - 1 }, false) & 0x4000) != 0)
				AutomapView[map.x][map.y - 1] = true;
			if (GetAutomapType({ map.x, map.y + 1 }, false) == 0x4007)
				AutomapView[map.x][map.y + 1] = true;
		} else if ((GetAutomapType({ map.x - 1, map.y }, false) & 0x4000) != 0) {
			AutomapView[map.x - 1][map.y] = true;
		}
		break;
	case 6:
		if (solid != 0) {
			if ((GetAutomapType({ map.x - 1, map.y }, false) & 0x4000) != 0)
				AutomapView[map.x - 1][map.y] = true;
			if (GetAutomapType({ map.x + 1, map.y }, false) == 0x4007)
				AutomapView[map.x + 1][map.y] = true;
		} else if ((GetAutomapType({ map.x, map.y - 1 }, false) & 0x4000) != 0) {
			AutomapView[map.x][map.y - 1] = true;
		}
		break;
	}
}

void AutomapZoomReset()
{
	AutomapOffset = { 0, 0 };
	AmLine64 = (AutoMapScale * 64) / 100;
	AmLine32 = AmLine64 / 2;
	AmLine16 = AmLine32 / 2;
	AmLine8 = AmLine16 / 2;
	AmLine4 = AmLine8 / 2;
}

} // namespace devilution

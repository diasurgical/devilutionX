/**
 * @file automap.cpp
 *
 * Implementation of the in-game map overlay.
 */
#include "automap.h"

#include <algorithm>
#include <cstdint>

#include <fmt/format.h>

#include "control.h"
#include "engine/load_file.hpp"
#include "engine/palette.h"
#include "engine/render/automap_render.hpp"
#include "levels/gendung.h"
#include "levels/setmaps.h"
#include "player.h"
#include "utils/attributes.h"
#include "utils/enum_traits.h"
#include "utils/language.h"
#include "utils/ui_fwd.h"
#include "utils/utf8.hpp"

#ifdef _DEBUG
#include "debug.h"
#endif

namespace devilution {

namespace {
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
	/** color for lava/water on automap */
	MapColorsLava = 1,
};

struct AutomapTile {
	/** The general shape of the tile */
	enum class Types : uint8_t {
		None,
		Diamond,
		Vertical,
		Horizontal,
		Cross,
		FenceVertical,
		FenceHorizontal,
		Corner,
		CaveHorizontalCross,
		CaveVerticalCross,
		CaveHorizontal,
		CaveVertical,
		CaveCross,
		Bridge,
		River,
		RiverCornerEast,
		RiverCornerNorth,
		RiverCornerSouth,
		RiverCornerWest,
		RiverForkIn,
		RiverForkOut,
		RiverLeftIn,
		RiverLeftOut,
		RiverRightIn,
		RiverRightOut,
		CaveHorizontalWoodCross,
		CaveVerticalWoodCross,
		CaveLeftCorner,
		CaveRightCorner,
		CaveBottomCorner,
		CaveHorizontalWood,
		CaveVerticalWood,
		CaveWoodCross,
		CaveRightWoodCross,
		CaveLeftWoodCross,
		HorizontalLavaThin,
		VerticalLavaThin,
		BendSouthLavaThin,
		BendWestLavaThin,
		BendEastLavaThin,
		BendNorthLavaThin,
		VerticalWallLava,
		HorizontalWallLava,
		SELava,
		SWLava,
		NELava,
		NWLava,
		SLava,
		WLava,
		ELava,
		NLava,
		Lava,
		CaveHorizontalWallLava,
		CaveVerticalWallLava,
		HorizontalBridgeLava,
		VerticalBridgeLava,

	};

	Types type;

	/** Additional details about the given tile */
	enum class Flags : uint8_t {
		// clang-format off
		VerticalDoor      = 1 << 0,
		HorizontalDoor    = 1 << 1,
		VerticalArch      = 1 << 2,
		HorizontalArch    = 1 << 3,
		VerticalGrate     = 1 << 4,
		HorizontalGrate   = 1 << 5,
		VerticalPassage   = VerticalDoor | VerticalArch | VerticalGrate,
		HorizontalPassage = HorizontalDoor | HorizontalArch | HorizontalGrate,
		Dirt              = 1 << 6,
		Stairs            = 1 << 7,
		// clang-format on
	};

	Flags flags;

	constexpr bool HasFlag(Flags test) const
	{
		return (static_cast<uint8_t>(flags) & static_cast<uint8_t>(test)) != 0;
	}

	template <typename... Args>
	[[nodiscard]] DVL_ALWAYS_INLINE constexpr bool hasAnyFlag(Flags flag, Args... flags)
	{
		return (static_cast<uint8_t>(this->flags)
		           & (static_cast<uint8_t>(flag) | ... | static_cast<uint8_t>(flags)))
		    != 0;
	}
};

/**
 * Maps from tile_id to automap type.
 */
std::array<AutomapTile, 256> AutomapTypeTiles;

/**
 * @brief Draw a diamond on top tile.
 */
void DrawDiamond(const Surface &out, Point center, uint8_t color)
{
	DrawMapLineNE(out, center + AmOffset(AmWidthOffset::HalfTileLeft, AmHeightOffset::HalfTileUp), AmLine(AmLineLength::FullTile), color);
	DrawMapLineSE(out, center + AmOffset(AmWidthOffset::HalfTileLeft, AmHeightOffset::HalfTileUp), AmLine(AmLineLength::FullTile), color);
	DrawMapLineSE(out, center + AmOffset(AmWidthOffset::None, AmHeightOffset::FullTileUp), AmLine(AmLineLength::FullTile), color);
	DrawMapLineNE(out, center + AmOffset(AmWidthOffset::None, AmHeightOffset::None), AmLine(AmLineLength::FullTile), color);
}

/**
 * @brief Non-Catacombs: Draw a diamond on left tile, and draw a half-tile length line between door and top-right edge of top tile.
 * Catacombs: Draw a diamond on top tile, and draw a half-tile length line between diamond and bottom-left edge of left tile.
 */
void DrawMapVerticalDoorOrGrate(const Surface &out, Point center, uint8_t colorBright, uint8_t colorDim)
{
	AmWidthOffset lWidthOffset;
	AmHeightOffset lHeightOffset;
	AmWidthOffset dWidthOffset;
	AmHeightOffset dHeightOffset;
	AmLineLength length;

	switch (leveltype) {
	case DTYPE_CATHEDRAL:
	case DTYPE_CRYPT:
		lWidthOffset = AmWidthOffset::QuarterTileLeft;
		lHeightOffset = AmHeightOffset::QuarterTileUp;

		dWidthOffset = AmWidthOffset::HalfTileLeft;
		dHeightOffset = AmHeightOffset::HalfTileDown;

		length = AmLineLength::HalfTile;
		break;
	case DTYPE_CATACOMBS:
		lWidthOffset = AmWidthOffset::ThreeQuartersTileLeft;
		lHeightOffset = AmHeightOffset::QuarterTileDown;

		dWidthOffset = AmWidthOffset::None;
		dHeightOffset = AmHeightOffset::None;

		length = AmLineLength::FullTile;
		break;
	case DTYPE_CAVES:
	case DTYPE_NEST:
		lWidthOffset = AmWidthOffset::QuarterTileRight;
		lHeightOffset = AmHeightOffset::QuarterTileDown;

		dWidthOffset = AmWidthOffset::None;
		dHeightOffset = AmHeightOffset::FullTileDown;

		length = AmLineLength::FullTile;
		break;
	default:
		app_fatal("Invalid leveltype");
	}
	DrawMapLineNE(out, center + AmOffset(lWidthOffset, lHeightOffset), AmLine(length), colorDim);
	DrawDiamond(out, center + AmOffset(dWidthOffset, dHeightOffset), colorBright);
}

/**
 * @brief Non-Catacombs: Draw a diamond on right tile, and draw a half-tile length line between diamond and top-left edge of top tile.
 * Catacombs: Draw a diamond on top tile, and draw a half-tile length line between diamond and bottom-right edge of right tile.
 */
void DrawMapHorizontalDoorOrGrate(const Surface &out, Point center, uint8_t colorBright, uint8_t colorDim)
{
	AmWidthOffset lWidthOffset;
	AmHeightOffset lHeightOffset;
	AmWidthOffset dWidthOffset;
	AmHeightOffset dHeightOffset;
	AmLineLength length;

	switch (leveltype) {
	case DTYPE_CATHEDRAL:
	case DTYPE_CRYPT:
		lWidthOffset = AmWidthOffset::None;
		lHeightOffset = AmHeightOffset::HalfTileUp;

		dWidthOffset = AmWidthOffset::HalfTileRight;
		dHeightOffset = AmHeightOffset::HalfTileDown;

		length = AmLineLength::HalfTile;
		break;
	case DTYPE_CATACOMBS:
		lWidthOffset = AmWidthOffset::QuarterTileRight;
		lHeightOffset = AmHeightOffset::QuarterTileUp;

		dWidthOffset = AmWidthOffset::None;
		dHeightOffset = AmHeightOffset::None;

		length = AmLineLength::FullTile;
		break;
	case DTYPE_CAVES:
	case DTYPE_NEST:
		lWidthOffset = AmWidthOffset::QuarterTileLeft;
		lHeightOffset = AmHeightOffset::QuarterTileDown;

		dWidthOffset = AmWidthOffset::HalfTileLeft;
		dHeightOffset = AmHeightOffset::HalfTileDown;

		length = AmLineLength::FullTile;
		break;
		break;
	default:
		app_fatal("Invalid leveltype");
	}
	DrawMapLineSE(out, center + AmOffset(lWidthOffset, lHeightOffset), AmLine(length), colorDim);
	DrawDiamond(out, center + AmOffset(dWidthOffset, dHeightOffset), colorBright);
}

/**
 * @brief Draw 16 individual pixels equally spaced apart, used to communicate OOB area to the player.
 */
void DrawDirt(const Surface &out, Point center, AutomapTile nwTile, uint8_t color)
{
	out.SetPixel(center + AmOffset(AmWidthOffset::ThreeQuartersTileLeft, AmHeightOffset::QuarterTileDown), color);
	out.SetPixel(center + AmOffset(AmWidthOffset::HalfTileLeft, AmHeightOffset::None), color);
	out.SetPixel(center + AmOffset(AmWidthOffset::HalfTileLeft, AmHeightOffset::HalfTileDown), color);
	out.SetPixel(center + AmOffset(AmWidthOffset::QuarterTileLeft, AmHeightOffset::QuarterTileUp), color);
	out.SetPixel(center + AmOffset(AmWidthOffset::QuarterTileLeft, AmHeightOffset::QuarterTileDown), color);
	out.SetPixel(center + AmOffset(AmWidthOffset::QuarterTileLeft, AmHeightOffset::ThreeQuartersTileDown), color);
	if (!nwTile.HasFlag(AutomapTile::Flags::HorizontalArch))                                                       // Prevent the top dirt pixel from appearing inside arch diamonds
		out.SetPixel(center + AmOffset(AmWidthOffset::None, AmHeightOffset::HalfTileUp), color);
	out.SetPixel(center, color);
	out.SetPixel(center + AmOffset(AmWidthOffset::None, AmHeightOffset::HalfTileDown), color);
	out.SetPixel(center + AmOffset(AmWidthOffset::None, AmHeightOffset::FullTileDown), color);
	out.SetPixel(center + AmOffset(AmWidthOffset::QuarterTileRight, AmHeightOffset::QuarterTileUp), color);
	out.SetPixel(center + AmOffset(AmWidthOffset::QuarterTileRight, AmHeightOffset::QuarterTileDown), color);
	out.SetPixel(center + AmOffset(AmWidthOffset::QuarterTileRight, AmHeightOffset::ThreeQuartersTileDown), color);
	out.SetPixel(center + AmOffset(AmWidthOffset::HalfTileRight, AmHeightOffset::None), color);
	out.SetPixel(center + AmOffset(AmWidthOffset::HalfTileRight, AmHeightOffset::HalfTileDown), color);
	out.SetPixel(center + AmOffset(AmWidthOffset::ThreeQuartersTileRight, AmHeightOffset::QuarterTileDown), color);
}

void DrawBridge(const Surface &out, Point center, uint8_t color)
{
	out.SetPixel(center, color);

	out.SetPixel(center + AmOffset(AmWidthOffset::QuarterTileRight, AmHeightOffset::QuarterTileUp), color);
	out.SetPixel(center + AmOffset(AmWidthOffset::QuarterTileRight, AmHeightOffset::QuarterTileDown), color);

	out.SetPixel(center + AmOffset(AmWidthOffset::HalfTileRight, AmHeightOffset::None), color);
	out.SetPixel(center + AmOffset(AmWidthOffset::HalfTileRight, AmHeightOffset::HalfTileDown), color);

	out.SetPixel(center + AmOffset(AmWidthOffset::ThreeQuartersTileRight, AmHeightOffset::QuarterTileDown), color);
}

void DrawRiverRightIn(const Surface &out, Point center, uint8_t color)
{
	out.SetPixel(center + AmOffset(AmWidthOffset::HalfTileLeft, AmHeightOffset::HalfTileDown), color);

	out.SetPixel(center + AmOffset(AmWidthOffset::QuarterTileLeft, AmHeightOffset::QuarterTileDown), color);
	out.SetPixel(center + AmOffset(AmWidthOffset::QuarterTileLeft, AmHeightOffset::ThreeQuartersTileDown), color);

	out.SetPixel(center, color);
	out.SetPixel(center + AmOffset(AmWidthOffset::None, AmHeightOffset::HalfTileDown), color);
	out.SetPixel(center + AmOffset(AmWidthOffset::None, AmHeightOffset::FullTileDown), color);

	out.SetPixel(center + AmOffset(AmWidthOffset::QuarterTileRight, AmHeightOffset::QuarterTileDown), color);
	out.SetPixel(center + AmOffset(AmWidthOffset::QuarterTileRight, AmHeightOffset::ThreeQuartersTileDown), color);

	out.SetPixel(center + AmOffset(AmWidthOffset::HalfTileRight, AmHeightOffset::None), color);
	out.SetPixel(center + AmOffset(AmWidthOffset::HalfTileRight, AmHeightOffset::HalfTileDown), color);

	out.SetPixel(center + AmOffset(AmWidthOffset::ThreeQuartersTileRight, AmHeightOffset::QuarterTileDown), color);
}

void DrawRiverCornerSouth(const Surface &out, Point center, uint8_t color)
{
	out.SetPixel(center + AmOffset(AmWidthOffset::None, AmHeightOffset::FullTileDown), color);
}

void DrawRiverCornerNorth(const Surface &out, Point center, uint8_t color)
{
	out.SetPixel(center + AmOffset(AmWidthOffset::QuarterTileLeft, AmHeightOffset::QuarterTileUp), color);
	out.SetPixel(center + AmOffset(AmWidthOffset::None, AmHeightOffset::HalfTileUp), color);
	out.SetPixel(center + AmOffset(AmWidthOffset::QuarterTileRight, AmHeightOffset::QuarterTileUp), color);
}

void DrawRiverLeftOut(const Surface &out, Point center, uint8_t color)
{
	out.SetPixel(center + AmOffset(AmWidthOffset::ThreeQuartersTileLeft, AmHeightOffset::QuarterTileDown), color);

	out.SetPixel(center + AmOffset(AmWidthOffset::HalfTileLeft, AmHeightOffset::None), color);
	out.SetPixel(center + AmOffset(AmWidthOffset::HalfTileLeft, AmHeightOffset::HalfTileDown), color);

	out.SetPixel(center + AmOffset(AmWidthOffset::QuarterTileLeft, AmHeightOffset::QuarterTileDown), color);
	out.SetPixel(center + AmOffset(AmWidthOffset::QuarterTileLeft, AmHeightOffset::ThreeQuartersTileDown), color);

	out.SetPixel(center, color);
	out.SetPixel(center + AmOffset(AmWidthOffset::None, AmHeightOffset::HalfTileDown), color);

	out.SetPixel(center + AmOffset(AmWidthOffset::QuarterTileRight, AmHeightOffset::QuarterTileUp), color);
	out.SetPixel(center + AmOffset(AmWidthOffset::QuarterTileRight, AmHeightOffset::QuarterTileDown), color);
	out.SetPixel(center + AmOffset(AmWidthOffset::QuarterTileRight, AmHeightOffset::FullTileDown), color);

	out.SetPixel(center + AmOffset(AmWidthOffset::HalfTileRight, AmHeightOffset::None), color);
	out.SetPixel(center + AmOffset(AmWidthOffset::HalfTileRight, AmHeightOffset::HalfTileDown), color);

	out.SetPixel(center + AmOffset(AmWidthOffset::ThreeQuartersTileRight, AmHeightOffset::QuarterTileDown), color);
}

void DrawRiverLeftIn(const Surface &out, Point center, uint8_t color)
{
	out.SetPixel(center + AmOffset(AmWidthOffset::HalfTileLeft, AmHeightOffset::None), color);
	out.SetPixel(center + AmOffset(AmWidthOffset::HalfTileLeft, AmHeightOffset::HalfTileDown), color);

	out.SetPixel(center + AmOffset(AmWidthOffset::QuarterTileLeft, AmHeightOffset::QuarterTileUp), color);
	out.SetPixel(center + AmOffset(AmWidthOffset::QuarterTileLeft, AmHeightOffset::QuarterTileDown), color);
	out.SetPixel(center + AmOffset(AmWidthOffset::QuarterTileLeft, AmHeightOffset::ThreeQuartersTileDown), color);

	out.SetPixel(center + AmOffset(AmWidthOffset::None, AmHeightOffset::HalfTileUp), color);
	out.SetPixel(center, color);
	out.SetPixel(center + AmOffset(AmWidthOffset::None, AmHeightOffset::HalfTileDown), color);
	out.SetPixel(center + AmOffset(AmWidthOffset::None, AmHeightOffset::FullTileDown), color);

	out.SetPixel(center + AmOffset(AmWidthOffset::QuarterTileRight, AmHeightOffset::QuarterTileUp), color);
	out.SetPixel(center + AmOffset(AmWidthOffset::QuarterTileRight, AmHeightOffset::QuarterTileDown), color);
	out.SetPixel(center + AmOffset(AmWidthOffset::QuarterTileRight, AmHeightOffset::ThreeQuartersTileDown), color);
}

void DrawRiverCornerWest(const Surface &out, Point center, uint8_t color)
{
	out.SetPixel(center + AmOffset(AmWidthOffset::ThreeQuartersTileLeft, AmHeightOffset::QuarterTileDown), color);
	out.SetPixel(center + AmOffset(AmWidthOffset::HalfTileLeft, AmHeightOffset::None), color);
}

void DrawRiverCornerEast(const Surface &out, Point center, uint8_t color)
{
	out.SetPixel(center + AmOffset(AmWidthOffset::HalfTileRight, AmHeightOffset::None), color);
	out.SetPixel(center + AmOffset(AmWidthOffset::HalfTileRight, AmHeightOffset::HalfTileDown), color);
	out.SetPixel(center + AmOffset(AmWidthOffset::ThreeQuartersTileRight, AmHeightOffset::QuarterTileDown), color);
}

void DrawRiverRightOut(const Surface &out, Point center, uint8_t color)
{
	out.SetPixel(center + AmOffset(AmWidthOffset::QuarterTileLeft, AmHeightOffset::QuarterTileDown), color);
	out.SetPixel(center + AmOffset(AmWidthOffset::QuarterTileLeft, AmHeightOffset::ThreeQuartersTileDown), color);

	out.SetPixel(center, color);
	out.SetPixel(center + AmOffset(AmWidthOffset::None, AmHeightOffset::HalfTileDown), color);
	out.SetPixel(center + AmOffset(AmWidthOffset::None, AmHeightOffset::FullTileDown), color);

	out.SetPixel(center + AmOffset(AmWidthOffset::QuarterTileRight, AmHeightOffset::QuarterTileUp), color);
	out.SetPixel(center + AmOffset(AmWidthOffset::QuarterTileRight, AmHeightOffset::QuarterTileDown), color);
	out.SetPixel(center + AmOffset(AmWidthOffset::QuarterTileRight, AmHeightOffset::ThreeQuartersTileDown), color);

	out.SetPixel(center + AmOffset(AmWidthOffset::HalfTileRight, AmHeightOffset::None), color);
	out.SetPixel(center + AmOffset(AmWidthOffset::HalfTileRight, AmHeightOffset::HalfTileDown), color);

	out.SetPixel(center + AmOffset(AmWidthOffset::ThreeQuartersTileRight, AmHeightOffset::QuarterTileDown), color);
}

void DrawRiver(const Surface &out, Point center, uint8_t color)
{
	out.SetPixel(center + AmOffset(AmWidthOffset::HalfTileLeft, AmHeightOffset::HalfTileDown), color);

	out.SetPixel(center + AmOffset(AmWidthOffset::QuarterTileLeft, AmHeightOffset::QuarterTileDown), color);
	out.SetPixel(center + AmOffset(AmWidthOffset::QuarterTileLeft, AmHeightOffset::FullTileDown), color);

	out.SetPixel(center, color);
	out.SetPixel(center + AmOffset(AmWidthOffset::None, AmHeightOffset::HalfTileDown), color);
	out.SetPixel(center + AmOffset(AmWidthOffset::None, AmHeightOffset::FullTileDown), color);

	out.SetPixel(center + AmOffset(AmWidthOffset::QuarterTileRight, AmHeightOffset::QuarterTileUp), color);
	out.SetPixel(center + AmOffset(AmWidthOffset::QuarterTileRight, AmHeightOffset::QuarterTileDown), color);
	out.SetPixel(center + AmOffset(AmWidthOffset::QuarterTileRight, AmHeightOffset::ThreeQuartersTileDown), color);

	out.SetPixel(center + AmOffset(AmWidthOffset::HalfTileRight, AmHeightOffset::None), color);
	out.SetPixel(center + AmOffset(AmWidthOffset::HalfTileRight, AmHeightOffset::HalfTileDown), color);

	out.SetPixel(center + AmOffset(AmWidthOffset::ThreeQuartersTileRight, AmHeightOffset::QuarterTileDown), color);
}

void DrawRiverForkIn(const Surface &out, Point center, uint8_t color)
{
	out.SetPixel(center + AmOffset(AmWidthOffset::HalfTileLeft, AmHeightOffset::None), color);
	out.SetPixel(center + AmOffset(AmWidthOffset::HalfTileLeft, AmHeightOffset::HalfTileDown), color);

	out.SetPixel(center + AmOffset(AmWidthOffset::QuarterTileLeft, AmHeightOffset::QuarterTileUp), color);
	out.SetPixel(center + AmOffset(AmWidthOffset::QuarterTileLeft, AmHeightOffset::QuarterTileDown), color);
	out.SetPixel(center + AmOffset(AmWidthOffset::QuarterTileLeft, AmHeightOffset::FullTileDown), color);

	out.SetPixel(center + AmOffset(AmWidthOffset::None, AmHeightOffset::HalfTileUp), color);
	out.SetPixel(center, color);
	out.SetPixel(center + AmOffset(AmWidthOffset::None, AmHeightOffset::HalfTileDown), color);
	out.SetPixel(center + AmOffset(AmWidthOffset::None, AmHeightOffset::FullTileDown), color);

	out.SetPixel(center + AmOffset(AmWidthOffset::QuarterTileRight, AmHeightOffset::QuarterTileUp), color);
	out.SetPixel(center + AmOffset(AmWidthOffset::QuarterTileRight, AmHeightOffset::QuarterTileDown), color);
	out.SetPixel(center + AmOffset(AmWidthOffset::QuarterTileRight, AmHeightOffset::ThreeQuartersTileDown), color);

	out.SetPixel(center + AmOffset(AmWidthOffset::HalfTileRight, AmHeightOffset::None), color);
	out.SetPixel(center + AmOffset(AmWidthOffset::HalfTileRight, AmHeightOffset::HalfTileDown), color);

	out.SetPixel(center + AmOffset(AmWidthOffset::ThreeQuartersTileRight, AmHeightOffset::QuarterTileDown), color);
}

void DrawRiverForkOut(const Surface &out, Point center, uint8_t color)
{
	out.SetPixel(center + AmOffset(AmWidthOffset::ThreeQuartersTileLeft, AmHeightOffset::QuarterTileDown), color);

	out.SetPixel(center + AmOffset(AmWidthOffset::HalfTileLeft, AmHeightOffset::None), color);
	out.SetPixel(center + AmOffset(AmWidthOffset::HalfTileLeft, AmHeightOffset::HalfTileDown), color);

	out.SetPixel(center + AmOffset(AmWidthOffset::QuarterTileLeft, AmHeightOffset::ThreeQuartersTileDown), color);

	out.SetPixel(center + AmOffset(AmWidthOffset::None, AmHeightOffset::FullTileDown), color);

	out.SetPixel(center + AmOffset(AmWidthOffset::QuarterTileRight, AmHeightOffset::ThreeQuartersTileDown), color);
}

void DrawHorizontalLavaThin(const Surface &out, Point center, uint8_t color)
{
	// Start at lowest x,y (southeast is positive x and southwest is positive y)
	// First row (y = 0)
	out.SetPixel(center + AmOffset(AmWidthOffset::HalfTileLeft, AmHeightOffset::None), color);
	out.SetPixel(center + AmOffset(AmWidthOffset::QuarterTileLeft, AmHeightOffset::QuarterTileUp), color);

	// Second row (y = 1)
	out.SetPixel(center + AmOffset(AmWidthOffset::QuarterTileLeft, AmHeightOffset::QuarterTileDown), color);
	out.SetPixel(center + AmOffset(AmWidthOffset::None, AmHeightOffset::None), color);

	// Third row (y = 2)
	out.SetPixel(center + AmOffset(AmWidthOffset::None, AmHeightOffset::HalfTileDown), color);
	out.SetPixel(center + AmOffset(AmWidthOffset::QuarterTileRight, AmHeightOffset::QuarterTileDown), color);

	// Fourth row (y = 3)
	out.SetPixel(center + AmOffset(AmWidthOffset::QuarterTileRight, AmHeightOffset::ThreeQuartersTileDown), color);
	out.SetPixel(center + AmOffset(AmWidthOffset::HalfTileRight, AmHeightOffset::HalfTileDown), color);
}

void DrawVerticalLavaThin(const Surface &out, Point center, uint8_t color)
{
	// Start at lowest x,y (southeast is positive x and southwest is positive y)
	// Second row (y = 1)
	out.SetPixel(center + AmOffset(AmWidthOffset::HalfTileLeft, AmHeightOffset::HalfTileDown), color);
	out.SetPixel(center + AmOffset(AmWidthOffset::QuarterTileLeft, AmHeightOffset::QuarterTileDown), color);
	out.SetPixel(center + AmOffset(AmWidthOffset::None, AmHeightOffset::None), color);
	out.SetPixel(center + AmOffset(AmWidthOffset::QuarterTileRight, AmHeightOffset::QuarterTileUp), color);

	// Third row (y = 2)
	out.SetPixel(center + AmOffset(AmWidthOffset::QuarterTileLeft, AmHeightOffset::ThreeQuartersTileDown), color);
	out.SetPixel(center + AmOffset(AmWidthOffset::None, AmHeightOffset::HalfTileDown), color);
	out.SetPixel(center + AmOffset(AmWidthOffset::QuarterTileRight, AmHeightOffset::QuarterTileDown), color);
	out.SetPixel(center + AmOffset(AmWidthOffset::HalfTileRight, AmHeightOffset::None), color);
}
void DrawBendSouthLavaThin(const Surface &out, Point center, uint8_t color)
{
	// Start at lowest x,y (southeast is positive x and southwest is positive y)
	// First row (y = 0)
	//out.SetPixel(center + AmOffset(AmWidthOffset::None, AmHeightOffset::HalfTileUp), color);
	//out.SetPixel(center + AmOffset(AmWidthOffset::QuarterTileLeft, AmHeightOffset::QuarterTileUp), color);
	//out.SetPixel(center + AmOffset(AmWidthOffset::HalfTileLeft, AmHeightOffset::None), color);
	//out.SetPixel(center + AmOffset(AmWidthOffset::ThreeQuartersTileLeft, AmHeightOffset::QuarterTileDown), color);

	// Second row (y = 1)
	//out.SetPixel(center + AmOffset(AmWidthOffset::QuarterTileRight, AmHeightOffset::QuarterTileUp), color);
	//out.SetPixel(center + AmOffset(AmWidthOffset::None, AmHeightOffset::None), color);
	out.SetPixel(center + AmOffset(AmWidthOffset::QuarterTileLeft, AmHeightOffset::QuarterTileDown), color);
	out.SetPixel(center + AmOffset(AmWidthOffset::HalfTileLeft, AmHeightOffset::HalfTileDown), color);

	// Third row (y = 2)
	//out.SetPixel(center + AmOffset(AmWidthOffset::HalfTileRight, AmHeightOffset::None), color);
	out.SetPixel(center + AmOffset(AmWidthOffset::QuarterTileRight, AmHeightOffset::QuarterTileDown), color);
	out.SetPixel(center + AmOffset(AmWidthOffset::None, AmHeightOffset::HalfTileDown), color);
	out.SetPixel(center + AmOffset(AmWidthOffset::QuarterTileLeft, AmHeightOffset::ThreeQuartersTileDown), color);

	// Fourth row (y = 3)
	//out.SetPixel(center + AmOffset(AmWidthOffset::ThreeQuartersTileRight, AmHeightOffset::QuarterTileDown), color);
	out.SetPixel(center + AmOffset(AmWidthOffset::HalfTileRight, AmHeightOffset::HalfTileDown), color);
	out.SetPixel(center + AmOffset(AmWidthOffset::QuarterTileRight, AmHeightOffset::ThreeQuartersTileDown), color);
	//out.SetPixel(center + AmOffset(AmWidthOffset::None, AmHeightOffset::FullTileDown), color);
}

void DrawBendWestLavaThin(const Surface &out, Point center, uint8_t color)
{
	// Start at lowest x,y (southeast is positive x and southwest is positive y)
	// First row (y = 0)
	//out.SetPixel(center + AmOffset(AmWidthOffset::None, AmHeightOffset::HalfTileUp), color);
	out.SetPixel(center + AmOffset(AmWidthOffset::QuarterTileLeft, AmHeightOffset::QuarterTileUp), color);
	out.SetPixel(center + AmOffset(AmWidthOffset::HalfTileLeft, AmHeightOffset::None), color);
	//out.SetPixel(center + AmOffset(AmWidthOffset::ThreeQuartersTileLeft, AmHeightOffset::QuarterTileDown), color);

	// Second row (y = 1)
	//out.SetPixel(center + AmOffset(AmWidthOffset::QuarterTileRight, AmHeightOffset::QuarterTileUp), color);
	out.SetPixel(center + AmOffset(AmWidthOffset::None, AmHeightOffset::None), color);
	out.SetPixel(center + AmOffset(AmWidthOffset::QuarterTileLeft, AmHeightOffset::QuarterTileDown), color);
	out.SetPixel(center + AmOffset(AmWidthOffset::HalfTileLeft, AmHeightOffset::HalfTileDown), color);

	// Third row (y = 2)
	//out.SetPixel(center + AmOffset(AmWidthOffset::HalfTileRight, AmHeightOffset::None), color);
	//out.SetPixel(center + AmOffset(AmWidthOffset::QuarterTileRight, AmHeightOffset::QuarterTileDown), color);
	out.SetPixel(center + AmOffset(AmWidthOffset::None, AmHeightOffset::HalfTileDown), color);
	out.SetPixel(center + AmOffset(AmWidthOffset::QuarterTileLeft, AmHeightOffset::ThreeQuartersTileDown), color);

	// Fourth row (y = 3)
	//out.SetPixel(center + AmOffset(AmWidthOffset::ThreeQuartersTileRight, AmHeightOffset::QuarterTileDown), color);
	//out.SetPixel(center + AmOffset(AmWidthOffset::HalfTileRight, AmHeightOffset::HalfTileDown), color);
	//out.SetPixel(center + AmOffset(AmWidthOffset::QuarterTileRight, AmHeightOffset::ThreeQuartersTileDown), color);
	//out.SetPixel(center + AmOffset(AmWidthOffset::None, AmHeightOffset::FullTileDown), color);
}

void DrawBendEastLavaThin(const Surface &out, Point center, uint8_t color)
{
	// Start at lowest x,y (southeast is positive x and southwest is positive y)
	// First row (y = 0)
	//out.SetPixel(center + AmOffset(AmWidthOffset::None, AmHeightOffset::HalfTileUp), color);
	//out.SetPixel(center + AmOffset(AmWidthOffset::QuarterTileLeft, AmHeightOffset::QuarterTileUp), color);
	//out.SetPixel(center + AmOffset(AmWidthOffset::HalfTileLeft, AmHeightOffset::None), color);
	//out.SetPixel(center + AmOffset(AmWidthOffset::ThreeQuartersTileLeft, AmHeightOffset::QuarterTileDown), color);

	// Second row (y = 1)
	out.SetPixel(center + AmOffset(AmWidthOffset::QuarterTileRight, AmHeightOffset::QuarterTileUp), color);
	out.SetPixel(center + AmOffset(AmWidthOffset::None, AmHeightOffset::None), color);
	//out.SetPixel(center + AmOffset(AmWidthOffset::QuarterTileLeft, AmHeightOffset::QuarterTileDown), color);
	//out.SetPixel(center + AmOffset(AmWidthOffset::HalfTileLeft, AmHeightOffset::HalfTileDown), color);

	// Third row (y = 2)
	out.SetPixel(center + AmOffset(AmWidthOffset::HalfTileRight, AmHeightOffset::None), color);
	out.SetPixel(center + AmOffset(AmWidthOffset::QuarterTileRight, AmHeightOffset::QuarterTileDown), color);
	out.SetPixel(center + AmOffset(AmWidthOffset::None, AmHeightOffset::HalfTileDown), color);
	//out.SetPixel(center + AmOffset(AmWidthOffset::QuarterTileLeft, AmHeightOffset::ThreeQuartersTileDown), color);

	// Fourth row (y = 3)
	//out.SetPixel(center + AmOffset(AmWidthOffset::ThreeQuartersTileRight, AmHeightOffset::QuarterTileDown), color);
	out.SetPixel(center + AmOffset(AmWidthOffset::HalfTileRight, AmHeightOffset::HalfTileDown), color);
	out.SetPixel(center + AmOffset(AmWidthOffset::QuarterTileRight, AmHeightOffset::ThreeQuartersTileDown), color);
	//out.SetPixel(center + AmOffset(AmWidthOffset::None, AmHeightOffset::FullTileDown), color);
}

void DrawBendNorthLavaThin(const Surface &out, Point center, uint8_t color)
{
	// Start at lowest x,y (southeast is positive x and southwest is positive y)
	// First row (y = 0)
	//out.SetPixel(center + AmOffset(AmWidthOffset::None, AmHeightOffset::HalfTileUp), color);
	out.SetPixel(center + AmOffset(AmWidthOffset::QuarterTileLeft, AmHeightOffset::QuarterTileUp), color);
	out.SetPixel(center + AmOffset(AmWidthOffset::HalfTileLeft, AmHeightOffset::None), color);
	//out.SetPixel(center + AmOffset(AmWidthOffset::ThreeQuartersTileLeft, AmHeightOffset::QuarterTileDown), color);

	// Second row (y = 1)
	out.SetPixel(center + AmOffset(AmWidthOffset::QuarterTileRight, AmHeightOffset::QuarterTileUp), color);
	out.SetPixel(center + AmOffset(AmWidthOffset::None, AmHeightOffset::None), color);
	out.SetPixel(center + AmOffset(AmWidthOffset::QuarterTileLeft, AmHeightOffset::QuarterTileDown), color);
	//out.SetPixel(center + AmOffset(AmWidthOffset::HalfTileLeft, AmHeightOffset::HalfTileDown), color);

	// Third row (y = 2)
	out.SetPixel(center + AmOffset(AmWidthOffset::HalfTileRight, AmHeightOffset::None), color);
	out.SetPixel(center + AmOffset(AmWidthOffset::QuarterTileRight, AmHeightOffset::QuarterTileDown), color);
	//out.SetPixel(center + AmOffset(AmWidthOffset::None, AmHeightOffset::HalfTileDown), color);
	//out.SetPixel(center + AmOffset(AmWidthOffset::QuarterTileLeft, AmHeightOffset::ThreeQuartersTileDown), color);

	// Fourth row (y = 3)
	//out.SetPixel(center + AmOffset(AmWidthOffset::ThreeQuartersTileRight, AmHeightOffset::QuarterTileDown), color);
	//out.SetPixel(center + AmOffset(AmWidthOffset::HalfTileRight, AmHeightOffset::HalfTileDown), color);
	//out.SetPixel(center + AmOffset(AmWidthOffset::QuarterTileRight, AmHeightOffset::ThreeQuartersTileDown), color);
	//out.SetPixel(center + AmOffset(AmWidthOffset::None, AmHeightOffset::FullTileDown), color);
}

void DrawHorizontalWallLava(const Surface &out, Point center, uint8_t color)
{
	// Start at lowest x,y (southeast is positive x and southwest is positive y)
	// First row (y = 0)
	//out.SetPixel(center + AmOffset(AmWidthOffset::None, AmHeightOffset::HalfTileUp), color);
	//out.SetPixel(center + AmOffset(AmWidthOffset::QuarterTileLeft, AmHeightOffset::QuarterTileUp), color);
	//out.SetPixel(center + AmOffset(AmWidthOffset::HalfTileLeft, AmHeightOffset::None), color);
	//out.SetPixel(center + AmOffset(AmWidthOffset::ThreeQuartersTileLeft, AmHeightOffset::QuarterTileDown), color);

	// Second row (y = 1)
	out.SetPixel(center + AmOffset(AmWidthOffset::QuarterTileRight, AmHeightOffset::QuarterTileUp), color);
	out.SetPixel(center + AmOffset(AmWidthOffset::None, AmHeightOffset::None), color);
	out.SetPixel(center + AmOffset(AmWidthOffset::QuarterTileLeft, AmHeightOffset::QuarterTileDown), color);
	out.SetPixel(center + AmOffset(AmWidthOffset::HalfTileLeft, AmHeightOffset::HalfTileDown), color);

	// Third row (y = 2)
	out.SetPixel(center + AmOffset(AmWidthOffset::HalfTileRight, AmHeightOffset::None), color);
	out.SetPixel(center + AmOffset(AmWidthOffset::QuarterTileRight, AmHeightOffset::QuarterTileDown), color);
	out.SetPixel(center + AmOffset(AmWidthOffset::None, AmHeightOffset::HalfTileDown), color);
	out.SetPixel(center + AmOffset(AmWidthOffset::QuarterTileLeft, AmHeightOffset::ThreeQuartersTileDown), color);

	// Fourth row (y = 3)
	//out.SetPixel(center + AmOffset(AmWidthOffset::ThreeQuartersTileRight, AmHeightOffset::QuarterTileDown), color);
	//out.SetPixel(center + AmOffset(AmWidthOffset::HalfTileRight, AmHeightOffset::HalfTileDown), color);
	//out.SetPixel(center + AmOffset(AmWidthOffset::QuarterTileRight, AmHeightOffset::ThreeQuartersTileDown), color);
	//out.SetPixel(center + AmOffset(AmWidthOffset::None, AmHeightOffset::FullTileDown), color);
}

void DrawVerticalWallLava(const Surface &out, Point center, uint8_t color)
{
	// Start at lowest x,y (southeast is positive x and southwest is positive y)
	// First row (y = 0)
	//out.SetPixel(center + AmOffset(AmWidthOffset::None, AmHeightOffset::HalfTileUp), color);
	//out.SetPixel(center + AmOffset(AmWidthOffset::QuarterTileLeft, AmHeightOffset::QuarterTileUp), color);
	//out.SetPixel(center + AmOffset(AmWidthOffset::HalfTileLeft, AmHeightOffset::None), color);
	//out.SetPixel(center + AmOffset(AmWidthOffset::ThreeQuartersTileLeft, AmHeightOffset::QuarterTileDown), color);

	// Second row (y = 1)
	//out.SetPixel(center + AmOffset(AmWidthOffset::QuarterTileRight, AmHeightOffset::QuarterTileUp), color);
	out.SetPixel(center + AmOffset(AmWidthOffset::None, AmHeightOffset::None), color);
	out.SetPixel(center + AmOffset(AmWidthOffset::QuarterTileLeft, AmHeightOffset::QuarterTileDown), color);
	//out.SetPixel(center + AmOffset(AmWidthOffset::HalfTileLeft, AmHeightOffset::HalfTileDown), color);

	// Third row (y = 2)
	//out.SetPixel(center + AmOffset(AmWidthOffset::HalfTileRight, AmHeightOffset::None), color);
	out.SetPixel(center + AmOffset(AmWidthOffset::QuarterTileRight, AmHeightOffset::QuarterTileDown), color);
	out.SetPixel(center + AmOffset(AmWidthOffset::None, AmHeightOffset::HalfTileDown), color);
	//out.SetPixel(center + AmOffset(AmWidthOffset::QuarterTileLeft, AmHeightOffset::ThreeQuartersTileDown), color);

	// Fourth row (y = 3)
	//out.SetPixel(center + AmOffset(AmWidthOffset::ThreeQuartersTileRight, AmHeightOffset::QuarterTileDown), color);
	out.SetPixel(center + AmOffset(AmWidthOffset::HalfTileRight, AmHeightOffset::HalfTileDown), color);
	out.SetPixel(center + AmOffset(AmWidthOffset::QuarterTileRight, AmHeightOffset::ThreeQuartersTileDown), color);
	//out.SetPixel(center + AmOffset(AmWidthOffset::None, AmHeightOffset::FullTileDown), color);
}


void DrawSELava(const Surface &out, Point center, uint8_t color)
{
	// Start at lowest x,y (southeast is positive x and southwest is positive y)
	// First row (y = 0)
	//out.SetPixel(center + AmOffset(AmWidthOffset::None, AmHeightOffset::HalfTileUp), color);
	//out.SetPixel(center + AmOffset(AmWidthOffset::QuarterTileLeft, AmHeightOffset::QuarterTileUp), color);
	//out.SetPixel(center + AmOffset(AmWidthOffset::HalfTileLeft, AmHeightOffset::None), color);
	//out.SetPixel(center + AmOffset(AmWidthOffset::ThreeQuartersTileLeft, AmHeightOffset::QuarterTileDown), color);

	// Second row (y = 1)
	out.SetPixel(center + AmOffset(AmWidthOffset::QuarterTileRight, AmHeightOffset::QuarterTileUp), color);
	out.SetPixel(center + AmOffset(AmWidthOffset::None, AmHeightOffset::None), color);
	out.SetPixel(center + AmOffset(AmWidthOffset::QuarterTileLeft, AmHeightOffset::QuarterTileDown), color);
	out.SetPixel(center + AmOffset(AmWidthOffset::HalfTileLeft, AmHeightOffset::HalfTileDown), color);

	// Third row (y = 2)
	out.SetPixel(center + AmOffset(AmWidthOffset::HalfTileRight, AmHeightOffset::None), color);
	out.SetPixel(center + AmOffset(AmWidthOffset::QuarterTileRight, AmHeightOffset::QuarterTileDown), color);
	out.SetPixel(center + AmOffset(AmWidthOffset::None, AmHeightOffset::HalfTileDown), color);
	out.SetPixel(center + AmOffset(AmWidthOffset::QuarterTileLeft, AmHeightOffset::ThreeQuartersTileDown), color);

	// Fourth row (y = 3)
	out.SetPixel(center + AmOffset(AmWidthOffset::ThreeQuartersTileRight, AmHeightOffset::QuarterTileDown), color);
	out.SetPixel(center + AmOffset(AmWidthOffset::HalfTileRight, AmHeightOffset::HalfTileDown), color);
	out.SetPixel(center + AmOffset(AmWidthOffset::QuarterTileRight, AmHeightOffset::ThreeQuartersTileDown), color);
	out.SetPixel(center + AmOffset(AmWidthOffset::None, AmHeightOffset::FullTileDown), color);
}

void DrawSWLava(const Surface &out, Point center, uint8_t color)
{
	// Start at lowest x,y (southeast is positive x and southwest is positive y)
	// First row (y = 0)
	//out.SetPixel(center + AmOffset(AmWidthOffset::None, AmHeightOffset::HalfTileUp), color);
	out.SetPixel(center + AmOffset(AmWidthOffset::QuarterTileLeft, AmHeightOffset::QuarterTileUp), color);
	out.SetPixel(center + AmOffset(AmWidthOffset::HalfTileLeft, AmHeightOffset::None), color);
	out.SetPixel(center + AmOffset(AmWidthOffset::ThreeQuartersTileLeft, AmHeightOffset::QuarterTileDown), color);

	// Second row (y = 1)
	//out.SetPixel(center + AmOffset(AmWidthOffset::QuarterTileRight, AmHeightOffset::QuarterTileUp), color);
	out.SetPixel(center + AmOffset(AmWidthOffset::None, AmHeightOffset::None), color);
	out.SetPixel(center + AmOffset(AmWidthOffset::QuarterTileLeft, AmHeightOffset::QuarterTileDown), color);
	out.SetPixel(center + AmOffset(AmWidthOffset::HalfTileLeft, AmHeightOffset::HalfTileDown), color);

	// Third row (y = 2)
	//out.SetPixel(center + AmOffset(AmWidthOffset::HalfTileRight, AmHeightOffset::None), color);
	out.SetPixel(center + AmOffset(AmWidthOffset::QuarterTileRight, AmHeightOffset::QuarterTileDown), color);
	out.SetPixel(center + AmOffset(AmWidthOffset::None, AmHeightOffset::HalfTileDown), color);
	out.SetPixel(center + AmOffset(AmWidthOffset::QuarterTileLeft, AmHeightOffset::ThreeQuartersTileDown), color);

	// Fourth row (y = 3)
	//out.SetPixel(center + AmOffset(AmWidthOffset::ThreeQuartersTileRight, AmHeightOffset::QuarterTileDown), color);
	out.SetPixel(center + AmOffset(AmWidthOffset::HalfTileRight, AmHeightOffset::HalfTileDown), color);
	out.SetPixel(center + AmOffset(AmWidthOffset::QuarterTileRight, AmHeightOffset::ThreeQuartersTileDown), color);
	out.SetPixel(center + AmOffset(AmWidthOffset::None, AmHeightOffset::FullTileDown), color);
}

void DrawNELava(const Surface &out, Point center, uint8_t color)
{
	// Start at lowest x,y (southeast is positive x and southwest is positive y)
	// First row (y = 0)
	out.SetPixel(center + AmOffset(AmWidthOffset::None, AmHeightOffset::HalfTileUp), color);
	out.SetPixel(center + AmOffset(AmWidthOffset::QuarterTileLeft, AmHeightOffset::QuarterTileUp), color);
	out.SetPixel(center + AmOffset(AmWidthOffset::HalfTileLeft, AmHeightOffset::None), color);
	//out.SetPixel(center + AmOffset(AmWidthOffset::ThreeQuartersTileLeft, AmHeightOffset::QuarterTileDown), color);

	// Second row (y = 1)
	out.SetPixel(center + AmOffset(AmWidthOffset::QuarterTileRight, AmHeightOffset::QuarterTileUp), color);
	out.SetPixel(center + AmOffset(AmWidthOffset::None, AmHeightOffset::None), color);
	out.SetPixel(center + AmOffset(AmWidthOffset::QuarterTileLeft, AmHeightOffset::QuarterTileDown), color);
	//out.SetPixel(center + AmOffset(AmWidthOffset::HalfTileLeft, AmHeightOffset::HalfTileDown), color);

	// Third row (y = 2)	
	out.SetPixel(center + AmOffset(AmWidthOffset::HalfTileRight, AmHeightOffset::None), color);
	out.SetPixel(center + AmOffset(AmWidthOffset::QuarterTileRight, AmHeightOffset::QuarterTileDown), color);
	out.SetPixel(center + AmOffset(AmWidthOffset::None, AmHeightOffset::HalfTileDown), color);
	//out.SetPixel(center + AmOffset(AmWidthOffset::QuarterTileLeft, AmHeightOffset::ThreeQuartersTileDown), color);

	// Fourth row (y = 3)	
	out.SetPixel(center + AmOffset(AmWidthOffset::ThreeQuartersTileRight, AmHeightOffset::QuarterTileDown), color);
	out.SetPixel(center + AmOffset(AmWidthOffset::HalfTileRight, AmHeightOffset::HalfTileDown), color);
	out.SetPixel(center + AmOffset(AmWidthOffset::QuarterTileRight, AmHeightOffset::ThreeQuartersTileDown), color);
	//out.SetPixel(center + AmOffset(AmWidthOffset::None, AmHeightOffset::FullTileDown), color);
}

void DrawNWLava(const Surface &out, Point center, uint8_t color)
{
	// Start at lowest x,y (southeast is positive x and southwest is positive y)
	// First row (y = 0)
	out.SetPixel(center + AmOffset(AmWidthOffset::None, AmHeightOffset::HalfTileUp), color);
	out.SetPixel(center + AmOffset(AmWidthOffset::QuarterTileLeft, AmHeightOffset::QuarterTileUp), color);
	out.SetPixel(center + AmOffset(AmWidthOffset::HalfTileLeft, AmHeightOffset::None), color);
	out.SetPixel(center + AmOffset(AmWidthOffset::ThreeQuartersTileLeft, AmHeightOffset::QuarterTileDown), color);

	// Second row (y = 1)
	out.SetPixel(center + AmOffset(AmWidthOffset::QuarterTileRight, AmHeightOffset::QuarterTileUp), color);
	out.SetPixel(center + AmOffset(AmWidthOffset::None, AmHeightOffset::None), color);
	out.SetPixel(center + AmOffset(AmWidthOffset::QuarterTileLeft, AmHeightOffset::QuarterTileDown), color);
	out.SetPixel(center + AmOffset(AmWidthOffset::HalfTileLeft, AmHeightOffset::HalfTileDown), color);

	// Third row (y = 2)
	out.SetPixel(center + AmOffset(AmWidthOffset::HalfTileRight, AmHeightOffset::None), color);
	out.SetPixel(center + AmOffset(AmWidthOffset::QuarterTileRight, AmHeightOffset::QuarterTileDown), color);
	out.SetPixel(center + AmOffset(AmWidthOffset::None, AmHeightOffset::HalfTileDown), color);
	out.SetPixel(center + AmOffset(AmWidthOffset::QuarterTileLeft, AmHeightOffset::ThreeQuartersTileDown), color);

	// Fourth row (y = 3)
	//out.SetPixel(center + AmOffset(AmWidthOffset::ThreeQuartersTileRight, AmHeightOffset::QuarterTileDown), color);
	//out.SetPixel(center + AmOffset(AmWidthOffset::HalfTileRight, AmHeightOffset::HalfTileDown), color);
	//out.SetPixel(center + AmOffset(AmWidthOffset::QuarterTileRight, AmHeightOffset::ThreeQuartersTileDown), color);
	//out.SetPixel(center + AmOffset(AmWidthOffset::None, AmHeightOffset::FullTileDown), color);
}

void DrawSLava(const Surface &out, Point center, uint8_t color)
{
	// Start at lowest x,y (southeast is positive x and southwest is positive y)
	// First row (y = 0)
	//out.SetPixel(center + AmOffset(AmWidthOffset::None, AmHeightOffset::HalfTileUp), color);
	//out.SetPixel(center + AmOffset(AmWidthOffset::QuarterTileLeft, AmHeightOffset::QuarterTileUp), color);
	//out.SetPixel(center + AmOffset(AmWidthOffset::HalfTileLeft, AmHeightOffset::None), color);
	//out.SetPixel(center + AmOffset(AmWidthOffset::ThreeQuartersTileLeft, AmHeightOffset::QuarterTileDown), color);

	// Second row (y = 1)
	//out.SetPixel(center + AmOffset(AmWidthOffset::QuarterTileRight, AmHeightOffset::QuarterTileUp), color);
	//out.SetPixel(center + AmOffset(AmWidthOffset::None, AmHeightOffset::None), color);
	out.SetPixel(center + AmOffset(AmWidthOffset::QuarterTileLeft, AmHeightOffset::QuarterTileDown), color);
	out.SetPixel(center + AmOffset(AmWidthOffset::HalfTileLeft, AmHeightOffset::HalfTileDown), color);

	// Third row (y = 2)
	//out.SetPixel(center + AmOffset(AmWidthOffset::HalfTileRight, AmHeightOffset::None), color);
	out.SetPixel(center + AmOffset(AmWidthOffset::QuarterTileRight, AmHeightOffset::QuarterTileDown), color);
	out.SetPixel(center + AmOffset(AmWidthOffset::None, AmHeightOffset::HalfTileDown), color);
	out.SetPixel(center + AmOffset(AmWidthOffset::QuarterTileLeft, AmHeightOffset::ThreeQuartersTileDown), color);

	// Fourth row (y = 3)
	//out.SetPixel(center + AmOffset(AmWidthOffset::ThreeQuartersTileRight, AmHeightOffset::QuarterTileDown), color);
	out.SetPixel(center + AmOffset(AmWidthOffset::HalfTileRight, AmHeightOffset::HalfTileDown), color);
	out.SetPixel(center + AmOffset(AmWidthOffset::QuarterTileRight, AmHeightOffset::ThreeQuartersTileDown), color);
	out.SetPixel(center + AmOffset(AmWidthOffset::None, AmHeightOffset::FullTileDown), color);
}

void DrawWLava(const Surface &out, Point center, uint8_t color)
{
	// Start at lowest x,y (southeast is positive x and southwest is positive y)
	// First row (y = 0)
	//out.SetPixel(center + AmOffset(AmWidthOffset::None, AmHeightOffset::HalfTileUp), color);
	out.SetPixel(center + AmOffset(AmWidthOffset::QuarterTileLeft, AmHeightOffset::QuarterTileUp), color);
	out.SetPixel(center + AmOffset(AmWidthOffset::HalfTileLeft, AmHeightOffset::None), color);
	out.SetPixel(center + AmOffset(AmWidthOffset::ThreeQuartersTileLeft, AmHeightOffset::QuarterTileDown), color);

	// Second row (y = 1)
	//out.SetPixel(center + AmOffset(AmWidthOffset::QuarterTileRight, AmHeightOffset::QuarterTileUp), color);
	out.SetPixel(center + AmOffset(AmWidthOffset::None, AmHeightOffset::None), color);
	out.SetPixel(center + AmOffset(AmWidthOffset::QuarterTileLeft, AmHeightOffset::QuarterTileDown), color);
	out.SetPixel(center + AmOffset(AmWidthOffset::HalfTileLeft, AmHeightOffset::HalfTileDown), color);

	// Third row (y = 2)
	//out.SetPixel(center + AmOffset(AmWidthOffset::HalfTileRight, AmHeightOffset::None), color);
	//out.SetPixel(center + AmOffset(AmWidthOffset::QuarterTileRight, AmHeightOffset::QuarterTileDown), color);
	out.SetPixel(center + AmOffset(AmWidthOffset::None, AmHeightOffset::HalfTileDown), color);
	out.SetPixel(center + AmOffset(AmWidthOffset::QuarterTileLeft, AmHeightOffset::ThreeQuartersTileDown), color);

	// Fourth row (y = 3)
	//out.SetPixel(center + AmOffset(AmWidthOffset::ThreeQuartersTileRight, AmHeightOffset::QuarterTileDown), color);
	//out.SetPixel(center + AmOffset(AmWidthOffset::HalfTileRight, AmHeightOffset::HalfTileDown), color);
	//out.SetPixel(center + AmOffset(AmWidthOffset::QuarterTileRight, AmHeightOffset::ThreeQuartersTileDown), color);
	//out.SetPixel(center + AmOffset(AmWidthOffset::None, AmHeightOffset::FullTileDown), color);
}

void DrawELava(const Surface &out, Point center, uint8_t color)
{
	// Start at lowest x,y (southeast is positive x and southwest is positive y)
	// First row (y = 0)
	//out.SetPixel(center + AmOffset(AmWidthOffset::None, AmHeightOffset::HalfTileUp), color);
	//out.SetPixel(center + AmOffset(AmWidthOffset::QuarterTileLeft, AmHeightOffset::QuarterTileUp), color);
	//out.SetPixel(center + AmOffset(AmWidthOffset::HalfTileLeft, AmHeightOffset::None), color);
	//out.SetPixel(center + AmOffset(AmWidthOffset::ThreeQuartersTileLeft, AmHeightOffset::QuarterTileDown), color);

	// Second row (y = 1)
	out.SetPixel(center + AmOffset(AmWidthOffset::QuarterTileRight, AmHeightOffset::QuarterTileUp), color);
	out.SetPixel(center + AmOffset(AmWidthOffset::None, AmHeightOffset::None), color);
	//out.SetPixel(center + AmOffset(AmWidthOffset::QuarterTileLeft, AmHeightOffset::QuarterTileDown), color);
	//out.SetPixel(center + AmOffset(AmWidthOffset::HalfTileLeft, AmHeightOffset::HalfTileDown), color);

	// Third row (y = 2)
	out.SetPixel(center + AmOffset(AmWidthOffset::HalfTileRight, AmHeightOffset::None), color);
	out.SetPixel(center + AmOffset(AmWidthOffset::QuarterTileRight, AmHeightOffset::QuarterTileDown), color);
	out.SetPixel(center + AmOffset(AmWidthOffset::None, AmHeightOffset::HalfTileDown), color);
	//out.SetPixel(center + AmOffset(AmWidthOffset::QuarterTileLeft, AmHeightOffset::ThreeQuartersTileDown), color);

	// Fourth row (y = 3)
	out.SetPixel(center + AmOffset(AmWidthOffset::ThreeQuartersTileRight, AmHeightOffset::QuarterTileDown), color);
	out.SetPixel(center + AmOffset(AmWidthOffset::HalfTileRight, AmHeightOffset::HalfTileDown), color);
	out.SetPixel(center + AmOffset(AmWidthOffset::QuarterTileRight, AmHeightOffset::ThreeQuartersTileDown), color);
	//out.SetPixel(center + AmOffset(AmWidthOffset::None, AmHeightOffset::FullTileDown), color);
}

void DrawNLava(const Surface &out, Point center, uint8_t color)
{
	// Start at lowest x,y (southeast is positive x and southwest is positive y)
	// First row (y = 0)
	out.SetPixel(center + AmOffset(AmWidthOffset::None, AmHeightOffset::HalfTileUp), color);
	out.SetPixel(center + AmOffset(AmWidthOffset::QuarterTileLeft, AmHeightOffset::QuarterTileUp), color);
	out.SetPixel(center + AmOffset(AmWidthOffset::HalfTileLeft, AmHeightOffset::None), color);
	//out.SetPixel(center + AmOffset(AmWidthOffset::ThreeQuartersTileLeft, AmHeightOffset::QuarterTileDown), color);

	// Second row (y = 1)
	out.SetPixel(center + AmOffset(AmWidthOffset::QuarterTileRight, AmHeightOffset::QuarterTileUp), color);
	out.SetPixel(center + AmOffset(AmWidthOffset::None, AmHeightOffset::None), color);
	out.SetPixel(center + AmOffset(AmWidthOffset::QuarterTileLeft, AmHeightOffset::QuarterTileDown), color);
	//out.SetPixel(center + AmOffset(AmWidthOffset::HalfTileLeft, AmHeightOffset::HalfTileDown), color);

	// Third row (y = 2)
	out.SetPixel(center + AmOffset(AmWidthOffset::HalfTileRight, AmHeightOffset::None), color);
	out.SetPixel(center + AmOffset(AmWidthOffset::QuarterTileRight, AmHeightOffset::QuarterTileDown), color);
	//out.SetPixel(center + AmOffset(AmWidthOffset::None, AmHeightOffset::HalfTileDown), color);
	//out.SetPixel(center + AmOffset(AmWidthOffset::QuarterTileLeft, AmHeightOffset::ThreeQuartersTileDown), color);

	// Fourth row (y = 3)
	//out.SetPixel(center + AmOffset(AmWidthOffset::ThreeQuartersTileRight, AmHeightOffset::QuarterTileDown), color);
	//out.SetPixel(center + AmOffset(AmWidthOffset::HalfTileRight, AmHeightOffset::HalfTileDown), color);
	//out.SetPixel(center + AmOffset(AmWidthOffset::QuarterTileRight, AmHeightOffset::ThreeQuartersTileDown), color);
	//out.SetPixel(center + AmOffset(AmWidthOffset::None, AmHeightOffset::FullTileDown), color);
}

void DrawLava(const Surface &out, Point center, uint8_t color)
{
	// Start at lowest x,y (southeast is positive x and southwest is positive y)
	// First row (y = 0)
	out.SetPixel(center + AmOffset(AmWidthOffset::None, AmHeightOffset::HalfTileUp), color);
	out.SetPixel(center + AmOffset(AmWidthOffset::QuarterTileLeft, AmHeightOffset::QuarterTileUp), color);
	out.SetPixel(center + AmOffset(AmWidthOffset::HalfTileLeft, AmHeightOffset::None), color);
	out.SetPixel(center + AmOffset(AmWidthOffset::ThreeQuartersTileLeft, AmHeightOffset::QuarterTileDown), color);

	// Second row (y = 1)
	out.SetPixel(center + AmOffset(AmWidthOffset::QuarterTileRight, AmHeightOffset::QuarterTileUp), color);
	out.SetPixel(center + AmOffset(AmWidthOffset::None, AmHeightOffset::None), color);
	out.SetPixel(center + AmOffset(AmWidthOffset::QuarterTileLeft, AmHeightOffset::QuarterTileDown), color);
	out.SetPixel(center + AmOffset(AmWidthOffset::HalfTileLeft, AmHeightOffset::HalfTileDown), color);

	// Third row (y = 2)
	out.SetPixel(center + AmOffset(AmWidthOffset::HalfTileRight, AmHeightOffset::None), color);
	out.SetPixel(center + AmOffset(AmWidthOffset::QuarterTileRight, AmHeightOffset::QuarterTileDown), color);
	out.SetPixel(center + AmOffset(AmWidthOffset::None, AmHeightOffset::HalfTileDown), color);
	out.SetPixel(center + AmOffset(AmWidthOffset::QuarterTileLeft, AmHeightOffset::ThreeQuartersTileDown), color);

	// Fourth row (y = 3)
	out.SetPixel(center + AmOffset(AmWidthOffset::ThreeQuartersTileRight, AmHeightOffset::QuarterTileDown), color);
	out.SetPixel(center + AmOffset(AmWidthOffset::HalfTileRight, AmHeightOffset::HalfTileDown), color);
	out.SetPixel(center + AmOffset(AmWidthOffset::QuarterTileRight, AmHeightOffset::ThreeQuartersTileDown), color);
	out.SetPixel(center + AmOffset(AmWidthOffset::None, AmHeightOffset::FullTileDown), color);
}

void DrawCaveHorizontalWallLava(const Surface &out, Point center, uint8_t color)
{
	// Start at lowest x,y (southeast is positive x and southwest is positive y)
	// First row (y = 0)
	//out.SetPixel(center + AmOffset(AmWidthOffset::None, AmHeightOffset::HalfTileUp), color);
	//out.SetPixel(center + AmOffset(AmWidthOffset::QuarterTileLeft, AmHeightOffset::QuarterTileUp), color);
	//out.SetPixel(center + AmOffset(AmWidthOffset::HalfTileLeft, AmHeightOffset::None), color);
	//out.SetPixel(center + AmOffset(AmWidthOffset::ThreeQuartersTileLeft, AmHeightOffset::QuarterTileDown), color);

	// Second row (y = 1)
	//out.SetPixel(center + AmOffset(AmWidthOffset::QuarterTileRight, AmHeightOffset::QuarterTileUp), color);
	out.SetPixel(center + AmOffset(AmWidthOffset::None, AmHeightOffset::None), color);
	out.SetPixel(center + AmOffset(AmWidthOffset::QuarterTileLeft, AmHeightOffset::QuarterTileDown), color);
	//out.SetPixel(center + AmOffset(AmWidthOffset::HalfTileLeft, AmHeightOffset::HalfTileDown), color);

	// Third row (y = 2)
	//out.SetPixel(center + AmOffset(AmWidthOffset::HalfTileRight, AmHeightOffset::None), color);
	out.SetPixel(center + AmOffset(AmWidthOffset::QuarterTileRight, AmHeightOffset::QuarterTileDown), color);
	out.SetPixel(center + AmOffset(AmWidthOffset::None, AmHeightOffset::HalfTileDown), color);
	//out.SetPixel(center + AmOffset(AmWidthOffset::QuarterTileLeft, AmHeightOffset::ThreeQuartersTileDown), color);

	// Fourth row (y = 3)
	//out.SetPixel(center + AmOffset(AmWidthOffset::ThreeQuartersTileRight, AmHeightOffset::QuarterTileDown), color);
	//out.SetPixel(center + AmOffset(AmWidthOffset::HalfTileRight, AmHeightOffset::HalfTileDown), color);
	//out.SetPixel(center + AmOffset(AmWidthOffset::QuarterTileRight, AmHeightOffset::ThreeQuartersTileDown), color);
	//out.SetPixel(center + AmOffset(AmWidthOffset::None, AmHeightOffset::FullTileDown), color);
}

void DrawCaveVerticalWallLava(const Surface &out, Point center, uint8_t color)
{
	// Start at lowest x,y (southeast is positive x and southwest is positive y)
	// First row (y = 0)
	//out.SetPixel(center + AmOffset(AmWidthOffset::None, AmHeightOffset::HalfTileUp), color);
	out.SetPixel(center + AmOffset(AmWidthOffset::QuarterTileLeft, AmHeightOffset::QuarterTileUp), color);
	out.SetPixel(center + AmOffset(AmWidthOffset::HalfTileLeft, AmHeightOffset::None), color);
	//out.SetPixel(center + AmOffset(AmWidthOffset::ThreeQuartersTileLeft, AmHeightOffset::QuarterTileDown), color);

	// Second row (y = 1)
	//out.SetPixel(center + AmOffset(AmWidthOffset::QuarterTileRight, AmHeightOffset::QuarterTileUp), color);
	out.SetPixel(center + AmOffset(AmWidthOffset::None, AmHeightOffset::None), color);
	out.SetPixel(center + AmOffset(AmWidthOffset::QuarterTileLeft, AmHeightOffset::QuarterTileDown), color);
	//out.SetPixel(center + AmOffset(AmWidthOffset::HalfTileLeft, AmHeightOffset::HalfTileDown), color);

	// Third row (y = 2)
	//out.SetPixel(center + AmOffset(AmWidthOffset::HalfTileRight, AmHeightOffset::None), color);
	out.SetPixel(center + AmOffset(AmWidthOffset::QuarterTileRight, AmHeightOffset::QuarterTileDown), color);
	out.SetPixel(center + AmOffset(AmWidthOffset::None, AmHeightOffset::HalfTileDown), color);
	//out.SetPixel(center + AmOffset(AmWidthOffset::QuarterTileLeft, AmHeightOffset::ThreeQuartersTileDown), color);

	// Fourth row (y = 3)
	//out.SetPixel(center + AmOffset(AmWidthOffset::ThreeQuartersTileRight, AmHeightOffset::QuarterTileDown), color);
	//out.SetPixel(center + AmOffset(AmWidthOffset::HalfTileRight, AmHeightOffset::HalfTileDown), color);
	//out.SetPixel(center + AmOffset(AmWidthOffset::QuarterTileRight, AmHeightOffset::ThreeQuartersTileDown), color);
	//out.SetPixel(center + AmOffset(AmWidthOffset::None, AmHeightOffset::FullTileDown), color);
}

void DrawHorizontalBridgeLava(const Surface &out, Point center, uint8_t color)
{
	// Start at lowest x,y (southeast is positive x and southwest is positive y)
	// First row (y = 0)
	//out.SetPixel(center + AmOffset(AmWidthOffset::None, AmHeightOffset::HalfTileUp), color);
	out.SetPixel(center + AmOffset(AmWidthOffset::QuarterTileLeft, AmHeightOffset::QuarterTileUp), color); // maybe first row too??
	out.SetPixel(center + AmOffset(AmWidthOffset::HalfTileLeft, AmHeightOffset::None), color);
	//out.SetPixel(center + AmOffset(AmWidthOffset::ThreeQuartersTileLeft, AmHeightOffset::QuarterTileDown), color);

	// Second row (y = 1)
	//out.SetPixel(center + AmOffset(AmWidthOffset::QuarterTileRight, AmHeightOffset::QuarterTileUp), color);
	//out.SetPixel(center + AmOffset(AmWidthOffset::None, AmHeightOffset::None), color);
	//out.SetPixel(center + AmOffset(AmWidthOffset::QuarterTileLeft, AmHeightOffset::QuarterTileDown), color);
	//out.SetPixel(center + AmOffset(AmWidthOffset::HalfTileLeft, AmHeightOffset::HalfTileDown), color);

	// Third row (y = 2)
	//out.SetPixel(center + AmOffset(AmWidthOffset::HalfTileRight, AmHeightOffset::None), color);
	out.SetPixel(center + AmOffset(AmWidthOffset::QuarterTileRight, AmHeightOffset::QuarterTileDown), color);
	out.SetPixel(center + AmOffset(AmWidthOffset::None, AmHeightOffset::HalfTileDown), color);
	//out.SetPixel(center + AmOffset(AmWidthOffset::QuarterTileLeft, AmHeightOffset::ThreeQuartersTileDown), color);

	// Fourth row (y = 3)
	//out.SetPixel(center + AmOffset(AmWidthOffset::ThreeQuartersTileRight, AmHeightOffset::QuarterTileDown), color);
	out.SetPixel(center + AmOffset(AmWidthOffset::HalfTileRight, AmHeightOffset::HalfTileDown), color);
	out.SetPixel(center + AmOffset(AmWidthOffset::QuarterTileRight, AmHeightOffset::ThreeQuartersTileDown), color);
	//out.SetPixel(center + AmOffset(AmWidthOffset::None, AmHeightOffset::FullTileDown), color);
}

void DrawVerticalBridgeLava(const Surface &out, Point center, uint8_t color)
{
	// Start at lowest x,y (southeast is positive x and southwest is positive y)
	// First row (y = 0)
	//out.SetPixel(center + AmOffset(AmWidthOffset::None, AmHeightOffset::HalfTileUp), color);
	//out.SetPixel(center + AmOffset(AmWidthOffset::QuarterTileLeft, AmHeightOffset::QuarterTileUp), color);
	//out.SetPixel(center + AmOffset(AmWidthOffset::HalfTileLeft, AmHeightOffset::None), color);
	//out.SetPixel(center + AmOffset(AmWidthOffset::ThreeQuartersTileLeft, AmHeightOffset::QuarterTileDown), color);

	// Second row (y = 1)
	out.SetPixel(center + AmOffset(AmWidthOffset::QuarterTileRight, AmHeightOffset::QuarterTileUp), color); // maybe this too?
	//out.SetPixel(center + AmOffset(AmWidthOffset::None, AmHeightOffset::None), color);
	out.SetPixel(center + AmOffset(AmWidthOffset::QuarterTileLeft, AmHeightOffset::QuarterTileDown), color);
	out.SetPixel(center + AmOffset(AmWidthOffset::HalfTileLeft, AmHeightOffset::HalfTileDown), color);

	// Third row (y = 2)
	out.SetPixel(center + AmOffset(AmWidthOffset::HalfTileRight, AmHeightOffset::None), color); // maybe this too?
	//out.SetPixel(center + AmOffset(AmWidthOffset::QuarterTileRight, AmHeightOffset::QuarterTileDown), color);
	out.SetPixel(center + AmOffset(AmWidthOffset::None, AmHeightOffset::HalfTileDown), color);
	out.SetPixel(center + AmOffset(AmWidthOffset::QuarterTileLeft, AmHeightOffset::ThreeQuartersTileDown), color);

	// Fourth row (y = 3)
	//out.SetPixel(center + AmOffset(AmWidthOffset::ThreeQuartersTileRight, AmHeightOffset::QuarterTileDown), color);
	//out.SetPixel(center + AmOffset(AmWidthOffset::HalfTileRight, AmHeightOffset::HalfTileDown), color);
	//out.SetPixel(center + AmOffset(AmWidthOffset::QuarterTileRight, AmHeightOffset::ThreeQuartersTileDown), color);
	//out.SetPixel(center + AmOffset(AmWidthOffset::None, AmHeightOffset::FullTileDown), color);
}



/**
 * @brief Draw 4 south-east facing lines, used to communicate trigger locations to the player.
 */
void DrawStairs(const Surface &out, Point center, uint8_t color)
{
	constexpr int NumStairSteps = 4;
	const Displacement offset = AmOffset(AmWidthOffset::QuarterTileLeft, AmHeightOffset::QuarterTileDown);

	// Initial point based on the 'center' position.
	Point p = center + AmOffset(AmWidthOffset::QuarterTileLeft, AmHeightOffset::QuarterTileUp);

	for (int i = 0; i < NumStairSteps; ++i) {
		DrawMapLineSE(out, p, AmLine(AmLineLength::DoubleTile), color);
		p += offset;
	}
}

/**
 * @brief Draw half-tile length lines to connect walls to any walls to the north-west and/or north-east
 */
void DrawWallConnections(const Surface &out, Point center, AutomapTile nwTile, AutomapTile neTile, uint8_t colorDim)
{
	bool doorCorrection = false;
	if (IsAnyOf(nwTile.type, AutomapTile::Types::Horizontal, AutomapTile::Types::FenceHorizontal, AutomapTile::Types::Cross, AutomapTile::Types::CaveVerticalWoodCross, AutomapTile::Types::CaveRightCorner)
	    && !nwTile.HasFlag(AutomapTile::Flags::HorizontalArch) && !neTile.HasFlag(AutomapTile::Flags::VerticalArch)) {
		if (!IsAnyOf(leveltype, DTYPE_CATACOMBS) && nwTile.HasFlag(AutomapTile::Flags::HorizontalDoor)) {
			doorCorrection = true;
		}
		DrawMapLineSE(out, center + AmOffset(AmWidthOffset::QuarterTileLeft, AmHeightOffset::ThreeQuartersTileUp, doorCorrection), AmLine(AmLineLength::HalfTile, doorCorrection), colorDim);
	}
	doorCorrection = false;
	if (IsAnyOf(neTile.type, AutomapTile::Types::Vertical, AutomapTile::Types::FenceVertical, AutomapTile::Types::Cross, AutomapTile::Types::CaveHorizontalWoodCross, AutomapTile::Types::CaveLeftCorner)
	    && !nwTile.HasFlag(AutomapTile::Flags::HorizontalArch) && !neTile.HasFlag(AutomapTile::Flags::VerticalArch)) {
		if (neTile.HasFlag(AutomapTile::Flags::VerticalDoor)) {
			doorCorrection = true;
		}
		DrawMapLineNE(out, center + AmOffset(AmWidthOffset::None, AmHeightOffset::HalfTileUp), AmLine(AmLineLength::HalfTile, doorCorrection), colorDim);
	}
}

/**
 * Left-facing obstacle
 */
void DrawHorizontal(const Surface &out, Point center, AutomapTile tile, AutomapTile nwTile, AutomapTile neTile, AutomapTile seTile, uint8_t colorBright, uint8_t colorDim, bool noConnection)
{
	AmWidthOffset w = AmWidthOffset::None;
	AmHeightOffset h = AmHeightOffset::HalfTileUp;
	AmLineLength l = AmLineLength::FullAndHalfTile;

	if (neTile.HasFlag(AutomapTile::Flags::VerticalArch)
	    || nwTile.HasFlag(AutomapTile::Flags::HorizontalArch)
	    || tile.HasFlag(AutomapTile::Flags::VerticalArch)
	    || tile.HasFlag(AutomapTile::Flags::HorizontalArch)) {
		noConnection = true;
		w = AmWidthOffset::QuarterTileRight;
		h = AmHeightOffset::QuarterTileUp;
		l = AmLineLength::FullTile;
		DrawDiamond(out, center, colorDim);
	}
	if (IsAnyOf(leveltype, DTYPE_CAVES, DTYPE_NEST)
	    && IsAnyOf(tile.type, AutomapTile::Types::CaveVerticalCross, AutomapTile::Types::CaveVerticalWoodCross)
	    && !(IsAnyOf(seTile.type, AutomapTile::Types::Horizontal, AutomapTile::Types::CaveVerticalCross, AutomapTile::Types::CaveVerticalWoodCross, AutomapTile::Types::Corner))) {
		l = AmLineLength::FullTile;
	}
	if (!tile.HasFlag(AutomapTile::Flags::HorizontalPassage)) {
		DrawMapLineSE(out, center + AmOffset(w, h), AmLine(l), colorDim);
		return;
	}
	if (tile.HasFlag(AutomapTile::Flags::HorizontalDoor)) {
		DrawMapHorizontalDoorOrGrate(out, center, colorBright, colorDim);
	} else if (tile.HasFlag(AutomapTile::Flags::HorizontalGrate)) {
		DrawMapHorizontalDoorOrGrate(out, center, colorDim, colorDim);
	}
}

/**
 * Right-facing obstacle
 */
void DrawVertical(const Surface &out, Point center, AutomapTile tile, AutomapTile nwTile, AutomapTile neTile, AutomapTile swTile, uint8_t colorBright, uint8_t colorDim, bool noConnection)
{
	AmWidthOffset w = AmWidthOffset::ThreeQuartersTileLeft;
	AmHeightOffset h = AmHeightOffset::QuarterTileDown;
	AmLineLength l = AmLineLength::FullAndHalfTile;

	if (neTile.HasFlag(AutomapTile::Flags::VerticalArch)
	    || nwTile.HasFlag(AutomapTile::Flags::HorizontalArch)
	    || tile.HasFlag(AutomapTile::Flags::VerticalArch)
	    || tile.HasFlag(AutomapTile::Flags::HorizontalArch)) {
		noConnection = true;
		l = AmLineLength::FullTile;
		DrawDiamond(out, center, colorDim);
	}
	if (IsAnyOf(leveltype, DTYPE_CAVES, DTYPE_NEST)
	    && IsAnyOf(tile.type, AutomapTile::Types::CaveHorizontalCross, AutomapTile::Types::CaveHorizontalWoodCross)
	    && !(IsAnyOf(swTile.type, AutomapTile::Types::Vertical, AutomapTile::Types::CaveHorizontalCross, AutomapTile::Types::CaveHorizontalWoodCross, AutomapTile::Types::Corner))) {
		w = AmWidthOffset::HalfTileLeft;
		h = AmHeightOffset::None;
		l = AmLineLength::FullTile;
	}
	if (!tile.HasFlag(AutomapTile::Flags::VerticalPassage)) {
		DrawMapLineNE(out, center + AmOffset(w, h), AmLine(l), colorDim);
		return;
	}
	if (tile.HasFlag(AutomapTile::Flags::VerticalDoor)) {
		DrawMapVerticalDoorOrGrate(out, center, colorBright, colorDim);
	} else if (tile.HasFlag(AutomapTile::Flags::VerticalGrate)) {
		DrawMapVerticalDoorOrGrate(out, center, colorDim, colorDim);
	}
}

/**
 * @brief Draw half-tile length lines to connect walls to any walls to the south-west and/or south-east
 * (For caves the horizontal/vertical flags are swapped)
 */
void DrawCaveWallConnections(const Surface &out, Point center, AutomapTile swTile, AutomapTile seTile, uint8_t colorDim)
{
	bool doorCorrection = false;
	if (IsAnyOf(swTile.type, AutomapTile::Types::CaveVertical, AutomapTile::Types::CaveVerticalWood, AutomapTile::Types::CaveCross, AutomapTile::Types::CaveWoodCross, AutomapTile::Types::CaveRightWoodCross, AutomapTile::Types::CaveLeftWoodCross, AutomapTile::Types::CaveRightCorner)) {
		//if (swTile.HasFlag(AutomapTile::Flags::HorizontalDoor)) {
		//	doorCorrection = true;
		//}
		DrawMapLineNE(out, center + AmOffset(AmWidthOffset::QuarterTileLeft, AmHeightOffset::ThreeQuartersTileDown, doorCorrection), AmLine(AmLineLength::HalfTile, doorCorrection), colorDim);
	}
	doorCorrection = false;
	if (IsAnyOf(seTile.type, AutomapTile::Types::CaveHorizontal, AutomapTile::Types::CaveHorizontalWood, AutomapTile::Types::CaveCross, AutomapTile::Types::CaveWoodCross, AutomapTile::Types::CaveRightWoodCross, AutomapTile::Types::CaveLeftWoodCross, AutomapTile::Types::CaveLeftCorner)) {
		//if (seTile.HasFlag(AutomapTile::Flags::VerticalDoor)) {
		//doorCorrection = true;
		//}
		DrawMapLineSE(out, center + AmOffset(AmWidthOffset::None, AmHeightOffset::HalfTileDown), AmLine(AmLineLength::HalfTile, doorCorrection), colorDim);
	}
}

/**
 * For caves the horizontal/vertical flags are swapped
 */
void DrawCaveHorizontal(const Surface &out, Point center, AutomapTile tile, uint8_t colorBright, uint8_t colorDim)
{
	if (tile.HasFlag(AutomapTile::Flags::VerticalDoor)) {
		DrawMapHorizontalDoorOrGrate(out, center, colorBright, colorDim);
	} else {
		AmWidthOffset w;
		AmHeightOffset h;
		AmLineLength l;

		if (IsAnyOf(tile.type, AutomapTile::Types::CaveHorizontalCross, AutomapTile::Types::CaveHorizontalWoodCross)) {
			w = AmWidthOffset::HalfTileLeft;
			h = AmHeightOffset::None;
			l = AmLineLength::FullTile;
		} else {
			w = AmWidthOffset::ThreeQuartersTileLeft;
			h = AmHeightOffset::QuarterTileUp;
			l = AmLineLength::FullAndHalfTile;
		}
		if (!(IsAnyOf(tile.type, AutomapTile::Types::CaveHorizontalWood, AutomapTile::Types::CaveHorizontalWoodCross, AutomapTile::Types::CaveWoodCross, AutomapTile::Types::CaveLeftWoodCross))) {
			out.SetPixel(center + AmOffset(AmWidthOffset::ThreeQuartersTileLeft, AmHeightOffset::QuarterTileDown), colorDim);
			out.SetPixel(center + AmOffset(AmWidthOffset::HalfTileLeft, AmHeightOffset::HalfTileDown), colorDim);
			out.SetPixel(center + AmOffset(AmWidthOffset::QuarterTileLeft, AmHeightOffset::ThreeQuartersTileDown), colorDim);
			out.SetPixel(center + AmOffset(AmWidthOffset::None, AmHeightOffset::FullTileDown), colorDim);
		}

		DrawMapLineSE(out, center + AmOffset(w, h), AmLine(l), colorDim);
	}
}

/**
 * For caves the horizontal/vertical flags are swapped
 */
void DrawCaveVertical(const Surface &out, Point center, AutomapTile tile, uint8_t colorBright, uint8_t colorDim)
{
	if (tile.HasFlag(AutomapTile::Flags::HorizontalDoor)) {
		DrawMapVerticalDoorOrGrate(out, center, colorBright, colorDim);
	} else {
		AmWidthOffset w;
		AmHeightOffset h;
		AmLineLength l;

		if (IsAnyOf(tile.type, AutomapTile::Types::CaveVerticalCross, AutomapTile::Types::CaveVerticalWoodCross)) {
			l = AmLineLength::FullTile;
		} else {
			l = AmLineLength::FullAndHalfTile;
		}
		if (!(IsAnyOf(tile.type, AutomapTile::Types::CaveVerticalWood, AutomapTile::Types::CaveVerticalWoodCross, AutomapTile::Types::CaveWoodCross, AutomapTile::Types::CaveRightWoodCross))) {
			out.SetPixel(center + AmOffset(AmWidthOffset::None, AmHeightOffset::FullTileDown), colorDim);
			out.SetPixel(center + AmOffset(AmWidthOffset::QuarterTileRight, AmHeightOffset::ThreeQuartersTileDown), colorDim);
			out.SetPixel(center + AmOffset(AmWidthOffset::HalfTileRight, AmHeightOffset::HalfTileDown), colorDim);
			out.SetPixel(center + AmOffset(AmWidthOffset::ThreeQuartersTileRight, AmHeightOffset::QuarterTileDown), colorDim);
		}

		DrawMapLineNE(out, { center + AmOffset(AmWidthOffset::None, AmHeightOffset::HalfTileDown) }, AmLine(l), colorDim);
	}
}

void DrawCaveLeftCorner(const Surface &out, Point center, uint8_t colorDim)
{
	DrawMapLineSE(out, center + AmOffset(AmWidthOffset::ThreeQuartersTileLeft, AmHeightOffset::QuarterTileUp), AmLine(AmLineLength::HalfTile), colorDim);
	DrawMapLineNE(out, center + AmOffset(AmWidthOffset::ThreeQuartersTileLeft, AmHeightOffset::QuarterTileDown), AmLine(AmLineLength::HalfTile), colorDim);
}

void DrawCaveRightCorner(const Surface &out, Point center, uint8_t colorDim)
{
	DrawMapLineSE(out, center + AmOffset(AmWidthOffset::HalfTileRight, AmHeightOffset::None), AmLine(AmLineLength::HalfTile), colorDim);
	DrawMapLineNE(out, center + AmOffset(AmWidthOffset::HalfTileRight, AmHeightOffset::None), AmLine(AmLineLength::HalfTile), colorDim);
}

/**
 * @brief Check if a given tile has the provided AutomapTile flag
 */
bool HasAutomapFlag(Point position, AutomapTile::Flags type)
{
	if (position.x < 0 || position.x >= DMAXX || position.y < 0 || position.y >= DMAXX) {
		return false;
	}

	return AutomapTypeTiles[dungeon[position.x][position.y]].HasFlag(type);
}

/**
 * @brief Returns the automap shape at the given coordinate.
 */
AutomapTile GetAutomapType(Point position)
{
	if (position.x < 0 || position.x >= DMAXX || position.y < 0 || position.y >= DMAXX) {
		return {};
	}

	AutomapTile tile = AutomapTypeTiles[dungeon[position.x][position.y]];
	if (tile.type == AutomapTile::Types::Corner) {
		if (HasAutomapFlag({ position.x - 1, position.y }, AutomapTile::Flags::HorizontalArch)) {
			if (HasAutomapFlag({ position.x, position.y - 1 }, AutomapTile::Flags::VerticalArch)) {
				tile.type = AutomapTile::Types::Diamond;
			}
		}
	}

	return tile;
}

/**
 * @brief Returns the automap shape at the given coordinate.
 */
AutomapTile GetAutomapTypeView(Point map)
{
	if (map.x == -1 && map.y >= 0 && map.y < DMAXY && AutomapView[0][map.y] != MAP_EXP_NONE) {
		if (HasAutomapFlag({ 0, map.y + 1 }, AutomapTile::Flags::Dirt) && HasAutomapFlag({ 0, map.y }, AutomapTile::Flags::Dirt) && HasAutomapFlag({ 0, map.y - 1 }, AutomapTile::Flags::Dirt)) {
			return {};
		}
		return { AutomapTile::Types::None, AutomapTile::Flags::Dirt };
	}

	if (map.y == -1 && map.x >= 0 && map.x < DMAXY && AutomapView[map.x][0] != MAP_EXP_NONE) {
		if (HasAutomapFlag({ map.x + 1, 0 }, AutomapTile::Flags::Dirt) && HasAutomapFlag({ map.x, 0 }, AutomapTile::Flags::Dirt) && HasAutomapFlag({ map.x - 1, 0 }, AutomapTile::Flags::Dirt)) {
			return {};
		}
		return { AutomapTile::Types::None, AutomapTile::Flags::Dirt };
	}

	if (map.x < 0 || map.x >= DMAXX) {
		return {};
	}
	if (map.y < 0 || map.y >= DMAXX) {
		return {};
	}
	if (AutomapView[map.x][map.y] == MAP_EXP_NONE) {
		return {};
	}

	uint8_t tile = dungeon[map.x][map.y];
	// This tile incorrectly has flags for both HorizontalArch and VerticalArch in the amp data
	if (leveltype == DTYPE_CATACOMBS && tile == 42)
		return { AutomapTile::Types::Cross, AutomapTile::Flags::VerticalArch };
	// Manually set cave corners so we can draw them
	if (IsAnyOf(leveltype, DTYPE_CAVES, DTYPE_NEST)) {
		switch (tile) {
		case 5:
			return { AutomapTile::Types::CaveBottomCorner };
		case 13:
			return { AutomapTile::Types::CaveRightCorner };
		case 14:
			return { AutomapTile::Types::CaveLeftCorner };
		case 130:
		case 132:
			return { AutomapTile::Types::CaveHorizontalWoodCross };
		case 134:
		case 136:
		case 151:
			return { AutomapTile::Types::CaveHorizontalWood };
		case 146:
		case 148:
			return { AutomapTile::Types::CaveHorizontalWood, AutomapTile::Flags::VerticalDoor };
		case 131:
		case 133:
			return { AutomapTile::Types::CaveVerticalWoodCross };
		case 135:
		case 137:
		case 152:
			return { AutomapTile::Types::CaveVerticalWood };
		case 147:
		case 149:
			return { AutomapTile::Types::CaveVerticalWood, AutomapTile::Flags::HorizontalDoor };
		case 138:
		case 141:
		case 142:
		case 143:
			return { AutomapTile::Types::CaveWoodCross };
		case 139:
			return { AutomapTile::Types::CaveRightWoodCross };
		case 140:
			return { AutomapTile::Types::CaveLeftWoodCross };
		case 15:
			return { AutomapTile::Types::HorizontalLavaThin };
		case 16:
			return { AutomapTile::Types::HorizontalLavaThin };
		case 17:
			return { AutomapTile::Types::VerticalLavaThin };
		case 18:
			return { AutomapTile::Types::VerticalLavaThin };
		case 19:
			return { AutomapTile::Types::BendSouthLavaThin };
		case 20:
			return { AutomapTile::Types::BendWestLavaThin };
		case 21:
			return { AutomapTile::Types::BendEastLavaThin };
		case 22:
			return { AutomapTile::Types::BendNorthLavaThin };
		case 23:
			return { AutomapTile::Types::VerticalWallLava };
		case 24:
			return { AutomapTile::Types::HorizontalWallLava };
		case 25:
			return { AutomapTile::Types::SELava };
		case 26:
			return { AutomapTile::Types::SWLava };
		case 27:
			return { AutomapTile::Types::NELava };
		case 28:
			return { AutomapTile::Types::NWLava };
		case 29:
			return { AutomapTile::Types::SLava };
		case 30:
			return { AutomapTile::Types::WLava };
		case 31:
			return { AutomapTile::Types::ELava };
		case 32:
			return { AutomapTile::Types::NLava };
		case 33:
		case 34:
		case 35:
		case 36:
		case 37:
		case 38:
		case 39:
		case 40:
		case 41:
			return { AutomapTile::Types::Lava };
		case 42:
			return { AutomapTile::Types::CaveHorizontalWallLava };
		case 43:
			return { AutomapTile::Types::CaveVerticalWallLava };
		case 44:
			return { AutomapTile::Types::HorizontalBridgeLava };
		case 45:
			return { AutomapTile::Types::VerticalBridgeLava };
		}
	}

	return GetAutomapType(map);
}

/**
 * @brief Renders the given automap shape at the specified screen coordinates.
 */
void DrawAutomapTile(const Surface &out, Point center, Point map)
{
	AutomapTile tile = GetAutomapTypeView(map);
	AutomapTile nwTile = GetAutomapTypeView(map + Displacement { -1, 0 });
	AutomapTile neTile = GetAutomapTypeView(map + Displacement { 0, -1 });
	AutomapTile swTile = GetAutomapTypeView(map + Displacement { 0, 1 });
	AutomapTile seTile = GetAutomapTypeView(map + Displacement { 1, 0 });

	uint8_t colorBright = MapColorsBright;
	uint8_t colorDim = MapColorsDim;
	MapExplorationType explorationType = static_cast<MapExplorationType>(AutomapView[std::clamp(map.x, 0, DMAXX - 1)][std::clamp(map.y, 0, DMAXY - 1)]);

	switch (explorationType) {
	case MAP_EXP_SHRINE:
		colorDim = PAL16_GRAY + 11;
		colorBright = PAL16_GRAY + 3;
		break;
	case MAP_EXP_OTHERS:
		colorDim = PAL16_BEIGE + 10;
		colorBright = PAL16_BEIGE + 2;
		break;
	case MAP_EXP_SELF:
	case MAP_EXP_NONE:
	case MAP_EXP_OLD:
		break;
	}

	bool noConnect = false;

	// If the tile contains an arch, we draw a diamond and therefore don't want connection lines
	if (tile.HasFlag(AutomapTile::Flags::HorizontalArch) || tile.HasFlag(AutomapTile::Flags::VerticalArch)) {
		noConnect = true;
	}

	// These tilesets have doors where the connection lines would be drawn
	if (IsAnyOf(leveltype, DTYPE_CATACOMBS, DTYPE_CAVES, DTYPE_NEST) && (tile.HasFlag(AutomapTile::Flags::HorizontalDoor) || tile.HasFlag(AutomapTile::Flags::VerticalDoor)))
		noConnect = true;

	if (tile.HasFlag(AutomapTile::Flags::Dirt)) {
		DrawDirt(out, center, nwTile, colorDim);
	}

	if (tile.HasFlag(AutomapTile::Flags::Stairs)) {
		DrawStairs(out, center, colorBright);
	}

	if (!noConnect) {
		if (IsAnyOf(leveltype, DTYPE_CAVES, DTYPE_NEST))
			DrawCaveWallConnections(out, center, swTile, seTile, colorDim);
		DrawWallConnections(out, center, nwTile, neTile, colorDim);
	}

	// debug
	int colorBright2 = PAL8_BLUE;
	int colorDim2 = PAL16_BLUE + 8;

	switch (tile.type) {
	case AutomapTile::Types::Diamond: // stand-alone column or other unpassable object
		DrawDiamond(out, center, colorDim);
		break;
	case AutomapTile::Types::Vertical:
	case AutomapTile::Types::FenceVertical:
		DrawVertical(out, center, tile, nwTile, neTile, swTile, colorBright, colorDim, noConnect);
		break;
	case AutomapTile::Types::Horizontal:
	case AutomapTile::Types::FenceHorizontal:
		DrawHorizontal(out, center, tile, nwTile, neTile, seTile, colorBright, colorDim, noConnect);
		break;
	case AutomapTile::Types::Cross:
		DrawVertical(out, center, tile, nwTile, neTile, swTile, colorBright, colorDim, noConnect);
		DrawHorizontal(out, center, tile, nwTile, neTile, seTile, colorBright, colorDim, noConnect);
		break;
	case AutomapTile::Types::CaveHorizontalCross:
	case AutomapTile::Types::CaveHorizontalWoodCross:
		DrawVertical(out, center, tile, nwTile, neTile, swTile, colorBright, colorDim, noConnect);
		DrawCaveHorizontal(out, center, tile, colorBright, colorDim);
		break;
	case AutomapTile::Types::CaveVerticalCross:
	case AutomapTile::Types::CaveVerticalWoodCross:
		DrawHorizontal(out, center, tile, nwTile, neTile, seTile, colorBright, colorDim, noConnect);
		DrawCaveVertical(out, center, tile, colorBright, colorDim);
		break;
	case AutomapTile::Types::CaveHorizontal:
	case AutomapTile::Types::CaveHorizontalWood:
		DrawCaveHorizontal(out, center, tile, colorBright, colorDim);
		break;
	case AutomapTile::Types::CaveVertical:
	case AutomapTile::Types::CaveVerticalWood:
		DrawCaveVertical(out, center, tile, colorBright, colorDim);
		break;
	case AutomapTile::Types::CaveCross:
	case AutomapTile::Types::CaveWoodCross:
	case AutomapTile::Types::CaveRightWoodCross:
	case AutomapTile::Types::CaveLeftWoodCross:
		DrawCaveHorizontal(out, center, tile, colorBright, colorDim);
		DrawCaveVertical(out, center, tile, colorBright, colorDim);
		break;
	case AutomapTile::Types::CaveLeftCorner:
		DrawCaveLeftCorner(out, center, colorDim);
		break;
	case AutomapTile::Types::CaveRightCorner:
		DrawCaveRightCorner(out, center, colorDim);
		break;
	case AutomapTile::Types::Corner:
	case AutomapTile::Types::CaveBottomCorner:
	case AutomapTile::Types::None:
		break;
	case AutomapTile::Types::Bridge:
		DrawBridge(out, center, MapColorsItem);
		break;
	case AutomapTile::Types::River:
		DrawRiver(out, center, MapColorsItem);
		break;
	case AutomapTile::Types::RiverCornerEast:
		DrawRiverCornerEast(out, center, MapColorsItem);
		break;
	case AutomapTile::Types::RiverCornerNorth:
		DrawRiverCornerNorth(out, center, MapColorsItem);
		break;
	case AutomapTile::Types::RiverCornerSouth:
		DrawRiverCornerSouth(out, center, MapColorsItem);
		break;
	case AutomapTile::Types::RiverCornerWest:
		DrawRiverCornerWest(out, center, MapColorsItem);
		break;
	case AutomapTile::Types::RiverForkIn:
		DrawRiverForkIn(out, center, MapColorsItem);
		break;
	case AutomapTile::Types::RiverForkOut:
		DrawRiverForkOut(out, center, MapColorsItem);
		break;
	case AutomapTile::Types::RiverLeftIn:
		DrawRiverLeftIn(out, center, MapColorsItem);
		break;
	case AutomapTile::Types::RiverLeftOut:
		DrawRiverLeftOut(out, center, MapColorsItem);
		break;
	case AutomapTile::Types::RiverRightIn:
		DrawRiverRightIn(out, center, MapColorsItem);
		break;
	case AutomapTile::Types::RiverRightOut:
		DrawRiverRightOut(out, center, MapColorsItem);
		break;
	case AutomapTile::Types::HorizontalLavaThin:
		DrawHorizontalLavaThin(out, center, MapColorsLava);
		break;
	case AutomapTile::Types::VerticalLavaThin:
		DrawVerticalLavaThin(out, center, MapColorsLava);
		break;
	case AutomapTile::Types::BendSouthLavaThin:
		DrawBendSouthLavaThin(out, center, MapColorsLava);
		break;
	case AutomapTile::Types::BendWestLavaThin:
		DrawBendWestLavaThin(out, center, MapColorsLava);
		break;
	case AutomapTile::Types::BendEastLavaThin:
		DrawBendEastLavaThin(out, center, MapColorsLava);
		break;
	case AutomapTile::Types::BendNorthLavaThin:
		DrawBendNorthLavaThin(out, center, MapColorsLava);
		break;
	case AutomapTile::Types::VerticalWallLava:
		DrawVertical(out, center, tile, nwTile, neTile, swTile, colorBright, colorDim, noConnect);
		DrawVerticalWallLava(out, center, MapColorsLava);
		break;
	case AutomapTile::Types::HorizontalWallLava:
		DrawHorizontal(out, center, tile, nwTile, neTile, swTile, colorBright, colorDim, noConnect);
		DrawHorizontalWallLava(out, center, MapColorsLava);
		break;
	case AutomapTile::Types::SELava:
		DrawSELava(out, center, MapColorsLava);
		break;
	case AutomapTile::Types::SWLava:
		DrawSWLava(out, center, MapColorsLava);
		break;
	case AutomapTile::Types::NELava:
		DrawNELava(out, center, MapColorsLava);
		break;
	case AutomapTile::Types::NWLava:
		DrawNWLava(out, center, MapColorsLava);
		break;
	case AutomapTile::Types::SLava:
		DrawSLava(out, center, MapColorsLava);
		break;
	case AutomapTile::Types::WLava:
		DrawWLava(out, center, MapColorsLava);
		break;
	case AutomapTile::Types::ELava:
		DrawELava(out, center, MapColorsLava);
		break;
	case AutomapTile::Types::NLava:
		DrawNLava(out, center, MapColorsLava);
		break;
	case AutomapTile::Types::Lava:
		DrawLava(out, center, MapColorsLava);
		break;
	case AutomapTile::Types::CaveHorizontalWallLava:
		DrawCaveHorizontal(out, center, tile, colorBright, colorDim);
		DrawCaveHorizontalWallLava(out, center, MapColorsLava);
		break;
	case AutomapTile::Types::CaveVerticalWallLava:
		DrawCaveVertical(out, center, tile, colorBright, colorDim);
		DrawCaveVerticalWallLava(out, center, MapColorsLava);
		break;
	case AutomapTile::Types::HorizontalBridgeLava:
		DrawHorizontalBridgeLava(out, center, MapColorsLava);
		break;
	case AutomapTile::Types::VerticalBridgeLava:
		DrawVerticalBridgeLava(out, center, MapColorsLava);
		break;
	}
}

void SearchAutomapItem(const Surface &out, const Displacement &myPlayerOffset, int searchRadius, tl::function_ref<bool(Point position)> highlightTile)
{
	const Player &player = *MyPlayer;
	Point tile = player.position.tile;
	if (player._pmode == PM_WALK_SIDEWAYS) {
		tile = player.position.future;
		if (player._pdir == Direction::West)
			tile.x++;
		else
			tile.y++;
	}

	const int startX = std::clamp(tile.x - searchRadius, 0, MAXDUNX);
	const int startY = std::clamp(tile.y - searchRadius, 0, MAXDUNY);

	const int endX = std::clamp(tile.x + searchRadius, 0, MAXDUNX);
	const int endY = std::clamp(tile.y + searchRadius, 0, MAXDUNY);

	for (int i = startX; i < endX; i++) {
		for (int j = startY; j < endY; j++) {
			if (!highlightTile({ i, j }))
				continue;

			int px = i - 2 * AutomapOffset.deltaX - ViewPosition.x;
			int py = j - 2 * AutomapOffset.deltaY - ViewPosition.y;

			Point screen = {
				(myPlayerOffset.deltaX * AutoMapScale / 100 / 2) + (px - py) * AmLine(AmLineLength::DoubleTile) + gnScreenWidth / 2,
				(myPlayerOffset.deltaY * AutoMapScale / 100 / 2) + (px + py) * AmLine(AmLineLength::FullTile) + (gnScreenHeight - GetMainPanel().size.height) / 2
			};

			if (CanPanelsCoverView()) {
				if (IsRightPanelOpen())
					screen.x -= 160;
				if (IsLeftPanelOpen())
					screen.x += 160;
			}
			screen.y -= AmLine(AmLineLength::FullTile);
			DrawDiamond(out, screen, MapColorsItem);
		}
	}
}

/**
 * @brief Renders an arrow on the automap, centered on and facing the direction of the player.
 */
void DrawAutomapPlr(const Surface &out, const Displacement &myPlayerOffset, int playerId)
{
	int playerColor = MapColorsPlayer + (8 * playerId) % 128;

	Player &player = Players[playerId];
	Point tile = player.position.tile;
	if (player._pmode == PM_WALK_SIDEWAYS) {
		tile = player.position.future;
	}

	int px = tile.x - 2 * AutomapOffset.deltaX - ViewPosition.x;
	int py = tile.y - 2 * AutomapOffset.deltaY - ViewPosition.y;

	Displacement playerOffset = {};
	if (player.isWalking())
		playerOffset = GetOffsetForWalking(player.AnimInfo, player._pdir);

	Point base = {
		((playerOffset.deltaX + myPlayerOffset.deltaX) * AutoMapScale / 100 / 2) + (px - py) * AmLine(AmLineLength::DoubleTile) + gnScreenWidth / 2,
		((playerOffset.deltaY + myPlayerOffset.deltaY) * AutoMapScale / 100 / 2) + (px + py) * AmLine(AmLineLength::FullTile) + (gnScreenHeight - GetMainPanel().size.height) / 2 + AmOffset(AmWidthOffset::None, AmHeightOffset::FullTileDown).deltaY
	};

	if (CanPanelsCoverView()) {
		if (IsRightPanelOpen())
			base.x -= gnScreenWidth / 4;
		if (IsLeftPanelOpen())
			base.x += gnScreenWidth / 4;
	}
	base.y -= AmLine(AmLineLength::DoubleTile);

	switch (player._pdir) {
	case Direction::North: {
		const Point point = base + AmOffset(AmWidthOffset::None, AmHeightOffset::FullTileUp);
		DrawVerticalLine(out, point, AmLine(AmLineLength::DoubleTile), playerColor);
		//DrawMapLineSteepNE(out, { point.x - AmLine(4), point.y + 2 * AmLine(4) }, AmLine(AmLineLength::HalfTile), playerColor);
		DrawMapLineSteepNE(out, point + AmOffset(AmWidthOffset::EighthTileLeft, AmHeightOffset::HalfTileDown), AmLine(AmLineLength::HalfTile), playerColor);
		//DrawMapLineSteepNW(out, { point.x + AmLine(4), point.y + 2 * AmLine(4) }, AmLine(AmLineLength::HalfTile), playerColor);
		DrawMapLineSteepNW(out, point + AmOffset(AmWidthOffset::EighthTileRight, AmHeightOffset::HalfTileDown), AmLine(AmLineLength::HalfTile), playerColor);
	} break;
	case Direction::NorthEast: {
		const Point point = base + AmOffset(AmWidthOffset::HalfTileRight, AmHeightOffset::HalfTileUp);
		DrawHorizontalLine(out, point + AmOffset(AmWidthOffset::QuarterTileLeft, AmHeightOffset::None), AmLine(AmLineLength::FullTile), playerColor);
		//DrawMapLineNE(out, { point.x - 2 * AmLine(8), point.y + AmLine(8) }, AmLine(AmLineLength::FullTile), playerColor);
		DrawMapLineNE(out, point + AmOffset(AmWidthOffset::HalfTileLeft, AmHeightOffset::HalfTileDown), AmLine(AmLineLength::FullTile), playerColor);
		DrawMapLineSteepSW(out, point, AmLine(AmLineLength::HalfTile), playerColor);
	} break;
	case Direction::East: {
		const Point point = base + AmOffset(AmWidthOffset::HalfTileRight, AmHeightOffset::None);
		DrawMapLineNW(out, point, AmLine(AmLineLength::HalfTile), playerColor);
		DrawHorizontalLine(out, point + AmOffset(AmWidthOffset::HalfTileLeft, AmHeightOffset::None), AmLine(AmLineLength::DoubleTile), playerColor);
		DrawMapLineSW(out, point, AmLine(AmLineLength::HalfTile), playerColor);
	} break;
	case Direction::SouthEast: {
		const Point point = base + AmOffset(AmWidthOffset::HalfTileRight, AmHeightOffset::HalfTileDown);
		DrawMapLineSteepNW(out, point, AmLine(AmLineLength::HalfTile), playerColor);
		DrawMapLineSE(out, point + AmOffset(AmWidthOffset::HalfTileLeft, AmHeightOffset::HalfTileUp), AmLine(AmLineLength::FullTile), playerColor);
		DrawHorizontalLine(out, point + AmOffset(AmWidthOffset::QuarterTileLeft, AmHeightOffset::None) + Displacement { -1, 0 }, AmLine(AmLineLength::FullTile) + 1, playerColor);
	} break;
	case Direction::South: {
		const Point point = base + AmOffset(AmWidthOffset::None, AmHeightOffset::FullTileDown);
		DrawVerticalLine(out, point + AmOffset(AmWidthOffset::None, AmHeightOffset::FullTileUp), AmLine(AmLineLength::DoubleTile), playerColor);
		DrawMapLineSteepSW(out, point + AmOffset(AmWidthOffset::EighthTileRight, AmHeightOffset::HalfTileUp), AmLine(AmLineLength::HalfTile), playerColor);
		DrawMapLineSteepSE(out, point + AmOffset(AmWidthOffset::EighthTileLeft, AmHeightOffset::HalfTileUp), AmLine(AmLineLength::HalfTile), playerColor);
	} break;
	case Direction::SouthWest: {
		const Point point = base + AmOffset(AmWidthOffset::HalfTileLeft, AmHeightOffset::HalfTileDown);
		DrawMapLineSteepNE(out, point, AmLine(AmLineLength::HalfTile), playerColor);
		DrawMapLineSW(out, point + AmOffset(AmWidthOffset::HalfTileRight, AmHeightOffset::HalfTileUp), AmLine(AmLineLength::FullTile), playerColor);
		DrawHorizontalLine(out, point, AmLine(AmLineLength::FullTile) + 1, playerColor);
	} break;
	case Direction::West: {
		const Point point = base + AmOffset(AmWidthOffset::HalfTileLeft, AmHeightOffset::None);
		DrawMapLineNE(out, point, AmLine(AmLineLength::HalfTile), playerColor);
		DrawHorizontalLine(out, point, AmLine(AmLineLength::DoubleTile) + 1, playerColor);
		DrawMapLineSE(out, point, AmLine(AmLineLength::HalfTile), playerColor);
	} break;
	case Direction::NorthWest: {
		const Point point = base + AmOffset(AmWidthOffset::HalfTileLeft, AmHeightOffset::HalfTileUp);
		DrawMapLineNW(out, point + AmOffset(AmWidthOffset::HalfTileRight, AmHeightOffset::HalfTileDown), AmLine(AmLineLength::FullTile), playerColor);
		DrawHorizontalLine(out, point, AmLine(AmLineLength::FullTile) + 1, playerColor);
		DrawMapLineSteepSE(out, point, AmLine(AmLineLength::HalfTile), playerColor);
	} break;
	case Direction::NoDirection:
		break;
	}
}

/**
 * @brief Renders game info, such as the name of the current level, and in multi player the name of the game and the game password.
 */
void DrawAutomapText(const Surface &out)
{
	Point linePosition { 8, 8 };

	if (gbIsMultiplayer) {
		if (GameName != "0.0.0.0" && !IsLoopback) {
			std::string description = std::string(_("Game: "));
			description.append(GameName);
			DrawString(out, description, linePosition);
			linePosition.y += 15;
		}

		std::string description;
		if (IsLoopback) {
			description = std::string(_("Offline Game"));
		} else if (!PublicGame) {
			description = std::string(_("Password: "));
			description.append(GamePassword);
		} else {
			description = std::string(_("Public Game"));
		}
		DrawString(out, description, linePosition);
		linePosition.y += 15;
	}

	if (setlevel) {
		DrawString(out, _(QuestLevelNames[setlvlnum]), linePosition);
		return;
	}

	std::string description;
	switch (leveltype) {
	case DTYPE_NEST:
		description = fmt::format(fmt::runtime(_("Level: Nest {:d}")), currlevel - 16);
		break;
	case DTYPE_CRYPT:
		description = fmt::format(fmt::runtime(_("Level: Crypt {:d}")), currlevel - 20);
		break;
	case DTYPE_TOWN:
		description = std::string(_("Town"));
		break;
	default:
		description = fmt::format(fmt::runtime(_("Level: {:d}")), currlevel);
		break;
	}

	DrawString(out, description, linePosition);
	linePosition.y += 15;
	std::string_view difficulty;
	switch (sgGameInitInfo.nDifficulty) {
	case DIFF_NORMAL:
		difficulty = _("Normal");
		break;
	case DIFF_NIGHTMARE:
		difficulty = _("Nightmare");
		break;
	case DIFF_HELL:
		difficulty = _("Hell");
		break;
	}

	std::string difficultyString = fmt::format(fmt::runtime(_(/* TRANSLATORS: {:s} means: Game Difficulty. */ "Difficulty: {:s}")), difficulty);
	DrawString(out, difficultyString, linePosition);
}

std::unique_ptr<AutomapTile[]> LoadAutomapData(size_t &tileCount)
{
	switch (leveltype) {
	case DTYPE_TOWN:
		return LoadFileInMem<AutomapTile>("levels\\towndata\\automap.amp", &tileCount);
	case DTYPE_CATHEDRAL:
		return LoadFileInMem<AutomapTile>("levels\\l1data\\l1.amp", &tileCount);
	case DTYPE_CATACOMBS:
		return LoadFileInMem<AutomapTile>("levels\\l2data\\l2.amp", &tileCount);
	case DTYPE_CAVES:
		return LoadFileInMem<AutomapTile>("levels\\l3data\\l3.amp", &tileCount);
	case DTYPE_HELL:
		return LoadFileInMem<AutomapTile>("levels\\l4data\\l4.amp", &tileCount);
	case DTYPE_NEST:
		return LoadFileInMem<AutomapTile>("nlevels\\l6data\\l6.amp", &tileCount);
	case DTYPE_CRYPT:
		return LoadFileInMem<AutomapTile>("nlevels\\l5data\\l5.amp", &tileCount);
	default:
		return nullptr;
	}
}

} // namespace

bool AutomapActive;
uint8_t AutomapView[DMAXX][DMAXY];
int AutoMapScale;
Displacement AutomapOffset;

void InitAutomapOnce()
{
	AutomapActive = false;
	AutoMapScale = 50;
}

void InitAutomap()
{
	size_t tileCount = 0;
	std::unique_ptr<AutomapTile[]> tileTypes = LoadAutomapData(tileCount);
	for (unsigned i = 0; i < tileCount; i++) {
		AutomapTypeTiles[i + 1] = tileTypes[i];
	}

	memset(AutomapView, 0, sizeof(AutomapView));

	for (auto &column : dFlags)
		for (auto &dFlag : column)
			dFlag &= ~DungeonFlag::Explored;
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

	AutoMapScale += 25;
}

void AutomapZoomOut()
{
	if (AutoMapScale <= 25)
		return;

	AutoMapScale -= 25;
}

void DrawAutomap(const Surface &out)
{
	Automap = { (ViewPosition.x - 8) / 2, (ViewPosition.y - 8) / 2 };
	if (leveltype != DTYPE_TOWN) {
		Automap += { -4, -4 };
	}
	while (Automap.x + AutomapOffset.deltaX < 0)
		AutomapOffset.deltaX++;
	while (Automap.x + AutomapOffset.deltaX >= DMAXX)
		AutomapOffset.deltaX--;

	while (Automap.y + AutomapOffset.deltaY < 0)
		AutomapOffset.deltaY++;
	while (Automap.y + AutomapOffset.deltaY >= DMAXY)
		AutomapOffset.deltaY--;

	Automap += AutomapOffset;

	const Player &myPlayer = *MyPlayer;
	Displacement myPlayerOffset = {};
	if (myPlayer.isWalking())
		myPlayerOffset = GetOffsetForWalking(myPlayer.AnimInfo, myPlayer._pdir, true);
	//myPlayerOffset += Displacement { -1, (leveltype != DTYPE_CAVES) ? TILE_HEIGHT - 1 : -1 };

	int d = (AutoMapScale * 64) / 100;
	int cells = 2 * (gnScreenWidth / 2 / d) + 1;
	if (((gnScreenWidth / 2) % d) != 0)
		cells++;
	if (((gnScreenWidth / 2) % d) >= (AutoMapScale * 32) / 100)
		cells++;
	if ((myPlayerOffset.deltaX + myPlayerOffset.deltaY) != 0)
		cells++;

	Point screen {
		gnScreenWidth / 2,
		(gnScreenHeight - GetMainPanel().size.height) / 2
	};

	screen += AmOffset(AmWidthOffset::None, AmHeightOffset::HalfTileDown);

	if ((cells & 1) != 0) {
		screen.x -= AmOffset(AmWidthOffset::DoubleTileRight, AmHeightOffset::None).deltaX * ((cells - 1) / 2);
		screen.y -= AmOffset(AmWidthOffset::None, AmHeightOffset::DoubleTileDown).deltaY * ((cells + 1) / 2);

	} else {
		screen.x -= AmOffset(AmWidthOffset::DoubleTileRight, AmHeightOffset::None).deltaX * (cells / 2) + AmOffset(AmWidthOffset::FullTileLeft, AmHeightOffset::None).deltaX;
		screen.y -= AmOffset(AmWidthOffset::None, AmHeightOffset::DoubleTileDown).deltaY * (cells / 2) + AmOffset(AmWidthOffset::None, AmHeightOffset::FullTileDown).deltaY;
	}
	if ((ViewPosition.x & 1) != 0) {
		screen.x -= AmOffset(AmWidthOffset::HalfTileRight, AmHeightOffset::None).deltaX;
		screen.y -= AmOffset(AmWidthOffset::None, AmHeightOffset::HalfTileDown).deltaY;
	}
	if ((ViewPosition.y & 1) != 0) {
		screen.x += AmOffset(AmWidthOffset::HalfTileRight, AmHeightOffset::None).deltaX;
		screen.y -= AmOffset(AmWidthOffset::None, AmHeightOffset::HalfTileDown).deltaY;
	}

	screen.x += AutoMapScale * myPlayerOffset.deltaX / 100 / 2;
	screen.y += AutoMapScale * myPlayerOffset.deltaY / 100 / 2;

	if (CanPanelsCoverView()) {
		if (IsRightPanelOpen()) {
			screen.x -= gnScreenWidth / 4;
		}
		if (IsLeftPanelOpen()) {
			screen.x += gnScreenWidth / 4;
		}
	}

	Point map = { Automap.x - cells, Automap.y - 1 };

	for (int i = 0; i <= cells + 1; i++) {
		Point tile1 = screen;
		for (int j = 0; j < cells; j++) {
			DrawAutomapTile(out, tile1, { map.x + j, map.y - j });
			tile1.x += AmOffset(AmWidthOffset::DoubleTileRight, AmHeightOffset::None).deltaX;
		}
		map.y++;

		Point tile2 = screen + AmOffset(AmWidthOffset::FullTileLeft, AmHeightOffset::FullTileDown);
		for (int j = 0; j <= cells; j++) {
			DrawAutomapTile(out, tile2, { map.x + j, map.y - j });
			tile2.x += AmOffset(AmWidthOffset::DoubleTileRight, AmHeightOffset::None).deltaX;
		}
		map.x++;
		screen.y += AmOffset(AmWidthOffset::None, AmHeightOffset::DoubleTileDown).deltaY;
	}

	//if (leveltype == DTYPE_CAVES)
	//	myPlayerOffset.deltaY += TILE_HEIGHT;
	for (size_t playerId = 0; playerId < Players.size(); playerId++) {
		Player &player = Players[playerId];
		if (player.isOnActiveLevel() && player.plractive && !player._pLvlChanging && (&player == MyPlayer || player.friendlyMode)) {
			DrawAutomapPlr(out, myPlayerOffset, playerId);
		}
	}

	//myPlayerOffset.deltaY -= TILE_HEIGHT / 2;
	if (AutoMapShowItems)
		SearchAutomapItem(out, myPlayerOffset, 8, [](Point position) { return dItem[position.x][position.y] != 0; });
#ifdef _DEBUG
	if (IsDebugAutomapHighlightNeeded())
		SearchAutomapItem(out, myPlayerOffset, std::max(MAXDUNX, MAXDUNY), ShouldHighlightDebugAutomapTile);
#endif

	DrawAutomapText(out);
}

void UpdateAutomapExplorer(Point map, MapExplorationType explorer)
{
	if (AutomapView[map.x][map.y] < explorer)
		AutomapView[map.x][map.y] = explorer;
}

void SetAutomapView(Point position, MapExplorationType explorer)
{
	const Point map { (position.x - 16) / 2, (position.y - 16) / 2 };

	if (map.x < 0 || map.x >= DMAXX || map.y < 0 || map.y >= DMAXY) {
		return;
	}

	UpdateAutomapExplorer(map, explorer);

	AutomapTile tile = GetAutomapType(map);
	bool solid = tile.HasFlag(AutomapTile::Flags::Dirt);

	switch (tile.type) {
	case AutomapTile::Types::Vertical:
		if (solid) {
			auto tileSW = GetAutomapType({ map.x, map.y + 1 });
			if (tileSW.type == AutomapTile::Types::Corner && tileSW.HasFlag(AutomapTile::Flags::Dirt))
				UpdateAutomapExplorer({ map.x, map.y + 1 }, explorer);
		} else if (HasAutomapFlag({ map.x - 1, map.y }, AutomapTile::Flags::Dirt)) {
			UpdateAutomapExplorer({ map.x - 1, map.y }, explorer);
		}
		break;
	case AutomapTile::Types::Horizontal:
		if (solid) {
			auto tileSE = GetAutomapType({ map.x + 1, map.y });
			if (tileSE.type == AutomapTile::Types::Corner && tileSE.HasFlag(AutomapTile::Flags::Dirt))
				UpdateAutomapExplorer({ map.x + 1, map.y }, explorer);
		} else if (HasAutomapFlag({ map.x, map.y - 1 }, AutomapTile::Flags::Dirt)) {
			UpdateAutomapExplorer({ map.x, map.y - 1 }, explorer);
		}
		break;
	case AutomapTile::Types::Cross:
		if (solid) {
			auto tileSW = GetAutomapType({ map.x, map.y + 1 });
			if (tileSW.type == AutomapTile::Types::Corner && tileSW.HasFlag(AutomapTile::Flags::Dirt))
				UpdateAutomapExplorer({ map.x, map.y + 1 }, explorer);
			auto tileSE = GetAutomapType({ map.x + 1, map.y });
			if (tileSE.type == AutomapTile::Types::Corner && tileSE.HasFlag(AutomapTile::Flags::Dirt))
				UpdateAutomapExplorer({ map.x + 1, map.y }, explorer);
		} else {
			if (HasAutomapFlag({ map.x - 1, map.y }, AutomapTile::Flags::Dirt))
				UpdateAutomapExplorer({ map.x - 1, map.y }, explorer);
			if (HasAutomapFlag({ map.x, map.y - 1 }, AutomapTile::Flags::Dirt))
				UpdateAutomapExplorer({ map.x, map.y - 1 }, explorer);
			if (HasAutomapFlag({ map.x - 1, map.y - 1 }, AutomapTile::Flags::Dirt))
				UpdateAutomapExplorer({ map.x - 1, map.y - 1 }, explorer);
		}
		break;
	case AutomapTile::Types::FenceVertical:
		if (solid) {
			if (HasAutomapFlag({ map.x, map.y - 1 }, AutomapTile::Flags::Dirt))
				UpdateAutomapExplorer({ map.x, map.y - 1 }, explorer);
			auto tileSW = GetAutomapType({ map.x, map.y + 1 });
			if (tileSW.type == AutomapTile::Types::Corner && tileSW.HasFlag(AutomapTile::Flags::Dirt))
				UpdateAutomapExplorer({ map.x, map.y + 1 }, explorer);
		} else if (HasAutomapFlag({ map.x - 1, map.y }, AutomapTile::Flags::Dirt)) {
			UpdateAutomapExplorer({ map.x - 1, map.y }, explorer);
		}
		break;
	case AutomapTile::Types::FenceHorizontal:
		if (solid) {
			if (HasAutomapFlag({ map.x - 1, map.y }, AutomapTile::Flags::Dirt))
				UpdateAutomapExplorer({ map.x - 1, map.y }, explorer);
			auto tileSE = GetAutomapType({ map.x + 1, map.y });
			if (tileSE.type == AutomapTile::Types::Corner && tileSE.HasFlag(AutomapTile::Flags::Dirt))
				UpdateAutomapExplorer({ map.x + 1, map.y }, explorer);
		} else if (HasAutomapFlag({ map.x, map.y - 1 }, AutomapTile::Flags::Dirt)) {
			UpdateAutomapExplorer({ map.x, map.y - 1 }, explorer);
		}
		break;
	default:
		break;
	}
}

void AutomapZoomReset()
{
	AutomapOffset = { 0, 0 };
}

} // namespace devilution

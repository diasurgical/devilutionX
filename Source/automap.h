/**
 * @file automap.h
 *
 * Interface of the in-game map overlay.
 */
#pragma once

#include <cstdint>

#include "engine.h"
#include "engine/displacement.hpp"
#include "engine/point.hpp"
#include "levels/gendung.h"
#include "utils/attributes.h"

namespace devilution {

enum MapExplorationType : uint8_t {
	/** unexplored map tile */
	MAP_EXP_NONE,
	/** map tile explored in vanilla - compatibility reasons */
	MAP_EXP_OLD,
	/** map explored by a shrine */
	MAP_EXP_SHRINE,
	/** map tile explored by someone else in multiplayer */
	MAP_EXP_OTHERS,
	/** map tile explored by current player */
	MAP_EXP_SELF,
};

/** Specifies whether the automap is enabled. */
extern DVL_API_FOR_TEST bool AutomapActive;
/** Tracks the explored areas of the map. */
extern uint8_t AutomapView[DMAXX][DMAXY];
/** Specifies the scale of the automap. */
extern DVL_API_FOR_TEST int AutoMapScale;
extern DVL_API_FOR_TEST Displacement AutomapOffset;
/** Defines the offsets used for Automap lines */
enum class AmWidthOffset : int8_t {
	None,

	EighthTileRight = TILE_WIDTH >> 4,
	QuarterTileRight = TILE_WIDTH >> 3,
	HalfTileRight = TILE_WIDTH >> 2,
	FullTileRight = TILE_WIDTH >> 1,
	DoubleTileRight = TILE_WIDTH,

	ThreeQuartersTileRight = FullTileRight - QuarterTileRight,

	EighthTileLeft = -EighthTileRight,
	QuarterTileLeft = -QuarterTileRight,
	HalfTileLeft = -HalfTileRight,
	FullTileLeft = -FullTileRight,
	DoubleTileLeft = -DoubleTileRight,

	ThreeQuartersTileLeft = -ThreeQuartersTileRight,
};

enum class AmHeightOffset : int8_t {
	None,

	QuarterTileDown = TILE_HEIGHT >> 3,
	HalfTileDown = TILE_HEIGHT >> 2,
	FullTileDown = TILE_HEIGHT >> 1,
	DoubleTileDown = TILE_HEIGHT,

	ThreeQuartersTileDown = FullTileDown - QuarterTileDown,

	QuarterTileUp = -QuarterTileDown,
	HalfTileUp = -HalfTileDown,
	FullTileUp = -FullTileDown,
	DoubleTileUp = -DoubleTileDown,

	ThreeQuartersTileUp = -ThreeQuartersTileDown,
};

enum class AmLineLength : uint8_t {
	QuarterTile = 2,
	ThirdTile = 3,
	HalfTile = 4,
	FullTile = 8,
	FullAndHalfTile = 12,
	DoubleTile = 16,
};

inline Displacement AmOffset(AmWidthOffset x, AmHeightOffset y)
{
	return { AutoMapScale * static_cast<int>(x) / 100, AutoMapScale * static_cast<int>(y) / 100 };
}

inline int AmLine(AmLineLength l)
{
	return AutoMapScale * static_cast<int>(l) / 100;
}

/**
 * @brief Initializes the automap.
 */
void InitAutomapOnce();

/**
 * @brief Loads the mapping between tile IDs and automap shapes.
 */
void InitAutomap();

/**
 * @brief Displays the automap.
 */
void StartAutomap();

/**
 * @brief Scrolls the automap upwards.
 */
void AutomapUp();

/**
 * @brief Scrolls the automap downwards.
 */
void AutomapDown();

/**
 * @brief Scrolls the automap leftwards.
 */
void AutomapLeft();

/**
 * @brief Scrolls the automap rightwards.
 */
void AutomapRight();

/**
 * @brief Increases the zoom level of the automap.
 */
void AutomapZoomIn();

/**
 * @brief Decreases the zoom level of the automap.
 */
void AutomapZoomOut();

/**
 * @brief Renders the automap to the given buffer.
 */
void DrawAutomap(const Surface &out);

/**
 * @brief Updates automap explorer at point if value is higher than existing.
 */
void UpdateAutomapExplorer(Point map, MapExplorationType explorer);

/**
 * @brief Marks the given coordinate as within view on the automap.
 */
void SetAutomapView(Point tile, MapExplorationType explorer);

/**
 * @brief Resets the zoom level of the automap.
 */
void AutomapZoomReset();

} // namespace devilution

/**
 * @file scrollrt.h
 *
 * Interface of functionality for rendering the dungeons, monsters and calling other render routines.
 */
#pragma once

#include <cstdint>

#include "engine.h"
#include "engine/animationinfo.h"
#include "engine/point.hpp"

namespace devilution {

enum class ScrollDirection : uint8_t {
	None,
	North,
	NorthEast,
	East,
	SouthEast,
	South,
	SouthWest,
	West,
	NorthWest,
};

extern int LightTableIndex;
extern uint32_t level_cel_block;
extern char arch_draw_type;
extern bool cel_transparency_active;
extern bool cel_foliage_active;
extern int level_piece_id;
extern bool AutoMapShowItems;
extern bool frameflag;

/**
 * @brief Returns the offset for the walking animation
 * @param animationInfo the current active walking animation
 * @param dir walking direction
 * @param cameraMode Adjusts the offset relative to the camera
 */
Displacement GetOffsetForWalking(const AnimationInfo &animationInfo, const Direction dir, bool cameraMode = false);

/**
 * @brief Clear cursor state
 */
void ClearCursor();

/**
 * @brief Shifting the view area along the logical grid
 *        Note: this won't allow you to shift between even and odd rows
 * @param horizontal Shift the screen left or right
 * @param vertical Shift the screen up or down
 */
void ShiftGrid(int *x, int *y, int horizontal, int vertical);

/**
 * @brief Gets the number of rows covered by the main panel
 */
int RowsCoveredByPanel();

/**
 * @brief Calculate the offset needed for centering tiles in view area
 * @param offsetX Offset in pixels
 * @param offsetY Offset in pixels
 */
void CalcTileOffset(int *offsetX, int *offsetY);

/**
 * @brief Calculate the needed diamond tile to cover the view area
 * @param columns Tiles needed per row
 * @param rows Both even and odd rows
 */
void TilesInView(int *columns, int *rows);
void CalcViewportGeometry();

/**
 * @brief Render the whole screen black
 */
void ClearScreenBuffer();
#ifdef _DEBUG

/**
 * @brief Scroll the screen when mouse is close to the edge
 */
void ScrollView();
#endif

/**
 * @brief Initialize the FPS meter
 */
void EnableFrameCount();

/**
 * @brief Redraw screen
 */
void scrollrt_draw_game_screen();

/**
 * @brief Render the game
 */
void DrawAndBlit();

} // namespace devilution

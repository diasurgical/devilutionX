/**
 * @file scrollrt.h
 *
 * Interface of functionality for rendering the dungeons, monsters and calling other render routines.
 */
#pragma once

#include <cstdint>

#include "engine/animationinfo.h"
#include "engine.h"

namespace devilution {

enum _scroll_direction : uint8_t {
	SDIR_NONE,
	SDIR_N,
	SDIR_NE,
	SDIR_E,
	SDIR_SE,
	SDIR_S,
	SDIR_SW,
	SDIR_W,
	SDIR_NW,
};

// Defined in SourceX/controls/plctrls.cpp
extern bool sgbControllerActive;
extern bool IsMovingMouseCursorWithController();

extern int light_table_index;
extern uint32_t level_cel_block;
extern char arch_draw_type;
extern bool cel_transparency_active;
extern bool cel_foliage_active;
extern int level_piece_id;
extern bool AutoMapShowItems;

/**
 * @brief Returns the offset for the walking animation
 * @param animationInfo the current active walking animation
 * @param dir walking direction
 * @param cameraMode Adjusts the offset relative to the camera
 */
Point GetOffsetForWalking(const AnimationInfo &animationInfo, const Direction dir, bool cameraMode = false);

void ClearCursor();
void ShiftGrid(int *x, int *y, int horizontal, int vertical);
int RowsCoveredByPanel();
void CalcTileOffset(int *offsetX, int *offsetY);
void TilesInView(int *columns, int *rows);
void CalcViewportGeometry();

/**
 * @brief Start rendering of screen, town variation
 * @param out Buffer to render to
 * @param StartX Center of view in dPiece coordinate
 * @param StartY Center of view in dPiece coordinate
 */
void DrawView(const CelOutputBuffer &out, int StartX, int StartY);

void ClearScreenBuffer();
#ifdef _DEBUG
void ScrollView();
#endif
void EnableFrameCount();
void scrollrt_draw_game_screen(bool draw_cursor);
void DrawAndBlit();

} // namespace devilution

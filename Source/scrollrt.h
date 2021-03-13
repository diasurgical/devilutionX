/**
 * @file scrollrt.h
 *
 * Interface of functionality for rendering the dungeons, monsters and calling other render routines.
 */
#ifndef __SCROLLRT_H__
#define __SCROLLRT_H__

DEVILUTION_BEGIN_NAMESPACE

#ifdef __cplusplus
extern "C" {
#endif

// Defined in SourceX/controls/plctrls.cpp
extern bool sgbControllerActive;
extern bool IsMovingMouseCursorWithController();

extern int light_table_index;
extern DWORD level_cel_block;
extern char arch_draw_type;
extern int cel_transparency_active;
extern int cel_foliage_active;
extern int level_piece_id;
extern BOOLEAN AutoMapShowItems;

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
void DrawView(CelOutputBuffer out, int StartX, int StartY);

void ClearScreenBuffer();
#ifdef _DEBUG
void ScrollView();
#endif
void EnableFrameCount();
void scrollrt_draw_game_screen(BOOL draw_cursor);
void DrawAndBlit();

#ifdef __cplusplus
}
#endif

DEVILUTION_END_NAMESPACE

#endif /* __SCROLLRT_H__ */

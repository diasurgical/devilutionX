/**
 * @file text_render.hpp
 *
 * Text rendering.
 */
#pragma once

#include <cstdint>

#include <SDL.h>

#include "DiabloUI/ui_item.h"
#include "engine.h"

namespace devilution {

enum GameFontTables : uint8_t {
	GameFontSmall,
	GameFontMed,
	GameFontBig,
};

enum text_color : uint8_t {
	COL_WHITE,
	COL_BLUE,
	COL_RED,
	COL_GOLD,
	COL_BLACK,
};

extern const uint8_t gbFontTransTbl[256];
extern const uint8_t fontframe[3][128];
extern const uint8_t fontkern[3][68];

extern std::optional<CelSprite> pPanelText;
extern std::optional<CelSprite> pMedTextCels;
extern std::optional<CelSprite> BigTGold_cel;

extern std::optional<CelSprite> pSPentSpn2Cels;

void InitText();
void FreeText();

/**
 * @brief Print letter to the given buffer
 * @param out The buffer to print to
 * @param sx Backbuffer offset
 * @param sy Backbuffer offset
 * @param nCel Number of letter in Windows-1252
 * @param col text_color color value
 */
void PrintChar(const CelOutputBuffer &out, int sx, int sy, int nCel, text_color col);

/**
 * @brief Calculate pixel width of first line of text, respecting kerning
 * @param text Text to check, will read until first eol or terminator
 * @param size Font size to use
 * @param spacing Extra spacing to add per character
 * @param lineLength Receives characters read until newline or terminator
 * @return Line width in pixels
*/
int GetLineWidth(const char *text, GameFontTables size = GameFontSmall, int spacing = 1, int* lineLength = nullptr);
void WordWrapGameString(char *text, size_t width, size_t size = GameFontSmall, int spacing = 1);
void DrawString(const CelOutputBuffer &out, const char *text, const SDL_Rect &rect, uint16_t flags = 0, int spacing = 1, bool drawTextCursor = false);
int PentSpn2Spin();

} // namespace devilution

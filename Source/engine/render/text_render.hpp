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
#include "engine/cel_sprite.hpp"
#include "engine/rectangle.hpp"
#include "utils/stdcompat/optional.hpp"

namespace devilution {

enum GameFontTables : uint8_t {
	GameFontSmall,
	GameFontMed,
	GameFontBig,
};

extern std::optional<CelSprite> pSPentSpn2Cels;

void InitText();

/**
 * @brief Calculate pixel width of first line of text, respecting kerning
 * @param text Text to check, will read until first eol or terminator
 * @param size Font size to use
 * @param spacing Extra spacing to add per character
 * @param charactersInLine Receives characters read until newline or terminator
 * @return Line width in pixels
 */
int GetLineWidth(const char *text, GameFontTables size = GameFontSmall, int spacing = 1, int *charactersInLine = nullptr);
void WordWrapGameString(char *text, size_t width, GameFontTables size = GameFontSmall, int spacing = 1);

/**
 * @brief Draws a line of text within a clipping rectangle (positioned relative to the origin of the output buffer).
 *
 * Specifying a small width (0 to less than two characters wide) should be avoided as this causes issues when laying
 * out the text. To wrap based on available space use the overload taking a Point. If the rect passed through has 0
 * height then the clipping area is extended to the bottom edge of the output buffer. If the clipping rectangle
 * dimensions extend past the edge of the output buffer text wrapping will be calculated using those dimensions (as if
 * the text was being rendered off screen). The text will not actually be drawn beyond the bounds of the output
 * buffer, this is purely to allow for clipping without wrapping.
 *
 * @param out The screen buffer to draw on.
 * @param text String to be drawn.
 * @param rect Clipping region relative to the output buffer describing where to draw the text and when to wrap long lines.
 * @param flags A combination of UiFlags to describe font size, color, alignment, etc. See ui_items.h for available options
 * @param spacing Additional space to add between characters.
 *                This value may be adjusted if the flag UIS_FIT_SPACING is passed in the flags parameter.
 * @param lineHeight Allows overriding the default line height, useful for multi-line strings.
 * @param drawTextCursor Whether to draw an animated cursor sprite at the end of the text (default is to display nothing).
 * @return The number of characters rendered, including characters "drawn" outside the buffer.
 */
uint16_t DrawString(const Surface &out, const char *text, const Rectangle &rect, uint16_t flags = 0, int spacing = 1, int lineHeight = -1, bool drawTextCursor = false);

/**
 * @brief Draws a line of text at the given position relative to the origin of the output buffer.
 *
 * This method is provided as a convenience to pass through to DrawString(..., Rectangle, ...) when no explicit

 * clipping/wrapping is requested. Note that this will still wrap the rendered string if it would end up being drawn
 * beyond the right edge of the output buffer and clip it if it would extend beyond the bottom edge of the buffer.
 *
 * @param out The screen buffer to draw on.
 * @param text String to be drawn.
 * @param position Location of the top left corner of the string relative to the top left corner of the output buffer.
 * @param flags A combination of UiFlags to describe font size, color, alignment, etc. See ui_items.h for available options
 * @param spacing Additional space to add between characters.
 *                This value may be adjusted if the flag UIS_FIT_SPACING is passed in the flags parameter.
 * @param lineHeight Allows overriding the default line height, useful for multi-line strings.
 * @param drawTextCursor Whether to draw an animated cursor sprite at the end of the text (default is to display nothing).
 * @return The number of characters rendered (could be less than the string length if it wrapped past the bottom of the buffer).
 */
inline uint16_t DrawString(const Surface &out, const char *text, const Point &position, uint16_t flags = 0, int spacing = 1, int lineHeight = -1, bool drawTextCursor = false)
{
	return DrawString(out, text, { position, { out.w() - position.x, 0 } }, flags, spacing, lineHeight, drawTextCursor);
}

uint8_t PentSpn2Spin();

} // namespace devilution

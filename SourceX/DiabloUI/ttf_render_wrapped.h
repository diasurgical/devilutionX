#pragma once

#include "all.h"

#include <SDL_ttf.h>

namespace dvl {

enum class TextAlignment {
	BEGIN = 0,
	CENTER,
	END,
};

/**
 * Renders UTF-8, wrapping lines to avoid exceeding wrapLength, and aligning
 * according to the `x_align` argument.
 *
 * This method is slow. Caching the result is recommended.
 */
SDL_Surface *RenderUTF8_Solid_Wrapped(
    TTF_Font *font, const char *text, SDL_Color fg, Uint32 wrapLength, TextAlignment x_align = TextAlignment::BEGIN);

} // namespace dvl

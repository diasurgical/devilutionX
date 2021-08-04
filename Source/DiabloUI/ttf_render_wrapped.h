#pragma once

#include <SDL_ttf.h>
#include <cstdint>
#include "utils/sdl_ptrs.h"

namespace devilution {

enum TextAlignment : uint8_t {
	TextAlignment_BEGIN,
	TextAlignment_CENTER,
	TextAlignment_END,
};

/**
 * Renders UTF-8, wrapping lines to avoid exceeding wrapLength, and aligning
 * according to the `x_align` argument.
 *
 * This method is slow. Caching the result is recommended.
 */
SDLSurfaceUniquePtr RenderUTF8_Solid_Wrapped(
    TTF_Font *font, const char *text, SDL_Color fg, Uint32 wrapLength, const int xAlign = TextAlignment_BEGIN);

} // namespace devilution

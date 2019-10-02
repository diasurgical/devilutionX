#pragma once

#include "devilution.h"

#include <SDL_ttf.h>

namespace dvl {

enum class TextAlignment {
	BEGIN = 0,
	CENTER,
	END,
};

// `TTF_RenderUTF8_Solid_Wrapped` is only available in SDL2 2.0.16 (unreleased
// as of this writing). This is a hacky alternative.
SDL_Surface *RenderUTF8_Solid_Wrapped(
    TTF_Font *font, const char *text, SDL_Color fg, Uint32 wrapLength, TextAlignment x_align = TextAlignment::BEGIN);

} // namespace dvl

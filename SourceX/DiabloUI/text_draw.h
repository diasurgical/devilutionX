#pragma once

#include <SDL.h>

#include "sdl_ptrs.h"

namespace devilution {

struct TtfSurfaceCache {
	SDLSurfaceUniquePtr text;
	SDLSurfaceUniquePtr shadow;
};

void DrawTTF(const char *text, const SDL_Rect &rect, int flags,
    const SDL_Color &text_color, const SDL_Color &shadow_color,
    TtfSurfaceCache **surface_cache);

void DrawArtStr(const char *text, const SDL_Rect &rect, int flags, bool drawTextCursor = false);

} // namespace devilution

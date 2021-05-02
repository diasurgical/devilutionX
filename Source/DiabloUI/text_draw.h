#pragma once

#include <SDL.h>

#include "utils/sdl_ptrs.h"

namespace devilution {

struct TtfSurfaceCache {
	SDLUniquePtr<SDL_Surface> text;
	SDLUniquePtr<SDL_Surface> shadow;
};

void DrawTTF(const char *text, const SDL_Rect &rect, int flags,
    const SDL_Color &text_color, const SDL_Color &shadow_color,
    TtfSurfaceCache &render_cache);

void DrawArtStr(const char *text, const SDL_Rect &rect, int flags, bool drawTextCursor = false);

} // namespace devilution

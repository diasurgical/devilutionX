#pragma once

#include <SDL.h>

#include "utils/sdl_ptrs.h"

namespace devilution {

struct TtfSurfaceCache {
	SDLSurfaceUniquePtr text;
	SDLSurfaceUniquePtr shadow;
};

void DrawTTF(const char *text, const SDL_Rect &rect, int flags,
    const SDL_Color &textColor, const SDL_Color &shadowColor,
    TtfSurfaceCache &renderCache);

void DrawArtStr(const char *text, const SDL_Rect &rect, int flags, bool drawTextCursor = false);

} // namespace devilution

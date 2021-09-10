#pragma once

#include <SDL.h>

#include "utils/sdl_ptrs.h"

namespace devilution {

struct TtfSurfaceCache {
	SDLSurfaceUniquePtr text;
	SDLSurfaceUniquePtr shadow;
};

enum class UiFlags; // Defined in ui_item.h, declared here to avoid circular dependency

void DrawTTF(const char *text, const SDL_Rect &rect, UiFlags flags,
    const SDL_Color &textColor, const SDL_Color &shadowColor,
    TtfSurfaceCache &renderCache);

} // namespace devilution

#pragma once

#include "all.h"

namespace dvl {

struct TtfSurfaceCache {

	TtfSurfaceCache()
	{
		text = NULL;
		shadow = NULL;
	}

	~TtfSurfaceCache()
	{
		mem_free_dbg(text);
		mem_free_dbg(shadow);
	}

	SDL_Surface *text;
	SDL_Surface *shadow;
};

void DrawTTF(const char *text, const SDL_Rect &rect, int flags,
    const SDL_Color &text_color, const SDL_Color &shadow_color,
    TtfSurfaceCache **surface_cache);

void DrawArtStr(const char *text, const SDL_Rect &rect, int flags, bool drawTextCursor = false);

} // namespace dvl

#pragma once

#include <SDL_ttf.h>
#include <SDL.h>

#include "appfat.h"
#include "utils/sdl_ptrs.h"

namespace devilution {

namespace TTFWrap {

inline SDLSurfaceUniquePtr RenderText_Solid(TTF_Font *font, const char *text, SDL_Color fg)
{
	SDLSurfaceUniquePtr ret { TTF_RenderText_Solid(font, text, fg) };
	if (ret == nullptr)
		ErrTtf();

	return ret;
}

} //namespace TTFWrap

} //namespace devilution

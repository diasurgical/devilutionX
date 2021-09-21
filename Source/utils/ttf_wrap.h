#pragma once

#include <SDL_ttf.h>
#include <SDL.h>
#ifdef USE_SDL1
#include "utils/sdl2_to_1_2_backports.h"
#else
#include "utils/sdl2_backports.h"
#endif

#include "appfat.h"
#include "utils/sdl_ptrs.h"

namespace devilution {

namespace TTFWrap {

inline SDLSurfaceUniquePtr RenderUTF8_Solid(TTF_Font *font, const char *text, SDL_Color fg)
{
	SDLSurfaceUniquePtr ret { TTF_RenderUTF8_Solid(font, text, fg) };
	if (ret == nullptr)
		ErrTtf();

	return ret;
}

} //namespace TTFWrap

} //namespace devilution

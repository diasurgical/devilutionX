#pragma once

#include <SDL.h>
#ifdef USE_SDL1
#include "utils/sdl2_to_1_2_backports.h"
#else
#include "utils/sdl2_backports.h"
#endif

#include "appfat.h"
#include "utils/sdl_ptrs.h"

namespace devilution {

namespace SDLWrap {

inline SDLSurfaceUniquePtr CreateRGBSurface(Uint32 flags, int width, int height, int depth, Uint32 Rmask, Uint32 Gmask, Uint32 Bmask, Uint32 Amask)
{
	SDLSurfaceUniquePtr ret { SDL_CreateRGBSurface(flags, width, height, depth, Rmask, Gmask, Bmask, Amask) };
	if (ret == nullptr)
		ErrSdl();

	return ret;
}

inline SDLSurfaceUniquePtr CreateRGBSurfaceWithFormat(Uint32 flags, int width, int height, int depth, Uint32 format)
{
	SDLSurfaceUniquePtr ret { SDL_CreateRGBSurfaceWithFormat(flags, width, height, depth, format) };
	if (ret == nullptr)
		ErrSdl();

	return ret;
}

} //namespace SDLWrap

} //namespace devilution

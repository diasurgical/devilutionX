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

inline SDLSurfaceUniquePtr CreateRGBSurfaceWithFormatFrom(void *pixels, int width, int height, int depth, int pitch, Uint32 format)
{
	SDLSurfaceUniquePtr ret { SDL_CreateRGBSurfaceWithFormatFrom(pixels, width, height, depth, pitch, format) };
	if (ret == nullptr)
		ErrSdl();

	return ret;
}

#ifndef USE_SDL1
inline SDLSurfaceUniquePtr ConvertSurface(SDL_Surface *src, const SDL_PixelFormat *fmt, Uint32 flags)
#else
inline SDLSurfaceUniquePtr ConvertSurface(SDL_Surface *src, SDL_PixelFormat *fmt, Uint32 flags)
#endif
{
	SDLSurfaceUniquePtr ret { SDL_ConvertSurface(src, fmt, flags) };
	if (ret == nullptr)
		ErrSdl();

	return ret;
}

#ifndef USE_SDL1
inline SDLSurfaceUniquePtr ConvertSurfaceFormat(SDL_Surface *src, Uint32 pixel_format, Uint32 flags)
{
	SDLSurfaceUniquePtr ret { SDL_ConvertSurfaceFormat(src, pixel_format, flags) };
	if (ret == nullptr)
		ErrSdl();

	return ret;
}
#endif

#ifndef USE_SDL1
inline SDLTextureUniquePtr CreateTexture(SDL_Renderer *renderer, Uint32 format, int access, int w, int h)
{
	SDLTextureUniquePtr ret { SDL_CreateTexture(renderer, format, access, w, h) };
	if (ret == nullptr)
		ErrSdl();

	return ret;
}
#endif

} //namespace SDLWrap

} //namespace devilution

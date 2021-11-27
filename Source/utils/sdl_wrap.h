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

template <typename T>
T NonNull(T x)
{
	if (x == nullptr)
		ErrSdl();
	return x;
}

inline SDLSurfaceUniquePtr CreateRGBSurface(Uint32 flags, int width, int height, int depth, Uint32 Rmask, Uint32 Gmask, Uint32 Bmask, Uint32 Amask)
{
	return SDLSurfaceUniquePtr { NonNull(SDL_CreateRGBSurface(flags, width, height, depth, Rmask, Gmask, Bmask, Amask)) };
}

inline SDLSurfaceUniquePtr CreateRGBSurfaceWithFormat(Uint32 flags, int width, int height, int depth, Uint32 format)
{
	return SDLSurfaceUniquePtr { NonNull(SDL_CreateRGBSurfaceWithFormat(flags, width, height, depth, format)) };
}

inline SDLSurfaceUniquePtr CreateRGBSurfaceWithFormatFrom(void *pixels, int width, int height, int depth, int pitch, Uint32 format)
{
	return SDLSurfaceUniquePtr { NonNull(SDL_CreateRGBSurfaceWithFormatFrom(pixels, width, height, depth, pitch, format)) };
}

#ifndef USE_SDL1
inline SDLSurfaceUniquePtr ConvertSurface(SDL_Surface *src, const SDL_PixelFormat *fmt, Uint32 flags)
#else
inline SDLSurfaceUniquePtr ConvertSurface(SDL_Surface *src, SDL_PixelFormat *fmt, Uint32 flags)
#endif
{
	return SDLSurfaceUniquePtr { NonNull(SDL_ConvertSurface(src, fmt, flags)) };
}

#ifndef USE_SDL1
inline SDLSurfaceUniquePtr ConvertSurfaceFormat(SDL_Surface *src, Uint32 pixel_format, Uint32 flags)
{
	return SDLSurfaceUniquePtr { NonNull(SDL_ConvertSurfaceFormat(src, pixel_format, flags)) };
}
#endif

#ifndef USE_SDL1
inline SDLTextureUniquePtr CreateTexture(SDL_Renderer *renderer, Uint32 format, int access, int w, int h)
{
	return SDLTextureUniquePtr { NonNull(SDL_CreateTexture(renderer, format, access, w, h)) };
}
#endif

inline SDLPaletteUniquePtr AllocPalette(int ncolors = 256)
{
	return SDLPaletteUniquePtr { NonNull(SDL_AllocPalette(ncolors)) };
}

} // namespace SDLWrap

} // namespace devilution

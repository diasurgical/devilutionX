#pragma once

#include <SDL.h>
#include <errno.h>
#include <stdio.h>
#include <math.h>
#ifndef _XBOX
#include <unistd.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <cstddef>
#endif

#include "../SourceX/stubs.h"

#define WINDOW_ICON_NAME 0

//== Utility

#define SDL_zero(x) SDL_memset(&(x), 0, sizeof((x)))
#define SDL_InvalidParamError(param) SDL_SetError("Parameter '%s' is invalid", (param))
#define SDL_floor floor

//== Events handling

#define SDL_threadID Uint32

#define SDL_Keysym SDL_keysym
#define SDL_Keycode SDLKey

#define SDLK_PRINTSCREEN SDLK_PRINT
#define SDLK_SCROLLLOCK SDLK_SCROLLOCK
#define SDLK_NUMLOCKCLEAR SDLK_NUMLOCK
#define SDLK_KP_1 SDLK_KP1
#define SDLK_KP_2 SDLK_KP2
#define SDLK_KP_3 SDLK_KP3
#define SDLK_KP_4 SDLK_KP4
#define SDLK_KP_5 SDLK_KP5
#define SDLK_KP_6 SDLK_KP6
#define SDLK_KP_7 SDLK_KP7
#define SDLK_KP_8 SDLK_KP8
#define SDLK_KP_9 SDLK_KP9
#define SDLK_KP_0 SDLK_KP0
#define SDLK_LGUI SDLK_LSUPER
#define SDLK_RGUI SDLK_RSUPER

// Haptic events are not supported in SDL1.
#define SDL_INIT_HAPTIC 0

// For now we only process ASCII input when using SDL1.
#define SDL_TEXTINPUTEVENT_TEXT_SIZE 2

#define SDL_JoystickNameForIndex SDL_JoystickName

inline void SDL_Log(const char *fmt, ...)
{
	va_list ap;
	va_start(ap, fmt);
	vprintf(fmt, ap);
	va_end(ap);
	puts("");
}

static SDL_bool SDLBackport_IsTextInputActive = SDL_FALSE;

inline SDL_bool SDL_IsTextInputActive()
{
	return SDLBackport_IsTextInputActive;
}

inline void SDL_StartTextInput()
{
	SDLBackport_IsTextInputActive = SDL_TRUE;
}
inline void SDL_StopTextInput()
{
	SDLBackport_IsTextInputActive = SDL_FALSE;
}

//== Graphics helpers

typedef struct SDL_Point {
	int x;
	int y;
} SDL_Point;

inline SDL_bool SDL_PointInRect(const SDL_Point *p, const SDL_Rect *r)
{
	return ((p->x >= r->x) && (p->x < (r->x + r->w)) && (p->y >= r->y) && (p->y < (r->y + r->h))) ? SDL_TRUE : SDL_FALSE;
}

inline void SDL_DisableScreenSaver()
{
	DUMMY();
}

//= Window handling

#define SDL_Window SDL_Surface


inline void SDL_ShowWindow(SDL_Window *window)
{
	DUMMY();
}

inline void SDL_HideWindow(SDL_Window *window)
{
	DUMMY();
}

inline void SDL_RaiseWindow(SDL_Window *window)
{
	DUMMY();
}

inline void SDL_DestroyWindow(SDL_Window *window)
{
	SDL_FreeSurface(window);
}

inline void
SDL_WarpMouseInWindow(SDL_Window *window, int x, int y)
{
	SDL_WarpMouse(x, y);
}

//= Renderer stubs

#define SDL_Renderer void

inline void SDL_DestroyRenderer(SDL_Renderer *renderer)
{
	if (renderer != NULL)
		UNIMPLEMENTED();
}

//= Texture stubs

#define SDL_Texture void

inline void SDL_DestroyTexture(SDL_Texture *texture)
{
	if (texture != NULL)
		UNIMPLEMENTED();
}

//= Palette handling

inline SDL_Palette *
SDL_AllocPalette(int ncolors)
{
	SDL_Palette *palette;

	/* Input validation */
	if (ncolors < 1) {
		SDL_InvalidParamError("ncolors");
		return NULL;
	}

	palette = (SDL_Palette *)SDL_malloc(sizeof(*palette));
	if (!palette) {
		SDL_OutOfMemory();
		return NULL;
	}
	palette->colors = (SDL_Color *)SDL_malloc(ncolors * sizeof(*palette->colors));
	if (!palette->colors) {
		SDL_free(palette);
		return NULL;
	}
	palette->ncolors = ncolors;
	SDL_memset(palette->colors, 0xFF, ncolors * sizeof(*palette->colors));
	return palette;
}

inline void
SDL_FreePalette(SDL_Palette *palette)
{
	if (!palette) {
		SDL_InvalidParamError("palette");
		return;
	}
	SDL_free(palette->colors);
	SDL_free(palette);
}

inline bool SDL_HasColorKey(SDL_Surface *surface)
{
	return (surface->flags & SDL_SRCCOLORKEY) != 0;
}

//= Pixel formats

#define SDL_PIXELFORMAT_INDEX8 1
#define SDL_PIXELFORMAT_RGB888 2
#define SDL_PIXELFORMAT_RGBA8888 3

inline void SDLBackport_PixelformatToMask(int pixelformat, Uint32 *flags, Uint32 *rmask,
    Uint32 *gmask,
    Uint32 *bmask,
    Uint32 *amask)
{
	if (pixelformat == SDL_PIXELFORMAT_RGBA8888) {
#if SDL_BYTEORDER == SDL_BIG_ENDIAN
		*rmask = 0xff000000;
		*gmask = 0x00ff0000;
		*bmask = 0x0000ff00;
		*amask = 0x000000ff;
#else
		*rmask = 0x000000ff;
		*gmask = 0x0000ff00;
		*bmask = 0x00ff0000;
		*amask = 0xff000000;
#endif
	} else if (pixelformat == SDL_PIXELFORMAT_RGB888) {
#if SDL_BYTEORDER == SDL_BIG_ENDIAN
		*rmask = 0xff000000;
		*gmask = 0x00ff0000;
		*bmask = 0x0000ff00;
#else
		*rmask = 0x000000ff;
		*gmask = 0x0000ff00;
		*bmask = 0x00ff0000;
#endif
		*amask = 0;
	} else {
		*rmask = *gmask = *bmask = *amask = 0;
	}
}

/**
 * A limited implementation of `a.format` == `b.format` from SDL2.
 */
inline bool SDLBackport_PixelFormatFormatEq(const SDL_PixelFormat *a, const SDL_PixelFormat *b)
{
	return a->BitsPerPixel == b->BitsPerPixel && (a->palette != NULL) == (b->palette != NULL)
	    && a->Rmask == b->Rmask && a->Gmask == b->Gmask && a->Bmask == b->Bmask;
}

/**
 * Similar to `SDL_ISPIXELFORMAT_INDEXED` from SDL2.
 */
inline bool SDLBackport_IsPixelFormatIndexed(const SDL_PixelFormat *pf)
{
	return pf->BitsPerPixel == 8 && pf->palette != NULL;
}

//= Surface creation

inline SDL_Surface *
SDL_CreateRGBSurfaceWithFormat(Uint32 flags, int width, int height, int depth,
    Uint32 format)
{
	Uint32 rmask, gmask, bmask, amask;
	SDLBackport_PixelformatToMask(format, &flags, &rmask, &gmask, &bmask, &amask);
	return SDL_CreateRGBSurface(flags, width, height, depth, rmask, gmask, bmask, amask);
}

inline SDL_Surface *
SDL_CreateRGBSurfaceWithFormatFrom(void *pixels, Uint32 flags, int width, int height, int depth,
    Uint32 format)
{
	Uint32 rmask, gmask, bmask, amask;
	SDLBackport_PixelformatToMask(format, &flags, &rmask, &gmask, &bmask, &amask);
	return SDL_CreateRGBSurfaceFrom(pixels, flags, width, height, depth, rmask, gmask, bmask, amask);
}

//= BlitScaled backport from SDL 2.0.9.

#define SDL_BlitScaled SDL_UpperBlitScaled


inline int
SDL_LowerBlitScaled(SDL_Surface *src, SDL_Rect *srcrect,
    SDL_Surface *dst, SDL_Rect *dstrect)
{
	if (SDLBackport_PixelFormatFormatEq(src->format, dst->format) && !SDLBackport_IsPixelFormatIndexed(src->format)) {
		return SDL_SoftStretch(src, srcrect, dst, dstrect);
	} else {
		return SDL_LowerBlit(src, srcrect, dst, dstrect);
	}
}

// NOTE: The second argument is const in SDL2 but not here.
inline int
SDL_UpperBlitScaled(SDL_Surface *src, SDL_Rect *srcrect,
    SDL_Surface *dst, SDL_Rect *dstrect)
{
	double src_x0, src_y0, src_x1, src_y1;
	double dst_x0, dst_y0, dst_x1, dst_y1;
	SDL_Rect final_src, final_dst;
	double scaling_w, scaling_h;
	int src_w, src_h;
	int dst_w, dst_h;

	/* Make sure the surfaces aren't locked */
	if (!src || !dst) {
		SDL_SetError("SDL_UpperBlitScaled: passed a NULL surface");
		return -1;
	}
	if (src->locked || dst->locked) {
		SDL_SetError("Surfaces must not be locked during blit");
		return -1;
	}

	if (NULL == srcrect) {
		src_w = src->w;
		src_h = src->h;
	} else {
		src_w = srcrect->w;
		src_h = srcrect->h;
	}

	if (NULL == dstrect) {
		dst_w = dst->w;
		dst_h = dst->h;
	} else {
		dst_w = dstrect->w;
		dst_h = dstrect->h;
	}

	if (dst_w == src_w && dst_h == src_h) {
		/* No scaling, defer to regular blit */
		return SDL_BlitSurface(src, srcrect, dst, dstrect);
	}

	scaling_w = (double)dst_w / src_w;
	scaling_h = (double)dst_h / src_h;

	if (NULL == dstrect) {
		dst_x0 = 0;
		dst_y0 = 0;
		dst_x1 = dst_w - 1;
		dst_y1 = dst_h - 1;
	} else {
		dst_x0 = dstrect->x;
		dst_y0 = dstrect->y;
		dst_x1 = dst_x0 + dst_w - 1;
		dst_y1 = dst_y0 + dst_h - 1;
	}

	if (NULL == srcrect) {
		src_x0 = 0;
		src_y0 = 0;
		src_x1 = src_w - 1;
		src_y1 = src_h - 1;
	} else {
		src_x0 = srcrect->x;
		src_y0 = srcrect->y;
		src_x1 = src_x0 + src_w - 1;
		src_y1 = src_y0 + src_h - 1;

		/* Clip source rectangle to the source surface */

		if (src_x0 < 0) {
			dst_x0 -= src_x0 * scaling_w;
			src_x0 = 0;
		}

		if (src_x1 >= src->w) {
			dst_x1 -= (src_x1 - src->w + 1) * scaling_w;
			src_x1 = src->w - 1;
		}

		if (src_y0 < 0) {
			dst_y0 -= src_y0 * scaling_h;
			src_y0 = 0;
		}

		if (src_y1 >= src->h) {
			dst_y1 -= (src_y1 - src->h + 1) * scaling_h;
			src_y1 = src->h - 1;
		}
	}

	/* Clip destination rectangle to the clip rectangle */

	/* Translate to clip space for easier calculations */
	dst_x0 -= dst->clip_rect.x;
	dst_x1 -= dst->clip_rect.x;
	dst_y0 -= dst->clip_rect.y;
	dst_y1 -= dst->clip_rect.y;

	if (dst_x0 < 0) {
		src_x0 -= dst_x0 / scaling_w;
		dst_x0 = 0;
	}

	if (dst_x1 >= dst->clip_rect.w) {
		src_x1 -= (dst_x1 - dst->clip_rect.w + 1) / scaling_w;
		dst_x1 = dst->clip_rect.w - 1;
	}

	if (dst_y0 < 0) {
		src_y0 -= dst_y0 / scaling_h;
		dst_y0 = 0;
	}

	if (dst_y1 >= dst->clip_rect.h) {
		src_y1 -= (dst_y1 - dst->clip_rect.h + 1) / scaling_h;
		dst_y1 = dst->clip_rect.h - 1;
	}

	/* Translate back to surface coordinates */
	dst_x0 += dst->clip_rect.x;
	dst_x1 += dst->clip_rect.x;
	dst_y0 += dst->clip_rect.y;
	dst_y1 += dst->clip_rect.y;

	final_src.x = (Sint16)SDL_floor(src_x0 + 0.5);
	final_src.y = (Sint16)SDL_floor(src_y0 + 0.5);
	src_w = (int)SDL_floor(src_x1 + 1 + 0.5) - (int)SDL_floor(src_x0 + 0.5);
	src_h = (int)SDL_floor(src_y1 + 1 + 0.5) - (int)SDL_floor(src_y0 + 0.5);
	if (src_w < 0)
		src_w = 0;
	if (src_h < 0)
		src_h = 0;

	final_src.w = static_cast<Uint16>(src_w);
	final_src.h = static_cast<Uint16>(src_h);

	final_dst.x = (Sint16)SDL_floor(dst_x0 + 0.5);
	final_dst.y = (Sint16)SDL_floor(dst_y0 + 0.5);
	dst_w = (int)SDL_floor(dst_x1 - dst_x0 + 1.5);
	dst_h = (int)SDL_floor(dst_y1 - dst_y0 + 1.5);
	if (dst_w < 0)
		dst_w = 0;
	if (dst_h < 0)
		dst_h = 0;

	final_dst.w = static_cast<Uint16>(dst_w);
	final_dst.h = static_cast<Uint16>(dst_h);

	if (dstrect)
		*dstrect = final_dst;

	if (final_dst.w == 0 || final_dst.h == 0 || final_src.w == 0 || final_src.h == 0) {
		/* No-op. */
		return 0;
	}

	return SDL_LowerBlitScaled(src, &final_src, dst, &final_dst);
}

//= Display handling

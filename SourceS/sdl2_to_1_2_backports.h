#pragma once

#include <SDL.h>
#include <unistd.h>
#include <errno.h>
#include <stdio.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <math.h>
#include <cstddef>

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

//= Messagebox (simply logged to stderr for now)

typedef enum {
	SDL_MESSAGEBOX_ERROR = 0x00000010,      /**< error dialog */
	SDL_MESSAGEBOX_WARNING = 0x00000020,    /**< warning dialog */
	SDL_MESSAGEBOX_INFORMATION = 0x00000040 /**< informational dialog */
} SDL_MessageBoxFlags;

inline int SDL_ShowSimpleMessageBox(Uint32 flags,
    const char *title,
    const char *message,
    SDL_Surface *window)
{
	SDL_Log("MSGBOX: %s\n%s", title, message);
	return 0;
}

//= Window handling

#define SDL_Window SDL_Surface

inline void SDL_GetWindowPosition(SDL_Window *window, int *x, int *y)
{
	*x = window->clip_rect.x;
	*y = window->clip_rect.x;
	printf("SDL_GetWindowPosition %d %d", *x, *y);
}

inline void SDL_SetWindowPosition(SDL_Window *window, int x, int y)
{
	DUMMY();
}

inline void SDL_GetWindowSize(SDL_Window *window, int *w, int *h)
{
	*w = window->clip_rect.w;
	*h = window->clip_rect.h;
	printf("SDL_GetWindowSize %d %d", *w, *h);
}

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

#define DEFINE_COPY_ROW(name, type)                              \
	static void name(type *src, int src_w, type *dst, int dst_w) \
	{                                                            \
		int i;                                                   \
		int pos, inc;                                            \
		type pixel = 0;                                          \
                                                                 \
		pos = 0x10000;                                           \
		inc = (src_w << 16) / dst_w;                             \
		for (i = dst_w; i > 0; --i) {                            \
			while (pos >= 0x10000L) {                            \
				pixel = *src++;                                  \
				pos -= 0x10000L;                                 \
			}                                                    \
			*dst++ = pixel;                                      \
			pos += inc;                                          \
		}                                                        \
	}
DEFINE_COPY_ROW(copy_row1, Uint8)
DEFINE_COPY_ROW(copy_row2, Uint16)
DEFINE_COPY_ROW(copy_row4, Uint32)

static void
copy_row3(Uint8 *src, int src_w, Uint8 *dst, int dst_w)
{
	int i;
	int pos, inc;
	Uint8 pixel[3] = { 0, 0, 0 };

	pos = 0x10000;
	inc = (src_w << 16) / dst_w;
	for (i = dst_w; i > 0; --i) {
		while (pos >= 0x10000L) {
			pixel[0] = *src++;
			pixel[1] = *src++;
			pixel[2] = *src++;
			pos -= 0x10000L;
		}
		*dst++ = pixel[0];
		*dst++ = pixel[1];
		*dst++ = pixel[2];
		pos += inc;
	}
}

// NOTE: Not thread-safe
inline int
SDL_SoftStretch(SDL_Surface *src, const SDL_Rect *srcrect,
    SDL_Surface *dst, const SDL_Rect *dstrect)
{
	// All the ASM support has been removed, as the platforms that the ASM
	// implementation exists for support SDL2 anyway.
	int src_locked;
	int dst_locked;
	int pos, inc;
	int dst_maxrow;
	int src_row, dst_row;
	Uint8 *srcp = NULL;
	Uint8 *dstp;
	SDL_Rect full_src;
	SDL_Rect full_dst;
	const int bpp = dst->format->BytesPerPixel;

	if (!SDLBackport_PixelFormatFormatEq(src->format, dst->format)) {
		SDL_SetError("Only works with same format surfaces");
		return -1;
	}

	/* Verify the blit rectangles */
	if (srcrect) {
		if ((srcrect->x < 0) || (srcrect->y < 0) || ((srcrect->x + srcrect->w) > src->w) || ((srcrect->y + srcrect->h) > src->h)) {
			SDL_SetError("Invalid source blit rectangle");
			return -1;
		}
	} else {
		full_src.x = 0;
		full_src.y = 0;
		full_src.w = src->w;
		full_src.h = src->h;
		srcrect = &full_src;
	}
	if (dstrect) {
		if ((dstrect->x < 0) || (dstrect->y < 0) || ((dstrect->x + dstrect->w) > dst->w) || ((dstrect->y + dstrect->h) > dst->h)) {
			SDL_SetError("Invalid destination blit rectangle");
			return -1;
		}
	} else {
		full_dst.x = 0;
		full_dst.y = 0;
		full_dst.w = dst->w;
		full_dst.h = dst->h;
		dstrect = &full_dst;
	}

	/* Lock the destination if it's in hardware */
	dst_locked = 0;
	if (SDL_MUSTLOCK(dst)) {
		if (SDL_LockSurface(dst) < 0) {
			SDL_SetError("Unable to lock destination surface");
			return -1;
		}
		dst_locked = 1;
	}
	/* Lock the source if it's in hardware */
	src_locked = 0;
	if (SDL_MUSTLOCK(src)) {
		if (SDL_LockSurface(src) < 0) {
			if (dst_locked) {
				SDL_UnlockSurface(dst);
			}
			SDL_SetError("Unable to lock source surface");
			return -1;
		}
		src_locked = 1;
	}

	/* Set up the data... */
	pos = 0x10000;
	inc = (srcrect->h << 16) / dstrect->h;
	src_row = srcrect->y;
	dst_row = dstrect->y;

	/* Perform the stretch blit */
	for (dst_maxrow = dst_row + dstrect->h; dst_row < dst_maxrow; ++dst_row) {
		dstp = (Uint8 *)dst->pixels + (dst_row * dst->pitch)
		    + (dstrect->x * bpp);
		while (pos >= 0x10000L) {
			srcp = (Uint8 *)src->pixels + (src_row * src->pitch)
			    + (srcrect->x * bpp);
			++src_row;
			pos -= 0x10000L;
		}
		switch (bpp) {
		case 1:
			copy_row1(srcp, srcrect->w, dstp, dstrect->w);
			break;
		case 2:
			copy_row2((Uint16 *)srcp, srcrect->w,
			    (Uint16 *)dstp, dstrect->w);
			break;
		case 3:
			copy_row3(srcp, srcrect->w, dstp, dstrect->w);
			break;
		case 4:
			copy_row4((Uint32 *)srcp, srcrect->w,
			    (Uint32 *)dstp, dstrect->w);
			break;
		}
		pos += inc;
	}

	/* We need to unlock the surfaces if they're locked */
	if (dst_locked) {
		SDL_UnlockSurface(dst);
	}
	if (src_locked) {
		SDL_UnlockSurface(src);
	}
	return (0);
}

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

typedef struct
{
	Uint32 format;    /**< pixel format */
	int w;            /**< width, in screen coordinates */
	int h;            /**< height, in screen coordinates */
	int refresh_rate; /**< refresh rate (or zero for unspecified) */
	void *driverdata; /**< driver-specific data, initialize to 0 */
} SDL_DisplayMode;

inline int SDL_GetCurrentDisplayMode(int displayIndex, SDL_DisplayMode *mode)
{
	if (displayIndex != 0)
		UNIMPLEMENTED();

	const SDL_VideoInfo *info = SDL_GetVideoInfo();
	if (info == NULL)
		return 0;

	switch (info->vfmt->BitsPerPixel) {
	case 8:
		mode->format = SDL_PIXELFORMAT_INDEX8;
		break;
	case 24:
		mode->format = SDL_PIXELFORMAT_RGB888;
		break;
	case 32:
		mode->format = SDL_PIXELFORMAT_RGBA8888;
		break;
	default:
		mode->format = 0;
		break;
	}

	mode->w = info->current_w;
	mode->h = info->current_h;
	mode->refresh_rate = 0;
	mode->driverdata = NULL;

	return 0;
}

//== Filesystem

#if !defined(__QNXNTO__)
inline char *
readSymLink(const char *path)
{
	// From sdl2-2.0.9/src/filesystem/unix/SDL_sysfilesystem.c
	char *retval = NULL;
	ssize_t len = 64;
	ssize_t rc = -1;

	while (1) {
		char *ptr = (char *)SDL_realloc(retval, (size_t)len);
		if (ptr == NULL) {
			SDL_OutOfMemory();
			break;
		}

		retval = ptr;

		rc = readlink(path, retval, len);
		if (rc == -1) {
			break; /* not a symlink, i/o error, etc. */
		} else if (rc < len) {
			retval[rc] = '\0'; /* readlink doesn't null-terminate. */
			return retval;     /* we're good to go. */
		}

		len *= 2; /* grow buffer, try again. */
	}

	SDL_free(retval);
	return NULL;
}
#endif

inline char *SDL_GetBasePath()
{
	// From sdl2-2.0.9/src/filesystem/unix/SDL_sysfilesystem.c

	char *retval = NULL;

#if defined(__FREEBSD__)
	char fullpath[PATH_MAX];
	size_t buflen = sizeof(fullpath);
	const int mib[] = { CTL_KERN, KERN_PROC, KERN_PROC_PATHNAME, -1 };
	if (sysctl(mib, SDL_arraysize(mib), fullpath, &buflen, NULL, 0) != -1) {
		retval = SDL_strdup(fullpath);
		if (!retval) {
			SDL_OutOfMemory();
			return NULL;
		}
	}
#endif
#if defined(__OPENBSD__)
	char **retvalargs;
	size_t len;
	const int mib[] = { CTL_KERN, KERN_PROC_ARGS, getpid(), KERN_PROC_ARGV };
	if (sysctl(mib, 4, NULL, &len, NULL, 0) != -1) {
		retvalargs = SDL_malloc(len);
		if (!retvalargs) {
			SDL_OutOfMemory();
			return NULL;
		}
		sysctl(mib, 4, retvalargs, &len, NULL, 0);
		retval = SDL_malloc(PATH_MAX + 1);
		if (retval)
			realpath(retvalargs[0], retval);

		SDL_free(retvalargs);
	}
#endif
#if defined(__SOLARIS__)
	const char *path = getexecname();
	if ((path != NULL) && (path[0] == '/')) { /* must be absolute path... */
		retval = SDL_strdup(path);
		if (!retval) {
			SDL_OutOfMemory();
			return NULL;
		}
	}
#endif

	/* is a Linux-style /proc filesystem available? */
	if (!retval && (access("/proc", F_OK) == 0)) {
		/* !!! FIXME: after 2.0.6 ships, let's delete this code and just
                      use the /proc/%llu version. There's no reason to have
                      two copies of this plus all the #ifdefs. --ryan. */
#if defined(__FREEBSD__)
		retval = readSymLink("/proc/curproc/file");
#elif defined(__NETBSD__)
		retval = readSymLink("/proc/curproc/exe");
#elif defined(__QNXNTO__)
		retval = SDL_LoadFile("/proc/self/exefile", NULL);
#else
		retval = readSymLink("/proc/self/exe"); /* linux. */
		if (retval == NULL) {
			/* older kernels don't have /proc/self ... try PID version... */
			char path[64];
			const int rc = (int)SDL_snprintf(path, sizeof(path),
			    "/proc/%llu/exe",
			    (unsigned long long)getpid());
			if ((rc > 0) && (static_cast<std::size_t>(rc) < sizeof(path))) {
				retval = readSymLink(path);
			}
		}
#endif
	}

	/* If we had access to argv[0] here, we could check it for a path,
        or troll through $PATH looking for it, too. */

	if (retval != NULL) { /* chop off filename. */
		char *ptr = SDL_strrchr(retval, '/');
		if (ptr != NULL) {
			*(ptr + 1) = '\0';
		} else { /* shouldn't happen, but just in case... */
			SDL_free(retval);
			retval = NULL;
		}
	}

	if (retval != NULL) {
		/* try to shrink buffer... */
		char *ptr = (char *)SDL_realloc(retval, strlen(retval) + 1);
		if (ptr != NULL)
			retval = ptr; /* oh well if it failed. */
	}

	return retval;
}

inline char *SDL_GetPrefPath(const char *org, const char *app)
{
	// From sdl2-2.0.9/src/filesystem/unix/SDL_sysfilesystem.c
	/*
     * We use XDG's base directory spec, even if you're not on Linux.
     *  This isn't strictly correct, but the results are relatively sane
     *  in any case.
     *
     * http://standards.freedesktop.org/basedir-spec/basedir-spec-latest.html
     */
	const char *envr = SDL_getenv("XDG_DATA_HOME");
	const char *append;
	char *retval = NULL;
	char *ptr = NULL;
	size_t len = 0;

	if (!app) {
		SDL_InvalidParamError("app");
		return NULL;
	}
	if (!org) {
		org = "";
	}

	if (!envr) {
		/* You end up with "$HOME/.local/share/Game Name 2" */
		envr = SDL_getenv("HOME");
		if (!envr) {
			/* we could take heroic measures with /etc/passwd, but oh well. */
			SDL_SetError("neither XDG_DATA_HOME nor HOME environment is set");
			return NULL;
		}
#if defined(__unix__) || defined(__unix)
		append = "/.local/share/";
#else
		append = "/";
#endif
	} else {
		append = "/";
	}

	len = SDL_strlen(envr);
	if (envr[len - 1] == '/')
		append += 1;

	len += SDL_strlen(append) + SDL_strlen(org) + SDL_strlen(app) + 3;
	retval = (char *)SDL_malloc(len);
	if (!retval) {
		SDL_OutOfMemory();
		return NULL;
	}

	if (*org) {
		SDL_snprintf(retval, len, "%s%s%s/%s", envr, append, org, app);
	} else {
		SDL_snprintf(retval, len, "%s%s%s", envr, append, app);
	}

	for (ptr = retval + 1; *ptr; ptr++) {
		if (*ptr == '/') {
			*ptr = '\0';
			if (mkdir(retval, 0700) != 0 && errno != EEXIST)
				goto error;
			*ptr = '/';
		}
	}
	if (mkdir(retval, 0700) != 0 && errno != EEXIST) {
	error:
		SDL_SetError("Couldn't create directory '%s': '%s'", retval, strerror(errno));
		SDL_free(retval);
		return NULL;
	}

	// Append trailing /
	size_t final_len = SDL_strlen(retval);
	if (final_len + 1 < len) {
		retval[final_len++] = '/';
		retval[final_len] = '\0';
	}

	return retval;
}

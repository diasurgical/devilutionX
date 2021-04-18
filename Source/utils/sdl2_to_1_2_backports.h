#pragma once

#include <SDL.h>
#include <unistd.h>
#include <errno.h>
#include <stdio.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <math.h>
#include <cstddef>

#include "utils/console.h"
#include "utils/stubs.h"

#define WINDOW_ICON_NAME 0

//== Utility

#define SDL_zero(x) SDL_memset(&(x), 0, sizeof((x)))
#define SDL_InvalidParamError(param) SDL_SetError("Parameter '%s' is invalid", (param))
#define SDL_floor floor

#define SDL_MAX_UINT32 ((Uint32)0xFFFFFFFFu)

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

#define SDL_JoystickID Sint32
#define SDL_JoystickNameForIndex SDL_JoystickName

enum SDL_LogCategory {
	SDL_LOG_CATEGORY_APPLICATION,
	SDL_LOG_CATEGORY_ERROR,
	SDL_LOG_CATEGORY_ASSERT,
	SDL_LOG_CATEGORY_SYSTEM,
	SDL_LOG_CATEGORY_AUDIO,
	SDL_LOG_CATEGORY_VIDEO,
	SDL_LOG_CATEGORY_RENDER,
	SDL_LOG_CATEGORY_INPUT,
	SDL_LOG_CATEGORY_TEST,
};

enum SDL_LogPriority {
	SDL_LOG_PRIORITY_VERBOSE = 1,
	SDL_LOG_PRIORITY_DEBUG,
	SDL_LOG_PRIORITY_INFO,
	SDL_LOG_PRIORITY_WARN,
	SDL_LOG_PRIORITY_ERROR,
	SDL_LOG_PRIORITY_CRITICAL,
	SDL_NUM_LOG_PRIORITIES
};

void SDL_Log(const char *fmt, ...) DVL_PRINTF_ATTRIBUTE(1, 2);
void SDL_LogVerbose(int category, const char *fmt, ...) DVL_PRINTF_ATTRIBUTE(2, 3);
void SDL_LogDebug(int category, const char *fmt, ...) DVL_PRINTF_ATTRIBUTE(2, 3);
void SDL_LogInfo(int category, const char *fmt, ...) DVL_PRINTF_ATTRIBUTE(2, 3);
void SDL_LogWarn(int category, const char *fmt, ...) DVL_PRINTF_ATTRIBUTE(2, 3);
void SDL_LogError(int category, const char *fmt, ...) DVL_PRINTF_ATTRIBUTE(2, 3);
void SDL_LogCritical(int category, const char *fmt, ...) DVL_PRINTF_ATTRIBUTE(2, 3);
void SDL_LogMessageV(int category, SDL_LogPriority priority, const char *fmt, va_list ap) DVL_PRINTF_ATTRIBUTE(3, 0);

void SDL_LogSetAllPriority(SDL_LogPriority priority);
void SDL_LogSetPriority(int category, SDL_LogPriority priority);
SDL_LogPriority SDL_LogGetPriority(int category);

inline void SDL_StartTextInput()
{
}

inline void SDL_StopTextInput()
{
}

inline void SDL_SetTextInputRect(const SDL_Rect *r)
{
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

//= Messagebox (simply logged to stderr for now)

enum {
	// clang-format off
	SDL_MESSAGEBOX_ERROR       = 1 << 4, /**< error dialog */
	SDL_MESSAGEBOX_WARNING     = 1 << 5, /**< warning dialog */
	SDL_MESSAGEBOX_INFORMATION = 1 << 6, /**< informational dialog */
	// clang-format on
};

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
	SDL_Log("SDL_GetWindowPosition %d %d", *x, *y);
}

inline void SDL_GetWindowSize(SDL_Window *window, int *w, int *h)
{
	*w = window->clip_rect.w;
	*h = window->clip_rect.h;
	SDL_Log("SDL_GetWindowSize %d %d", *w, *h);
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

// NOTE: Not thread-safe
int SDL_SoftStretch(SDL_Surface *src, const SDL_Rect *srcrect,
    SDL_Surface *dst, const SDL_Rect *dstrect);

// NOTE: The second argument is const in SDL2 but not here.
int SDL_BlitScaled(SDL_Surface *src, SDL_Rect *srcrect,
    SDL_Surface *dst, SDL_Rect *dstrect);

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
#if defined(__3DS__)
	retval = SDL_strdup("file:sdmc:/3ds/devilutionx/");
	return retval;
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

#if defined(__3DS__)
	retval = SDL_strdup("sdmc:/3ds/devilutionx/");
	return retval;
#endif

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

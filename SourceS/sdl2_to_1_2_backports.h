#pragma once

#include <SDL.h>
#include <unistd.h>
#include <errno.h>
#include <stdio.h>
#include <sys/stat.h>
#include <sys/types.h>

#include "stubs.h"

#define WINDOW_ICON_NAME 0

//== Utility

#define SDL_zero(x) SDL_memset(&(x), 0, sizeof((x)))
#define SDL_InvalidParamError(param) SDL_SetError("Parameter '%s' is invalid", (param))
#define SDL_Log puts

//== Events handling

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

//= Messagebox (simply logged to stderr for now)

typedef enum {
	SDL_MESSAGEBOX_ERROR       = 0x00000010, /**< error dialog */
	SDL_MESSAGEBOX_WARNING     = 0x00000020, /**< warning dialog */
	SDL_MESSAGEBOX_INFORMATION = 0x00000040  /**< informational dialog */
} SDL_MessageBoxFlags;

inline int SDL_ShowSimpleMessageBox(Uint32 flags,
    const char *title,
    const char *message,
    SDL_Surface *window)
{
	fprintf(stderr, "MSGBOX: %s\n%s\n", title, message);
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

inline void SDL_SetWindowPosition(SDL_Window *window, int x, int y) {
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
SDL_WarpMouseInWindow(SDL_Window * window, int x, int y)
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

inline int SDL_SetPaletteColors(SDL_Palette *palette, const SDL_Color *colors,
    int firstcolor, int ncolors)
{
	int status = 0;

	/* Verify the parameters */
	if (!palette) {
		return -1;
	}
	if (ncolors > (palette->ncolors - firstcolor)) {
		ncolors = (palette->ncolors - firstcolor);
		status  = -1;
	}

	if (colors != (palette->colors + firstcolor)) {
		SDL_memcpy(palette->colors + firstcolor, colors,
		    ncolors * sizeof(*colors));
	}

	return status;
}

//= Pixel formats

#define SDL_PIXELFORMAT_RGBA8888 1
#define SDL_PIXELFORMAT_INDEX8 2

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
	}
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
	case 32:
		mode->format = SDL_PIXELFORMAT_RGBA8888;
	default:
		mode->format = 0;
	}

	mode->w            = info->current_w;
	mode->h            = info->current_h;
	mode->refresh_rate = 0;
	mode->driverdata   = NULL;

	return 0;
}

//== Filesystem

#if !defined(__QNXNTO__)
inline char *
readSymLink(const char *path)
{
	// From sdl2-2.0.9/src/filesystem/unix/SDL_sysfilesystem.c
	char *retval = NULL;
	ssize_t len  = 64;
	ssize_t rc   = -1;

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
	size_t buflen   = sizeof(fullpath);
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
			if ((rc > 0) && (rc < sizeof(path))) {
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
	char *ptr    = NULL;
	size_t len   = 0;

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
		append = "/.local/share/";
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
		SDL_snprintf(retval, len, "%s%s%s/%s/", envr, append, org, app);
	} else {
		SDL_snprintf(retval, len, "%s%s%s/", envr, append, app);
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

	return retval;
}

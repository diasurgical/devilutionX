#pragma once

#include <SDL2/SDL.h>
//#include <SDL_image.h>
#include <SDL_mixer.h>
#include "SDL_FontCache.h"
#include "miniwin_sdl.h"


#include <assert.h>
#include <stdio.h>

#define eprintf(...) fprintf(stderr, __VA_ARGS__)

#define UNIMPLEMENTED() \
	{ \
		eprintf("UNIMPLEMENTED: %s @ %s:%d\n", __FUNCTION__, __FILE__, __LINE__); \
		abort(); \
	}

#define DUMMY() /*eprintf("DUMMY: %s @ %s:%d\n", __FUNCTION__, __FILE__, __LINE__)*/

#define DUMMY_ONCE() \
	{ \
		static bool dummy_shown = false; \
		if (!dummy_shown) { \
			DUMMY(); \
			dummy_shown = true; \
		} \
	}

#define DUMMY_PRINT(fmt, ...) eprintf("DUMMY: %s : " fmt "\n", __FUNCTION__, __VA_ARGS__)

static inline const char *nullstr(const char *a)
{
	return a ? a : "(null)";
}

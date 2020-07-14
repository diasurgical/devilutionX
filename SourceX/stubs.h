#pragma once

#include <assert.h>
#include <stdio.h>
#include <stdlib.h>

#define UNIMPLEMENTED()                                                           \
	do {                                                                          \
		SDL_Log("UNIMPLEMENTED: %s @ %s:%d", __FUNCTION__, __FILE__, __LINE__); \
		abort();                                                                  \
	} while (0)

#define UNIMPLEMENTED_UNLESS(x) \
	do {                        \
		if (!(x))               \
			UNIMPLEMENTED();    \
	} while (0)

#define ABORT()                                                           \
	do {                                                                  \
		SDL_Log("ABORT: %s @ %s:%d", __FUNCTION__, __FILE__, __LINE__); \
		abort();                                                          \
	} while (0)

#define DUMMY() SDL_Log("DUMMY: %s @ %s:%d", __FUNCTION__, __FILE__, __LINE__)

#define DUMMY_ONCE()                     \
	do {                                 \
		static bool dummy_shown = false; \
		if (!dummy_shown) {              \
			DUMMY();                     \
			dummy_shown = true;          \
		}                                \
	} while (0)

static inline const char *nullstr(const char *a)
{
	return a ? a : "(null)";
}

#define ASSERT(x)                                                         \
	if (!(x)) {                                                           \
		SDL_Log("Assertion failed in %s:%d: %s", __FILE__, __LINE__, #x); \
		abort();                                                          \
	}

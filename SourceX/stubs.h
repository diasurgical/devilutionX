#pragma once

#include <assert.h>
#include <stdio.h>
#include <stdlib.h>

#ifdef __ANDROID__
#include <android/log.h>
#define printf(...) __android_log_print(ANDROID_LOG_DEBUG, "DevilutionX", __VA_ARGS__)
#define eprintf(...) printf(__VA_ARGS__)
#else
#define eprintf(...) fprintf(stderr, __VA_ARGS__)
#endif

#define UNIMPLEMENTED()                                                           \
	do {                                                                          \
		eprintf("UNIMPLEMENTED: %s @ %s:%d\n", __FUNCTION__, __FILE__, __LINE__); \
		abort();                                                                  \
	} while (0)

#define UNIMPLEMENTED_UNLESS(x) \
	do {                        \
		if (!(x))               \
			UNIMPLEMENTED();    \
	} while (0)

#define ABORT()                                                           \
	do {                                                                  \
		eprintf("ABORT: %s @ %s:%d\n", __FUNCTION__, __FILE__, __LINE__); \
		abort();                                                          \
	} while (0)

#define DUMMY() eprintf("DUMMY: %s @ %s:%d\n", __FUNCTION__, __FILE__, __LINE__)

#define DUMMY_ONCE()                     \
	do {                                 \
		static bool dummy_shown = false; \
		if (!dummy_shown) {              \
			DUMMY();                     \
			dummy_shown = true;          \
		}                                \
	} while (0)

#define DUMMY_PRINT(fmt, ...) eprintf("DUMMY: %s : " fmt "\n", __FUNCTION__, ##__VA_ARGS__)

static inline const char *nullstr(const char *a)
{
	return a ? a : "(null)";
}

#define ASSERT(x)                                                                   \
	if (!(x)) {                                                                     \
		fprintf(stderr, "Assertion failed in %s:%d: %s\n", __FILE__, __LINE__, #x); \
		abort();                                                                    \
	}

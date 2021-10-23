/** A pointer to a Storm file as a `FILE *`. Only available on some platforms. */
#pragma once
#if (defined(__linux__) && !defined(__ANDROID__)) || defined(__FreeBSD__) || defined(__DragonFly__) || defined(__HAIKU__)
#include <cstdio>

#include <SDL.h>

#define DEVILUTIONX_SDL_RWOPS_FILE_WRAPPER_AVAILABLE
#define DEVILUTIONX_SDL_RWOPS_FILE_WRAPPER_IMPL_FOPENCOOKIE

namespace devilution {

FILE *FILE_FromSDL_RWops(SDL_RWops *handle);

} // namespace devilution

#endif

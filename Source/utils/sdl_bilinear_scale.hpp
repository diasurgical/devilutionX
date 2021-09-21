#pragma once

#include <SDL_surface.h>

namespace devilution {

/**
 * @brief Bilinear 32-bit scaling.
 * Requires `src` and `dst` to have the same pixel format (ARGB8888 or RGBA8888).
 */
void BilinearScale32(SDL_Surface *src, SDL_Surface *dst);

} // namespace devilution

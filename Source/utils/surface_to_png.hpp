#include <cstdio>
#include <string>

#include <SDL.h>
#include <expected.hpp>

#include "engine/surface.hpp"

namespace devilution {

/**
 * @brief Writes the given surface to `dst` as PNG.
 *
 * Takes ownership of `dst` and closes it when done.
 */
tl::expected<void, std::string>
WriteSurfaceToFilePng(const Surface &buf, SDL_RWops *dst);

} // namespace devilution

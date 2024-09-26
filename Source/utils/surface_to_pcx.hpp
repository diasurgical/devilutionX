#include <cstdio>
#include <string>

#include <SDL.h>
#include <expected.hpp>

#include "engine/surface.hpp"

namespace devilution {

tl::expected<void, std::string>
WriteSurfaceToFilePcx(const Surface &buf, SDL_RWops *outStream);

} // namespace devilution

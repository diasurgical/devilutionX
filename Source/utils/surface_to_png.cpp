#include "utils/surface_to_png.hpp"

#include <cstdio>
#include <string>

#include <SDL.h>
#include <expected.hpp>

#include "engine/surface.hpp"

namespace devilution {

extern "C" int IMG_SavePNG_RW(SDL_Surface *surface, SDL_RWops *dst, int freedst);

tl::expected<void, std::string>
WriteSurfaceToFilePng(const Surface &buf, SDL_RWops *dst)
{
	if (IMG_SavePNG_RW(buf.surface, dst, /*freedst=*/1) != 0) {
		tl::expected<void, std::string> result = tl::make_unexpected(std::string(SDL_GetError()));
		SDL_ClearError();
		return result;
	}
	return {};
}

} // namespace devilution

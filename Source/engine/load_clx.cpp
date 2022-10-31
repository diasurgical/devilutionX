#include "engine/load_clx.hpp"

#include <SDL.h>

#ifdef USE_SDL1
#include "utils/sdl2_to_1_2_backports.h"
#endif

#include "engine/assets.hpp"
#include "engine/load_file.hpp"

namespace devilution {

OptionalOwnedClxSpriteListOrSheet LoadOptionalClxListOrSheet(const char *path)
{
	SDL_RWops *handle = OpenAsset(path);
	if (handle == nullptr)
		return std::nullopt;
	const size_t size = SDL_RWsize(handle);
	std::unique_ptr<uint8_t[]> data { new uint8_t[size] };
	SDL_RWread(handle, data.get(), size, 1);
	SDL_RWclose(handle);
	return OwnedClxSpriteListOrSheet::FromBuffer(std::move(data), size);
}

OwnedClxSpriteListOrSheet LoadClxListOrSheet(const char *path)
{
	size_t size;
	std::unique_ptr<uint8_t[]> data = LoadFileInMem<uint8_t>(path, &size);
	return OwnedClxSpriteListOrSheet::FromBuffer(std::move(data), size);
}

} // namespace devilution

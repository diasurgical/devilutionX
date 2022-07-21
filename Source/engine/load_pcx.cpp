#include "engine/load_pcx.hpp"

#include <cstddef>
#include <memory>
#include <utility>

#ifdef DEBUG_PCX_TO_CL2_SIZE
#include <iostream>
#endif

#include <SDL.h>

#include "engine/assets.hpp"
#include "utils/log.hpp"
#include "utils/pcx.hpp"
#include "utils/pcx_to_clx.hpp"

#ifdef USE_SDL1
#include "utils/sdl2_to_1_2_backports.h"
#endif

namespace devilution {

OptionalOwnedClxSpriteList LoadPcxSpriteList(const char *filename, int numFramesOrFrameHeight, std::optional<uint8_t> transparentColor, SDL_Color *outPalette)
{
	SDL_RWops *handle = OpenAsset(filename);
	if (handle == nullptr) {
		LogError("Missing file: {}", filename);
		return std::nullopt;
	}
#ifdef DEBUG_PCX_TO_CL2_SIZE
	std::cout << filename;
#endif
	OptionalOwnedClxSpriteList result = PcxToClx(handle, numFramesOrFrameHeight, transparentColor, outPalette);
	if (!result)
		return std::nullopt;
	return result;
}

} // namespace devilution

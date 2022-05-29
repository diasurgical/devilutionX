#include "engine/load_pcx_as_cel.hpp"

#include <SDL.h>

#include "engine/assets.hpp"
#include "utils/log.hpp"
#include "utils/pcx_to_cel.hpp"

namespace devilution {

std::optional<OwnedCelSpriteWithFrameHeight> LoadPcxAssetAsCel(const char *filename, unsigned numFrames, bool generateFrameHeaders, uint8_t transparentColorIndex)
{
	SDL_RWops *handle = OpenAsset(filename);
	if (handle == nullptr) {
		LogError("Missing file: {}", filename);
		return std::nullopt;
	}
	return LoadPcxAsCel(handle, numFrames, generateFrameHeaders, transparentColorIndex);
}

} // namespace devilution

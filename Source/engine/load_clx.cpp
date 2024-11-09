#include "engine/load_clx.hpp"

#include <cstdint>

#include <SDL.h>

#ifdef USE_SDL1
#include "utils/sdl2_to_1_2_backports.h"
#endif

#include "engine/assets.hpp"
#include "engine/clx_sprite.hpp"
#include "engine/load_file.hpp"

namespace devilution {

OptionalOwnedClxSpriteListOrSheet LoadOptionalClxListOrSheet(const char *path)
{
	std::string_view pathStrView = path;
	AssetRef ref = FindAsset(pathStrView);
	if (!ref.ok())
		return std::nullopt;
	const size_t size = ref.size();
	std::unique_ptr<uint8_t[]> data { new uint8_t[size] };
	{
		AssetHandle handle = OpenAsset(std::move(ref));
		if (!handle.ok() || !handle.read(data.get(), size))
			return std::nullopt;
	}
	pathStrView.remove_suffix(4);
	return OwnedClxSpriteListOrSheet::fromBuffer(
#ifdef DEVILUTIONX_RESOURCE_TRACKING_ENABLED
	    pathStrView, /*trnName=*/ {},
#endif
	    std::move(data), size);
}

OwnedClxSpriteListOrSheet LoadClxListOrSheet(const char *path)
{
	size_t size;
	std::unique_ptr<uint8_t[]> data = LoadFileInMem<uint8_t>(path, &size);
	std::string_view pathStrView = path;
	pathStrView.remove_suffix(4);
	return OwnedClxSpriteListOrSheet::fromBuffer(
#ifdef DEVILUTIONX_RESOURCE_TRACKING_ENABLED
	    pathStrView, /*trnName=*/ {},
#endif
	    std::move(data), size);
}

} // namespace devilution

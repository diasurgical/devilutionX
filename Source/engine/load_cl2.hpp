#pragma once

#include <cstdint>

#include <function_ref.hpp>

#include "appfat.h"
#include "engine/clx_sprite.hpp"
#include "engine/load_file.hpp"
#include "utils/cl2_to_clx.hpp"
#include "utils/endian.hpp"
#include "utils/pointer_value_union.hpp"
#include "utils/static_vector.hpp"

namespace devilution {

OwnedClxSpriteListOrSheet LoadCl2ListOrSheet(const char *pszName, PointerOrValue<uint16_t> widthOrWidths);

template <size_t MaxCount>
OwnedClxSpriteSheet LoadMultipleCl2Sheet(tl::function_ref<const char *(size_t)> filenames, size_t count, uint16_t width)
{
	StaticVector<SFile, MaxCount> files;
	StaticVector<size_t, MaxCount> fileSizes;
	const size_t sheetHeaderSize = 4 * count;
	size_t totalSize = sheetHeaderSize;
	for (size_t i = 0; i < count; ++i) {
		const char *filename = filenames(i);
		files.emplace_back(filename);
		const size_t size = files[i].Size();
		fileSizes.emplace_back(size);
		totalSize += size;
	}
	auto data = std::unique_ptr<uint8_t[]> { new uint8_t[totalSize] };
	const PointerOrValue<uint16_t> frameWidth { width };
	size_t accumulatedSize = sheetHeaderSize;
	for (size_t i = 0; i < count; ++i) {
		const size_t size = fileSizes[i];
		if (!files[i].Read(&data[accumulatedSize], size))
			app_fatal(StrCat("Read failed: ", SDL_GetError()));
		WriteLE32(&data[i * 4], accumulatedSize);
		[[maybe_unused]] const uint16_t numLists = Cl2ToClx(&data[accumulatedSize], size, frameWidth);
		assert(numLists == 0);
		accumulatedSize += size;
	}
	return OwnedClxSpriteSheet { std::move(data), static_cast<uint16_t>(count) };
}

inline OwnedClxSpriteList LoadCl2(const char *pszName, uint16_t width)
{
	return LoadCl2ListOrSheet(pszName, PointerOrValue<uint16_t> { width }).list();
}

inline OwnedClxSpriteList LoadCl2(const char *pszName, const uint16_t *widths)
{
	return LoadCl2ListOrSheet(pszName, PointerOrValue<uint16_t> { widths }).list();
}

inline OwnedClxSpriteSheet LoadCl2Sheet(const char *pszName, uint16_t width)
{
	return LoadCl2ListOrSheet(pszName, PointerOrValue<uint16_t> { width }).sheet();
}

} // namespace devilution

#pragma once

#include <cstdint>

#include "engine/clx_sprite.hpp"
#include "utils/pointer_value_union.hpp"

namespace devilution {

OwnedClxSpriteListOrSheet LoadCelListOrSheet(const char *path, PointerOrValue<uint16_t> widthOrWidths);

inline OwnedClxSpriteList LoadCel(const char *path, uint16_t width)
{
	return LoadCelListOrSheet(path, PointerOrValue<uint16_t> { width }).list();
}

inline OwnedClxSpriteList LoadCel(const char *path, const uint16_t *widths)
{
	return LoadCelListOrSheet(path, PointerOrValue<uint16_t> { widths }).list();
}

inline OwnedClxSpriteSheet LoadCelSheet(const char *path, uint16_t width)
{
	return LoadCelListOrSheet(path, PointerOrValue<uint16_t> { width }).sheet();
}

} // namespace devilution

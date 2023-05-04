#pragma once

#include <cstdint>

#include "engine/clx_sprite.hpp"
#include "utils/pointer_value_union.hpp"

#ifdef UNPACKED_MPQS
#define DEVILUTIONX_CEL_EXT ".clx"
#else
#define DEVILUTIONX_CEL_EXT ".cel"
#endif

namespace devilution {

OwnedClxSpriteListOrSheet LoadCelListOrSheet(const char *pszName, PointerOrValue<uint16_t> widthOrWidths);

inline OwnedClxSpriteList LoadCel(const char *pszName, uint16_t width)
{
	return LoadCelListOrSheet(pszName, PointerOrValue<uint16_t> { width }).list();
}

inline OwnedClxSpriteList LoadCel(const char *pszName, const uint16_t *widths)
{
	return LoadCelListOrSheet(pszName, PointerOrValue<uint16_t> { widths }).list();
}

inline OwnedClxSpriteSheet LoadCelSheet(const char *pszName, uint16_t width)
{
	return LoadCelListOrSheet(pszName, PointerOrValue<uint16_t> { width }).sheet();
}

} // namespace devilution

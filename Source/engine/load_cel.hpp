#pragma once

#include <cstdint>

#include "engine/cel_sprite.hpp"

namespace devilution {

OwnedCelSprite LoadCelAsCl2(const char *pszName, uint16_t width);
OwnedCelSprite LoadCelAsCl2(const char *pszName, const uint16_t *widths);

} // namespace devilution

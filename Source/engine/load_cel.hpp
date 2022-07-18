#pragma once

#include <cstdint>

#include "engine/cel_sprite.hpp"

namespace devilution {

/**
 * @brief Loads a Cel sprite and sets its width
 */
OwnedCelSprite LoadCel(const char *pszName, uint16_t width);
OwnedCelSprite LoadCel(const char *pszName, const uint16_t *widths);

OwnedCelSprite LoadCelAsCl2(const char *pszName, uint16_t width);
OwnedCelSprite LoadCelAsCl2(const char *pszName, const uint16_t *widths);

} // namespace devilution

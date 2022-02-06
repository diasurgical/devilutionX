#pragma once

#include "engine/cel_sprite.hpp"

namespace devilution {

/**
 * @brief Loads a Cel sprite and sets its width
 */
OwnedCelSprite LoadCel(const char *pszName, int width);
OwnedCelSprite LoadCel(const char *pszName, const int *widths);

} // namespace devilution

#pragma once

#include "engine/cel_sprite.hpp"

namespace devilution {

/**
 * @brief Loads a Cel sprite and sets its width
 */
CelSprite LoadCel(const char *pszName, int width);
CelSprite LoadCel(const char *pszName, const int *widths);

} // namespace devilution

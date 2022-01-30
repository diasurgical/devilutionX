#pragma once

#include "engine/cel_sprite.hpp"
#include "utils/stdcompat/optional.hpp"

namespace devilution {

/**
 * @brief Info box frame
 *
 * Used in stores, the quest log, the help window, and the unique item info window.
 */
extern std::optional<CelSprite> pSTextBoxCels;

/**
 * @brief Info box scrollbar graphics.
 *
 * Used in stores and `DrawDiabloMsg`.
 */
extern std::optional<CelSprite> pSTextSlidCels;

void InitInfoBoxGfx();
void FreeInfoBoxGfx();

} // namespace devilution

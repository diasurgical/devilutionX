#pragma once

#include "engine/clx_sprite.hpp"

namespace devilution {

/**
 * @brief Fixed size info box frame
 *
 * Used in stores, the quest log, the help window, and the unique item info window.
 */
extern OptionalOwnedClxSpriteList pSTextBoxCels;

/**
 * @brief Dynamic size info box frame and scrollbar graphics.
 *
 * Used in stores and `DrawDiabloMsg`.
 */
extern OptionalOwnedClxSpriteList pSTextSlidCels;

void InitInfoBoxGfx();
void FreeInfoBoxGfx();

} // namespace devilution

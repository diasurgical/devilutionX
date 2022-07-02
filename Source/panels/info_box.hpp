#pragma once

#include "engine/cel_sprite.hpp"

namespace devilution {

/**
 * @brief Info box frame
 *
 * Used in stores, the quest log, the help window, and the unique item info window.
 */
extern OptionalOwnedCelSprite pSTextBoxCels;

/**
 * @brief Info box scrollbar graphics.
 *
 * Used in stores and `DrawDiabloMsg`.
 */
extern OptionalOwnedCelSprite pSTextSlidCels;

void InitInfoBoxGfx();
void FreeInfoBoxGfx();

} // namespace devilution

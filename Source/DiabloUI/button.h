#pragma once

#include "DiabloUI/ui_item.h"
#include "engine/pcx_sprite.hpp"

namespace devilution {

const Uint16 SML_BUTTON_WIDTH = 110;
const Uint16 SML_BUTTON_HEIGHT = 28;

PcxSprite ButtonSprite(bool pressed);
void RenderButton(UiButton *button);
bool HandleMouseEventButton(const SDL_Event &event, UiButton *button);
void HandleGlobalMouseUpButton(UiButton *button);

} // namespace devilution

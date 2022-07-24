#pragma once

#include "DiabloUI/ui_item.h"
#include "engine/cel_sprite.hpp"

namespace devilution {

const Uint16 DialogButtonWidth = 110;
const Uint16 DialogButtonHeight = 28;

void LoadDialogButtonGraphics();
void FreeDialogButtonGraphics();
CelFrameWithHeight ButtonSprite(bool pressed);
void RenderButton(UiButton *button);
bool HandleMouseEventButton(const SDL_Event &event, UiButton *button);
void HandleGlobalMouseUpButton(UiButton *button);

} // namespace devilution

#pragma once

#include "DiabloUI/art.h"
#include "DiabloUI/ui_item.h"

namespace dvl {

extern Art SmlButton;
void LoadSmlButtonArt();
inline void UnloadSmlButtonArt()
{
	SmlButton.Unload();
}
const Uint16 SML_BUTTON_WIDTH = 110;
const Uint16 SML_BUTTON_HEIGHT = 28;

void RenderButton(UiButton *button);
bool HandleMouseEventButton(const SDL_Event &event, UiButton *button);
void HandleGlobalMouseUpButton(UiButton *button);

} // namespace dvl

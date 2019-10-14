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
constexpr decltype(SDL_Rect().w) SML_BUTTON_WIDTH = 110;
constexpr decltype(SDL_Rect().h) SML_BUTTON_HEIGHT = 28;

constexpr UiButton MakeSmlButton(
    const char *text, void (*action)(), decltype(SDL_Rect().x) x, decltype(SDL_Rect().y) y, int flags = 0)
{
	return UiButton(
	    &SmlButton,
	    text,
	    action,
	    SDL_Rect{ x, y, SML_BUTTON_WIDTH, SML_BUTTON_HEIGHT },
	    flags);
}

void RenderButton(UiButton *button);
bool HandleMouseEventButton(const SDL_Event &event, UiButton *button);
void HandleGlobalMouseUpButton(UiButton *button);

} // namespace dvl

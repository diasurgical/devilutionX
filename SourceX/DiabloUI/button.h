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

enum class ButtonFrame {
	BG_GOLD = 0,
	BG_GOLD_PRESSED,
	BG_YELLOW,
	BG_YELLOW_PRESSED,
	DISABLED,
};

constexpr UiButton::FrameMap ButtonBgYellowFrameMap = {
	static_cast<int>(ButtonFrame::BG_YELLOW),
	static_cast<int>(ButtonFrame::BG_YELLOW_PRESSED),
	static_cast<int>(ButtonFrame::DISABLED),
};
constexpr UiButton::FrameMap ButtonBgGoldFrameMap = {
	static_cast<int>(ButtonFrame::BG_GOLD),
	static_cast<int>(ButtonFrame::BG_GOLD_PRESSED),
	static_cast<int>(ButtonFrame::DISABLED),
};

constexpr UiButton MakeSmlButton(
    const char *text, void (*action)(), decltype(SDL_Rect().x) x, decltype(SDL_Rect().y) y, int flags = 0)
{
	return UiButton(
	    &SmlButton,
	    (flags & UIS_GOLD) ? ButtonBgGoldFrameMap : ButtonBgYellowFrameMap,
	    text,
	    action,
	    SDL_Rect{ x, y, SML_BUTTON_WIDTH, SML_BUTTON_HEIGHT },
	    flags);
}

void RenderButton(UiButton *button);
bool HandleMouseEventButton(const SDL_Event &event, UiButton *button);
void HandleGlobalMouseUpButton(UiButton *button);

} // namespace dvl

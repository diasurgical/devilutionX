#include "DiabloUI/button.h"
#include "DiabloUI/art_draw.h"
#include "DiabloUI/text_draw.h"
#include "DiabloUI/errorart.h"

namespace dvl {

Art SmlButton;

void LoadSmlButtonArt()
{
	LoadArt(&SmlButton, btnData, SML_BUTTON_WIDTH, SML_BUTTON_HEIGHT * 2, 2);
}

void RenderButton(UiButton *button)
{
	int frame;
	if (button->pressed) {
		frame = UiButton::PRESSED;
	} else {
		frame = UiButton::DEFAULT;
	}
	DrawArt(button->rect.x, button->rect.y, button->art, frame, button->rect.w, button->rect.h);

	SDL_Rect text_rect = button->rect;
	if (!button->pressed)
		--text_rect.y;
	DrawTTF(button->text, text_rect, UIS_CENTER,
	    SDL_Color { 243, 243, 243, 0 }, SDL_Color { 0, 0, 0, 0 }, &button->render_cache);
}

bool HandleMouseEventButton(const SDL_Event &event, UiButton *button)
{
	if (event.button.button != SDL_BUTTON_LEFT)
		return false;
	switch (event.type) {
	case SDL_MOUSEBUTTONUP:
		button->action();
		return true;
	case SDL_MOUSEBUTTONDOWN:
		button->pressed = true;
		return true;
	default:
		return false;
	}
}

void HandleGlobalMouseUpButton(UiButton *button)
{
	button->pressed = false;
}

} // namespace dvl

#include "DiabloUI/button.h"
#include "DiabloUI/art_draw.h"
#include "DiabloUI/text_draw.h"

namespace dvl {

Art SmlButton;

void LoadSmlButtonArt()
{
	LoadArt("ui_art\\but_sml.pcx", &SmlButton, 15);
}

void RenderButton(UiButton *button)
{
	int frame;
	if (button->has_flag(UIS_DISABLED)) {
		frame = button->frame_map[UiButton::DISABLED];
	} else if (button->pressed) {
		frame = button->frame_map[UiButton::PRESSED];
	} else {
		frame = button->frame_map[UiButton::DEFAULT];
	}
	DrawArt(button->rect.x, button->rect.y, button->art, frame, button->rect.w, button->rect.h);

	SDL_Rect text_rect = button->rect;
	if (!button->pressed) --text_rect.y;
	DrawTTF(button->text, text_rect, UIS_CENTER,
	    SDL_Color{ 243, 243, 243, 0 }, SDL_Color{ 0, 0, 0, 0 }, &button->render_cache);
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

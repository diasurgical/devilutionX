#include "DiabloUI/art_draw.h"
#include "DiabloUI/button.h"
#include "DiabloUI/errorart.h"
#include "DiabloUI/text_draw.h"
#include "utils/display.h"

namespace devilution {

Art SmlButton;

void LoadSmlButtonArt()
{
	LoadArt(&SmlButton, btnData, SML_BUTTON_WIDTH, SML_BUTTON_HEIGHT * 2, 2);
}

void RenderButton(UiButton *button)
{
	int frame;
	if (button->m_pressed) {
		frame = UiButton::PRESSED;
	} else {
		frame = UiButton::DEFAULT;
	}
	DrawArt({ button->m_rect.x, button->m_rect.y }, button->m_art, frame, button->m_rect.w, button->m_rect.h);

	SDL_Rect textRect = button->m_rect;
	if (!button->m_pressed)
		--textRect.y;

	SDL_Color color1 = { 243, 243, 243, 0 };
	SDL_Color color2 = { 0, 0, 0, 0 };
	DrawTTF(button->m_text, textRect, UIS_CENTER,
	    color1, color2, button->m_render_cache);
}

bool HandleMouseEventButton(const SDL_Event &event, UiButton *button)
{
	if (event.button.button != SDL_BUTTON_LEFT)
		return false;
	switch (event.type) {
	case SDL_MOUSEBUTTONUP:
		button->m_action();
		return true;
	case SDL_MOUSEBUTTONDOWN:
		button->m_pressed = true;
		return true;
	default:
		return false;
	}
}

void HandleGlobalMouseUpButton(UiButton *button)
{
	button->m_pressed = false;
}

} // namespace devilution

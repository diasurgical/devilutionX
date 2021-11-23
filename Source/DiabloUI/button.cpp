#include "DiabloUI/button.h"
#include "DiabloUI/art_draw.h"
#include "DiabloUI/diabloui.h"
#include "DiabloUI/errorart.h"
#include "engine/render/text_render.hpp"
#include "utils/display.h"

namespace devilution {

Art SmlButton;

void LoadSmlButtonArt()
{
	LoadArt(&SmlButton, ButtonData, SML_BUTTON_WIDTH, SML_BUTTON_HEIGHT * 2, 2);
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

	Rectangle textRect { { button->m_rect.x, button->m_rect.y }, { button->m_rect.w, button->m_rect.h } };
	if (!button->m_pressed)
		--textRect.position.y;

	const Surface &out = Surface(DiabloUiSurface());
	DrawString(out, button->m_text, textRect, UiFlags::AlignCenter | UiFlags::FontSizeDialog | UiFlags::ColorDialogWhite);
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

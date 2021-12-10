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
	DrawArt({ button->m_rect.x, button->m_rect.y }, button->GetArt(), button->GetFrame(), button->m_rect.w, button->m_rect.h);

	Rectangle textRect { { button->m_rect.x, button->m_rect.y }, { button->m_rect.w, button->m_rect.h } };
	if (!button->IsPressed()) {
		--textRect.position.y;
	}

	const Surface &out = Surface(DiabloUiSurface());
	DrawString(out, button->GetText(), textRect, UiFlags::AlignCenter | UiFlags::FontSizeDialog | UiFlags::ColorDialogWhite);
}

bool HandleMouseEventButton(const SDL_Event &event, UiButton *button)
{
	if (event.button.button != SDL_BUTTON_LEFT)
		return false;
	switch (event.type) {
	case SDL_MOUSEBUTTONUP:
		if (button->IsPressed()) {
			button->Activate();
			return true;
		}
		return false;
	case SDL_MOUSEBUTTONDOWN:
		button->Press();
		return true;
	default:
		return false;
	}
}

void HandleGlobalMouseUpButton(UiButton *button)
{
	button->Release();
}

} // namespace devilution

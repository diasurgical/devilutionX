#include "DiabloUI/button.h"

#include "DiabloUI/diabloui.h"
#include "DiabloUI/errorart.h"
#include "engine/render/pcx_render.hpp"
#include "engine/render/text_render.hpp"
#include "utils/display.h"

namespace devilution {

PcxSprite ButtonSprite(bool pressed)
{
	return PcxSprite { pressed ? ButtonPcxPressed : ButtonPcxDefault, SML_BUTTON_WIDTH, SML_BUTTON_HEIGHT };
}

void RenderButton(UiButton *button)
{
	const Surface &out = Surface(DiabloUiSurface());
	RenderPcxSprite(out, ButtonSprite(button->IsPressed()), { button->m_rect.x, button->m_rect.y });

	Rectangle textRect { { button->m_rect.x, button->m_rect.y }, { button->m_rect.w, button->m_rect.h } };
	if (!button->IsPressed()) {
		--textRect.position.y;
	}

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

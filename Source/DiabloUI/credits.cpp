#include <algorithm>
#include <memory>
#include <vector>

#include "DiabloUI/art.h"
#include "DiabloUI/art_draw.h"
#include "DiabloUI/credits_lines.h"
#include "DiabloUI/diabloui.h"
#include "DiabloUI/support_lines.h"
#include "control.h"
#include "controls/menu_controls.h"
#include "engine/render/text_render.hpp"
#include "hwcursor.hpp"
#include "utils/display.h"
#include "utils/language.h"
#include "utils/log.hpp"
#include "utils/sdl_compat.h"

namespace devilution {

namespace {

const SDL_Rect VIEWPORT = { 0, 114, 640, 251 };
const int LINE_H = 22;

char const *const *Text;
std::size_t textLines;

// The maximum number of visible lines is the number of whole lines
// (VIEWPORT.h / LINE_H) rounded up, plus one extra line for when
// a line is leaving the screen while another one is entering.
#define MAX_VISIBLE_LINES ((VIEWPORT.h - 1) / LINE_H + 2)

class CreditsRenderer {

public:
	CreditsRenderer()
	{
		ticks_begin_ = SDL_GetTicks();
		prev_offset_y_ = 0;
		finished_ = false;
	}

	~CreditsRenderer()
	{
		ArtBackgroundWidescreen.Unload();
		ArtBackground.Unload();
	}

	void Render();

	[[nodiscard]] bool Finished() const
	{
		return finished_;
	}

private:
	bool finished_;
	Uint32 ticks_begin_;
	int prev_offset_y_;
};

void CreditsRenderer::Render()
{
	const int offsetY = -VIEWPORT.h + (SDL_GetTicks() - ticks_begin_) / 40;
	if (offsetY == prev_offset_y_)
		return;
	prev_offset_y_ = offsetY;

	SDL_FillRect(DiabloUiSurface(), nullptr, 0x000000);
	DrawArt({ PANEL_LEFT - 320, UI_OFFSET_Y }, &ArtBackgroundWidescreen);
	DrawArt({ PANEL_LEFT, UI_OFFSET_Y }, &ArtBackground);

	const std::size_t linesBegin = std::max(offsetY / LINE_H, 0);
	const std::size_t linesEnd = std::min(linesBegin + MAX_VISIBLE_LINES, textLines);

	if (linesBegin >= linesEnd) {
		if (linesEnd == textLines)
			finished_ = true;
		return;
	}

	SDL_Rect viewport = VIEWPORT;
	viewport.x += PANEL_LEFT;
	viewport.y += UI_OFFSET_Y;
	ScaleOutputRect(&viewport);
	SDL_SetClipRect(DiabloUiSurface(), &viewport);

	// We use unscaled coordinates for calculation throughout.
	Sint16 destY = UI_OFFSET_Y + VIEWPORT.y - (offsetY - linesBegin * LINE_H);
	for (std::size_t i = linesBegin; i < linesEnd; ++i, destY += LINE_H) {
		Sint16 destX = PANEL_LEFT + VIEWPORT.x + 31;
		int j = 0;
		while (Text[i][j] == '\t') {
			destX += 40;
			j++;
		}

		SDL_Rect dstRect { destX, destY, 0, 0 };
		ScaleOutputRect(&dstRect);
		if (Text[i][j] != '\0') {
			const Surface &out = Surface(DiabloUiSurface());
			DrawString(out, _(Text[i]), Point { dstRect.x, dstRect.y }, UiFlags::FontSizeDialog | UiFlags::ColorDialogWhite, 0);
		}
	}
	SDL_SetClipRect(DiabloUiSurface(), nullptr);
}

bool TextDialog()
{
	CreditsRenderer creditsRenderer;
	bool endMenu = false;

	if (IsHardwareCursor())
		SetHardwareCursorVisible(false);

	SDL_Event event;
	do {
		creditsRenderer.Render();
		UiFadeIn();
		while (SDL_PollEvent(&event) != 0) {
			switch (event.type) {
			case SDL_KEYDOWN:
			case SDL_MOUSEBUTTONDOWN:
				endMenu = true;
				break;
			default:
				switch (GetMenuAction(event)) {
				case MenuAction_BACK:
				case MenuAction_SELECT:
					endMenu = true;
					break;
				default:
					break;
				}
			}
			UiHandleEvents(&event);
		}
	} while (!endMenu && !creditsRenderer.Finished());

	return true;
}

} // namespace

bool UiCreditsDialog()
{
	Text = CreditLines;
	textLines = CreditLinesSize;

	LoadArt("ui_art\\creditsw.pcx", &ArtBackgroundWidescreen);
	LoadBackgroundArt("ui_art\\credits.pcx");

	return TextDialog();
}

bool UiSupportDialog()
{
	Text = SupportLines;
	textLines = SupportLinesSize;

	if (gbIsHellfire) {
		LoadArt("ui_art\\supportw.pcx", &ArtBackgroundWidescreen);
		LoadBackgroundArt("ui_art\\support.pcx");
	} else {
		LoadArt("ui_art\\creditsw.pcx", &ArtBackgroundWidescreen);
		LoadBackgroundArt("ui_art\\credits.pcx");
	}

	return TextDialog();
}

} // namespace devilution

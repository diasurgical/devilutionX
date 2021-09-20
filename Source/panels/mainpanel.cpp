#include "panels/mainpanel.hpp"

#include "control.h"
#include "engine/render/cel_render.hpp"
#include "engine/render/text_render.hpp"
#include "utils/display.h"
#include "utils/language.h"
#include "utils/sdl_compat.h"
#include "utils/sdl_geometry.h"

namespace devilution {

Art PanelButtonDown;
Art TalkButton;

namespace {

Art PanelButton;
Art PanelButtonGrime;
Art PanelButtonDownGrime;

void DrawButtonText(const Surface &out, const char *text, Rectangle placement, UiFlags style, int spacing = 1)
{
	DrawString(out, text, { placement.position + Displacement { 0, 1 }, placement.size }, UiFlags::AlignCenter | UiFlags::KerningFitSpacing | UiFlags::ColorBlack, spacing);
	DrawString(out, text, placement, UiFlags::AlignCenter | UiFlags::KerningFitSpacing | style, spacing);
}

void DrawButtonOnPanel(Point position, const char *text, int frame)
{
	DrawArt(*pBtmBuff, position, &PanelButton, frame);
	int spacing = 2;
	int width = std::min(GetLineWidth(text, GameFont12, spacing), PanelButton.w());
	if (width > 38) {
		spacing = 1;
		width = std::min(GetLineWidth(text, GameFont12, spacing), PanelButton.w());
	}
	DrawArt(*pBtmBuff, position + Displacement { (PanelButton.w() - width) / 2, 7 }, &PanelButtonGrime, frame, width);
	DrawButtonText(*pBtmBuff, text, { position, { PanelButton.w(), 0 } }, UiFlags::ColorButtonface, spacing);
}

void RenderMainButton(int buttonId, const char *text, int frame)
{
	Point panelPosition { PanBtnPos[buttonId].x + 4, PanBtnPos[buttonId].y + 17 };
	DrawButtonOnPanel(panelPosition, text, frame);
	if (IsChatAvailable())
		DrawButtonOnPanel(panelPosition + Displacement { 0, PANEL_HEIGHT + 16 }, text, frame);

	const Surface out(PanelButtonDown.surface.get());
	Point position { 0, 19 * buttonId };
	int spacing = 2;
	int width = std::min(GetLineWidth(text, GameFont12, spacing), PanelButton.w());
	if (width > 38) {
		spacing = 1;
		width = std::min(GetLineWidth(text, GameFont12, spacing), PanelButton.w());
	}
	DrawArt(out, position + Displacement { (PanelButton.w() - width) / 2, 9 }, &PanelButtonDownGrime, frame, width);
	DrawButtonText(out, text, { position + Displacement { 0, 2 }, { PanelButtonDown.w(), 0 } }, UiFlags::ColorButtonpushed, spacing);
}

void DrawTalkButton(int buttonId)
{
	const char *text = _("voice");
	Point position { 176, PANEL_HEIGHT + 101 + 18 * buttonId };
	DrawArt(*pBtmBuff, position, &TalkButton);
	int width = std::min(GetLineWidth(text, GameFont12, 1), PanelButton.w());
	DrawArt(*pBtmBuff, position + Displacement { (TalkButton.w() - width) / 2, 6 }, &PanelButtonGrime, 1, width, 9);
	DrawButtonText(*pBtmBuff, text, { position, { TalkButton.w(), 0 } }, UiFlags::ColorButtonface);
}

} // namespace

void LoadMainPanel()
{
	LoadArt("data\\panel8buc.pcx", &PanelButton, 2);
	LoadArt("data\\dirtybuc.pcx", &PanelButtonGrime, 2);
	LoadArt("data\\panel8bucp.pcx", &PanelButtonDown, 6);
	LoadArt("data\\dirtybucp.pcx", &PanelButtonDownGrime, 2);

	// Load palette to render targets
	UpdatePalette(&PanelButtonDown);
	if (SDLC_SetSurfaceColors(pBtmBuff->surface, PalSurface->format->palette) <= -1)
		ErrSdl();

	RenderMainButton(0, _("char"), 0);
	RenderMainButton(1, _("quests"), 1);
	RenderMainButton(2, _("map"), 1);
	RenderMainButton(3, _("menu"), 0);
	RenderMainButton(4, _("inv"), 1);
	RenderMainButton(5, _("spells"), 0);

	if (IsChatAvailable()) {
		LoadArt("data\\talkbutton.pcx", &TalkButton, 3);
		UpdatePalette(&TalkButton);

		// Must be done before adding the text to TalkButton
		DrawTalkButton(0);
		DrawTalkButton(1);
		DrawTalkButton(2);

		const Surface talkSurface(TalkButton.surface.get());
		int muteWidth = GetLineWidth(_("mute"), GameFont12, 2);
		DrawArt(talkSurface, { (TalkButton.w() - muteWidth) / 2, 6 }, &PanelButtonGrime, 1, muteWidth, 9);
		DrawButtonText(talkSurface, _("mute"), { { 0, 0 }, { TalkButton.w(), 0 } }, UiFlags::ColorButtonface);
		DrawArt(talkSurface, { (TalkButton.w() - muteWidth) / 2, 23 }, &PanelButtonGrime, 1, muteWidth, 9);
		DrawButtonText(talkSurface, _("mute"), { { 0, 17 }, { TalkButton.w(), 0 } }, UiFlags::ColorButtonpushed);
		int voiceWidth = GetLineWidth(_("voice"), GameFont12, 2);
		DrawArt(talkSurface, { (TalkButton.w() - voiceWidth) / 2, 39 }, &PanelButtonGrime, 1, voiceWidth, 9);
		DrawButtonText(talkSurface, _("voice"), { { 0, 33 }, { TalkButton.w(), 0 } }, UiFlags::ColorButtonpushed);
	}

	UnloadFonts(GameFont12, ColorButtonface);
	UnloadFonts(GameFont12, ColorButtonpushed);

	PanelButton.Unload();
	PanelButtonGrime.Unload();
	PanelButtonDownGrime.Unload();
}

void FreeMainPanel()
{
	PanelButtonDown.Unload();
	TalkButton.Unload();
}

} // namespace devilution

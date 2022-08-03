#include "panels/mainpanel.hpp"

#include "control.h"
#include "engine/clx_sprite.hpp"
#include "engine/load_clx.hpp"
#include "engine/render/clx_render.hpp"
#include "engine/render/text_render.hpp"
#include "utils/display.h"
#include "utils/language.h"
#include "utils/sdl_compat.h"
#include "utils/sdl_geometry.h"

namespace devilution {

Art PanelButtonDown;
Art TalkButton;

namespace {

OptionalOwnedClxSpriteList PanelButton;
OptionalOwnedClxSpriteList PanelButtonGrime;
OptionalOwnedClxSpriteList PanelButtonDownGrime;

void DrawButtonText(const Surface &out, string_view text, Rectangle placement, UiFlags style, int spacing = 1)
{
	DrawString(out, text, { placement.position + Displacement { 0, 1 }, placement.size }, UiFlags::AlignCenter | UiFlags::KerningFitSpacing | UiFlags::ColorBlack, spacing);
	DrawString(out, text, placement, UiFlags::AlignCenter | UiFlags::KerningFitSpacing | style, spacing);
}

void DrawButtonOnPanel(Point position, string_view text, int frame)
{
	RenderClxSprite(*pBtmBuff, (*PanelButton)[frame], position);
	int spacing = 2;
	int width = std::min<int>(GetLineWidth(text, GameFont12, spacing), (*PanelButton)[0].width());
	if (width > 38) {
		spacing = 1;
		width = std::min<int>(GetLineWidth(text, GameFont12, spacing), (*PanelButton)[0].width());
	}
	RenderClxSprite(pBtmBuff->subregion(position.x + ((*PanelButton)[0].width() - width) / 2, position.y + 7, width, pBtmBuff->h() - 7), (*PanelButtonGrime)[frame], { 0, 0 });
	DrawButtonText(*pBtmBuff, text, { position, { (*PanelButton)[0].width(), 0 } }, UiFlags::ColorButtonface, spacing);
}

void RenderMainButton(int buttonId, string_view text, int frame)
{
	Point panelPosition { PanBtnPos[buttonId].x + 4, PanBtnPos[buttonId].y + 17 };
	DrawButtonOnPanel(panelPosition, text, frame);
	if (IsChatAvailable())
		DrawButtonOnPanel(panelPosition + Displacement { 0, GetMainPanel().size.height + 16 }, text, frame);

	const Surface out(PanelButtonDown.surface.get());
	Point position { 0, 19 * buttonId };
	int spacing = 2;
	int width = std::min<int>(GetLineWidth(text, GameFont12, spacing), (*PanelButton)[0].width());
	if (width > 38) {
		spacing = 1;
		width = std::min<int>(GetLineWidth(text, GameFont12, spacing), (*PanelButton)[0].width());
	}
	RenderClxSprite(out.subregion(position.x + ((*PanelButton)[0].width() - width) / 2, position.y + 9, width, out.h() - position.y - 9), (*PanelButtonDownGrime)[frame], { 0, 0 });
	DrawButtonText(out, text, { position + Displacement { 0, 2 }, { PanelButtonDown.w(), 0 } }, UiFlags::ColorButtonpushed, spacing);
}

void DrawTalkButton(int buttonId)
{
	string_view text = _("voice");
	Point position { 176, GetMainPanel().size.height + 101 + 18 * buttonId };
	DrawArt(*pBtmBuff, position, &TalkButton);
	int width = std::min<int>(GetLineWidth(text, GameFont12, 1), (*PanelButton)[0].width());
	RenderClxSprite(pBtmBuff->subregion(position.x + (TalkButton.w() - width) / 2, position.y + 6, width, 9), (*PanelButtonGrime)[1], { 0, 0 });
	DrawButtonText(*pBtmBuff, text, { position, { TalkButton.w(), 0 } }, UiFlags::ColorButtonface);
}

} // namespace

void LoadMainPanel()
{
	LoadArt("data\\panel8bucp.pcx", &PanelButtonDown, 6);
	PanelButton = LoadClx("data\\panel8buc.clx");
	PanelButtonGrime = LoadClx("data\\dirtybuc.clx");
	PanelButtonDownGrime = LoadClx("data\\dirtybucp.clx");

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
		RenderClxSprite(talkSurface.subregion((TalkButton.w() - muteWidth) / 2, 6, muteWidth, 9), (*PanelButtonGrime)[1], { 0, 0 });
		DrawButtonText(talkSurface, _("mute"), { { 0, 0 }, { TalkButton.w(), 0 } }, UiFlags::ColorButtonface);
		RenderClxSprite(talkSurface.subregion((TalkButton.w() - muteWidth) / 2, 23, muteWidth, 9), (*PanelButtonGrime)[1], { 0, 0 });
		DrawButtonText(talkSurface, _("mute"), { { 0, 17 }, { TalkButton.w(), 0 } }, UiFlags::ColorButtonpushed);
		int voiceWidth = GetLineWidth(_("voice"), GameFont12, 2);
		RenderClxSprite(talkSurface.subregion((TalkButton.w() - voiceWidth) / 2, 39, voiceWidth, 9), (*PanelButtonGrime)[1], { 0, 0 });
		DrawButtonText(talkSurface, _("voice"), { { 0, 33 }, { TalkButton.w(), 0 } }, UiFlags::ColorButtonpushed);
	}

	UnloadFonts(GameFont12, ColorButtonface);
	UnloadFonts(GameFont12, ColorButtonpushed);

	PanelButtonDownGrime = std::nullopt;
	PanelButtonGrime = std::nullopt;
	PanelButton = std::nullopt;
}

void FreeMainPanel()
{
	PanelButtonDown.Unload();
	TalkButton.Unload();
}

} // namespace devilution

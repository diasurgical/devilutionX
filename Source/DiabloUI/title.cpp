#include "DiabloUI/diabloui.h"
#include "control.h"
#include "controls/menu_controls.h"
#include "discord/discord.h"
#include "main_loop.hpp"
#include "utils/language.h"
#include "utils/sdl_geometry.h"

namespace devilution {
namespace {

class TitleDialog : public MainLoopHandler {
public:
	TitleDialog()
	{
		if (gbIsHellfire) {
			SDL_Rect rect = MakeSdlRect(0, UI_OFFSET_Y, 0, 0);
			items_.push_back(std::make_unique<UiImage>(&ArtBackgroundWidescreen, rect, UiFlags::AlignCenter, /*bAnimated=*/true));
			items_.push_back(std::make_unique<UiImage>(&ArtBackground, rect, UiFlags::AlignCenter, /*bAnimated=*/true));
			LoadBackgroundArt("ui_art\\hf_logo1.pcx", 16);
			LoadArt("ui_art\\hf_titlew.pcx", &ArtBackgroundWidescreen);
		} else {
			UiAddBackground(&items_);
			UiAddLogo(&items_, LOGO_BIG, 182);

			SDL_Rect rect = MakeSdlRect(PANEL_LEFT, UI_OFFSET_Y + 410, 640, 26);
			items_.push_back(std::make_unique<UiArtText>(_("Copyright © 1996-2001 Blizzard Entertainment"), rect, UiFlags::AlignCenter | UiFlags::FontSize24 | UiFlags::ColorUiSilver));
			LoadBackgroundArt("ui_art\\title.pcx");
			LoadMaskedArt("ui_art\\logo.pcx", &ArtLogos[LOGO_BIG], 15);
		}

		timeout_ = SDL_GetTicks() + 7000;
	}

	~TitleDialog() override
	{
		ArtBackground.Unload();
		ArtBackgroundWidescreen.Unload();
		ArtLogos[LOGO_BIG].Unload();
	}

	void HandleEvent(SDL_Event &event) override
	{
		if (GetMenuAction(event) != MenuAction_NONE) {
			NextMainLoopHandler();
			return;
		}
		switch (event.type) {
		case SDL_KEYDOWN:
		case SDL_MOUSEBUTTONDOWN:
			NextMainLoopHandler();
			return;
		}
		UiHandleEvents(&event);
	}

	void Render() override
	{
		if (SDL_GetTicks() >= timeout_) {
			NextMainLoopHandler();
			return;
		}
		UiRenderItems(items_);
		UiFadeIn();
		discord_manager::UpdateMenu();
	}

private:
	std::vector<std::unique_ptr<UiItemBase>> items_;
	Uint32 timeout_;
};

} // namespace

void UiTitleDialog()
{
	SetMainLoopHandler(std::make_unique<TitleDialog>());
}

} // namespace devilution

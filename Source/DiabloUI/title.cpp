#include "DiabloUI/diabloui.h"
#include "control.h"
#include "controls/input.h"
#include "controls/menu_controls.h"
#include "discord/discord.h"
#include "engine/load_pcx.hpp"
#include "engine/load_pcx_as_cel.hpp"
#include "utils/language.h"

namespace devilution {
namespace {

std::vector<std::unique_ptr<UiItemBase>> vecTitleScreen;

void TitleLoad()
{
	if (gbIsHellfire) {
		LoadBackgroundArt("ui_art\\hf_logo1.pcx", 16);
		ArtBackgroundWidescreen = LoadPcxAsset("ui_art\\hf_titlew.pcx");
	} else {
		LoadBackgroundArt("ui_art\\title.pcx");
		ArtLogos[LOGO_BIG] = LoadPcxAssetAsCel("ui_art\\logo.pcx", /*numFrames=*/15, /*generateFrameHeaders=*/false, /*transparentColorIndex=*/250);
	}
}

void TitleFree()
{
	ArtBackground = std::nullopt;
	ArtBackgroundWidescreen = std::nullopt;
	ArtLogos[LOGO_BIG] = std::nullopt;

	vecTitleScreen.clear();
}

} // namespace

void UiTitleDialog()
{
	TitleLoad();
	const Point uiPosition = GetUIRectangle().position;
	if (gbIsHellfire) {
		SDL_Rect rect = { 0, uiPosition.y, 0, 0 };
		if (ArtBackgroundWidescreen)
			vecTitleScreen.push_back(std::make_unique<UiImagePcx>(PcxSprite { *ArtBackgroundWidescreen }, rect, UiFlags::AlignCenter));
		vecTitleScreen.push_back(std::make_unique<UiImageAnimatedPcx>(PcxSpriteSheet { *ArtBackground }, rect, UiFlags::AlignCenter));
	} else {
		UiAddBackground(&vecTitleScreen);
		UiAddLogo(&vecTitleScreen, LOGO_BIG, 182);

		SDL_Rect rect = { (Sint16)(uiPosition.x), (Sint16)(uiPosition.y + 410), 640, 26 };
		vecTitleScreen.push_back(std::make_unique<UiArtText>(_("Copyright © 1996-2001 Blizzard Entertainment").c_str(), rect, UiFlags::AlignCenter | UiFlags::FontSize24 | UiFlags::ColorUiSilver));
	}

	bool endMenu = false;
	Uint32 timeOut = SDL_GetTicks() + 7000;

	SDL_Event event;
	while (!endMenu && SDL_GetTicks() < timeOut) {
		UiRenderItems(vecTitleScreen);
		UiFadeIn();

		discord_manager::UpdateMenu();

		while (PollEvent(&event) != 0) {
			if (GetMenuAction(event) != MenuAction_NONE) {
				endMenu = true;
				break;
			}
			switch (event.type) {
			case SDL_KEYDOWN:
			case SDL_MOUSEBUTTONUP:
				endMenu = true;
				break;
			}
			UiHandleEvents(&event);
		}
	}

	TitleFree();
}

} // namespace devilution

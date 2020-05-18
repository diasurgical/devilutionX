#include "all.h"
#include "controls/menu_controls.h"
#include "DiabloUI/diabloui.h"

namespace dvl {

std::vector<UiItemBase*> vecTitleScreen;

void title_Load()
{
	LoadBackgroundArt("ui_art\\title.pcx");
	LoadMaskedArt("ui_art\\logo.pcx", &ArtLogos[LOGO_BIG], 15);
}

void title_Free()
{
	ArtBackground.Unload();
	ArtLogos[LOGO_BIG].Unload();

	for(std::size_t i = 0; i < vecTitleScreen.size(); i++)
	{
		UiItemBase* pUIItem = vecTitleScreen[i];
		if(pUIItem)
			delete pUIItem;

		vecTitleScreen.clear();
	}
}

void UiTitleDialog()
{
	SDL_Rect rect1 = { PANEL_LEFT, 0, 640, 480 };
	vecTitleScreen.push_back(new UiImage(&ArtBackground, rect1));

	SDL_Rect rect2 = { 0, 182, 0, 0 };
	vecTitleScreen.push_back(new UiImage(&ArtLogos[LOGO_BIG], /*animated=*/true, /*frame=*/0, rect2, UIS_CENTER));

	SDL_Rect rect3 = { PANEL_LEFT + 49, 410, 550, 26 };
	vecTitleScreen.push_back(new UiArtText("Copyright \xA9 1996-2001 Blizzard Entertainment", rect3, UIS_MED | UIS_CENTER));

	title_Load();

	bool endMenu = false;
	Uint32 timeOut = SDL_GetTicks() + 7000;

	SDL_Event event;
	while (!endMenu && SDL_GetTicks() < timeOut) {
		UiRenderItems(vecTitleScreen);
		UiFadeIn();

		while (SDL_PollEvent(&event)) {
			if (GetMenuAction(event) != MenuActionNS::NONE) {
				endMenu = true;
				break;
			}
			switch (event.type) {
			case SDL_KEYDOWN: /* To match the original uncomment this
				if (event.key.keysym.sym == SDLK_UP
				    || event.key.keysym.sym == SDLK_UP
				    || event.key.keysym.sym == SDLK_LEFT
				    || event.key.keysym.sym == SDLK_RIGHT) {
					break;
				}*/
			case SDL_MOUSEBUTTONDOWN:
				endMenu = true;
				break;
			}
			UiHandleEvents(&event);
		}
	}

	title_Free();
}

void UiSetSpawned(BOOL bSpawned)
{
	gbSpawned = bSpawned;
}

} // namespace dvl

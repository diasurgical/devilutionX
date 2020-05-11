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

	for(int i = 0; i < (int)vecTitleScreen.size(); i++)
	{
		UiItemBase* pUIItem = vecTitleScreen[i];
		if(pUIItem)
			delete pUIItem;

		vecTitleScreen.clear();
	}
}

void UiTitleDialog()
{
	SDL_Rect rect1 = {0, 0, SCREEN_WIDTH, SCREEN_HEIGHT};
	vecTitleScreen.push_back(new UiImage(&ArtBackground, rect1));
	
	SDL_Rect rect2 = { 0, 182, 0, 0 };
	vecTitleScreen.push_back(new UiImage(&ArtLogos[LOGO_BIG], /*animated=*/true, /*frame=*/0, rect2, UIS_CENTER));

	title_Load();

	bool endMenu = false;
	Uint32 timeOut = SDL_GetTicks() + 7000;

	SDL_Event event;
	while (!endMenu && SDL_GetTicks() < timeOut) {
		UiRenderItems(vecTitleScreen, vecTitleScreen.size());
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

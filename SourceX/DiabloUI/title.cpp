#include "all.h"
#include "controls/menu_controls.h"
#include "DiabloUI/diabloui.h"

namespace dvl {

void title_Load()
{
	LoadBackgroundArt("ui_art\\title.pcx");
	LoadMaskedArt("ui_art\\logo.pcx", &ArtLogos[LOGO_BIG], 15);
}

void title_Free()
{
	ArtBackground.Unload();
	ArtLogos[LOGO_BIG].Unload();
}

void UiTitleDialog()
{
	UiItem TITLESCREEN_DIALOG[] = {
		MAINMENU_BACKGROUND,
		UiImage(&ArtLogos[LOGO_BIG], /*animated=*/true, /*frame=*/0, { 0, 182, 0, 0 }, UIS_CENTER),
		UiArtText("Copyright \xA9 1996-2001 Blizzard Entertainment", { PANEL_LEFT + 49, 410, 550, 26 }, UIS_MED | UIS_CENTER)
	};

	title_Load();

	bool endMenu = false;
	Uint32 timeOut = SDL_GetTicks() + 7000;

	SDL_Event event;
	while (!endMenu && SDL_GetTicks() < timeOut) {
		UiRenderItems(TITLESCREEN_DIALOG, size(TITLESCREEN_DIALOG));
		UiFadeIn();

		while (SDL_PollEvent(&event)) {
			if (GetMenuAction(event) != MenuAction_NONE) {
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

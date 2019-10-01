#include "devilution.h"
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

BOOL UiTitleDialog(int a1)
{
	UiItem TITLESCREEN_DIALOG[] = {
		UiImage(&ArtBackground, { 0, 0, 640, 480 }),
		UiText("Copyright \xA9 1996-2001 Blizzard Entertainment", { 49, 410, 550, 26 }, UIS_MED | UIS_CENTER)
	};

	title_Load();

	bool endMenu = false;
	Uint32 timeOut = SDL_GetTicks() + 7000;

	SDL_Event event;
	while (!endMenu && SDL_GetTicks() < timeOut) {
		UiRenderItems(TITLESCREEN_DIALOG, size(TITLESCREEN_DIALOG));
		DrawLogo(182, LOGO_BIG);
		UiFadeIn();

		while (SDL_PollEvent(&event)) {
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
			case SDL_QUIT:
				exit(0);
			}
		}
	}
	BlackPalette();

	title_Free();

	return true;
}

void UiSetSpawned(BOOL bSpawned)
{
	gbSpawned = bSpawned;
}

}

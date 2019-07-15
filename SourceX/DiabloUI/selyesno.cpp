#include "selyesno.h"

#include "diablo.h"
#include "diabloui.h"

namespace dvl {

_uiheroinfo selyesno_heroInfo;
int selyesno_endMenu;
BOOL(*selyesno_gfnRemove)
(_uiheroinfo *);
char selyesno_confirmationMessage[256];
char selyesno_title[32];

UI_Item DEL_SELYESNO_DIALOG[] = {
	{ { 0, 0, 640, 480 }, UI_IMAGE, 0, 0, NULL, &ArtBackground },
	{ { 24, 161, 590, 35 }, UI_TEXT, UIS_CENTER | UIS_BIG, 0, selyesno_title },
	{ { 100, 230, 280, 270 }, UI_TEXT, UIS_BIG, 0, selyesno_confirmationMessage },
	{ { 230, 390, 180, 35 }, UI_LIST, UIS_CENTER | UIS_BIG | UIS_GOLD, 0, "Yes" },
	{ { 230, 426, 180, 35 }, UI_LIST, UIS_CENTER | UIS_BIG | UIS_GOLD, 1, "No" },
};

void selyesno_Free()
{
	mem_free_dbg(ArtBackground.data);
	ArtBackground.data = NULL;
}

void selyesno_Select(int value)
{
	if (value == 0)
		selyesno_gfnRemove(&selyesno_heroInfo);

	selyesno_endMenu = true;
}

void selyesno_Esc()
{
	selyesno_endMenu = true;
}

BOOL UiSelHeroDelYesNoDialog(
    BOOL (*fnremove)(_uiheroinfo *),
    _uiheroinfo *selectHero,
    bool isMultiplayer)
{
	selyesno_heroInfo = *selectHero;
	selyesno_gfnRemove = fnremove;
	LoadBackgroundArt("ui_art\\black.pcx");

	if (isMultiplayer) {
		sprintf(selyesno_title, "Delete Multiplayer Hero");
	} else {
		sprintf(selyesno_title, "Delete Single Player Hero");
	}

	sprintf(selyesno_confirmationMessage, "Are you sure you want to delete the character \"%s\"?", selyesno_heroInfo.name);
	WordWrap(&DEL_SELYESNO_DIALOG[2]);

	UiInitList(0, 1, NULL, selyesno_Select, selyesno_Esc, DEL_SELYESNO_DIALOG, size(DEL_SELYESNO_DIALOG), false, NULL);

	// forcing the default selection to be "No",
	// remove this code to be consistent with
	// the retail v 1.09 game
	SDL_Event sdlevent = {};
	sdlevent.type = SDL_KEYDOWN;
	sdlevent.key.keysym.sym = SDLK_DOWN;
	SDL_PushEvent(&sdlevent);

	selyesno_endMenu = false;
	while (!selyesno_endMenu) {
		UiRenderItems(DEL_SELYESNO_DIALOG, size(DEL_SELYESNO_DIALOG));
		UiRender();
	}

	BlackPalette();

	selyesno_Free();
	return true;
}
}

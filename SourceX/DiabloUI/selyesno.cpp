#include "selyesno.h"

#include "devilution.h"
#include "DiabloUI/diabloui.h"
#include "DiabloUI/text.h"

namespace dvl {

_uiheroinfo selyesno_heroInfo;
int selyesno_endMenu;
BOOL(*selyesno_gfnRemove)
(_uiheroinfo *);
char selyesno_confirmationMessage[256];
char selyesno_title[32];

UiListItem SELYESNO_DIALOG_ITEMS[] = {
	{ "Yes", 0 },
	{ "No", 1 }
};

UiItem SELYESNO_DIALOG[] = {
	MAINMENU_BACKGROUND,
	MAINMENU_LOGO,
	UiArtText(selyesno_title, { 24, 161, 590, 35 }, UIS_CENTER | UIS_BIG),
	UiArtText(selyesno_confirmationMessage, { 120, 236, 280, 168 }, UIS_MED),
	UiList(SELYESNO_DIALOG_ITEMS, 230, 390, 180, 35, UIS_CENTER | UIS_BIG | UIS_GOLD)
};
UiArtText *SELYESNO_DIALOG_CONFIRMATION_MESSAGE = &SELYESNO_DIALOG[3].art_text;

void selyesno_Free()
{
	ArtBackground.Unload();
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
		strcpy(selyesno_title, "Delete Multi Player Hero");
	} else {
		strcpy(selyesno_title, "Delete Single Player Hero");
	}

	sprintf(selyesno_confirmationMessage, "Are you sure you want to delete the character \"%s\"?", selyesno_heroInfo.name);
	WordWrapArtStr(selyesno_confirmationMessage, SELYESNO_DIALOG_CONFIRMATION_MESSAGE->rect.w);

	UiInitList(0, 1, NULL, selyesno_Select, selyesno_Esc, SELYESNO_DIALOG, size(SELYESNO_DIALOG), true, NULL);

	selyesno_endMenu = false;
	while (!selyesno_endMenu) {
		UiRenderItems(SELYESNO_DIALOG, size(SELYESNO_DIALOG));
		UiPollAndRender();
	}

	BlackPalette();

	selyesno_Free();
	return true;
}
}

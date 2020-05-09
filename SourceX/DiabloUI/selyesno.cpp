#include "selyesno.h"

#include "all.h"
#include "DiabloUI/diabloui.h"
#include "DiabloUI/text.h"

namespace dvl {

bool selyesno_endMenu;
bool selyesno_value;
char selyesno_confirmationMessage[256];
char selyesno_title[32];

UiListItem SELYESNO_DIALOG_ITEMS[] = {
	{ "Yes", 0 },
	{ "No", 1 }
};

UiItem SELYESNO_DIALOG[] = {
	MAINMENU_BACKGROUND,
	MAINMENU_LOGO,
	UiArtText(selyesno_title, { PANEL_LEFT + 24, 161, 590, 35 }, UIS_CENTER | UIS_BIG),
	UiArtText(selyesno_confirmationMessage, { PANEL_LEFT + 120, 236, 280, 168 }, UIS_MED),
	UiList(SELYESNO_DIALOG_ITEMS, PANEL_LEFT + 230, 390, 180, 35, UIS_CENTER | UIS_BIG | UIS_GOLD)
};
UiArtText *SELYESNO_DIALOG_CONFIRMATION_MESSAGE = &SELYESNO_DIALOG[3].art_text;

void selyesno_Free()
{
	ArtBackground.Unload();
}

void selyesno_Select(int value)
{
	selyesno_value = value == 0;
	selyesno_endMenu = true;
}

void selyesno_Esc()
{
	selyesno_value = false;
	selyesno_endMenu = true;
}

bool UiSelHeroYesNoDialog(const char *title, const char *body)
{
	LoadBackgroundArt("ui_art\\black.pcx");

	strcpy(selyesno_title, title);
	strcpy(selyesno_confirmationMessage, body);
	WordWrapArtStr(selyesno_confirmationMessage, SELYESNO_DIALOG_CONFIRMATION_MESSAGE->rect.w);

	UiInitList(0, 1, NULL, selyesno_Select, selyesno_Esc, SELYESNO_DIALOG, size(SELYESNO_DIALOG), true, NULL);

	selyesno_value = true;
	selyesno_endMenu = false;
	while (!selyesno_endMenu) {
		UiClearScreen();
		UiRenderItems(SELYESNO_DIALOG, size(SELYESNO_DIALOG));
		UiPollAndRender();
	}

	selyesno_Free();

	return selyesno_value;
}
}

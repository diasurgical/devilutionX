#include "selyesno.h"

#include "all.h"
#include "DiabloUI/diabloui.h"
#include "DiabloUI/text.h"

namespace dvl {

bool selyesno_endMenu;
bool selyesno_value;
char selyesno_confirmationMessage[256];

std::vector<UiListItem *> vecSelYesNoDialogItems;
std::vector<UiItemBase *> vecSelYesNoDialog;

#define MESSAGE_WIDTH 280

void selyesno_Free()
{
	ArtBackground.Unload();

	for (std::size_t i = 0; i < vecSelYesNoDialogItems.size(); i++) {
		UiListItem *pUIListItem = vecSelYesNoDialogItems[i];
		if (pUIListItem)
			delete pUIListItem;
	}
	vecSelYesNoDialogItems.clear();

	for (std::size_t i = 0; i < vecSelYesNoDialog.size(); i++) {
		UiItemBase *pUIItem = vecSelYesNoDialog[i];
		delete pUIItem;
	}
	vecSelYesNoDialog.clear();
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
	UiAddBackground(&vecSelYesNoDialog);
	UiAddLogo(&vecSelYesNoDialog);

	SDL_Rect rect1 = { PANEL_LEFT + 24, 161, 590, 35 };
	vecSelYesNoDialog.push_back(new UiArtText(title, rect1, UIS_CENTER | UIS_BIG));

	SDL_Rect rect2 = { PANEL_LEFT + 120, 236, MESSAGE_WIDTH, 168 };
	vecSelYesNoDialog.push_back(new UiArtText(selyesno_confirmationMessage, rect2, UIS_MED));

	vecSelYesNoDialogItems.push_back(new UiListItem("Yes", 0));
	vecSelYesNoDialogItems.push_back(new UiListItem("No", 1));
	vecSelYesNoDialog.push_back(new UiList(vecSelYesNoDialogItems, PANEL_LEFT + 230, 390, 180, 35, UIS_CENTER | UIS_BIG | UIS_GOLD));

	strcpy(selyesno_confirmationMessage, body);
	WordWrapArtStr(selyesno_confirmationMessage, MESSAGE_WIDTH);

	UiInitList(0, 1, NULL, selyesno_Select, selyesno_Esc, vecSelYesNoDialog, true, NULL);

	selyesno_value = true;
	selyesno_endMenu = false;
	while (!selyesno_endMenu) {
		UiClearScreen();
		UiRenderItems(vecSelYesNoDialog);
		UiPollAndRender();
	}

	selyesno_Free();

	return selyesno_value;
}
} // namespace dvl

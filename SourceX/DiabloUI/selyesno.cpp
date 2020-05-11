#include "selyesno.h"

#include "all.h"
#include "DiabloUI/diabloui.h"
#include "DiabloUI/text.h"

namespace dvl {

bool selyesno_endMenu;
bool selyesno_value;
char selyesno_confirmationMessage[256];
char selyesno_title[32];

std::vector<UiListItem*> vecSelYesNoDialogItems;
std::vector<UiItemBase*> vecSelYesNoDialog;
UiArtText *SELYESNO_DIALOG_CONFIRMATION_MESSAGE;

void selyesno_Free()
{
	ArtBackground.Unload();

	for(int i = 0; i < (int)vecSelYesNoDialogItems.size(); i++)
	{
		UiListItem* pUIListItem = vecSelYesNoDialogItems[i];
		if(pUIListItem)
			delete pUIListItem;

		vecSelYesNoDialogItems.clear();
	}

	for(int i = 0; i < (int)vecSelYesNoDialog.size(); i++)
	{
		UiItemBase* pUIItem = vecSelYesNoDialog[i];
		if(pUIItem)
			delete pUIItem;

		vecSelYesNoDialog.clear();
	}
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

	vecSelYesNoDialogItems.push_back(new UiListItem( "Yes", 0 ));
	vecSelYesNoDialogItems.push_back(new UiListItem( "No", 1 ));

	SDL_Rect rect = {0, 0, SCREEN_WIDTH, SCREEN_HEIGHT};
	vecSelYesNoDialog.push_back(new UiImage(&ArtBackground, rect));
	
	SDL_Rect rect2 = { 0, 0, 0, 0 };
	vecSelYesNoDialog.push_back(new UiImage(&ArtLogos[LOGO_MED], /*animated=*/true, /*frame=*/0, rect2, UIS_CENTER));

	SDL_Rect rect3 = { 24, 161, 590, 35 };
	vecSelYesNoDialog.push_back(new UiArtText(selyesno_title, rect3, UIS_CENTER | UIS_BIG));

	SDL_Rect rect4 = { 120, 236, 280, 168 };
	SELYESNO_DIALOG_CONFIRMATION_MESSAGE = new UiArtText(selyesno_confirmationMessage, rect4, UIS_MED);
	vecSelYesNoDialog.push_back(SELYESNO_DIALOG_CONFIRMATION_MESSAGE);

	vecSelYesNoDialog.push_back(new UiList(vecSelYesNoDialogItems, 230, 390, 180, 35, UIS_CENTER | UIS_BIG | UIS_GOLD));

	strcpy(selyesno_title, title);
	strcpy(selyesno_confirmationMessage, body);
	WordWrapArtStr(selyesno_confirmationMessage, SELYESNO_DIALOG_CONFIRMATION_MESSAGE->m_rect.w);

	UiInitList(0, 1, NULL, selyesno_Select, selyesno_Esc, vecSelYesNoDialog, vecSelYesNoDialog.size(), true, NULL);

	selyesno_value = true;
	selyesno_endMenu = false;
	while (!selyesno_endMenu) {
		UiRenderItems(vecSelYesNoDialog, vecSelYesNoDialog.size());
		UiPollAndRender();
	}

	selyesno_Free();

	return selyesno_value;
}
}

#include "all.h"
#include "DiabloUI/diabloui.h"
#include "DiabloUI/text.h"
#include "DiabloUI/selok.h"

namespace dvl {

namespace {

char dialogText[256];

} // namespace

int selok_endMenu;
char selok_title[32];

std::vector<UiListItem*> vecSelOkDialogItems;
std::vector<UiItemBase*> vecSelOkDialog;

void selok_Free()
{
	ArtBackground.Unload();

	for(std::size_t i = 0; i < vecSelOkDialogItems.size(); i++)
	{
		UiListItem* pUIListItem = vecSelOkDialogItems[i];
		if(pUIListItem)
			delete pUIListItem;
	}
	vecSelOkDialogItems.clear();

	for(std::size_t i = 0; i < vecSelOkDialog.size(); i++)
	{
		UiItemBase* pUIItem = vecSelOkDialog[i];
		if(pUIItem)
			delete pUIItem;
	}
	vecSelOkDialog.clear();
}

void selok_Select(int value)
{
	selok_endMenu = true;
}

void selok_Esc()
{
	selok_endMenu = true;
}

void UiSelOkDialog(const char *title, const char *body, bool background)
{
	SDL_Rect rect;

	if (!background) {
		LoadBackgroundArt("ui_art\\black.pcx");
	} else {
		if (!gbSpawned) {
			LoadBackgroundArt("ui_art\\mainmenu.pcx");
		} else {
			LoadBackgroundArt("ui_art\\swmmenu.pcx");
		}
	}

	UiAddBackground(&vecSelOkDialog);

	if (!gbSpawned) {
		rect = { 0, 0, 0, 0 };
		vecSelOkDialog.push_back(new UiImage(&ArtLogos[LOGO_MED], /*animated=*/true, /*frame=*/0, rect, UIS_CENTER));

		rect = { PANEL_LEFT + 24, 161, 590, 35 };
		vecSelOkDialog.push_back(new UiArtText(selok_title, rect, UIS_CENTER | UIS_BIG));
	} else {
		rect = { 0, 182, 0, 0 };
		vecSelOkDialog.push_back(new UiImage(&ArtLogos[LOGO_BIG], /*animated=*/true, /*frame=*/0, rect, UIS_CENTER));
	}

	rect = { PANEL_LEFT + 140, 210, 560, 168 };
	vecSelOkDialog.push_back(new UiArtText(dialogText, rect, UIS_MED));

	vecSelOkDialogItems.push_back(new UiListItem("OK", 0));
	vecSelOkDialog.push_back(new UiList(vecSelOkDialogItems, PANEL_LEFT + 230, 390, 180, 35, UIS_CENTER | UIS_BIG | UIS_GOLD));

	if (title != NULL) {
		strcpy(selok_title, title);
	}

	strcpy(dialogText, body);
	WordWrapArtStr(dialogText, 280);

	UiInitList(0, 0, NULL, selok_Select, selok_Esc, vecSelOkDialog, false, NULL);

	selok_endMenu = false;
	while (!selok_endMenu) {
		UiClearScreen();
		UiRenderItems(vecSelOkDialog);
		UiPollAndRender();
	}

	selok_Free();
}
} // namespace dvl

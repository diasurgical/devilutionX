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
std::vector<UiItemBase*> vecSpawnErrorOkDialog;

void selok_Free()
{
	ArtBackground.Unload();

	for(int i = 0; i < (int)vecSelOkDialogItems.size(); i++)
	{
		UiListItem* pUIListItem = vecSelOkDialogItems[i];
		if(pUIListItem)
			delete pUIListItem;

		vecSelOkDialogItems.clear();
	}

	for(int i = 0; i < (int)vecSelOkDialog.size(); i++)
	{
		UiItemBase* pUIItem = vecSelOkDialog[i];
		if(pUIItem)
			delete pUIItem;

		vecSelOkDialog.clear();
	}

	for(int i = 0; i < (int)vecSpawnErrorOkDialog.size(); i++)
	{
		UiItemBase* pUIItem = vecSpawnErrorOkDialog[i];
		if(pUIItem)
			delete pUIItem;

		vecSpawnErrorOkDialog.clear();
	}
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
	vecSelOkDialogItems.push_back(new UiListItem("OK", 0));

	{
		SDL_Rect rect1 = {0, 0, SCREEN_WIDTH, SCREEN_HEIGHT};
		vecSelOkDialog.push_back(new UiImage(&ArtBackground, rect1));
	
		SDL_Rect rect2 = { 0, 0, 0, 0 };
		vecSelOkDialog.push_back(new UiImage(&ArtLogos[LOGO_MED], /*animated=*/true, /*frame=*/0, rect2, UIS_CENTER));

		SDL_Rect rect3 = { 24, 161, 590, 35 };
		vecSelOkDialog.push_back(new UiArtText(selok_title, rect3, UIS_CENTER | UIS_BIG));

		SDL_Rect rect4 = { 140, 210, 560, 168 };
		vecSelOkDialog.push_back(new UiArtText(dialogText, rect4, UIS_MED));

		vecSelOkDialog.push_back(new UiList(vecSelOkDialogItems, 230, 390, 180, 35, UIS_CENTER | UIS_BIG | UIS_GOLD));
	}

	{
		SDL_Rect rect1 = {0, 0, SCREEN_WIDTH, SCREEN_HEIGHT};
		vecSpawnErrorOkDialog.push_back(new UiImage(&ArtBackground, rect1));
	
		SDL_Rect rect2 = { 0, 182, 0, 0 };
		vecSpawnErrorOkDialog.push_back(new UiImage(&ArtLogos[LOGO_BIG], /*animated=*/true, /*frame=*/0, rect2, UIS_CENTER));

		SDL_Rect rect3 = { 140, 197, 560, 168 };
		vecSpawnErrorOkDialog.push_back(new UiArtText(dialogText, rect3, UIS_MED));

		vecSpawnErrorOkDialog.push_back(new UiList(vecSelOkDialogItems, 230, 390, 180, 35, UIS_CENTER | UIS_BIG | UIS_GOLD));
	}

	if (!background) {
		LoadBackgroundArt("ui_art\\black.pcx");
	} else {
		if (!gbSpawned) {
			LoadBackgroundArt("ui_art\\mainmenu.pcx");
		} else {
			LoadBackgroundArt("ui_art\\swmmenu.pcx");
		}
	}

	vUiItemBase items = vecSpawnErrorOkDialog;
	int itemCnt = items.size();
	if (title != NULL) {
		strcpy(selok_title, title);
		items = vecSelOkDialog;
		itemCnt = items.size();
	}

	strcpy(dialogText, body);
	WordWrapArtStr(dialogText, 280);

	UiInitList(0, 0, NULL, selok_Select, selok_Esc, items, itemCnt, false, NULL);

	selok_endMenu = false;
	while (!selok_endMenu) {
		UiRenderItems(items, itemCnt);
		UiPollAndRender();
	}

	selok_Free();
}
} // namespace dvl

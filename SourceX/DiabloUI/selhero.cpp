#include "selhero.h"

#include <algorithm>
#ifndef _XBOX
#include <chrono>
#include <random>
#endif

#include "DiabloUI/diabloui.h"
#include "../../DiabloUI/diabloui.h"
#include "all.h"
#include "DiabloUI/dialogs.h"
#include "DiabloUI/scrollbar.h"
#include "DiabloUI/selyesno.h"
#include "DiabloUI/selok.h"

namespace dvl {

const char *selhero_GenerateName(uint8_t hero_class);

std::size_t selhero_SaveCount = 0;
_uiheroinfo selhero_heros[MAX_CHARACTERS];
_uiheroinfo selhero_heroInfo;
std::size_t listOffset = 0;
const size_t kMaxViewportItems = 6;
char textStats[5][4];
char title[32];
char selhero_Lable[32];
char selhero_Description[256];
int selhero_result;
bool selhero_endMenu;
bool selhero_isMultiPlayer;
bool selhero_navigateYesNo;
bool selhero_deleteEnabled;

BOOL (*gfnHeroInfo)
(BOOL (*fninfofunc)(_uiheroinfo *));
BOOL(*gfnHeroCreate)
(_uiheroinfo *);
BOOL(*gfnHeroStats)
(unsigned int, _uidefaultstats *);

namespace {

std::vector<UiItemBase*> vecSelHeroDialog;
std::vector<UiListItem*> vecSellistDlgItems;
std::vector<UiItemBase*> vecSelDlgItems;
std::vector<UiItemBase*> vecSelClassDialog;
std::vector<UiListItem*> vecClassDlgItems;
std::vector<UiItemBase*> vecSelLoadDialog;
std::vector<UiListItem*> vecSelLoadDlgItems;
std::vector<UiItemBase*> vecSelNameDialog;

UiImage *SELHERO_DIALOG_HERO_IMG;
} // namespace

bool bUIElementsLoaded = false;

void selhero_UiFocusNavigationYesNo()
{
	if (selhero_deleteEnabled)
		UiFocusNavigationYesNo();
}

void selhero_Free()
{
	ArtBackground.Unload();

	for(int i = 0; i < (int)vecSelHeroDialog.size(); i++)
	{
		UiItemBase* pUIItem = vecSelHeroDialog[i];
		if(pUIItem)
			delete pUIItem;

		vecSelHeroDialog.clear();
	}

	for(int i = 0; i < (int)vecSelDlgItems.size(); i++)
	{
		UiItemBase* pUIItem = vecSelDlgItems[i];
		if(pUIItem)
			delete pUIItem;

		vecSelDlgItems.clear();
	}

	for(int i = 0; i < (int)vecSelClassDialog.size(); i++)
	{
		UiItemBase* pUIItem = vecSelClassDialog[i];
		if(pUIItem)
			delete pUIItem;

		vecSelClassDialog.clear();
	}

	for(int i = 0; i < (int)vecClassDlgItems.size(); i++)
	{
		UiListItem* pUIItem = vecClassDlgItems[i];
		if(pUIItem)
			delete pUIItem;

		vecClassDlgItems.clear();
	}

	for(int i = 0; i < (int)vecSelLoadDialog.size(); i++)
	{
		UiItemBase* pUIItem = vecSelLoadDialog[i];
		if(pUIItem)
			delete pUIItem;

		vecSelLoadDialog.clear();
	}

	for(int i = 0; i < (int)vecSelLoadDlgItems.size(); i++)
	{
		UiListItem* pUIItem = vecSelLoadDlgItems[i];
		if(pUIItem)
			delete pUIItem;

		vecSelLoadDlgItems.clear();
	}

	for(int i = 0; i < (int)vecSelNameDialog.size(); i++)
	{
		UiItemBase* pUIItem = vecSelNameDialog[i];
		if(pUIItem)
			delete pUIItem;

		vecSelNameDialog.clear();
	}

	bUIElementsLoaded = false;
}

void selhero_SetStats()
{
	SELHERO_DIALOG_HERO_IMG->m_frame = selhero_heroInfo.heroclass;
	sprintf(textStats[0], "%d", selhero_heroInfo.level);
	sprintf(textStats[1], "%d", selhero_heroInfo.strength);
	sprintf(textStats[2], "%d", selhero_heroInfo.magic);
	sprintf(textStats[3], "%d", selhero_heroInfo.dexterity);
	sprintf(textStats[4], "%d", selhero_heroInfo.vitality);
}

namespace {

UiList *SELLIST_DIALOG_LIST;
UiScrollBar* SELLIST_SCROLLBAR;
UiArtTextButton *SELLIST_DIALOG_DELETE_BUTTON;

void selhero_UpdateViewportItems()
{
	const size_t num_viewport_heroes = min(selhero_SaveCount - listOffset, kMaxViewportItems);
	SELLIST_DIALOG_LIST->m_length = num_viewport_heroes;
	for (std::size_t i = 0; i < num_viewport_heroes; i++) {
		const std::size_t index = i + listOffset;
		vecSellistDlgItems[i]->m_text = selhero_heros[index].name;
		vecSellistDlgItems[i]->m_value = static_cast<int>(index);
//		SELLIST_DIALOG_ITEMS[i] = { selhero_heros[index].name, static_cast<int>(index) };
	}
	if (num_viewport_heroes < kMaxViewportItems) {
		vecSellistDlgItems[i]->m_text = "New Hero";
		vecSellistDlgItems[i]->m_value = static_cast<int>(selhero_SaveCount);
//		SELLIST_DIALOG_ITEMS[num_viewport_heroes] = { "New Hero", static_cast<int>(selhero_SaveCount) };
		++SELLIST_DIALOG_LIST->m_length;
	}
}

void selhero_ScrollIntoView(std::size_t index)
{
	std::size_t new_offset = listOffset;
	if (index >= listOffset + kMaxViewportItems)
		new_offset = index - (kMaxViewportItems - 1);
	if (index < listOffset)
		new_offset = index;
	if (new_offset != listOffset) {
		listOffset = new_offset;
		selhero_UpdateViewportItems();
	}
}

} // namespace

void selhero_List_Init()
{
	SDL_Rect rect1 = {264, 211, 320, 33};
	vecSelDlgItems.push_back(new UiArtText("Select Hero", rect1, UIS_CENTER | UIS_BIG));	

	SELLIST_DIALOG_LIST = new UiList(vecSellistDlgItems, 265, 256, 320, 26, UIS_CENTER | UIS_MED | UIS_GOLD);
	vecSelDlgItems.push_back(SELLIST_DIALOG_LIST);

	SDL_Rect rect2 = { 585, 244, 25, 178 };
	UiScrollBar *SELLIST_SCROLLBAR = new UiScrollBar(&ArtScrollBarBackground, &ArtScrollBarThumb, &ArtScrollBarArrow, rect2);
	vecSelDlgItems.push_back(new UiScrollBar(&ArtScrollBarBackground, &ArtScrollBarThumb, &ArtScrollBarArrow, rect2));

	SDL_Rect rect3 = {239, 429, 120, 35 };
	vecSelDlgItems.push_back(new UiArtTextButton("OK", &UiFocusNavigationSelect, rect3, UIS_CENTER | UIS_BIG | UIS_GOLD));

	SDL_Rect rect4 = {364, 429, 120, 35};
	SELLIST_DIALOG_DELETE_BUTTON = new UiArtTextButton("Delete", &selhero_UiFocusNavigationYesNo, rect4, UIS_CENTER | UIS_BIG | UIS_DISABLED);
	vecSelDlgItems.push_back(SELLIST_DIALOG_DELETE_BUTTON);

	SDL_Rect rect5 = {489, 429, 120, 35};
	vecSelDlgItems.push_back(new UiArtTextButton("Cancel", &UiFocusNavigationEsc, rect5, UIS_CENTER | UIS_BIG | UIS_GOLD));

	listOffset = 0;
	selhero_UpdateViewportItems();
	UiInitList(0, selhero_SaveCount, selhero_List_Focus, selhero_List_Select, selhero_List_Esc, /*SELLIST_DIALOG*/vecSelDlgItems, /*size(SELLIST_DIALOG)*/vecSelDlgItems.size(), false, selhero_List_DeleteYesNo);
	UiInitScrollBar(SELLIST_SCROLLBAR, kMaxViewportItems, &listOffset);
	if (selhero_isMultiPlayer) {
		strcpy(title, "Multi Player Characters");
	} else {
		strcpy(title, "Single Player Characters");
	}
}

void selhero_List_Focus(int value)
{
	const std::size_t index = static_cast<std::size_t>(value);
	selhero_ScrollIntoView(index);
	int baseFlags = UIS_CENTER | UIS_BIG;
	if (selhero_SaveCount && index < selhero_SaveCount) {
		memcpy(&selhero_heroInfo, &selhero_heros[index], sizeof(selhero_heroInfo));
		selhero_SetStats();
		SELLIST_DIALOG_DELETE_BUTTON->m_iFlags = baseFlags | UIS_GOLD;
		selhero_deleteEnabled = true;
		return;
	}

	SELHERO_DIALOG_HERO_IMG->m_frame = UI_NUM_CLASSES;
	strcpy(textStats[0], "--");
	strcpy(textStats[1], "--");
	strcpy(textStats[2], "--");
	strcpy(textStats[3], "--");
	strcpy(textStats[4], "--");
	SELLIST_DIALOG_DELETE_BUTTON->m_iFlags = baseFlags | UIS_DISABLED;
	selhero_deleteEnabled = false;
}

bool selhero_List_DeleteYesNo()
{
	selhero_navigateYesNo = selhero_deleteEnabled;

	return selhero_navigateYesNo;
}

void selhero_List_Select(int value)
{
	SDL_Rect rect1 = { 264, 211, 320, 33 };
	vecSelClassDialog.push_back(new UiArtText("Choose Class", rect1, UIS_CENTER | UIS_BIG));	

	vecClassDlgItems.push_back(new UiListItem("Warrior", UI_WARRIOR));
	vecClassDlgItems.push_back(new UiListItem("Rogue", UI_ROGUE));
	vecClassDlgItems.push_back(new UiListItem("Sorcerer", UI_SORCERER));

	SDL_Rect rect2 = { 264, 285, 320, 33 };
	vecSelClassDialog.push_back(new UiList(/*SELCLAS_DIALOG_ITEMS*/vecClassDlgItems, 264, 285, 320, 33, UIS_CENTER | UIS_MED | UIS_GOLD));	

	SDL_Rect rect3 = { 279, 429, 140, 35 };
	vecSelClassDialog.push_back(new UiArtTextButton("OK", &UiFocusNavigationSelect, rect3, UIS_CENTER | UIS_BIG | UIS_GOLD));	

	SDL_Rect rect4 = { 429, 429, 140, 35 };
	vecSelClassDialog.push_back(new UiArtTextButton("Cancel", &UiFocusNavigationEsc, rect4, UIS_CENTER | UIS_BIG | UIS_GOLD));	

	if (static_cast<std::size_t>(value) == selhero_SaveCount) {
		UiInitList(0, 2, selhero_ClassSelector_Focus, selhero_ClassSelector_Select, selhero_ClassSelector_Esc, /*SELCLASS_DIALOG*/vecSelClassDialog, /*size(SELCLASS_DIALOG)*/vecSelClassDialog.size());
		memset(&selhero_heroInfo.name, 0, sizeof(selhero_heroInfo.name));
		strcpy(title, "New Single Player Hero");
		if (selhero_isMultiPlayer) {
			strcpy(title, "New Multi Player Hero");
		}
		return;
	} else if (selhero_heroInfo.hassaved) {
		vecSelLoadDlgItems.push_back(new UiListItem("Load Game", 0));
		vecSelLoadDlgItems.push_back(new UiListItem("New Game", 1));

		SDL_Rect rect1 = { 264, 211, 320, 33 };
		vecSelLoadDialog.push_back(new UiArtText("Save File Exists", rect1, UIS_CENTER | UIS_BIG));	

		vecSelLoadDialog.push_back(new UiList(/*SELLOAD_DIALOG_ITEMS*/vecSelLoadDlgItems, 265, 285, 320, 33, UIS_CENTER | UIS_MED | UIS_GOLD));	

		SDL_Rect rect2 = { 279, 427, 140, 35 };
		vecSelLoadDialog.push_back(new UiArtTextButton("OK", &UiFocusNavigationSelect, rect2 , UIS_CENTER | UIS_VCENTER | UIS_BIG | UIS_GOLD));

		SDL_Rect rect3 = { 429, 427, 140, 35 };
		vecSelLoadDialog.push_back(new UiArtTextButton("Cancel", &UiFocusNavigationEsc, rect3, UIS_CENTER | UIS_VCENTER | UIS_BIG | UIS_GOLD));

		UiInitList(0, 1, selhero_Load_Focus, selhero_Load_Select, selhero_List_Init, /*SELLOAD_DIALOG*/vecSelLoadDialog, /*size(SELLOAD_DIALOG)*/vecSelLoadDialog.size(), true);
		strcpy(title, "Single Player Characters");
		return;
	}

	UiInitList_clear();
	selhero_endMenu = true;
}

void selhero_List_Esc()
{
	UiInitList_clear();

	selhero_endMenu = true;
	selhero_result = SELHERO_PREVIOUS;
}

void selhero_ClassSelector_Focus(int value)
{
	_uidefaultstats defaults;
	gfnHeroStats(value, &defaults);

	selhero_heroInfo.level = 1;
	selhero_heroInfo.heroclass = value;
	selhero_heroInfo.strength = defaults.strength;
	selhero_heroInfo.magic = defaults.magic;
	selhero_heroInfo.dexterity = defaults.dexterity;
	selhero_heroInfo.vitality = defaults.vitality;

	selhero_SetStats();
}

void selhero_ClassSelector_Select(int value)
{
	if (gbSpawned && (value == 1 || value == 2)) {
		selhero_Free();
		UiSelOkDialog(NULL, "The Rogue and Sorcerer are only available in the full retail version of Diablo. Visit https://www.gog.com/game/diablo to purchase.", false);
		LoadBackgroundArt("ui_art\\selhero.pcx");
		selhero_List_Select(selhero_SaveCount);
		return;
	}

	strcpy(title, "New Single Player Hero");
	if (selhero_isMultiPlayer) {
		strcpy(title, "New Multi Player Hero");
	}
	memset(selhero_heroInfo.name, '\0', sizeof(selhero_heroInfo.name));
#if 1//def PREFILL_PLAYER_NAME
	strcpy(selhero_heroInfo.name, selhero_GenerateName(selhero_heroInfo.heroclass));
#endif
	SDL_Rect rect1 = { 264, 211, 320, 33 };
	vecSelNameDialog.push_back(new UiArtText("Enter Name", rect1, UIS_CENTER | UIS_BIG));	

	SDL_Rect rect2 = { 265, 317, 320, 33 };
	vecSelNameDialog.push_back(new UiEdit(selhero_heroInfo.name, 15, rect2, UIS_MED | UIS_GOLD));

	SDL_Rect rect3 = { 279, 429, 140, 35 };
	vecSelNameDialog.push_back(new UiArtTextButton("OK", &UiFocusNavigationSelect, rect3, UIS_CENTER | UIS_BIG | UIS_GOLD));	

	SDL_Rect rect4 = { 429, 429, 140, 35  };
	vecSelNameDialog.push_back(new UiArtTextButton("Cancel", &UiFocusNavigationEsc, rect4, UIS_CENTER | UIS_BIG | UIS_GOLD));	

	UiInitList(0, 0, NULL, selhero_Name_Select, selhero_Name_Esc, /*ENTERNAME_DIALOG*/vecSelNameDialog, /*size(ENTERNAME_DIALOG)*/vecSelNameDialog.size());
}

void selhero_ClassSelector_Esc()
{
	for(int i = 0; i < (int)vecClassDlgItems.size(); i++)
	{
		UiListItem* pUIItem = vecClassDlgItems[i];
		if(pUIItem)
			delete pUIItem;

		vecClassDlgItems.clear();
	}

	for(int i = 0; i < (int)vecSellistDlgItems.size(); i++)
	{
		UiListItem* pUIItem = vecSellistDlgItems[i];
		if(pUIItem)
			delete pUIItem;

		vecSellistDlgItems.clear();
	}

	for(int i = 0; i < (int)vecSelClassDialog.size(); i++)
	{
		UiItemBase* pUIItem = vecSelClassDialog[i];
		if(pUIItem)
			delete pUIItem;

		vecSelClassDialog.clear();
	}

	if (selhero_SaveCount) {
		selhero_List_Init();
		return;
	}

	selhero_List_Esc();
}

void selhero_Name_Select(int value)
{

	if (!UiValidPlayerName(selhero_heroInfo.name)) {
		selhero_Free();
		UiSelOkDialog(title, "Invalid name. A name cannot contain spaces, reserved characters, or reserved words.\n", false);
		LoadBackgroundArt("ui_art\\selhero.pcx");
	} else {
		bool overwrite = true;
		for (std::size_t i = 0; i < selhero_SaveCount; i++) {
			if (strcasecmp(selhero_heros[i].name, selhero_heroInfo.name) == 0) {
				selhero_Free();
				char dialogText[256];
				sprintf(dialogText, "Character already exists. Do you want to overwrite \"%s\"?", selhero_heroInfo.name);
				overwrite = UiSelHeroYesNoDialog(title, dialogText);
				LoadBackgroundArt("ui_art\\selhero.pcx");
				break;
			}
		}

		if (overwrite) {
			if (gfnHeroCreate(&selhero_heroInfo)) {
				UiInitList_clear();
				selhero_endMenu = true;
				return;
			} else {
				UiErrorOkDialog("Unable to create character.", vecSelHeroDialog, vecSelHeroDialog.size());
			}
		}
	}

	memset(selhero_heroInfo.name, '\0', sizeof(selhero_heroInfo.name));
#ifdef PREFILL_PLAYER_NAME
	strcpy(selhero_heroInfo.name, selhero_GenerateName(selhero_heroInfo.heroclass));
#endif
	selhero_ClassSelector_Select(selhero_heroInfo.heroclass);
}

void selhero_Name_Esc()
{
	selhero_List_Select(selhero_SaveCount);
}

void selhero_Load_Focus(int value)
{
}

void selhero_Load_Select(int value)
{
	UiInitList_clear();
	selhero_endMenu = true;
	if (value == 0) {
		selhero_result = SELHERO_CONTINUE;
		return;
	}

	selhero_result = 0;
}

BOOL SelHero_GetHeroInfo(_uiheroinfo *pInfo)
{
	selhero_heros[selhero_SaveCount] = *pInfo;
	selhero_SaveCount++;

	return true;
}

BOOL UiSelHeroDialog(
    BOOL (*fninfo)(BOOL (*fninfofunc)(_uiheroinfo *)),
    BOOL (*fncreate)(_uiheroinfo *),
    BOOL (*fnstats)(unsigned int, _uidefaultstats *),
    BOOL (*fnremove)(_uiheroinfo *),
    int *dlgresult,
    char *name)
{
	bUIElementsLoaded = true;

	for(int i = 0; i < (int)kMaxViewportItems; i++)
		vecSellistDlgItems.push_back(new UiListItem("na", 0));

	do {
		LoadBackgroundArt("ui_art\\selhero.pcx");
		LoadScrollBar();

		SDL_Rect rect1 = {0, 0, SCREEN_WIDTH, SCREEN_HEIGHT};
		vecSelHeroDialog.push_back(new UiImage(&ArtBackground, rect1));

 		SDL_Rect rect2 = {0, 0, 0, 0};
		vecSelHeroDialog.push_back(new UiImage(&ArtLogos[LOGO_MED], /*animated=*/true, /*frame=*/0, rect2, UIS_CENTER));

 		SDL_Rect rect3 = { 24, 161, 590, 35 };
		vecSelHeroDialog.push_back(new UiArtText(title, rect3, UIS_CENTER | UIS_BIG));

 		SDL_Rect rect4 = {30, 211, 180, 76 };
		SELHERO_DIALOG_HERO_IMG = new UiImage(&ArtHero, UI_NUM_CLASSES, rect4);
		vecSelHeroDialog.push_back(SELHERO_DIALOG_HERO_IMG);

 		SDL_Rect rect5 = { 39, 323, 110, 21 };
		vecSelHeroDialog.push_back(new UiArtText("Level:", rect5, UIS_RIGHT));

 		SDL_Rect rect6 = { 39, 323, 110, 21 };
		vecSelHeroDialog.push_back(new UiArtText("Level:", rect6, UIS_RIGHT));
 		SDL_Rect rect7 = { 159, 323, 40, 21 };
		vecSelHeroDialog.push_back(new UiArtText(textStats[0], rect7, UIS_CENTER));

 		SDL_Rect rect8 = { 39, 358, 110, 21 };
		vecSelHeroDialog.push_back(new UiArtText("Strength:", rect8, UIS_RIGHT));
 		SDL_Rect rect9 = { 159, 358, 40, 21 };
		vecSelHeroDialog.push_back(new UiArtText(textStats[1], rect9, UIS_CENTER));

 		SDL_Rect rect10 = { 39, 380, 110, 21  };
		vecSelHeroDialog.push_back(new UiArtText("Magic:", rect10, UIS_RIGHT));
 		SDL_Rect rect11 = { 159, 380, 40, 21  };
		vecSelHeroDialog.push_back(new UiArtText(textStats[2], rect11, UIS_CENTER));

 		SDL_Rect rect12 = { 39, 401, 110, 21 };
		vecSelHeroDialog.push_back(new UiArtText("Dexterity:", rect12, UIS_RIGHT));
 		SDL_Rect rect13 = { 159, 401, 40, 21 };
		vecSelHeroDialog.push_back(new UiArtText(textStats[3], rect13, UIS_CENTER));

 		SDL_Rect rect14 = {39, 422, 110, 21 };
		vecSelHeroDialog.push_back(new UiArtText("Vitality:", rect14, UIS_RIGHT));
 		SDL_Rect rect15 = {159, 422, 40, 21  };
		vecSelHeroDialog.push_back(new UiArtText(textStats[4], rect15, UIS_CENTER));

		gfnHeroInfo = fninfo;
		gfnHeroCreate = fncreate;
		gfnHeroStats = fnstats;
		selhero_result = *dlgresult;

		selhero_navigateYesNo = false;

		selhero_SaveCount = 0;
		gfnHeroInfo(SelHero_GetHeroInfo);
		std::reverse(selhero_heros, selhero_heros + selhero_SaveCount);

		if (selhero_SaveCount) {
			selhero_List_Init();
		} else {
			selhero_List_Select(selhero_SaveCount);
		}

		selhero_endMenu = false;
		while (!selhero_endMenu && !selhero_navigateYesNo) {
			UiRenderItems(vecSelHeroDialog, vecSelHeroDialog.size());
			UiPollAndRender();
		}
		selhero_Free();

		if (selhero_navigateYesNo) {
			char dialogTitle[32];
			char dialogText[256];
			if (selhero_isMultiPlayer) {
				strcpy(dialogTitle, "Delete Multi Player Hero");
			} else {
				strcpy(dialogTitle, "Delete Single Player Hero");
			}
			sprintf(dialogText, "Are you sure you want to delete the character \"%s\"?", selhero_heroInfo.name);

			if (UiSelHeroYesNoDialog(dialogTitle, dialogText))
				fnremove(&selhero_heroInfo);
		}
	} while (selhero_navigateYesNo);

	*dlgresult = selhero_result;
	strcpy(name, selhero_heroInfo.name);

	UnloadScrollBar();
	return true;
}

BOOL UiSelHeroSingDialog(
    BOOL (*fninfo)(BOOL (*fninfofunc)(_uiheroinfo *)),
    BOOL (*fncreate)(_uiheroinfo *),
    BOOL (*fnremove)(_uiheroinfo *),
    BOOL (*fnstats)(unsigned int, _uidefaultstats *),
    int *dlgresult,
    char *name,
    int *difficulty)
{
	selhero_isMultiPlayer = false;
	return UiSelHeroDialog(fninfo, fncreate, fnstats, fnremove, dlgresult, name);
}

BOOL UiSelHeroMultDialog(
    BOOL (*fninfo)(BOOL (*fninfofunc)(_uiheroinfo *)),
    BOOL (*fncreate)(_uiheroinfo *),
    BOOL (*fnremove)(_uiheroinfo *),
    BOOL (*fnstats)(unsigned int, _uidefaultstats *),
    int *dlgresult,
    BOOL *hero_is_created,
    char *name)
{
	selhero_isMultiPlayer = true;
	return UiSelHeroDialog(fninfo, fncreate, fnstats, fnremove, dlgresult, name);
}

const char *selhero_GenerateName(uint8_t hero_class)
{
	static const char *const kNames[3][10] = {
		{
		    "Aidan",
		    "Qarak",
		    "Born",
		    "Cathan",
		    "Halbu",
		    "Lenalas",
		    "Maximus",
		    "Vane",
		    "Myrdgar",
		    "Rothat",
		},
		{
		    "Moreina",
		    "Akara",
		    "Kashya",
		    "Flavie",
		    "Divo",
		    "Oriana",
		    "Iantha",
		    "Shikha",
		    "Basanti",
		    "Elexa",
		},
		{
		    "Jazreth",
		    "Drognan",
		    "Armin",
		    "Fauztin",
		    "Jere",
		    "Kazzulk",
		    "Ranslor",
		    "Sarnakyle",
		    "Valthek",
		    "Horazon",
		}
	};
	
	int iRand = rand() % 9;

	return kNames[hero_class][iRand];
}

} // namespace dvl

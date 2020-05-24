#include "selhero.h"

#include <algorithm>
#include <chrono>
#include <random>

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

BOOL(*gfnHeroInfo)
(BOOL (*fninfofunc)(_uiheroinfo *));
BOOL(*gfnHeroCreate)
(_uiheroinfo *);
BOOL(*gfnHeroStats)
(unsigned int, _uidefaultstats *);

namespace {

std::vector<UiItemBase *> vecSelHeroDialog;
std::vector<UiListItem *> vecSelHeroDlgItems;
std::vector<UiItemBase *> vecSelDlgItems;

UiImage *SELHERO_DIALOG_HERO_IMG;
} // namespace

bool bUIElementsLoaded = false;

void selhero_UiFocusNavigationYesNo()
{
	if (selhero_deleteEnabled)
		UiFocusNavigationYesNo();
}

void selhero_FreeListItems()
{
	for (std::size_t i = 0; i < vecSelHeroDlgItems.size(); i++) {
		UiListItem *pUIItem = vecSelHeroDlgItems[i];
		delete pUIItem;
	}
	vecSelHeroDlgItems.clear();
}

void selhero_FreeDlgItems()
{
	for (std::size_t i = 0; i < vecSelDlgItems.size(); i++) {
		UiItemBase *pUIItem = vecSelDlgItems[i];
		delete pUIItem;
	}
	vecSelDlgItems.clear();
}

void selhero_Free()
{
	ArtBackground.Unload();

	for (std::size_t i = 0; i < vecSelHeroDialog.size(); i++) {
		UiItemBase *pUIItem = vecSelHeroDialog[i];
		delete pUIItem;
	}
	vecSelHeroDialog.clear();

	selhero_FreeDlgItems();
	selhero_FreeListItems();

	bUIElementsLoaded = false;
}

void selhero_SetStats()
{
	SELHERO_DIALOG_HERO_IMG->m_frame = selhero_heroInfo.heroclass;
	snprintf(textStats[0], sizeof(textStats[0]), "%d", selhero_heroInfo.level);
	snprintf(textStats[1], sizeof(textStats[1]), "%d", selhero_heroInfo.strength);
	snprintf(textStats[2], sizeof(textStats[2]), "%d", selhero_heroInfo.magic);
	snprintf(textStats[3], sizeof(textStats[3]), "%d", selhero_heroInfo.dexterity);
	snprintf(textStats[4], sizeof(textStats[4]), "%d", selhero_heroInfo.vitality);
}

namespace {

std::size_t listOffset = 0;
UiArtTextButton *SELLIST_DIALOG_DELETE_BUTTON;

void selhero_UpdateViewportItems()
{
	const std::size_t num_viewport_heroes = std::min(selhero_SaveCount - listOffset, kMaxViewportItems);
	for (std::size_t i = 0; i < num_viewport_heroes; i++) {
		const std::size_t index = i + listOffset;
		vecSelHeroDlgItems[i]->m_text = selhero_heros[index].name;
		vecSelHeroDlgItems[i]->m_value = static_cast<int>(index);
	}
	if (num_viewport_heroes < kMaxViewportItems) {
		vecSelHeroDlgItems[num_viewport_heroes]->m_text = "New Hero";
		vecSelHeroDlgItems[num_viewport_heroes]->m_value = static_cast<int>(selhero_SaveCount);
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
	listOffset = 0;
	selhero_FreeDlgItems();

	SDL_Rect rect1 = { PANEL_LEFT + 264, 211, 320, 33 };
	vecSelDlgItems.push_back(new UiArtText("Select Hero", rect1, UIS_CENTER | UIS_BIG));

	selhero_FreeListItems();
	const size_t num_viewport_heroes = std::min(selhero_SaveCount + 1, kMaxViewportItems);
	for (std::size_t i = 0; i < num_viewport_heroes; i++) {
		vecSelHeroDlgItems.push_back(new UiListItem("", -1));
	}
	selhero_UpdateViewportItems();

	vecSelDlgItems.push_back(new UiList(vecSelHeroDlgItems, PANEL_LEFT + 265, 256, 320, 26, UIS_CENTER | UIS_MED | UIS_GOLD));

	SDL_Rect rect2 = { PANEL_LEFT + 585, 244, 25, 178 };
	UiScrollBar *scrollBar = new UiScrollBar(&ArtScrollBarBackground, &ArtScrollBarThumb, &ArtScrollBarArrow, rect2);
	vecSelDlgItems.push_back(scrollBar);

	SDL_Rect rect3 = { PANEL_LEFT + 239, 429, 120, 35 };
	vecSelDlgItems.push_back(new UiArtTextButton("OK", &UiFocusNavigationSelect, rect3, UIS_CENTER | UIS_BIG | UIS_GOLD));

	SDL_Rect rect4 = { PANEL_LEFT + 364, 429, 120, 35 };
	SELLIST_DIALOG_DELETE_BUTTON = new UiArtTextButton("Delete", &selhero_UiFocusNavigationYesNo, rect4, UIS_CENTER | UIS_BIG | UIS_DISABLED);
	vecSelDlgItems.push_back(SELLIST_DIALOG_DELETE_BUTTON);

	SDL_Rect rect5 = { PANEL_LEFT + 489, 429, 120, 35 };
	vecSelDlgItems.push_back(new UiArtTextButton("Cancel", &UiFocusNavigationEsc, rect5, UIS_CENTER | UIS_BIG | UIS_GOLD));

	UiInitList(0, selhero_SaveCount, selhero_List_Focus, selhero_List_Select, selhero_List_Esc, vecSelDlgItems, false, selhero_List_DeleteYesNo);
	UiInitScrollBar(scrollBar, kMaxViewportItems, &listOffset);
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
	strncpy(textStats[0], "--", sizeof(textStats[0]) - 1);
	strncpy(textStats[1], "--", sizeof(textStats[1]) - 1);
	strncpy(textStats[2], "--", sizeof(textStats[2]) - 1);
	strncpy(textStats[3], "--", sizeof(textStats[3]) - 1);
	strncpy(textStats[4], "--", sizeof(textStats[4]) - 1);
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
	if (static_cast<std::size_t>(value) == selhero_SaveCount) {
		selhero_FreeDlgItems();

		SDL_Rect rect1 = { PANEL_LEFT + 264, 211, 320, 33 };
		vecSelDlgItems.push_back(new UiArtText("Choose Class", rect1, UIS_CENTER | UIS_BIG));

		selhero_FreeListItems();
		vecSelHeroDlgItems.push_back(new UiListItem("Warrior", UI_WARRIOR));
		vecSelHeroDlgItems.push_back(new UiListItem("Rogue", UI_ROGUE));
		vecSelHeroDlgItems.push_back(new UiListItem("Sorcerer", UI_SORCERER));
		vecSelDlgItems.push_back(new UiList(vecSelHeroDlgItems, PANEL_LEFT + 264, 285, 320, 33, UIS_CENTER | UIS_MED | UIS_GOLD));

		SDL_Rect rect2 = { PANEL_LEFT + 279, 429, 140, 35 };
		vecSelDlgItems.push_back(new UiArtTextButton("OK", &UiFocusNavigationSelect, rect2, UIS_CENTER | UIS_BIG | UIS_GOLD));

		SDL_Rect rect3 = { PANEL_LEFT + 429, 429, 140, 35 };
		vecSelDlgItems.push_back(new UiArtTextButton("Cancel", &UiFocusNavigationEsc, rect3, UIS_CENTER | UIS_BIG | UIS_GOLD));

		UiInitList(0, 2, selhero_ClassSelector_Focus, selhero_ClassSelector_Select, selhero_ClassSelector_Esc, vecSelDlgItems);
		memset(&selhero_heroInfo.name, 0, sizeof(selhero_heroInfo.name));
		strncpy(title, "New Single Player Hero", sizeof(title) - 1);
		if (selhero_isMultiPlayer) {
			strncpy(title, "New Multi Player Hero", sizeof(title) - 1);
		}
		return;
	}

	if (selhero_heroInfo.hassaved) {
		selhero_FreeDlgItems();

		SDL_Rect rect1 = { PANEL_LEFT + 264, 211, 320, 33 };
		vecSelDlgItems.push_back(new UiArtText("Save File Exists", rect1, UIS_CENTER | UIS_BIG));

		selhero_FreeListItems();
		vecSelHeroDlgItems.push_back(new UiListItem("Load Game", 0));
		vecSelHeroDlgItems.push_back(new UiListItem("New Game", 1));
		vecSelDlgItems.push_back(new UiList(vecSelHeroDlgItems, PANEL_LEFT + 265, 285, 320, 33, UIS_CENTER | UIS_MED | UIS_GOLD));

		SDL_Rect rect2 = { PANEL_LEFT + 279, 427, 140, 35 };
		vecSelDlgItems.push_back(new UiArtTextButton("OK", &UiFocusNavigationSelect, rect2, UIS_CENTER | UIS_VCENTER | UIS_BIG | UIS_GOLD));

		SDL_Rect rect3 = { PANEL_LEFT + 429, 427, 140, 35 };
		vecSelDlgItems.push_back(new UiArtTextButton("Cancel", &UiFocusNavigationEsc, rect3, UIS_CENTER | UIS_VCENTER | UIS_BIG | UIS_GOLD));

		UiInitList(0, 1, selhero_Load_Focus, selhero_Load_Select, selhero_List_Init, vecSelDlgItems, true);
		strncpy(title, "Single Player Characters", sizeof(title) - 1);
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
		ArtBackground.Unload();
		UiSelOkDialog(NULL, "The Rogue and Sorcerer are only available in the full retail version of Diablo. Visit https://www.gog.com/game/diablo to purchase.", false);
		LoadBackgroundArt("ui_art\\selhero.pcx");
		selhero_List_Select(selhero_SaveCount);
		return;
	}

	strncpy(title, "New Single Player Hero", sizeof(title) - 1);
	if (selhero_isMultiPlayer) {
		strncpy(title, "New Multi Player Hero", sizeof(title) - 1);
	}
	memset(selhero_heroInfo.name, '\0', sizeof(selhero_heroInfo.name));
#ifdef PREFILL_PLAYER_NAME
	strncpy(selhero_heroInfo.name, selhero_GenerateName(selhero_heroInfo.heroclass), sizeof(selhero_heroInfo.name) - 1);
#endif
	selhero_FreeDlgItems();
	SDL_Rect rect1 = { PANEL_LEFT + 264, 211, 320, 33 };
	vecSelDlgItems.push_back(new UiArtText("Enter Name", rect1, UIS_CENTER | UIS_BIG));

	SDL_Rect rect2 = { PANEL_LEFT + 265, 317, 320, 33 };
	vecSelDlgItems.push_back(new UiEdit(selhero_heroInfo.name, 15, rect2, UIS_MED | UIS_GOLD));

	SDL_Rect rect3 = { PANEL_LEFT + 279, 429, 140, 35 };
	vecSelDlgItems.push_back(new UiArtTextButton("OK", &UiFocusNavigationSelect, rect3, UIS_CENTER | UIS_BIG | UIS_GOLD));

	SDL_Rect rect4 = { PANEL_LEFT + 429, 429, 140, 35 };
	vecSelDlgItems.push_back(new UiArtTextButton("Cancel", &UiFocusNavigationEsc, rect4, UIS_CENTER | UIS_BIG | UIS_GOLD));

	UiInitList(0, 0, NULL, selhero_Name_Select, selhero_Name_Esc, vecSelDlgItems);
}

void selhero_ClassSelector_Esc()
{
	selhero_FreeDlgItems();
	selhero_FreeListItems();

	if (selhero_SaveCount) {
		selhero_List_Init();
		return;
	}

	selhero_List_Esc();
}

void selhero_Name_Select(int value)
{

	if (!UiValidPlayerName(selhero_heroInfo.name)) {
		ArtBackground.Unload();
		UiSelOkDialog(title, "Invalid name. A name cannot contain spaces, reserved characters, or reserved words.\n", false);
		LoadBackgroundArt("ui_art\\selhero.pcx");
	} else {
		bool overwrite = true;
		for (std::size_t i = 0; i < selhero_SaveCount; i++) {
			if (strcasecmp(selhero_heros[i].name, selhero_heroInfo.name) == 0) {
				ArtBackground.Unload();
				char dialogText[256];
				snprintf(dialogText, sizeof(dialogText), "Character already exists. Do you want to overwrite \"%s\"?", selhero_heroInfo.name);
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
				UiErrorOkDialog("Unable to create character.", vecSelDlgItems);
			}
		}
	}

	memset(selhero_heroInfo.name, '\0', sizeof(selhero_heroInfo.name));
#ifdef PREFILL_PLAYER_NAME
	strncpy(selhero_heroInfo.name, selhero_GenerateName(selhero_heroInfo.heroclass), sizeof(selhero_heroInfo.name) - 1);
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

	do {
		LoadBackgroundArt("ui_art\\selhero.pcx");
		UiAddBackground(&vecSelHeroDialog);
		UiAddLogo(&vecSelHeroDialog);
		LoadScrollBar();

		selhero_FreeDlgItems();
		SDL_Rect rect1 = { PANEL_LEFT + 24, 161, 590, 35 };
		vecSelHeroDialog.push_back(new UiArtText(title, rect1, UIS_CENTER | UIS_BIG));

		SDL_Rect rect2 = { PANEL_LEFT + 30, 211, 180, 76 };
		SELHERO_DIALOG_HERO_IMG = new UiImage(&ArtHero, UI_NUM_CLASSES, rect2);
		vecSelHeroDialog.push_back(SELHERO_DIALOG_HERO_IMG);

		SDL_Rect rect3 = { PANEL_LEFT + 39, 323, 110, 21 };
		vecSelHeroDialog.push_back(new UiArtText("Level:", rect3, UIS_RIGHT));

		SDL_Rect rect4 = { PANEL_LEFT + 39, 323, 110, 21 };
		vecSelHeroDialog.push_back(new UiArtText("Level:", rect4, UIS_RIGHT));
		SDL_Rect rect5 = { PANEL_LEFT + 159, 323, 40, 21 };
		vecSelHeroDialog.push_back(new UiArtText(textStats[0], rect5, UIS_CENTER));

		SDL_Rect rect6 = { PANEL_LEFT + 39, 358, 110, 21 };
		vecSelHeroDialog.push_back(new UiArtText("Strength:", rect6, UIS_RIGHT));
		SDL_Rect rect7 = { PANEL_LEFT + 159, 358, 40, 21 };
		vecSelHeroDialog.push_back(new UiArtText(textStats[1], rect7, UIS_CENTER));

		SDL_Rect rect8 = { PANEL_LEFT + 39, 380, 110, 21 };
		vecSelHeroDialog.push_back(new UiArtText("Magic:", rect8, UIS_RIGHT));
		SDL_Rect rect9 = { PANEL_LEFT + 159, 380, 40, 21 };
		vecSelHeroDialog.push_back(new UiArtText(textStats[2], rect9, UIS_CENTER));

		SDL_Rect rect10 = { PANEL_LEFT + 39, 401, 110, 21 };
		vecSelHeroDialog.push_back(new UiArtText("Dexterity:", rect10, UIS_RIGHT));
		SDL_Rect rect11 = { PANEL_LEFT + 159, 401, 40, 21 };
		vecSelHeroDialog.push_back(new UiArtText(textStats[3], rect11, UIS_CENTER));

		SDL_Rect rect12 = { PANEL_LEFT + 39, 422, 110, 21 };
		vecSelHeroDialog.push_back(new UiArtText("Vitality:", rect12, UIS_RIGHT));
		SDL_Rect rect13 = { PANEL_LEFT + 159, 422, 40, 21 };
		vecSelHeroDialog.push_back(new UiArtText(textStats[4], rect13, UIS_CENTER));

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
			UiClearScreen();
			UiRenderItems(vecSelHeroDialog);
			UiPollAndRender();
		}
		selhero_Free();

		if (selhero_navigateYesNo) {
			char dialogTitle[32];
			char dialogText[256];
			if (selhero_isMultiPlayer) {
				strncpy(dialogTitle, "Delete Multi Player Hero", sizeof(dialogTitle) - 1);
			} else {
				strncpy(dialogTitle, "Delete Single Player Hero", sizeof(dialogTitle) - 1);
			}
			snprintf(dialogText, sizeof(dialogText), "Are you sure you want to delete the character \"%s\"?", selhero_heroInfo.name);

			if (UiSelHeroYesNoDialog(dialogTitle, dialogText))
				fnremove(&selhero_heroInfo);
		}
	} while (selhero_navigateYesNo);

	*dlgresult = selhero_result;
	strncpy(name, selhero_heroInfo.name, sizeof(name) - 1);

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

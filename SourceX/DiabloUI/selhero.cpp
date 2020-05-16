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

UiItem SELHERO_DIALOG[] = {
	MAINMENU_BACKGROUND,
	MAINMENU_LOGO,
	UiArtText(title, { PANEL_LEFT + 24, 161, 590, 35 }, UIS_CENTER | UIS_BIG),
	UiImage(&ArtHero, UI_NUM_CLASSES, { PANEL_LEFT + 30, 211, 180, 76 }),
	UiArtText("Level:", { PANEL_LEFT + 39, 323, 110, 21 }, UIS_RIGHT),
	UiArtText(textStats[0], { PANEL_LEFT + 159, 323, 40, 21 }, UIS_CENTER),
	UiArtText("Strength:", { PANEL_LEFT + 39, 358, 110, 21 }, UIS_RIGHT),
	UiArtText(textStats[1], { PANEL_LEFT + 159, 358, 40, 21 }, UIS_CENTER),
	UiArtText("Magic:", { PANEL_LEFT + 39, 380, 110, 21 }, UIS_RIGHT),
	UiArtText(textStats[2], { PANEL_LEFT + 159, 380, 40, 21 }, UIS_CENTER),
	UiArtText("Dexterity:", { PANEL_LEFT + 39, 401, 110, 21 }, UIS_RIGHT),
	UiArtText(textStats[3], { PANEL_LEFT + 159, 401, 40, 21 }, UIS_CENTER),
	UiArtText("Vitality:", { PANEL_LEFT + 39, 422, 110, 21 }, UIS_RIGHT),
	UiArtText(textStats[4], { PANEL_LEFT + 159, 422, 40, 21 }, UIS_CENTER),
};
UiImage *SELHERO_DIALOG_HERO_IMG = &SELHERO_DIALOG[3].image;

UiListItem SELLIST_DIALOG_ITEMS[kMaxViewportItems];
UiItem SELLIST_DIALOG[] = {
	UiArtText("Select Hero", { PANEL_LEFT + 264, 211, 320, 33 }, UIS_CENTER | UIS_BIG),
	UiList(SELLIST_DIALOG_ITEMS, PANEL_LEFT + 265, 256, 320, 26, UIS_CENTER | UIS_MED | UIS_GOLD),
	MakeScrollBar({ PANEL_LEFT + 585, 244, 25, 178 }),
	UiArtTextButton("OK", &UiFocusNavigationSelect, { PANEL_LEFT + 239, 429, 120, 35 }, UIS_CENTER | UIS_BIG | UIS_GOLD),
	UiArtTextButton("Delete", &selhero_UiFocusNavigationYesNo, { PANEL_LEFT + 364, 429, 120, 35 }, UIS_CENTER | UIS_BIG | UIS_DISABLED),
	UiArtTextButton("Cancel", &UiFocusNavigationEsc, { PANEL_LEFT + 489, 429, 120, 35 }, UIS_CENTER | UIS_BIG | UIS_GOLD)
};
UiList *SELLIST_DIALOG_LIST = &SELLIST_DIALOG[1].list;
UiScrollBar *SELLIST_SCROLLBAR = &SELLIST_DIALOG[2].scrollbar;
UiArtTextButton *SELLIST_DIALOG_DELETE_BUTTON = &SELLIST_DIALOG[4].art_text_button;

UiListItem SELCLAS_DIALOG_ITEMS[] = {
	{ "Warrior", UI_WARRIOR },
	{ "Rogue", UI_ROGUE },
	{ "Sorcerer", UI_SORCERER }
};
UiItem SELCLASS_DIALOG[] = {
	UiArtText("Choose Class", { PANEL_LEFT + 264, 211, 320, 33 }, UIS_CENTER | UIS_BIG),
	UiList(SELCLAS_DIALOG_ITEMS, PANEL_LEFT + 264, 285, 320, 33, UIS_CENTER | UIS_MED | UIS_GOLD),
	UiArtTextButton("OK", &UiFocusNavigationSelect, { PANEL_LEFT + 279, 429, 140, 35 }, UIS_CENTER | UIS_BIG | UIS_GOLD),
	UiArtTextButton("Cancel", &UiFocusNavigationEsc, { PANEL_LEFT + 429, 429, 140, 35 }, UIS_CENTER | UIS_BIG | UIS_GOLD)
};

UiItem ENTERNAME_DIALOG[] = {
	UiArtText("Enter Name", { PANEL_LEFT + 264, 211, 320, 33 }, UIS_CENTER | UIS_BIG),
	UiEdit(selhero_heroInfo.name, 15, { PANEL_LEFT + 265, 317, 320, 33 }, UIS_MED | UIS_GOLD),
	UiArtTextButton("OK", &UiFocusNavigationSelect, { PANEL_LEFT + 279, 429, 140, 35 }, UIS_CENTER | UIS_BIG | UIS_GOLD),
	UiArtTextButton("Cancel", &UiFocusNavigationEsc, { PANEL_LEFT + 429, 429, 140, 35 }, UIS_CENTER | UIS_BIG | UIS_GOLD)
};

UiListItem SELLOAD_DIALOG_ITEMS[] = {
	{ "Load Game", 0 },
	{ "New Game", 1 }
};
UiItem SELLOAD_DIALOG[] = {
	UiArtText("Save File Exists", { PANEL_LEFT + 264, 211, 320, 33 }, UIS_CENTER | UIS_BIG),
	UiList(SELLOAD_DIALOG_ITEMS, PANEL_LEFT + 265, 285, 320, 33, UIS_CENTER | UIS_MED | UIS_GOLD),
	UiArtTextButton("OK", &UiFocusNavigationSelect, { PANEL_LEFT + 279, 427, 140, 35 }, UIS_CENTER | UIS_VCENTER | UIS_BIG | UIS_GOLD),
	UiArtTextButton("Cancel", &UiFocusNavigationEsc, { PANEL_LEFT + 429, 427, 140, 35 }, UIS_CENTER | UIS_VCENTER | UIS_BIG | UIS_GOLD)
};

} // namespace

void selhero_UiFocusNavigationYesNo()
{
	if (selhero_deleteEnabled)
		UiFocusNavigationYesNo();
}

void selhero_Free()
{
	ArtBackground.Unload();
}

void selhero_SetStats()
{
	SELHERO_DIALOG_HERO_IMG->frame = selhero_heroInfo.heroclass;
	sprintf(textStats[0], "%d", selhero_heroInfo.level);
	sprintf(textStats[1], "%d", selhero_heroInfo.strength);
	sprintf(textStats[2], "%d", selhero_heroInfo.magic);
	sprintf(textStats[3], "%d", selhero_heroInfo.dexterity);
	sprintf(textStats[4], "%d", selhero_heroInfo.vitality);
}

namespace {

void selhero_UpdateViewportItems()
{
	const std::size_t num_viewport_heroes = std::min(selhero_SaveCount - listOffset, kMaxViewportItems);
	SELLIST_DIALOG_LIST->length = num_viewport_heroes;
	for (std::size_t i = 0; i < num_viewport_heroes; i++) {
		const std::size_t index = i + listOffset;
		SELLIST_DIALOG_ITEMS[i] = { selhero_heros[index].name, static_cast<int>(index) };
	}
	if (num_viewport_heroes < kMaxViewportItems) {
		SELLIST_DIALOG_ITEMS[num_viewport_heroes] = { "New Hero", static_cast<int>(selhero_SaveCount) };
		++SELLIST_DIALOG_LIST->length;
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
	selhero_UpdateViewportItems();
	UiInitList(0, selhero_SaveCount, selhero_List_Focus, selhero_List_Select, selhero_List_Esc, SELLIST_DIALOG, size(SELLIST_DIALOG), false, selhero_List_DeleteYesNo);
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
		SELLIST_DIALOG_DELETE_BUTTON->flags = baseFlags | UIS_GOLD;
		selhero_deleteEnabled = true;
		return;
	}

	SELHERO_DIALOG_HERO_IMG->frame = UI_NUM_CLASSES;
	strcpy(textStats[0], "--");
	strcpy(textStats[1], "--");
	strcpy(textStats[2], "--");
	strcpy(textStats[3], "--");
	strcpy(textStats[4], "--");
	SELLIST_DIALOG_DELETE_BUTTON->flags = baseFlags | UIS_DISABLED;
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
		UiInitList(0, 2, selhero_ClassSelector_Focus, selhero_ClassSelector_Select, selhero_ClassSelector_Esc, SELCLASS_DIALOG, size(SELCLASS_DIALOG));
		memset(&selhero_heroInfo.name, 0, sizeof(selhero_heroInfo.name));
		strcpy(title, "New Single Player Hero");
		if (selhero_isMultiPlayer) {
			strcpy(title, "New Multi Player Hero");
		}
		return;
	} else if (selhero_heroInfo.hassaved) {
		UiInitList(0, 1, selhero_Load_Focus, selhero_Load_Select, selhero_List_Init, SELLOAD_DIALOG, size(SELLOAD_DIALOG), true);
		strcpy(title, "Single Player Characters");
		return;
	}

	UiInitList(0, 0, NULL, NULL, NULL, NULL, 0);
	selhero_endMenu = true;
}

void selhero_List_Esc()
{
	UiInitList(0, 0, NULL, NULL, NULL, NULL, 0);
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
#ifdef PREFILL_PLAYER_NAME
	strcpy(selhero_heroInfo.name, selhero_GenerateName(selhero_heroInfo.heroclass));
#endif
	UiInitList(0, 0, NULL, selhero_Name_Select, selhero_Name_Esc, ENTERNAME_DIALOG, size(ENTERNAME_DIALOG));
}

void selhero_ClassSelector_Esc()
{
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
				UiInitList(0, 0, NULL, NULL, NULL, NULL, 0);
				selhero_endMenu = true;
				return;
			} else {
				UiErrorOkDialog("Unable to create character.", SELHERO_DIALOG, size(SELHERO_DIALOG));
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
	UiInitList(0, 0, NULL, NULL, NULL, NULL, 0);
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
	do {
		LoadBackgroundArt("ui_art\\selhero.pcx");
		LoadScrollBar();

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
			UiRenderItems(SELHERO_DIALOG, size(SELHERO_DIALOG));
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

const char *selhero_GenerateName(std::uint8_t hero_class)
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
	const int seed = std::chrono::system_clock::now().time_since_epoch().count();
	std::default_random_engine generator(seed);
	std::uniform_int_distribution<std::size_t> dist(0, sizeof(kNames[0]) / sizeof(kNames[0][0]) - 1);
	return kNames[hero_class][dist(generator)];
}

} // namespace dvl

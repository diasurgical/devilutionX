#include "selhero.h"
#include "selyesno.h"

#include "DiabloUI/diabloui.h"
#include "devilution.h"

namespace dvl {

int selhero_SaveCount = 0;
_uiheroinfo selhero_heros[MAX_CHARACTERS];
_uiheroinfo selhero_heroInfo;
char listItems[6][16];
char textStats[5][4];
char title[32];
char selhero_Lable[32];
char selhero_Description[256];
int selhero_result;
bool selhero_endMenu;
bool selhero_isMultiPlayer;
bool selhero_navigateYesNo;
bool selhero_deleteEnabled;

BOOL(*gfnHeroStats)
(unsigned int, _uidefaultstats *);
BOOL(*gfnHeroCreate)
(_uiheroinfo *);
BOOL(*gfnHeroDelete)
(_uiheroinfo *);

UI_Item SELHERO_DIALOG[] = {
	{ { 0, 0, 640, 480 }, UI_IMAGE, 0, 0, NULL, &ArtBackground },
	{ { 24, 161, 590, 35 }, UI_TEXT, UIS_CENTER | UIS_BIG, 0, title },
	{ { 30, 211, 180, 76 }, UI_IMAGE, 0, UI_NUM_CLASSES, NULL, &ArtHero },
	{ { 39, 323, 110, 21 }, UI_TEXT, UIS_RIGHT, 0, "Level:" },
	{ { 159, 323, 40, 21 }, UI_TEXT, UIS_CENTER, 0, textStats[0] },
	{ { 39, 358, 110, 21 }, UI_TEXT, UIS_RIGHT, 0, "Strength:" },
	{ { 159, 358, 40, 21 }, UI_TEXT, UIS_CENTER, 0, textStats[1] },
	{ { 39, 380, 110, 21 }, UI_TEXT, UIS_RIGHT, 0, "Magic:" },
	{ { 159, 380, 40, 21 }, UI_TEXT, UIS_CENTER, 0, textStats[2] },
	{ { 39, 401, 110, 21 }, UI_TEXT, UIS_RIGHT, 0, "Dexterity:" },
	{ { 159, 401, 40, 21 }, UI_TEXT, UIS_CENTER, 0, textStats[3] },
	{ { 39, 422, 110, 21 }, UI_TEXT, UIS_RIGHT, 0, "Vitality:" },
	{ { 159, 422, 40, 21 }, UI_TEXT, UIS_CENTER, 0, textStats[4] },
};

UI_Item SELLIST_DIALOG[] = {
	{ { 264, 211, 320, 33 }, UI_TEXT, UIS_CENTER | UIS_BIG, 0, "Select Hero" },
	{ { 265, 256, 320, 26 }, UI_LIST, UIS_CENTER | UIS_MED | UIS_GOLD, 0, listItems[0] },
	{ { 265, 282, 320, 26 }, UI_LIST, UIS_CENTER | UIS_MED | UIS_GOLD, 1, listItems[1] },
	{ { 265, 308, 320, 26 }, UI_LIST, UIS_CENTER | UIS_MED | UIS_GOLD, 2, listItems[2] },
	{ { 265, 334, 320, 26 }, UI_LIST, UIS_CENTER | UIS_MED | UIS_GOLD, 3, listItems[3] },
	{ { 265, 360, 320, 26 }, UI_LIST, UIS_CENTER | UIS_MED | UIS_GOLD, 4, listItems[4] },
	{ { 265, 386, 320, 26 }, UI_LIST, UIS_CENTER | UIS_MED | UIS_GOLD, 5, listItems[5] },
	{ { 239, 429, 120, 35 }, UI_BUTTON, UIS_CENTER | UIS_BIG | UIS_GOLD, 0, "OK", (void *)UiFocusNavigationSelect },
	{ { 364, 429, 120, 35 }, UI_BUTTON, UIS_CENTER | UIS_BIG | UIS_DISABLED, 0, "Delete", (void *)selhero_UiFocusNavigationYesNo },
	{ { 489, 429, 120, 35 }, UI_BUTTON, UIS_CENTER | UIS_BIG | UIS_GOLD, 0, "Cancel", (void *)UiFocusNavigationEsc },
};

UI_Item SELCLASS_DIALOG[] = {
	{ { 264, 211, 320, 33 }, UI_TEXT, UIS_CENTER | UIS_BIG, 0, "Choose Class" },
	{ { 264, 285, 320, 33 }, UI_LIST, UIS_CENTER | UIS_MED | UIS_GOLD, UI_WARRIOR, "Warrior" },
	{ { 264, 318, 320, 33 }, UI_LIST, UIS_CENTER | UIS_MED | UIS_GOLD, UI_ROGUE, "Rogue" },
	{ { 264, 352, 320, 33 }, UI_LIST, UIS_CENTER | UIS_MED | UIS_GOLD, UI_SORCERER, "Sorcerer" },
	{ { 279, 429, 140, 35 }, UI_BUTTON, UIS_CENTER | UIS_BIG | UIS_GOLD, 0, "OK", (void *)UiFocusNavigationSelect },
	{ { 429, 429, 140, 35 }, UI_BUTTON, UIS_CENTER | UIS_BIG | UIS_GOLD, 0, "Cancel", (void *)UiFocusNavigationEsc },
};

UI_Item ENTERNAME_DIALOG[] = {
	{ { 264, 211, 320, 33 }, UI_TEXT, UIS_CENTER | UIS_BIG, 0, "Enter Name" },
	{ { 265, 317, 320, 33 }, UI_EDIT, UIS_LIST | UIS_MED | UIS_GOLD, 15, selhero_heroInfo.name },
	{ { 279, 429, 140, 35 }, UI_BUTTON, UIS_CENTER | UIS_BIG | UIS_GOLD, 0, "OK", (void *)UiFocusNavigationSelect },
	{ { 429, 429, 140, 35 }, UI_BUTTON, UIS_CENTER | UIS_BIG | UIS_GOLD, 0, "Cancel", (void *)UiFocusNavigationEsc },
};

UI_Item SELLOAD_DIALOG[] = {
	{ { 264, 211, 320, 33 }, UI_TEXT, UIS_CENTER | UIS_BIG, 0, "Save File Exists" },
	{ { 265, 285, 320, 26 }, UI_LIST, UIS_CENTER | UIS_MED | UIS_GOLD, 0, "Load Game" },
	{ { 265, 318, 320, 26 }, UI_LIST, UIS_CENTER | UIS_MED | UIS_GOLD, 1, "New Game" },
	{ { 279, 427, 140, 35 }, UI_BUTTON, UIS_CENTER | UIS_VCENTER | UIS_BIG | UIS_GOLD, 0, "OK", (void *)UiFocusNavigationSelect },
	{ { 429, 427, 140, 35 }, UI_BUTTON, UIS_CENTER | UIS_VCENTER | UIS_BIG | UIS_GOLD, 0, "Cancel", (void *)UiFocusNavigationEsc },
};

void selhero_UiFocusNavigationYesNo()
{
	if (selhero_deleteEnabled)
		UiFocusNavigationYesNo();
}

void selhero_Free()
{
	mem_free_dbg(ArtBackground.data);
	ArtBackground.data = NULL;
	memset(listItems, 0, sizeof(listItems));
}

void selhero_SetStats()
{
	SELHERO_DIALOG[2].value = selhero_heroInfo.heroclass;
	sprintf(textStats[0], "%d", selhero_heroInfo.level);
	sprintf(textStats[1], "%d", selhero_heroInfo.strength);
	sprintf(textStats[2], "%d", selhero_heroInfo.magic);
	sprintf(textStats[3], "%d", selhero_heroInfo.dexterity);
	sprintf(textStats[4], "%d", selhero_heroInfo.vitality);
}

void selhero_List_Init()
{
	UiInitList(0, selhero_SaveCount, selhero_List_Focus, selhero_List_Select, selhero_List_Esc, SELLIST_DIALOG, size(SELLIST_DIALOG), false, selhero_List_DeleteYesNo);
	int i;
	for (i = 0; i < selhero_SaveCount && i < 6; i++) {
		sprintf(listItems[i], selhero_heros[i].name);
	}
	if (i < 6)
		sprintf(listItems[i], "New Hero");

	sprintf(title, "Single Player Characters");
	if (selhero_isMultiPlayer) {
		sprintf(title, "Multi Player Characters");
	}
}

void selhero_List_Focus(int value)
{
	int baseFlags = UIS_CENTER | UIS_BIG;
	if (selhero_SaveCount && value < selhero_SaveCount) {
		memcpy(&selhero_heroInfo, &selhero_heros[value], sizeof(selhero_heroInfo));
		selhero_SetStats();
		SELLIST_DIALOG[8].flags = baseFlags | UIS_GOLD;
		selhero_deleteEnabled = true;
		return;
	}

	SELHERO_DIALOG[2].value = UI_NUM_CLASSES;
	sprintf(textStats[0], "--");
	sprintf(textStats[1], "--");
	sprintf(textStats[2], "--");
	sprintf(textStats[3], "--");
	sprintf(textStats[4], "--");
	SELLIST_DIALOG[8].flags = baseFlags | UIS_DISABLED;
	selhero_deleteEnabled = false;
}

bool selhero_List_DeleteYesNo()
{
	selhero_navigateYesNo = selhero_deleteEnabled;

	return selhero_navigateYesNo;
}

void selhero_List_Select(int value)
{
	if (value == selhero_SaveCount) {
		UiInitList(0, 2, selhero_ClassSelector_Focus, selhero_ClassSelector_Select, selhero_ClassSelector_Esc, SELCLASS_DIALOG, size(SELCLASS_DIALOG));
		memset(&selhero_heroInfo.name, 0, sizeof(selhero_heroInfo.name));
		sprintf(title, "New Single Player Hero");
		if (selhero_isMultiPlayer) {
			sprintf(title, "New Multi Player Hero");
		}
		return;
	} else if (selhero_heroInfo.hassaved) {
		UiInitList(0, 1, selhero_Load_Focus, selhero_Load_Select, selhero_List_Init, SELLOAD_DIALOG, size(SELLOAD_DIALOG), true);
		sprintf(title, "Single Player Characters");
		return;
	}

	UiInitList(0, 0, NULL, NULL, NULL, NULL, 0);
	selhero_endMenu = true;
}

void selhero_List_Esc()
{
	UiInitList(0, 0, NULL, NULL, NULL, NULL, 0);
	selhero_endMenu = true;
	selhero_result = EXIT_MENU;
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
	sprintf(title, "New Single Player Hero");
	if (selhero_isMultiPlayer) {
		sprintf(title, "New Multi Player Hero");
	}
	memset(selhero_heroInfo.name, '\0', sizeof(selhero_heroInfo.name));
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
	UiInitList(0, 0, NULL, NULL, NULL, NULL, 0);
	gfnHeroCreate(&selhero_heroInfo);
	selhero_endMenu = true;
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
		selhero_result = LOAD_GAME;
		return;
	}

	selhero_result = NEW_GAME;
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
		selhero_result = *dlgresult;
		gfnHeroStats = fnstats;
		gfnHeroCreate = fncreate;
		gfnHeroDelete = fnremove;
		LoadBackgroundArt("ui_art\\selhero.pcx");

		selhero_navigateYesNo = false;

		selhero_SaveCount = 0;
		fninfo(SelHero_GetHeroInfo);

		if (selhero_SaveCount) {
			selhero_List_Init();
		} else {
			selhero_List_Select(selhero_SaveCount);
		}

		selhero_endMenu = false;
		while (!selhero_endMenu && !selhero_navigateYesNo) {
			UiRenderItems(SELHERO_DIALOG, size(SELHERO_DIALOG));
			UiRender();
		}
		BlackPalette();
		selhero_Free();

		if (selhero_navigateYesNo) {
			if (!UiSelHeroDelYesNoDialog(gfnHeroDelete, &selhero_heroInfo, selhero_isMultiPlayer))
				app_fatal("Unable to load Yes/No dialog");
		}
	} while (selhero_navigateYesNo);

	*dlgresult = selhero_result;
	strcpy(name, selhero_heroInfo.name);

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
}

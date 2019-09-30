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

UiItem SELHERO_DIALOG[] = {
	UiImage(&ArtBackground, { 0, 0, 640, 480 }),
	UiText(title, { 24, 161, 590, 35 }, UIS_CENTER | UIS_BIG),
	UiImage(&ArtHero, UI_NUM_CLASSES, { 30, 211, 180, 76 }),
	UiText("Level:", { 39, 323, 110, 21 }, UIS_RIGHT),
	UiText(textStats[0], { 159, 323, 40, 21 }, UIS_CENTER),
	UiText("Strength:", { 39, 358, 110, 21 }, UIS_RIGHT),
	UiText(textStats[1], { 159, 358, 40, 21 }, UIS_CENTER),
	UiText("Magic:", { 39, 380, 110, 21 }, UIS_RIGHT),
	UiText(textStats[2], { 159, 380, 40, 21 }, UIS_CENTER),
	UiText("Dexterity:", { 39, 401, 110, 21 }, UIS_RIGHT),
	UiText(textStats[3], { 159, 401, 40, 21 }, UIS_CENTER),
	UiText("Vitality:", { 39, 422, 110, 21 }, UIS_RIGHT),
	UiText(textStats[4], { 159, 422, 40, 21 }, UIS_CENTER),
};
UiImage *SELHERO_DIALOG_HERO_IMG = &SELHERO_DIALOG[2].image;

UiListItem SELLIST_DIALOG_ITEMS[] = {
	{ listItems[0], 0 },
	{ listItems[1], 1 },
	{ listItems[2], 2 },
	{ listItems[3], 3 },
	{ listItems[4], 4 },
	{ listItems[5], 5 },
};
UiItem SELLIST_DIALOG[] = {
	UiText("Select Hero", { 264, 211, 320, 33 }, UIS_CENTER | UIS_BIG),
	UiList(SELLIST_DIALOG_ITEMS, 265, 256, 320, 26, UIS_CENTER | UIS_MED | UIS_GOLD),
	UiButton("OK", &UiFocusNavigationSelect, { 239, 429, 120, 35 }, UIS_CENTER | UIS_BIG | UIS_GOLD),
	UiButton("Delete", &selhero_UiFocusNavigationYesNo, { 364, 429, 120, 35 }, UIS_CENTER | UIS_BIG | UIS_DISABLED),
	UiButton("Cancel", &UiFocusNavigationEsc, { 489, 429, 120, 35 }, UIS_CENTER | UIS_BIG | UIS_GOLD)
};
UiButton *SELLIST_DIALOG_DELETE_BUTTON = &SELLIST_DIALOG[3].button;

UiListItem SELCLAS_DIALOG_ITEMS[] = {
	{"Warrior", UI_WARRIOR},
	{"Rogue", UI_ROGUE},
	{"Sorcerer", UI_SORCERER}
};
UiItem SELCLASS_DIALOG[] = {
	UiText("Choose Class", { 264, 211, 320, 33 }, UIS_CENTER | UIS_BIG),
	UiList(SELCLAS_DIALOG_ITEMS, 264, 285, 320, 33, UIS_CENTER | UIS_MED | UIS_GOLD),
	UiButton("OK", &UiFocusNavigationSelect, { 279, 429, 140, 35 },UIS_CENTER | UIS_BIG | UIS_GOLD),
	UiButton("Cancel", &UiFocusNavigationEsc, { 429, 429, 140, 35 }, UIS_CENTER | UIS_BIG | UIS_GOLD)
};

UiItem ENTERNAME_DIALOG[] = {
	UiText("Enter Name", { 264, 211, 320, 33 }, UIS_CENTER | UIS_BIG),
	UiEdit(selhero_heroInfo.name, 15, { 265, 317, 320, 33 }, UIS_LIST | UIS_MED | UIS_GOLD),
	UiButton("OK", &UiFocusNavigationSelect, { 279, 429, 140, 35 },UIS_CENTER | UIS_BIG | UIS_GOLD),
	UiButton("Cancel", &UiFocusNavigationEsc, { 429, 429, 140, 35 }, UIS_CENTER | UIS_BIG | UIS_GOLD)
};

UiListItem SELLOAD_DIALOG_ITEMS[] = {
	{"Load Game", 0},
	{"New Game", 1}
};
UiItem SELLOAD_DIALOG[] = {
	UiText("Save File Exists", { 264, 211, 320, 33 }, UIS_CENTER | UIS_BIG),
	UiList(SELLOAD_DIALOG_ITEMS, 265, 285, 320, 33, UIS_CENTER | UIS_MED | UIS_GOLD),
	UiButton("OK", &UiFocusNavigationSelect, { 279, 427, 140, 35 },UIS_CENTER | UIS_VCENTER |UIS_BIG | UIS_GOLD),
	UiButton("Cancel", &UiFocusNavigationEsc, { 429, 427, 140, 35 }, UIS_CENTER | UIS_VCENTER |UIS_BIG | UIS_GOLD)
};

void selhero_UiFocusNavigationYesNo()
{
	if (selhero_deleteEnabled)
		UiFocusNavigationYesNo();
}

void selhero_Free()
{
	ArtBackground.Unload();
	memset(listItems, 0, sizeof(listItems));
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
		SELLIST_DIALOG_DELETE_BUTTON->flags = baseFlags | UIS_GOLD;
		selhero_deleteEnabled = true;
		return;
	}

	SELHERO_DIALOG_HERO_IMG->frame = UI_NUM_CLASSES;
	sprintf(textStats[0], "--");
	sprintf(textStats[1], "--");
	sprintf(textStats[2], "--");
	sprintf(textStats[3], "--");
	sprintf(textStats[4], "--");
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

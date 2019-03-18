#include "selgame.h"

#include "devilution.h"
#include "DiabloUI/diabloui.h"

namespace dvl {

char selgame_Lable[32];
char selgame_Ip[129] = "";
char selgame_Password[16] = "";
char selgame_Description[256];
bool selgame_enteringGame;
int selgame_selectedGame;
bool selgame_endMenu;
int *gdwPlayerId;
int gbDifficulty;

static _SNETPROGRAMDATA *m_client_info;
extern DWORD provider;

UI_Item SELUDPGAME_DIALOG[] = {
	{ { 0, 0, 640, 480 }, UI_IMAGE, 0, 0, NULL, &ArtBackground },
	{ { 24, 161, 590, 35 }, UI_TEXT, UIS_CENTER | UIS_BIG, 0, "Join TCP/UDP Games" },
	{ { 35, 211, 205, 33 }, UI_TEXT, UIS_MED, 0, "Description:" },
	{ { 35, 256, 205, 192 }, UI_TEXT, 0, 0, selgame_Description }, // Description
	{ { 300, 211, 295, 33 }, UI_TEXT, UIS_CENTER | UIS_BIG, 0, "Select Action" },
	{ { 305, 255, 285, 26 }, UI_LIST, UIS_CENTER | UIS_MED | UIS_GOLD, 0, "Create Game" },
	{ { 305, 281, 285, 26 }, UI_LIST, UIS_CENTER | UIS_MED | UIS_GOLD, 1, "Enter IP" },
	{ { 305, 307, 285, 26 }, UI_LIST, UIS_CENTER | UIS_MED | UIS_GOLD },
	{ { 305, 333, 285, 26 }, UI_LIST, UIS_CENTER | UIS_MED | UIS_GOLD },
	{ { 305, 359, 285, 26 }, UI_LIST, UIS_CENTER | UIS_MED | UIS_GOLD },
	{ { 305, 385, 285, 26 }, UI_LIST, UIS_CENTER | UIS_MED | UIS_GOLD },
	{ { 299, 427, 140, 35 }, UI_BUTTON, UIS_CENTER | UIS_VCENTER | UIS_BIG | UIS_GOLD, 0, "OK", (void *)UiFocusNavigationSelect },
	{ { 449, 427, 140, 35 }, UI_BUTTON, UIS_CENTER | UIS_VCENTER | UIS_BIG | UIS_GOLD, 0, "Cancel", (void *)UiFocusNavigationEsc },
};

UI_Item SELDIFF_DIALOG[] = {
	{ { 0, 0, 640, 480 }, UI_IMAGE, 0, 0, NULL, &ArtBackground },
	{ { 24, 161, 590, 35 }, UI_TEXT, UIS_CENTER | UIS_BIG, 0, "Create Game" },
	{ { 34, 211, 205, 33 }, UI_TEXT, UIS_CENTER | UIS_BIG, 0, selgame_Lable }, // DIFF
	{ { 35, 256, 205, 192 }, UI_TEXT, 0, 0, selgame_Description },             // Description
	{ { 299, 211, 295, 35 }, UI_TEXT, UIS_CENTER | UIS_BIG, 0, "Select Difficulty" },
	{ { 300, 282, 295, 26 }, UI_LIST, UIS_CENTER | UIS_MED | UIS_GOLD, DIFF_NORMAL, "Normal" },
	{ { 300, 308, 295, 26 }, UI_LIST, UIS_CENTER | UIS_MED | UIS_GOLD, DIFF_NIGHTMARE, "Nightmare" },
	{ { 300, 334, 295, 26 }, UI_LIST, UIS_CENTER | UIS_MED | UIS_GOLD, DIFF_HELL, "Hell" },
	{ { 299, 427, 140, 35 }, UI_BUTTON, UIS_CENTER | UIS_VCENTER | UIS_BIG | UIS_GOLD, 0, "OK", (void *)UiFocusNavigationSelect },
	{ { 449, 427, 140, 35 }, UI_BUTTON, UIS_CENTER | UIS_VCENTER | UIS_BIG | UIS_GOLD, 0, "Cancel", (void *)UiFocusNavigationEsc },
};

UI_Item ENTERIP_DIALOG[] = {
	{ { 0, 0, 640, 480 }, UI_IMAGE, 0, 0, NULL, &ArtBackground },
	{ { 24, 161, 590, 35 }, UI_TEXT, UIS_CENTER | UIS_BIG, 0, "Join TCP/UDP Games" },
	{ { 35, 211, 205, 33 }, UI_TEXT, UIS_MED, 0, "Description:" },
	{ { 35, 256, 205, 192 }, UI_TEXT, 0, 0, selgame_Description }, // Description
	{ { 305, 211, 285, 33 }, UI_TEXT, UIS_CENTER | UIS_BIG, 0, "Enter IP" },
	{ { 305, 314, 285, 33 }, UI_EDIT, UIS_LIST | UIS_MED | UIS_GOLD, 128, selgame_Ip },
	{ { 299, 427, 140, 35 }, UI_BUTTON, UIS_CENTER | UIS_VCENTER | UIS_BIG | UIS_GOLD, 0, "OK", (void *)UiFocusNavigationSelect },
	{ { 449, 427, 140, 35 }, UI_BUTTON, UIS_CENTER | UIS_VCENTER | UIS_BIG | UIS_GOLD, 0, "Cancel", (void *)UiFocusNavigationEsc },
};

UI_Item ENTERPASSWORD_DIALOG[] = {
	{ { 0, 0, 640, 480 }, UI_IMAGE, 0, 0, NULL, &ArtBackground },
	{ { 24, 161, 590, 35 }, UI_TEXT, UIS_CENTER | UIS_BIG, 0, "Join TCP/UDP Games" },
	{ { 35, 211, 205, 33 }, UI_TEXT, UIS_MED, 0, "Description:" },
	{ { 35, 256, 205, 192 }, UI_TEXT, 0, 0, selgame_Description }, // Description
	{ { 305, 211, 285, 33 }, UI_TEXT, UIS_CENTER | UIS_BIG, 0, "Enter Password" },
	{ { 305, 314, 285, 33 }, UI_EDIT, UIS_LIST | UIS_MED | UIS_GOLD, 15, selgame_Password },
	{ { 299, 427, 140, 35 }, UI_BUTTON, UIS_CENTER | UIS_VCENTER | UIS_BIG | UIS_GOLD, 0, "OK", (void *)UiFocusNavigationSelect },
	{ { 449, 427, 140, 35 }, UI_BUTTON, UIS_CENTER | UIS_VCENTER | UIS_BIG | UIS_GOLD, 0, "Cancel", (void *)UiFocusNavigationEsc },
};

void selgame_Free()
{
	mem_free_dbg(ArtBackground.data);
	ArtBackground.data = NULL;
}

void selgame_GameSelection_Init()
{
	selgame_enteringGame = false;
	selgame_selectedGame = 0;

	if (provider == 'SCBL') {
		selgame_enteringGame = true;
		selgame_GameSelection_Select(0);
		return;
	}

	SRegLoadString("Phone Book", "Entry1", 0, selgame_Ip, 128);
	UiInitList(0, 1, selgame_GameSelection_Focus, selgame_GameSelection_Select, selgame_GameSelection_Esc, SELUDPGAME_DIALOG, size(SELUDPGAME_DIALOG));
}

void selgame_GameSelection_Focus(int value)
{
	switch (value) {
	case 0:
		sprintf(selgame_Description, "Create a new game with a difficulty setting of your choice.");
		break;
	case 1:
		sprintf(selgame_Description, "Enter an IP and join a game already in progress at that address.");
		break;
	}

	for (auto &item : SELUDPGAME_DIALOG) {
		if (item.caption != NULL && !(item.flags & (UIS_VCENTER | UIS_CENTER)))
			WordWrap(&item);
	}
}

void selgame_GameSelection_Select(int value)
{
	selgame_enteringGame = true;
	selgame_selectedGame = value;

	switch (value) {
	case 0:
		UiInitList(0, 2, selgame_Diff_Focus, selgame_Diff_Select, selgame_Diff_Esc, SELDIFF_DIALOG, size(SELDIFF_DIALOG));
		break;
	case 1:
		UiInitList(0, 0, NULL, selgame_Password_Init, selgame_GameSelection_Init, ENTERIP_DIALOG, size(ENTERIP_DIALOG));
		break;
	}
}

void selgame_GameSelection_Esc()
{
	UiInitList(0, 0, NULL, NULL, NULL, NULL, 0);
	selgame_enteringGame = false;
	selgame_endMenu = true;
}

void selgame_Diff_Focus(int value)
{
	switch (value) {
	case DIFF_NORMAL:
		sprintf(selgame_Lable, "Normal");
		sprintf(selgame_Description, "Normal Difficulty\nThis is where a starting character should begin the quest to defeat Diablo.");
		break;
	case DIFF_NIGHTMARE:
		sprintf(selgame_Lable, "Nightmare");
		sprintf(selgame_Description, "Nightmare Difficulty\nThe denizens of the Labyrinth have been bolstered and will prove to be a greater challenge. This is recommended for experienced characters only.");
		break;
	case DIFF_HELL:
		sprintf(selgame_Lable, "Hell");
		sprintf(selgame_Description, "Hell Difficulty\nThe most powerful of the underworld's creatures lurk at the gateway into Hell. Only the most experienced characters should venture in this realm.");
		break;
	}

	for (auto &item : SELDIFF_DIALOG) {
		if (item.caption != NULL && !(item.flags & (UIS_VCENTER | UIS_CENTER)))
			WordWrap(&item);
	}
}

void selgame_Diff_Select(int value)
{
	gbDifficulty = value;

	if (provider == 'SCBL') {
		selgame_Password_Select(0);
		return;
	}

	selgame_Password_Init(0);
}

void selgame_Diff_Esc()
{
	if (provider == 'SCBL') {
		selgame_GameSelection_Esc();
		return;
	}

	selgame_GameSelection_Init();
}

void selgame_Password_Init(int value)
{
	memset(&selgame_Password, 0, sizeof(selgame_Password));
	UiInitList(0, 0, NULL, selgame_Password_Select, selgame_Password_Esc, ENTERPASSWORD_DIALOG, size(ENTERPASSWORD_DIALOG));
}

void selgame_Password_Select(int value)
{
	UiInitList(0, 0, NULL, NULL, NULL, NULL, 0);
	selgame_endMenu = true;

	if (selgame_selectedGame) {
		SRegSaveString("Phone Book", "Entry1", 0, selgame_Ip);
		if (!SNetJoinGame(selgame_selectedGame, selgame_Ip, selgame_Password, NULL, NULL, gdwPlayerId)) {
			DrawDlg("Unable to establish a connection. A game of Devilution 0.2.0 was not detected at the specified IP address.");
			selgame_Password_Init(selgame_selectedGame);
		}
		return;
	}

	_gamedata *info = m_client_info->initdata;
	info->bDiff = gbDifficulty;

	if (!SNetCreateGame(NULL, selgame_Password, NULL, 0, (char *)info, sizeof(_gamedata), MAX_PLRS, NULL, NULL, gdwPlayerId)) {
		DrawDlg("Unable to create game.");
		selgame_Password_Init(0);
	}
}

void selgame_Password_Esc()
{
	selgame_GameSelection_Select(selgame_selectedGame);
}

int UiSelectGame(int a1, _SNETPROGRAMDATA *client_info, _SNETPLAYERDATA *user_info, _SNETUIDATA *ui_info,
    _SNETVERSIONDATA *file_info, int *playerId)
{
	gdwPlayerId = playerId;
	m_client_info = client_info;
	LoadBackgroundArt("ui_art\\selgame.pcx");
	selgame_GameSelection_Init();

	selgame_endMenu = false;
	while (!selgame_endMenu) {
		UiRender();
	}
	BlackPalette();
	selgame_Free();

	return selgame_enteringGame;
}

}

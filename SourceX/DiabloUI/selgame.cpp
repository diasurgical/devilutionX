#include "selgame.h"

#include "devilution.h"
#include "DiabloUI/diabloui.h"

namespace dvl {

char selgame_Label[32];
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

UiText SELGAME_DESCRIPTION(selgame_Description, { 35, 256, 205, 192 });
UiListItem SELUDPGAME_DIALOG_ITEMS[] = {
	{ "Create Game", 0 },
	{ "Enter IP", 1 },
};
UiItem SELUDPGAME_DIALOG[] = {
	UiImage(&ArtBackground, { 0, 0, 640, 480 }),
	UiText("Join TCP/UDP Games", { 24, 161, 590, 35 }, UIS_CENTER | UIS_BIG),
	UiText("Description:", { 35, 256, 205, 192 }, UIS_MED),
	SELGAME_DESCRIPTION,
	UiText("Select Action", { 300, 211, 295, 33 }, UIS_CENTER | UIS_BIG),
	UiList(SELUDPGAME_DIALOG_ITEMS, 305, 255, 285, 26, UIS_CENTER | UIS_MED | UIS_GOLD),
	UiButton("OK", &UiFocusNavigationSelect, { 299, 427, 140, 35 }, UIS_CENTER | UIS_VCENTER | UIS_BIG | UIS_GOLD),
	UiButton("CANCEL", &UiFocusNavigationEsc, { 449, 427, 140, 35 }, UIS_CENTER | UIS_VCENTER | UIS_BIG | UIS_GOLD)
};

UiListItem SELDIFF_DIALOG_ITEMS[] = {
	{ "Normal", DIFF_NORMAL },
	{ "Nightmare", DIFF_NIGHTMARE },
	{ "Hell", DIFF_HELL }
};
UiItem SELDIFF_DIALOG[] = {
	UiImage(&ArtBackground, { 0, 0, 640, 480 }),
	UiText("Create Game", { 24, 161, 590, 35 }, UIS_CENTER | UIS_BIG),
	UiText(selgame_Label, { 34, 211, 205, 33 }, UIS_CENTER | UIS_BIG), // DIFF
	SELGAME_DESCRIPTION,
	UiText("Select Difficulty", { 299, 211, 295, 35 }, UIS_CENTER | UIS_BIG),
	UiList(SELDIFF_DIALOG_ITEMS, 300, 282, 295, 26, UIS_CENTER | UIS_MED | UIS_GOLD),
	UiButton("OK", &UiFocusNavigationSelect, { 299, 427, 140, 35 }, UIS_CENTER | UIS_VCENTER | UIS_BIG | UIS_GOLD),
	UiButton("CANCEL", &UiFocusNavigationEsc, { 449, 427, 140, 35 }, UIS_CENTER | UIS_VCENTER | UIS_BIG | UIS_GOLD)
};

UiItem ENTERIP_DIALOG[] = {
	UiImage(&ArtBackground, { 0, 0, 640, 480 }),
	UiText("Join TCP/UDP Games", { 24, 161, 590, 35 }, UIS_CENTER | UIS_BIG),
	UiText("Description:", { 35, 211, 205, 33 }, UIS_MED),
	SELGAME_DESCRIPTION,
	UiText("Enter IP", { 305, 211, 285, 33 }, UIS_CENTER | UIS_BIG),
	UiEdit(selgame_Ip, 128, { 305, 314, 285, 33 }, UIS_LIST | UIS_MED | UIS_GOLD),
	UiButton("OK", &UiFocusNavigationSelect, { 299, 427, 140, 35 }, UIS_CENTER | UIS_VCENTER | UIS_BIG | UIS_GOLD),
	UiButton("CANCEL", &UiFocusNavigationEsc, { 449, 427, 140, 35 }, UIS_CENTER | UIS_VCENTER | UIS_BIG | UIS_GOLD)
};

UiItem ENTERPASSWORD_DIALOG[] = {
	UiImage(&ArtBackground, { 0, 0, 640, 480 }),
	UiText("Join TCP/UDP Games", { 24, 161, 590, 35 }, UIS_CENTER | UIS_BIG),
	UiText("Description:", { 35, 211, 205, 33 }, UIS_MED),
	SELGAME_DESCRIPTION,
	UiText("Enter Password", { 305, 211, 285, 33 }, UIS_CENTER | UIS_BIG),
	UiEdit(selgame_Password, 15, { 305, 314, 285, 33 }, UIS_LIST | UIS_MED | UIS_GOLD),
	UiButton("OK", &UiFocusNavigationSelect, { 299, 427, 140, 35 }, UIS_CENTER | UIS_VCENTER | UIS_BIG | UIS_GOLD),
	UiButton("CANCEL", &UiFocusNavigationEsc, { 449, 427, 140, 35 }, UIS_CENTER | UIS_VCENTER | UIS_BIG | UIS_GOLD)
};

void selgame_Free()
{
	ArtBackground.Unload();
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
		strcpy(selgame_Description, "Create a new game with a difficulty setting of your choice.");
		break;
	case 1:
		strcpy(selgame_Description, "Enter an IP and join a game already in progress at that address.");
		break;
	}
	WordWrap(&SELGAME_DESCRIPTION);
}

void selgame_GameSelection_Select(int value)
{
	selgame_enteringGame = true;
	selgame_selectedGame = value;

	switch (value) {
	case 0:
		UiInitList(0, NUM_DIFFICULTIES - 1, selgame_Diff_Focus, selgame_Diff_Select, selgame_Diff_Esc, SELDIFF_DIALOG, size(SELDIFF_DIALOG));
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
		strcpy(selgame_Label, "Normal");
		strcpy(selgame_Description, "Normal Difficulty\nThis is where a starting character should begin the quest to defeat Diablo.");
		break;
	case DIFF_NIGHTMARE:
		strcpy(selgame_Label, "Nightmare");
		strcpy(selgame_Description, "Nightmare Difficulty\nThe denizens of the Labyrinth have been bolstered and will prove to be a greater challenge. This is recommended for experienced characters only.");
		break;
	case DIFF_HELL:
		strcpy(selgame_Label, "Hell");
		strcpy(selgame_Description, "Hell Difficulty\nThe most powerful of the underworld's creatures lurk at the gateway into Hell. Only the most experienced characters should venture in this realm.");
		break;
	}
	WordWrap(&SELGAME_DESCRIPTION);
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

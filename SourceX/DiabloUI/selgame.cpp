#include "selgame.h"

#include "all.h"
#include "config.h"
#include "DiabloUI/diabloui.h"
#include "DiabloUI/text.h"
#include "DiabloUI/dialogs.h"
#include "DiabloUI/selok.h"

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
int heroLevel;

static _SNETPROGRAMDATA *m_client_info;
extern int provider;

#define DESCRIPTION_WIDTH 205

namespace {

char title[32];

std::vector<UiListItem *> vecSelGameDlgItems;
std::vector<UiItemBase *> vecSelGameDialog;

} // namespace

void selgame_FreeVectors()
{
	for (std::size_t i = 0; i < vecSelGameDlgItems.size(); i++) {
		UiListItem *pUIItem = vecSelGameDlgItems[i];
		delete pUIItem;
	}
	vecSelGameDlgItems.clear();

	for (std::size_t i = 0; i < vecSelGameDialog.size(); i++) {
		UiItemBase *pUIItem = vecSelGameDialog[i];
		delete pUIItem;
	}
	vecSelGameDialog.clear();
}

void selgame_Free()
{
	ArtBackground.Unload();

	selgame_FreeVectors();
}

void selgame_GameSelection_Init()
{
	selgame_enteringGame = false;
	selgame_selectedGame = 0;

	if (provider == SELCONN_LOOPBACK) {
		selgame_enteringGame = true;
		selgame_GameSelection_Select(0);
		return;
	}

	getIniValue("Phone Book", "Entry1", selgame_Ip, 128);

	selgame_FreeVectors();

	UiAddBackground(&vecSelGameDialog);
	UiAddLogo(&vecSelGameDialog);

	SDL_Rect rect1 = { PANEL_LEFT + 24, 161, 590, 35 };
	vecSelGameDialog.push_back(new UiArtText("Client-Server (TCP)", rect1, UIS_CENTER | UIS_BIG));

	SDL_Rect rect2 = { PANEL_LEFT + 35, 211, 205, 192 };
	vecSelGameDialog.push_back(new UiArtText("Description:", rect2, UIS_MED));

	SDL_Rect rect3 = { PANEL_LEFT + 35, 256, DESCRIPTION_WIDTH, 192 };
	vecSelGameDialog.push_back(new UiArtText(selgame_Description, rect3));

	SDL_Rect rect4 = { PANEL_LEFT + 300, 211, 295, 33 };
	vecSelGameDialog.push_back(new UiArtText("Select Action", rect4, UIS_CENTER | UIS_BIG));

	vecSelGameDlgItems.push_back(new UiListItem("Create Game", 0));
	vecSelGameDlgItems.push_back(new UiListItem("Join Game", 1));

	vecSelGameDialog.push_back(new UiList(vecSelGameDlgItems, PANEL_LEFT + 305, 255, 285, 26, UIS_CENTER | UIS_MED | UIS_GOLD));

	SDL_Rect rect5 = { PANEL_LEFT + 299, 427, 140, 35 };
	vecSelGameDialog.push_back(new UiArtTextButton("OK", &UiFocusNavigationSelect, rect5, UIS_CENTER | UIS_VCENTER | UIS_BIG | UIS_GOLD));

	SDL_Rect rect6 = { PANEL_LEFT + 449, 427, 140, 35 };
	vecSelGameDialog.push_back(new UiArtTextButton("CANCEL", &UiFocusNavigationEsc, rect6, UIS_CENTER | UIS_VCENTER | UIS_BIG | UIS_GOLD));

	UiInitList(0, 1, selgame_GameSelection_Focus, selgame_GameSelection_Select, selgame_GameSelection_Esc, vecSelGameDialog);
}

void selgame_GameSelection_Focus(int value)
{
	switch (value) {
	case 0:
		strncpy(selgame_Description, "Create a new game with a difficulty setting of your choice.", sizeof(selgame_Description) - 1);
		break;
	case 1:
		strncpy(selgame_Description, "Enter an IP or a hostname and join a game already in progress at that address.", sizeof(selgame_Description) - 1);
		break;
	}
	WordWrapArtStr(selgame_Description, DESCRIPTION_WIDTH);
}

/**
 * @brief Load the current hero level from save file
 * @param pInfo Hero info
 * @return always true
 */
BOOL UpdateHeroLevel(_uiheroinfo *pInfo)
{
	if (strcasecmp(pInfo->name, gszHero) == 0)
		heroLevel = pInfo->level;

	return true;
}

void selgame_GameSelection_Select(int value)
{
	selgame_enteringGame = true;
	selgame_selectedGame = value;

	gfnHeroInfo(UpdateHeroLevel);

	selgame_FreeVectors();

	UiAddBackground(&vecSelGameDialog);
	UiAddLogo(&vecSelGameDialog);

	SDL_Rect rect1 = { PANEL_LEFT + 24, 161, 590, 35 };
	vecSelGameDialog.push_back(new UiArtText(title, rect1, UIS_CENTER | UIS_BIG));

	SDL_Rect rect2 = { PANEL_LEFT + 34, 211, 205, 33 };
	vecSelGameDialog.push_back(new UiArtText(selgame_Label, rect2, UIS_CENTER | UIS_BIG));

	SDL_Rect rect3 = { PANEL_LEFT + 35, 256, DESCRIPTION_WIDTH, 192 };
	vecSelGameDialog.push_back(new UiArtText(selgame_Description, rect3));

	switch (value) {
	case 0: {
		strncpy(title, "Create Game", sizeof(title) - 1);

		SDL_Rect rect4 = { PANEL_LEFT + 299, 211, 295, 35 };
		vecSelGameDialog.push_back(new UiArtText("Select Difficulty", rect4, UIS_CENTER | UIS_BIG));

		vecSelGameDlgItems.push_back(new UiListItem("Normal", DIFF_NORMAL));
		vecSelGameDlgItems.push_back(new UiListItem("Nightmare", DIFF_NIGHTMARE));
		vecSelGameDlgItems.push_back(new UiListItem("Hell", DIFF_HELL));

		vecSelGameDialog.push_back(new UiList(vecSelGameDlgItems, PANEL_LEFT + 300, 282, 295, 26, UIS_CENTER | UIS_MED | UIS_GOLD));

		SDL_Rect rect5 = { PANEL_LEFT + 299, 427, 140, 35 };
		vecSelGameDialog.push_back(new UiArtTextButton("OK", &UiFocusNavigationSelect, rect5, UIS_CENTER | UIS_VCENTER | UIS_BIG | UIS_GOLD));

		SDL_Rect rect6 = { PANEL_LEFT + 449, 427, 140, 35 };
		vecSelGameDialog.push_back(new UiArtTextButton("CANCEL", &UiFocusNavigationEsc, rect6, UIS_CENTER | UIS_VCENTER | UIS_BIG | UIS_GOLD));

		UiInitList(0, NUM_DIFFICULTIES - 1, selgame_Diff_Focus, selgame_Diff_Select, selgame_Diff_Esc, vecSelGameDialog);
		break;
	}
	case 1:
		strncpy(title, "Join TCP Games", sizeof(title) - 1);

		SDL_Rect rect4 = { PANEL_LEFT + 305, 211, 285, 33 };
		vecSelGameDialog.push_back(new UiArtText("Enter address", rect4, UIS_CENTER | UIS_BIG));

		SDL_Rect rect5 = { PANEL_LEFT + 305, 314, 285, 33 };
		vecSelGameDialog.push_back(new UiEdit(selgame_Ip, 128, rect5, UIS_MED | UIS_GOLD));

		SDL_Rect rect6 = { PANEL_LEFT + 299, 427, 140, 35 };
		vecSelGameDialog.push_back(new UiArtTextButton("OK", &UiFocusNavigationSelect, rect6, UIS_CENTER | UIS_VCENTER | UIS_BIG | UIS_GOLD));

		SDL_Rect rect7 = { PANEL_LEFT + 449, 427, 140, 35 };
		vecSelGameDialog.push_back(new UiArtTextButton("CANCEL", &UiFocusNavigationEsc, rect7, UIS_CENTER | UIS_VCENTER | UIS_BIG | UIS_GOLD));

		UiInitList(0, 0, NULL, selgame_Password_Init, selgame_GameSelection_Init, vecSelGameDialog);
		break;
	}
}

void selgame_GameSelection_Esc()
{
	UiInitList_clear();
	selgame_enteringGame = false;
	selgame_endMenu = true;
}

void selgame_Diff_Focus(int value)
{
	switch (value) {
	case DIFF_NORMAL:
		strncpy(selgame_Label, "Normal", sizeof(selgame_Label) - 1);
		strncpy(selgame_Description, "Normal Difficulty\nThis is where a starting character should begin the quest to defeat Diablo.", sizeof(selgame_Description) - 1);
		break;
	case DIFF_NIGHTMARE:
		strncpy(selgame_Label, "Nightmare", sizeof(selgame_Label) - 1);
		strncpy(selgame_Description, "Nightmare Difficulty\nThe denizens of the Labyrinth have been bolstered and will prove to be a greater challenge. This is recommended for experienced characters only.", sizeof(selgame_Description) - 1);
		break;
	case DIFF_HELL:
		strncpy(selgame_Label, "Hell", sizeof(selgame_Label) - 1);
		strncpy(selgame_Description, "Hell Difficulty\nThe most powerful of the underworld's creatures lurk at the gateway into Hell. Only the most experienced characters should venture in this realm.", sizeof(selgame_Description) - 1);
		break;
	}
	WordWrapArtStr(selgame_Description, DESCRIPTION_WIDTH);
}

bool IsDifficultyAllowed(int value)
{
	if (value == 0 || (value == 1 && heroLevel >= 20) || (value == 2 && heroLevel >= 30)) {
		return true;
	}

	selgame_Free();

	if (value == 1)
		UiSelOkDialog(title, "Your character must reach level 20 before you can enter a multiplayer game of Nightmare difficulty.", false);
	if (value == 2)
		UiSelOkDialog(title, "Your character must reach level 30 before you can enter a multiplayer game of Hell difficulty.", false);

	LoadBackgroundArt("ui_art\\selgame.pcx");

	return false;
}

void selgame_Diff_Select(int value)
{
	if (!IsDifficultyAllowed(value)) {
		selgame_GameSelection_Select(0);
		return;
	}

	gbDifficulty = value;

	if (provider == SELCONN_LOOPBACK) {
		selgame_Password_Select(0);
		return;
	}

	selgame_Password_Init(0);
}

void selgame_Diff_Esc()
{
	if (provider == SELCONN_LOOPBACK) {
		selgame_GameSelection_Esc();
		return;
	}

	selgame_GameSelection_Init();
}

void selgame_Password_Init(int value)
{
	memset(&selgame_Password, 0, sizeof(selgame_Password));

	selgame_FreeVectors();

	UiAddBackground(&vecSelGameDialog);
	UiAddLogo(&vecSelGameDialog);

	SDL_Rect rect1 = { PANEL_LEFT + 24, 161, 590, 35 };
	vecSelGameDialog.push_back(new UiArtText("Client-Server (TCP)", rect1, UIS_CENTER | UIS_BIG));

	SDL_Rect rect2 = { PANEL_LEFT + 35, 211, 205, 192 };
	vecSelGameDialog.push_back(new UiArtText("Description:", rect2, UIS_MED));

	SDL_Rect rect3 = { PANEL_LEFT + 35, 256, DESCRIPTION_WIDTH, 192 };
	vecSelGameDialog.push_back(new UiArtText(selgame_Description, rect3));

	SDL_Rect rect4 = { PANEL_LEFT + 305, 211, 285, 33 };
	vecSelGameDialog.push_back(new UiArtText("Enter Password", rect4, UIS_CENTER | UIS_BIG));

	SDL_Rect rect5 = { PANEL_LEFT + 305, 314, 285, 33 };
	vecSelGameDialog.push_back(new UiEdit(selgame_Password, 15, rect5, UIS_MED | UIS_GOLD));

	SDL_Rect rect6 = { PANEL_LEFT + 299, 427, 140, 35 };
	vecSelGameDialog.push_back(new UiArtTextButton("OK", &UiFocusNavigationSelect, rect6, UIS_CENTER | UIS_VCENTER | UIS_BIG | UIS_GOLD));

	SDL_Rect rect7 = { PANEL_LEFT + 449, 427, 140, 35 };
	vecSelGameDialog.push_back(new UiArtTextButton("CANCEL", &UiFocusNavigationEsc, rect7, UIS_CENTER | UIS_VCENTER | UIS_BIG | UIS_GOLD));

	UiInitList(0, 0, NULL, selgame_Password_Select, selgame_Password_Esc, vecSelGameDialog);
}

void selgame_Password_Select(int value)
{
	if (selgame_selectedGame) {
		setIniValue("Phone Book", "Entry1", selgame_Ip);
		if (SNetJoinGame(selgame_selectedGame, selgame_Ip, selgame_Password, NULL, NULL, gdwPlayerId)) {
			if (!IsDifficultyAllowed(m_client_info->initdata->bDiff)) {
				selgame_GameSelection_Select(1);
				return;
			}

			UiInitList_clear();
			selgame_endMenu = true;
		} else {
			selgame_Free();
			UiSelOkDialog("Multi Player Game", SDL_GetError(), false);
			LoadBackgroundArt("ui_art\\selgame.pcx");
			selgame_Password_Init(selgame_selectedGame);
		}
		return;
	}

	_gamedata *info = m_client_info->initdata;
	info->bDiff = gbDifficulty;

	if (SNetCreateGame(NULL, selgame_Password, NULL, 0, (char *)info, sizeof(_gamedata), MAX_PLRS, NULL, NULL, gdwPlayerId)) {
		UiInitList_clear();
		selgame_endMenu = true;
	} else {
		selgame_Free();
		UiSelOkDialog("Multi Player Game", SDL_GetError(), false);
		LoadBackgroundArt("ui_art\\selgame.pcx");
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
		UiClearScreen();
		UiPollAndRender();
	}
	selgame_Free();

	return selgame_enteringGame;
}
} // namespace dvl

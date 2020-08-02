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

constexpr UiArtTextButton SELGAME_OK = UiArtTextButton("OK", &UiFocusNavigationSelect, { PANEL_LEFT + 299, 427, 140, 35 }, UIS_CENTER | UIS_VCENTER | UIS_BIG | UIS_GOLD);
constexpr UiArtTextButton SELGAME_CANCEL = UiArtTextButton("CANCEL", &UiFocusNavigationEsc, { PANEL_LEFT + 449, 427, 140, 35 }, UIS_CENTER | UIS_VCENTER | UIS_BIG | UIS_GOLD);

UiArtText SELGAME_DESCRIPTION(selgame_Description, { PANEL_LEFT + 35, 256, 205, 192 });

namespace {

char title[32];
UiListItem SELDIFF_DIALOG_ITEMS[] = {
	{ "Normal", DIFF_NORMAL },
	{ "Nightmare", DIFF_NIGHTMARE },
	{ "Hell", DIFF_HELL }
};
UiItem SELDIFF_DIALOG[] = {
	MAINMENU_BACKGROUND,
	MAINMENU_LOGO,
	UiArtText(title, { PANEL_LEFT + 24, 161, 590, 35 }, UIS_CENTER | UIS_BIG),
	UiArtText(selgame_Label, { PANEL_LEFT + 34, 211, 205, 33 }, UIS_CENTER | UIS_BIG), // DIFF
	SELGAME_DESCRIPTION,
	UiArtText("Select Difficulty", { PANEL_LEFT + 299, 211, 295, 35 }, UIS_CENTER | UIS_BIG),
	UiList(SELDIFF_DIALOG_ITEMS, PANEL_LEFT + 300, 282, 295, 26, UIS_CENTER | UIS_MED | UIS_GOLD),
	SELGAME_OK,
	SELGAME_CANCEL,
};

constexpr UiArtText SELUDPGAME_TITLE = UiArtText(title, { PANEL_LEFT + 24, 161, 590, 35 }, UIS_CENTER | UIS_BIG);
constexpr UiArtText SELUDPGAME_DESCRIPTION_LABEL = UiArtText("Description:", { PANEL_LEFT + 35, 211, 205, 192 }, UIS_MED);

UiListItem SELUDPGAME_DIALOG_ITEMS[] = {
	{ "Create Game", 0 },
	{ "Join Game", 1 },
};
UiItem SELUDPGAME_DIALOG[] = {
	MAINMENU_BACKGROUND,
	MAINMENU_LOGO,
	SELUDPGAME_TITLE,
	SELUDPGAME_DESCRIPTION_LABEL,
	SELGAME_DESCRIPTION,
	UiArtText("Select Action", { PANEL_LEFT + 300, 211, 295, 33 }, UIS_CENTER | UIS_BIG),
	UiList(SELUDPGAME_DIALOG_ITEMS, PANEL_LEFT + 305, 255, 285, 26, UIS_CENTER | UIS_MED | UIS_GOLD),
	SELGAME_OK,
	SELGAME_CANCEL,
};

UiItem ENTERIP_DIALOG[] = {
	MAINMENU_BACKGROUND,
	MAINMENU_LOGO,
	SELUDPGAME_TITLE,
	SELUDPGAME_DESCRIPTION_LABEL,
	SELGAME_DESCRIPTION,
	UiArtText("Enter address", { PANEL_LEFT + 305, 211, 285, 33 }, UIS_CENTER | UIS_BIG),

	UiEdit(selgame_Ip, 128, { PANEL_LEFT + 305, 314, 285, 33 }, UIS_MED | UIS_GOLD),
	SELGAME_OK,
	SELGAME_CANCEL,
};

UiItem ENTERPASSWORD_DIALOG[] = {
	MAINMENU_BACKGROUND,
	MAINMENU_LOGO,
	SELUDPGAME_TITLE,
	SELUDPGAME_DESCRIPTION_LABEL,
	SELGAME_DESCRIPTION,
	UiArtText("Enter Password", { PANEL_LEFT + 305, 211, 285, 33 }, UIS_CENTER | UIS_BIG),
	UiEdit(selgame_Password, 15, { PANEL_LEFT + 305, 314, 285, 33 }, UIS_MED | UIS_GOLD),
	SELGAME_OK,
	SELGAME_CANCEL,
};

} // namespace

void selgame_Free()
{
	ArtBackground.Unload();
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
	strcpy(title, "Client-Server (TCP)");
	UiInitList(0, 1, selgame_GameSelection_Focus, selgame_GameSelection_Select, selgame_GameSelection_Esc, SELUDPGAME_DIALOG, size(SELUDPGAME_DIALOG));
}

void selgame_GameSelection_Focus(int value)
{
	switch (value) {
	case 0:
		strcpy(selgame_Description, "Create a new game with a difficulty setting of your choice.");
		break;
	case 1:
		strcpy(selgame_Description, "Enter an IP or a hostname and join a game already in progress at that address.");
		break;
	}
	WordWrapArtStr(selgame_Description, SELGAME_DESCRIPTION.rect.w);
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

	switch (value) {
	case 0:
		strcpy(title, "Create Game");
		UiInitList(0, NUM_DIFFICULTIES - 1, selgame_Diff_Focus, selgame_Diff_Select, selgame_Diff_Esc, SELDIFF_DIALOG, size(SELDIFF_DIALOG));
		break;
	case 1:
		strcpy(title, "Join TCP Games");
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
	WordWrapArtStr(selgame_Description, SELGAME_DESCRIPTION.rect.w);
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
	UiInitList(0, 0, NULL, selgame_Password_Select, selgame_Password_Esc, ENTERPASSWORD_DIALOG, size(ENTERPASSWORD_DIALOG));
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

			UiInitList(0, 0, NULL, NULL, NULL, NULL, 0);
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
		UiInitList(0, 0, NULL, NULL, NULL, NULL, 0);
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

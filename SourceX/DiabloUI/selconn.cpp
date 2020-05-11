#include "selconn.h"

#include "all.h"
#include "DiabloUI/diabloui.h"
#include "DiabloUI/text.h"

namespace dvl {

char selconn_MaxPlayers[21];
char selconn_Description[64];
char selconn_Gateway[129];
bool selconn_ReturnValue = false;
bool selconn_EndMenu = false;
_SNETPROGRAMDATA *selconn_ClientInfo;
_SNETPLAYERDATA *selconn_UserInfo;
_SNETUIDATA *selconn_UiInfo;
_SNETVERSIONDATA *selconn_FileInfo;

int provider;

std::vector<UiListItem *> vecConnItems;
std::vector<UiItemBase *> vecSelConnDlg;

#define DESCRIPTION_WIDTH 205

void selconn_Load()
{
	LoadBackgroundArt("ui_art\\selconn.pcx");

	// vecConnItems Should be in the same order as conn_type (See enums.h)
#ifndef NONET
	vecConnItems.push_back(new UiListItem("Client-Server (TCP)", SELCONN_TCP));
#ifdef BUGGY
	vecConnItems.push_back(new UiListItem("Peer-to-Peer (UDP)", SELCONN_UDP));
#endif
#endif
	vecConnItems.push_back(new UiListItem("Loopback", SELCONN_LOOPBACK));

	UiAddBackground(&vecSelConnDlg);
	UiAddLogo(&vecSelConnDlg);

	SDL_Rect rect1 = { PANEL_LEFT + 24, 161, 590, 35 };
	vecSelConnDlg.push_back(new UiArtText("Multi Player Game", rect1, UIS_CENTER | UIS_BIG));

	SDL_Rect rect2 = { PANEL_LEFT + 35, 218, DESCRIPTION_WIDTH, 21 };
	vecSelConnDlg.push_back(new UiArtText(selconn_MaxPlayers, rect2));

	SDL_Rect rect3 = { PANEL_LEFT + 35, 256, DESCRIPTION_WIDTH, 21 };
	vecSelConnDlg.push_back(new UiArtText("Requirements:", rect3));

	SDL_Rect rect4 = { PANEL_LEFT + 35, 275, DESCRIPTION_WIDTH, 66 };
	vecSelConnDlg.push_back(new UiArtText(selconn_Description, rect4));

	SDL_Rect rect5 = { PANEL_LEFT + 30, 356, 220, 31 };
	vecSelConnDlg.push_back(new UiArtText("no gateway needed", rect5, UIS_CENTER | UIS_MED));

	SDL_Rect rect6 = { PANEL_LEFT + 35, 393, DESCRIPTION_WIDTH, 21 };
	vecSelConnDlg.push_back(new UiArtText(selconn_Gateway, rect6, UIS_CENTER));

	SDL_Rect rect7 = { PANEL_LEFT + 300, 211, 295, 33 };
	vecSelConnDlg.push_back(new UiArtText("Select Connection", rect7, UIS_CENTER | UIS_BIG));

	SDL_Rect rect8 = { PANEL_LEFT + 16, 427, 250, 35 };
	vecSelConnDlg.push_back(new UiArtTextButton("Change Gateway", NULL, rect8, UIS_CENTER | UIS_VCENTER | UIS_BIG | UIS_GOLD | UIS_HIDDEN));

	vecSelConnDlg.push_back(new UiList(vecConnItems, PANEL_LEFT + 305, 256, 285, 26, UIS_CENTER | UIS_VCENTER | UIS_GOLD));

	SDL_Rect rect9 = { PANEL_LEFT + 299, 427, 140, 35 };
	vecSelConnDlg.push_back(new UiArtTextButton("OK", &UiFocusNavigationSelect, rect9, UIS_CENTER | UIS_VCENTER | UIS_BIG | UIS_GOLD));

	SDL_Rect rect10 = { PANEL_LEFT + 454, 427, 140, 35 };
	vecSelConnDlg.push_back(new UiArtTextButton("Cancel", &UiFocusNavigationEsc, rect10, UIS_CENTER | UIS_VCENTER | UIS_BIG | UIS_GOLD));

	UiInitList(0, vecConnItems.size() - 1, selconn_Focus, selconn_Select, selconn_Esc, vecSelConnDlg);
}

void selconn_Free()
{
	ArtBackground.Unload();

	for (std::size_t i = 0; i < vecConnItems.size(); i++) {
		UiListItem *pUIItem = vecConnItems[i];
		delete pUIItem;
	}
	vecConnItems.clear();

	for (std::size_t i = 0; i < vecSelConnDlg.size(); i++) {
		UiItemBase *pUIMenuItem = vecSelConnDlg[i];
		if (pUIMenuItem)
			delete pUIMenuItem;
	}
	vecSelConnDlg.clear();
}

void selconn_Esc()
{
	selconn_ReturnValue = false;
	selconn_EndMenu = true;
}

void selconn_Focus(int value)
{
	int players = MAX_PLRS;
	switch (value) {
#ifndef NONET
	case SELCONN_TCP:
		strcpy(selconn_Description, "All computers must be connected to a TCP-compatible network.");
		players = MAX_PLRS;
		break;
#ifdef BUGGY
	case SELCONN_UDP:
		strcpy(selconn_Description, "All computers must be connected to a UDP-compatible network.");
		players = MAX_PLRS;
		break;
#endif
#endif
	case SELCONN_LOOPBACK:
		strcpy(selconn_Description, "Play by yourself with no network exposure.");
		players = 1;
		break;
	}

	sprintf(selconn_MaxPlayers, "Players Supported: %d", players);
	WordWrapArtStr(selconn_Description, DESCRIPTION_WIDTH);
}

void selconn_Select(int value)
{
	provider = value;

	selconn_Free();
	selconn_EndMenu = SNetInitializeProvider(provider, selconn_ClientInfo, selconn_UserInfo, selconn_UiInfo, selconn_FileInfo);
	selconn_Load();
}

int UiSelectProvider(
    int a1,
    _SNETPROGRAMDATA *client_info,
    _SNETPLAYERDATA *user_info,
    _SNETUIDATA *ui_info,
    _SNETVERSIONDATA *file_info,
    int *type)
{
	selconn_ClientInfo = client_info;
	selconn_UserInfo = user_info;
	selconn_UiInfo = ui_info;
	selconn_FileInfo = file_info;
	selconn_Load();

	selconn_ReturnValue = true;
	selconn_EndMenu = false;
	while (!selconn_EndMenu) {
		UiClearScreen();
		UiPollAndRender();
	}
	selconn_Free();

	return selconn_ReturnValue;
}

} // namespace dvl

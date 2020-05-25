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

std::vector<UiListItem*> vecConnItems;
vUiItemBase vecSelConnDlg;

#define DESCRIPTION_WIDTH 205

void selconn_Load()
{
	SDL_Rect rect;

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

	rect = { PANEL_LEFT + 24, 161, 590, 35 };
	vecSelConnDlg.push_back(new UiArtText("Multi Player Game", rect, UIS_CENTER | UIS_BIG));

	rect = { PANEL_LEFT + 35, 218, DESCRIPTION_WIDTH, 21 };
	vecSelConnDlg.push_back(new UiArtText(selconn_MaxPlayers, rect));

	rect = { PANEL_LEFT + 35, 256, DESCRIPTION_WIDTH, 21 };
	vecSelConnDlg.push_back(new UiArtText("Requirements:", rect));

	rect = { PANEL_LEFT + 35, 275, DESCRIPTION_WIDTH, 66 };
	vecSelConnDlg.push_back(new UiArtText(selconn_Description, rect));

	rect = { PANEL_LEFT + 30, 356, 220, 31 };
	vecSelConnDlg.push_back(new UiArtText("no gateway needed", rect, UIS_CENTER | UIS_MED));

	rect = { PANEL_LEFT + 35, 393, DESCRIPTION_WIDTH, 21 };
	vecSelConnDlg.push_back(new UiArtText(selconn_Gateway, rect, UIS_CENTER));

	rect = { PANEL_LEFT + 300, 211, 295, 33 };
	vecSelConnDlg.push_back(new UiArtText("Select Connection", rect, UIS_CENTER | UIS_BIG));

	rect = { PANEL_LEFT + 16, 427, 250, 35 };
	vecSelConnDlg.push_back(new UiArtTextButton("Change Gateway", NULL, rect, UIS_CENTER | UIS_VCENTER | UIS_BIG | UIS_GOLD | UIS_HIDDEN));

	vecSelConnDlg.push_back(new UiList(vecConnItems, PANEL_LEFT + 305, 256, 285, 26, UIS_CENTER | UIS_VCENTER | UIS_GOLD));

	rect = { PANEL_LEFT + 299, 427, 140, 35 };
	vecSelConnDlg.push_back(new UiArtTextButton("OK", &UiFocusNavigationSelect, rect, UIS_CENTER | UIS_VCENTER | UIS_BIG | UIS_GOLD));

	rect = { PANEL_LEFT + 454, 427, 140, 35 };
	vecSelConnDlg.push_back(new UiArtTextButton("Cancel", &UiFocusNavigationEsc, rect, UIS_CENTER | UIS_VCENTER | UIS_BIG | UIS_GOLD));

	UiInitList(0, vecConnItems.size() - 1, selconn_Focus, selconn_Select, selconn_Esc, vecSelConnDlg);
}

void selconn_Free()
{
	ArtBackground.Unload();

	for(std::size_t i = 0; i < vecConnItems.size(); i++)
	{
		UiListItem* pUIItem = vecConnItems[i];
		if(pUIItem)
			delete pUIItem;
	}
	vecConnItems.clear();

	for(std::size_t i = 0; i < vecSelConnDlg.size(); i++)
	{
		UiItemBase* pUIMenuItem = vecSelConnDlg[i];
		if(pUIMenuItem)
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
		strncpy(selconn_Description, "All computers must be connected to a TCP-compatible network.", sizeof(selconn_Description) - 1);
		players = MAX_PLRS;
		break;
#ifdef BUGGY
	case SELCONN_UDP:
		strncpy(selconn_Description, "All computers must be connected to a UDP-compatible network.", sizeof(selconn_Description) - 1);
		players = MAX_PLRS;
		break;
#endif
#endif
	case SELCONN_LOOPBACK:
		strncpy(selconn_Description, "Play by yourself with no network exposure.", sizeof(selconn_Description) - 1);
		players = 1;
		break;
	}

	snprintf(selconn_MaxPlayers, sizeof(selconn_MaxPlayers), "Players Supported: %d", players);
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

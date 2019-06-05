#include "selconn.h"

#include "devilution.h"
#include "DiabloUI/diabloui.h"

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

DWORD provider;

UI_Item SELCONNECT_DIALOG[] = {
	{ { 0, 0, 640, 480 }, UI_IMAGE, 0, 0, NULL, &ArtBackground },
	{ { 24, 161, 590, 35 }, UI_TEXT, UIS_CENTER | UIS_BIG, 0, "Multi Player Game" },
	{ { 35, 218, 205, 21 }, UI_TEXT, 0, 0, selconn_MaxPlayers }, // Max players
	{ { 35, 256, 205, 21 }, UI_TEXT, 0, 0, "Requirements:" },
	{ { 35, 275, 205, 66 }, UI_TEXT, 0, 0, selconn_Description }, //Description
	{ { 30, 356, 220, 31 }, UI_TEXT, UIS_CENTER | UIS_MED, 0, "no gateway needed" },
	{ { 35, 393, 205, 21 }, UI_TEXT, UIS_CENTER, 0, selconn_Gateway }, // Gateway
	{ { 16, 427, 250, 35 }, UI_BUTTON, UIS_CENTER | UIS_VCENTER | UIS_BIG | UIS_GOLD | UIS_HIDDEN, 0, "Change Gateway" },
	{ { 300, 211, 295, 33 }, UI_TEXT, UIS_CENTER | UIS_BIG, 0, "Select Connection" },
	{ { 305, 256, 285, 26 }, UI_LIST, UIS_CENTER | UIS_VCENTER | UIS_GOLD, 0, "Client-Server (TCP)" },
	{ { 305, 282, 285, 26 }, UI_LIST, UIS_CENTER | UIS_VCENTER | UIS_GOLD, 1, "Peer-to-Peer (UDP)" },
	{ { 305, 308, 285, 26 }, UI_LIST, UIS_CENTER | UIS_VCENTER | UIS_GOLD, 2, "Loopback" },
	{ { 305, 334, 285, 26 }, UI_LIST, UIS_CENTER | UIS_VCENTER | UIS_GOLD },
	{ { 305, 360, 285, 26 }, UI_LIST, UIS_CENTER | UIS_VCENTER | UIS_GOLD },
	{ { 305, 386, 285, 26 }, UI_LIST, UIS_CENTER | UIS_VCENTER | UIS_GOLD },
	{ { 299, 427, 140, 35 }, UI_BUTTON, UIS_CENTER | UIS_VCENTER | UIS_BIG | UIS_GOLD, 0, "OK", (void *)UiFocusNavigationSelect },
	{ { 454, 427, 140, 35 }, UI_BUTTON, UIS_CENTER | UIS_VCENTER | UIS_BIG | UIS_GOLD, 0, "Cancel", (void *)UiFocusNavigationEsc },
};

void selconn_Load()
{
	LoadBackgroundArt("ui_art\\selconn.pcx");
	UiInitList(0, 2, selconn_Focus, selconn_Select, selconn_Esc, SELCONNECT_DIALOG, size(SELCONNECT_DIALOG));
}

void selconn_Free()
{
	mem_free_dbg(ArtBackground.data);
	ArtBackground.data = NULL;
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
	case 0:
		sprintf(selconn_Description, "All computers must be connected to a TCP-compatible network.");
		players = MAX_PLRS;
		break;
	case 1:
		sprintf(selconn_Description, "All computers must be connected to a UDP-compatible network.");
		players = MAX_PLRS;
		break;
	case 2:
		sprintf(selconn_Description, "Play by yourself with no network exposure.");
		players = 1;
		break;
	}

	sprintf(selconn_MaxPlayers, "Players Supported: %d", players);

	for (auto &item : SELCONNECT_DIALOG) {
		if (item.caption != NULL && !(item.flags & (UIS_VCENTER | UIS_CENTER)))
			WordWrap(&item);
	}
}

void selconn_Select(int value)
{
	switch (value) {
	case 0:
		provider = 'TCPN';
		break;
	case 1:
		provider = 'UDPN';
		break;
	case 2:
		provider = 'SCBL';
		break;
	}

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
		UiRender();
	}
	BlackPalette();
	selconn_Free();

	return selconn_ReturnValue;
}

}

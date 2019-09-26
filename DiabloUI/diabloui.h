//HEADER_GOES_HERE
#ifndef __DIABLOUI_H__
#define __DIABLOUI_H__

#include "miniwin/pushdecl.inc"
namespace dvl {

//#if defined(__GNUC__) || defined(__cplusplus)
//extern "C" {
//#endif

struct FontStruct {
	unsigned char fontbin[258];
	HANDLE fonttrans[256];
	BOOL active;
};

struct ProfileStruct {
	char *name;
	char field_4;
	int msg;
	int field_C;
};

struct ProfFntStruct {
	int size;
	char *fontname;
	int field_8;
};

void __stdcall UiDestroy();
BOOL __stdcall UiTitleDialog(int a1);
void __stdcall UiSetSpawned(BOOL bSpawned);
void __stdcall UiInitialize();
BOOL __stdcall UiCopyProtError(int *pdwResult);
void __stdcall UiAppActivate(BOOL bActive);
BOOL __fastcall UiValidPlayerName(char *name); /* check __stdcall */
BOOL __stdcall UiSelHeroMultDialog(BOOL(__stdcall *fninfo)(BOOL(__stdcall *fninfofunc)(_uiheroinfo *)), BOOL(__stdcall *fncreate)(_uiheroinfo *), BOOL(__stdcall *fnremove)(_uiheroinfo *), BOOL(__stdcall *fnstats)(unsigned int, _uidefaultstats *), int *dlgresult, int *a6, char *name);
BOOL __stdcall UiSelHeroSingDialog(BOOL(__stdcall *fninfo)(BOOL(__stdcall *fninfofunc)(_uiheroinfo *)), BOOL(__stdcall *fncreate)(_uiheroinfo *), BOOL(__stdcall *fnremove)(_uiheroinfo *), BOOL(__stdcall *fnstats)(unsigned int, _uidefaultstats *), int *dlgresult, char *name, int *difficulty);
BOOL __stdcall UiCreditsDialog(int a1);
BOOL __stdcall UiMainMenuDialog(char *name, int *pdwResult, void(__stdcall *fnSound)(char *file), int attractTimeOut);
BOOL __stdcall UiProgressDialog(HWND window, char *msg, int enable, int(*fnfunc)(), int rate);
int __stdcall UiProfileGetString();
void __cdecl UiProfileCallback();
void __cdecl UiProfileDraw();
BOOL __stdcall UiCategoryCallback(int a1, int a2, int a3, int a4, int a5, DWORD *a6, DWORD *a7);
BOOL __stdcall UiGetDataCallback(int game_type, int data_code, void *a3, int a4, int a5);
BOOL __stdcall UiAuthCallback(int a1, char *a2, char *a3, char a4, char *a5, LPSTR lpBuffer, int cchBufferMax);
BOOL __stdcall UiSoundCallback(int a1, int type, int a3);
void __stdcall UiMessageBoxCallback(HWND hWnd, char *lpText, LPCSTR lpCaption, UINT uType);
BOOL __stdcall UiDrawDescCallback(int game_type, COLORREF color, LPCSTR lpString, char *a4, int a5, UINT align, time_t a7, HDC *a8);
BOOL __stdcall UiCreateGameCallback(int a1, int a2, int a3, int a4, int a5, int a6);
BOOL __stdcall UiArtCallback(int game_type, unsigned int art_code, PALETTEENTRY *pPalette, BYTE *pBuffer, DWORD dwBuffersize, DWORD *pdwWidth, DWORD *pdwHeight, DWORD *pdwBpp);
int __stdcall UiSelectGame(int a1, _SNETPROGRAMDATA *client_info, _SNETPLAYERDATA *user_info, _SNETUIDATA *ui_info, _SNETVERSIONDATA *file_info, int *a6);
int __stdcall UiSelectProvider(int a1, _SNETPROGRAMDATA *client_info, _SNETPLAYERDATA *user_info, _SNETUIDATA *ui_info, _SNETVERSIONDATA *file_info, int *type);
BOOL __stdcall UiCreatePlayerDescription(_uiheroinfo *info, DWORD mode, char *desc);
void __stdcall UiSetupPlayerInfo(char *infostr, _uiheroinfo *pInfo, DWORD type);
void __stdcall UiCreateGameCriteria(_uiheroinfo *pInfo, char *str);
BOOL __stdcall UiGetDefaultStats(int pclass, _uidefaultstats *pStats);
BOOL __stdcall UiBetaDisclaimer(int a1);

//#if defined(__GNUC__) || defined(__cplusplus)
//}
//#endif

}
#include "miniwin/popdecl.inc"

#endif /* __DIABLOUI_H__ */

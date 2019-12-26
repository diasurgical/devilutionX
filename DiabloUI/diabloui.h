//HEADER_GOES_HERE
#ifndef __DIABLOUI_H__
#define __DIABLOUI_H__

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

void UiDestroy();
void UiTitleDialog();
void UiSetSpawned(BOOL bSpawned);
void UiInitialize();
BOOL UiCopyProtError(int *pdwResult);
void UiAppActivate(BOOL bActive);
BOOL UiValidPlayerName(char *name); /* check */
BOOL UiSelHeroMultDialog(BOOL (*fninfo)(BOOL (*fninfofunc)(_uiheroinfo *)), BOOL (*fncreate)(_uiheroinfo *), BOOL (*fnremove)(_uiheroinfo *), BOOL (*fnstats)(unsigned int, _uidefaultstats *), int *dlgresult, BOOL *hero_is_created, char *name);
BOOL UiSelHeroSingDialog(BOOL (*fninfo)(BOOL (*fninfofunc)(_uiheroinfo *)), BOOL (*fncreate)(_uiheroinfo *), BOOL (*fnremove)(_uiheroinfo *), BOOL (*fnstats)(unsigned int, _uidefaultstats *), int *dlgresult, char *name, int *difficulty);
BOOL UiCreditsDialog(int a1);
BOOL UiMainMenuDialog(char *name, int *pdwResult, void (*fnSound)(char *file), int attractTimeOut);
BOOL UiProgressDialog(HWND window, char *msg, int enable, int (*fnfunc)(), int rate);
int UiProfileGetString();
void UiProfileCallback();
void UiProfileDraw();
BOOL UiCategoryCallback(int a1, int a2, int a3, int a4, int a5, DWORD *a6, DWORD *a7);
BOOL UiGetDataCallback(int game_type, int data_code, void *a3, int a4, int a5);
BOOL UiAuthCallback(int a1, char *a2, char *a3, char a4, char *a5, LPSTR lpBuffer, int cchBufferMax);
BOOL UiSoundCallback(int a1, int type, int a3);
void UiMessageBoxCallback(HWND hWnd, char *lpText, LPCSTR lpCaption, UINT uType);
BOOL UiDrawDescCallback(int game_type, COLORREF color, LPCSTR lpString, char *a4, int a5, UINT align, time_t a7, HDC *a8);
BOOL UiCreateGameCallback(int a1, int a2, int a3, int a4, int a5, int a6);
BOOL UiArtCallback(int game_type, unsigned int art_code, PALETTEENTRY *pPalette, BYTE *pBuffer, DWORD dwBuffersize, DWORD *pdwWidth, DWORD *pdwHeight, DWORD *pdwBpp);
int UiSelectGame(int a1, _SNETPROGRAMDATA *client_info, _SNETPLAYERDATA *user_info, _SNETUIDATA *ui_info, _SNETVERSIONDATA *file_info, int *a6);
int UiSelectProvider(int a1, _SNETPROGRAMDATA *client_info, _SNETPLAYERDATA *user_info, _SNETUIDATA *ui_info, _SNETVERSIONDATA *file_info, int *type);
BOOL UiCreatePlayerDescription(_uiheroinfo *info, DWORD mode, char *desc);
void UiSetupPlayerInfo(char *infostr, _uiheroinfo *pInfo, DWORD type);
void UiCreateGameCriteria(_uiheroinfo *pInfo, char *str);
BOOL UiGetDefaultStats(int pclass, _uidefaultstats *pStats);
BOOL UiBetaDisclaimer(int a1);

//#if defined(__GNUC__) || defined(__cplusplus)
//}
//#endif

}

#endif /* __DIABLOUI_H__ */

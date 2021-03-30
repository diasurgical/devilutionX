//HEADER_GOES_HERE
#ifndef __DIABLOUI_H__
#define __DIABLOUI_H__

namespace dvl {

void UiDestroy();
void UiTitleDialog();
void UiSetSpawned(BOOL bSpawned);
void UiInitialize();
BOOL UiValidPlayerName(const char *name); /* check */
void UiSelHeroMultDialog(BOOL (*fninfo)(BOOL (*fninfofunc)(_uiheroinfo *)), BOOL (*fncreate)(_uiheroinfo *), BOOL (*fnremove)(_uiheroinfo *), void (*fnstats)(unsigned int, _uidefaultstats *), int *dlgresult, BOOL *hero_is_created, char (*name)[16]);
void UiSelHeroSingDialog(BOOL (*fninfo)(BOOL (*fninfofunc)(_uiheroinfo *)), BOOL (*fncreate)(_uiheroinfo *), BOOL (*fnremove)(_uiheroinfo *), void (*fnstats)(unsigned int, _uidefaultstats *), int *dlgresult, char (*name)[16], int *difficulty);
BOOL UiCreditsDialog();
BOOL UiSupportDialog();
BOOL UiMainMenuDialog(const char *name, int *pdwResult, void (*fnSound)(const char *file), int attractTimeOut);
BOOL UiProgressDialog(const char *msg, int enable, int (*fnfunc)(), int rate);
const char **UiProfileGetString();
void UiProfileCallback();
void UiProfileDraw();
BOOL UiCategoryCallback(int a1, int a2, int a3, int a4, int a5, DWORD *a6, DWORD *a7);
BOOL UiGetDataCallback(int game_type, int data_code, void *a3, int a4, int a5);
BOOL UiAuthCallback(int a1, char *a2, char *a3, char a4, char *a5, char *lpBuffer, int cchBufferMax);
BOOL UiSoundCallback(int a1, int type, int a3);
BOOL UiDrawDescCallback(int game_type, DWORD color, const char *lpString, char *a4, int a5, UINT align, time_t a7, HDC *a8);
BOOL UiCreateGameCallback(int a1, int a2, int a3, int a4, int a5, int a6);
BOOL UiArtCallback(int game_type, unsigned int art_code, SDL_Color *pPalette, BYTE *pBuffer, DWORD dwBuffersize, DWORD *pdwWidth, DWORD *pdwHeight, DWORD *pdwBpp);
int UiSelectGame(int a1, _SNETPROGRAMDATA *client_info, _SNETPLAYERDATA *user_info, _SNETUIDATA *ui_info, _SNETVERSIONDATA *file_info, int *a6);
int UiSelectProvider(int a1, _SNETPROGRAMDATA *client_info, _SNETPLAYERDATA *user_info, _SNETUIDATA *ui_info, _SNETVERSIONDATA *file_info, int *type);
BOOL UiCreatePlayerDescription(_uiheroinfo *info, Uint32 mode, char (*desc)[128]);
void UiSetupPlayerInfo(char *infostr, _uiheroinfo *pInfo, Uint32 type);
} // namespace dvl

#endif /* __DIABLOUI_H__ */

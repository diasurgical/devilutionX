//HEADER_GOES_HERE
#ifndef __DIABLOUI_H__
#define __DIABLOUI_H__

#ifdef __GNUC__
extern "C" {
#endif

void __cdecl UiDestroy();
void __stdcall UiTitleDialog(int a1);
void __cdecl UiInitialize();
void __stdcall UiCopyProtError(int a1);
void __stdcall UiAppActivate(int a1);
int __stdcall UiValidPlayerName(char *a1);
int __stdcall UiSelHeroMultDialog(void *fninfo, void *fncreate, void *fnremove, void *fnstats, int *a5, int *a6, char *name);
int __stdcall UiSelHeroSingDialog(void (*fninfo)(void (__stdcall *ui_add_hero_info)(_uiheroinfo *)), void (*fncreate)(_uiheroinfo *heroinfo), void (*fnremove)(), void (*fnstats)(), int *a5, char *name, int *difficulty);
void __stdcall UiCreditsDialog(int a1);
int __stdcall UiMainMenuDialog(char *name, int *a2, void *fnSound, int a4);
int __stdcall UiProgressDialog(HWND window, char *msg, int a3, void *fnfunc, int a5);
int __cdecl UiProfileGetString();
void __cdecl UiProfileCallback();
void __cdecl UiProfileDraw();
void __cdecl UiCategoryCallback();
void __cdecl UiGetDataCallback();
void __cdecl UiAuthCallback();
void __cdecl UiSoundCallback();
void __cdecl UiMessageBoxCallback();
void __cdecl UiDrawDescCallback();
void __cdecl UiCreateGameCallback();
void __cdecl UiArtCallback();
int __stdcall UiSelectGame(int a1, _SNETPROGRAMDATA *client_info, _SNETPLAYERDATA *user_info, _SNETUIDATA *ui_info, _SNETVERSIONDATA *file_info, int *a6);
int __stdcall UiSelectProvider(int a1, _SNETPROGRAMDATA *client_info, _SNETPLAYERDATA *user_info, _SNETUIDATA *ui_info, _SNETVERSIONDATA *file_info, int *type);
int __stdcall UiCreatePlayerDescription(_uiheroinfo *info, int mode, char *desc);
int __stdcall UiSetupPlayerInfo(char *str, _uiheroinfo *info, int mode);

#ifdef __GNUC__
}
#endif

#endif /* __DIABLOUI_H__ */

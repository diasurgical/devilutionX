#include <vector>

#include "../types.h"
#include "stubs.h"

void __cdecl UiDestroy()
{
	DUMMY();
}

BOOL __stdcall UiTitleDialog(int a1)
{
	DUMMY();
	return TRUE;
}

void __cdecl UiInitialize()
{
	DUMMY();
}

static std::vector<_uiheroinfo> hero_infos;

static BOOL __stdcall ui_add_hero_info(_uiheroinfo *info)
{
	hero_infos.emplace_back(*info);
	return TRUE;
}

BOOL __stdcall UiSelHeroSingDialog(BOOL(__stdcall *fninfo)(BOOL(__stdcall *fninfofunc)(_uiheroinfo *)),
                                   BOOL(__stdcall *fncreate)(_uiheroinfo *), BOOL(__stdcall *fnremove)(_uiheroinfo *),
                                   BOOL(__stdcall *fnstats)(int, _uidefaultstats *), int *dlgresult, char *name,
                                   int *difficulty)
{
	DUMMY();

	hero_infos.clear();
	fninfo(&ui_add_hero_info);

	// If a hero is available, load it, otherwise create a new one
	if (!hero_infos.empty()) {
		const char *hero_name = hero_infos[0].name;
		DUMMY_PRINT("use hero: %s", hero_name);
		strcpy(name, hero_name);

		*dlgresult = 2;
	} else {
		const char *test_name = "tester";
		DUMMY_PRINT("create hero: %s", test_name);

		strcpy(name, test_name);

		_uiheroinfo hero_info = {0};
		strcpy(hero_info.name, test_name);
		hero_info.heroclass = PC_SORCERER;

		fncreate(&hero_info);
	}

	return TRUE;
}

BOOL __stdcall UiMainMenuDialog(char *name, int *pdwResult, void(__stdcall *fnSound)(char *file), int a4)
{
	DUMMY();
	// Pretend we selected Single Player from the main menu
	*pdwResult = MAINMENU_SINGLE_PLAYER;
	return TRUE;
}

int __cdecl UiProfileGetString()
{
	DUMMY();
	return 0;
}

void __stdcall UiSetupPlayerInfo(char *infostr, _uiheroinfo *pInfo, int type)
{
	DUMMY_PRINT("chr: %s", infostr);
}

BOOL __stdcall UiCopyProtError(int *pdwResult)
{
	UNIMPLEMENTED();
}

void __stdcall UiAppActivate(BOOL bActive)
{
	UNIMPLEMENTED();
}

BOOL __fastcall UiValidPlayerName(char *name)
{
	UNIMPLEMENTED();
}

int __stdcall UiProgressDialog(HWND window, char *msg, int a3, void *fnfunc, int a5)
{
	UNIMPLEMENTED();
}

BOOL __stdcall UiSelHeroMultDialog(BOOL(__stdcall *fninfo)(BOOL(__stdcall *fninfofunc)(_uiheroinfo *)),
                                   BOOL(__stdcall *fncreate)(_uiheroinfo *), BOOL(__stdcall *fnremove)(_uiheroinfo *),
                                   BOOL(__stdcall *fnstats)(int, _uidefaultstats *), int *dlgresult, int *a6,
                                   char *name)
{
	UNIMPLEMENTED();
}

BOOL __stdcall UiCreditsDialog(int a1)
{
	UNIMPLEMENTED();
}

void __cdecl UiProfileCallback()
{
	UNIMPLEMENTED();
}

void __cdecl UiProfileDraw()
{
	UNIMPLEMENTED();
}

BOOL __stdcall UiCategoryCallback(int a1, int a2, int a3, int a4, int a5, _DWORD *a6, _DWORD *a7)
{
	UNIMPLEMENTED();
}

BOOL __stdcall UiGetDataCallback(int game_type, int data_code, void *a3, int a4, int a5)
{
	UNIMPLEMENTED();
}

BOOL __stdcall UiAuthCallback(int a1, char *a2, char *a3, char a4, char *a5, LPSTR lpBuffer, int cchBufferMax)
{
	UNIMPLEMENTED();
}

BOOL __stdcall UiSoundCallback(int a1, int type, int a3)
{
	UNIMPLEMENTED();
}

void __stdcall UiMessageBoxCallback(HWND hWnd, char *lpText, LPCSTR lpCaption, UINT uType)
{
	UNIMPLEMENTED();
}

BOOL __stdcall UiDrawDescCallback(int arg0, COLORREF color, LPCSTR lpString, char *a4, int a5, UINT align, time_t a7,
                                  HDC *a8)
{
	UNIMPLEMENTED();
}

BOOL __stdcall UiCreateGameCallback(int a1, int a2, int a3, int a4, int a5, int a6)
{
	UNIMPLEMENTED();
}

BOOL __stdcall UiArtCallback(int game_type, unsigned int art_code, PALETTEENTRY *pPalette, void *pBuffer,
                             DWORD dwBuffersize, DWORD *pdwWidth, DWORD *pdwHeight, DWORD *pdwBpp)
{
	UNIMPLEMENTED();
}

int __stdcall UiSelectGame(int a1, _SNETPROGRAMDATA *client_info, _SNETPLAYERDATA *user_info, _SNETUIDATA *ui_info,
                           _SNETVERSIONDATA *file_info, int *a6)
{
	UNIMPLEMENTED();
}

int __stdcall UiSelectProvider(int a1, _SNETPROGRAMDATA *client_info, _SNETPLAYERDATA *user_info, _SNETUIDATA *ui_info,
                               _SNETVERSIONDATA *file_info, int *type)
{
	UNIMPLEMENTED();
}

int __stdcall UiCreatePlayerDescription(_uiheroinfo *info, int mode, char *desc)
{
	UNIMPLEMENTED();
}

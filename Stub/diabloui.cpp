#include <vector>

#include "../types.h"
#include "stubs.h"

void __cdecl UiDestroy()
{
	DUMMY();
}

void __stdcall UiTitleDialog(int a1)
{
	printf("UiTitleDialog\n");
	DUMMY();
}

void __cdecl UiInitialize()
{
	DUMMY();
}

void __stdcall UiCopyProtError(int a1)
{
	UNIMPLEMENTED();
}

void __stdcall UiAppActivate(int a1)
{
	UNIMPLEMENTED();
}

int __stdcall UiValidPlayerName(char *a1)
{
	UNIMPLEMENTED();
}

int __stdcall UiSelHeroMultDialog(void *fninfo, void *fncreate, void *fnremove, void *fnstats, int *a5, int *a6,
                                  char *name)
{
	UNIMPLEMENTED();
}

std::vector<_uiheroinfo> hero_infos;

static void __stdcall ui_add_hero_info(_uiheroinfo *info)
{
	hero_infos.emplace_back(*info);
}

int __stdcall UiSelHeroSingDialog(void (*fninfo)(void(__stdcall *ui_add_hero_info)(_uiheroinfo *)),
                                  void (*fncreate)(_uiheroinfo *heroinfo), void (*fnremove)(), void (*fnstats)(),
                                  int *a5, char *name, int *difficulty)
{
	DUMMY();

	fninfo(&ui_add_hero_info);

	if (!hero_infos.empty()) {
		//const char *hero_name = hero_infos[0].name;
		DUMMY_PRINT("use hero: %s", chr_name_str);
		strcpy(name, chr_name_str);

		*a5 = 2;
	} else {
		const char *test_name = "tester";
		DUMMY_PRINT("create hero: %s", test_name);

		strcpy(name, test_name);

		_uiheroinfo hero_info = {0};
		strcpy(hero_info.name, test_name);
		hero_info.heroclass = 2; // Sorcerer

		fncreate(&hero_info);
	}

	return TRUE;
}

void __stdcall UiCreditsDialog(int a1)
{
	UNIMPLEMENTED();
}

int __stdcall UiMainMenuDialog(char *name, int *a2, void *fnSound, int a4)
{
	DUMMY();
	*a2 = MAINMENU_SINGLE_PLAYER;
	return TRUE;
}

int __stdcall UiProgressDialog(HWND window, char *msg, int a3, void *fnfunc, int a5)
{
	UNIMPLEMENTED();
}

int __cdecl UiProfileGetString()
{
	DUMMY();
	return 0;
}

void __cdecl UiProfileCallback()
{
	UNIMPLEMENTED();
}

void __cdecl UiProfileDraw()
{
	UNIMPLEMENTED();
}

void __cdecl UiCategoryCallback()
{
	UNIMPLEMENTED();
}

void __cdecl UiGetDataCallback()
{
	UNIMPLEMENTED();
}

void __cdecl UiAuthCallback()
{
	UNIMPLEMENTED();
}

void __cdecl UiSoundCallback()
{
	UNIMPLEMENTED();
}

void __cdecl UiMessageBoxCallback()
{
	UNIMPLEMENTED();
}

void __cdecl UiDrawDescCallback()
{
	UNIMPLEMENTED();
}

void __cdecl UiCreateGameCallback()
{
	UNIMPLEMENTED();
}

void __cdecl UiArtCallback()
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

int __stdcall UiSetupPlayerInfo(char *str, _uiheroinfo *info, int mode)
{
	printf("UiSetupPlayerInfo\n");
	DUMMY_PRINT("chr: %s", str);
	return TRUE;
}

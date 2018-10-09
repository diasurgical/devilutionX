#include <windows.h>
#include <stdio.h>
#include <ddraw.h>
#include <dsound.h>
#include <io.h>
#include <math.h>
#include <time.h>
#include <process.h>
#include <shlobj.h>

#ifdef _MSC_VER
#pragma warning (disable : 4018) // signed/unsigned mismatch
#endif

#include "..\defs.h"
#include "..\enums.h"
#include "..\structs.h"
#include "..\3rdParty\Storm\Source\storm.h"
#include "diabloui.h"

#define UNKCALL     __fastcall
#define USERCALL    __fastcall
#define USERPURGE   __fastcall

//temporarily include everything directly
#include "_temp_funcs.h"
#include "_temp_data.cpp"

#include "artfont.cpp"
#include "bnetgw.cpp"
#include "bn_prof.cpp"
#include "connect.cpp"
#include "copyprot.cpp"
#include "cr8game.cpp"
#include "creadung.cpp"
#include "creastat.cpp"
#include "credits.cpp"
#include "diabedit.cpp"
#include "dirlink.cpp"
#include "disclaim.cpp"
#include "doom.cpp"
#include "entdial.cpp"
#include "entname.cpp"
#include "fade.cpp"
#include "focus.cpp"
#include "local.cpp"
#include "mainmenu.cpp"
#include "modem.cpp"
#include "modmstat.cpp"
#include "okcancel.cpp"
#include "progress.cpp"
#include "sbar.cpp"
#include "selclass.cpp"
#include "selconn.cpp"
#include "seldial.cpp"
#include "selgame.cpp"
#include "selhero.cpp"
#include "selipx.cpp"
#include "sellist.cpp"
#include "selload.cpp"
#include "selmodem.cpp"
#include "selregn.cpp"
#include "selyesno.cpp"
#include "title.cpp"
#include "titlesnd.cpp"


// ref: 0x10005C2A
int __cdecl DiabloUI_GetSpawned()
{
	return sgbIsSpawn;
}
// 1002972C: using guessed type int sgbIsSpawn;

// ref: 0x10005C30
void __stdcall UiOnPaint(int a1)
{
	return;
}

// ref: 0x10005C33
void __stdcall UiSetBackgroundBitmap(int a1, PALETTEENTRY *a2, int a3, int a4, int a5)
{
	backbmp_flag1 = a3;
	backbmp_flag2 = a4;
	backbmp_flag3 = a5;
	SDrawUpdatePalette(0xAu, 0xECu, a2 + 10, 0);
}
// 1002971C: using guessed type int backbmp_flag1;
// 10029720: using guessed type int backbmp_flag2;
// 10029724: using guessed type int backbmp_flag3;

// ref: 0x10005C67
void __stdcall UiSetSpawned(BOOL bSpawned)
{
	sgbIsSpawn = bSpawned;
}
// 1002972C: using guessed type int sgbIsSpawn;

// ref: 0x10005C73
void __cdecl UiInitialize()
{
	sgbUiIsInitialized = 1;
	TitleSnd_InitSoundFunc();
	artfont_InitAllFonts();
	Connect_LoadGFXAndStuff();
	local_LoadArtCursor();
	bn_prof_100021C4();
}
// 10029714: using guessed type int sgbUiIsInitialized;

// ref: 0x10005C96
void __cdecl UiDestroy()
{
	bn_prof_10002247();
	local_FreeArtCursor();
	Connect_FreeConnectData();
	sgbUiIsInitialized = 0;
}
// 10029714: using guessed type int sgbUiIsInitialized;

// ref: 0x10005CAD
void __stdcall UiAppActivate(BOOL bActive)
{
	app_is_active = bActive;
}
// 10029728: using guessed type int app_is_active;

// ref: 0x10005CB9
BOOL __stdcall DllMain(HINSTANCE hinstDLL, DWORD fdwReason, LPVOID lpvReserved)
{
	if ( fdwReason )
	{
		if ( fdwReason == 1 )
		{
			ghUiInst = hinstDLL;
			DiabEdit_SetupWindow();
			local_InitUiPalette();
		}
	}
	else
	{
		local_DelUiPalette();
	}
	return 1;
}

// ref: 0x10005CEA
void __cdecl DiabloUI_cpp_init()
{
	DiabloUI_cpp_float = DiabloUI_cpp_float_value;
}
// 1001F410: using guessed type int DiabloUI_cpp_float_value;
// 10029710: using guessed type int DiabloUI_cpp_float;
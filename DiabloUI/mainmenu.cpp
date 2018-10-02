// ref: 0x10008164
void __cdecl MainMenu_cpp_init()
{
	mainmenu_cpp_float = mainmenu_cpp_float_value;
}
// 1001F434: using guessed type int mainmenu_cpp_float_value;
// 1002A0D4: using guessed type int mainmenu_cpp_float;

// ref: 0x1000816F
BOOL __stdcall UiMainMenuDialog(char *name, int *pdwResult, void (__stdcall *fnSound)(char *file), int a4)
{
	int v4; // eax
	int v5; // esi

	menu_item_timer = a4;
	TitleSnd_SetSoundFunction(fnSound);
	artfont_LoadAllFonts();
	menu_version_str[0] = 0;
	if ( name )
		strncpy(menu_version_str, name, 0x40u);
	v4 = (int)SDrawGetFrameWindow();
	v5 = SDlgDialogBoxParam(ghUiInst, "MAINMENU_DIALOG", v4, MainMenu_WndProc, 0);
	if ( v5 == 5 )
		artfont_FreeAllFonts();
	if ( pdwResult )
		*pdwResult = v5;
	return 1;
}
// 10010382: using guessed type _DWORD __stdcall SDrawGetFrameWindow();
// 1002A118: using guessed type int menu_item_timer;

// ref: 0x100081E3
LRESULT __stdcall MainMenu_WndProc(HWND hWnd, UINT Msg, WPARAM wParam, LPARAM lParam)
{
	HWND v5; // eax

	if ( Msg <= 0x113 )
	{
		if ( Msg != 275 )
		{
			if ( Msg == 2 )
			{
				MainMenu_KillAndFreeMenu(hWnd);
			}
			else if ( Msg > 0x103 )
			{
				if ( Msg <= 0x105 )
				{
					v5 = (HWND)SDrawGetFrameWindow();
					SendMessageA(v5, Msg, wParam, lParam);
				}
				else
				{
					if ( Msg == 272 )
					{
						MainMenu_LoadMenuGFX(hWnd);
						PostMessageA(hWnd, 0x7E8u, 0, 0);
						return 1;
					}
					if ( Msg == 273 )
					{
						MainMenu_SetMenuTimer(hWnd);
						switch ( HIWORD(wParam) )
						{
							case 7:
								Focus_GetAndBlitSpin(hWnd, lParam);
								break;
							case 6:
								Focus_CheckPlayMove(lParam);
								Focus_DoBlitSpinIncFrame(hWnd, (HWND)lParam);
								break;
							case 0:
								MainMenu_CheckWParamFocus(hWnd, (unsigned short)wParam);
								break;
						}
					}
				}
			}
			return (LRESULT)SDlgDefDialogProc(hWnd, Msg, (HDC)wParam, (HWND)lParam);
		}
		if ( wParam == 3 && !DiabloUI_GetSpawned() )
		{
			if ( app_is_active )
				MainMenu_DoOptions(hWnd, 6, 0);
			else
				MainMenu_SetMenuTimer(hWnd);
		}
		return 0;
	}
	if ( Msg >= 0x200 )
	{
		if ( Msg <= 0x202 )
			goto LABEL_34;
		if ( Msg <= 0x203 )
			return (LRESULT)SDlgDefDialogProc(hWnd, Msg, (HDC)wParam, (HWND)lParam);
		if ( Msg <= 0x205 )
		{
LABEL_34:
			MainMenu_SetMenuTimer(hWnd);
			return (LRESULT)SDlgDefDialogProc(hWnd, Msg, (HDC)wParam, (HWND)lParam);
		}
		if ( Msg == 2024 )
		{
			if ( !Fade_CheckRange5() )
				Fade_SetFadeTimer((int)hWnd);
			return 0;
		}
	}
	return (LRESULT)SDlgDefDialogProc(hWnd, Msg, (HDC)wParam, (HWND)lParam);
}
// 10010382: using guessed type _DWORD __stdcall SDrawGetFrameWindow();
// 10029728: using guessed type int app_is_active;

// ref: 0x10008354
void __fastcall MainMenu_KillAndFreeMenu(HWND hWnd)
{
	void **v2; // eax

	Title_KillTitleTimer(hWnd);
	Focus_KillFocusTimer(hWnd);
	Doom_DeleteFreeProcs(hWnd, menumsgs_5options);
	Doom_DeleteFreeProcs(hWnd, menumsgs_1option);
	v2 = (void **)GetWindowLongA(hWnd, -21);
	local_FreeMemPtr(v2);
	local_SetCursorDefault();
}

// ref: 0x10008391
void __fastcall MainMenu_SetMenuTimer(HWND hWnd)
{
	SDlgSetTimer((int)hWnd, 3, 1000 * menu_item_timer, 0);
}
// 1002A118: using guessed type int menu_item_timer;

// ref: 0x100083A8
void __fastcall MainMenu_LoadMenuGFX(HWND hWnd)
{
	DWORD *v2; // eax MAPDST
	bool v4; // zf
	const char *v5; // eax
	HWND v6; // eax

	v2 = local_AllocWndLongData();
	if ( v2 )
	{
		SetWindowLongA(hWnd, -21, (LONG)v2);
		v4 = DiabloUI_GetSpawned() == 0;
		v5 = "ui_art\\swmmenu.pcx";
		if ( v4 )
			v5 = "ui_art\\mainmenu.pcx";
		local_LoadArtWithPal(hWnd, 0, &nullcharacter, -1, 1, v5, (BYTE **)v2, v2 + 1, 0);
		Fade_NoInputAndArt(hWnd, 1);
	}
	v6 = GetDlgItem(hWnd, 1042);
	SetWindowTextA(v6, menu_version_str);
	Doom_ParseWndProc3(hWnd, menumsgs_1option, 1);
	Doom_ParseWndProcs(hWnd, menumsgs_5options, 6, 1);
	Focus_SetFocusTimer(hWnd, "ui_art\\focus42.pcx");
	Title_LoadImgSetTimer(hWnd, "ui_art\\smlogo.pcx");
	MainMenu_SetMenuTimer(hWnd);
	local_DoUiWndProc2(hWnd, (DWORD *)menumsgs_5options);
}

// ref: 0x1000845A
void __fastcall MainMenu_DoOptions(HWND hWnd, int option, int PlaySelect)
{
	SDlgKillTimer((int)hWnd, 3);
	if ( DiabloUI_GetSpawned() && option == 3 )
	{
		SelYesNo_SpawnErrDialog(hWnd, 70, 1);
LABEL_7:
		SDlgSetTimer((int)hWnd, 3, 1000 * menu_item_timer, 0);
		return;
	}
	if ( option == 2 && !MainMenu_CheckEnoughMemory() )
	{
		SelYesNo_SpawnErrDialog(hWnd, 78, 1);
		goto LABEL_7;
	}
	Fade_Range5SetZero();
	if ( PlaySelect )
		TitleSnd_PlaySelectSound();
	Fade_UpdatePaletteRange(10);
	SDlgEndDialog(hWnd, (HANDLE)option);
}
// 1002A118: using guessed type int menu_item_timer;

// ref: 0x100084D5
BOOL __cdecl MainMenu_CheckEnoughMemory()
{
	struct _MEMORYSTATUS Buffer; // [esp+0h] [ebp-20h]

	Buffer.dwLength = 32;
	GlobalMemoryStatus(&Buffer);
	return Buffer.dwTotalPhys > 0xDAC000;
}

// ref: 0x100084FA
void __fastcall MainMenu_CheckWParamFocus(HWND hWnd, WPARAM wParam)
{
	HWND v3; // eax
	LONG v4; // eax
	int v5; // [esp-8h] [ebp-Ch]

	switch ( wParam )
	{
		case 1u:
			v3 = GetFocus();
			v4 = GetWindowLongA(v3, -12);
			SendMessageA(hWnd, 0x111u, v4, 0);
			return;
		case 2u:
			v5 = 5;
			goto LABEL_12;
		case 0x3E9u:
			v5 = 2;
			goto LABEL_12;
		case 0x3EAu:
			v5 = 3;
			goto LABEL_12;
		case 0x3EBu:
			v5 = 4;
LABEL_12:
			MainMenu_DoOptions(hWnd, v5, 1);
			return;
		case 0x414u:
			MainMenu_DoOptions(hWnd, 1, 1);
			break;
	}
}

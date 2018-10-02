// ref: 0x1000FA49
int __fastcall SelYesNo_YesNoDialog(HWND hWnd, char *dialogstr, char *hero, int nofocus)
{
	yesno_dialog_string = dialogstr;
	yesno_hero_name = hero;
	yesno_remove_focus = nofocus;
	yesno_is_popup = 0;
	YesNoFunc = 0;
	return SDlgDialogBoxParam(ghUiInst, "SELYESNO_DIALOG", (int)hWnd, SelYesNo_WndProc, 0);
}
// 1002A500: using guessed type int yesno_remove_focus;
// 1002A508: using guessed type int (*YesNoFunc)(void);
// 1002A50C: using guessed type int yesno_is_popup;

// ref: 0x1000FA87
LRESULT __stdcall SelYesNo_WndProc(HWND hWnd, UINT Msg, WPARAM wParam, LPARAM lParam)
{
	HWND v4; // eax
	HWND v5; // eax
	int v7; // edx
	HWND v8; // eax
	LONG v9; // eax
	HWND v10; // ecx
	HWND v11; // eax

	if ( Msg == 2 )
	{
		SelYesNo_RemoveYNDialog(hWnd);
		return (LRESULT)SDlgDefDialogProc(hWnd, Msg, (HDC)wParam, (HWND)lParam);
	}
	if ( Msg > 0x103 )
	{
		if ( Msg <= 0x105 )
		{
			v11 = (HWND)SDrawGetFrameWindow();
			SendMessageA(v11, Msg, wParam, lParam);
			return (LRESULT)SDlgDefDialogProc(hWnd, Msg, (HDC)wParam, (HWND)lParam);
		}
		switch ( Msg )
		{
			case 0x110u:
				SelYesNo_LoadSelYN_GFX(hWnd);
				return 0;
			case 0x111u:
				if ( HIWORD(wParam) == 7 )
				{
					Focus_GetAndBlitSpin(hWnd, lParam);
				}
				else if ( HIWORD(wParam) == 6 )
				{
					Focus_CheckPlayMove(lParam);
					Focus_DoBlitSpinIncFrame(hWnd, (HWND)lParam);
				}
				else
				{
					v7 = 1;
					if ( (_WORD)wParam == 1 )
					{
						v8 = GetFocus();
						v9 = GetWindowLongA(v8, -12);
						v10 = hWnd;
						if ( v9 == 1109 )
							v7 = 1;
						else
							v7 = 2;
					}
					else
					{
						if ( (_WORD)wParam == 2 )
						{
							v7 = 2;
						}
						else if ( (_WORD)wParam != 1109 )
						{
							return (LRESULT)SDlgDefDialogProc(hWnd, Msg, (HDC)wParam, (HWND)lParam);
						}
						v10 = hWnd;
					}
					SelYesNo_DoSelectYesNo(v10, v7);
				}
				break;
			case 0x113u:
				v4 = GetFocus();
				if ( !Focus_DoBlitSpinIncFrame(hWnd, v4) )
				{
					v5 = GetDlgItem(hWnd, 1109);
					if ( !v5 )
						v5 = GetDlgItem(hWnd, 2);
					SetFocus(v5);
				}
				return 0;
		}
	}
	return (LRESULT)SDlgDefDialogProc(hWnd, Msg, (HDC)wParam, (HWND)lParam);
}
// 10010382: using guessed type _DWORD __stdcall SDrawGetFrameWindow();

// ref: 0x1000FBC7
void __fastcall SelYesNo_RemoveYNDialog(HWND hWnd)
{
	HWND v1; // esi
	void **v2; // eax
	HWND v3; // eax

	v1 = hWnd;
	v2 = (void **)GetWindowLongA(hWnd, -21);
	local_FreeMemPtr(v2);
	if ( yesno_remove_focus )
		Focus_DeleteSpinners();
	Doom_DeleteFreeProcs(v1, yesno_msgtbl1);
	Doom_DeleteFreeProcs(v1, yesno_msgtbl2);
	if ( yesno_hero_name )
	{
		v3 = GetParent(v1);
		SelHero_SetStringWithMsg(v3, 0);
	}
}
// 1002A500: using guessed type int yesno_remove_focus;

// ref: 0x1000FC1C
void __fastcall SelYesNo_LoadSelYN_GFX(HWND hWnd)
{
	HWND v2; // eax
	DWORD *v3; // eax
	DWORD *v4; // edi
	const char *v5; // eax
	char *v6; // ST18_4
	HWND v7; // eax

	v2 = GetParent(hWnd);
	if ( yesno_hero_name )
		SelHero_SetStringWithMsg(v2, yesno_hero_name);
	v3 = local_AllocWndLongData();
	v4 = v3;
	if ( v3 )
	{
		SetWindowLongA(hWnd, -21, (LONG)v3);
		if ( yesno_is_popup )
		{
			if ( DiabloUI_GetSpawned() )
				v5 = "ui_art\\swmmpop.pcx";
			else
				v5 = "ui_art\\mmpopup.pcx";
		}
		else
		{
			v5 = "ui_art\\black.pcx";
		}
		local_LoadArtWithPal(hWnd, 0, "Popup", -1, 1, v5, (BYTE **)v4, v4 + 1, 1);
	}
	v6 = yesno_dialog_string;
	v7 = GetDlgItem(hWnd, 1026);
	SetWindowTextA(v7, v6);
	Doom_ParseWndProc3(hWnd, yesno_msgtbl2, 3);
	Doom_ParseWndProcs(hWnd, yesno_msgtbl1, 4, 1);
	if ( yesno_remove_focus )
		Focus_LoadSpinner("ui_art\\focus.pcx");
	else
		Focus_ResetSpinToZero();
	SDlgSetTimer((int)hWnd, 1, 55, 0);
	local_DoUiWndProc2(hWnd, (DWORD *)yesno_msgtbl1);
}
// 1002A500: using guessed type int yesno_remove_focus;
// 1002A50C: using guessed type int yesno_is_popup;

// ref: 0x1000FCF6
void __fastcall SelYesNo_DoSelectYesNo(HWND hWnd, int option)
{
	HWND v4; // eax

	TitleSnd_PlaySelectSound();
	SDlgKillTimer((int)hWnd, 1);
	if ( option == 2 )
	{
		if ( !YesNoFunc )
			goto LABEL_6;
		YesNoFunc();
	}
	if ( option == 1 )
	{
		v4 = GetFocus();
		option = GetWindowLongA(v4, -12);
	}
LABEL_6:
	SDlgEndDialog(hWnd, (HANDLE)option);
}
// 1002A508: using guessed type int (*YesNoFunc)(void);

// ref: 0x1000FD39
int __fastcall SelYesNo_SelOkDialog(HWND hWnd, char *dialogstr, char *hero, int nofocus)
{
	yesno_dialog_string = dialogstr;
	yesno_hero_name = hero;
	yesno_remove_focus = nofocus;
	yesno_is_popup = 0;
	YesNoFunc = 0;
	return SDlgDialogBoxParam(ghUiInst, "SELOK_DIALOG", (int)hWnd, SelYesNo_WndProc, 0);
}
// 1002A500: using guessed type int yesno_remove_focus;
// 1002A508: using guessed type int (*YesNoFunc)(void);
// 1002A50C: using guessed type int yesno_is_popup;

// ref: 0x1000FD77
int __fastcall SelYesNo_SpawnErrDialog(HWND hWnd, int string_rsrc, int is_popup)
{
	char Buffer[256]; // [esp+4h] [ebp-100h]

	LoadStringA(ghUiInst, string_rsrc, Buffer, 255);
	yesno_is_popup = is_popup;
	yesno_remove_focus = 0;
	yesno_hero_name = 0;
	yesno_dialog_string = Buffer;
	YesNoFunc = 0;
	return SDlgDialogBoxParam(ghUiInst, "SPAWNERR_DIALOG", (int)hWnd, SelYesNo_WndProc, 0);
}
// 1002A500: using guessed type int yesno_remove_focus;
// 1002A508: using guessed type int (*YesNoFunc)(void);
// 1002A50C: using guessed type int yesno_is_popup;

// ref: 0x1000FDE3
void __cdecl SelYesNo_cpp_init()
{
	SelYesNo_cpp_float = SelYesNo_cpp_float_value;
}
// 1001F478: using guessed type int SelYesNo_cpp_float_value;
// 1002A4FC: using guessed type int SelYesNo_cpp_float;

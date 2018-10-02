// ref: 0x10006F7C
LRESULT __stdcall EntName_WndProc(HWND hWnd, UINT Msg, WPARAM wParam, LPARAM lParam)
{
	int v4; // edx
	HWND v5; // eax
	HWND v6; // eax
	HWND v7; // eax
	HWND v9; // eax

	v4 = 2;
	if ( Msg == 2 )
	{
		EntName_DelEntNameMsgs(hWnd);
		return (LRESULT)SDlgDefDialogProc(hWnd, Msg, (HDC)wParam, (HWND)lParam);
	}
	if ( Msg <= 0x103 )
		return (LRESULT)SDlgDefDialogProc(hWnd, Msg, (HDC)wParam, (HWND)lParam);
	if ( Msg <= 0x105 )
	{
		v9 = (HWND)SDrawGetFrameWindow();
		SendMessageA(v9, Msg, wParam, lParam);
		return (LRESULT)SDlgDefDialogProc(hWnd, Msg, (HDC)wParam, (HWND)lParam);
	}
	switch ( Msg )
	{
		case 0x110u:
			entname_charname = (char *)lParam;
			EntName_LoadFocusChkName(hWnd);
			return 0;
		case 0x111u:
			if ( (unsigned short)wParam != 1 )
			{
				if ( (unsigned short)wParam != 2 )
				{
					if ( (unsigned short)wParam == 1065 )
						EntName_GetMessageName(hWnd, wParam, lParam);
					return (LRESULT)SDlgDefDialogProc(hWnd, Msg, (HDC)wParam, (HWND)lParam);
				}
				goto LABEL_11;
			}
			goto LABEL_18;
		case 0x113u:
			v7 = GetFocus();
			Focus_DoBlitSpinIncFrame(hWnd, v7);
			return 0;
	}
	if ( Msg != 513 )
		return (LRESULT)SDlgDefDialogProc(hWnd, Msg, (HDC)wParam, (HWND)lParam);
	v5 = GetDlgItem(hWnd, 1056);
	if ( local_GetBottomRect(hWnd, v5, (unsigned short)lParam, (unsigned int)lParam >> 16) )
	{
LABEL_18:
		v4 = 1;
		goto LABEL_11;
	}
	v6 = GetDlgItem(hWnd, 1054);
	if ( local_GetBottomRect(hWnd, v6, (unsigned short)lParam, (unsigned int)lParam >> 16) )
	{
		v4 = 2;
LABEL_11:
		EntName_SetCharName(hWnd, v4);
	}
	return (LRESULT)SDlgDefDialogProc(hWnd, Msg, (HDC)wParam, (HWND)lParam);
}
// 10010382: using guessed type _DWORD __stdcall SDrawGetFrameWindow();

// ref: 0x1000709E
void __fastcall EntName_DelEntNameMsgs(HWND hWnd)
{
	HWND v2; // eax

	Focus_DeleteSpinners();
	Doom_DeleteFreeProcs(hWnd, entname_msgtbl3);
	Doom_DeleteFreeProcs(hWnd, entname_msgtbl2);
	Doom_DeleteFreeProcs(hWnd, entname_msgtbl1);
	v2 = GetParent(hWnd);
	SelHero_SetStringWithMsg(v2, 0);
}

// ref: 0x100070DB
void __fastcall EntName_LoadFocusChkName(HWND hWnd)
{
	HWND v2; // edi
	LONG v3; // eax
	HWND v4; // ebx
	char Buffer[32]; // [esp+Ch] [ebp-20h]

	v2 = GetParent(hWnd);
	if ( SelHero_GetHeroIsGood() == 1 )
		LoadStringA(ghUiInst, 0x20u, Buffer, 31);
	else
		LoadStringA(ghUiInst, 0x1Fu, Buffer, 31);
	SelHero_SetStringWithMsg(v2, Buffer);
	v3 = GetWindowLongA(v2, -21);
	SetWindowLongA(hWnd, -21, v3);
	Doom_ParseWndProc3(hWnd, entname_msgtbl1, 5);
	Doom_ParseWndProcs(hWnd, entname_msgtbl2, 4, 0);
	Doom_ParseWndProc5(hWnd, entname_msgtbl3, 2);
	Focus_LoadSpinner("ui_art\\focus.pcx");
	SDlgSetTimer((int)hWnd, 1, 55, 0);
	v4 = GetDlgItem(hWnd, 1065);
	SendMessageA(v4, 0x401u, 0xFu, 0);
	if ( SelHero_GetHeroIsGood() == 1 )
		SendMessageA(v4, 0x404u, 0, (LPARAM)" ,<>%&\\\"?*#/");
}

// ref: 0x100071AC
void __fastcall EntName_SetCharName(HWND hWnd, int a2)
{
	char *v4; // ST08_4
	HWND v5; // eax

	TitleSnd_PlaySelectSound();
	SDlgKillTimer((int)hWnd, 1);
	v4 = entname_charname;
	v5 = GetDlgItem(hWnd, 1065);
	GetWindowTextA(v5, v4, 16);
	entname_charname[15] = 0;
	SDlgEndDialog(hWnd, (HANDLE)a2);
}

// ref: 0x100071ED
void __fastcall EntName_GetMessageName(HWND hWnd, unsigned int a2, int a3)
{
	unsigned int v3; // edx
	HWND v5; // eax

	v3 = (a2 >> 16) - 1;
	if ( v3 )
	{
		if ( v3 == 2 )
			Doom_ParseWndProc6(hWnd, entname_msgtbl3, 2);
	}
	else
	{
		v5 = GetFocus();
		Focus_DoBlitSpinIncFrame(hWnd, v5);
	}
}

// ref: 0x10007220
void __cdecl EntName_cpp_init()
{
	EntName_cpp_float = EntName_cpp_float_value;
}
// 1001F424: using guessed type int EntName_cpp_float_value;
// 10029860: using guessed type int EntName_cpp_float;

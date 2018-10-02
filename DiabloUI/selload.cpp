// ref: 0x1000E1C2
LRESULT __stdcall SelLoad_WndProc(HWND hWnd, UINT Msg, WPARAM wParam, LPARAM lParam)
{
	HWND v4; // eax
	int v5; // edx
	HWND v6; // eax
	HWND v7; // eax
	HWND v9; // eax

	if ( Msg == 2 )
	{
		SelLoad_DeleteProcsAndSpin(hWnd);
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
	if ( Msg == 272 )
	{
		SelLoad_LoadFocusAndMsg(hWnd);
		return 0;
	}
	if ( Msg != 273 )
	{
		if ( Msg != 275 )
		{
			if ( Msg == 513 )
			{
				v4 = GetDlgItem(hWnd, 1056);
				if ( local_GetBottomRect(hWnd, v4, (unsigned short)lParam, (unsigned int)lParam >> 16) )
				{
					v5 = 1;
LABEL_19:
					SelLoad_SelectSndLoad(hWnd, v5);
					return (LRESULT)SDlgDefDialogProc(hWnd, Msg, (HDC)wParam, (HWND)lParam);
				}
				v6 = GetDlgItem(hWnd, 1054);
				if ( local_GetBottomRect(hWnd, v6, (unsigned short)lParam, (unsigned int)lParam >> 16) )
				{
LABEL_21:
					v5 = 2;
					goto LABEL_19;
				}
			}
			return (LRESULT)SDlgDefDialogProc(hWnd, Msg, (HDC)wParam, (HWND)lParam);
		}
		v7 = GetFocus();
		Focus_DoBlitSpinIncFrame(hWnd, v7);
		return 0;
	}
	if ( HIWORD(wParam) == 7 )
	{
		Focus_GetAndBlitSpin(hWnd, lParam);
	}
	else
	{
		if ( HIWORD(wParam) != 6 )
		{
			v5 = 1;
			if ( HIWORD(wParam) == 5 || (_WORD)wParam == 1 )
				goto LABEL_19;
			if ( (_WORD)wParam != 2 )
				return (LRESULT)SDlgDefDialogProc(hWnd, Msg, (HDC)wParam, (HWND)lParam);
			goto LABEL_21;
		}
		Focus_CheckPlayMove(lParam);
		Focus_DoBlitSpinIncFrame(hWnd, (HWND)lParam);
	}
	return (LRESULT)SDlgDefDialogProc(hWnd, Msg, (HDC)wParam, (HWND)lParam);
}
// 10010382: using guessed type _DWORD __stdcall SDrawGetFrameWindow();

// ref: 0x1000E30E
void __fastcall SelLoad_DeleteProcsAndSpin(HWND hWnd)
{
	HWND v2; // eax

	Focus_DeleteSpinners();
	Doom_DeleteFreeProcs(hWnd, selload_msgtbl3);
	Doom_DeleteFreeProcs(hWnd, selload_msgtbl2);
	Doom_DeleteFreeProcs(hWnd, selload_msgtbl1);
	v2 = GetParent(hWnd);
	SelHero_SetStringWithMsg(v2, 0);
}

// ref: 0x1000E34B
void __fastcall SelLoad_LoadFocusAndMsg(HWND hWnd)
{
	HWND v2; // edi
	LONG v3; // eax
	char Buffer[32]; // [esp+8h] [ebp-20h]

	v2 = GetParent(hWnd);
	LoadStringA(ghUiInst, 0x1Du, Buffer, 31);
	SelHero_SetStringWithMsg(v2, Buffer);
	v3 = GetWindowLongA(v2, -21);
	SetWindowLongA(hWnd, -21, v3);
	local_DoUiWndProc(hWnd, (DWORD *)selload_msgtbl3);
	Doom_ParseWndProc3(hWnd, selload_msgtbl1, 5);
	Doom_ParseWndProcs(hWnd, selload_msgtbl2, 4, 0);
	Doom_ParseWndProcs(hWnd, selload_msgtbl3, 2, 1);
	Focus_LoadSpinner("ui_art\\focus16.pcx");
	SDlgSetTimer((int)hWnd, 1, 55, 0);
}

// ref: 0x1000E3E2
void __fastcall SelLoad_SelectSndLoad(HWND hWnd, int a2)
{
	int v2; // esi
	HWND v4; // eax

	v2 = a2;
	TitleSnd_PlaySelectSound();
	SDlgKillTimer((int)hWnd, 1);
	if ( v2 == 1 )
	{
		v4 = GetFocus();
		v2 = GetWindowLongA(v4, -12);
	}
	SDlgEndDialog(hWnd, (HANDLE)v2);
}

// ref: 0x1000E41A
void __cdecl SelLoad_cpp_init()
{
	SelLoad_cpp_float = SelLoad_cpp_float_value;
}
// 1001F46C: using guessed type int SelLoad_cpp_float_value;
// 1002A4C8: using guessed type int SelLoad_cpp_float;

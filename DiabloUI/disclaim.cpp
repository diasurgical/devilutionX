// ref: 0x100063B3
BOOL __stdcall UiBetaDisclaimer(int a1)
{
	int v1; // eax

	v1 = (int)SDrawGetFrameWindow();
	SDlgDialogBoxParam(ghUiInst, "DISCLAIMER_DIALOG", v1, disclaim_WndProc, a1);
	return 1;
}
// 10010382: using guessed type _DWORD __stdcall SDrawGetFrameWindow();

// ref: 0x100063DA
LRESULT __stdcall disclaim_WndProc(HWND hWnd, UINT Msg, WPARAM wParam, LPARAM lParam)
{
	HWND v5; // eax

	if ( Msg > 0x111 )
	{
		if ( Msg != 513 && Msg != 516 )
		{
			if ( Msg == 528 )
			{
				if ( (_WORD)wParam == 513 || (_WORD)wParam == 516 )
					disclaim_FadeFromDisclaim(hWnd);
			}
			else if ( Msg == 2024 )
			{
				if ( !Fade_CheckRange5() )
					Fade_SetFadeTimer((int)hWnd);
				return 0;
			}
			return (LRESULT)SDlgDefDialogProc(hWnd, Msg, (HDC)wParam, (HWND)lParam);
		}
	}
	else if ( Msg != 273 )
	{
		if ( Msg != 2 )
		{
			if ( Msg != 256 )
			{
				if ( Msg > 0x103 )
				{
					if ( Msg <= 0x105 )
					{
						v5 = (HWND)SDrawGetFrameWindow();
						SendMessageA(v5, Msg, wParam, lParam);
					}
					else if ( Msg == 272 )
					{
						disclaim_LoadDisclaimGFX(hWnd);
						PostMessageA(hWnd, 0x7E8u, 0, 0);
						return 1;
					}
				}
				return (LRESULT)SDlgDefDialogProc(hWnd, Msg, (HDC)wParam, (HWND)lParam);
			}
			goto LABEL_21;
		}
		disclaim_DelDisclaimProcs(hWnd);
		return (LRESULT)SDlgDefDialogProc(hWnd, Msg, (HDC)wParam, (HWND)lParam);
	}
LABEL_21:
	disclaim_FadeFromDisclaim(hWnd);
	return 0;
}
// 10010382: using guessed type _DWORD __stdcall SDrawGetFrameWindow();

// ref: 0x100064C9
void __fastcall disclaim_DelDisclaimProcs(HWND hWnd)
{
	void **v2; // eax

	Doom_DeleteFreeProcs(hWnd, disclaim_msgtbl2);
	Doom_DeleteFreeProcs(hWnd, disclaim_msgtbl1);
	v2 = (void **)GetWindowLongA(hWnd, -21);
	local_FreeMemPtr(v2);
}

// ref: 0x100064F3
void __fastcall disclaim_LoadDisclaimGFX(HWND hWnd)
{
	DWORD *v2; // eax MAPDST

	v2 = local_AllocWndLongData();
	if ( v2 )
	{
		SetWindowLongA(hWnd, -21, (LONG)v2);
		local_LoadArtWithPal(hWnd, 0, &nullcharacter, -1, 1, "ui_art\\disclaim.pcx", (BYTE **)v2, v2 + 1, 0);
		Fade_NoInputAndArt(hWnd, 0);
	}
	Doom_ParseWndProc3(hWnd, disclaim_msgtbl1, 5);
	Doom_ParseWndProc3(hWnd, disclaim_msgtbl2, 2);
}

// ref: 0x10006552
void __fastcall disclaim_FadeFromDisclaim(HWND hWnd)
{
	Fade_Range5SetZero();
	Fade_UpdatePaletteRange(10);
	SDlgEndDialog(hWnd, (void *)HANDLE_FLAG_INHERIT);
}

// ref: 0x10006571
void __cdecl disclaim_cpp_init()
{
	disclaim_cpp_float = disclaim_cpp_float_value;
}
// 1001F418: using guessed type int disclaim_cpp_float_value;
// 10029850: using guessed type int disclaim_cpp_float;

// ref: 0x1000FDEE
void __fastcall Title_BlitTitleBuffer(HWND hWnd)
{
	_DWORD *v2; // edi
	int v3; // eax
	HANDLE v4; // esi
	struct tagRECT Rect; // [esp+Ch] [ebp-18h]
	HWND hWnda; // [esp+20h] [ebp-4h]

	v2 = (_DWORD *)GetWindowLongA(hWnd, -21);
	hWnda = GetDlgItem(hWnd, 1043);
	if ( IsWindowVisible(hWnd) && hWnda && v2 && *v2 && titlePHTrans[0] )
	{
		v3 = titleTransIdx + 1;
		titleTransIdx = v3;
		if ( !titlePHTrans[v3] || v3 >= 30 )
			titleTransIdx = 0;
		GetWindowRect(hWnda, &Rect);
		ScreenToClient(hWnd, (LPPOINT)&Rect);
		ScreenToClient(hWnd, (LPPOINT)&Rect.right);
		v4 = GetPropA(hWnd, "TITLE_BUFFER");
		if ( v4 )
		{
			SBltROP3(
				*(void **)v4,
				(void *)(Rect.left + *v2 + Rect.top * v2[1]),
				*((_DWORD *)v4 + 1),
				*((_DWORD *)v4 + 2),
				*((_DWORD *)v4 + 1),
				v2[1],
				0,
				0xCC0020u);
			STransBlt(*(void **)v4, 0, 0, *((_DWORD *)v4 + 1), (HANDLE)titlePHTrans[titleTransIdx]);
			InvalidateRect(hWnda, 0, 0);
		}
	}
}
// 1002A58C: using guessed type int titleTransIdx;

// ref: 0x1000FEED
void __cdecl Title_DeletePhTrans()
{
	int *v0; // esi

	v0 = (int *)titlePHTrans;
	do
	{
		if ( *v0 )
		{
			STransDelete((HANDLE)*v0);
			*v0 = 0;
		}
		++v0;
	}
	while ( (signed int)v0 < (signed int)&titlePHTrans[30] );
}

// ref: 0x1000FF0F
void __fastcall Title_FreeTransMem(HWND hWnd)
{
	void **v2; // eax MAPDST
	void *v4; // eax

	Title_DeletePhTrans();
	v2 = (void **)RemovePropA(hWnd, "TITLE_BUFFER");
	if ( v2 )
	{
		v4 = *v2;
		if ( v4 )
		{
			SMemFree(v4, "C:\\Src\\Diablo\\DiabloUI\\Title.cpp", 114, 0);
			*v2 = 0;
		}
		SMemFree(v2, "C:\\Src\\Diablo\\DiabloUI\\Title.cpp", 117, 0);
	}
}

// ref: 0x1000FF51
void __fastcall Title_SetTitleBMP(HWND hWnd)
{
	HWND v1; // eax MAPDST
	_DWORD *v2; // esi
	void *v3; // eax
	struct tagRECT Rect; // [esp+0h] [ebp-18h]

	Title_FreeTransMem(hWnd);
	v1 = GetDlgItem(hWnd, 1043);
	if ( v1 )
	{
		GetClientRect(v1, &Rect);
		v2 = (unsigned int *)SMemAlloc(0xCu, "C:\\Src\\Diablo\\DiabloUI\\Title.cpp", 134, 0);
		v3 = SMemAlloc(Rect.right * Rect.bottom, "C:\\Src\\Diablo\\DiabloUI\\Title.cpp", 136, 8);
		*v2 = (unsigned int)v3;
		v2[1] = Rect.right;
		v2[2] = Rect.bottom;
		SDlgSetBitmapI(v1, 0, 0, -1, 1, v3, 0, Rect.right, Rect.bottom, -1);
		SetPropA(hWnd, "TITLE_BUFFER", v2);
	}
}

// ref: 0x1000FFE8
void __fastcall Title_LoadTitleImage(HWND hWnd, const char *pszFileName)
{
	int v3; // edi
	_DWORD *v4; // eax
	_DWORD *v5; // esi
	int v6; // ebx
	int a5[4]; // [esp+8h] [ebp-20h]
	int data[2]; // [esp+18h] [ebp-10h]
	HANDLE *phTransOut; // [esp+20h] [ebp-8h]
	BYTE *pBuffer; // [esp+24h] [ebp-4h]

	v3 = 0;
	pBuffer = 0;
	local_LoadArtImage(pszFileName, &pBuffer, (DWORD *)data);
	v4 = (unsigned int *)GetPropA(hWnd, "TITLE_BUFFER");
	v5 = v4;
	if ( pBuffer )
	{
		if ( v4 )
		{
			v6 = data[1] / v4[2];
			if ( v6 > 30 )
				v6 = 30;
			if ( v6 > 0 )
			{
				phTransOut = (HANDLE *)titlePHTrans;
				do
				{
					a5[0] = 0;
					a5[2] = v5[1] - 1;
					a5[1] = v3 * v5[2];
					a5[3] = v5[2] + a5[1] - 1;
					STransCreateI(pBuffer, v5[1], v5[2], 8, (int)a5, 16777466, phTransOut);
					++phTransOut;
					++v3;
				}
				while ( v3 < v6 );
			}
		}
		SMemFree(pBuffer, "C:\\Src\\Diablo\\DiabloUI\\Title.cpp", 197, 0);
	}
	titleTransIdx = 0;
}
// 1002A58C: using guessed type int titleTransIdx;

// ref: 0x1001009E
void __fastcall Title_LoadImgSetTimer(HWND hWnd, const char *pszFileName)
{
	Title_SetTitleBMP(hWnd);
	Title_LoadTitleImage(hWnd, pszFileName);
	Title_BlitTitleBuffer(hWnd);
	SDlgSetTimer((int)hWnd, 2, 55, Title_BlitTitleBufFnc);
}

// ref: 0x100100CB
void __stdcall Title_BlitTitleBufFnc(int hWnd, int a2, int a3, int a4)
{
	Title_BlitTitleBuffer((HWND)hWnd);
}

// ref: 0x100100DC
void __cdecl Title_cpp_init()
{
	Title_cpp_float = Title_cpp_float_value;
}
// 1001F47C: using guessed type int Title_cpp_float_value;
// 1002A588: using guessed type int Title_cpp_float;

// ref: 0x100100E7
void __fastcall Title_KillTitleTimer(HWND hWnd)
{
	HWND v1; // esi

	v1 = hWnd;
	SDlgKillTimer((int)hWnd, 2);
	Title_FreeTransMem(v1);
}

// ref: 0x100100FA
BOOL __stdcall UiTitleDialog(int a1)
{
	int v1; // eax

	artfont_LoadAllFonts();
	v1 = (int)SDrawGetFrameWindow();
	SDlgDialogBoxParam(ghUiInst, "TITLESCREEN_DIALOG", v1, Title_MainProc, a1);
	return 1;
}
// 10010382: using guessed type _DWORD __stdcall SDrawGetFrameWindow();

// ref: 0x10010126
LRESULT __stdcall Title_MainProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	HWND v5; // eax

	if ( uMsg <= 0x111 )
	{
		if ( uMsg != 273 )
		{
			if ( uMsg != 2 )
			{
				if ( uMsg == 135 )
					return 4;
				if ( uMsg != 256 )
				{
					if ( uMsg > 0x103 )
					{
						if ( uMsg <= 0x105 )
						{
							v5 = (HWND)SDrawGetFrameWindow();
							SendMessageA(v5, uMsg, wParam, lParam);
						}
						else if ( uMsg == 272 )
						{
							Title_LoadAllTitleImgs(hWnd, lParam);
							PostMessageA(hWnd, 0x7E8u, 0, 0);
							return 1;
						}
					}
					return (LRESULT)SDlgDefDialogProc(hWnd, uMsg, (HDC)wParam, (HWND)lParam);
				}
				goto LABEL_25;
			}
			Title_KillTimerAndFree(hWnd);
			return (LRESULT)SDlgDefDialogProc(hWnd, uMsg, (HDC)wParam, (HWND)lParam);
		}
		goto LABEL_25;
	}
	if ( uMsg != 275 )
	{
		if ( uMsg != 513 && uMsg != 516 )
		{
			if ( uMsg == 528 )
			{
				if ( (_WORD)wParam == 513 || (_WORD)wParam == 516 )
					Title_KillAndFadeDlg(hWnd);
			}
			else if ( uMsg == 2024 )
			{
				if ( !Fade_CheckRange5() )
					Fade_SetFadeTimer((int)hWnd);
				return 0;
			}
			return (LRESULT)SDlgDefDialogProc(hWnd, uMsg, (HDC)wParam, (HWND)lParam);
		}
LABEL_25:
		Title_KillAndFadeDlg(hWnd);
		return 0;
	}
	if ( wParam == 1 )
		goto LABEL_25;
	return 0;
}
// 1001037C: using guessed type int __stdcall SDlgDefDialogProc(_DWORD, _DWORD, _DWORD, _DWORD);
// 10010382: using guessed type _DWORD __stdcall SDrawGetFrameWindow();

// ref: 0x10010235
void __fastcall Title_KillTimerAndFree(HWND hWnd)
{
	void **v2; // eax

	Title_KillTitleTimer(hWnd);
	Doom_DeleteFreeProcs(hWnd, titlemsgtbl);
	v2 = (void **)GetWindowLongA(hWnd, -21);
	local_FreeMemPtr(v2);
}

// ref: 0x1001025A
void __fastcall Title_LoadAllTitleImgs(HWND hWnd, int time)
{
	DWORD *v4; // edi

	v4 = local_AllocWndLongData();
	SetWindowLongA(hWnd, -21, (LONG)v4);
	if ( v4 )
	{
		local_LoadArtWithPal(hWnd, 0, &nullcharacter, -1, 1, "ui_art\\title.pcx", (BYTE **)v4, v4 + 1, 0);
		Fade_NoInputAndArt(hWnd, 0);
	}
	Doom_ParseWndProc3(hWnd, titlemsgtbl, 3);
	Title_LoadImgSetTimer(hWnd, "ui_art\\logo.pcx");
	if ( time )
		SDlgSetTimer((int)hWnd, 1, 1000 * time, 0);
	else
		SDlgSetTimer((int)hWnd, 1, 5000, 0);
}

// ref: 0x100102D7
void __fastcall Title_KillAndFadeDlg(HWND hWnd)
{
	Fade_Range5SetZero();
	SDlgKillTimer((int)hWnd, 1);
	Fade_UpdatePaletteRange(10);
	SDlgEndDialog(hWnd, (void *)HANDLE_FLAG_INHERIT);
}

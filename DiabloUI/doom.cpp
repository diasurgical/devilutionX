// ref: 0x10006581
void __cdecl Doom_cpp_init()
{
	doom_cpp_float = doom_cpp_float_value;
}
// 1001F41C: using guessed type int doom_cpp_float_value;
// 10029854: using guessed type int doom_cpp_float;

// ref: 0x1000658C
void __fastcall Doom_ParseWndProcs(HWND hWnd, int *msgtbl, int a3, int a4)
{
	HWND v6; // eax

	while ( *msgtbl )
	{
		v6 = GetDlgItem(hWnd, *msgtbl);
		Doom_GetSetWndText(hWnd, (int)v6, a3, a4);
		++msgtbl;
	}
}

// ref: 0x100065BB
void __fastcall Doom_GetSetWndText(HWND hWnd, int msg, int nFont, int a4)
{
	HWND v4; // esi
	HWND v5; // edi
	int v6; // eax
	char String[256]; // [esp+8h] [ebp-100h]

	v4 = (HWND)msg;
	v5 = hWnd;
	if ( msg )
	{
		Doom_AllocAndSetBMP(hWnd, msg, 1521);
		Doom_GetWindowROP3(v5, v4);
		artfont_SetArtFont(nFont);
		Doom_PrintStrWithSpin(v4, a4);
		GetWindowTextA(v4, String, 255);
		if ( strlen(String) )
		{
			v6 = GetWindowLongA(v4, -21);
			local_SetWndLongStr(v6, String);
			SetWindowTextA(v4, &nullcharacter);
		}
	}
}

// ref: 0x1000663F
void __fastcall Doom_PrintStrWithSpin(HWND hWnd, BOOL a2)
{
	_DWORD *v3; // eax
	_DWORD *v4; // esi
	char *v5; // ebx
	int v6; // edi
	size_t v7; // eax
	char *i; // eax
	int v9; // kr04_4
	int v10; // eax
	char String[256]; // [esp+8h] [ebp-10Ch]
	char *v12; // [esp+108h] [ebp-Ch]
	int v14; // [esp+110h] [ebp-4h]

	v3 = (_DWORD *)GetWindowLongA(hWnd, -21);
	v4 = v3;
	if ( v3 && *v3 )
	{
		GetWindowTextA(hWnd, String, 255);
		v5 = String;
		if ( !strlen(String) )
			v5 = (char *)(v4 + 4);
		v14 = artfont_GetFontWidth(v5);
		if ( a2 )
		{
			v6 = v4[1] - 2 * Focus_GetSpinWidthOrZero();
			v7 = strlen(v5);
			if ( v14 > v6 )
			{
				for ( i = &v5[v7]; ; i = v12 )
				{
					v12 = i - 1;
					*v12 = 0;
					v14 = artfont_GetFontWidth(v5);
					if ( v14 <= v6 )
						break;
				}
			}
		}
		v9 = v4[1] - v14 - 1;
		v10 = artfont_GetFontMaxHeight();
		artfont_PrintFontStr(v5, (DWORD **)v4, v9 / 2, (v4[2] - v10) / 2);
		InvalidateRect(hWnd, 0, 0);
	}
}

// ref: 0x10006719
void __fastcall Doom_AllocAndSetBMP(HWND hWnd, int a2, int bmp_flags)
{
	DWORD *v4; // esi
	struct tagRECT Rect; // [esp+8h] [ebp-10h]

	GetClientRect((HWND)a2, &Rect);
	v4 = local_AllocWndLongData();
	v4[1] = Rect.right;
	v4[2] = Rect.bottom;
	*v4 = (DWORD)SMemAlloc(Rect.right * Rect.bottom, "C:\\Src\\Diablo\\DiabloUI\\Doom.cpp", 139, 0);
	SetWindowLongA((HWND)a2, -21, (LONG)v4);
	SDlgSetBitmapI((HWND)a2, 0, &nullcharacter, -1, bmp_flags, (void *)*v4, 0, v4[1], v4[2], -1);
}

// ref: 0x1000678A
void __fastcall Doom_GetWindowROP3(HWND hWnd1, HWND hWnd2)
{
	_DWORD *v3; // ebx
	LONG v4; // eax MAPDST
	struct tagRECT Rect; // [esp+Ch] [ebp-14h]

	v3 = (_DWORD *)GetWindowLongA(hWnd1, -21);
	v4 = GetWindowLongA(hWnd2, -21);
	if ( v3 && *v3 && v4 )
	{
		if ( *(_DWORD *)v4 )
		{
			GetWindowRect(hWnd2, &Rect);
			ScreenToClient(hWnd1, (LPPOINT)&Rect);
			ScreenToClient(hWnd1, (LPPOINT)&Rect.right);
			SBltROP3(
				*(void **)v4,
				(void *)(Rect.left + *v3 + Rect.top * v3[1]),
				*(_DWORD *)(v4 + 4),
				*(_DWORD *)(v4 + 8),
				*(_DWORD *)(v4 + 4),
				v3[1],
				0,
				0xCC0020u);
		}
	}
}

// ref: 0x1000680A
void __fastcall Doom_ParseWndProc2(HWND hWnd, int *msgtbl, int a3, int a4)
{
	HWND v6; // eax

	while ( *msgtbl )
	{
		v6 = GetDlgItem(hWnd, *msgtbl);
		Doom_GetSetWndTxt2(hWnd, (int)v6, a3, a4);
		++msgtbl;
	}
}

// ref: 0x10006839
void __fastcall Doom_GetSetWndTxt2(HWND hWnd, int msg, int nFont, int a4)
{
	HWND v4; // esi
	int v5; // eax
	char String[256]; // [esp+4h] [ebp-100h]

	v4 = (HWND)msg;
	if ( msg )
	{
		Doom_GetWindowROP3(hWnd, (HWND)msg);
		artfont_SetArtFont(nFont);
		Doom_PrintStrWithSpin(v4, a4);
		GetWindowTextA(v4, String, 255);
		if ( strlen(String) )
		{
			v5 = GetWindowLongA(v4, -21);
			local_SetWndLongStr(v5, String);
			SetWindowTextA(v4, &nullcharacter);
		}
	}
}

// ref: 0x100068AB
void __fastcall Doom_ParseWndProc3(HWND hWnd, int *msgtbl, int a3)
{
	HWND v5; // eax

	while ( *msgtbl )
	{
		v5 = GetDlgItem(hWnd, *msgtbl);
		Doom_GetSetWndTxt3(hWnd, (int)v5, a3);
		++msgtbl;
	}
}

// ref: 0x100068D6
void __fastcall Doom_GetSetWndTxt3(HWND hWnd, int msg, int nFont)
{
	HWND v3; // esi
	HWND v4; // edi
	int v5; // eax
	int v6; // eax
	char String[256]; // [esp+8h] [ebp-100h]

	v3 = (HWND)msg;
	v4 = hWnd;
	if ( msg )
	{
		Doom_AllocAndSetBMP(hWnd, msg, 1);
		Doom_GetWindowROP3(v4, v3);
		artfont_SetArtFont(nFont);
		v5 = GetWindowLongA(v3, -16);
		Doom_PrintStrWithSpn2(v3, v5);
		GetWindowTextA(v3, String, 255);
		if ( strlen(String) )
		{
			v6 = GetWindowLongA(v3, -21);
			local_SetWndLongStr(v6, String);
			SetWindowTextA(v3, &nullcharacter);
		}
	}
}

// ref: 0x1000695D
void __fastcall Doom_PrintStrWithSpn2(HWND hWnd, int justify_type)
{
	_DWORD *v2; // eax
	_DWORD *v3; // esi
	char *v4; // edi
	int v5; // eax
	char String[256]; // [esp+4h] [ebp-108h]

	v2 = (_DWORD *)GetWindowLongA(hWnd, -21);
	v3 = v2;
	if ( v2 && *v2 )
	{
		GetWindowTextA(hWnd, String, 255);
		v4 = String;
		if ( !strlen(String) )
			v4 = (char *)(v3 + 4);
		if ( justify_type & 2 )
		{
			v5 = v3[1] - artfont_GetFontWidth(v4) - 1;
		}
		else if ( justify_type & 1 )
		{
			v5 = (v3[1] - artfont_GetFontWidth(v4) - 1) / 2;
		}
		else
		{
			v5 = 0;
		}
		artfont_PrintFontStr(v4, (DWORD **)v3, v5, 0);
		InvalidateRect(hWnd, 0, 0);
	}
}

// ref: 0x10006A13
void __fastcall Doom_ParseWndProc4(HWND hWnd, int *msgtbl, int a3)
{
	HWND v5; // eax

	while ( *msgtbl )
	{
		v5 = GetDlgItem(hWnd, *msgtbl);
		Doom_GetSetWndTxt4(hWnd, (int)v5, a3);
		++msgtbl;
	}
}

// ref: 0x10006A3E
void __fastcall Doom_GetSetWndTxt4(HWND hWnd, int msg, int nFont)
{
	HWND v3; // edi
	int v4; // eax
	int v5; // eax
	char String[256]; // [esp+8h] [ebp-100h]

	v3 = (HWND)msg;
	if ( msg )
	{
		Doom_GetWindowROP3(hWnd, (HWND)msg);
		artfont_SetArtFont(nFont);
		v4 = GetWindowLongA(v3, -16);
		Doom_PrintStrWithSpn2(v3, v4);
		GetWindowTextA(v3, String, 255);
		if ( strlen(String) )
		{
			v5 = GetWindowLongA(v3, -21);
			local_SetWndLongStr(v5, String);
			SetWindowTextA(v3, &nullcharacter);
		}
	}
}

// ref: 0x10006AB8
void __fastcall Doom_ParseWndProc5(HWND hWnd, int *msgtbl, int a3)
{
	HWND v5; // eax

	while ( *msgtbl )
	{
		v5 = GetDlgItem(hWnd, *msgtbl);
		Doom_GetSetWndTxt5(hWnd, (int)v5, a3);
		++msgtbl;
	}
}

// ref: 0x10006AE3
void __fastcall Doom_GetSetWndTxt5(HWND hWnd, int msg, int nFont)
{
	HWND v3; // esi

	v3 = (HWND)msg;
	if ( msg )
	{
		Doom_AllocAndSetBMP(hWnd, msg, 1);
		Doom_GetWindowROP3(hWnd, v3);
		artfont_SetArtFont(nFont);
		Doom_PrintTextMsg403(v3);
	}
}

// ref: 0x10006B12
void __fastcall Doom_PrintTextMsg403(HWND hWnd)
{
	BYTE *v2; // eax
	int v3; // edi
	int v4; // edi
	int v5; // esi
	int v6; // esi
	char *i; // ebx
	int v8; // eax
	//char v9; // [esp+3h] [ebp-11Dh]
	char String[256]; // [esp+4h] [ebp-11Ch]
	struct tagRECT Rect; // [esp+104h] [ebp-1Ch]
	LRESULT v12; // [esp+114h] [ebp-Ch]
	BYTE *pWidthBin; // [esp+118h] [ebp-8h]
	size_t v14; // [esp+11Ch] [ebp-4h]

	v2 = (BYTE *)GetWindowLongA(hWnd, -21);
	pWidthBin = v2;
	if ( v2 && *(_DWORD *)v2 )
	{
		GetWindowTextA(hWnd, String, 255);
		v14 = strlen(String);
		v3 = Focus_GetSpinWidthOrZero();
		v4 = artfont_GetFontDefWidth() + v3;
		GetClientRect(hWnd, &Rect);
		v5 = Focus_GetSpinWidthOrZero();
		v6 = Rect.right - 2 * (artfont_GetFontDefWidth() + v5);
		v12 = SendMessageA(hWnd, 0x403u, 0, 0);
		if ( v12 == 1 )
			String[v14-1] = 0; // *(&v9 + v14) = 0;
		for ( i = String; *i; ++i )
		{
			if ( artfont_GetFontWidth(i) <= v6 )
				break;
		}
		if ( v12 )
			String[v14-1] = 124; // *(&v9 + v14) = 124;
		v8 = artfont_GetFontMaxHeight();
		artfont_PrintFontStr(i, (DWORD **)pWidthBin, v4, (*((_DWORD *)pWidthBin + 2) - v8) / 2);
	}
}

// ref: 0x10006C08
void __fastcall Doom_ParseWndProc6(HWND hWnd, int *msgtbl, int a3)
{
	HWND v5; // eax

	while ( *msgtbl )
	{
		v5 = GetDlgItem(hWnd, *msgtbl);
		Doom_GetSetWndTxt6(hWnd, (int)v5, a3);
		++msgtbl;
	}
}

// ref: 0x10006C33
void __fastcall Doom_GetSetWndTxt6(HWND hWnd, int msg, int nFont)
{
	HWND v3; // esi

	v3 = (HWND)msg;
	if ( msg )
	{
		Doom_GetWindowROP3(hWnd, (HWND)msg);
		artfont_SetArtFont(nFont);
		Doom_PrintTextMsg403(v3);
	}
}

// ref: 0x10006C53
void __fastcall Doom_DeleteFreeProcs(HWND hWnd, int *msgtbl)
{
	int i; // eax
	HWND v5; // eax MAPDST
	void **v7; // eax

	for ( i = *msgtbl; *msgtbl; i = *msgtbl )
	{
		v5 = GetDlgItem(hWnd, i);
		if ( v5 )
		{
			v7 = (void **)GetWindowLongA(v5, -21);
			local_FreeMemPtr(v7);
			SetWindowLongA(v5, -21, 0);
		}
		++msgtbl;
	}
}

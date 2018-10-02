// ref: 0x10007825
void __cdecl local_InitUiPalette()
{
	LOGPALETTE *v0; // eax MAPDST
	PALETTEENTRY *v2; // eax
	int v3; // ecx

	v0 = (LOGPALETTE *)SMemAlloc(0x404u, "C:\\Src\\Diablo\\DiabloUI\\local.cpp", 27, 0);
	if ( v0 )
	{
		v0->palVersion = 768;
		v0->palNumEntries = 256;
		v2 = v0->palPalEntry;
		v3 = 0;
		do
		{
			v2[v3].peFlags = 2;
			++v3;
		}
		while ( v3 < 256 );
		objPalette = CreatePalette(v0);
		SMemFree(v0, "C:\\Src\\Diablo\\DiabloUI\\local.cpp", 41, 0);
	}
}

// ref: 0x1000787D
void __cdecl local_DelUiPalette()
{
	if ( objPalette )
	{
		DeleteObject(objPalette);
		objPalette = 0;
	}
}

// ref: 0x10007895
tagPALETTEENTRY *__fastcall local_GetArtPalEntry(int entry)
{
	return &artpal[entry];
}

// ref: 0x1000789D
void __fastcall local_ClearPalette(PALETTEENTRY *pPal)
{
	BYTE *v1; // eax
	signed int v2; // ecx

	v1 = &pPal->peBlue;
	v2 = 256;
	do
	{
		*(v1 - 2) = 0;
		*(v1 - 1) = 0;
		*v1 = 0;
		v1 += 4;
		--v2;
	}
	while ( v2 );
}

// ref: 0x100078B6
void __cdecl local_ClearSurface()
{
	SDrawClearSurface(0);
}
// 1001043C: using guessed type int __stdcall SDrawClearSurface(_DWORD);

// ref: 0x100078BE
BOOL __fastcall local_LoadArtImage(const char *pszFileName, BYTE **pBuffer, DWORD *pdwSize)
{
	BYTE *v4; // eax
	DWORD v5; // ecx
	DWORD dwHeight; // [esp+10h] [ebp-8h]
	DWORD dwWidth; // [esp+14h] [ebp-4h]

	*pBuffer = 0;
	if ( !SBmpLoadImage(pszFileName, 0, 0, 0, &dwWidth, &dwHeight, 0) )
		return 0;
	v4 = (BYTE *)SMemAlloc(dwHeight * dwWidth, "C:\\Src\\Diablo\\DiabloUI\\local.cpp", 93, 0);
	v5 = dwWidth;
	*pBuffer = v4;
	if ( !SBmpLoadImage(pszFileName, 0, v4, dwHeight * v5, 0, 0, 0) )
	{
		SMemFree(*pBuffer, "C:\\Src\\Diablo\\DiabloUI\\local.cpp", 95, 0);
		*pBuffer = 0;
		return 0;
	}
	if ( pdwSize )
	{
		*pdwSize = dwWidth;
		pdwSize[1] = dwHeight;
	}
	return 1;
}

// ref: 0x10007944
BOOL __fastcall local_LoadArtWithPal(HWND hWnd, int a2, char *src, int mask, int flags, const char *pszFileName, BYTE **pBuffer, DWORD *pdwSize, BOOL a9)
{
	BYTE *v10; // eax
	DWORD v11; // ST18_4
	HPALETTE v13; // edi
	tagPALETTEENTRY pPalEntries[256]; // [esp+Ch] [ebp-40Ch]
	DWORD pdwWidth; // [esp+410h] [ebp-8h]
	DWORD dwHeight; // [esp+414h] [ebp-4h]

	if ( !SBmpLoadImage(pszFileName, 0, 0, 0, &pdwWidth, &dwHeight, 0) )
		return 0;
	v10 = (BYTE *)SMemAlloc(dwHeight * pdwWidth, "C:\\Src\\Diablo\\DiabloUI\\local.cpp", 129, 0);
	v11 = dwHeight * pdwWidth;
	*pBuffer = v10;
	if ( !SBmpLoadImage(pszFileName, pPalEntries, v10, v11, 0, 0, 0)
	  || !SDlgSetBitmapI(hWnd, a2, src, mask, flags, *pBuffer, 0, pdwWidth, dwHeight, -1) )
	{
		return 0;
	}
	if ( !src || !*src )
	{
		v13 = (HPALETTE)GetStockObject(15);
		GetPaletteEntries(v13, 0, 0xAu, pPalEntries);
		GetPaletteEntries(v13, 0xAu, 0xAu, &pPalEntries[246]);
		memcpy(artpal, pPalEntries, 0x400u);
		if ( a9 )
		{
			SDrawUpdatePalette(0, 255, artpal, 1);
		}
		else
		{
			local_ClearPalette(pPalEntries);
			SDrawUpdatePalette(0, 256, pPalEntries, 1);
		}
	}
	if ( pdwSize )
	{
		*pdwSize = pdwWidth;
		pdwSize[1] = dwHeight;
	}
	return 1;
}
// 100103FA: using guessed type int __stdcall SDrawUpdatePalette(_DWORD, _DWORD, _DWORD, _DWORD);

// ref: 0x10007A68
void __fastcall local_AdjustRectSize(tagRECT *pRect, int a2, int a3)
{
	int v3; // eax
	int v4; // edx

	v3 = a2 - pRect->left;
	pRect->left = a2;
	pRect->right += v3 - 1;
	v4 = a3 - pRect->top;
	pRect->top = a3;
	pRect->bottom += v4 - 1;
}

// ref: 0x10007A85
BOOL __fastcall local_SetStaticBmp(HWND hWnd, int nIDDlgItem, BYTE *pBuffer, DWORD *pdwSize)
{
	HWND v4; // edi
	HWND v5; // ebx
	struct tagRECT Rect; // [esp+Ch] [ebp-10h]

	v4 = hWnd;
	v5 = GetDlgItem(hWnd, nIDDlgItem);
	GetWindowRect(v5, &Rect);
	ScreenToClient(v4, (LPPOINT)&Rect);
	ScreenToClient(v4, (LPPOINT)&Rect.right);
	SDlgSetBitmapI(v5, 0, "Static", -1, 1, pBuffer, (int)&Rect, *pdwSize, pdwSize[1], -1);
	return 1;
}

// ref: 0x10007AEA
void __cdecl local_cpp_init()
{
	local_cpp_float = 0x7F800000;
}
// 10029CB8: using guessed type int local_cpp_float;

// ref: 0x10007AF5
BOOL __fastcall local_SetButtonBmp(HWND hWnd, int flags, int a7, void *pBuffer, DWORD *pdwSize)
{
	return SDlgSetBitmapI(hWnd, 0, "Button", -1, flags, pBuffer, a7, *pdwSize, pdwSize[1], -1);
}

// ref: 0x10007B1B
void __fastcall local_FitButtonDlg(HWND hWnd, int *a2, void *pBuffer, DWORD *pdwSize)
{
	int v4; // eax
	HWND v5; // esi
	struct tagRECT Rect; // [esp+0h] [ebp-1Ch]
	int a3; // [esp+14h] [ebp-8h]
	int *v9; // [esp+18h] [ebp-4h]

	v4 = *a2;
	a3 = 0;
	v9 = a2;
	if ( v4 )
	{
		do
		{
			v5 = GetDlgItem(hWnd, v4);
			if ( v5 )
			{
				GetClientRect(v5, &Rect);
				local_AdjustRectSize(&Rect, 0, a3);
				local_SetButtonBmp(v5, 16, (int)&Rect, pBuffer, pdwSize);
				++Rect.bottom;
				++Rect.right;
				local_AdjustRectSize(&Rect, 0, Rect.bottom);
				local_SetButtonBmp(v5, 64, (int)&Rect, pBuffer, pdwSize);
				++Rect.bottom;
				++Rect.right;
				local_AdjustRectSize(&Rect, 0, Rect.bottom);
				local_SetButtonBmp(v5, 32, (int)&Rect, pBuffer, pdwSize);
				++Rect.bottom;
				++Rect.right;
				local_AdjustRectSize(&Rect, 0, Rect.bottom);
				local_SetButtonBmp(v5, 128, (int)&Rect, pBuffer, pdwSize);
				++Rect.bottom;
				++Rect.right;
				local_AdjustRectSize(&Rect, 0, Rect.bottom);
				local_SetButtonBmp(v5, 1280, (int)&Rect, pBuffer, pdwSize);
				++Rect.bottom;
				++Rect.right;
				a3 = Rect.bottom;
			}
			++v9;
			v4 = *v9;
		}
		while ( *v9 );
	}
}

// ref: 0x10007C2E
void __fastcall local_SetWhiteText(HDC hdc)
{
	SetTextColor(hdc, 0xFFFFu);
}

// ref: 0x10007C3B
BOOL __fastcall local_GetBottomRect(HWND hWnd1, HWND hWnd2, int width, int height)
{
	BOOL result; // eax
	struct tagRECT Rect; // [esp+4h] [ebp-10h]

	if ( hWnd1
	  && hWnd2
	  && (GetWindowRect(hWnd2, &Rect),
		  ScreenToClient(hWnd1, (LPPOINT)&Rect),
		  ScreenToClient(hWnd1, (LPPOINT)&Rect.right),
		  width >= Rect.left)
	  && width < Rect.right
	  && height >= Rect.top )
	{
		result = height < Rect.bottom;
	}
	else
	{
		result = 0;
	}
	return result;
}

// ref: 0x10007C95
void __fastcall local_DlgDoPaint(HWND hWnd)
{
	char v2[64]; // [esp+4h] [ebp-40h]

	SDlgBeginPaint(hWnd, v2);
	SDlgEndPaint(hWnd, v2);
}
// 10010442: using guessed type int __stdcall SDlgEndPaint(_DWORD, _DWORD);
// 10010448: using guessed type int __stdcall SDlgBeginPaint(_DWORD, _DWORD);

// ref: 0x10007CB5
void __fastcall local_DoUiWndProc(HWND hWnd, DWORD *pdwMsgTbl)
{
	DWORD *v2; // edi
	int i; // eax
	HWND v5; // eax
	HWND v6; // esi
	void *v7; // eax

	v2 = pdwMsgTbl;
	for ( i = *pdwMsgTbl; *v2; i = *v2 )
	{
		v5 = GetDlgItem(hWnd, i);
		v6 = v5;
		if ( v5 )
		{
			v7 = (void *)GetWindowLongA(v5, -4);
			SetPropA(v6, "UIWNDPROC", v7);
			SetWindowLongA(v6, -4, (LONG)local_PostUiWndProc);
		}
		++v2;
	}
}

// ref: 0x10007D01
LRESULT __stdcall local_PostUiWndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	LRESULT (__stdcall *v4)(HWND, UINT, WPARAM, LPARAM); // ebx
	HWND v5; // eax
	HWND v6; // eax
	HWND v7; // eax
	WPARAM v9; // [esp-8h] [ebp-14h]
	BOOL v10; // [esp-4h] [ebp-10h]

	v4 = (LRESULT (__stdcall *)(HWND, UINT, WPARAM, LPARAM))GetPropA(hWnd, "UIWNDPROC");
	switch ( uMsg )
	{
		case 2u:
			RemovePropA(hWnd, "UIWNDPROC");
			if ( !v4 )
				return DefWindowProcA(hWnd, uMsg, wParam, lParam);
			SetWindowLongA(hWnd, -4, (LONG)v4);
			goto LABEL_21;
		case 0xFu:
			local_DlgDoPaint(hWnd);
			return 0;
		case 0x87u:
			return 4;
	}
	if ( uMsg != 256 )
		goto LABEL_21;
	switch ( wParam )
	{
		case 0xDu:
			goto LABEL_26;
		case 0x1Bu:
			v9 = 2;
LABEL_15:
			v7 = GetParent(hWnd);
			SendMessageA(v7, 0x111u, v9, 0);
			goto LABEL_21;
		case 0x20u:
LABEL_26:
			v9 = 1;
			goto LABEL_15;
	}
	if ( wParam <= 0x24 )
		goto LABEL_21;
	if ( wParam <= 0x26 )
	{
		v10 = 1;
	}
	else
	{
		if ( wParam > 0x28 )
			goto LABEL_21;
		v10 = 0;
	}
	v5 = GetParent(hWnd);
	v6 = GetNextDlgGroupItem(v5, hWnd, v10);
	SetFocus(v6);
LABEL_21:
	if ( v4 )
		return CallWindowProcA(v4, hWnd, uMsg, wParam, lParam);
	return DefWindowProcA(hWnd, uMsg, wParam, lParam);
}

// ref: 0x10007DE9
void __fastcall local_DoUiWndProc2(HWND hWnd, DWORD *pdwMsgTbl)
{
	DWORD *v2; // edi
	int i; // eax
	HWND v5; // eax
	HWND v6; // esi
	void *v7; // eax

	v2 = pdwMsgTbl;
	for ( i = *pdwMsgTbl; *v2; i = *v2 )
	{
		v5 = GetDlgItem(hWnd, i);
		v6 = v5;
		if ( v5 )
		{
			v7 = (void *)GetWindowLongA(v5, -4);
			SetPropA(v6, "UIWNDPROC", v7);
			SetWindowLongA(v6, -4, (LONG)local_PostUiWndProc2);
		}
		++v2;
	}
}

// ref: 0x10007E35
LRESULT __stdcall local_PostUiWndProc2(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	LRESULT (__stdcall *v4)(HWND, UINT, WPARAM, LPARAM); // ebx
	WPARAM v5; // ST0C_4
	HWND v6; // eax
	HWND v8; // [esp+18h] [ebp+Ch]

	v4 = (LRESULT (__stdcall *)(HWND, UINT, WPARAM, LPARAM))GetPropA(hWnd, "UIWNDPROC");
	switch ( uMsg )
	{
		case 2u:
			RemovePropA(hWnd, "UIWNDPROC");
			if ( !v4 )
				return DefWindowProcA(hWnd, uMsg, wParam, lParam);
			SetWindowLongA(hWnd, -4, (LONG)v4);
			break;
		case 0xFu:
			local_DlgDoPaint(hWnd);
			return 0;
		case 0x201u:
			v8 = GetFocus();
			SetFocus(hWnd);
			InvalidateRect(v8, 0, 0);
			InvalidateRect(hWnd, 0, 0);
			UpdateWindow(v8);
			UpdateWindow(hWnd);
			v5 = (unsigned short)GetWindowLongA(hWnd, -12);
			v6 = GetParent(hWnd);
			PostMessageA(v6, 0x111u, v5, (LPARAM)hWnd);
			return 0;
	}
	if ( v4 )
		return CallWindowProcA(v4, hWnd, uMsg, wParam, lParam);
	return DefWindowProcA(hWnd, uMsg, wParam, lParam);
}

// ref: 0x10007F04
BOOL __fastcall local_DisableKeyWaitMouse(HWND hWnd)
{
	BOOL result; // eax
	struct tagMSG Msg; // [esp+8h] [ebp-1Ch]

	do
	{
		while ( PeekMessageA(&Msg, hWnd, 0x100u, 0x108u, 1u) )
			;
		result = PeekMessageA(&Msg, hWnd, 0x200u, 0x209u, 1u);
	}
	while ( result );
	return result;
}

// ref: 0x10007F46
DWORD *__cdecl local_AllocWndLongData()
{
	DWORD *result; // eax

	result = (DWORD *)SMemAlloc(0x110u, "C:\\Src\\Diablo\\DiabloUI\\local.cpp", 480, 0);
	if ( result )
	{
		*result = 0;
		result[1] = 0;
		result[2] = 0;
		result[3] = 0;
		*((_BYTE *)result + 16) = 0;
	}
	return result;
}

// ref: 0x10007F72
void __fastcall local_FreeMemPtr(void **p)
{
	if ( p )
	{
		if ( *p )
			SMemFree(*p, "C:\\Src\\Diablo\\DiabloUI\\local.cpp", 498, 0);
		SMemFree(p, "C:\\Src\\Diablo\\DiabloUI\\local.cpp", 499, 0);
	}
}

// ref: 0x10007FA4
void __fastcall local_SetWndLongStr(int WndLongData, const char *pszStr)
{
	if ( WndLongData )
	{
		if ( pszStr )
		{
			strncpy((char *)(WndLongData + 16), pszStr, 0xFFu);
			*(_BYTE *)(WndLongData + 271) = 0;
		}
		else
		{
			*(_BYTE *)(WndLongData + 16) = 0;
		}
	}
}

// ref: 0x10007FD0
void __cdecl local_LoadArtCursor()
{
	DWORD dwHeight; // [esp+8h] [ebp-8h]
	DWORD dwWidth; // [esp+Ch] [ebp-4h]

	if ( SBmpLoadImage("ui_art\\cursor.pcx", 0, 0, 0, &dwWidth, &dwHeight, 0) )
	{
		gpCursorArt = (BYTE *)SMemAlloc(dwHeight * dwWidth, "C:\\Src\\Diablo\\DiabloUI\\local.cpp", 553, 0);
		gpCursorArt2 = (BYTE *)SMemAlloc(dwHeight * dwWidth, "C:\\Src\\Diablo\\DiabloUI\\local.cpp", 554, 0);
		if ( SBmpLoadImage("ui_art\\cursor.pcx", 0, gpCursorArt, dwHeight * dwWidth, 0, 0, 0) )
		{
			gdwCursData[0] = dwWidth;
			gdwCursData[1] = dwHeight;
			local_InitArtCursor();
		}
	}
}

// ref: 0x10008062
void __cdecl local_InitArtCursor()
{
	BYTE *v0; // eax
	BYTE *v1; // ecx
	int i; // esi
	char v3; // dl

	v0 = gpCursorArt2;
	v1 = gpCursorArt;
	if ( gpCursorArt2 )
	{
		if ( gpCursorArt )
		{
			for ( i = 0; i < gdwCursData[0] * gdwCursData[1]; ++i )
			{
				v3 = *v1++;
				if ( v3 )
					*v0 = 0;
				else
					*v0 = -1;
				++v0;
			}
		}
	}
}

// ref: 0x100080AD
void __cdecl local_FreeArtCursor()
{
	if ( gpCursorArt )
	{
		SMemFree(gpCursorArt, "C:\\Src\\Diablo\\DiabloUI\\local.cpp", 571, 0);
		gpCursorArt = 0;
	}
	if ( gpCursorArt2 )
	{
		SMemFree(gpCursorArt2, "C:\\Src\\Diablo\\DiabloUI\\local.cpp", 575, 0);
		gpCursorArt2 = 0;
	}
}

// ref: 0x100080F1
void __cdecl local_SetCursorArt()
{
	if ( !gpCursorArt )
		local_LoadArtCursor();
	SDlgSetSystemCursor(gpCursorArt2, gpCursorArt, (int *)gdwCursData, 32512);
}
// 1001044E: using guessed type int __stdcall SDlgSetSystemCursor(_DWORD, _DWORD, _DWORD, _DWORD);

// ref: 0x1000811B
void __cdecl local_SetCursorDefault()
{
	SDlgSetSystemCursor(0, 0, 0, 32512);
}
// 1001044E: using guessed type int __stdcall SDlgSetSystemCursor(_DWORD, _DWORD, _DWORD, _DWORD);

// ref: 0x1000812B
void __fastcall local_SetDiabloCursor(HWND hWnd)
{
	HCURSOR v2; // eax
	int v3; // [esp+4h] [ebp-4h]

	v3 = 0;
	local_SetCursorDefault();
	v2 = LoadCursorA(ghUiInst, "DIABLOCURSOR");
	SDlgSetCursor(hWnd, v2, 32512, &v3);
}
// 10010454: using guessed type int __stdcall SDlgSetCursor(_DWORD, _DWORD, _DWORD, _DWORD);

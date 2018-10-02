// ref: 0x10008F31
BOOL __fastcall OkCancel_DrawString(HWND hWnd, char *str)
{
	HDC v3; // edi
	void *v4; // eax
	int v5; // eax
	BOOL result; // eax
	LONG v7; // [esp+14h] [ebp-20h]
	LONG v8; // [esp+18h] [ebp-1Ch]
	struct tagRECT Rect; // [esp+1Ch] [ebp-18h]
	HGDIOBJ h; // [esp+2Ch] [ebp-8h]

	if ( !str || !*str )
		goto LABEL_13;
	if ( !hWnd )
		goto LABEL_14;
	GetClientRect(hWnd, &Rect);
	--Rect.right;
	--Rect.bottom;
	v7 = Rect.right;
	v8 = Rect.bottom;
	v3 = GetDC(hWnd);
	v4 = (void *)SendMessageA(hWnd, 0x31u, 0, 0);
	h = SelectObject(v3, v4);
	if ( !v3 )
		goto LABEL_13;
	v5 = strlen(str);
	DrawTextA(v3, str, v5, &Rect, 0x410u);
	if ( h )
		SelectObject(v3, h);
	ReleaseDC(hWnd, v3);
	if ( Rect.bottom > v8 || Rect.right > v7 )
LABEL_14:
		result = 1;
	else
LABEL_13:
		result = 0;
	return result;
}

// ref: 0x10008FEC
void __cdecl OkCancel_cpp_init()
{
	OkCancel_cpp_float = OkCancel_cpp_float_value;
}
// 1001F440: using guessed type int OkCancel_cpp_float_value;
// 1002A2E4: using guessed type int OkCancel_cpp_float;

// ref: 0x10008FF7
LRESULT __stdcall OkCancel_WndProc(HWND hWnd, UINT Msg, WPARAM wParam, LPARAM lParam)
{
	HWND v5; // ecx
	int v6; // edx
	HWND v7; // eax
	LONG v8; // eax
	HWND v9; // eax

	if ( Msg == 2 )
	{
		ShowCursor(0);
		OkCancel_FreeDlgBmp(hWnd);
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
	if ( Msg != 272 )
	{
		if ( Msg != 273 )
		{
			if ( Msg == 312 && GetWindowLongA((HWND)lParam, -12) == 1038 )
			{
				local_SetWhiteText((HDC)wParam);
				return (LRESULT)GetStockObject(5);
			}
			return (LRESULT)SDlgDefDialogProc(hWnd, Msg, (HDC)wParam, (HWND)lParam);
		}
		if ( (unsigned short)wParam == 1 )
		{
			v7 = GetFocus();
			v8 = GetWindowLongA(v7, -12);
			v5 = hWnd;
			if ( v8 == 1109 )
			{
				v6 = 1;
				goto LABEL_16;
			}
		}
		else
		{
			if ( (unsigned short)wParam != 2 )
			{
				if ( (unsigned short)wParam == 1109 )
				{
					v5 = hWnd;
					v6 = 1;
LABEL_16:
					OkCancel_PlaySndEndDlg(v5, v6);
				}
				return (LRESULT)SDlgDefDialogProc(hWnd, Msg, (HDC)wParam, (HWND)lParam);
			}
			v5 = hWnd;
		}
		v6 = 2;
		goto LABEL_16;
	}
	ShowCursor(1);
	if ( !OkCancel_LoadOkCancGFX(hWnd, (DWORD *)lParam) )
		SDlgEndDialog(hWnd, (HANDLE)0xFF000000);
	return 1;
}
// 10010382: using guessed type _DWORD __stdcall SDrawGetFrameWindow();

// ref: 0x10009117
void __fastcall OkCancel_FreeDlgBmp(HWND hWnd)
{
	void **v1; // eax MAPDST
	void *v3; // eax
	void *v4; // eax

	v1 = (void **)RemovePropA(hWnd, "DLGBMP");
	if ( v1 )
	{
		v3 = *v1;
		if ( v3 )
			SMemFree(v3, "C:\\Src\\Diablo\\DiabloUI\\OkCancel.cpp", 48, 0);
		v4 = v1[1];
		if ( v4 )
			SMemFree(v4, "C:\\Src\\Diablo\\DiabloUI\\OkCancel.cpp", 50, 0);
		SMemFree(v1, "C:\\Src\\Diablo\\DiabloUI\\OkCancel.cpp", 51, 0);
	}
}

// ref: 0x10009161
BOOL __fastcall OkCancel_LoadOkCancGFX(HWND hWnd, DWORD *lParam)
{
	bool v3; // zf
	HWND v4; // edi
	tagPALETTEENTRY *v5; // edi
	HWND v6; // eax
	HWND v7; // edi
	BYTE **v8; // edi
	HWND v9; // eax
	const CHAR *v10; // ST1C_4
	HWND v11; // eax
	HWND v12; // edi
	int a2a; // [esp+Ch] [ebp-20h]
	int v15; // [esp+10h] [ebp-1Ch]
	int v16; // [esp+14h] [ebp-18h]
	DWORD data[2]; // [esp+18h] [ebp-14h]
	BYTE **pBuffer; // [esp+20h] [ebp-Ch]
	char *pszFileName; // [esp+24h] [ebp-8h]

	a2a = 1109;
	v15 = 2;
	v16 = 0;
	pBuffer = (BYTE **)SMemAlloc(8u, "C:\\Src\\Diablo\\DiabloUI\\OkCancel.cpp", 110, 0);
	SetPropA(hWnd, "DLGBMP", pBuffer);
	if ( lParam[2] )
	{
		v3 = lParam[3] == 0;
		pszFileName = "ui_art\\lrpopup.pcx";
		if ( v3 )
			pszFileName = "ui_art\\lpopup.pcx";
	}
	else if ( lParam[3] )
	{
		pszFileName = "ui_art\\srpopup.pcx";
	}
	else
	{
		pszFileName = "ui_art\\spopup.pcx";
	}
	v4 = GetParent(hWnd);
	if ( (HWND)SDrawGetFrameWindow() == v4 )
	{
		local_LoadArtWithPal(hWnd, 0, &nullcharacter, -1, 1, pszFileName, pBuffer, 0, 1);
		v5 = local_GetArtPalEntry(0);
		SDrawUpdatePalette(0, 0xAu, v5, 0);
		SDrawUpdatePalette(0x70u, 0x90u, v5 + 112, 1);
	}
	else
	{
		v6 = GetParent(hWnd);
		local_LoadArtWithPal(hWnd, (int)v6, "Popup", -1, 1, pszFileName, pBuffer, 0, 1);
	}
	v7 = GetParent(hWnd);
	if ( (HWND)SDrawGetFrameWindow() == v7 )
		Fade_SetInputWindow(hWnd);
	v8 = pBuffer + 1;
	local_LoadArtImage("ui_art\\but_sml.pcx", pBuffer + 1, data);
	local_FitButtonDlg(hWnd, &a2a, *v8, data);
	v9 = GetDlgItem(hWnd, 1026);
	v10 = (const CHAR *)lParam[1];
	pBuffer = (BYTE **)v9;
	SetWindowTextA(v9, v10);
	if ( lParam[4] && OkCancel_DrawString((HWND)pBuffer, (char *)lParam[1]) )
		return 0;
	if ( *lParam )
	{
		v11 = GetDlgItem(hWnd, 1038);
		v12 = v11;
		if ( lParam[4] && OkCancel_DrawString(v11, (char *)*lParam) )
			return 0;
		if ( v12 )
			SetWindowTextA(v12, (LPCSTR)*lParam);
	}
	return 1;
}
// 10010382: using guessed type _DWORD __stdcall SDrawGetFrameWindow();

// ref: 0x100092F5
void __fastcall OkCancel_PlaySndEndDlg(HWND hWnd, int a2)
{
	TitleSnd_PlaySelectSound();
	SDlgEndDialog(hWnd, (HANDLE)a2);
}

// ref: 0x1000930A
void __fastcall OkCancel_DoOkDialog(HWND hWnd, char *str, int a3)
{
	int a5[5]; // [esp+0h] [ebp-14h]

	a5[1] = (int)str;
	a5[2] = 0;
	a5[0] = 0;
	a5[4] = 0;
	a5[3] = a3;
	SDlgDialogBoxParam(ghUiInst, "OK_DIALOG", (int)hWnd, OkCancel_WndProc, (int)a5);
}

// ref: 0x10009342
void __stdcall UiMessageBoxCallback(HWND hWnd, char *lpText, LPCSTR lpCaption, UINT uType)
{
	int v4; // eax
	unsigned char v5; // sf
	unsigned char v6; // of
	size_t v7; // eax
	char *v8; // eax
	int v9; // ecx
	int a5[5]; // [esp+0h] [ebp-24h]
	int v11; // [esp+14h] [ebp-10h]
	void *location; // [esp+18h] [ebp-Ch]
	char *szDialog; // [esp+1Ch] [ebp-8h]
	char *v14; // [esp+20h] [ebp-4h]

	a5[0] = (int)lpCaption;
	a5[1] = (int)lpText;
	szDialog = "OK_DIALOG";
	a5[4] = 1;
	if ( uType & 0xF )
		szDialog = "OKCANCEL_DIALOG";
	a5[3] = (uType & 0xF0) == 16 || (uType & 0xF0) == 48;
	v4 = 0;
	a5[2] = 0;
	while ( 1 )
	{
		v14 = (char *)SDlgDialogBoxParam(ghUiInst, &szDialog[32 * v4], (int)hWnd, OkCancel_WndProc, (int)a5);
		if ( v14 != (char *)0xFF000000 )
			break;
		v4 = a5[2] + 1;
		v6 = __OFSUB__(a5[2] + 1, 2);
		v5 = a5[2]++ - 1 < 0;
		if ( !(v5 ^ v6) )
		{
			v7 = strlen(lpText);
			v8 = (char *)SMemAlloc(v7 + 256, "C:\\Src\\Diablo\\DiabloUI\\OkCancel.cpp", 392, 0);
			v11 = 0; /* check */
			location = v8;
			v14 = lpText;
			if ( *lpText )
			{
				v9 = v11;
				do
				{
					if ( *v14 <= 32 )
						v9 = 0;
					*v8++ = *v14;
					if ( ++v9 > 18 )
					{
						*v8++ = 10;
						v9 = 0;
					}
					++v14;
				}
				while ( *v14 );
			}
			*v8 = 0;
			a5[1] = (int)location;
			v14 = (char *)SDlgDialogBoxParam(ghUiInst, szDialog, (int)hWnd, OkCancel_WndProc, (int)a5);
			SMemFree(location, "C:\\Src\\Diablo\\DiabloUI\\OkCancel.cpp", 416, 0);
			if ( v14 == (char *)0xFF000000 )
				MessageBoxA(hWnd, lpText, lpCaption, uType);
			return;
		}
	}
}

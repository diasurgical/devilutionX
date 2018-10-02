// ref: 0x10004054
BOOL __stdcall UiCopyProtError(int *pdwResult)
{
	int v1; // eax
	int v2; // eax
	char Buffer[128]; // [esp+0h] [ebp-80h]

	if ( DiabloUI_GetSpawned() )
		LoadStringA(ghUiInst, 0x3Fu, Buffer, 127);
	else
		LoadStringA(ghUiInst, 0x1Bu, Buffer, 127);
	v1 = (int)SDrawGetFrameWindow();
	v2 = SDlgDialogBoxParam(ghUiInst, "OKCANCEL_DIALOG", v1, CopyProt_WndProc, (int)Buffer);
	if ( pdwResult )
		*pdwResult = v2;
	return 1;
}
// 10010382: using guessed type _DWORD __stdcall SDrawGetFrameWindow();

// ref: 0x100040AF
LRESULT __stdcall CopyProt_WndProc(HWND hWnd, UINT Msg, WPARAM wParam, LPARAM lParam)
{
	HWND v4; // ecx
	int v5; // edx
	HWND v6; // eax
	LONG v7; // eax
	HWND v9; // eax

	if ( Msg == 2 )
	{
		CopyProt_FreeCopyResrcs();
		Fade_UpdatePaletteRange(10);
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
		CopyProt_LoadCopyStuff(hWnd, lParam);
		return 1;
	}
	if ( Msg != 273 )
		return (LRESULT)SDlgDefDialogProc(hWnd, Msg, (HDC)wParam, (HWND)lParam);
	switch ( (unsigned short)wParam )
	{
		case 1u:
			v6 = GetFocus();
			v7 = GetWindowLongA(v6, -12);
			v4 = hWnd;
			if ( v7 == 1109 )
			{
				v5 = 1;
				goto LABEL_13;
			}
			goto LABEL_12;
		case 2u:
			v4 = hWnd;
LABEL_12:
			v5 = 2;
			goto LABEL_13;
		case 0x455u:
			v4 = hWnd;
			v5 = 1;
LABEL_13:
			CopyProt_EndCopyDlg(v4, v5);
			break;
	}
	return (LRESULT)SDlgDefDialogProc(hWnd, Msg, (HDC)wParam, (HWND)lParam);
}
// 10010382: using guessed type _DWORD __stdcall SDrawGetFrameWindow();

// ref: 0x10004173
void __cdecl CopyProt_FreeCopyResrcs()
{
	if ( copyprot_artpal )
	{
		FreeResource(copyprot_artpal);
		copyprot_artpal = 0;
	}
	if ( copyprot_btnart )
	{
		FreeResource(copyprot_btnart);
		copyprot_btnart = 0;
	}
	if ( copyprot_popupart )
	{
		FreeResource(copyprot_popupart);
		copyprot_popupart = 0;
	}
}

// ref: 0x100041B5
BOOL __fastcall CopyProt_LoadCopyStuff(HWND hWnd, int a2)
{
	HRSRC v2; // eax
	HRSRC v3; // eax
	HRSRC v4; // eax
	void *v5; // edi
	void *v6; // ebx
	HWND v7; // eax
	PALETTEENTRY pPalEntries[256]; // [esp+Ch] [ebp-420h]
	int msgs[3]; // [esp+40Ch] [ebp-20h]
	DWORD data[2]; // [esp+418h] [ebp-14h]
	LPCSTR lpString; // [esp+420h] [ebp-Ch]
	void *v13; // [esp+424h] [ebp-8h]

	msgs[2] = 0;
	lpString = (LPCSTR)a2;
	data[0] = 112;
	data[1] = 140;
	msgs[0] = 1109;
	msgs[1] = 2;
	v2 = FindResourceA(ghUiInst, "IDR_POPUPART", "ART_FILES");
	copyprot_popupart = LoadResource(ghUiInst, v2);
	v3 = FindResourceA(ghUiInst, "IDR_BTNART", "ART_FILES");
	copyprot_btnart = LoadResource(ghUiInst, v3);
	v4 = FindResourceA(ghUiInst, "IDR_ARTPAL", "ART_FILES");
	copyprot_artpal = LoadResource(ghUiInst, v4);
	v5 = LockResource(copyprot_popupart);
	v6 = LockResource(copyprot_btnart);
	v13 = LockResource(copyprot_artpal);
	if ( v5 )
		SDlgSetBitmapI(hWnd, 0, &nullcharacter, -1, 1, v5, 0, 284, 148, -1);
	ShowCursor(1);
	Fade_SetInputWindow(hWnd);
	if ( v6 )
		local_FitButtonDlg(hWnd, msgs, v6, data);
	if ( v13 )
	{
		memcpy(pPalEntries, v13, 0x400u);
		SDrawUpdatePalette(0, 0x100u, pPalEntries, 1);
	}
	v7 = GetDlgItem(hWnd, 1026);
	SetWindowTextA(v7, lpString);
	return 1;
}

// ref: 0x1000430C
void __fastcall CopyProt_EndCopyDlg(HWND hWnd, int a2)
{
	ShowCursor(0);
	SDlgEndDialog(hWnd, (HANDLE)a2);
}

// ref: 0x10004329
void __cdecl CopyProt_cpp_init()
{
	CopyProt_cpp_float = CopyProt_cpp_float_value;
}
// 1001F3F8: using guessed type int CopyProt_cpp_float_value;
// 10029620: using guessed type int CopyProt_cpp_float;

// ref: 0x10009480
signed int Progress_10009480() { return 0; }
/* {
	signed int result; // eax

	result = 2139095040;
	dword_1002A2EC = 2139095040;
	return result;
} */
// 1002A2EC: using guessed type int dword_1002A2EC;

// ref: 0x1000948B
int __stdcall UiProgressDialog(HWND window, char *msg, int a3, void *fnfunc, int a5) { return 0; }
/* {
	HWND v5; // eax
	BOOL result; // eax

	dword_1002A2E8 = -1;
	dword_1002A2F8 = 0;
	dword_1002A2F4 = a4;
	bEnable = a3;
	dword_1002A2F0 = a5;
	v5 = (HWND)SDlgCreateDialogParam(hInstance, "PROGRESS_DIALOG", a1, Progress_100094F4, a2);
	result = 0;
	if ( v5 )
	{
		Progress_1000991C(v5);
		if ( dword_1002A2E8 != 2 && dword_1002A2E8 != -1 )
			result = 1;
	}
	return result;
} */
// 1001045A: using guessed type int __stdcall SDlgCreateDialogParam(_DWORD, _DWORD, _DWORD, _DWORD, _DWORD);
// 1002A2E8: using guessed type int dword_1002A2E8;
// 1002A2F0: using guessed type int dword_1002A2F0;
// 1002A2F4: using guessed type int (*dword_1002A2F4)(void);
// 1002A2F8: using guessed type int dword_1002A2F8;

// ref: 0x100094F4
int __stdcall Progress_100094F4(HWND hWnd, UINT Msg, WPARAM wParam, LPARAM lParam) { return 0; }
/* {
	HWND v4; // eax
	void *v5; // eax
	HWND v7; // eax

	if ( Msg == 2 )
	{
		ShowCursor(0);
		Progress_100095EC();
	}
	else if ( Msg > 0x103 )
	{
		if ( Msg <= 0x105 )
		{
			v7 = (HWND)SDrawGetFrameWindow();
			SendMessageA(v7, Msg, wParam, lParam);
		}
		else
		{
			switch ( Msg )
			{
				case 0x110u:
					Progress_10009675(hWnd, (const CHAR *)lParam);
					v5 = (void *)SDrawGetFrameWindow();
					local_1000812B(v5);
					local_1000812B(hWnd);
					ShowCursor(1);
					return 1;
				case 0x111u:
					if ( (_WORD)wParam == 2 )
					{
						SDlgKillTimer(hWnd, 1);
						v4 = GetParent(hWnd);
						if ( (HWND)SDrawGetFrameWindow() == v4 )
							Fade_100072BE(10);
						Progress_100098B0();
					}
					break;
				case 0x113u:
					Progress_100098C5(hWnd);
					break;
			}
		}
	}
	return SDlgDefDialogProc(hWnd, Msg, wParam, lParam);
} */
// 1001037C: using guessed type int __stdcall SDlgDefDialogProc(_DWORD, _DWORD, _DWORD, _DWORD);
// 10010382: using guessed type _DWORD __stdcall SDrawGetFrameWindow();
// 10010418: using guessed type int __stdcall SDlgKillTimer(_DWORD, _DWORD);

// ref: 0x100095EC
void *Progress_100095EC() { return 0; }
/* {
	int result; // eax

	if ( dword_1002A318 )
	{
		SMemFree(dword_1002A318, "C:\\Src\\Diablo\\DiabloUI\\Progress.cpp", 88, 0);
		dword_1002A318 = 0;
	}
	if ( dword_1002A31C )
	{
		SMemFree(dword_1002A31C, "C:\\Src\\Diablo\\DiabloUI\\Progress.cpp", 92, 0);
		dword_1002A31C = 0;
	}
	if ( dword_1002A320 )
	{
		SMemFree(dword_1002A320, "C:\\Src\\Diablo\\DiabloUI\\Progress.cpp", 96, 0);
		dword_1002A320 = 0;
	}
	if ( dword_1002A324 )
	{
		SMemFree(dword_1002A324, "C:\\Src\\Diablo\\DiabloUI\\Progress.cpp", 100, 0);
		dword_1002A324 = 0;
	}
	result = dword_1002A328;
	if ( dword_1002A328 )
	{
		result = SMemFree(dword_1002A328, "C:\\Src\\Diablo\\DiabloUI\\Progress.cpp", 104, 0);
		dword_1002A328 = 0;
	}
	return result;
} */
// 10010340: using guessed type int __stdcall SMemFree(_DWORD, _DWORD, _DWORD, _DWORD);
// 1002A318: using guessed type int dword_1002A318;
// 1002A31C: using guessed type int dword_1002A31C;
// 1002A320: using guessed type int dword_1002A320;
// 1002A324: using guessed type int dword_1002A324;
// 1002A328: using guessed type int dword_1002A328;

// ref: 0x10009675
BOOL __fastcall Progress_10009675(HWND hWnd, const CHAR *a2) { return 0; }
/* {
	HWND v2; // ebx
	HWND v3; // esi
	char *v4; // esi
	HWND v5; // eax
	HWND v6; // esi
	struct tagRECT Rect; // [esp+Ch] [ebp-28h]
	char v9; // [esp+1Ch] [ebp-18h]
	int v10; // [esp+24h] [ebp-10h]
	int v11; // [esp+28h] [ebp-Ch]
	LPCSTR lpString; // [esp+2Ch] [ebp-8h]
	HWND v13; // [esp+30h] [ebp-4h]

	v2 = hWnd;
	lpString = a2;
	v10 = 2;
	v11 = 0;
	if ( dword_1002A2F0 )
		SDlgSetTimer(hWnd, 1, 0x3E8u / dword_1002A2F0, 0);
	else
		SDlgSetTimer(hWnd, 1, 50, 0);
	local_10007944((int)v2, 0, &byte_10029448, -1, 1, (int)"ui_art\\spopup.pcx", &dword_1002A318, 0, 0);
	v3 = GetParent(v2);
	if ( (HWND)SDrawGetFrameWindow() == v3 )
		Fade_100073EF(v2);
	v4 = local_10007895(0);
	SDrawUpdatePalette(0, 10, v4, 0);
	SDrawUpdatePalette(112, 144, v4 + 448, 1);
	local_100078BE((int)"ui_art\\but_sml.pcx", &dword_1002A31C, &v9);
	local_10007B1B(v2, &v10, dword_1002A31C, &v9);
	local_100078BE((int)"ui_art\\prog_bg.pcx", &dword_1002A320, &dword_1002A310);
	local_100078BE((int)"ui_art\\prog_fil.pcx", &dword_1002A324, &dword_1002A308);
	v13 = GetDlgItem(v2, 1030);
	GetClientRect(v13, &Rect);
	dword_1002A328 = SMemAlloc(Rect.right * Rect.bottom, "C:\\Src\\Diablo\\DiabloUI\\Progress.cpp", 170, 0);
	dword_1002A300 = Rect.right;
	dword_1002A304 = Rect.bottom;
	SDlgSetBitmapI(v13, 0, 0, -1, 1, dword_1002A328, 0, Rect.right, Rect.bottom, -1);
	Progress_10009805(v2, 0);
	v5 = GetDlgItem(v2, 1031);
	SetWindowTextA(v5, lpString);
	v6 = GetDlgItem(v2, 2);
	ShowWindow(v6, bEnable != 0);
	return EnableWindow(v6, bEnable);
} */
// 10010364: using guessed type int __stdcall SMemAlloc(_DWORD, _DWORD, _DWORD, _DWORD);
// 10010382: using guessed type _DWORD __stdcall SDrawGetFrameWindow();
// 100103FA: using guessed type int __stdcall SDrawUpdatePalette(_DWORD, _DWORD, _DWORD, _DWORD);
// 10010400: using guessed type int __stdcall SDlgSetBitmapI(_DWORD, _DWORD, _DWORD, _DWORD, _DWORD, _DWORD, _DWORD, _DWORD, _DWORD, _DWORD);
// 10010412: using guessed type int __stdcall SDlgSetTimer(_DWORD, _DWORD, _DWORD, _DWORD);
// 1002A2F0: using guessed type int dword_1002A2F0;
// 1002A300: using guessed type int dword_1002A300;
// 1002A304: using guessed type int dword_1002A304;
// 1002A308: using guessed type int dword_1002A308;
// 1002A310: using guessed type int dword_1002A310;
// 1002A318: using guessed type int dword_1002A318;
// 1002A31C: using guessed type int dword_1002A31C;
// 1002A320: using guessed type int dword_1002A320;
// 1002A324: using guessed type int dword_1002A324;
// 1002A328: using guessed type int dword_1002A328;

// ref: 0x10009805
BOOL __fastcall Progress_10009805(HWND hWnd, int a2) { return 0; }
/* {
	HWND v2; // edi
	struct tagRECT Rect; // [esp+8h] [ebp-18h]
	HWND hWnda; // [esp+18h] [ebp-8h]
	int v6; // [esp+1Ch] [ebp-4h]

	v2 = hWnd;
	v6 = a2;
	hWnda = GetDlgItem(hWnd, 1030);
	SBltROP3(
		dword_1002A328,
		dword_1002A320,
		dword_1002A300,
		dword_1002A304,
		dword_1002A300,
		dword_1002A310,
		0,
		13369376);
	SBltROP3(
		dword_1002A328,
		dword_1002A324,
		v6 * dword_1002A300 / 100,
		dword_1002A304,
		dword_1002A300,
		dword_1002A308,
		0,
		13369376);
	GetWindowRect(hWnda, &Rect);
	ScreenToClient(v2, (LPPOINT)&Rect);
	ScreenToClient(v2, (LPPOINT)&Rect.right);
	return InvalidateRect(v2, &Rect, 0);
} */
// 100103F4: using guessed type int __stdcall SBltROP3(_DWORD, _DWORD, _DWORD, _DWORD, _DWORD, _DWORD, _DWORD, _DWORD);
// 1002A300: using guessed type int dword_1002A300;
// 1002A304: using guessed type int dword_1002A304;
// 1002A308: using guessed type int dword_1002A308;
// 1002A310: using guessed type int dword_1002A310;
// 1002A320: using guessed type int dword_1002A320;
// 1002A324: using guessed type int dword_1002A324;
// 1002A328: using guessed type int dword_1002A328;

// ref: 0x100098B0
void Progress_100098B0() { return; }
/* {
	dword_1002A2E8 = 2;
	dword_1002A2F8 = 1;
} */
// 1002A2E8: using guessed type int dword_1002A2E8;
// 1002A2F8: using guessed type int dword_1002A2F8;

// ref: 0x100098C5
void UNKCALL Progress_100098C5(HWND hWnd) { return; }
/* {
	HWND v1; // esi
	int v2; // eax
	HWND v3; // edi
	int v4; // edx

	v1 = hWnd;
	v2 = dword_1002A2F4();
	if ( v2 >= 100 )
	{
		SDlgKillTimer(v1, 1);
		v3 = GetParent(v1);
		if ( (HWND)SDrawGetFrameWindow() == v3 )
			Fade_100072BE(10);
		Progress_100098B0();
		dword_1002A2E8 &= v4;
	}
	else
	{
		Progress_10009805(v1, v2);
	}
} */
// 10010382: using guessed type _DWORD __stdcall SDrawGetFrameWindow();
// 10010418: using guessed type int __stdcall SDlgKillTimer(_DWORD, _DWORD);
// 1002A2E8: using guessed type int dword_1002A2E8;
// 1002A2F4: using guessed type int (*dword_1002A2F4)(void);

// ref: 0x1000991C
BOOL UNKCALL Progress_1000991C(HWND hWnd) { return 0; }
/* {
	HWND v1; // edi
	BOOL result; // eax
	struct tagMSG Msg; // [esp+Ch] [ebp-1Ch]

	v1 = hWnd;
	for ( result = IsWindow(hWnd); result; result = IsWindow(v1) )
	{
		if ( dword_1002A2F8 )
			goto LABEL_12;
		if ( PeekMessageA(&Msg, 0, 0, 0, 1u) )
		{
			if ( Msg.message == 18 )
			{
				PostQuitMessage(Msg.wParam);
			}
			else if ( !IsDialogMessageA(v1, &Msg) )
			{
				TranslateMessage(&Msg);
				DispatchMessageA(&Msg);
			}
		}
		else
		{
			SDlgCheckTimers();
			SDlgUpdateCursor();
		}
	}
	if ( !dword_1002A2F8 )
		return result;
LABEL_12:
	result = DestroyWindow(v1);
	dword_1002A2F8 = 0;
	return result;
} */
// 10010460: using guessed type _DWORD __stdcall SDlgUpdateCursor();
// 10010466: using guessed type _DWORD __stdcall SDlgCheckTimers();
// 1002A2F8: using guessed type int dword_1002A2F8;

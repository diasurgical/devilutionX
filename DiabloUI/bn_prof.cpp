// ref: 0x100014E8
signed int bn_prof_100014E8() { return 0; }
/* {
	signed int result; // eax

	result = 2139095040;
	dword_10029404 = 2139095040;
	return result;
} */
// 10029404: using guessed type int dword_10029404;

// ref: 0x100014F3
int __cdecl UiProfileGetString() { return 0; }
//const char *UiProfileGetString() { return 0; }
/* {
	return "profile\\sex";
} */

// ref: 0x100014F9
void __cdecl UiProfileCallback() { return; }
//BOOL __stdcall UiProfileCallback(int a1, int a2, int a3, int a4, LPARAM a5, int a6, int a7, int a8, int (__stdcall *a9)(_DWORD, _DWORD, _DWORD, _DWORD)) { return 0; }
/* {
	const char *v9; // eax
	int v10; // eax

	lParam = a5;
	dword_10029408 = a6;
	dword_1002941C = a7;
	dword_10029418 = a8;
	dword_10029430 = a9;
	v9 = "DIALOG_PROFILE";
	if ( !a9 )
		v9 = "DIALOG_STATIC_PROFILE";
	v10 = SDlgDialogBoxParam(hInstance, v9, *(_DWORD *)(a3 + 8), bn_prof_1000155F, 0);
	return v10 && v10 != -1;
} */
// 10010370: using guessed type int __stdcall SDlgDialogBoxParam(_DWORD, _DWORD, _DWORD, _DWORD, _DWORD);
// 10029408: using guessed type int dword_10029408;
// 10029418: using guessed type int dword_10029418;
// 1002941C: using guessed type int dword_1002941C;
// 10029430: using guessed type int (__stdcall *dword_10029430)(_DWORD, _DWORD, _DWORD, _DWORD);

// ref: 0x1000155F
HGDIOBJ __stdcall bn_prof_1000155F(HWND hWnd, UINT Msg, WPARAM wParam, LPARAM lParam) { return 0; }
/* {
	HWND v4; // eax
	HWND v6; // edi

	if ( Msg <= 0x110 )
	{
		switch ( Msg )
		{
			case 0x110u:
				bn_prof_10001C0E(hWnd);
				break;
			case 2u:
				bn_prof_10001F29();
				break;
			case 0x2Bu:
				if ( wParam == 1134 )
				{
					bn_prof_100018CE((int)hWnd, lParam);
					return (HGDIOBJ)1;
				}
				return (HGDIOBJ)SDlgDefDialogProc(hWnd, Msg, wParam, lParam);
			default:
				if ( Msg > 0x103 && Msg <= 0x105 )
				{
					v4 = (HWND)SDrawGetFrameWindow();
					SendMessageA(v4, Msg, wParam, lParam);
				}
				return (HGDIOBJ)SDlgDefDialogProc(hWnd, Msg, wParam, lParam);
		}
		return 0;
	}
	if ( Msg == 273 )
	{
		if ( (unsigned short)wParam == 1 )
		{
			EnableWindow((HWND)lParam, 0);
			TitleSnd_10010315();
			if ( dword_10029430 )
				bn_prof_100016DD(hWnd);
			SDlgEndDialog(hWnd, 1);
		}
		else if ( (unsigned short)wParam == 2 )
		{
			EnableWindow((HWND)lParam, 0);
			TitleSnd_10010315();
			SDlgEndDialog(hWnd, 0);
		}
		else
		{
			if ( (unsigned short)wParam != 1134 || HIWORD(wParam) || dword_10029430 )
				return (HGDIOBJ)SDlgDefDialogProc(hWnd, Msg, wParam, lParam);
			bn_prof_10001A10(hWnd, (HWND)lParam);
		}
		return 0;
	}
	if ( Msg == 274 )
	{
		if ( wParam != 61536 )
			return (HGDIOBJ)SDlgDefDialogProc(hWnd, Msg, wParam, lParam);
		v6 = GetParent(hWnd);
		SDlgEndDialog(hWnd, 0);
		PostMessageA(v6, 0x112u, 0xF060u, lParam);
		return 0;
	}
	if ( Msg != 312 || GetWindowLongA((HWND)lParam, -12) != 1124 )
		return (HGDIOBJ)SDlgDefDialogProc(hWnd, Msg, wParam, lParam);
	SetTextColor((HDC)wParam, 0xFFFFu);
	return GetStockObject(5);
} */
// 10010376: using guessed type int __stdcall SDlgEndDialog(_DWORD, _DWORD);
// 1001037C: using guessed type int __stdcall SDlgDefDialogProc(_DWORD, _DWORD, _DWORD, _DWORD);
// 10010382: using guessed type _DWORD __stdcall SDrawGetFrameWindow();
// 10029430: using guessed type int (__stdcall *dword_10029430)(_DWORD, _DWORD, _DWORD, _DWORD);

// ref: 0x100016DD
void UNKCALL bn_prof_100016DD(HWND arg) { return; }
/* {
	int v1; // ebx
	int v2; // eax
	const char **v3; // edi
	int v4; // eax
	int v5; // ebx
	int *v6; // ebx
	LRESULT v7; // eax
	LPARAM v8; // eax
	size_t v9; // eax
	char *v10; // eax
	int v11; // ebx
	_DWORD *v12; // edi
	int v13; // eax
	int v14; // ebx
	size_t v15; // [esp+4h] [ebp-28h]
	char *v16; // [esp+8h] [ebp-24h]
	HWND hDlg; // [esp+Ch] [ebp-20h]
	int v18; // [esp+10h] [ebp-1Ch]
	int v19; // [esp+14h] [ebp-18h]
	int v20; // [esp+18h] [ebp-14h]
	char *v21; // [esp+1Ch] [ebp-10h]
	int v22; // [esp+20h] [ebp-Ch]
	int v23; // [esp+24h] [ebp-8h]
	char *v24; // [esp+28h] [ebp-4h]
	size_t v25; // [esp+28h] [ebp-4h]

	v1 = 0;
	hDlg = arg;
	if ( dword_10029430 )
	{
		v2 = SMemAlloc(4 * dword_10029408, "C:\\Src\\Diablo\\DiabloUI\\bn_prof.cpp", 193, 0);
		v3 = (const char **)v2;
		v22 = v2;
		v4 = SMemAlloc(4 * dword_10029408, "C:\\Src\\Diablo\\DiabloUI\\bn_prof.cpp", 195, 0);
		v18 = v4;
		v20 = 0;
		v23 = 0;
		if ( dword_10029408 > 0 )
		{
			v19 = v4 - (_DWORD)v3;
			do
			{
				v5 = 0;
				v24 = byte_1001F37C;
				v16 = *(char **)(4 * v23 + dword_10029418);
				v21 = *(char **)(4 * v23 + dword_1002941C);
				while ( 1 )
				{
					if ( !_strcmpi(v21, *((const char **)v24 - 1)) )
					{
						v15 = strlen(v16);
						if ( *v24 & 1 )
							break;
					}
					v24 += 16;
					++v5;
					if ( (signed int)v24 >= (signed int)&unk_1001F3BC )
						goto LABEL_13;
				}
				v6 = &dword_1001F380[4 * v5];
				v7 = SendDlgItemMessageA(hDlg, *v6, 0xEu, 0, 0);
				v25 = v7;
				v8 = SMemAlloc(v7 + 1, "C:\\Src\\Diablo\\DiabloUI\\bn_prof.cpp", 218, 0);
				*v3 = (const char *)v8;
				SendDlgItemMessageA(hDlg, *v6, 0xDu, v25 + 1, v8);
				(*v3)[v25] = 0;
				if ( v25 == v15 && !_strnicmp(v16, *v3, v25) )
				{
					SMemFree(*v3, "C:\\Src\\Diablo\\DiabloUI\\bn_prof.cpp", 236, 0);
				}
				else
				{
					v9 = strlen(v21);
					v10 = (char *)SMemAlloc(v9 + 1, "C:\\Src\\Diablo\\DiabloUI\\bn_prof.cpp", 232, 0);
					*(const char **)((char *)v3 + v19) = v10;
					strcpy(v10, v21);
					++v20;
					++v3;
				}
LABEL_13:
				++v23;
			}
			while ( v23 < dword_10029408 );
			v1 = v20;
		}
		dword_10029430(&byte_10029448, v1, v18, v22);
		v11 = v1 - 1;
		if ( v11 >= 0 )
		{
			v12 = (_DWORD *)(v22 + 4 * v11);
			v13 = v18 - v22;
			v14 = v11 + 1;
			while ( 1 )
			{
				SMemFree(*(_DWORD *)((char *)v12 + v13), "C:\\Src\\Diablo\\DiabloUI\\bn_prof.cpp", 250, 0);
				SMemFree(*v12, "C:\\Src\\Diablo\\DiabloUI\\bn_prof.cpp", 251, 0);
				--v12;
				if ( !--v14 )
					break;
				v13 = v18 - v22;
			}
		}
		SMemFree(v18, "C:\\Src\\Diablo\\DiabloUI\\bn_prof.cpp", 253, 0);
		SMemFree(v22, "C:\\Src\\Diablo\\DiabloUI\\bn_prof.cpp", 254, 0);
	}
} */
// 10010340: using guessed type int __stdcall SMemFree(_DWORD, _DWORD, _DWORD, _DWORD);
// 10010364: using guessed type int __stdcall SMemAlloc(_DWORD, _DWORD, _DWORD, _DWORD);
// 1001F380: using guessed type int dword_1001F380[];
// 10029408: using guessed type int dword_10029408;
// 10029418: using guessed type int dword_10029418;
// 1002941C: using guessed type int dword_1002941C;
// 10029430: using guessed type int (__stdcall *dword_10029430)(_DWORD, _DWORD, _DWORD, _DWORD);

// ref: 0x100018CE
void __fastcall bn_prof_100018CE(int a1, int a2) { return; }
/* {
	int v2; // esi
	LRESULT v3; // eax
	WPARAM v4; // edi
	char *v5; // ebx

	v2 = a2;
	if ( *(_DWORD *)(a2 + 24) && *(_DWORD *)a2 == 5 )
	{
		v3 = SendMessageA(*(HWND *)(a2 + 20), 0xEu, 0, 0);
		v4 = v3 + 1;
		if ( v3 != 0 )
		{
			v5 = (char *)SMemAlloc(v3 + 1, "C:\\Src\\Diablo\\DiabloUI\\bn_prof.cpp", 362, 0);
			SendMessageA(*(HWND *)(v2 + 20), 0xDu, v4, (LPARAM)v5);
			bn_prof_10001938(*(HDC *)(v2 + 24), (_DWORD *)(v2 + 28), v5, 0, 0);
			SMemFree(v5, "C:\\Src\\Diablo\\DiabloUI\\bn_prof.cpp", 367, 0);
		}
	}
} */
// 10010340: using guessed type int __stdcall SMemFree(_DWORD, _DWORD, _DWORD, _DWORD);
// 10010364: using guessed type int __stdcall SMemAlloc(_DWORD, _DWORD, _DWORD, _DWORD);

// ref: 0x10001938
int __fastcall bn_prof_10001938(HDC a1, _DWORD *a2, char *a3, int a4, int a5) { return 0; }
/* {
	int result; // eax
	char *v6; // edi
	char *v7; // eax
	char *v8; // esi
	char v9; // bl
	char *v10; // eax
	RECT rc; // [esp+Ch] [ebp-14h]
	_DWORD *v12; // [esp+1Ch] [ebp-4h]
	char *v13; // [esp+28h] [ebp+8h]

	result = (int)bn_prof_10002410(a1, a2);
	v12 = (_DWORD *)result;
	if ( result )
	{
		v6 = a3;
		if ( a3 )
		{
			v13 = (char *)(a4 != 0 ? (unsigned int)&rc : 0);
			while ( 1 )
			{
				v7 = strstr(v6, "http://");
				v8 = v7;
				v9 = 0;
				if ( v7 )
				{
					v9 = *v7;
					*v7 = 0;
				}
				if ( !bn_prof_10002456((int)v12, v6, 1, v13) || !v8 )
					break;
				*v8 = v9;
				v10 = strpbrk(v8, " \n\r\t");
				v6 = v10;
				if ( v10 )
				{
					v9 = *v10;
					*v10 = 0;
				}
				if ( !bn_prof_10002456((int)v12, v8, 2, v13) )
					break;
				if ( a4 && PtInRect(&rc, *(POINT *)a4) )
				{
					if ( a5 )
						*(_DWORD *)a5 = v8;
					return 1;
				}
				if ( !v6 )
					break;
				*v6 = v9;
			}
		}
		bn_prof_100026C4(v12);
		result = a4 == 0;
	}
	return result;
} */

// ref: 0x10001A10
int __fastcall bn_prof_10001A10(HWND a1, HWND a2) { return 0; }
/* {
	HWND v2; // esi
	int result; // eax
	WPARAM v4; // esi
	HWND v5; // eax
	HDC v6; // edi
	HWND v7; // eax
	struct tagRECT v8; // [esp+Ch] [ebp-3Ch]
	struct tagRECT Rect; // [esp+1Ch] [ebp-2Ch]
	struct tagPOINT Point; // [esp+2Ch] [ebp-1Ch]
	HWND v11; // [esp+34h] [ebp-14h]
	int v12; // [esp+38h] [ebp-10h]
	int v13; // [esp+3Ch] [ebp-Ch]
	HWND hWnd; // [esp+40h] [ebp-8h]
	char *v15; // [esp+44h] [ebp-4h]

	v2 = a2;
	hWnd = a2;
	v11 = a1;
	result = GetCursorPos(&Point);
	if ( result )
	{
		result = GetWindowRect(v2, &Rect);
		if ( result )
		{
			result = GetClientRect(v2, &v8);
			if ( result )
			{
				Point.x -= Rect.left;
				Point.y -= Rect.top;
				result = SendMessageA(v2, 0xEu, 0, 0);
				v4 = result + 1;
				if ( result + 1 > 1 )
				{
					v15 = (char *)SMemAlloc(result + 1, "C:\\Src\\Diablo\\DiabloUI\\bn_prof.cpp", 438, 0);
					SendMessageA(hWnd, 0xDu, v4, (LPARAM)v15);
					v5 = GetDesktopWindow();
					v6 = GetDC(v5);
					hWnd = (HWND)CreateCompatibleDC(v6);
					v13 = bn_prof_10001938((HDC)hWnd, &v8, v15, (int)&Point, (int)&v12);
					DeleteDC((HDC)hWnd);
					v7 = GetDesktopWindow();
					ReleaseDC(v7, v6);
					if ( v13 )
						bn_prof_10001B0A(v11, (const CHAR *)v12);
					result = SMemFree(v15, "C:\\Src\\Diablo\\DiabloUI\\bn_prof.cpp", 450, 0);
				}
			}
		}
	}
	return result;
} */
// 10010340: using guessed type int __stdcall SMemFree(_DWORD, _DWORD, _DWORD, _DWORD);
// 10010364: using guessed type int __stdcall SMemAlloc(_DWORD, _DWORD, _DWORD, _DWORD);

// ref: 0x10001B0A
HINSTANCE __fastcall bn_prof_10001B0A(HWND a1, const CHAR *a2) { return 0; }
/* {
	const CHAR *v2; // ebp
	HWND v3; // eax
	HWND v4; // eax
	HINSTANCE result; // eax
	FILE *v6; // eax
	HWND v7; // eax
	HWND v8; // eax
	HWND v9; // eax
	HWND hWnd; // [esp+10h] [ebp-348h]
	CHAR Caption; // [esp+14h] [ebp-344h]
	CHAR Result; // [esp+54h] [ebp-304h]
	CHAR Buffer; // [esp+158h] [ebp-200h]

	v2 = a2;
	hWnd = a1;
	v3 = GetDesktopWindow();
	SetForegroundWindow(v3);
	v4 = (HWND)SDrawGetFrameWindow();
	result = ShellExecuteA(v4, "open", v2, 0, 0, 1);
	if ( (unsigned int)result <= 0x20 )
	{
		v6 = fopen("battle.htm", "wb");
		if ( v6 )
			fclose(v6);
		if ( (unsigned int)FindExecutableA("battle.htm", 0, &Result) <= 0x20 )
		{
			v7 = (HWND)SDrawGetFrameWindow();
			if ( (unsigned int)ShellExecuteA(v7, "open", &Result, v2, 0, 1) <= 0x20 )
			{
				v8 = (HWND)SDrawGetFrameWindow();
				SetActiveWindow(v8);
				v9 = (HWND)SDrawGetFrameWindow();
				ShowWindow(v9, 0);
				LoadStringA(hInstance, 0x50u, &Buffer, 512);
				LoadStringA(hInstance, 0x51u, &Caption, 64);
				UiMessageBoxCallback(hWnd, &Buffer, &Caption, 0x30u);
			}
		}
		result = (HINSTANCE)DeleteFileA("battle.htm");
	}
	return result;
} */
// 10010382: using guessed type _DWORD __stdcall SDrawGetFrameWindow();

// ref: 0x10001C0E
HWND UNKCALL bn_prof_10001C0E(HWND hWnd) { return 0; }
/* {
	HWND v1; // esi
	LPARAM v2; // ST10_4
	HWND v3; // eax
	HWND v4; // eax
	HWND v5; // eax
	HWND v6; // eax

	v1 = hWnd;
	bn_prof_10001CF3(hWnd);
	ho = bn_prof_10001D81(v1, 2, (int)&unk_1001F3B8);
	dword_10029424 = bn_prof_10001D81(v1, 1, (int)&unk_1001F3D0);
	v2 = lParam;
	v3 = GetDlgItem(v1, 1126);
	SendMessageA(v3, 0xCu, 0, v2);
	bn_prof_10001CB9(
		(_DWORD *)dword_1002941C,
		dword_10029418,
		(void (__fastcall *)(_BYTE *, _DWORD, int))bn_prof_10001ED0,
		0);
	bn_prof_10001E34(v1);
	if ( dword_10029430 )
	{
		v4 = GetDlgItem(v1, 1128);
	}
	else
	{
		v5 = GetDlgItem(v1, 1134);
		bn_prof_100022A2(v5);
		v6 = GetDlgItem(v1, 1);
		EnableWindow(v6, 0);
		v4 = GetDlgItem(v1, 2);
	}
	return SetFocus(v4);
} */
// 10029418: using guessed type int dword_10029418;
// 1002941C: using guessed type int dword_1002941C;
// 10029430: using guessed type int (__stdcall *dword_10029430)(_DWORD, _DWORD, _DWORD, _DWORD);

// ref: 0x10001CB9
void __fastcall bn_prof_10001CB9(_DWORD *a1, int a2, void (__fastcall *a3)(_BYTE *, _DWORD, int), int a4) { return; }
/* {
	_BYTE *v4; // eax
	_DWORD *v5; // esi
	int v6; // edi

	if ( a1 )
	{
		if ( a2 )
		{
			if ( a3 )
			{
				v4 = (_BYTE *)*a1;
				if ( *(_BYTE *)*a1 )
				{
					v5 = a1;
					v6 = a2 - (_DWORD)a1;
					do
					{
						a3(v4, *(_DWORD *)((char *)v5 + v6), a4);
						++v5;
						v4 = (_BYTE *)*v5;
					}
					while ( *(_BYTE *)*v5 );
				}
			}
		}
	}
} */

// ref: 0x10001CF3
int UNKCALL bn_prof_10001CF3(HWND hWnd) { return 0; }
/* {
	HWND v1; // edi
	const char *v2; // eax
	int v3; // ST10_4
	HWND v4; // eax
	int v6; // [esp+8h] [ebp-14h]
	int v7; // [esp+Ch] [ebp-10h]
	int v8; // [esp+10h] [ebp-Ch]
	char v9; // [esp+14h] [ebp-8h]

	v8 = 0;
	v1 = hWnd;
	v6 = 1;
	v7 = 2;
	v2 = "ui_art\\bnprofile.pcx";
	if ( !dword_10029430 )
		v2 = "ui_art\\bnstaticprofile.pcx";
	v3 = (int)v2;
	v4 = GetParent(hWnd);
	local_10007944((int)v1, (int)v4, "Popup", -1, 1, v3, &dword_10029410, &v9, 1);
	local_10007944(0, 0, "Button", -1, 1, (int)"ui_art\\but_xsm.pcx", &dword_10029428, &v9, 1);
	return SDlgSetControlBitmaps(v1, &v6, 0, dword_10029428, &v9, 1, -1);
} */
// 10010388: using guessed type int __stdcall SDlgSetControlBitmaps(_DWORD, _DWORD, _DWORD, _DWORD, _DWORD, _DWORD, _DWORD);
// 10029410: using guessed type int dword_10029410;
// 10029428: using guessed type int dword_10029428;
// 10029430: using guessed type int (__stdcall *dword_10029430)(_DWORD, _DWORD, _DWORD, _DWORD);

// ref: 0x10001D81
HFONT __fastcall bn_prof_10001D81(HWND hWnd, int a2, int a3) { return 0; }
/* {
	int v3; // esi
	void *v4; // ebx
	int v5; // esi
	int v6; // eax
	const char *v7; // ST10_4
	HFONT v8; // ebx
	int *v10; // esi
	int pv; // [esp+8h] [ebp-40h]
	int v12; // [esp+18h] [ebp-30h]
	char v13; // [esp+24h] [ebp-24h]
	HWND hDlg; // [esp+44h] [ebp-4h]

	v3 = a2;
	hDlg = hWnd;
	v4 = (void *)SendMessageA(hWnd, 0x31u, 0, 0);
	if ( !v4 )
		return 0;
	memset(&pv, 0, 0x3Cu);
	if ( !GetObjectA(v4, 60, &pv) )
		return 0;
	v5 = 3 * v3;
	v6 = MulDiv(dword_10022260[v5], 96, 72);
	v7 = (&off_10022264)[v5];
	pv = -v6;
	v12 = dword_10022268[v5];
	strcpy(&v13, v7);
	v8 = CreateFontIndirectA((const LOGFONTA *)&pv);
	if ( !v8 )
		return 0;
	v10 = (int *)a3;
	if ( a3 )
	{
		while ( *v10 )
		{
			SendDlgItemMessageA(hDlg, *v10, 0x30u, (WPARAM)v8, 0);
			++v10;
		}
	}
	return v8;
} */
// 10022260: using guessed type int dword_10022260[];
// 10022264: using guessed type char *off_10022264;
// 10022268: using guessed type int dword_10022268[];

// ref: 0x10001E34
void UNKCALL bn_prof_10001E34(void *arg) { return; }
/* {
	bn_prof_10001CB9(
		(_DWORD *)dword_1002941C,
		dword_10029418,
		(void (__fastcall *)(_BYTE *, _DWORD, int))bn_prof_10001E4C,
		(int)arg);
} */
// 10029418: using guessed type int dword_10029418;
// 1002941C: using guessed type int dword_1002941C;

// ref: 0x10001E4C
void __fastcall bn_prof_10001E4C(char *a1, LPARAM lParam, HWND hDlg) { return; }
/* {
	int v3; // esi
	bool v4; // zf
	LPARAM v5; // ebp
	char *v6; // ebx
	const char **v7; // edi
	int v8; // esi
	HWND v9; // ebx

	v3 = 0;
	v4 = dword_10022258 == 0;
	v5 = lParam;
	v6 = a1;
	if ( dword_10022258 > 0 )
	{
		v7 = (const char **)&off_1001F378;
		do
		{
			if ( !_strcmpi(v6, *v7) )
				break;
			++v3;
			v7 += 4;
		}
		while ( v3 < dword_10022258 );
		v4 = v3 == dword_10022258;
	}
	if ( !v4 )
	{
		v8 = 4 * v3;
		if ( !(byte_1001F37C[v8 * 4] & 4) )
		{
			v9 = GetDlgItem(hDlg, dword_1001F380[v8]);
			SendMessageA(v9, 0xCu, 0, v5);
			if ( dword_10029430 )
				SendMessageA(v9, 0xC5u, dword_1001F384[v8], 0);
		}
	}
} */
// 1001F378: using guessed type char *off_1001F378;
// 1001F380: using guessed type int dword_1001F380[];
// 1001F384: using guessed type int dword_1001F384[];
// 10022258: using guessed type int dword_10022258;
// 10029430: using guessed type int (__stdcall *dword_10029430)(_DWORD, _DWORD, _DWORD, _DWORD);

// ref: 0x10001ED0
void __fastcall bn_prof_10001ED0(char *a1, _BYTE *a2, int a3) { return; }
/* {
	int v3; // esi
	bool v4; // zf
	_BYTE *v5; // edi
	char *v6; // ebp
	const char **v7; // ebx

	v3 = 0;
	v4 = dword_10022258 == 0;
	v5 = a2;
	v6 = a1;
	if ( dword_10022258 > 0 )
	{
		v7 = (const char **)&off_1001F378;
		do
		{
			if ( !_strcmpi(v6, *v7) )
				break;
			++v3;
			v7 += 4;
		}
		while ( v3 < dword_10022258 );
		v4 = v3 == dword_10022258;
	}
	if ( !v4 )
	{
		if ( *v5 )
			dword_10029438[v3] = (int)v5;
		else
			dword_10029438[v3] = (int)"0";
	}
} */
// 1001F378: using guessed type char *off_1001F378;
// 10022258: using guessed type int dword_10022258;

// ref: 0x10001F29
void *bn_prof_10001F29() { return 0; }
/* {
	int result; // eax

	bn_prof_100023D8();
	bn_prof_10001F84();
	if ( ho )
		DeleteObject(ho);
	ho = 0;
	if ( dword_10029424 )
		DeleteObject(dword_10029424);
	result = dword_10029434;
	dword_10029424 = 0;
	if ( dword_10029434 )
		result = SMemFree(dword_10029434, "C:\\Src\\Diablo\\DiabloUI\\bn_prof.cpp", 509, 0);
	dword_10029434 = 0;
	return result;
} */
// 10010340: using guessed type int __stdcall SMemFree(_DWORD, _DWORD, _DWORD, _DWORD);
// 10029434: using guessed type int dword_10029434;

// ref: 0x10001F84
BYTE *bn_prof_10001F84() { return 0; }
/* {
	int result; // eax

	if ( dword_10029410 )
	{
		SMemFree(dword_10029410, "C:\\Src\\Diablo\\DiabloUI\\bn_prof.cpp", 129, 0);
		dword_10029410 = 0;
	}
	result = dword_10029428;
	if ( dword_10029428 )
	{
		result = SMemFree(dword_10029428, "C:\\Src\\Diablo\\DiabloUI\\bn_prof.cpp", 133, 0);
		dword_10029428 = 0;
	}
	return result;
} */
// 10010340: using guessed type int __stdcall SMemFree(_DWORD, _DWORD, _DWORD, _DWORD);
// 10029410: using guessed type int dword_10029410;
// 10029428: using guessed type int dword_10029428;

// ref: 0x10001FC8
void __cdecl UiProfileDraw() { return; }
//int __stdcall UiProfileDraw(int a1, int a2, int a3, int a4, HGDIOBJ ho, int a6, int a7, int a8, int a9, int a10, int a11) { return 0; }
/* {
	int v11; // eax
	HFONT v12; // eax
	int v14; // eax
	CHAR *v15; // edi
	int v16; // esi
	CHAR v17; // al
	int v18; // ecx
	int v19; // eax
	int v20; // edx
	CHAR Buffer[256]; // [esp+Ch] [ebp-150h]
	int v22; // [esp+10Ch] [ebp-50h]
	int v23; // [esp+110h] [ebp-4Ch]
	int v24; // [esp+114h] [ebp-48h]
	int v25; // [esp+118h] [ebp-44h]
	int v26; // [esp+11Ch] [ebp-40h]
	int v27; // [esp+120h] [ebp-3Ch]
	int v28; // [esp+124h] [ebp-38h]
	int v29; // [esp+128h] [ebp-34h]
	int v30; // [esp+12Ch] [ebp-30h]
	int v31; // [esp+130h] [ebp-2Ch]
	int v32; // [esp+134h] [ebp-28h]
	int v33; // [esp+138h] [ebp-24h]
	int v34; // [esp+13Ch] [ebp-20h]
	int v35; // [esp+140h] [ebp-1Ch]
	int v36; // [esp+144h] [ebp-18h]
	int v37; // [esp+148h] [ebp-14h]
	int v38; // [esp+14Ch] [ebp-10h]
	int v39; // [esp+150h] [ebp-Ch]
	int v40; // [esp+154h] [ebp-8h]
	int v41; // [esp+158h] [ebp-4h]
	HFONT hoa; // [esp+174h] [ebp+18h]
	int hob; // [esp+174h] [ebp+18h]

	if ( ho )
		return 0;
	if ( !a9 )
		return 0;
	if ( !a10 )
		return 0;
	if ( !a11 )
		return 0;
	v24 = dword_1002940C;
	v34 = dword_1002940C;
	v30 = 0;
	v31 = 0;
	v32 = a10;
	v33 = a11;
	v36 = a10;
	v37 = a11;
	v22 = 0;
	v23 = 0;
	v25 = dword_10029414;
	v35 = dword_10029414;
	if ( !SBltROP3Clipped(a9, &v30, &v36, a10, dword_1002942C, &v22, &v34, dword_1002940C, 0, 13369376) )
		return 0;
	if ( !LoadStringA(hInstance, 0x4Fu, Buffer, 255) )
		return 0;
	v11 = MulDiv(12, 96, 72);
	v12 = CreateFontA(-v11, 0, 0, 0, 400, 0, 0, 0, 0, 0, 0, 0, 0x20u, "Arial");
	hoa = v12;
	if ( !v12 )
		return 0;
	v41 = 0;
	if ( !SGdiImportFont(v12, &v41) )
		return 0;
	DeleteObject(hoa);
	if ( !v41 || !SGdiSelectObject(v41) )
		return 0;
	v26 = 0;
	v27 = 0;
	v28 = a10;
	v29 = a11;
	SGdiSetPitch(a10);
	v14 = SStrLen(Buffer);
	v15 = Buffer;
	hob = v14 + 1;
	v40 = 8;
	Buffer[v14 + 1] = 0;
	v16 = v14 + 1;
	do
	{
		if ( v15[v16 - 1] )
		{
			v17 = v15[v16 - 1];
			do
			{
				if ( v17 == 32 )
					break;
				if ( v17 == 10 )
					break;
				if ( v17 == 9 )
					break;
				if ( v16 <= 1 )
					break;
				v17 = v15[v16-- - 2];
			}
			while ( v17 );
		}
		SGdiGetTextExtent(v15, v16, &v38);
		if ( v38 >= a10 - 8 )
		{
			v20 = a10 * hob % v38;
			if ( --v16 >= a10 * hob / v38 )
				v16 = a10 * hob / v38;
		}
		else
		{
			v19 = v16 - 1;
			if ( v15[v16 - 1] )
				v19 = v16;
			SGdiExtTextOut(a9, 8, v40, &v26, 16777471, 1, 0, v15, v19);
			if ( v15[v16] == 32 )
				++v16;
			v15 += v16;
			v40 += v39;
			hob -= v16;
			v16 = hob;
		}
	}
	while ( v16 && hob && v40 < a11 - 8 );
	SGdiDeleteObject(v18, v20, v41);
	return 1;
} */
// 1001038E: using guessed type int __fastcall SGdiDeleteObject(_DWORD, _DWORD, _DWORD);
// 10010394: using guessed type int __stdcall SGdiExtTextOut(_DWORD, _DWORD, _DWORD, _DWORD, _DWORD, _DWORD, _DWORD, _DWORD, _DWORD);
// 1001039A: using guessed type int __stdcall SGdiGetTextExtent(_DWORD, _DWORD, _DWORD);
// 100103A0: using guessed type int __stdcall SStrLen(_DWORD);
// 100103A6: using guessed type int __stdcall SGdiSetPitch(_DWORD);
// 100103AC: using guessed type int __stdcall SGdiSelectObject(_DWORD);
// 100103B2: using guessed type int __stdcall SGdiImportFont(_DWORD, _DWORD);
// 100103B8: using guessed type int __stdcall SBltROP3Clipped(_DWORD, _DWORD, _DWORD, _DWORD, _DWORD, _DWORD, _DWORD, _DWORD, _DWORD, _DWORD);
// 1002940C: using guessed type int dword_1002940C;
// 10029414: using guessed type int dword_10029414;
// 1002942C: using guessed type int dword_1002942C;
// 10001FC8: using guessed type CHAR Buffer[256];

// ref: 0x100021C4
int bn_prof_100021C4() { return 0; }
/* {
	int v0; // edi
	int v2; // [esp+4h] [ebp-8h]
	int v3; // [esp+8h] [ebp-4h]

	v3 = 0;
	v2 = 0;
	if ( !SBmpLoadImage("ui_Art\\profilebkg.pcx", 0, 0, 0, &v3, &v2, 0) || !v3 || !v2 )
		return 0;
	v0 = v3 * v2;
	dword_1002942C = SMemAlloc(v3 * v2, "C:\\Src\\Diablo\\DiabloUI\\bn_prof.cpp", 760, 0);
	dword_1002940C = v3;
	dword_10029414 = v2;
	return SBmpLoadImage("ui_Art\\profilebkg.pcx", 0, dword_1002942C, v0, &v3, &v2, 0);
} */
// 10010364: using guessed type int __stdcall SMemAlloc(_DWORD, _DWORD, _DWORD, _DWORD);
// 100103BE: using guessed type int __stdcall SBmpLoadImage(_DWORD, _DWORD, _DWORD, _DWORD, _DWORD, _DWORD, _DWORD);
// 1002940C: using guessed type int dword_1002940C;
// 10029414: using guessed type int dword_10029414;
// 1002942C: using guessed type int dword_1002942C;

// ref: 0x10002247
void *bn_prof_10002247() { return 0; }
/* {
	int result; // eax

	result = dword_1002942C;
	if ( dword_1002942C )
	{
		result = SMemFree(dword_1002942C, "C:\\Src\\Diablo\\DiabloUI\\bn_prof.cpp", 776, 0);
		dword_1002942C = 0;
		dword_1002940C = 0;
		dword_10029414 = 0;
	}
	return result;
} */
// 10010340: using guessed type int __stdcall SMemFree(_DWORD, _DWORD, _DWORD, _DWORD);
// 1002940C: using guessed type int dword_1002940C;
// 10029414: using guessed type int dword_10029414;
// 1002942C: using guessed type int dword_1002942C;

// ref: 0x10002278
int j_bn_prof_10002282() { return 0; }
/* {
	bn_prof_10002282();
	return atexit(bn_prof_10002298);
} */

// ref: 0x10002282
_DWORD *bn_prof_10002282() { return 0; }
/* {
	_DWORD *result; // eax
	_DWORD *v1; // edx

	result = dword_10029460;
	v1 = &dword_10029460[1];
	*v1 = 0;
	v1[1] = 0;
	*v1 = v1;
	dword_10029460[0] = 0;
	dword_10029460[2] = ~(unsigned int)&dword_10029460[1];
	return result;
} */

// ref: 0x10002298
void __cdecl bn_prof_10002298() { return; }
/* {
	bn_prof_100026F0(dword_10029460);
} */

// ref: 0x100022A2
int UNKCALL bn_prof_100022A2(HWND hWnd) { return 0; }
/* {
	HFONT v2; // eax
	HFONT v3; // eax
	char pv; // [esp+4h] [ebp-40h]
	char v5; // [esp+19h] [ebp-2Bh]
	HANDLE h; // [esp+40h] [ebp-4h]

	if ( !hWnd )
		return 0;
	h = (HANDLE)SendMessageA(hWnd, 0x31u, 0, 0);
	if ( !h )
		return 0;
	memset(&pv, 0, 0x3Cu);
	if ( GetObjectA(h, 60, &pv) )
	{
		v2 = CreateFontIndirectA((const LOGFONTA *)&pv);
		if ( v2 )
		{
			dword_10029450 = v2;
			dword_10029458 = bn_prof_10002353(v2);
		}
	}
	memset(&pv, 0, 0x3Cu);
	if ( GetObjectA(h, 60, &pv) )
	{
		v5 = 1;
		v3 = CreateFontIndirectA((const LOGFONTA *)&pv);
		if ( v3 )
		{
			::h = v3;
			dword_1002946C = bn_prof_10002353(v3);
		}
	}
	return 1;
} */
// 10029458: using guessed type int dword_10029458;
// 1002946C: using guessed type int dword_1002946C;

// ref: 0x10002353
int UNKCALL bn_prof_10002353(HGDIOBJ h) { return 0; }
/* {
	HGDIOBJ v1; // ebx
	HWND v2; // eax
	HDC v3; // ebp
	HDC v4; // esi
	HWND v5; // eax
	int v7; // [esp+10h] [ebp-10h]
	HGDIOBJ ha; // [esp+14h] [ebp-Ch]
	struct tagSIZE psizl; // [esp+18h] [ebp-8h]

	v1 = h;
	v2 = GetDesktopWindow();
	v3 = GetDC(v2);
	v4 = CreateCompatibleDC(v3);
	v7 = 0;
	ha = SelectObject(v4, v1);
	if ( GetTextExtentPoint32A(v4, "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz", 52, &psizl) )
		v7 = (psizl.cx / 26 + 1) / 2;
	SelectObject(v4, ha);
	DeleteDC(v4);
	v5 = GetDesktopWindow();
	ReleaseDC(v5, v3);
	return v7;
} */

// ref: 0x100023D8
HGDIOBJ bn_prof_100023D8() { return 0; }
/* {
	HGDIOBJ result; // eax
	int v1; // esi

	if ( dword_10029450 )
	{
		DeleteObject(dword_10029450);
		dword_10029450 = 0;
	}
	result = h;
	if ( h )
	{
		result = (HGDIOBJ)DeleteObject(h);
		h = 0;
	}
	while ( 1 )
	{
		v1 = dword_10029460[2];
		if ( v1 <= 0 )
			break;
		bn_prof_100027D8((_DWORD *)dword_10029460[2]);
		result = (HGDIOBJ)SMemFree(v1, ".?AU_DRAWTEXT@@", -2, 0);
	}
	return result;
} */
// 10010340: using guessed type int __stdcall SMemFree(_DWORD, _DWORD, _DWORD, _DWORD);

// ref: 0x10002410
_DWORD *__fastcall bn_prof_10002410(HDC hdc, _DWORD *a2) { return 0; }
/* {
	HDC v2; // ebp
	_DWORD *v3; // esi
	_DWORD *v4; // eax
	_DWORD *v5; // ebx
	_DWORD *v6; // esi

	v2 = hdc;
	v3 = a2;
	v4 = bn_prof_10002782(dword_10029460, 2, 0, 0);
	v5 = v4;
	v4[2] = v2;
	v4[3] = *v3;
	++v3;
	v4[4] = *v3;
	++v3;
	v4[5] = *v3;
	v4[6] = v3[1];
	v4[7] = 0;
	v4[8] = 0;
	v6 = v4 + 9;
	GetTextMetricsA(v2, (LPTEXTMETRICA)(v4 + 9));
	v5[23] = *v6 + v5[13];
	return v5;
} */

// ref: 0x10002456
signed int __fastcall bn_prof_10002456(int a1, const CHAR *a2, char a3, _DWORD *a4) { return 0; }
/* {
	int v4; // esi
	HGDIOBJ v6; // edi
	int v7; // eax
	int v8; // ecx
	int v9; // edi
	int v10; // eax
	int v11; // eax
	int v12; // eax
	LPCSTR v13; // ebx
	LONG v14; // ecx
	int v15; // eax
	int v16; // eax
	int v17; // eax
	int v18; // ecx
	const CHAR *v19; // eax
	int v20; // eax
	int v21; // eax
	COLORREF v22; // [esp-Ch] [ebp-2Ch]
	struct tagSIZE Size; // [esp+4h] [ebp-1Ch]
	COLORREF color; // [esp+Ch] [ebp-14h]
	HGDIOBJ h; // [esp+10h] [ebp-10h]
	int cchString; // [esp+14h] [ebp-Ch]
	LPCSTR lpszString; // [esp+18h] [ebp-8h]
	int nFit; // [esp+1Ch] [ebp-4h]
	int v29; // [esp+28h] [ebp+8h]

	v4 = a1;
	lpszString = a2;
	if ( !a1 )
		return 0;
	cchString = strlen(a2);
	if ( a4 )
		memset(a4, 0, 0x10u);
	h = 0;
	if ( a3 & 2 )
	{
		v6 = ::h;
		v7 = dword_1002946C;
		v22 = 16711680;
	}
	else
	{
		v6 = dword_10029450;
		v7 = dword_10029458;
		v22 = 0xFFFFFF;
	}
	v29 = v7;
	color = SetTextColor(*(HDC *)(v4 + 8), v22);
	if ( v6 )
		h = SelectObject(*(HDC *)(v4 + 8), v6);
	if ( a4 )
	{
		*a4 = *(_DWORD *)(v4 + 28);
		a4[1] = *(_DWORD *)(v4 + 32);
	}
	do
	{
		while ( cchString > 0 && *(_WORD *)lpszString == 2573 )
		{
			v8 = *(_DWORD *)(v4 + 92);
			cchString -= 2;
			*(_DWORD *)(v4 + 32) += v8;
			lpszString += 2;
			*(_DWORD *)(v4 + 28) = 0;
		}
		if ( !cchString )
			break;
		nFit = 0;
		GetTextExtentExPointA(
			*(HDC *)(v4 + 8),
			lpszString,
			cchString,
			*(_DWORD *)(v4 + 20) - *(_DWORD *)(v4 + 28) - *(_DWORD *)(v4 + 12) - v29 + 1,
			&nFit,
			0,
			&Size);
		v9 = nFit;
		if ( nFit )
		{
			if ( nFit < cchString )
			{
				if ( nFit > 0 )
				{
					do
					{
						if ( isspace(lpszString[v9]) )
							break;
						--v9;
					}
					while ( v9 > 0 );
					if ( v9 > 0 )
					{
LABEL_26:
						++v9;
						goto LABEL_27;
					}
				}
				if ( *(_DWORD *)(v4 + 28) > 0 )
				{
					if ( isspace(lpszString[v9]) )
						goto LABEL_26;
LABEL_27:
					nFit = v9;
				}
			}
			v11 = 0;
			if ( nFit > 0 )
			{
				while ( *(_WORD *)&lpszString[v11] != 2573 && lpszString[v11] != 9 )
				{
					if ( ++v11 >= nFit )
						goto LABEL_34;
				}
				nFit = v11;
			}
LABEL_34:
			if ( a4 )
			{
				v12 = *(_DWORD *)(v4 + 28);
				if ( v12 < *a4 )
					*a4 = v12;
			}
			else
			{
				ExtTextOutA(
					*(HDC *)(v4 + 8),
					*(_DWORD *)(v4 + 28),
					*(_DWORD *)(v4 + 32),
					4u,
					(const RECT *)(v4 + 12),
					lpszString,
					nFit,
					0);
			}
			v13 = lpszString;
			GetTextExtentPoint32A(*(HDC *)(v4 + 8), lpszString, nFit, &Size);
			v14 = Size.cx;
			if ( a4 )
			{
				v15 = Size.cx + *(_DWORD *)(v4 + 28);
				if ( v15 > a4[2] )
					a4[2] = v15;
				v16 = Size.cy + *(_DWORD *)(v4 + 32);
				if ( v16 > a4[3] )
					a4[3] = v16;
			}
			v17 = nFit;
			*(_DWORD *)(v4 + 28) += v14;
			v18 = *(_DWORD *)(v4 + 28);
			if ( v17 < cchString )
			{
				v19 = &v13[v17];
				if ( *v19 == 9 )
				{
					++nFit;
					*(_DWORD *)(v4 + 28) = 8 * v29 + v18 - v18 % (8 * v29);
				}
				else
				{
					if ( *(_WORD *)v19 == 2573 )
						nFit += 2;
					v20 = *(_DWORD *)(v4 + 92);
					*(_DWORD *)(v4 + 28) = 0;
					*(_DWORD *)(v4 + 32) += v20;
				}
			}
			cchString -= nFit;
			lpszString += nFit;
			continue;
		}
		v10 = *(_DWORD *)(v4 + 92);
		*(_DWORD *)(v4 + 28) &= nFit;
		*(_DWORD *)(v4 + 32) += v10;
	}
	while ( cchString > 0 );
	if ( *(_DWORD *)(v4 + 28) > *(_DWORD *)(v4 + 20) - *(_DWORD *)(v4 + 12) - v29 + 1 )
	{
		v21 = *(_DWORD *)(v4 + 92);
		*(_DWORD *)(v4 + 28) = 0;
		*(_DWORD *)(v4 + 32) += v21;
	}
	if ( h )
		SelectObject(*(HDC *)(v4 + 8), h);
	SetTextColor(*(HDC *)(v4 + 8), color);
	return 1;
} */
// 10029458: using guessed type int dword_10029458;
// 1002946C: using guessed type int dword_1002946C;

// ref: 0x100026B9
signed int bn_prof_100026B9() { return 0; }
/* {
	signed int result; // eax

	result = 2139095040;
	dword_10029454 = 2139095040;
	return result;
} */
// 10029454: using guessed type int dword_10029454;

// ref: 0x100026C4
signed int UNKCALL bn_prof_100026C4(_DWORD *arg) { return 0; }
/* {
	if ( !arg )
		return 0;
	bn_prof_10002749((char *)dword_10029460, arg);
	return 1;
} */

// ref: 0x100026F0
void UNKCALL bn_prof_100026F0(_DWORD *arg) { return; }
/* {
	_DWORD *v1; // esi

	v1 = arg;
	bn_prof_1000287D(arg);
	bn_prof_10002890(v1 + 1);
} */

// ref: 0x10002749
int UNKCALL bn_prof_10002749(char *arg, _DWORD *a2) { return 0; }
/* {
	int v2; // eax
	int v3; // eax
	int v4; // esi

	v2 = (int)a2;
	if ( !a2 )
		v2 = (int)(arg + 4);
	v3 = *(_DWORD *)(v2 + 4);
	if ( v3 > 0 )
		v4 = v3;
	else
		v4 = 0;
	bn_prof_100027D8(a2);
	SMemFree(a2, ".?AU_DRAWTEXT@@", -2, 0);
	return v4;
} */
// 10010340: using guessed type int __stdcall SMemFree(_DWORD, _DWORD, _DWORD, _DWORD);

// ref: 0x10002782
_DWORD *UNKCALL bn_prof_10002782(int *arg, int a2, int a3, int a4) { return 0; }
/* {
	int v4; // eax
	int *v5; // edi
	_DWORD *v6; // eax
	_DWORD *v7; // esi

	v4 = a4;
	LOBYTE(v4) = a4 | 8;
	v5 = arg;
	v6 = (_DWORD *)SMemAlloc(a3 + 96, ".?AU_DRAWTEXT@@", -2, v4);
	if ( v6 )
		v7 = bn_prof_100027CE(v6);
	else
		v7 = 0;
	if ( a2 )
		bn_prof_1000280C(v5, v7, a2, 0);
	return v7;
} */
// 10010364: using guessed type int __stdcall SMemAlloc(_DWORD, _DWORD, _DWORD, _DWORD);

// ref: 0x100027CE
_DWORD *UNKCALL bn_prof_100027CE(_DWORD *arg) { return 0; }
/* {
	_DWORD *result; // eax

	result = arg;
	*arg = 0;
	arg[1] = 0;
	return result;
} */

// ref: 0x100027D8
void UNKCALL bn_prof_100027D8(_DWORD *arg) { return; }
/* {
	_DWORD *v1; // ST00_4

	v1 = arg;
	bn_prof_10002890(arg);
	bn_prof_10002890(v1);
} */

// ref: 0x1000280C
_DWORD *UNKCALL bn_prof_1000280C(int *arg, _DWORD *a2, int a3, _DWORD *a4) { return 0; }
/* {
	int *v4; // edi
	_DWORD *v5; // esi
	_DWORD *result; // eax
	int v7; // ecx
	int v8; // edx
	int v9; // ecx
	int v10; // edx

	v4 = arg;
	v5 = a2;
	if ( !a2 )
		v5 = arg + 1;
	if ( *v5 )
		bn_prof_10002890(v5);
	result = a4;
	if ( !a4 )
		result = v4 + 1;
	if ( a3 == 1 )
	{
		*v5 = result;
		v5[1] = result[1];
		v8 = result[1];
		v9 = *v4;
		if ( v8 > 0 )
		{
			if ( v9 < 0 )
				v9 = (int)result - *(_DWORD *)(*result + 4);
			v10 = v9 + v8;
		}
		else
		{
			v10 = ~v8;
		}
		*(_DWORD *)v10 = v5;
		result[1] = a2;
	}
	else if ( a3 == 2 )
	{
		v7 = *result;
		*v5 = *result;
		v5[1] = *(_DWORD *)(v7 + 4);
		*(_DWORD *)(v7 + 4) = a2;
		*result = v5;
	}
	return result;
} */

// ref: 0x1000287D
void UNKCALL bn_prof_1000287D(_DWORD *arg) { return; }
/* {
	_DWORD *v1; // esi
	_DWORD *v2; // ecx

	v1 = arg;
	while ( 1 )
	{
		v2 = (_DWORD *)v1[2];
		if ( (signed int)v2 <= 0 )
			break;
		bn_prof_10002890(v2);
	}
} */

// ref: 0x10002890
void UNKCALL bn_prof_10002890(_DWORD *arg) { return; }
/* {
	int v1; // esi
	int v2; // edx
	int v3; // edx

	v1 = *arg;
	if ( *arg )
	{
		v2 = arg[1];
		if ( v2 > 0 )
			v3 = (int)arg + v2 - *(_DWORD *)(v1 + 4);
		else
			v3 = ~v2;
		*(_DWORD *)v3 = v1;
		*(_DWORD *)(*arg + 4) = arg[1];
		*arg = 0;
		arg[1] = 0;
	}
} */

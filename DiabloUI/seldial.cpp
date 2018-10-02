// ref: 0x1000B011
int UNKCALL SelDial_1000B011(char *arg) { return 0; }
/* {
	signed int v1; // edi
	int i; // edi
	char v4; // [esp+8h] [ebp-24h]
	char v5; // [esp+27h] [ebp-5h]
	char *v6; // [esp+28h] [ebp-4h]

	v6 = arg;
	v1 = 0;
	do
	{
		if ( SRegLoadString("Diablo\\Phone Book", off_10022F8C[v1], 1u, &v4, 0x20u) )
		{
			v5 = 0;
			if ( !strcmp(&v4, v6) )
				break;
		}
		++v1;
	}
	while ( v1 < 4 );
	if ( v1 == 4 )
		v1 = 3;
	for ( i = v1 - 1; i >= 0; --i )
	{
		v4 = 0;
		if ( SRegLoadString("Diablo\\Phone Book", off_10022F8C[i], 1u, &v4, 0x20u) )
		{
			v5 = 0;
			if ( strlen(&v4) )
				SRegSaveString("Diablo\\Phone Book", off_10022F90[i], 1u, &v4);
		}
	}
	return SRegSaveString("Diablo\\Phone Book", off_10022F8C[0], 1u, v6);
} */
// 10010484: using guessed type int __stdcall SRegSaveString(const char *, const char *, unsigned int, const char *);
// 1001048A: using guessed type int __stdcall SRegLoadString(const char *, const char *, unsigned int, char *, unsigned int);
// 10022F8C: using guessed type char *off_10022F8C[4];
// 10022F90: using guessed type char *off_10022F90[3];

// ref: 0x1000B0C4
signed int SelDial_1000B0C4() { return 0; }
/* {
	signed int result; // eax

	result = 2139095040;
	dword_1002A37C = 2139095040;
	return result;
} */
// 1002A37C: using guessed type int dword_1002A37C;

// ref: 0x1000B0CF
int __stdcall SelDial_1000B0CF(HWND hWnd, UINT Msg, WPARAM wParam, LPARAM lParam) { return 0; }
/* {
	HWND v4; // eax
	int v6; // edx
	HWND v7; // eax
	int savedregs; // [esp+Ch] [ebp+0h]

	if ( Msg == 2 )
	{
		SelDial_1000B29A(hWnd);
		return SDlgDefDialogProc(hWnd, Msg, wParam, lParam);
	}
	if ( Msg > 0x103 )
	{
		if ( Msg <= 0x105 )
		{
			v7 = (HWND)SDrawGetFrameWindow();
			SendMessageA(v7, Msg, wParam, lParam);
			return SDlgDefDialogProc(hWnd, Msg, wParam, lParam);
		}
		if ( Msg == 272 )
		{
			dword_1002A378 = (char *)lParam;
			SelDial_1000B483(hWnd, (int)&savedregs);
			return 0;
		}
		if ( Msg != 273 )
		{
			if ( Msg != 275 )
			{
				if ( Msg == 513 )
					SelDial_1000B614(hWnd, (unsigned short)lParam, (unsigned int)lParam >> 16);
				return SDlgDefDialogProc(hWnd, Msg, wParam, lParam);
			}
			if ( wParam == 1 )
			{
				v4 = GetFocus();
				Focus_100075DC(hWnd, v4);
			}
			else if ( wParam == 2 )
			{
				SelDial_1000B354(hWnd);
			}
			return 0;
		}
		if ( HIWORD(wParam) == 7 )
		{
			Focus_100075B7(hWnd, (HWND)lParam);
		}
		else if ( HIWORD(wParam) == 6 )
		{
			Focus_10007458((void *)lParam);
			Focus_100075DC(hWnd, (HWND)lParam);
			SelDial_1000B1FB(hWnd, (unsigned short)wParam);
		}
		else
		{
			v6 = 1;
			if ( wParam != 327681 )
			{
				if ( (_WORD)wParam != 2 )
					return SDlgDefDialogProc(hWnd, Msg, wParam, lParam);
				v6 = 2;
			}
			SelDial_1000B2D8((int)hWnd, v6);
		}
	}
	return SDlgDefDialogProc(hWnd, Msg, wParam, lParam);
} */
// 1001037C: using guessed type int __stdcall SDlgDefDialogProc(_DWORD, _DWORD, _DWORD, _DWORD);
// 10010382: using guessed type _DWORD __stdcall SDrawGetFrameWindow();

// ref: 0x1000B1FB
HWND __fastcall SelDial_1000B1FB(HWND hWnd, int a2) { return 0; }
/* {
	int v2; // edi
	HWND v3; // ebx
	HWND v4; // eax
	CHAR v6; // [esp+Ch] [ebp-140h]
	CHAR Buffer; // [esp+10Ch] [ebp-40h]

	v2 = a2;
	v3 = hWnd;
	LoadStringA(hInstance, 0x39u, &Buffer, 63);
	if ( v2 == 1117 )
	{
		if ( Modem_1000855D() )
			LoadStringA(hInstance, 0x43u, &v6, 255);
		else
			LoadStringA(hInstance, 0x36u, &v6, 255);
	}
	else if ( v2 == 1118 )
	{
		if ( Modem_1000855D() )
			LoadStringA(hInstance, 0x44u, &v6, 255);
		else
			LoadStringA(hInstance, 0x37u, &v6, 255);
	}
	else
	{
		LoadStringA(hInstance, 0x38u, &v6, 255);
	}
	v4 = GetParent(v3);
	return Modem_10008563(v4, &Buffer, (int)&v6);
} */

// ref: 0x1000B29A
HWND UNKCALL SelDial_1000B29A(HWND hDlg) { return 0; }
/* {
	HWND v1; // esi
	HWND v2; // eax

	v1 = hDlg;
	Doom_10006C53(hDlg, (int *)&unk_10022FB0);
	Doom_10006C53(v1, (int *)&unk_10022FA4);
	Doom_10006C53(v1, (int *)&unk_10022F9C);
	Focus_100076C3();
	v2 = GetParent(v1);
	return Modem_10008563(v2, 0, 0);
} */

// ref: 0x1000B2D8
int __fastcall SelDial_1000B2D8(int a1, int a2) { return 0; }
/* {
	int v2; // esi
	int v3; // edi
	HWND v4; // eax
	LONG v5; // eax

	v2 = a2;
	v3 = a1;
	if ( a2 == 1 || a2 == 2 )
		TitleSnd_1001031F();
	SDlgKillTimer(v3, 1);
	SDlgKillTimer(v3, 2);
	if ( v2 != 1 )
		return SDlgEndDialog(v3, v2);
	v4 = GetFocus();
	v5 = GetWindowLongA(v4, -12);
	if ( v5 == 1117 )
		return SDlgEndDialog(v3, 3);
	if ( v5 == 1118 )
		return SDlgEndDialog(v3, 5);
	if ( dword_1002A378 )
		strcpy(dword_1002A378, &byte_1002A380[32 * (v5 - 1119)]);
	return SDlgEndDialog(v3, 4);
} */
// 10010376: using guessed type int __stdcall SDlgEndDialog(_DWORD, _DWORD);
// 10010418: using guessed type int __stdcall SDlgKillTimer(_DWORD, _DWORD);

// ref: 0x1000B354
HWND UNKCALL SelDial_1000B354(HWND hDlg) { return 0; }
/* {
	HWND v1; // edi
	HWND result; // eax
	HWND v3; // esi
	HWND v4; // eax

	v1 = hDlg;
	result = GetDlgItem(hDlg, 1118);
	v3 = result;
	if ( result )
	{
		if ( Modem_10008606() )
		{
			SelDial_1000B3D8(v1);
			EnableWindow(v3, 1);
			result = (HWND)ShowWindow(v3, 1);
		}
		else if ( SErrGetLastError() == 1222 )
		{
			result = (HWND)SelDial_1000B2D8((int)v1, 1222);
		}
		else
		{
			if ( GetFocus() == v3 )
			{
				v4 = GetDlgItem(v1, 1117);
				SetFocus(v4);
			}
			SelDial_1000B44C(v1);
			EnableWindow(v3, 0);
			result = (HWND)ShowWindow(v3, 0);
		}
	}
	return result;
} */
// 10010406: using guessed type _DWORD __stdcall SErrGetLastError();

// ref: 0x1000B3D8
HWND UNKCALL SelDial_1000B3D8(HWND hDlg) { return 0; }
/* {
	HWND v1; // esi
	HWND result; // eax
	HWND v3; // edi
	char *v4; // eax
	int v5; // ebx
	int v6; // eax
	CHAR Buffer; // [esp+8h] [ebp-40h]

	v1 = hDlg;
	result = GetDlgItem(hDlg, 1118);
	v3 = result;
	if ( result )
	{
		v4 = Modem_1000863D();
		v5 = Modem_1000865F(v4);
		if ( v5 <= Modem_10008659() )
			LoadStringA(hInstance, 0x4Au, &Buffer, 63);
		else
			LoadStringA(hInstance, 0x4Bu, &Buffer, 63);
		v6 = GetWindowLongA(v3, -21);
		local_10007FA4(v6, &Buffer);
		result = (HWND)Doom_1000680A(v1, (int *)&unk_10022FB0, 0, 1);
	}
	return result;
} */

// ref: 0x1000B44C
HWND UNKCALL SelDial_1000B44C(HWND hDlg) { return 0; }
/* {
	HWND v1; // esi
	HWND result; // eax
	int v3; // eax

	v1 = hDlg;
	result = GetDlgItem(hDlg, 1118);
	if ( result )
	{
		v3 = GetWindowLongA(result, -21);
		local_10007FA4(v3, 0);
		result = (HWND)Doom_1000680A(v1, (int *)&unk_10022FB0, 0, 1);
	}
	return result;
} */

// ref: 0x1000B483
HWND USERCALL SelDial_1000B483(HWND hWnd, int a2) { return 0; }
/* {
	HWND v2; // esi
	HWND v3; // eax
	LONG v4; // eax
	HWND result; // eax
	char *v6; // eax
	int v7; // ebx
	HWND v8; // eax
	HWND v9; // edi
	int v10; // esi
	const char *v11; // edx
	int v12; // [esp-64h] [ebp-6Ch]
	int v13; // [esp-24h] [ebp-2Ch]
	HWND v14; // [esp-4h] [ebp-Ch]
	const char *v15; // [esp+0h] [ebp-8h]
	int v16; // [esp+4h] [ebp-4h]

	v2 = hWnd;
	v3 = GetParent(hWnd);
	v4 = GetWindowLongA(v3, -21);
	SetWindowLongA(v2, -21, v4);
	Focus_10007719("ui_art\\focus16.pcx");
	SDlgSetTimer(v2, 1, 55, 0);
	local_10007CB5(v2, (int *)&unk_10022FB0);
	Doom_100068AB(v2, (int *)&unk_10022F9C, 5);
	Doom_1000658C(v2, (int *)&unk_10022FA4, 4, 0);
	Doom_1000658C(v2, (int *)&unk_10022FB0, 0, 1);
	if ( Modem_1000855D() )
	{
		SDlgSetTimer(v2, 2, 2000, 0);
		result = SelDial_1000B354(v2);
	}
	else
	{
		v16 = a2;
		v14 = v2;
		v6 = byte_1002A380;
		do
		{
			*v6 = 0;
			v6 += 32;
		}
		while ( (signed int)v6 < (signed int)&dword_1002A400 );
		SelDial_1000B5D9();
		LoadStringA(hInstance, 0x34u, (LPSTR)&v13, 31);
		v7 = 0;
		v15 = byte_1002A380;
		do
		{
			v8 = GetDlgItem(v14, v7 + 1119);
			v9 = v8;
			if ( v8 )
			{
				v10 = GetWindowLongA(v8, -21);
				if ( v10 )
				{
					if ( strlen(v15) )
					{
						wsprintfA((LPSTR)&v12, (LPCSTR)&v13, v15);
						v11 = (const char *)&v12;
					}
					else
					{
						EnableWindow(v9, 0);
						v11 = 0;
					}
					local_10007FA4(v10, v11);
				}
			}
			v15 += 32;
			++v7;
		}
		while ( (signed int)v15 < (signed int)&dword_1002A400 );
		result = (HWND)Doom_1000680A(v14, (int *)&unk_10022FB0, 0, 1);
	}
	return result;
} */
// 1000B483: could not find valid save-restore pair for ebp
// 10010412: using guessed type int __stdcall SDlgSetTimer(_DWORD, _DWORD, _DWORD, _DWORD);
// 1002A400: using guessed type int dword_1002A400;

// ref: 0x1000B5D9
int SelDial_1000B5D9() { return 0; }
/* {
	char *v0; // esi
	const char **v1; // edi
	int result; // eax

	v0 = byte_1002A380;
	v1 = (const char **)off_10022F8C;
	do
	{
		result = SRegLoadString("Diablo\\Phone Book", *v1, 1u, v0, 0x20u);
		if ( result )
			v0[31] = 0;
		else
			*v0 = 0;
		++v1;
		v0 += 32;
	}
	while ( (signed int)v1 < (signed int)&unk_10022F9C );
	return result;
} */
// 1001048A: using guessed type int __stdcall SRegLoadString(const char *, const char *, unsigned int, char *, unsigned int);
// 10022F8C: using guessed type char *off_10022F8C[4];

// ref: 0x1000B614
int __fastcall SelDial_1000B614(HWND hWnd, int a2, int a3) { return 0; }
/* {
	int v3; // ebx
	HWND v4; // esi
	int v5; // ST08_4
	HWND v6; // eax
	int v7; // edx
	HWND v8; // eax
	int result; // eax

	v3 = a2;
	v4 = hWnd;
	v5 = a2;
	v6 = GetDlgItem(hWnd, 1056);
	if ( local_10007C3B(v4, v6, v5, a3) )
	{
		v7 = 1;
	}
	else
	{
		v8 = GetDlgItem(v4, 1054);
		result = local_10007C3B(v4, v8, v3, a3);
		if ( !result )
			return result;
		v7 = 2;
	}
	return SelDial_1000B2D8((int)v4, v7);
} */

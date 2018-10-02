// ref: 0x10005CFA
signed int DirLink_10005CFA() { return 0; }
/* {
	signed int result; // eax

	result = 2139095040;
	dword_10029730 = 2139095040;
	return result;
} */
// 10029730: using guessed type int dword_10029730;

// ref: 0x10005D05
BOOL __fastcall DirLink_10005D05(int a1, int a2, int a3, _DWORD *a4, int a5, int a6) { return 0; }
/* {
	int v6; // esi

	dword_1002983C = a3;
	dword_1002984C = a2;
	dword_10029840 = a5;
	dword_10029848 = a4;
	dword_10029844 = a6;
	artfont_10001159();
	v6 = SDlgDialogBoxParam(hInstance, "DIRLINK_DIALOG", a4[2], DirLink_10005D63, 0);
	artfont_100010C8();
	return v6 == 1;
} */
// 10010370: using guessed type int __stdcall SDlgDialogBoxParam(_DWORD, _DWORD, _DWORD, _DWORD, _DWORD);
// 1002983C: using guessed type int dword_1002983C;
// 10029840: using guessed type int dword_10029840;
// 10029844: using guessed type int dword_10029844;
// 1002984C: using guessed type int dword_1002984C;

// ref: 0x10005D63
int __stdcall DirLink_10005D63(HWND hWnd, UINT Msg, WPARAM wParam, unsigned int lParam) { return 0; }
/* {
	HWND v4; // eax
	int v6; // [esp+0h] [ebp-Ch]
	char *v7; // [esp+4h] [ebp-8h]
	char *v8; // [esp+8h] [ebp-4h]

	if ( Msg > 0x111 )
	{
		switch ( Msg )
		{
			case 0x113u:
				if ( wParam == 3 )
				{
					DirLink_100062BF(hWnd, v6, v7, v8);
					DirLink_10006073(hWnd);
				}
				return 0;
			case 0x201u:
				DirLink_10006359(hWnd, (unsigned short)lParam, lParam >> 16);
				break;
			case 0x7E8u:
				if ( !Fade_1000739F() )
					Fade_100073FD(hWnd, v6);
				return 0;
		}
		return SDlgDefDialogProc(hWnd, Msg, wParam, lParam);
	}
	if ( Msg == 273 )
	{
		if ( HIWORD(wParam) == 7 )
		{
			Focus_100075B7(hWnd, (HWND)lParam);
		}
		else if ( HIWORD(wParam) == 6 )
		{
			Focus_10007458((void *)lParam);
			Focus_100075DC(hWnd, (HWND)lParam);
			DirLink_10005EB2(hWnd, (unsigned short)wParam);
		}
		else if ( wParam == 327681 )
		{
			DirLink_100060D1(hWnd);
		}
		else if ( (_WORD)wParam == 2 )
		{
			DirLink_10006047((int)hWnd, 2);
		}
		return SDlgDefDialogProc(hWnd, Msg, wParam, lParam);
	}
	if ( Msg == 2 )
	{
		DirLink_10005F1F(hWnd);
		return SDlgDefDialogProc(hWnd, Msg, wParam, lParam);
	}
	if ( Msg <= 0x103 )
		return SDlgDefDialogProc(hWnd, Msg, wParam, lParam);
	if ( Msg <= 0x105 )
	{
		v4 = (HWND)SDrawGetFrameWindow();
		SendMessageA(v4, Msg, wParam, lParam);
		return SDlgDefDialogProc(hWnd, Msg, wParam, lParam);
	}
	if ( Msg != 272 )
		return SDlgDefDialogProc(hWnd, Msg, wParam, lParam);
	PostMessageA(hWnd, 0x7E8u, 0, 0);
	DirLink_10005F7B(hWnd);
	return 0;
} */
// 1001037C: using guessed type int __stdcall SDlgDefDialogProc(_DWORD, _DWORD, _DWORD, _DWORD);
// 10010382: using guessed type _DWORD __stdcall SDrawGetFrameWindow();

// ref: 0x10005EB2
int __fastcall DirLink_10005EB2(HWND hDlg, int a2) { return 0; }
/* {
	HWND v2; // esi
	int v3; // edi
	HWND v4; // ebx
	int v5; // eax
	CHAR Buffer; // [esp+Ch] [ebp-100h]

	v2 = hDlg;
	v3 = a2;
	v4 = GetDlgItem(hDlg, 1102);
	if ( v3 == 1100 )
		LoadStringA(hInstance, 0x2Au, &Buffer, 255);
	else
		LoadStringA(hInstance, 0x2Fu, &Buffer, 255);
	v5 = GetWindowLongA(v4, -21);
	local_10007FA4(v5, &Buffer);
	return Doom_10006A13(v2, (int *)&unk_10022A40, 1);
} */

// ref: 0x10005F1F
int UNKCALL DirLink_10005F1F(HWND hDlg) { return 0; }
/* {
	HWND v1; // esi
	_DWORD *v2; // eax

	v1 = hDlg;
	Doom_10006C53(hDlg, (int *)&unk_10022A54);
	Doom_10006C53(v1, (int *)&unk_10022A48);
	Doom_10006C53(v1, (int *)&unk_10022A40);
	Doom_10006C53(v1, (int *)&unk_10022A38);
	Doom_10006C53(v1, (int *)&unk_10022A2C);
	v2 = (_DWORD *)GetWindowLongA(v1, -21);
	local_10007F72(v2);
	Title_100100E7(v1);
	return Focus_10007818(v1);
} */

// ref: 0x10005F7B
int UNKCALL DirLink_10005F7B(HWND hWnd) { return 0; }
/* {
	HWND v1; // esi
	HWND v2; // ST1C_4
	int v3; // eax
	int *v4; // edi
	HWND v6; // [esp-4h] [ebp-Ch]
	int v7; // [esp-4h] [ebp-Ch]
	char *v8; // [esp+0h] [ebp-8h]
	char *v9; // [esp+4h] [ebp-4h]

	v1 = hWnd;
	Focus_100077E9((int)hWnd, "ui_art\\focus16.pcx", v6);
	Title_1001009E(v1, (int)"ui_art\\smlogo.pcx", v2);
	v3 = local_10007F46();
	v4 = (int *)v3;
	if ( v3 )
	{
		SetWindowLongA(v1, -21, v3);
		local_10007944((int)v1, 0, &byte_10029448, -1, 1, (int)"ui_art\\selgame.pcx", v4, v4 + 1, 0);
		Fade_100073C5(v1, 1);
	}
	local_10007CB5(v1, (int *)&unk_10022A54);
	Doom_100068AB(v1, (int *)&unk_10022A2C, 5);
	Doom_100068AB(v1, (int *)&unk_10022A38, 3);
	Doom_100068AB(v1, (int *)&unk_10022A40, 1);
	Doom_1000658C(v1, (int *)&unk_10022A48, 4, 0);
	Doom_1000658C(v1, (int *)&unk_10022A54, 2, 1);
	DirLink_100062BF(v1, v7, v8, v9);
	DirLink_10006073(v1);
	return SDlgSetTimer(v1, 3, 2000, 0);
} */
// 10010412: using guessed type int __stdcall SDlgSetTimer(_DWORD, _DWORD, _DWORD, _DWORD);

// ref: 0x10006047
int __fastcall DirLink_10006047(int a1, int a2) { return 0; }
/* {
	int v2; // edi
	int v3; // esi

	v2 = a2;
	v3 = a1;
	TitleSnd_1001031F();
	Fade_100073B4();
	SDlgKillTimer(v3, 3);
	Fade_100072BE(10);
	return SDlgEndDialog(v3, v2);
} */
// 10010376: using guessed type int __stdcall SDlgEndDialog(_DWORD, _DWORD);
// 10010418: using guessed type int __stdcall SDlgKillTimer(_DWORD, _DWORD);

// ref: 0x10006073
void UNKCALL DirLink_10006073(void *arg) { return; }
/* {
	int v1; // esi
	char v2; // [esp+4h] [ebp-100h]
	char v3; // [esp+84h] [ebp-80h]

	v1 = (int)arg;
	if ( dword_10029738 )
	{
		Connect_10004028((int)&v2, 128, (int)&v3, 128);
		if ( SNetJoinGame(dword_10029738, &byte_1002973C, 0, &v2, &v3, dword_10029844) )
			DirLink_10006047(v1, 1);
	}
} */
// 10010430: using guessed type int __stdcall SNetJoinGame(_DWORD, _DWORD, _DWORD, _DWORD, _DWORD, _DWORD);
// 10029738: using guessed type int dword_10029738;
// 10029844: using guessed type int dword_10029844;

// ref: 0x100060D1
HWND UNKCALL DirLink_100060D1(HWND arg) { return 0; }
/* {
	HWND v1; // esi
	HWND v2; // eax
	HWND v3; // edi
	HWND result; // eax
	CHAR Buffer; // [esp+8h] [ebp-80h]

	v1 = arg;
	v2 = GetFocus();
	v3 = v2;
	result = GetParent(v2);
	if ( v1 == result )
	{
		if ( GetWindowLongA(v3, -12) == 1100 )
		{
			result = (HWND)DirLink_10006141(v1);
		}
		else if ( dword_10029738 )
		{
			result = (HWND)DirLink_100061E1(v1);
		}
		else
		{
			LoadStringA(hInstance, 0x2Bu, &Buffer, 127);
			result = (HWND)SelYesNo_1000FD39((int)v1, &Buffer, 0, 0);
		}
	}
	return result;
} */
// 10029738: using guessed type int dword_10029738;

// ref: 0x10006141
int UNKCALL DirLink_10006141(void *arg) { return 0; }
/* {
	int v1; // edi
	int result; // eax
	char v3; // [esp+8h] [ebp-E0h]
	int v4; // [esp+88h] [ebp-60h]
	int v5; // [esp+90h] [ebp-58h]
	int v6; // [esp+D8h] [ebp-10h]
	int v7; // [esp+DCh] [ebp-Ch]
	int v8; // [esp+E0h] [ebp-8h]
	int v9; // [esp+E4h] [ebp-4h]

	v1 = (int)arg;
	Connect_10004028((int)&v3, 128, 0, 0);
	memcpy(&v4, dword_10029848, 0x50u);
	v4 = 80;
	v5 = v1;
	memset(&v6, 0, 0x10u);
	v6 = 16;
	v7 = 1396916812;
	v8 = *(_DWORD *)(dword_1002984C + 24);
	v9 = 0;
	result = CreaDung_100051D8(
				 (int)&v6,
				 dword_1002984C,
				 dword_1002983C,
				 (int)&v4,
				 dword_10029840,
				 dword_10029844,
				 0,
				 (int)&v3);
	if ( result )
		result = DirLink_10006047(v1, 1);
	return result;
} */
// 1002983C: using guessed type int dword_1002983C;
// 10029840: using guessed type int dword_10029840;
// 10029844: using guessed type int dword_10029844;
// 1002984C: using guessed type int dword_1002984C;

// ref: 0x100061E1
int UNKCALL DirLink_100061E1(void *arg) { return 0; }
/* {
	int v1; // ebx
	CHAR *v2; // edx
	CHAR v4; // [esp+Ch] [ebp-380h]
	CHAR v5; // [esp+10Ch] [ebp-280h]
	int v6; // [esp+20Ch] [ebp-180h]
	char v7; // [esp+28Ch] [ebp-100h]
	CHAR Buffer; // [esp+30Ch] [ebp-80h]

	v1 = (int)arg;
	Connect_10004028((int)&v6, 128, (int)&v7, 128);
	if ( UiAuthCallback(2, (int)&v6, &v7, 0, &byte_100297BC, &v5, 256) )
	{
		if ( SNetJoinGame(dword_10029738, &byte_1002973C, 0, &v6, &v7, dword_10029844) )
			return DirLink_10006047(v1, 1);
		if ( SErrGetLastError() == -2062548871 )
			LoadStringA(hInstance, 0x32u, &Buffer, 127);
		else
			LoadStringA(hInstance, 0x25u, &Buffer, 127);
		wsprintfA(&v4, &Buffer, &byte_1002973C);
		v2 = &v4;
	}
	else
	{
		v2 = &v5;
	}
	return SelYesNo_1000FD39(v1, v2, 0, 0);
} */
// 10010406: using guessed type _DWORD __stdcall SErrGetLastError();
// 10010430: using guessed type int __stdcall SNetJoinGame(_DWORD, _DWORD, _DWORD, _DWORD, _DWORD, _DWORD);
// 10029738: using guessed type int dword_10029738;
// 10029844: using guessed type int dword_10029844;

// ref: 0x100062BF
int UNKCALL DirLink_100062BF(void *arg, int a2, char *a3, char *a4) { return 0; }
/* {
	int v4; // esi
	int result; // eax
	CHAR Buffer; // [esp+8h] [ebp-80h]

	v4 = (int)arg;
	dword_10029738 = 0;
	byte_1002973C = 0;
	byte_100297BC = 0;
	result = SNetEnumGames(0, 0, DirLink_1000632B, 0);
	if ( !result )
	{
		result = SErrGetLastError();
		if ( result == -2062548871 )
		{
			LoadStringA(hInstance, 0x32u, &Buffer, 127);
			SelYesNo_1000FD39(v4, &Buffer, 0, 0);
			result = DirLink_10006047(v4, 2);
		}
	}
	return result;
} */
// 10010406: using guessed type _DWORD __stdcall SErrGetLastError();
// 10010436: using guessed type int __stdcall SNetEnumGames(_DWORD, _DWORD, _DWORD, _DWORD);
// 10029738: using guessed type int dword_10029738;

// ref: 0x1000632B
signed int __stdcall DirLink_1000632B(int a1, char *a2, char *a3) { return 0; }
/* {
	dword_10029738 = a1;
	strcpy(&byte_1002973C, a2);
	strcpy(&byte_100297BC, a3);
	return 1;
} */
// 10029738: using guessed type int dword_10029738;

// ref: 0x10006359
HWND __fastcall DirLink_10006359(HWND hWnd, int a2, int a3) { return 0; }
/* {
	int v3; // ebx
	HWND v4; // esi
	int v5; // ST08_4
	HWND v6; // eax
	HWND result; // eax
	HWND v8; // eax

	v3 = a2;
	v4 = hWnd;
	v5 = a2;
	v6 = GetDlgItem(hWnd, 1056);
	if ( local_10007C3B(v4, v6, v5, a3) )
		return DirLink_100060D1(v4);
	v8 = GetDlgItem(v4, 1054);
	result = (HWND)local_10007C3B(v4, v8, v3, a3);
	if ( result )
		result = (HWND)DirLink_10006047((int)v4, 2);
	return result;
} */

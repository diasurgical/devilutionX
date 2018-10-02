// ref: 0x1000855D
int Modem_1000855D() { return 0; }
/* {
	return dword_1002A124;
} */
// 1002A124: using guessed type int dword_1002A124;

// ref: 0x10008563
HWND __fastcall Modem_10008563(HWND hDlg, const char *a2, int a3) { return 0; }
/* {
	HWND v3; // esi
	const char *v4; // ebp
	HWND result; // eax

	v3 = hDlg;
	v4 = a2;
	result = GetDlgItem(hDlg, 1108);
	if ( result )
	{
		result = (HWND)GetWindowLongA(result, -21);
		if ( result )
		{
			local_10007FA4((int)result, (const char *)a3);
			Doom_10006A13(v3, (int *)&unk_10022C5C, 1);
			result = GetDlgItem(v3, 1080);
			if ( result )
			{
				result = (HWND)GetWindowLongA(result, -21);
				if ( result )
				{
					local_10007FA4((int)result, v4);
					result = (HWND)Doom_10006A13(v3, (int *)&unk_10022C54, 3);
				}
			}
		}
	}
	return result;
} */

// ref: 0x100085D8
int __stdcall Modem_100085D8(int a1, char *a2, char *a3) { return 0; }
/* {
	dword_1002A150 = a1;
	strcpy(&byte_1002A154, a2);
	strcpy(&byte_1002A1D4, a3);
	return 1;
} */
// 1002A150: using guessed type int dword_1002A150;

// ref: 0x10008606
BOOL Modem_10008606() { return 0; }
/* {
	BOOL result; // eax

	dword_1002A150 = 0;
	byte_1002A154 = 0;
	byte_1002A1D4 = 0;
	if ( SNetEnumGames(0, 0, Modem_100085D8, 0) )
		result = dword_1002A150 != 0;
	else
		result = 0;
	return result;
} */
// 10010436: using guessed type int __stdcall SNetEnumGames(_DWORD, _DWORD, _DWORD, _DWORD);
// 1002A150: using guessed type int dword_1002A150;

// ref: 0x1000863D
char *Modem_1000863D() { return 0; }
/* {
	return &byte_1002A154;
} */

// ref: 0x10008648
signed int Modem_10008648() { return 0; }
/* {
	signed int result; // eax

	result = 2139095040;
	dword_1002A128 = 2139095040;
	return result;
} */
// 1002A128: using guessed type int dword_1002A128;

// ref: 0x10008653
int Modem_10008653() { return 0; }
/* {
	return dword_1002A148;
} */
// 1002A148: using guessed type int dword_1002A148;

// ref: 0x10008659
int Modem_10008659() { return 0; }
/* {
	return dword_1002A134;
} */
// 1002A134: using guessed type int dword_1002A134;

// ref: 0x1000865F
int UNKCALL Modem_1000865F(char *arg) { return 0; }
/* {
	char v1; // al
	int result; // eax

	while ( 1 )
	{
		v1 = *arg;
		if ( !*arg || (unsigned char)v1 >= 0x30u && (unsigned char)v1 <= 0x39u )
			break;
		++arg;
	}
	if ( *arg )
		result = atoi(arg);
	else
		result = 0;
	return result;
} */

// ref: 0x10008680
BOOL __fastcall Modem_10008680(int a1, int a2, int a3, _DWORD *a4, int a5, int a6) { return 0; }
/* {
	int v6; // esi

	dword_1002A13C = a3;
	dword_1002A138 = a2;
	dword_1002A144 = a5;
	dword_1002A14C = a4;
	dword_1002A140 = a6;
	artfont_10001159();
	v6 = SDlgDialogBoxParam(hInstance, "MODEM_DIALOG", a4[2], Modem_100086DE, 0);
	artfont_100010C8();
	return v6 == 1;
} */
// 10010370: using guessed type int __stdcall SDlgDialogBoxParam(_DWORD, _DWORD, _DWORD, _DWORD, _DWORD);
// 1002A138: using guessed type int dword_1002A138;
// 1002A13C: using guessed type int dword_1002A13C;
// 1002A140: using guessed type int dword_1002A140;
// 1002A144: using guessed type int dword_1002A144;

// ref: 0x100086DE
int __stdcall Modem_100086DE(HWND hWnd, UINT Msg, WPARAM wParam, LPARAM lParam) { return 0; }
/* {
	HWND v4; // eax
	int v6; // [esp+0h] [ebp-8h]

	if ( Msg > 0x7E8 )
	{
		switch ( Msg )
		{
			case 0xBD0u:
				Modem_100088DB(hWnd);
				return 0;
			case 0xBD1u:
				Modem_10008BB7(hWnd);
				return 0;
			case 0xBD2u:
				Modem_10008BFE(hWnd);
				return 0;
		}
		return SDlgDefDialogProc(hWnd, Msg, wParam, lParam);
	}
	if ( Msg == 2024 )
	{
		if ( !Fade_1000739F() )
			Fade_100073FD(hWnd, v6);
		return 0;
	}
	if ( Msg == 2 )
	{
		Modem_1000879E(hWnd);
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
	Modem_100087DB(hWnd);
	PostMessageA(hWnd, 0x7E8u, 0, 0);
	return 0;
} */
// 1001037C: using guessed type int __stdcall SDlgDefDialogProc(_DWORD, _DWORD, _DWORD, _DWORD);
// 10010382: using guessed type _DWORD __stdcall SDrawGetFrameWindow();

// ref: 0x1000879E
void **UNKCALL Modem_1000879E(HWND hDlg) { return 0; }
/* {
	HWND v1; // esi
	_DWORD *v2; // eax

	v1 = hDlg;
	Doom_10006C53(hDlg, (int *)&unk_10022C5C);
	Doom_10006C53(v1, (int *)&unk_10022C54);
	Doom_10006C53(v1, (int *)&unk_10022C4C);
	v2 = (_DWORD *)GetWindowLongA(v1, -21);
	local_10007F72(v2);
	return Title_100100E7(v1);
} */

// ref: 0x100087DB
BOOL UNKCALL Modem_100087DB(HWND hWnd) { return 0; }
/* {
	HWND v1; // esi
	int v2; // eax
	int *v3; // edi
	HWND v5; // [esp+0h] [ebp-10h]

	v1 = hWnd;
	Title_1001009E(hWnd, (int)"ui_art\\smlogo.pcx", v5);
	v2 = local_10007F46();
	v3 = (int *)v2;
	if ( v2 )
	{
		SetWindowLongA(v1, -21, v2);
		local_10007944((int)v1, 0, &byte_10029448, -1, 1, (int)"ui_art\\selgame.pcx", v3, v3 + 1, 0);
		Fade_100073C5(v1, 1);
	}
	Doom_100068AB(v1, (int *)&unk_10022C4C, 5);
	Doom_100068AB(v1, (int *)&unk_10022C54, 3);
	Doom_100068AB(v1, (int *)&unk_10022C5C, 1);
	Modem_10008888();
	if ( dword_1002A124 )
		return PostMessageA(v1, 0xBD2u, 0, 0);
	dword_1002A134 = 1;
	dword_1002A130 = 1;
	return PostMessageA(v1, 0xBD0u, 0, 0);
} */
// 1002A124: using guessed type int dword_1002A124;
// 1002A130: using guessed type int dword_1002A130;
// 1002A134: using guessed type int dword_1002A134;

// ref: 0x10008888
int Modem_10008888() { return 0; }
/* {
	int result; // eax

	dword_1002A150 = 0;
	byte_1002A154 = 0;
	byte_1002A1D4 = 0;
	result = SNetEnumGames(0, 0, Modem_100085D8, 0);
	if ( result )
	{
		dword_1002A124 = 1;
	}
	else
	{
		result = SErrGetLastError();
		if ( result == 1222 )
		{
			dword_1002A124 = 0;
			result = 1;
			dword_1002A134 = 1;
			dword_1002A130 = 1;
		}
	}
	return result;
} */
// 10010406: using guessed type _DWORD __stdcall SErrGetLastError();
// 10010436: using guessed type int __stdcall SNetEnumGames(_DWORD, _DWORD, _DWORD, _DWORD);
// 1002A124: using guessed type int dword_1002A124;
// 1002A130: using guessed type int dword_1002A130;
// 1002A134: using guessed type int dword_1002A134;
// 1002A150: using guessed type int dword_1002A150;

// ref: 0x100088DB
int UNKCALL Modem_100088DB(HWND hWnd) { return 0; }
/* {
	HWND v1; // esi
	int v2; // eax
	int v3; // eax
	int v5; // [esp+4h] [ebp-20h]

	v1 = hWnd;
	v2 = SDlgDialogBoxParam(hInstance, "SELDIAL_DIALOG", hWnd, SelDial_1000B0CF, &v5) - 3;
	if ( !v2 )
		return Modem_1000893D(v1);
	v3 = v2 - 1;
	if ( !v3 )
		return Modem_10008A38(v1, (int)&v5);
	if ( v3 == 1 )
		return PostMessageA(v1, 0xBD1u, 0, 0);
	return SelHero_1000C3E2((int)v1, 2);
} */
// 10010370: using guessed type int __stdcall SDlgDialogBoxParam(_DWORD, _DWORD, _DWORD, _DWORD, _DWORD);

// ref: 0x1000893D
int UNKCALL Modem_1000893D(HWND hWnd) { return 0; }
/* {
	HWND v1; // esi
	int v2; // eax
	int result; // eax
	CHAR v4; // [esp+8h] [ebp-C0h]
	int v5; // [esp+48h] [ebp-80h]
	HWND v6; // [esp+50h] [ebp-78h]
	CHAR Buffer; // [esp+98h] [ebp-30h]
	int v8; // [esp+B8h] [ebp-10h]
	int v9; // [esp+BCh] [ebp-Ch]
	int v10; // [esp+C0h] [ebp-8h]
	int v11; // [esp+C4h] [ebp-4h]

	v1 = hWnd;
	memcpy(&v5, dword_1002A14C, 0x50u);
	v5 = 80;
	v6 = v1;
	memset(&v8, 0, 0x10u);
	v8 = 16;
	v9 = 1297040461;
	v2 = *(_DWORD *)(dword_1002A138 + 24);
	v11 = 0;
	v10 = v2;
	LoadStringA(hInstance, 0x47u, &Buffer, 31);
	wsprintfA(&v4, &Buffer, dword_1002A130);
	if ( CreaDung_100051D8(
			 (int)&v8,
			 dword_1002A138,
			 dword_1002A13C,
			 (int)&v5,
			 dword_1002A144,
			 dword_1002A140,
			 1,
			 (int)&v4) )
	{
		++dword_1002A130;
		result = SelHero_1000C3E2((int)v1, 1);
	}
	else if ( dword_1002A124 )
	{
		if ( SErrGetLastError() == 183 )
			++dword_1002A130;
		result = PostMessageA(v1, 0xBD2u, 0, 0);
	}
	else
	{
		result = PostMessageA(v1, 0xBD0u, 0, 0);
	}
	return result;
} */
// 10010406: using guessed type _DWORD __stdcall SErrGetLastError();
// 1002A124: using guessed type int dword_1002A124;
// 1002A130: using guessed type int dword_1002A130;
// 1002A138: using guessed type int dword_1002A138;
// 1002A13C: using guessed type int dword_1002A13C;
// 1002A140: using guessed type int dword_1002A140;
// 1002A144: using guessed type int dword_1002A144;

// ref: 0x10008A38
int __fastcall Modem_10008A38(HWND hWnd, int a2) { return 0; }
/* {
	char *v2; // ebx
	HWND v3; // edi
	int v4; // eax
	int result; // eax
	CHAR Buffer; // [esp+Ch] [ebp-80h]

	v2 = (char *)a2;
	v3 = hWnd;
	dword_1002A148 = 0;
	_beginthread((int)Modem_10008B42, 0, a2);
	ModmStat_10008C87(v3);
	if ( !dword_1002A120 )
	{
		switch ( dword_1002A12C )
		{
			case -2062548871:
				LoadStringA(hInstance, 0x32u, &Buffer, 127);
				break;
			case 54:
				LoadStringA(hInstance, 0x42u, &Buffer, 127);
				break;
			case 1204:
				LoadStringA(hInstance, 0x4Cu, &Buffer, 127);
				break;
			case 1222:
				LoadStringA(hInstance, 0x41u, &Buffer, 127);
				break;
			case 1223:
				goto LABEL_18;
			case 2250:
				LoadStringA(hInstance, 0x40u, &Buffer, 127);
				break;
			default:
				LoadStringA(hInstance, 0x33u, &Buffer, 127);
				break;
		}
		SelYesNo_1000FD39((int)v3, &Buffer, 0, 1);
LABEL_18:
		if ( dword_1002A124 )
			result = PostMessageA(v3, 0xBD2u, 0, 0);
		else
			result = PostMessageA(v3, 0xBD0u, 0, 0);
		return result;
	}
	if ( !dword_1002A124 )
	{
		SelDial_1000B011(v2);
		Modem_10008606();
	}
	v4 = Modem_1000865F(&byte_1002A154);
	dword_1002A134 = v4;
	dword_1002A130 = v4 + 1;
	return SelHero_1000C3E2((int)v3, 1);
} */
// 1002A120: using guessed type int dword_1002A120;
// 1002A124: using guessed type int dword_1002A124;
// 1002A12C: using guessed type int dword_1002A12C;
// 1002A130: using guessed type int dword_1002A130;
// 1002A134: using guessed type int dword_1002A134;
// 1002A148: using guessed type int dword_1002A148;

// ref: 0x10008B42
void __cdecl Modem_10008B42(char *a1) { return; }
/* {
	char *v1; // eax
	char v2; // [esp+0h] [ebp-100h]
	char v3; // [esp+80h] [ebp-80h]

	Connect_10004028((int)&v2, 128, (int)&v3, 128);
	dword_1002A148 = 0;
	v1 = &byte_1002A154;
	if ( !dword_1002A124 )
		v1 = a1;
	dword_1002A120 = SNetJoinGame(0, v1, 0, &v2, &v3, dword_1002A140);
	if ( !dword_1002A120 )
		dword_1002A12C = SErrGetLastError();
	dword_1002A148 = 1;
	_endthread();
} */
// 10010406: using guessed type _DWORD __stdcall SErrGetLastError();
// 10010430: using guessed type int __stdcall SNetJoinGame(_DWORD, _DWORD, _DWORD, _DWORD, _DWORD, _DWORD);
// 10011E20: using guessed type int _endthread(void);
// 1002A120: using guessed type int dword_1002A120;
// 1002A124: using guessed type int dword_1002A124;
// 1002A12C: using guessed type int dword_1002A12C;
// 1002A140: using guessed type int dword_1002A140;
// 1002A148: using guessed type int dword_1002A148;

// ref: 0x10008BB7
int UNKCALL Modem_10008BB7(HWND hWnd) { return 0; }
/* {
	HWND v1; // esi
	int result; // eax
	int v3; // [esp+4h] [ebp-20h]

	v1 = hWnd;
	if ( SDlgDialogBoxParam(hInstance, "ENTERDIAL_DIALOG", hWnd, EntDial_10006C96, &v3) == 1 )
		result = Modem_10008A38(v1, (int)&v3);
	else
		result = PostMessageA(v1, 0xBD0u, 0, 0);
	return result;
} */
// 10010370: using guessed type int __stdcall SDlgDialogBoxParam(_DWORD, _DWORD, _DWORD, _DWORD, _DWORD);

// ref: 0x10008BFE
int UNKCALL Modem_10008BFE(HWND hWnd) { return 0; }
/* {
	HWND v1; // esi
	int v2; // eax
	int v3; // eax

	v1 = hWnd;
	v2 = SDlgDialogBoxParam(hInstance, "SELCRE8JOIN_DIALOG", hWnd, SelDial_1000B0CF, 0) - 3;
	if ( !v2 )
		return Modem_1000893D(v1);
	v3 = v2 - 2;
	if ( !v3 )
		return Modem_10008A38(v1, (int)&byte_1002A154);
	if ( v3 != 1217 )
		return SelHero_1000C3E2((int)v1, 2);
	dword_1002A124 = 0;
	return PostMessageA(v1, 0xBD0u, 0, 0);
} */
// 10010370: using guessed type int __stdcall SDlgDialogBoxParam(_DWORD, _DWORD, _DWORD, _DWORD, _DWORD);
// 1002A124: using guessed type int dword_1002A124;

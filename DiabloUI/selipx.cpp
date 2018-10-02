// ref: 0x1000C610
void *SelIPX_1000C610() { return 0; }
/* {
	return SMemAlloc(268, "C:\\Src\\Diablo\\DiabloUI\\SelIPX.cpp", 105, 0);
} */
// 10010364: using guessed type int __stdcall SMemAlloc(_DWORD, _DWORD, _DWORD, _DWORD);

// ref: 0x1000C629
signed int SelIPX_1000C629() { return 0; }
/* {
	signed int result; // eax

	result = 2139095040;
	dword_1002A4A4 = 2139095040;
	return result;
} */
// 1002A4A4: using guessed type int dword_1002A4A4;

// ref: 0x1000C634
BOOL __fastcall SelIPX_1000C634(int a1, int a2, int a3, _DWORD *a4, int a5, int a6) { return 0; }
/* {
	int v6; // esi

	dword_1002A49C = a3;
	dword_1002A4AC = a2;
	dword_1002A4BC = a5;
	dword_1002A4A0 = a4;
	dword_1002A4A8 = a6;
	artfont_10001159();
	v6 = SDlgDialogBoxParam(hInstance, "SELIPXGAME_DIALOG", a4[2], SelIPX_1000C692, 0);
	artfont_100010C8();
	return v6 == 1;
} */
// 10010370: using guessed type int __stdcall SDlgDialogBoxParam(_DWORD, _DWORD, _DWORD, _DWORD, _DWORD);
// 1002A49C: using guessed type int dword_1002A49C;
// 1002A4A8: using guessed type int dword_1002A4A8;
// 1002A4AC: using guessed type int dword_1002A4AC;
// 1002A4BC: using guessed type int dword_1002A4BC;

// ref: 0x1000C692
int __stdcall SelIPX_1000C692(HWND hWnd, UINT Msg, WPARAM wParam, LPARAM lParam) { return 0; }
/* {
	HWND v4; // eax
	HWND v6; // eax
	int v7; // [esp+0h] [ebp-Ch]
	int savedregs; // [esp+Ch] [ebp+0h]

	if ( Msg > 0x113 )
	{
		switch ( Msg )
		{
			case 0x201u:
				goto LABEL_35;
			case 0x202u:
				v6 = GetDlgItem(hWnd, 1105);
				if ( !Sbar_100099C0(v6) )
					return SDlgDefDialogProc(hWnd, Msg, wParam, lParam);
				goto LABEL_12;
			case 0x203u:
LABEL_35:
				SelIPX_1000D696(hWnd, (unsigned short)lParam, (unsigned int)lParam >> 16);
				return SDlgDefDialogProc(hWnd, Msg, wParam, lParam);
		}
		if ( Msg != 2024 )
			return SDlgDefDialogProc(hWnd, Msg, wParam, lParam);
		if ( !Fade_1000739F() )
			Fade_100073FD(hWnd, v7);
		return 0;
	}
	if ( Msg == 275 )
	{
		if ( wParam == 3 )
			SelIPX_1000C9DA(hWnd);
		return 0;
	}
	if ( Msg == 2 )
	{
		SelIPX_1000CC41(hWnd);
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
	if ( Msg == 272 )
	{
		SelIPX_1000CD4A(hWnd);
		PostMessageA(hWnd, 0x7E8u, 0, 0);
		return 0;
	}
	if ( Msg == 273 )
	{
		if ( HIWORD(wParam) == 7 )
		{
			Focus_100075B7(hWnd, (HWND)lParam);
			return SDlgDefDialogProc(hWnd, Msg, wParam, lParam);
		}
		if ( HIWORD(wParam) != 6 )
		{
			if ( wParam == 327681 )
			{
				SelIPX_1000D3C5(hWnd, (int)&savedregs);
			}
			else if ( (_WORD)wParam == 2 )
			{
				SelIPX_1000D3A0((int)hWnd, 2);
			}
			return SDlgDefDialogProc(hWnd, Msg, wParam, lParam);
		}
		Focus_10007458((void *)lParam);
		Focus_100075DC(hWnd, (HWND)lParam);
		SelIPX_1000C818(hWnd, (unsigned short)wParam);
LABEL_12:
		SelIPX_1000C982(hWnd);
	}
	return SDlgDefDialogProc(hWnd, Msg, wParam, lParam);
} */
// 1001037C: using guessed type int __stdcall SDlgDefDialogProc(_DWORD, _DWORD, _DWORD, _DWORD);
// 10010382: using guessed type _DWORD __stdcall SDrawGetFrameWindow();

// ref: 0x1000C818
LONG __fastcall SelIPX_1000C818(HWND hDlg, int nIDDlgItem) { return 0; }
/* {
	HWND v2; // ebx
	HWND v3; // edi
	LONG result; // eax
	const char *v5; // edi
	int v6; // eax
	CHAR *v7; // edx
	CHAR v8; // [esp+Ch] [ebp-280h]
	CHAR v9; // [esp+10Ch] [ebp-180h]
	char v10; // [esp+18Ch] [ebp-100h]
	CHAR Buffer; // [esp+20Ch] [ebp-80h]
	CHAR v12; // [esp+22Ch] [ebp-60h]
	char v13; // [esp+24Ch] [ebp-40h]
	unsigned short v14; // [esp+260h] [ebp-2Ch]
	unsigned char v15; // [esp+262h] [ebp-2Ah]
	char v16; // [esp+278h] [ebp-14h]
	unsigned char v17; // [esp+27Ch] [ebp-10h]
	int v18; // [esp+280h] [ebp-Ch]
	int v19; // [esp+284h] [ebp-8h]
	HWND hWnd; // [esp+288h] [ebp-4h]

	v2 = hDlg;
	v3 = GetDlgItem(hDlg, nIDDlgItem);
	hWnd = GetDlgItem(v2, 1098);
	result = GetWindowLongA(v3, -21);
	if ( result )
	{
		result = *(_DWORD *)(result + 12);
		if ( result )
		{
			v5 = (const char *)(result + 140);
			if ( *(_DWORD *)(result + 4) )
			{
				if ( result == -140 || strlen((const char *)(result + 140)) < 0x10 )
				{
					v6 = GetWindowLongA(hWnd, -21);
					v7 = (CHAR *)&byte_10029448;
				}
				else
				{
					v19 = (int)&byte_10029448;
					v18 = (int)&byte_10029448;
					strcpy(&v10, v5);
					if ( Connect_10003DAF(&v10, (int)&v16, (int)&v19, (int)&v18)
					  && Connect_10003E61((const char *)v18, &v13) )
					{
						LoadStringA(hInstance, v17 + 1003, &Buffer, 31);
						LoadStringA(hInstance, v15 + 4, &v12, 31);
						LoadStringA(hInstance, 0x31u, &v9, 127);
						wsprintfA(&v8, &v9, &Buffer, v19, v14, &v12);
						v6 = GetWindowLongA(hWnd, -21);
						v7 = &v8;
					}
					else
					{
						v6 = GetWindowLongA(hWnd, -21);
						v7 = 0;
					}
				}
			}
			else
			{
				v6 = GetWindowLongA(hWnd, -21);
				v7 = (CHAR *)v5;
			}
			local_10007FA4(v6, v7);
			result = Doom_10006A13(v2, (int *)&unk_10023104, 1);
		}
	}
	return result;
} */

// ref: 0x1000C982
HWND UNKCALL SelIPX_1000C982(HWND hDlg) { return 0; }
/* {
	HWND v1; // esi
	int v2; // eax

	v1 = hDlg;
	v2 = SelIPX_1000C99F();
	return Sbar_10009A99(v1, 1105, dword_1002A4B8, v2);
} */

// ref: 0x1000C99F
int SelIPX_1000C99F() { return 0; }
/* {
	HWND v0; // eax
	LONG v1; // eax
	_DWORD *v2; // ecx
	_DWORD *v3; // eax
	int v5; // edx

	v0 = GetFocus();
	if ( !v0 )
		return 0;
	v1 = GetWindowLongA(v0, -21);
	if ( !v1 )
		return 0;
	v2 = (_DWORD *)dword_1002A4B4;
	if ( !dword_1002A4B4 )
		return 0;
	v3 = *(_DWORD **)(v1 + 12);
	if ( !v3 )
		return 0;
	v5 = 0;
	do
	{
		if ( v2 == v3 )
			break;
		v2 = (_DWORD *)*v2;
		++v5;
	}
	while ( v2 );
	return v5;
} */
// 1002A4B4: using guessed type int dword_1002A4B4;

// ref: 0x1000C9DA
const char *UNKCALL SelIPX_1000C9DA(HWND hDlg) { return 0; }
/* {
	HWND v1; // esi
	const char *result; // eax
	int v3; // ST0C_4
	HWND v4; // eax
	HWND v5; // eax
	int v6; // eax

	v1 = hDlg;
	dword_1002A4B0 = 0;
	SelIPX_1000CA64((_DWORD *)dword_1002A4B4);
	SNetEnumGames(0, 0, SelIPX_1000CAD5, 0);
	result = (const char *)SelIPX_1000CA71((_DWORD *)dword_1002A4B4);
	dword_1002A4B4 = (int)result;
	if ( dword_1002A4B0 )
	{
		SelIPX_1000CB83(v1, result);
		v3 = dword_1002A4B8 > 6;
		v4 = GetDlgItem(v1, 1105);
		ShowWindow(v4, v3);
		v5 = GetFocus();
		v6 = GetWindowLongA(v5, -12);
		SelIPX_1000C818(v1, v6);
		result = (const char *)SelIPX_1000C982(v1);
	}
	return result;
} */
// 10010436: using guessed type int __stdcall SNetEnumGames(_DWORD, _DWORD, _DWORD, _DWORD);
// 1002A4B0: using guessed type int dword_1002A4B0;
// 1002A4B4: using guessed type int dword_1002A4B4;

// ref: 0x1000CA64
void __fastcall SelIPX_1000CA64(_DWORD *a1) { return; }
/* {
	while ( a1 )
	{
		a1[2] = 0;
		a1 = (_DWORD *)*a1;
	}
} */

// ref: 0x1000CA71
_DWORD **__fastcall SelIPX_1000CA71(_DWORD *a1) { return 0; }
/* {
	_DWORD **v1; // edi
	_DWORD *v2; // esi

	v1 = (_DWORD **)a1;
	v2 = 0;
	while ( a1 )
	{
		if ( a1[2] || !a1[1] )
		{
			v2 = a1;
			a1 = (_DWORD *)*a1;
		}
		else
		{
			if ( v2 )
				*v2 = *a1;
			else
				v1 = (_DWORD **)*a1;
			SelIPX_1000CAC1(a1);
			--dword_1002A4B8;
			dword_1002A4B0 = 1;
			if ( v2 )
				a1 = (_DWORD *)*v2;
			else
				a1 = *v1;
		}
	}
	return v1;
} */
// 1002A4B0: using guessed type int dword_1002A4B0;

// ref: 0x1000CAC1
int UNKCALL SelIPX_1000CAC1(void *arg) { return 0; }
/* {
	int result; // eax

	if ( arg )
		result = SMemFree(arg, "C:\\Src\\Diablo\\DiabloUI\\SelIPX.cpp", 110, 0);
	return result;
} */
// 10010340: using guessed type int __stdcall SMemFree(_DWORD, _DWORD, _DWORD, _DWORD);

// ref: 0x1000CAD5
void *__stdcall SelIPX_1000CAD5(int a1, char *a2, char *a3) { return 0; }
/* {
	_DWORD *v3; // eax
	int result; // eax
	int v5; // esi
	_DWORD *v6; // eax

	v3 = SelIPX_1000CB73((_DWORD *)dword_1002A4B4, a1);
	if ( v3 )
	{
		v3[2] = 1;
	}
	else
	{
		result = SelIPX_1000C610();
		v5 = result;
		if ( !result )
			return result;
		*(_DWORD *)result = 0;
		*(_DWORD *)(result + 4) = a1;
		*(_DWORD *)(result + 8) = 1;
		strcpy((char *)(result + 12), a2);
		strcpy((char *)(v5 + 140), a3);
		v6 = SelIPX_1000CB50((_DWORD *)dword_1002A4B4, (_DWORD *)v5);
		++dword_1002A4B8;
		dword_1002A4B4 = (int)v6;
		dword_1002A4B0 = 1;
	}
	return 1;
} */
// 1002A4B0: using guessed type int dword_1002A4B0;
// 1002A4B4: using guessed type int dword_1002A4B4;

// ref: 0x1000CB50
_DWORD *__fastcall SelIPX_1000CB50(_DWORD *a1, _DWORD *a2) { return 0; }
/* {
	_DWORD *result; // eax
	_DWORD *v3; // edi
	_DWORD *i; // esi

	result = a1;
	v3 = 0;
	for ( i = a1; i; i = (_DWORD *)*i )
		v3 = i;
	*a2 = i;
	if ( !v3 )
		return a2;
	*v3 = a2;
	return result;
} */

// ref: 0x1000CB73
_DWORD *__fastcall SelIPX_1000CB73(_DWORD *a1, int a2) { return 0; }
/* {
	_DWORD *result; // eax

	for ( result = a1; result && result[1] != a2; result = (_DWORD *)*result )
		;
	return result;
} */

// ref: 0x1000CB83
int __fastcall SelIPX_1000CB83(HWND a1, const char *a2) { return 0; }
/* {
	int *v2; // ebp
	HWND v3; // eax
	HWND v4; // esi
	int v5; // eax
	int v6; // eax
	const char *v8; // [esp+4h] [ebp-8h]
	HWND hDlg; // [esp+8h] [ebp-4h]

	v8 = a2;
	hDlg = a1;
	v2 = &dword_10023118;
	if ( dword_10023118 )
	{
		do
		{
			v3 = GetDlgItem(hDlg, *v2);
			v4 = v3;
			if ( v3 )
			{
				if ( v8 )
				{
					EnableWindow(v3, 1);
					v6 = GetWindowLongA(v4, -21);
					if ( v6 )
					{
						*(_DWORD *)(v6 + 12) = v8;
						local_10007FA4(v6, v8 + 12);
					}
					v8 = *(const char **)v8;
				}
				else
				{
					if ( v3 == GetFocus() )
						SelIPX_1000CCD9(v4);
					EnableWindow(v4, 0);
					v5 = GetWindowLongA(v4, -21);
					if ( v5 )
					{
						*(_DWORD *)(v5 + 12) = 0;
						local_10007FA4(v5, &byte_10029448);
					}
				}
			}
			++v2;
		}
		while ( *v2 );
	}
	return Doom_1000680A(hDlg, &dword_10023118, 2, 1);
} */
// 10023118: using guessed type int dword_10023118;

// ref: 0x1000CC41
int UNKCALL SelIPX_1000CC41(HWND hDlg) { return 0; }
/* {
	HWND v1; // esi
	_DWORD *v2; // eax

	v1 = hDlg;
	SelIPX_1000CCC5((_DWORD *)dword_1002A4B4);
	dword_1002A4B4 = 0;
	Sbar_10009CD2(v1, 1105);
	Doom_10006C53(v1, &dword_10023118);
	Doom_10006C53(v1, (int *)&unk_1002310C);
	Doom_10006C53(v1, (int *)&unk_10023104);
	Doom_10006C53(v1, (int *)&unk_100230FC);
	Doom_10006C53(v1, (int *)&unk_100230F0);
	v2 = (_DWORD *)GetWindowLongA(v1, -21);
	local_10007F72(v2);
	Title_100100E7(v1);
	Focus_10007818(v1);
	return SDrawClearSurface();
} */
// 1001043C: using guessed type _DWORD __stdcall SDrawClearSurface();
// 10023118: using guessed type int dword_10023118;
// 1002A4B4: using guessed type int dword_1002A4B4;

// ref: 0x1000CCC5
int __fastcall SelIPX_1000CCC5(_DWORD *a1) { return 0; }
/* {
	_DWORD *v1; // esi
	int result; // eax

	if ( a1 )
	{
		do
		{
			v1 = (_DWORD *)*a1;
			result = SelIPX_1000CAC1(a1);
			a1 = v1;
		}
		while ( v1 );
	}
	return result;
} */

// ref: 0x1000CCD9
HWND UNKCALL SelIPX_1000CCD9(HWND hWnd) { return 0; }
/* {
	HWND v1; // edi
	HWND v2; // ebx
	int nIDDlgItem[1085]; // [esp+0h] [ebp-1118h]
	int v5; // [esp+10F4h] [ebp-24h]
	int v6; // [esp+1100h] [ebp-18h]
	int v7; // [esp+1104h] [ebp-14h]
	int v8; // [esp+1108h] [ebp-10h]
	int v9; // [esp+110Ch] [ebp-Ch]
	int v10; // [esp+1110h] [ebp-8h]
	int v11; // [esp+1114h] [ebp-4h]

	v1 = hWnd;
	v6 = 1093;
	v7 = 1088;
	v8 = 1089;
	v9 = 1090;
	v10 = 1091;
	v11 = 1092;
	v2 = GetParent(hWnd);
	do
	{
		v5 = nIDDlgItem[GetWindowLongA(v1, -12)];
		v1 = GetDlgItem(v2, v5);
	}
	while ( !IsWindowEnabled(v1) );
	return SetFocus(v1);
} */
// 1000CCD9: using guessed type int nIDDlgItem[1085];

// ref: 0x1000CD4A
HWND UNKCALL SelIPX_1000CD4A(HWND hWnd) { return 0; }
/* {
	HWND v1; // esi
	HWND v2; // ST1C_4
	int v3; // eax
	int *v4; // edi
	HWND v5; // ebp
	int v6; // eax
	HWND result; // eax
	HWND v8; // eax
	HWND v9; // [esp+0h] [ebp-4Ch]
	CHAR Buffer; // [esp+Ch] [ebp-40h]

	v1 = hWnd;
	SelIPX_1000CEE6(hWnd);
	Focus_100077E9((int)v1, "ui_art\\focus16.pcx", v9);
	Title_1001009E(v1, (int)"ui_art\\smlogo.pcx", v2);
	v3 = local_10007F46();
	v4 = (int *)v3;
	if ( v3 )
	{
		SetWindowLongA(v1, -21, v3);
		local_10007944((int)v1, 0, &byte_10029448, -1, 1, (int)"ui_art\\selgame.pcx", v4, v4 + 1, 0);
		Fade_100073C5(v1, 1);
	}
	if ( SelGame_1000B67E() != 1230002254 )
	{
		v5 = GetDlgItem(v1, 1038);
		LoadStringA(hInstance, 0x4Du, &Buffer, 63);
		SetWindowTextA(v5, &Buffer);
	}
	Doom_100068AB(v1, (int *)&unk_100230F0, 5);
	Doom_100068AB(v1, (int *)&unk_100230FC, 3);
	Doom_100068AB(v1, (int *)&unk_10023104, 1);
	Doom_1000658C(v1, (int *)&unk_1002310C, 4, 0);
	Doom_1000658C(v1, &dword_10023118, 2, 1);
	dword_1002A4B8 = 0;
	v6 = SelIPX_1000C610();
	dword_1002A4B4 = v6;
	if ( v6 )
	{
		++dword_1002A4B8;
		*(_DWORD *)(v6 + 4) = 0;
		*(_BYTE *)(dword_1002A4B4 + 140) = 0;
		*(_DWORD *)dword_1002A4B4 = 0;
		LoadStringA(hInstance, 0x24u, (LPSTR)(dword_1002A4B4 + 12), 127);
		LoadStringA(hInstance, 0x2Au, (LPSTR)(dword_1002A4B4 + 140), 127);
	}
	SNetEnumGames(0, 0, SelIPX_1000CAD5, 0);
	SelIPX_1000CB83(v1, (const char *)dword_1002A4B4);
	SDlgSetTimer(v1, 3, 1000, 0);
	result = Sbar_10009BF1(v1, 1105);
	if ( dword_1002A4B8 <= 6 )
	{
		v8 = GetDlgItem(v1, 1105);
		result = (HWND)ShowWindow(v8, 0);
	}
	return result;
} */
// 10010412: using guessed type int __stdcall SDlgSetTimer(_DWORD, _DWORD, _DWORD, _DWORD);
// 10010436: using guessed type int __stdcall SNetEnumGames(_DWORD, _DWORD, _DWORD, _DWORD);
// 10023118: using guessed type int dword_10023118;
// 1002A4B4: using guessed type int dword_1002A4B4;

// ref: 0x1000CEE6
void UNKCALL SelIPX_1000CEE6(HWND hDlg) { return; }
/* {
	HWND v1; // ebx
	int *v2; // edi
	HWND v3; // eax
	HWND v4; // esi
	void *v5; // eax

	v1 = hDlg;
	v2 = &dword_10023118;
	if ( dword_10023118 )
	{
		do
		{
			v3 = GetDlgItem(v1, *v2);
			v4 = v3;
			if ( v3 )
			{
				v5 = (void *)GetWindowLongA(v3, -4);
				SetPropA(v4, "UIOLDPROC", v5);
				SetWindowLongA(v4, -4, (LONG)SelIPX_1000CF38);
			}
			++v2;
		}
		while ( *v2 );
	}
} */
// 10023118: using guessed type int dword_10023118;

// ref: 0x1000CF38
LRESULT __stdcall SelIPX_1000CF38(HWND hWnd, UINT Msg, WPARAM wParam, LPARAM lParam) { return 0; }
/* {
	LRESULT (__stdcall *v4)(HWND, UINT, WPARAM, LPARAM); // edi
	HWND v5; // eax
	UINT v7; // [esp-Ch] [ebp-18h]
	WPARAM v8; // [esp-8h] [ebp-14h]
	LPARAM v9; // [esp-4h] [ebp-10h]

	v4 = (LRESULT (__stdcall *)(HWND, UINT, WPARAM, LPARAM))GetPropA(hWnd, "UIOLDPROC");
	switch ( Msg )
	{
		case 2u:
			RemovePropA(hWnd, "UIOLDPROC");
			if ( !v4 )
				return DefWindowProcA(hWnd, Msg, wParam, lParam);
			SetWindowLongA(hWnd, -4, (LONG)v4);
			break;
		case 0xFu:
			local_10007C95(hWnd);
			return 0;
		case 0x87u:
			return 4;
		case 0x100u:
			if ( wParam > 0x21 )
			{
				if ( wParam == 34 )
				{
					SelIPX_1000D0E1(hWnd);
					return 0;
				}
				if ( wParam > 0x24 )
				{
					if ( wParam <= 0x26 )
					{
						SelIPX_1000D31C(hWnd);
						return 0;
					}
					if ( wParam <= 0x28 )
					{
						SelIPX_1000D284(hWnd);
						return 0;
					}
					if ( wParam == 46 )
					{
						v9 = lParam;
						v8 = 46;
						v7 = 256;
						goto LABEL_24;
					}
				}
			}
			else
			{
				switch ( wParam )
				{
					case 0x21u:
						SelIPX_1000D1D4(hWnd);
						break;
					case 9u:
						if ( GetKeyState(16) >= 0 )
							SelIPX_1000D070(hWnd);
						else
							SelIPX_1000CCD9(hWnd);
						return 0;
					case 0xDu:
						goto LABEL_38;
					case 0x1Bu:
						v9 = 0;
						v8 = 2;
						goto LABEL_12;
					case 0x20u:
LABEL_38:
						v9 = 0;
						v8 = 1;
LABEL_12:
						v7 = 273;
LABEL_24:
						v5 = GetParent(hWnd);
						SendMessageA(v5, v7, v8, v9);
						return 0;
				}
			}
			return 0;
	}
	if ( v4 )
		return CallWindowProcA(v4, hWnd, Msg, wParam, lParam);
	return DefWindowProcA(hWnd, Msg, wParam, lParam);
} */

// ref: 0x1000D070
HWND UNKCALL SelIPX_1000D070(HWND hWnd) { return 0; }
/* {
	HWND v1; // edi
	HWND v2; // ebx
	int nIDDlgItem[1085]; // [esp+0h] [ebp-1118h]
	int v5; // [esp+10F4h] [ebp-24h]
	int v6; // [esp+1100h] [ebp-18h]
	int v7; // [esp+1104h] [ebp-14h]
	int v8; // [esp+1108h] [ebp-10h]
	int v9; // [esp+110Ch] [ebp-Ch]
	int v10; // [esp+1110h] [ebp-8h]
	int v11; // [esp+1114h] [ebp-4h]

	v1 = hWnd;
	v6 = 1089;
	v7 = 1090;
	v8 = 1091;
	v9 = 1092;
	v10 = 1093;
	v11 = 1088;
	v2 = GetParent(hWnd);
	do
	{
		v5 = nIDDlgItem[GetWindowLongA(v1, -12)];
		v1 = GetDlgItem(v2, v5);
	}
	while ( !IsWindowEnabled(v1) );
	return SetFocus(v1);
} */
// 1000D070: using guessed type int nIDDlgItem[1085];

// ref: 0x1000D0E1
HWND UNKCALL SelIPX_1000D0E1(HWND hWnd) { return 0; }
/* {
	HWND v1; // ebp
	HWND result; // eax
	HWND v3; // esi
	HWND v4; // ebx
	HWND v5; // eax
	_DWORD *v6; // eax
	int v7; // eax
	const char *v8; // ebx
	int v9; // eax

	v1 = hWnd;
	result = GetParent(hWnd);
	v3 = result;
	if ( result )
	{
		result = GetDlgItem(result, 1088);
		v4 = result;
		if ( result )
		{
			v5 = GetDlgItem(v3, 1093);
			result = (HWND)GetWindowLongA(v5, -21);
			if ( result )
			{
				v6 = (_DWORD *)*((_DWORD *)result + 3);
				if ( v6 && *v6 )
				{
					v7 = SelIPX_1000D18C(v4) + 6;
					if ( v7 > dword_1002A4B8 - 6 )
						v7 = dword_1002A4B8 - 6;
					result = (HWND)SelIPX_1000D1C1(v7);
					v8 = (const char *)result;
					if ( result )
					{
						TitleSnd_10010315();
						SelIPX_1000CB83(v3, v8);
						v9 = GetWindowLongA(v1, -12);
						SelIPX_1000C818(v3, v9);
						result = SelIPX_1000C982(v3);
					}
				}
				else
				{
					result = SelIPX_1000CCD9(v4);
				}
			}
		}
	}
	return result;
} */

// ref: 0x1000D18C
int UNKCALL SelIPX_1000D18C(HWND hWnd) { return 0; }
/* {
	LONG v1; // eax
	_DWORD *v2; // ecx
	_DWORD *v3; // eax
	int v5; // edx

	if ( !hWnd )
		return 0;
	v1 = GetWindowLongA(hWnd, -21);
	if ( !v1 )
		return 0;
	v2 = (_DWORD *)dword_1002A4B4;
	if ( !dword_1002A4B4 )
		return 0;
	v3 = *(_DWORD **)(v1 + 12);
	if ( !v3 )
		return 0;
	v5 = 0;
	do
	{
		if ( v2 == v3 )
			break;
		v2 = (_DWORD *)*v2;
		++v5;
	}
	while ( v2 );
	return v5;
} */
// 1002A4B4: using guessed type int dword_1002A4B4;

// ref: 0x1000D1C1
_DWORD *__fastcall SelIPX_1000D1C1(int a1) { return 0; }
/* {
	_DWORD *result; // eax

	result = (_DWORD *)dword_1002A4B4;
	while ( result && a1 )
	{
		result = (_DWORD *)*result;
		--a1;
	}
	return result;
} */
// 1002A4B4: using guessed type int dword_1002A4B4;

// ref: 0x1000D1D4
HWND UNKCALL SelIPX_1000D1D4(HWND hWnd) { return 0; }
/* {
	HWND result; // eax
	HWND v2; // esi
	HWND v3; // edi
	HWND v4; // eax
	int v5; // eax
	const char *v6; // edi
	int v7; // eax
	HWND hWnda; // [esp+10h] [ebp-4h]

	hWnda = hWnd;
	result = GetParent(hWnd);
	v2 = result;
	if ( result )
	{
		result = GetDlgItem(result, 1088);
		v3 = result;
		if ( result )
		{
			result = (HWND)GetWindowLongA(result, -21);
			if ( result )
			{
				result = (HWND)*((_DWORD *)result + 3);
				if ( result )
				{
					if ( result == (HWND)dword_1002A4B4 )
					{
						v4 = GetDlgItem(v2, 1093);
						result = SelIPX_1000D070(v4);
					}
					else
					{
						v5 = SelIPX_1000D18C(v3) - 6;
						if ( v5 < 0 )
							v5 = 0;
						result = (HWND)SelIPX_1000D1C1(v5);
						v6 = (const char *)result;
						if ( result )
						{
							TitleSnd_10010315();
							SelIPX_1000CB83(v2, v6);
							v7 = GetWindowLongA(hWnda, -12);
							SelIPX_1000C818(v2, v7);
							result = SelIPX_1000C982(v2);
						}
					}
				}
			}
		}
	}
	return result;
} */
// 1002A4B4: using guessed type int dword_1002A4B4;

// ref: 0x1000D284
HWND UNKCALL SelIPX_1000D284(HWND hWnd) { return 0; }
/* {
	HWND v1; // edi
	HWND result; // eax
	HWND v3; // eax
	const char *v4; // ebp
	HWND v5; // eax
	int v6; // ebx
	HWND v7; // eax
	HWND v8; // eax

	v1 = hWnd;
	result = (HWND)GetWindowLongA(hWnd, -21);
	if ( result )
	{
		result = (HWND)*((_DWORD *)result + 3);
		if ( result )
		{
			if ( *(_DWORD *)result )
			{
				if ( GetWindowLongA(v1, -12) >= 1093 )
				{
					v3 = GetParent(v1);
					result = GetDlgItem(v3, 1089);
					if ( result )
					{
						result = (HWND)GetWindowLongA(result, -21);
						if ( result )
						{
							v4 = (const char *)*((_DWORD *)result + 3);
							if ( v4 )
							{
								TitleSnd_10010315();
								v5 = GetParent(v1);
								SelIPX_1000CB83(v5, v4);
								v6 = GetWindowLongA(v1, -12);
								v7 = GetParent(v1);
								SelIPX_1000C818(v7, v6);
								v8 = GetParent(v1);
								result = SelIPX_1000C982(v8);
							}
						}
					}
				}
				else
				{
					result = SelIPX_1000D070(v1);
				}
			}
		}
	}
	return result;
} */

// ref: 0x1000D31C
HWND UNKCALL SelIPX_1000D31C(HWND hWnd) { return 0; }
/* {
	HWND v1; // edi
	HWND result; // eax
	const char *v3; // ebx
	HWND v4; // eax
	int v5; // ebx
	HWND v6; // eax
	HWND v7; // eax

	v1 = hWnd;
	if ( GetWindowLongA(hWnd, -12) > 1088 )
		return SelIPX_1000CCD9(v1);
	result = (HWND)GetWindowLongA(v1, -21);
	if ( result )
	{
		result = (HWND)*((_DWORD *)result + 3);
		if ( result )
		{
			v3 = (const char *)dword_1002A4B4;
			if ( result != (HWND)dword_1002A4B4 )
			{
				while ( v3 && *(HWND *)v3 != result )
					v3 = *(const char **)v3;
				TitleSnd_10010315();
				v4 = GetParent(v1);
				SelIPX_1000CB83(v4, v3);
				v5 = GetWindowLongA(v1, -12);
				v6 = GetParent(v1);
				SelIPX_1000C818(v6, v5);
				v7 = GetParent(v1);
				result = SelIPX_1000C982(v7);
			}
		}
	}
	return result;
} */
// 1002A4B4: using guessed type int dword_1002A4B4;

// ref: 0x1000D3A0
int __fastcall SelIPX_1000D3A0(int a1, int a2) { return 0; }
/* {
	int v2; // edi
	int v3; // esi

	v2 = a2;
	v3 = a1;
	Fade_100073B4();
	SDlgKillTimer(v3, 3);
	Fade_100072BE(10);
	return SDlgEndDialog(v3, v2);
} */
// 10010376: using guessed type int __stdcall SDlgEndDialog(_DWORD, _DWORD);
// 10010418: using guessed type int __stdcall SDlgKillTimer(_DWORD, _DWORD);

// ref: 0x1000D3C5
HWND USERCALL SelIPX_1000D3C5(HWND hDlg, int a2) { return 0; }
/* {
	HWND v2; // edi
	HWND v3; // eax
	HWND v4; // esi
	HWND result; // eax
	int v6; // esi
	HWND v7; // esi
	int v8; // eax
	HWND v9; // edi
	int v10; // [esp-DCh] [ebp-E4h]
	signed int v11; // [esp-5Ch] [ebp-64h]
	HWND v12; // [esp-54h] [ebp-5Ch]
	signed int v13; // [esp-Ch] [ebp-14h]
	signed int v14; // [esp-8h] [ebp-10h]
	int v15; // [esp-4h] [ebp-Ch]
	int v16; // [esp+0h] [ebp-8h]
	int v17; // [esp+4h] [ebp-4h]

	v2 = hDlg;
	v3 = GetFocus();
	v4 = v3;
	result = GetParent(v3);
	if ( v2 == result )
	{
		result = (HWND)GetWindowLongA(v4, -21);
		if ( result )
		{
			v6 = *((_DWORD *)result + 3);
			TitleSnd_1001031F();
			if ( *(_DWORD *)(v6 + 4) )
			{
				result = (HWND)SelIPX_1000D5B0((int)v2, v6);
			}
			else
			{
				v17 = a2;
				v7 = v2;
				SelIPX_1000C9DA(v2);
				memcpy(&v11, dword_1002A4A0, 0x50u);
				v11 = 80;
				v12 = v2;
				memset(&v13, 0, 0x10u);
				v13 = 16;
				v14 = 1230002254;
				v8 = *(_DWORD *)(dword_1002A4AC + 24);
				v16 = 0;
				v15 = v8;
				v9 = GetFocus();
				SelIPX_1000D4CA(v7, 0);
				SelIPX_1000D520((char *)&v10);
				if ( CreaDung_100051D8(
						 (int)&v13,
						 dword_1002A4AC,
						 dword_1002A49C,
						 (int)&v11,
						 dword_1002A4BC,
						 dword_1002A4A8,
						 0,
						 (int)&v10) )
				{
					result = (HWND)SelIPX_1000D3A0((int)v7, 1);
				}
				else
				{
					SelIPX_1000D4CA(v7, 1);
					result = SetFocus(v9);
				}
			}
		}
	}
	return result;
} */
// 1000D3C5: could not find valid save-restore pair for ebp
// 1002A49C: using guessed type int dword_1002A49C;
// 1002A4A8: using guessed type int dword_1002A4A8;
// 1002A4AC: using guessed type int dword_1002A4AC;
// 1002A4BC: using guessed type int dword_1002A4BC;

// ref: 0x1000D4CA
BOOL __fastcall SelIPX_1000D4CA(HWND hDlg, int a2) { return 0; }
/* {
	HWND v2; // ebx
	int v3; // ebp
	HWND v4; // eax
	HWND v5; // eax
	BOOL result; // eax
	int nCmdShow; // [esp+10h] [ebp-4h]

	nCmdShow = a2;
	v2 = hDlg;
	v3 = 1088;
	do
	{
		v4 = GetDlgItem(v2, v3);
		if ( v4 )
			ShowWindow(v4, nCmdShow);
		++v3;
	}
	while ( v3 <= 1093 );
	v5 = GetDlgItem(v2, 1105);
	if ( dword_1002A4B8 > 6 )
		result = ShowWindow(v5, nCmdShow);
	else
		result = ShowWindow(v5, 0);
	return result;
} */

// ref: 0x1000D520
char *UNKCALL SelIPX_1000D520(char *arg) { return 0; }
/* {
	char *v1; // esi
	char *result; // eax
	signed int v3; // edi
	signed int v4; // eax
	char v5; // [esp+4h] [ebp-80h]

	v1 = arg;
	Connect_10004028((int)&v5, 128, 0, 0);
	if ( !SelIPX_1000D58D((const char *)dword_1002A4B4, &v5) )
		return strcpy(v1, &v5);
	v3 = 2;
	do
	{
		v4 = v3++;
		wsprintfA(v1, "%s %d", &v5, v4);
		result = (char *)SelIPX_1000D58D((const char *)dword_1002A4B4, v1);
	}
	while ( result );
	return result;
} */
// 1002A4B4: using guessed type int dword_1002A4B4;

// ref: 0x1000D58D
const char *__fastcall SelIPX_1000D58D(const char *a1, const char *a2) { return 0; }
/* {
	const char *v2; // edi
	const char *i; // esi

	v2 = a2;
	for ( i = a1; i && _strcmpi(i + 12, v2); i = *(const char **)i )
		;
	return i;
} */

// ref: 0x1000D5B0
int __fastcall SelIPX_1000D5B0(int a1, int a2) { return 0; }
/* {
	int v2; // esi
	CHAR *v3; // edx
	CHAR v5; // [esp+Ch] [ebp-384h]
	CHAR v6; // [esp+10Ch] [ebp-284h]
	char v7; // [esp+20Ch] [ebp-184h]
	CHAR Buffer; // [esp+28Ch] [ebp-104h]
	int v9; // [esp+30Ch] [ebp-84h]
	int v10; // [esp+38Ch] [ebp-4h]

	v2 = a2;
	v10 = a1;
	Connect_10004028((int)&v9, 128, (int)&v7, 128);
	if ( UiAuthCallback(2, (int)&v9, &v7, 0, (char *)(v2 + 140), &v6, 256) )
	{
		if ( SNetJoinGame(*(_DWORD *)(v2 + 4), v2 + 12, 0, &v9, &v7, dword_1002A4A8) )
			return SelIPX_1000D3A0(v10, 1);
		if ( SErrGetLastError() == -2062548871 )
			LoadStringA(hInstance, 0x32u, &Buffer, 127);
		else
			LoadStringA(hInstance, 0x25u, &Buffer, 127);
		wsprintfA(&v5, &Buffer, v2 + 12);
		v3 = &v5;
	}
	else
	{
		v3 = &v6;
	}
	return SelYesNo_1000FD39(v10, v3, 0, 0);
} */
// 10010406: using guessed type _DWORD __stdcall SErrGetLastError();
// 10010430: using guessed type int __stdcall SNetJoinGame(_DWORD, _DWORD, _DWORD, _DWORD, _DWORD, _DWORD);
// 1002A4A8: using guessed type int dword_1002A4A8;

// ref: 0x1000D696
HWND __fastcall SelIPX_1000D696(HWND hDlg, int a2, int a3) { return 0; }
/* {
	int v3; // ebx
	HWND v4; // esi
	int v5; // ST08_4
	HWND v6; // eax
	HWND result; // eax
	HWND v8; // eax
	HWND v9; // eax
	HWND v10; // eax
	int v11; // eax
	int v12; // eax
	int v13; // eax
	HWND v14; // eax
	HWND v15; // eax
	HWND v16; // eax
	HWND v17; // eax

	v3 = a2;
	v4 = hDlg;
	v5 = a2;
	v6 = GetDlgItem(hDlg, 1056);
	if ( local_10007C3B(v4, v6, v5, a3) )
		return SelIPX_1000D3C5(v4, a3);
	v8 = GetDlgItem(v4, 1054);
	if ( local_10007C3B(v4, v8, v3, a3) )
		return (HWND)SelIPX_1000D3A0((int)v4, 2);
	v9 = GetDlgItem(v4, 1105);
	result = (HWND)local_10007C3B(v4, v9, v3, a3);
	if ( result )
	{
		v10 = GetDlgItem(v4, 1105);
		v11 = Sbar_100099DC(v10, v3, a3) - 1;
		if ( v11 )
		{
			v12 = v11 - 1;
			if ( v12 )
			{
				v13 = v12 - 1;
				if ( v13 )
				{
					result = (HWND)(v13 - 1);
					if ( !result )
					{
						v14 = GetFocus();
						result = SelIPX_1000D0E1(v14);
					}
				}
				else
				{
					v15 = GetFocus();
					result = SelIPX_1000D1D4(v15);
				}
			}
			else
			{
				v16 = GetFocus();
				result = SelIPX_1000D284(v16);
			}
		}
		else
		{
			v17 = GetFocus();
			result = SelIPX_1000D31C(v17);
		}
	}
	return result;
} */

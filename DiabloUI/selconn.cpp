// ref: 0x1000A082
void *SelConn_1000A082() { return 0; }
/* {
	return SMemAlloc(272, "C:\\Src\\Diablo\\DiabloUI\\SelConn.cpp", 124, 0);
} */
// 10010364: using guessed type int __stdcall SMemAlloc(_DWORD, _DWORD, _DWORD, _DWORD);

// ref: 0x1000A09B
signed int SelConn_1000A09B() { return 0; }
/* {
	signed int result; // eax

	result = 2139095040;
	dword_1002A350 = 2139095040;
	return result;
} */
// 1002A350: using guessed type int dword_1002A350;

// ref: 0x1000A0A6
int __stdcall SelConn_1000A0A6(HWND hWnd, UINT Msg, WPARAM wParam, unsigned int lParam) { return 0; }
/* {
	HWND v4; // eax
	HWND v6; // eax
	char *v7; // [esp+0h] [ebp-Ch]
	int v8; // [esp+4h] [ebp-8h]

	if ( Msg > 0x201 )
	{
		if ( Msg == 514 )
		{
			v6 = GetDlgItem(hWnd, 1105);
			if ( !Sbar_100099C0(v6) )
				return SDlgDefDialogProc(hWnd, Msg, wParam, lParam);
			goto LABEL_27;
		}
		if ( Msg != 515 )
		{
			if ( Msg != 2024 )
				return SDlgDefDialogProc(hWnd, Msg, wParam, lParam);
			if ( !Fade_1000739F() )
				Fade_100073FD(hWnd, (int)v7);
			return 0;
		}
LABEL_25:
		SelConn_1000AE59(hWnd, (unsigned short)lParam, lParam >> 16);
		return SDlgDefDialogProc(hWnd, Msg, wParam, lParam);
	}
	if ( Msg == 513 )
		goto LABEL_25;
	if ( Msg == 2 )
	{
		SelConn_1000A43A(hWnd);
		BNetGW_10002A07(&unk_10029480);
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
		BNetGW_100028C2(&unk_10029480);
		SelConn_1000A4E4(hWnd, v7, v8);
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
				SelConn_1000AC30(hWnd);
			}
			else if ( (_WORD)wParam == 2 )
			{
				SelConn_1000AC07((int)hWnd, 2);
			}
			return SDlgDefDialogProc(hWnd, Msg, wParam, lParam);
		}
		Focus_10007458((void *)lParam);
		Focus_100075DC(hWnd, (HWND)lParam);
		SelConn_1000A226(hWnd, (unsigned short)wParam);
LABEL_27:
		SelConn_1000A3E2(hWnd);
	}
	return SDlgDefDialogProc(hWnd, Msg, wParam, lParam);
} */
// 1001037C: using guessed type int __stdcall SDlgDefDialogProc(_DWORD, _DWORD, _DWORD, _DWORD);
// 10010382: using guessed type _DWORD __stdcall SDrawGetFrameWindow();

// ref: 0x1000A226
HWND __fastcall SelConn_1000A226(HWND hDlg, int nIDDlgItem) { return 0; }
/* {
	HWND v2; // edi
	HWND result; // eax
	int v4; // ebx
	int v5; // eax
	HWND v6; // ebp
	unsigned int v7; // eax
	int v8; // eax
	const char *v9; // ebx
	int v10; // eax
	HWND v11; // eax
	HWND v12; // eax
	HWND v13; // eax
	HWND v14; // eax
	HWND v15; // eax
	HWND v16; // eax
	HWND v17; // eax
	HWND v18; // eax
	HWND hWnd; // [esp+10h] [ebp-8Ch]
	CHAR Buffer; // [esp+14h] [ebp-88h]
	CHAR v21; // [esp+54h] [ebp-48h]

	v2 = hDlg;
	result = GetDlgItem(hDlg, nIDDlgItem);
	if ( result )
	{
		result = (HWND)GetWindowLongA(result, -21);
		if ( result )
		{
			v4 = *((_DWORD *)result + 3);
			if ( v4 )
			{
				result = GetDlgItem(v2, 1081);
				if ( result )
				{
					v5 = GetWindowLongA(result, -21);
					local_10007FA4(v5, (const char *)(v4 + 144));
					result = GetDlgItem(v2, 1076);
					v6 = result;
					if ( result )
					{
						LoadStringA(hInstance, 0x21u, &Buffer, 63);
						if ( dword_1002A370 )
						{
							v7 = *(_DWORD *)(dword_1002A370 + 24);
							if ( v7 >= *(_DWORD *)(v4 + 12) )
								v7 = *(_DWORD *)(v4 + 12);
							wsprintfA(&v21, &Buffer, v7);
						}
						else
						{
							wsprintfA(&v21, &Buffer, *(_DWORD *)(v4 + 12));
						}
						v8 = GetWindowLongA(v6, -21);
						local_10007FA4(v8, &v21);
						if ( *(_DWORD *)(v4 + 8) == 1112425812 )
						{
							hWnd = GetDlgItem(v2, 1144);
							v9 = BNetGW_10002B21(&unk_10029480, dword_1002948C);
							if ( !v9 )
								v9 = &byte_10029448;
							if ( hWnd )
							{
								v10 = GetWindowLongA(hWnd, -21);
								local_10007FA4(v10, v9);
							}
							v11 = GetDlgItem(v2, 1143);
							ShowWindow(v11, 5);
							v12 = GetDlgItem(v2, 1147);
							ShowWindow(v12, 0);
							v13 = GetDlgItem(v2, 1144);
							ShowWindow(v13, 5);
							v14 = GetDlgItem(v2, 1145);
							ShowWindow(v14, 5);
							dword_1002A354 = 1;
						}
						else
						{
							v15 = GetDlgItem(v2, 1143);
							ShowWindow(v15, 0);
							v16 = GetDlgItem(v2, 1147);
							ShowWindow(v16, 5);
							v17 = GetDlgItem(v2, 1144);
							ShowWindow(v17, 0);
							v18 = GetDlgItem(v2, 1145);
							ShowWindow(v18, 0);
							dword_1002A354 = 0;
						}
						result = (HWND)Doom_10006A13(v2, (int *)&unk_10022EF0, 1);
					}
				}
			}
		}
	}
	return result;
} */
// 1002948C: using guessed type int dword_1002948C;
// 1002A354: using guessed type int dword_1002A354;
// 1002A370: using guessed type int dword_1002A370;

// ref: 0x1000A3E2
HWND UNKCALL SelConn_1000A3E2(HWND hDlg) { return 0; }
/* {
	HWND v1; // esi
	int v2; // eax

	v1 = hDlg;
	v2 = SelConn_1000A3FF();
	return Sbar_10009A99(v1, 1105, dword_1002A360, v2);
} */

// ref: 0x1000A3FF
int SelConn_1000A3FF() { return 0; }
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
	v2 = (_DWORD *)dword_1002A35C;
	if ( !dword_1002A35C )
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
// 1002A35C: using guessed type int dword_1002A35C;

// ref: 0x1000A43A
void UNKCALL SelConn_1000A43A(HWND hDlg) { return; }
/* {
	HWND v1; // esi
	_DWORD *v2; // eax

	v1 = hDlg;
	Title_100100E7(hDlg);
	Focus_10007818(v1);
	Sbar_10009CD2(v1, 1105);
	SelConn_1000A4B9((_DWORD *)dword_1002A35C);
	Doom_10006C53(v1, &dword_10022F18);
	Doom_10006C53(v1, (int *)&unk_10022F08);
	Doom_10006C53(v1, (int *)&unk_10022ED8);
	Doom_10006C53(v1, (int *)&unk_10022EE4);
	Doom_10006C53(v1, (int *)&unk_10022F00);
	Doom_10006C53(v1, (int *)&unk_10022EF0);
	v2 = (_DWORD *)GetWindowLongA(v1, -21);
	local_10007F72(v2);
} */
// 10022F18: using guessed type int dword_10022F18;
// 1002A35C: using guessed type int dword_1002A35C;

// ref: 0x1000A4B9
int __fastcall SelConn_1000A4B9(_DWORD *a1) { return 0; }
/* {
	_DWORD *v1; // esi
	int result; // eax

	if ( a1 )
	{
		do
		{
			v1 = (_DWORD *)*a1;
			result = SelConn_1000A4CD(a1);
			a1 = v1;
		}
		while ( v1 );
	}
	return result;
} */

// ref: 0x1000A4CD
int UNKCALL SelConn_1000A4CD(void *arg) { return 0; }
/* {
	int result; // eax

	if ( arg )
		result = SMemFree(arg, "C:\\Src\\Diablo\\DiabloUI\\SelConn.cpp", 130, 0);
	return result;
} */
// 10010340: using guessed type int __stdcall SMemFree(_DWORD, _DWORD, _DWORD, _DWORD);

// ref: 0x1000A4E4
HWND UNKCALL SelConn_1000A4E4(HWND hWnd, char *a2, int a3) { return 0; }
/* {
	HWND v3; // esi
	HWND v4; // ST1C_4
	int v5; // eax
	int *v6; // edi
	HWND result; // eax
	HWND v8; // eax
	HWND v9; // [esp+0h] [ebp-Ch]

	v3 = hWnd;
	SelConn_1000A6EC(hWnd);
	Focus_100077E9((int)v3, "ui_art\\focus16.pcx", v9);
	Title_1001009E(v3, (int)"ui_art\\smlogo.pcx", v4);
	v5 = local_10007F46();
	v6 = (int *)v5;
	if ( v5 )
	{
		SetWindowLongA(v3, -21, v5);
		local_10007944((int)v3, 0, &byte_10029448, -1, 1, (int)"ui_art\\selconn.pcx", v6, v6 + 1, 0);
		Fade_100073C5(v3, 1);
	}
	Doom_100068AB(v3, (int *)&unk_10022EF0, 1);
	Doom_100068AB(v3, (int *)&unk_10022F00, 1);
	Doom_100068AB(v3, (int *)&unk_10022ED8, 5);
	Doom_100068AB(v3, (int *)&unk_10022EE4, 3);
	Doom_1000658C(v3, (int *)&unk_10022F08, 4, 0);
	Doom_1000658C(v3, &dword_10022F18, 0, 1);
	dword_1002A360 = 0;
	dword_1002A368 = dword_10029488;
	dword_1002A35C = 0;
	SNetEnumProviders(0, SelConn_1000A5F3);
	SelConn_1000A670(v3, (const char *)dword_1002A35C);
	result = Sbar_10009BF1(v3, 1105);
	if ( dword_1002A360 <= 6 )
	{
		v8 = GetDlgItem(v3, 1105);
		result = (HWND)ShowWindow(v8, 0);
	}
	return result;
} */
// 10010472: using guessed type int __stdcall SNetEnumProviders(_DWORD, _DWORD);
// 10022F18: using guessed type int dword_10022F18;
// 10029488: using guessed type int dword_10029488;
// 1002A35C: using guessed type int dword_1002A35C;
// 1002A368: using guessed type int dword_1002A368;

// ref: 0x1000A5F3
signed int __stdcall SelConn_1000A5F3(int a1, char *a2, char *a3, int a4) { return 0; }
/* {
	int v4; // esi
	int v6; // edx
	_DWORD *v7; // eax

	v4 = SelConn_1000A082();
	if ( !v4 || a1 == 1112425812 && !dword_1002A368 )
		return 0;
	*(_DWORD *)v4 = 0;
	v6 = *(_DWORD *)(a4 + 4);
	*(_DWORD *)(v4 + 8) = a1;
	*(_DWORD *)(v4 + 4) = v6 & 2;
	*(_DWORD *)(v4 + 12) = *(_DWORD *)(a4 + 16);
	strcpy((char *)(v4 + 16), a2);
	strcpy((char *)(v4 + 144), a3);
	v7 = SelRegn_1000EF56(dword_1002A35C, (_DWORD *)v4);
	++dword_1002A360;
	dword_1002A35C = (int)v7;
	return 1;
} */
// 1002A35C: using guessed type int dword_1002A35C;
// 1002A368: using guessed type int dword_1002A368;

// ref: 0x1000A670
int __fastcall SelConn_1000A670(HWND a1, const char *a2) { return 0; }
/* {
	const char *v2; // edi
	int *v3; // ebx
	HWND v4; // eax
	HWND v5; // esi
	int v6; // eax
	HWND hDlg; // [esp+8h] [ebp-4h]

	v2 = a2;
	hDlg = a1;
	v3 = &dword_10022F18;
	if ( dword_10022F18 )
	{
		do
		{
			v4 = GetDlgItem(hDlg, *v3);
			v5 = v4;
			if ( v4 )
			{
				if ( v2 )
				{
					EnableWindow(v4, 1);
					v6 = GetWindowLongA(v5, -21);
					if ( v6 )
					{
						*(_DWORD *)(v6 + 12) = v2;
						local_10007FA4(v6, v2 + 16);
						v2 = *(const char **)v2;
					}
				}
				else
				{
					EnableWindow(v4, 0);
				}
			}
			++v3;
		}
		while ( *v3 );
	}
	return Doom_1000680A(hDlg, &dword_10022F18, 0, 1);
} */
// 10022F18: using guessed type int dword_10022F18;

// ref: 0x1000A6EC
void UNKCALL SelConn_1000A6EC(HWND hDlg) { return; }
/* {
	HWND v1; // ebx
	int *v2; // edi
	HWND v3; // eax
	HWND v4; // esi
	void *v5; // eax

	v1 = hDlg;
	v2 = &dword_10022F18;
	if ( dword_10022F18 )
	{
		do
		{
			v3 = GetDlgItem(v1, *v2);
			v4 = v3;
			if ( v3 )
			{
				v5 = (void *)GetWindowLongA(v3, -4);
				SetPropA(v4, "UIOLDPROC", v5);
				SetWindowLongA(v4, -4, (LONG)SelConn_1000A73E);
			}
			++v2;
		}
		while ( *v2 );
	}
} */
// 10022F18: using guessed type int dword_10022F18;

// ref: 0x1000A73E
LRESULT __stdcall SelConn_1000A73E(HWND hWnd, UINT Msg, WPARAM wParam, LPARAM lParam) { return 0; }
/* {
	LRESULT (__stdcall *v4)(HWND, UINT, WPARAM, LPARAM); // edi
	HWND v5; // eax
	WPARAM v7; // [esp-8h] [ebp-14h]

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
					SelConn_1000A948(hWnd);
				}
				else if ( wParam > 0x24 )
				{
					if ( wParam <= 0x26 )
					{
						SelConn_1000AB83(hWnd);
					}
					else if ( wParam <= 0x28 )
					{
						SelConn_1000AAEB(hWnd);
					}
				}
				return 0;
			}
			if ( wParam == 33 )
			{
				SelConn_1000AA3B(hWnd);
				return 0;
			}
			if ( wParam == 9 )
			{
				if ( GetKeyState(16) >= 0 )
					SelConn_1000A866(hWnd);
				else
					SelConn_1000A8D7(hWnd);
				return 0;
			}
			if ( wParam != 13 )
			{
				if ( wParam == 27 )
				{
					v7 = 2;
					goto LABEL_13;
				}
				if ( wParam != 32 )
					return 0;
			}
			v7 = 1;
LABEL_13:
			v5 = GetParent(hWnd);
			SendMessageA(v5, 0x111u, v7, 0);
			return 0;
	}
	if ( v4 )
		return CallWindowProcA(v4, hWnd, Msg, wParam, lParam);
	return DefWindowProcA(hWnd, Msg, wParam, lParam);
} */

// ref: 0x1000A866
HWND UNKCALL SelConn_1000A866(HWND hWnd) { return 0; }
/* {
	HWND v1; // edi
	HWND v2; // ebx
	int nIDDlgItem[1066]; // [esp+0h] [ebp-10CCh]
	int v5; // [esp+10A8h] [ebp-24h]
	int v6; // [esp+10B4h] [ebp-18h]
	int v7; // [esp+10B8h] [ebp-14h]
	int v8; // [esp+10BCh] [ebp-10h]
	int v9; // [esp+10C0h] [ebp-Ch]
	int v10; // [esp+10C4h] [ebp-8h]
	int v11; // [esp+10C8h] [ebp-4h]

	v1 = hWnd;
	v6 = 1070;
	v7 = 1071;
	v8 = 1072;
	v9 = 1073;
	v10 = 1074;
	v11 = 1069;
	v2 = GetParent(hWnd);
	do
	{
		v5 = nIDDlgItem[GetWindowLongA(v1, -12)];
		v1 = GetDlgItem(v2, v5);
	}
	while ( !IsWindowEnabled(v1) );
	return SetFocus(v1);
} */
// 1000A866: using guessed type int nIDDlgItem[1066];

// ref: 0x1000A8D7
HWND UNKCALL SelConn_1000A8D7(HWND hWnd) { return 0; }
/* {
	HWND v1; // edi
	HWND v2; // ebx
	int nIDDlgItem[1066]; // [esp+0h] [ebp-10CCh]
	int v5; // [esp+10A8h] [ebp-24h]
	int v6; // [esp+10B4h] [ebp-18h]
	int v7; // [esp+10B8h] [ebp-14h]
	int v8; // [esp+10BCh] [ebp-10h]
	int v9; // [esp+10C0h] [ebp-Ch]
	int v10; // [esp+10C4h] [ebp-8h]
	int v11; // [esp+10C8h] [ebp-4h]

	v1 = hWnd;
	v6 = 1074;
	v7 = 1069;
	v8 = 1070;
	v9 = 1071;
	v10 = 1072;
	v11 = 1073;
	v2 = GetParent(hWnd);
	do
	{
		v5 = nIDDlgItem[GetWindowLongA(v1, -12)];
		v1 = GetDlgItem(v2, v5);
	}
	while ( !IsWindowEnabled(v1) );
	return SetFocus(v1);
} */
// 1000A8D7: using guessed type int nIDDlgItem[1066];

// ref: 0x1000A948
HWND UNKCALL SelConn_1000A948(HWND hWnd) { return 0; }
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
		result = GetDlgItem(result, 1069);
		v4 = result;
		if ( result )
		{
			v5 = GetDlgItem(v3, 1074);
			result = (HWND)GetWindowLongA(v5, -21);
			if ( result )
			{
				v6 = (_DWORD *)*((_DWORD *)result + 3);
				if ( v6 && *v6 )
				{
					v7 = SelConn_1000A9F3(v4) + 6;
					if ( v7 > dword_1002A360 - 6 )
						v7 = dword_1002A360 - 6;
					result = (HWND)SelConn_1000AA28(v7);
					v8 = (const char *)result;
					if ( result )
					{
						TitleSnd_10010315();
						SelConn_1000A670(v3, v8);
						v9 = GetWindowLongA(v1, -12);
						SelConn_1000A226(v3, v9);
						result = SelConn_1000A3E2(v3);
					}
				}
				else
				{
					result = SelConn_1000A8D7(v4);
				}
			}
		}
	}
	return result;
} */

// ref: 0x1000A9F3
int UNKCALL SelConn_1000A9F3(HWND hWnd) { return 0; }
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
	v2 = (_DWORD *)dword_1002A35C;
	if ( !dword_1002A35C )
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
// 1002A35C: using guessed type int dword_1002A35C;

// ref: 0x1000AA28
_DWORD *__fastcall SelConn_1000AA28(int a1) { return 0; }
/* {
	_DWORD *result; // eax

	result = (_DWORD *)dword_1002A35C;
	while ( result && a1 )
	{
		result = (_DWORD *)*result;
		--a1;
	}
	return result;
} */
// 1002A35C: using guessed type int dword_1002A35C;

// ref: 0x1000AA3B
HWND UNKCALL SelConn_1000AA3B(HWND hWnd) { return 0; }
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
		result = GetDlgItem(result, 1069);
		v3 = result;
		if ( result )
		{
			result = (HWND)GetWindowLongA(result, -21);
			if ( result )
			{
				result = (HWND)*((_DWORD *)result + 3);
				if ( result )
				{
					if ( result == (HWND)dword_1002A35C )
					{
						v4 = GetDlgItem(v2, 1074);
						result = SelConn_1000A866(v4);
					}
					else
					{
						v5 = SelConn_1000A9F3(v3) - 6;
						if ( v5 < 0 )
							v5 = 0;
						result = (HWND)SelConn_1000AA28(v5);
						v6 = (const char *)result;
						if ( result )
						{
							TitleSnd_10010315();
							SelConn_1000A670(v2, v6);
							v7 = GetWindowLongA(hWnda, -12);
							SelConn_1000A226(v2, v7);
							result = SelConn_1000A3E2(v2);
						}
					}
				}
			}
		}
	}
	return result;
} */
// 1002A35C: using guessed type int dword_1002A35C;

// ref: 0x1000AAEB
HWND UNKCALL SelConn_1000AAEB(HWND hWnd) { return 0; }
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
				if ( GetWindowLongA(v1, -12) >= 1074 )
				{
					v3 = GetParent(v1);
					result = GetDlgItem(v3, 1070);
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
								SelConn_1000A670(v5, v4);
								v6 = GetWindowLongA(v1, -12);
								v7 = GetParent(v1);
								SelConn_1000A226(v7, v6);
								v8 = GetParent(v1);
								result = SelConn_1000A3E2(v8);
							}
						}
					}
				}
				else
				{
					result = SelConn_1000A866(v1);
				}
			}
		}
	}
	return result;
} */

// ref: 0x1000AB83
HWND UNKCALL SelConn_1000AB83(HWND hWnd) { return 0; }
/* {
	HWND v1; // edi
	HWND result; // eax
	const char *v3; // ebx
	HWND v4; // eax
	int v5; // ebx
	HWND v6; // eax
	HWND v7; // eax

	v1 = hWnd;
	if ( GetWindowLongA(hWnd, -12) > 1069 )
		return SelConn_1000A8D7(v1);
	result = (HWND)GetWindowLongA(v1, -21);
	if ( result )
	{
		result = (HWND)*((_DWORD *)result + 3);
		if ( result )
		{
			v3 = (const char *)dword_1002A35C;
			if ( result != (HWND)dword_1002A35C )
			{
				while ( v3 && *(HWND *)v3 != result )
					v3 = *(const char **)v3;
				TitleSnd_10010315();
				v4 = GetParent(v1);
				SelConn_1000A670(v4, v3);
				v5 = GetWindowLongA(v1, -12);
				v6 = GetParent(v1);
				SelConn_1000A226(v6, v5);
				v7 = GetParent(v1);
				result = SelConn_1000A3E2(v7);
			}
		}
	}
	return result;
} */
// 1002A35C: using guessed type int dword_1002A35C;

// ref: 0x1000AC07
int __fastcall SelConn_1000AC07(int a1, int a2) { return 0; }
/* {
	int v2; // esi
	int v3; // edi

	v2 = a2;
	v3 = a1;
	if ( a2 == 2 )
		TitleSnd_1001031F();
	Fade_100073B4();
	Fade_100072BE(10);
	return SDlgEndDialog(v3, v2);
} */
// 10010376: using guessed type int __stdcall SDlgEndDialog(_DWORD, _DWORD);

// ref: 0x1000AC30
int UNKCALL SelConn_1000AC30(HWND arg) { return 0; }
/* {
	int v1; // esi
	int v2; // edx
	int result; // eax
	int v4; // eax
	int v5; // ecx
	UINT v6; // [esp-4h] [ebp-8h]

	v1 = (int)arg;
	if ( SelConn_1000AC9E(arg) )
	{
		v2 = 1;
		return SelConn_1000AC07(v1, v2);
	}
	if ( SErrGetLastError() == -2062548873 )
	{
		dword_1002A374 = 1;
		v2 = 0;
		return SelConn_1000AC07(v1, v2);
	}
	result = SelGame_1000B67E();
	switch ( result )
	{
		case 1230002254:
			goto LABEL_17;
		case 1297040461:
			v4 = SErrGetLastError();
			v5 = v1;
			if ( v4 == 1222 )
				v6 = 46;
			else
				v6 = 53;
			goto LABEL_14;
		case 1396916812:
LABEL_17:
			v6 = 44;
			v5 = v1;
LABEL_14:
			result = SelConn_1000AE19(v5, v6);
			break;
	}
	return result;
} */
// 10010406: using guessed type _DWORD __stdcall SErrGetLastError();
// 1002A374: using guessed type int dword_1002A374;

// ref: 0x1000AC9E
int UNKCALL SelConn_1000AC9E(HWND hWnd) { return 0; }
/* {
	HWND v1; // ebx
	HWND v2; // eax
	LONG v3; // eax
	int v4; // esi
	char *v6; // ST10_4
	int v7; // ST08_4
	int v8; // eax
	int v9; // esi
	char *v10; // ST14_4
	int v11; // ST0C_4
	void *v12; // eax
	char v13; // [esp+8h] [ebp-8Ch]
	HWND v14; // [esp+10h] [ebp-84h]
	char v15; // [esp+58h] [ebp-3Ch]

	v1 = hWnd;
	TitleSnd_1001031F();
	SelGame_1000B677(0);
	SelGame_1000B66A(0);
	v2 = GetFocus();
	v3 = GetWindowLongA(v2, -21);
	if ( !v3 )
		return 0;
	v4 = *(_DWORD *)(v3 + 12);
	if ( !v4 )
		return 0;
	SelGame_1000B677(*(void **)(v4 + 8));
	SelGame_1000B66A(*(void **)(v4 + 4));
	if ( SelGame_1000B67E() == 1112425812 )
	{
		BNetGW_10002A07(&unk_10029480);
		BNetGW_100028C2(&unk_10029480);
	}
	if ( dword_1002A364 )
		qmemcpy(&v13, (const void *)dword_1002A364, 0x50u);
	v14 = v1;
	if ( dword_1002A370 )
		qmemcpy(&v15, (const void *)dword_1002A370, 0x3Cu);
	SelConn_1000ADA8(v1);
	v6 = dword_1002A358;
	v7 = dword_1002A34C;
	v8 = SelGame_1000B67E();
	v9 = SNetInitializeProvider(v8, &v15, v7, &v13, v6);
	if ( v9 )
	{
		v10 = dword_1002A358;
		v11 = dword_1002A34C;
		v12 = (void *)SelGame_1000B67E();
		v9 = SelModem_1000E435(v12, (int)&v15, v11, &v13, v10);
	}
	else
	{
		SNetDestroy();
	}
	if ( !v9 )
		SelConn_1000ADD0(v1);
	return v9;
} */
// 10010478: using guessed type _DWORD __stdcall SNetDestroy();
// 1001047E: using guessed type int __stdcall SNetInitializeProvider(_DWORD, _DWORD, _DWORD, _DWORD, _DWORD);
// 1002A364: using guessed type int dword_1002A364;
// 1002A370: using guessed type int dword_1002A370;

// ref: 0x1000ADA8
int UNKCALL SelConn_1000ADA8(HWND hWnd) { return 0; }
/* {
	HWND v1; // esi

	v1 = hWnd;
	Fade_100072BE(10);
	local_1000811B();
	ShowWindow(v1, 0);
	Title_100100E7(v1);
	return Focus_10007818(v1);
} */

// ref: 0x1000ADD0
BOOL UNKCALL SelConn_1000ADD0(HWND hWnd) { return 0; }
/* {
	HWND v1; // esi
	HWND v2; // ST10_4
	HWND v4; // [esp+0h] [ebp-4h]

	v1 = hWnd;
	Focus_100077E9((int)hWnd, "ui_art\\focus16.pcx", v4);
	Title_1001009E(v1, (int)"ui_art\\smlogo.pcx", v2);
	local_100080F1();
	Fade_100073C5(v1, 0);
	PostMessageA(v1, 0x7E8u, 0, 0);
	ShowWindow(v1, 5);
	return UpdateWindow(v1);
} */

// ref: 0x1000AE19
int __fastcall SelConn_1000AE19(int a1, UINT a2) { return 0; }
/* {
	UINT v2; // esi
	int v3; // edi
	int result; // eax
	CHAR Buffer; // [esp+8h] [ebp-80h]

	v2 = a2;
	v3 = a1;
	result = SErrGetLastError();
	if ( result != 1223 )
	{
		LoadStringA(hInstance, v2, &Buffer, 127);
		result = SelYesNo_1000FD39(v3, &Buffer, 0, 0);
	}
	return result;
} */
// 10010406: using guessed type _DWORD __stdcall SErrGetLastError();

// ref: 0x1000AE59
HWND __fastcall SelConn_1000AE59(HWND hWnd, int a2, int a3) { return 0; }
/* {
	int v3; // ebp
	int v4; // ebx
	int v5; // ST0C_4
	HWND v6; // esi
	int v7; // ST08_4
	HWND v8; // eax
	HWND result; // eax
	HWND v10; // eax
	HWND v11; // eax
	HWND v12; // eax
	HWND v13; // eax
	int v14; // eax
	int v15; // eax
	int v16; // eax
	HWND v17; // eax
	HWND v18; // eax
	HWND v19; // eax
	HWND v20; // eax

	v3 = a3;
	v4 = a2;
	v5 = a3;
	v6 = hWnd;
	v7 = a2;
	v8 = GetDlgItem(hWnd, 1056);
	if ( local_10007C3B(v6, v8, v7, v5) )
		return (HWND)SelConn_1000AC30(v6);
	v10 = GetDlgItem(v6, 1054);
	if ( local_10007C3B(v6, v10, v4, v3) )
		return (HWND)SelConn_1000AC07((int)v6, 2);
	if ( dword_1002A354 && (v11 = GetDlgItem(v6, 1145), local_10007C3B(v6, v11, v4, v3)) )
	{
		SelConn_1000ADA8(v6);
		TitleSnd_1001031F();
		UiSelectRegion(&a3);
		result = (HWND)SelConn_1000ADD0(v6);
	}
	else
	{
		v12 = GetDlgItem(v6, 1105);
		result = (HWND)local_10007C3B(v6, v12, v4, v3);
		if ( result )
		{
			v13 = GetDlgItem(v6, 1105);
			v14 = Sbar_100099DC(v13, v4, v3) - 1;
			if ( v14 )
			{
				v15 = v14 - 1;
				if ( v15 )
				{
					v16 = v15 - 1;
					if ( v16 )
					{
						result = (HWND)(v16 - 1);
						if ( !result )
						{
							v17 = GetFocus();
							result = SelConn_1000A948(v17);
						}
					}
					else
					{
						v18 = GetFocus();
						result = SelConn_1000AA3B(v18);
					}
				}
				else
				{
					v19 = GetFocus();
					result = SelConn_1000AAEB(v19);
				}
			}
			else
			{
				v20 = GetFocus();
				result = SelConn_1000AB83(v20);
			}
		}
	}
	return result;
} */
// 1002A354: using guessed type int dword_1002A354;

// ref: 0x1000AF69
int __stdcall UiSelectProvider(int a1, _SNETPROGRAMDATA *client_info, _SNETPLAYERDATA *user_info, _SNETUIDATA *ui_info, _SNETVERSIONDATA *file_info, int *type) { return 0; }
/* {
	int v6; // eax
	int v7; // ebx

	dword_1002A374 = 0;
	dword_1002A36C = a1;
	dword_1002A370 = a2;
	dword_1002A34C = a3;
	dword_1002A364 = a4;
	dword_1002A358 = a5;
	artfont_10001159();
	v6 = SDrawGetFrameWindow();
	v7 = SDlgDialogBoxParam(hInstance, "SELCONNECT_DIALOG", v6, SelConn_1000A0A6, 0);
	if ( a6 )
		*a6 = SelGame_1000B67E();
	if ( dword_1002A374 )
	{
		artfont_100010C8();
		local_100078B6();
		SErrSetLastError(-2062548873);
	}
	else
	{
		if ( v7 == 1 )
		{
			artfont_100010C8();
			local_100078B6();
			return 1;
		}
		SErrSetLastError(1223);
	}
	return 0;
} */
// 10010370: using guessed type int __stdcall SDlgDialogBoxParam(_DWORD, _DWORD, _DWORD, _DWORD, _DWORD);
// 10010382: using guessed type _DWORD __stdcall SDrawGetFrameWindow();
// 1001041E: using guessed type int __stdcall SErrSetLastError(_DWORD);
// 1002A364: using guessed type int dword_1002A364;
// 1002A36C: using guessed type int dword_1002A36C;
// 1002A370: using guessed type int dword_1002A370;
// 1002A374: using guessed type int dword_1002A374;

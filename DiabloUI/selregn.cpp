// ref: 0x1000EF42
void *SelRegn_1000EF42() { return 0; }
/* {
	return SMemAlloc(136, "C:\\Src\\Diablo\\DiabloUI\\SelRegn.cpp", 76, 0);
} */
// 10010364: using guessed type int __stdcall SMemAlloc(_DWORD, _DWORD, _DWORD, _DWORD);

// ref: 0x1000EF56
_uiheroinfo *__fastcall SelRegn_SetNextHero(_uiheroinfo *pNext, _uiheroinfo *pCurrent)
{
	_uiheroinfo *result; // eax

	result = pCurrent;
	pCurrent->next = pNext;
	return result;
}

// ref: 0x1000EF60
signed int SelRegn_1000EF60() { return 0; }
/* {
	signed int result; // eax

	result = 2139095040;
	dword_1002A4F0 = 2139095040;
	return result;
} */
// 1002A4F0: using guessed type int dword_1002A4F0;

// ref: 0x1000EF6B
int __stdcall SelRegn_1000EF6B(HWND hWnd, UINT Msg, WPARAM wParam, LPARAM lParam) { return 0; }
/* {
	HWND v4; // eax
	HWND v6; // eax
	int v7; // [esp+0h] [ebp-Ch]

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
				Fade_100073FD(hWnd, v7);
			return 0;
		}
LABEL_25:
		SelRegn_1000F929(hWnd, (unsigned short)lParam, (unsigned int)lParam >> 16);
		return SDlgDefDialogProc(hWnd, Msg, wParam, lParam);
	}
	if ( Msg == 513 )
		goto LABEL_25;
	if ( Msg == 2 )
	{
		SelRegn_1000F161(hWnd);
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
		SelRegn_1000F1FC(hWnd);
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
				SelRegn_1000F8DD(hWnd);
			}
			else if ( (_WORD)wParam == 2 )
			{
				SelConn_1000AC07((int)hWnd, 2);
			}
			return SDlgDefDialogProc(hWnd, Msg, wParam, lParam);
		}
		Focus_10007458((void *)lParam);
		Focus_100075DC(hWnd, (HWND)lParam);
		SelRegn_1000F0D7(hWnd, (unsigned short)wParam);
LABEL_27:
		SelRegn_1000F109(hWnd);
	}
	return SDlgDefDialogProc(hWnd, Msg, wParam, lParam);
} */
// 1001037C: using guessed type int __stdcall SDlgDefDialogProc(_DWORD, _DWORD, _DWORD, _DWORD);
// 10010382: using guessed type _DWORD __stdcall SDrawGetFrameWindow();

// ref: 0x1000F0D7
HWND __fastcall SelRegn_1000F0D7(HWND hDlg, int nIDDlgItem) { return 0; }
/* {
	HWND v2; // esi
	HWND result; // eax

	v2 = hDlg;
	result = GetDlgItem(hDlg, nIDDlgItem);
	if ( result )
	{
		result = (HWND)GetWindowLongA(result, -21);
		if ( result )
		{
			if ( *((_DWORD *)result + 3) )
				result = (HWND)Doom_10006A13(v2, (int *)&unk_10023250, 1);
		}
	}
	return result;
} */

// ref: 0x1000F109
HWND UNKCALL SelRegn_1000F109(HWND hDlg) { return 0; }
/* {
	HWND v1; // esi
	int v2; // eax

	v1 = hDlg;
	v2 = SelRegn_1000F126();
	return Sbar_10009A99(v1, 1105, dword_1002A4F4, v2);
} */

// ref: 0x1000F126
int SelRegn_1000F126() { return 0; }
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
	v2 = (_DWORD *)dword_1002A4EC;
	if ( !dword_1002A4EC )
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
// 1002A4EC: using guessed type int dword_1002A4EC;

// ref: 0x1000F161
void UNKCALL SelRegn_1000F161(HWND hDlg) { return; }
/* {
	HWND v1; // esi
	_DWORD *v2; // eax

	v1 = hDlg;
	Title_100100E7(hDlg);
	Focus_10007818(v1);
	Sbar_10009CD2(v1, 1105);
	SelRegn_1000F1D4((_DWORD *)dword_1002A4EC);
	Doom_10006C53(v1, &dword_1002326C);
	Doom_10006C53(v1, (int *)&unk_10023260);
	Doom_10006C53(v1, (int *)&unk_10023244);
	Doom_10006C53(v1, (int *)&unk_10023258);
	Doom_10006C53(v1, (int *)&unk_10023250);
	v2 = (_DWORD *)GetWindowLongA(v1, -21);
	local_10007F72(v2);
} */
// 1002326C: using guessed type int dword_1002326C;
// 1002A4EC: using guessed type int dword_1002A4EC;

// ref: 0x1000F1D4
int __fastcall SelRegn_1000F1D4(_DWORD *a1) { return 0; }
/* {
	_DWORD *v1; // esi
	int result; // eax

	if ( a1 )
	{
		do
		{
			v1 = (_DWORD *)*a1;
			result = SelRegn_1000F1E8(a1);
			a1 = v1;
		}
		while ( v1 );
	}
	return result;
} */

// ref: 0x1000F1E8
int UNKCALL SelRegn_1000F1E8(void *arg) { return 0; }
/* {
	int result; // eax

	if ( arg )
		result = SMemFree(arg, "C:\\Src\\Diablo\\DiabloUI\\SelRegn.cpp", 82, 0);
	return result;
} */
// 10010340: using guessed type int __stdcall SMemFree(_DWORD, _DWORD, _DWORD, _DWORD);

// ref: 0x1000F1FC
HWND UNKCALL SelRegn_1000F1FC(HWND hWnd) { return 0; }
/* {
	HWND v1; // esi
	HWND v2; // ST1C_4
	int v3; // eax
	int *v4; // edi
	HWND result; // eax
	HWND v6; // eax
	HWND v7; // [esp+0h] [ebp-Ch]

	v1 = hWnd;
	SelRegn_1000F3C2(hWnd);
	Focus_100077E9((int)v1, "ui_art\\focus16.pcx", v7);
	Title_1001009E(v1, (int)"ui_art\\smlogo.pcx", v2);
	v3 = local_10007F46();
	v4 = (int *)v3;
	if ( v3 )
	{
		SetWindowLongA(v1, -21, v3);
		local_10007944((int)v1, 0, &byte_10029448, -1, 1, (int)"ui_art\\selregn.pcx", v4, v4 + 1, 0);
		Fade_100073C5(v1, 1);
	}
	Doom_100068AB(v1, (int *)&unk_10023250, 1);
	Doom_100068AB(v1, (int *)&unk_10023258, 1);
	Doom_100068AB(v1, (int *)&unk_10023244, 5);
	Doom_1000658C(v1, (int *)&unk_10023260, 4, 0);
	Doom_1000658C(v1, &dword_1002326C, 0, 1);
	dword_1002A4F4 = 0;
	dword_1002A4EC = 0;
	SelRegn_1000F2ED();
	SelRegn_1000F346(v1, (const char *)dword_1002A4EC);
	result = Sbar_10009BF1(v1, 1105);
	if ( dword_1002A4F4 <= 6 )
	{
		v6 = GetDlgItem(v1, 1105);
		result = (HWND)ShowWindow(v6, 0);
	}
	return result;
} */
// 1002326C: using guessed type int dword_1002326C;
// 1002A4EC: using guessed type int dword_1002A4EC;

// ref: 0x1000F2ED
signed int SelRegn_1000F2ED() { return 0; }
/* {
	signed int i; // edi
	char *v1; // eax
	char *v2; // esi
	const char *v3; // eax
	_DWORD *v4; // eax

	for ( i = dword_10029488; ; --i )
	{
		if ( i <= 0 )
			return 1;
		v1 = (char *)SelRegn_1000EF42();
		v2 = v1;
		if ( !v1 )
			break;
		*(_DWORD *)v1 = 0;
		*((_DWORD *)v1 + 1) = i;
		v3 = BNetGW_10002B21(&unk_10029480, i);
		strcpy(v2 + 8, v3);
		v4 = SelRegn_1000EF56(dword_1002A4EC, v2);
		++dword_1002A4F4;
		dword_1002A4EC = (int)v4;
	}
	return 0;
} */
// 10029488: using guessed type int dword_10029488;
// 1002A4EC: using guessed type int dword_1002A4EC;

// ref: 0x1000F346
int __fastcall SelRegn_1000F346(HWND a1, const char *a2) { return 0; }
/* {
	const char *v2; // edi
	int *v3; // ebx
	HWND v4; // eax
	HWND v5; // esi
	int v6; // eax
	HWND hDlg; // [esp+8h] [ebp-4h]

	v2 = a2;
	hDlg = a1;
	v3 = &dword_1002326C;
	if ( dword_1002326C )
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
						local_10007FA4(v6, v2 + 8);
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
	return Doom_1000680A(hDlg, &dword_1002326C, 0, 1);
} */
// 1002326C: using guessed type int dword_1002326C;

// ref: 0x1000F3C2
void UNKCALL SelRegn_1000F3C2(HWND hDlg) { return; }
/* {
	HWND v1; // ebx
	int *v2; // edi
	HWND v3; // eax
	HWND v4; // esi
	void *v5; // eax

	v1 = hDlg;
	v2 = &dword_1002326C;
	if ( dword_1002326C )
	{
		do
		{
			v3 = GetDlgItem(v1, *v2);
			v4 = v3;
			if ( v3 )
			{
				v5 = (void *)GetWindowLongA(v3, -4);
				SetPropA(v4, "UIOLDPROC", v5);
				SetWindowLongA(v4, -4, (LONG)SelRegn_1000F414);
			}
			++v2;
		}
		while ( *v2 );
	}
} */
// 1002326C: using guessed type int dword_1002326C;

// ref: 0x1000F414
LRESULT __stdcall SelRegn_1000F414(HWND hWnd, UINT Msg, WPARAM wParam, LPARAM lParam) { return 0; }
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
					SelRegn_1000F61E(hWnd);
				}
				else if ( wParam > 0x24 )
				{
					if ( wParam <= 0x26 )
					{
						SelRegn_1000F859(hWnd);
					}
					else if ( wParam <= 0x28 )
					{
						SelRegn_1000F7C1(hWnd);
					}
				}
				return 0;
			}
			if ( wParam == 33 )
			{
				SelRegn_1000F711(hWnd);
				return 0;
			}
			if ( wParam == 9 )
			{
				if ( GetKeyState(16) >= 0 )
					SelRegn_1000F53C(hWnd);
				else
					SelRegn_1000F5AD(hWnd);
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

// ref: 0x1000F53C
HWND UNKCALL SelRegn_1000F53C(HWND hWnd) { return 0; }
/* {
	HWND v1; // edi
	HWND v2; // ebx
	int nIDDlgItem[1132]; // [esp+0h] [ebp-11D4h]
	int v5; // [esp+11B0h] [ebp-24h]
	int v6; // [esp+11BCh] [ebp-18h]
	int v7; // [esp+11C0h] [ebp-14h]
	int v8; // [esp+11C4h] [ebp-10h]
	int v9; // [esp+11C8h] [ebp-Ch]
	int v10; // [esp+11CCh] [ebp-8h]
	int v11; // [esp+11D0h] [ebp-4h]

	v1 = hWnd;
	v6 = 1136;
	v7 = 1137;
	v8 = 1138;
	v9 = 1139;
	v10 = 1140;
	v11 = 1135;
	v2 = GetParent(hWnd);
	do
	{
		v5 = nIDDlgItem[GetWindowLongA(v1, -12)];
		v1 = GetDlgItem(v2, v5);
	}
	while ( !IsWindowEnabled(v1) );
	return SetFocus(v1);
} */
// 1000F53C: using guessed type int nIDDlgItem[1132];

// ref: 0x1000F5AD
HWND UNKCALL SelRegn_1000F5AD(HWND hWnd) { return 0; }
/* {
	HWND v1; // edi
	HWND v2; // ebx
	int nIDDlgItem[1132]; // [esp+0h] [ebp-11D4h]
	int v5; // [esp+11B0h] [ebp-24h]
	int v6; // [esp+11BCh] [ebp-18h]
	int v7; // [esp+11C0h] [ebp-14h]
	int v8; // [esp+11C4h] [ebp-10h]
	int v9; // [esp+11C8h] [ebp-Ch]
	int v10; // [esp+11CCh] [ebp-8h]
	int v11; // [esp+11D0h] [ebp-4h]

	v1 = hWnd;
	v6 = 1140;
	v7 = 1135;
	v8 = 1136;
	v9 = 1137;
	v10 = 1138;
	v11 = 1139;
	v2 = GetParent(hWnd);
	do
	{
		v5 = nIDDlgItem[GetWindowLongA(v1, -12)];
		v1 = GetDlgItem(v2, v5);
	}
	while ( !IsWindowEnabled(v1) );
	return SetFocus(v1);
} */
// 1000F5AD: using guessed type int nIDDlgItem[1132];

// ref: 0x1000F61E
HWND UNKCALL SelRegn_1000F61E(HWND hWnd) { return 0; }
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
		result = GetDlgItem(result, 1135);
		v4 = result;
		if ( result )
		{
			v5 = GetDlgItem(v3, 1140);
			result = (HWND)GetWindowLongA(v5, -21);
			if ( result )
			{
				v6 = (_DWORD *)*((_DWORD *)result + 3);
				if ( v6 && *v6 )
				{
					v7 = SelRegn_1000F6C9(v4) + 6;
					if ( v7 > dword_1002A4F4 - 6 )
						v7 = dword_1002A4F4 - 6;
					result = (HWND)SelRegn_1000F6FE(v7);
					v8 = (const char *)result;
					if ( result )
					{
						TitleSnd_10010315();
						SelRegn_1000F346(v3, v8);
						v9 = GetWindowLongA(v1, -12);
						SelRegn_1000F0D7(v3, v9);
						result = SelRegn_1000F109(v3);
					}
				}
				else
				{
					result = SelRegn_1000F5AD(v4);
				}
			}
		}
	}
	return result;
} */

// ref: 0x1000F6C9
int UNKCALL SelRegn_1000F6C9(HWND hWnd) { return 0; }
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
	v2 = (_DWORD *)dword_1002A4EC;
	if ( !dword_1002A4EC )
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
// 1002A4EC: using guessed type int dword_1002A4EC;

// ref: 0x1000F6FE
_DWORD *__fastcall SelRegn_1000F6FE(int a1) { return 0; }
/* {
	_DWORD *result; // eax

	result = (_DWORD *)dword_1002A4EC;
	while ( result && a1 )
	{
		result = (_DWORD *)*result;
		--a1;
	}
	return result;
} */
// 1002A4EC: using guessed type int dword_1002A4EC;

// ref: 0x1000F711
HWND UNKCALL SelRegn_1000F711(HWND hWnd) { return 0; }
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
		result = GetDlgItem(result, 1135);
		v3 = result;
		if ( result )
		{
			result = (HWND)GetWindowLongA(result, -21);
			if ( result )
			{
				result = (HWND)*((_DWORD *)result + 3);
				if ( result )
				{
					if ( result == (HWND)dword_1002A4EC )
					{
						v4 = GetDlgItem(v2, 1140);
						result = SelRegn_1000F53C(v4);
					}
					else
					{
						v5 = SelRegn_1000F6C9(v3) - 6;
						if ( v5 < 0 )
							v5 = 0;
						result = (HWND)SelRegn_1000F6FE(v5);
						v6 = (const char *)result;
						if ( result )
						{
							TitleSnd_10010315();
							SelRegn_1000F346(v2, v6);
							v7 = GetWindowLongA(hWnda, -12);
							SelRegn_1000F0D7(v2, v7);
							result = SelRegn_1000F109(v2);
						}
					}
				}
			}
		}
	}
	return result;
} */
// 1002A4EC: using guessed type int dword_1002A4EC;

// ref: 0x1000F7C1
HWND UNKCALL SelRegn_1000F7C1(HWND hWnd) { return 0; }
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
				if ( GetWindowLongA(v1, -12) >= 1140 )
				{
					v3 = GetParent(v1);
					result = GetDlgItem(v3, 1136);
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
								SelRegn_1000F346(v5, v4);
								v6 = GetWindowLongA(v1, -12);
								v7 = GetParent(v1);
								SelRegn_1000F0D7(v7, v6);
								v8 = GetParent(v1);
								result = SelRegn_1000F109(v8);
							}
						}
					}
				}
				else
				{
					result = SelRegn_1000F53C(v1);
				}
			}
		}
	}
	return result;
} */

// ref: 0x1000F859
HWND UNKCALL SelRegn_1000F859(HWND hWnd) { return 0; }
/* {
	HWND v1; // edi
	HWND result; // eax
	const char *v3; // ebx
	HWND v4; // eax
	int v5; // ebx
	HWND v6; // eax
	HWND v7; // eax

	v1 = hWnd;
	if ( GetWindowLongA(hWnd, -12) > 1135 )
		return SelRegn_1000F5AD(v1);
	result = (HWND)GetWindowLongA(v1, -21);
	if ( result )
	{
		result = (HWND)*((_DWORD *)result + 3);
		if ( result )
		{
			v3 = (const char *)dword_1002A4EC;
			if ( result != (HWND)dword_1002A4EC )
			{
				while ( v3 && *(HWND *)v3 != result )
					v3 = *(const char **)v3;
				TitleSnd_10010315();
				v4 = GetParent(v1);
				SelRegn_1000F346(v4, v3);
				v5 = GetWindowLongA(v1, -12);
				v6 = GetParent(v1);
				SelRegn_1000F0D7(v6, v5);
				v7 = GetParent(v1);
				result = SelRegn_1000F109(v7);
			}
		}
	}
	return result;
} */
// 1002A4EC: using guessed type int dword_1002A4EC;

// ref: 0x1000F8DD
signed int UNKCALL SelRegn_1000F8DD(void *arg) { return 0; }
/* {
	int v1; // esi
	signed int result; // eax

	v1 = (int)arg;
	result = SelRegn_1000F8F6();
	if ( result )
		result = SelConn_1000AC07(v1, 1);
	return result;
} */

// ref: 0x1000F8F6
signed int SelRegn_1000F8F6() { return 0; }
/* {
	HWND v0; // eax
	LONG v1; // eax
	int v2; // eax

	TitleSnd_1001031F();
	v0 = GetFocus();
	v1 = GetWindowLongA(v0, -21);
	if ( !v1 )
		return 0;
	v2 = *(_DWORD *)(v1 + 12);
	if ( !v2 )
		return 0;
	BNetGW_10002B51(&unk_10029480, *(_DWORD *)(v2 + 4));
	return 1;
} */

// ref: 0x1000F929
HWND __fastcall SelRegn_1000F929(HWND hWnd, int a2, int a3) { return 0; }
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
	v4 = hWnd;
	v5 = a2;
	v6 = GetDlgItem(hWnd, 1056);
	if ( local_10007C3B(v4, v6, v5, a3) )
		return (HWND)SelRegn_1000F8DD(v4);
	v8 = GetDlgItem(v4, 1054);
	if ( local_10007C3B(v4, v8, v3, a3) )
		return (HWND)SelConn_1000AC07((int)v4, 2);
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
						result = SelRegn_1000F61E(v14);
					}
				}
				else
				{
					v15 = GetFocus();
					result = SelRegn_1000F711(v15);
				}
			}
			else
			{
				v16 = GetFocus();
				result = SelRegn_1000F7C1(v16);
			}
		}
		else
		{
			v17 = GetFocus();
			result = SelRegn_1000F859(v17);
		}
	}
	return result;
} */

// ref: 0x1000F9F7
signed int __stdcall UiSelectRegion(_DWORD *a1) { return 0; }
/* {
	int v1; // eax
	int v2; // eax
	signed int result; // eax

	artfont_10001159();
	v1 = SDrawGetFrameWindow();
	v2 = SDlgDialogBoxParam(hInstance, "SELREGION_DIALOG", v1, SelRegn_1000EF6B, 0);
	if ( a1 )
		*a1 = dword_1002948C;
	if ( v2 == 1 )
	{
		local_100078B6();
		result = 1;
	}
	else
	{
		SErrSetLastError(1223);
		result = 0;
	}
	return result;
} */
// 10010370: using guessed type int __stdcall SDlgDialogBoxParam(_DWORD, _DWORD, _DWORD, _DWORD, _DWORD);
// 10010382: using guessed type _DWORD __stdcall SDrawGetFrameWindow();
// 1001041E: using guessed type int __stdcall SErrSetLastError(_DWORD);
// 1002948C: using guessed type int dword_1002948C;

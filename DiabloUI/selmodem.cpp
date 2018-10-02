// ref: 0x1000E42A
signed int SelModem_1000E42A() { return 0; }
/* {
	signed int result; // eax

	result = 2139095040;
	dword_1002A4CC = 2139095040;
	return result;
} */
// 1002A4CC: using guessed type int dword_1002A4CC;

// ref: 0x1000E435
int __fastcall SelModem_1000E435(void *a1, int a2, int a3, char *a4, char *a5) { return 0; }
/* {
	void *v5; // edi

	dword_1002A4DC = 0;
	dword_1002A4D8 = 0;
	dword_1002A4E8 = a3;
	dword_1002A4D4 = a4;
	v5 = a1;
	dword_1002A4E0 = a2;
	dword_1002A4D0 = (int)a5;
	SNetEnumDevices(SelModem_1000E497);
	if ( !dword_1002A4D8 )
		return SelModem_1000E505(v5);
	if ( dword_1002A4D8 == 1 )
		return SelModem_1000E51E();
	return SelModem_1000E5CC();
} */
// 10010496: using guessed type int __stdcall SNetEnumDevices(_DWORD);
// 1002A4D0: using guessed type int dword_1002A4D0;
// 1002A4DC: using guessed type int dword_1002A4DC;
// 1002A4E0: using guessed type int dword_1002A4E0;
// 1002A4E8: using guessed type int dword_1002A4E8;

// ref: 0x1000E497
char *__stdcall SelModem_1000E497(int a1, char *a2, char *a3) { return 0; }
/* {
	int result; // eax
	int v4; // esi
	_DWORD *v5; // eax

	result = SelModem_1000E4EC();
	v4 = result;
	if ( result )
	{
		*(_DWORD *)result = 0;
		*(_DWORD *)(result + 4) = a1;
		strcpy((char *)(result + 8), a2);
		strcpy((char *)(v4 + 136), a3);
		v5 = SelModem_1000E500(dword_1002A4DC, (_DWORD *)v4);
		++dword_1002A4D8;
		dword_1002A4DC = (int)v5;
		result = 1;
	}
	return result;
} */
// 1002A4DC: using guessed type int dword_1002A4DC;

// ref: 0x1000E4EC
void *SelModem_1000E4EC() { return 0; }
/* {
	return SMemAlloc(264, "C:\\Src\\Diablo\\DiabloUI\\SelModem.cpp", 72, 0);
} */
// 10010364: using guessed type int __stdcall SMemAlloc(_DWORD, _DWORD, _DWORD, _DWORD);

// ref: 0x1000E500
_DWORD *__fastcall SelModem_1000E500(int a1, _DWORD *a2) { return 0; }
/* {
	_DWORD *result; // eax

	result = a2;
	*a2 = a1;
	return result;
} */

// ref: 0x1000E505
signed int UNKCALL SelModem_1000E505(void *arg) { return 0; }
/* {
	if ( arg != (void *)1297040461 )
		return 1;
	SErrSetLastError(1222);
	return 0;
} */
// 1001041E: using guessed type int __stdcall SErrSetLastError(_DWORD);

// ref: 0x1000E51E
signed int SelModem_1000E51E() { return 0; }
/* {
	signed int result; // eax

	if ( SelModem_1000E57B(*((_DWORD *)dword_1002A4D4 + 2), *(_DWORD *)(dword_1002A4DC + 4)) )
	{
		SelModem_1000E553((_DWORD *)dword_1002A4DC);
		result = 1;
	}
	else
	{
		SErrSetLastError(-2062548879);
		result = 0;
	}
	return result;
} */
// 1001041E: using guessed type int __stdcall SErrSetLastError(_DWORD);
// 1002A4DC: using guessed type int dword_1002A4DC;

// ref: 0x1000E553
int __fastcall SelModem_1000E553(_DWORD *a1) { return 0; }
/* {
	_DWORD *v1; // esi
	int result; // eax

	if ( a1 )
	{
		do
		{
			v1 = (_DWORD *)*a1;
			result = SelModem_1000E567(a1);
			a1 = v1;
		}
		while ( v1 );
	}
	return result;
} */

// ref: 0x1000E567
int UNKCALL SelModem_1000E567(void *arg) { return 0; }
/* {
	int result; // eax

	if ( arg )
		result = SMemFree(arg, "C:\\Src\\Diablo\\DiabloUI\\SelModem.cpp", 77, 0);
	return result;
} */
// 10010340: using guessed type int __stdcall SMemFree(_DWORD, _DWORD, _DWORD, _DWORD);

// ref: 0x1000E57B
int __fastcall SelModem_1000E57B(int a1, int a2) { return 0; }
/* {
	int v2; // ebx
	int v3; // esi
	int v5; // [esp+8h] [ebp-50h]
	int v6; // [esp+10h] [ebp-48h]
	int (__stdcall *v7)(char *, int, int, int, int); // [esp+30h] [ebp-28h]

	v2 = a2;
	v3 = a1;
	memcpy(&v5, dword_1002A4D4, 0x50u);
	v7 = ModmStat_10008C62;
	v5 = 80;
	v6 = v3;
	return SNetInitializeDevice(v2, dword_1002A4E0, dword_1002A4E8, &v5, dword_1002A4D0);
} */
// 1001049C: using guessed type int __stdcall SNetInitializeDevice(_DWORD, _DWORD, _DWORD, _DWORD, _DWORD);
// 1002A4D0: using guessed type int dword_1002A4D0;
// 1002A4E0: using guessed type int dword_1002A4E0;
// 1002A4E8: using guessed type int dword_1002A4E8;

// ref: 0x1000E5CC
signed int SelModem_1000E5CC() { return 0; }
/* {
	signed int v0; // esi
	signed int result; // eax

	v0 = 1;
	if ( SDlgDialogBoxParam(hInstance, "SELMODEM_DIALOG", *((_DWORD *)dword_1002A4D4 + 2), SelModem_1000E63E, 0) == 1 )
	{
		if ( !SelModem_1000E57B(*((_DWORD *)dword_1002A4D4 + 2), dword_1002A4E4) )
		{
			SErrSetLastError(-2062548879);
			v0 = 0;
		}
		SelModem_1000E553((_DWORD *)dword_1002A4DC);
		result = v0;
	}
	else
	{
		SelModem_1000E553((_DWORD *)dword_1002A4DC);
		SErrSetLastError(1223);
		result = 0;
	}
	return result;
} */
// 10010370: using guessed type int __stdcall SDlgDialogBoxParam(_DWORD, _DWORD, _DWORD, _DWORD, _DWORD);
// 1001041E: using guessed type int __stdcall SErrSetLastError(_DWORD);
// 1002A4DC: using guessed type int dword_1002A4DC;
// 1002A4E4: using guessed type int dword_1002A4E4;

// ref: 0x1000E63E
int __stdcall SelModem_1000E63E(HWND hDlg, UINT Msg, WPARAM wParam, LPARAM lParam) { return 0; }
/* {
	int v4; // edx
	HWND v5; // eax
	HWND v7; // eax
	int v8; // [esp+0h] [ebp-Ch]

	if ( Msg > 0x201 )
	{
		if ( Msg == 514 )
		{
			v7 = GetDlgItem(hDlg, 1105);
			if ( !Sbar_100099C0(v7) )
				return SDlgDefDialogProc(hDlg, Msg, wParam, lParam);
LABEL_27:
			SelModem_1000E7E9(hDlg);
			return SDlgDefDialogProc(hDlg, Msg, wParam, lParam);
		}
		if ( Msg != 515 )
		{
			if ( Msg != 2024 )
				return SDlgDefDialogProc(hDlg, Msg, wParam, lParam);
			if ( !Fade_1000739F() )
				Fade_100073FD(hDlg, v8);
			return 0;
		}
LABEL_25:
		SelModem_1000EE78(hDlg, (unsigned short)lParam, (unsigned int)lParam >> 16);
		return SDlgDefDialogProc(hDlg, Msg, wParam, lParam);
	}
	if ( Msg == 513 )
		goto LABEL_25;
	if ( Msg == 2 )
	{
		SelModem_1000E783(hDlg);
		return SDlgDefDialogProc(hDlg, Msg, wParam, lParam);
	}
	if ( Msg <= 0x103 )
		return SDlgDefDialogProc(hDlg, Msg, wParam, lParam);
	if ( Msg <= 0x105 )
	{
		v5 = (HWND)SDrawGetFrameWindow();
		SendMessageA(v5, Msg, wParam, lParam);
		return SDlgDefDialogProc(hDlg, Msg, wParam, lParam);
	}
	if ( Msg == 272 )
	{
		SelModem_1000E843(hDlg);
		return 0;
	}
	if ( Msg == 273 )
	{
		if ( HIWORD(wParam) == 7 )
		{
			Focus_100075B7(hDlg, (HWND)lParam);
			return SDlgDefDialogProc(hDlg, Msg, wParam, lParam);
		}
		if ( HIWORD(wParam) == 6 )
		{
			Focus_10007458((void *)lParam);
			Focus_100075DC(hDlg, (HWND)lParam);
			goto LABEL_27;
		}
		v4 = 1;
		if ( wParam != 327681 )
		{
			if ( (_WORD)wParam != 2 )
				return SDlgDefDialogProc(hDlg, Msg, wParam, lParam);
			v4 = 2;
		}
		SelModem_1000EE29((int)hDlg, v4);
	}
	return SDlgDefDialogProc(hDlg, Msg, wParam, lParam);
} */
// 1001037C: using guessed type int __stdcall SDlgDefDialogProc(_DWORD, _DWORD, _DWORD, _DWORD);
// 10010382: using guessed type _DWORD __stdcall SDrawGetFrameWindow();

// ref: 0x1000E783
void UNKCALL SelModem_1000E783(HWND hDlg) { return; }
/* {
	HWND v1; // esi
	_DWORD *v2; // eax

	v1 = hDlg;
	Sbar_10009CD2(hDlg, 1105);
	Doom_10006C53(v1, &dword_100231F4);
	Doom_10006C53(v1, (int *)&unk_100231E8);
	Doom_10006C53(v1, (int *)&unk_100231CC);
	Doom_10006C53(v1, (int *)&unk_100231D4);
	Doom_10006C53(v1, (int *)&unk_100231E0);
	Focus_10007818(v1);
	local_1000811B();
	v2 = (_DWORD *)GetWindowLongA(v1, -21);
	local_10007F72(v2);
} */
// 100231F4: using guessed type int dword_100231F4;

// ref: 0x1000E7E9
HWND UNKCALL SelModem_1000E7E9(HWND hDlg) { return 0; }
/* {
	HWND v1; // esi
	HWND v2; // eax
	int v3; // eax

	v1 = hDlg;
	v2 = GetFocus();
	v3 = SelModem_1000E80E(v2);
	return Sbar_10009A99(v1, 1105, dword_1002A4D8, v3);
} */

// ref: 0x1000E80E
int UNKCALL SelModem_1000E80E(HWND hWnd) { return 0; }
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
	v2 = (_DWORD *)dword_1002A4DC;
	if ( !dword_1002A4DC )
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
// 1002A4DC: using guessed type int dword_1002A4DC;

// ref: 0x1000E843
HWND UNKCALL SelModem_1000E843(HWND hWnd) { return 0; }
/* {
	HWND v1; // esi
	int v2; // eax
	int *v3; // edi
	HWND result; // eax
	HWND v5; // eax
	HWND v6; // [esp+0h] [ebp-Ch]

	v1 = hWnd;
	GetParent(hWnd);
	SelModem_1000E9B2(v1);
	Focus_100077E9((int)v1, "ui_art\\focus16.pcx", v6);
	local_100080F1();
	v2 = local_10007F46();
	v3 = (int *)v2;
	if ( v2 )
	{
		SetWindowLongA(v1, -21, v2);
		local_10007944((int)v1, 0, "popup", -1, 1, (int)"ui_art\\seldiff.pcx", v3, v3 + 1, 0);
	}
	Fade_100073C5(v1, 0);
	PostMessageA(v1, 0x7E8u, 0, 0);
	Doom_100068AB(v1, (int *)&unk_100231E0, 1);
	Doom_100068AB(v1, (int *)&unk_100231D4, 3);
	Doom_100068AB(v1, (int *)&unk_100231CC, 5);
	Doom_1000658C(v1, (int *)&unk_100231E8, 4, 0);
	Doom_1000658C(v1, &dword_100231F4, 0, 1);
	SelModem_1000E932(v1, (const char *)dword_1002A4DC);
	result = Sbar_10009BF1(v1, 1105);
	if ( dword_1002A4D8 <= 6 )
	{
		v5 = GetDlgItem(v1, 1105);
		result = (HWND)ShowWindow(v5, 0);
	}
	return result;
} */
// 100231F4: using guessed type int dword_100231F4;
// 1002A4DC: using guessed type int dword_1002A4DC;

// ref: 0x1000E932
int __fastcall SelModem_1000E932(HWND a1, const char *a2) { return 0; }
/* {
	const char *v2; // edi
	int *v3; // ebp
	HWND v4; // eax
	HWND v5; // esi
	int v6; // esi
	HWND hDlg; // [esp+8h] [ebp-4h]

	v2 = a2;
	hDlg = a1;
	v3 = &dword_100231F4;
	if ( dword_100231F4 )
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
					local_10007FA4(v6, v2 + 8);
					if ( v6 )
						*(_DWORD *)(v6 + 12) = v2;
					v2 = *(const char **)v2;
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
	return Doom_1000680A(hDlg, &dword_100231F4, 0, 1);
} */
// 100231F4: using guessed type int dword_100231F4;

// ref: 0x1000E9B2
void UNKCALL SelModem_1000E9B2(HWND hDlg) { return; }
/* {
	HWND v1; // ebx
	int *v2; // edi
	HWND v3; // eax
	HWND v4; // esi
	void *v5; // eax

	v1 = hDlg;
	v2 = &dword_100231F4;
	if ( dword_100231F4 )
	{
		do
		{
			v3 = GetDlgItem(v1, *v2);
			v4 = v3;
			if ( v3 )
			{
				v5 = (void *)GetWindowLongA(v3, -4);
				SetPropA(v4, "UIOLDPROC", v5);
				SetWindowLongA(v4, -4, (LONG)SelModem_1000EA04);
			}
			++v2;
		}
		while ( *v2 );
	}
} */
// 100231F4: using guessed type int dword_100231F4;

// ref: 0x1000EA04
LRESULT __stdcall SelModem_1000EA04(HWND hWnd, UINT Msg, WPARAM wParam, LPARAM lParam) { return 0; }
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
					SelModem_1000EC0E(hWnd);
				}
				else if ( wParam > 0x24 )
				{
					if ( wParam <= 0x26 )
					{
						SelModem_1000EDBC(hWnd);
					}
					else if ( wParam <= 0x28 )
					{
						SelModem_1000ED3B(hWnd);
					}
				}
				return 0;
			}
			if ( wParam == 33 )
			{
				SelModem_1000ECB2(hWnd);
				return 0;
			}
			if ( wParam == 9 )
			{
				if ( GetKeyState(16) >= 0 )
					SelModem_1000EB2C(hWnd);
				else
					SelModem_1000EB9D(hWnd);
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

// ref: 0x1000EB2C
HWND UNKCALL SelModem_1000EB2C(HWND hWnd) { return 0; }
/* {
	HWND v1; // edi
	HWND v2; // ebx
	int nIDDlgItem[1107]; // [esp+0h] [ebp-1170h]
	int v5; // [esp+114Ch] [ebp-24h]
	int v6; // [esp+1158h] [ebp-18h]
	int v7; // [esp+115Ch] [ebp-14h]
	int v8; // [esp+1160h] [ebp-10h]
	int v9; // [esp+1164h] [ebp-Ch]
	int v10; // [esp+1168h] [ebp-8h]
	int v11; // [esp+116Ch] [ebp-4h]

	v1 = hWnd;
	v6 = 1111;
	v7 = 1112;
	v8 = 1113;
	v9 = 1114;
	v10 = 1115;
	v11 = 1110;
	v2 = GetParent(hWnd);
	do
	{
		v5 = nIDDlgItem[GetWindowLongA(v1, -12)];
		v1 = GetDlgItem(v2, v5);
	}
	while ( !IsWindowEnabled(v1) );
	return SetFocus(v1);
} */
// 1000EB2C: using guessed type int nIDDlgItem[1107];

// ref: 0x1000EB9D
HWND UNKCALL SelModem_1000EB9D(HWND hWnd) { return 0; }
/* {
	HWND v1; // edi
	HWND v2; // ebx
	int nIDDlgItem[1107]; // [esp+0h] [ebp-1170h]
	int v5; // [esp+114Ch] [ebp-24h]
	int v6; // [esp+1158h] [ebp-18h]
	int v7; // [esp+115Ch] [ebp-14h]
	int v8; // [esp+1160h] [ebp-10h]
	int v9; // [esp+1164h] [ebp-Ch]
	int v10; // [esp+1168h] [ebp-8h]
	int v11; // [esp+116Ch] [ebp-4h]

	v1 = hWnd;
	v6 = 1115;
	v7 = 1110;
	v8 = 1111;
	v9 = 1112;
	v10 = 1113;
	v11 = 1114;
	v2 = GetParent(hWnd);
	do
	{
		v5 = nIDDlgItem[GetWindowLongA(v1, -12)];
		v1 = GetDlgItem(v2, v5);
	}
	while ( !IsWindowEnabled(v1) );
	return SetFocus(v1);
} */
// 1000EB9D: using guessed type int nIDDlgItem[1107];

// ref: 0x1000EC0E
HWND UNKCALL SelModem_1000EC0E(HWND hWnd) { return 0; }
/* {
	HWND result; // eax
	HWND v2; // edi
	HWND v3; // ebx
	HWND v4; // eax
	_DWORD *v5; // eax
	int v6; // eax
	const char *v7; // esi

	result = GetParent(hWnd);
	v2 = result;
	if ( result )
	{
		result = GetDlgItem(result, 1110);
		v3 = result;
		if ( result )
		{
			v4 = GetDlgItem(v2, 1115);
			result = (HWND)GetWindowLongA(v4, -21);
			if ( result )
			{
				v5 = (_DWORD *)*((_DWORD *)result + 3);
				if ( v5 && *v5 )
				{
					v6 = SelModem_1000E80E(v3) + 6;
					if ( v6 > dword_1002A4D8 - 6 )
						v6 = dword_1002A4D8 - 6;
					result = (HWND)SelModem_1000EC9F(v6);
					v7 = (const char *)result;
					if ( result )
					{
						TitleSnd_10010315();
						SelModem_1000E932(v2, v7);
						result = SelModem_1000E7E9(v2);
					}
				}
				else
				{
					result = SelModem_1000EB9D(v3);
				}
			}
		}
	}
	return result;
} */

// ref: 0x1000EC9F
_DWORD *__fastcall SelModem_1000EC9F(int a1) { return 0; }
/* {
	_DWORD *result; // eax

	result = (_DWORD *)dword_1002A4DC;
	while ( result && a1 )
	{
		result = (_DWORD *)*result;
		--a1;
	}
	return result;
} */
// 1002A4DC: using guessed type int dword_1002A4DC;

// ref: 0x1000ECB2
HWND UNKCALL SelModem_1000ECB2(HWND hWnd) { return 0; }
/* {
	HWND result; // eax
	HWND v2; // edi
	HWND v3; // esi
	HWND v4; // eax
	int v5; // eax
	const char *v6; // esi

	result = GetParent(hWnd);
	v2 = result;
	if ( result )
	{
		result = GetDlgItem(result, 1110);
		v3 = result;
		if ( result )
		{
			result = (HWND)GetWindowLongA(result, -21);
			if ( result )
			{
				result = (HWND)*((_DWORD *)result + 3);
				if ( result )
				{
					if ( result == (HWND)dword_1002A4DC )
					{
						v4 = GetDlgItem(v2, 1115);
						result = SelModem_1000EB2C(v4);
					}
					else
					{
						v5 = SelModem_1000E80E(v3) - 6;
						if ( v5 < 0 )
							v5 = 0;
						result = (HWND)SelModem_1000EC9F(v5);
						v6 = (const char *)result;
						if ( result )
						{
							TitleSnd_10010315();
							SelModem_1000E932(v2, v6);
							result = SelModem_1000E7E9(v2);
						}
					}
				}
			}
		}
	}
	return result;
} */
// 1002A4DC: using guessed type int dword_1002A4DC;

// ref: 0x1000ED3B
HWND UNKCALL SelModem_1000ED3B(HWND hWnd) { return 0; }
/* {
	HWND v1; // edi
	HWND result; // eax
	HWND v3; // eax
	const char *v4; // esi
	HWND v5; // eax
	HWND v6; // eax

	v1 = hWnd;
	result = (HWND)GetWindowLongA(hWnd, -21);
	if ( result )
	{
		result = (HWND)*((_DWORD *)result + 3);
		if ( result )
		{
			if ( *(_DWORD *)result )
			{
				if ( GetWindowLongA(v1, -12) >= 1115 )
				{
					v3 = GetParent(v1);
					result = GetDlgItem(v3, 1111);
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
								SelModem_1000E932(v5, v4);
								v6 = GetParent(v1);
								result = SelModem_1000E7E9(v6);
							}
						}
					}
				}
				else
				{
					result = SelModem_1000EB2C(v1);
				}
			}
		}
	}
	return result;
} */

// ref: 0x1000EDBC
HWND UNKCALL SelModem_1000EDBC(HWND hWnd) { return 0; }
/* {
	HWND v1; // ebx
	HWND result; // eax
	const char *v3; // esi
	HWND v4; // eax
	HWND v5; // eax

	v1 = hWnd;
	if ( GetWindowLongA(hWnd, -12) > 1110 )
		return SelModem_1000EB9D(v1);
	result = (HWND)GetWindowLongA(v1, -21);
	if ( result )
	{
		result = (HWND)*((_DWORD *)result + 3);
		if ( result )
		{
			v3 = (const char *)dword_1002A4DC;
			if ( result != (HWND)dword_1002A4DC )
			{
				while ( v3 && *(HWND *)v3 != result )
					v3 = *(const char **)v3;
				TitleSnd_10010315();
				v4 = GetParent(v1);
				SelModem_1000E932(v4, v3);
				v5 = GetParent(v1);
				result = SelModem_1000E7E9(v5);
			}
		}
	}
	return result;
} */
// 1002A4DC: using guessed type int dword_1002A4DC;

// ref: 0x1000EE29
int __fastcall SelModem_1000EE29(int a1, int a2) { return 0; }
/* {
	int v2; // esi
	int v3; // edi
	HWND v4; // eax
	LONG v5; // eax
	int v6; // eax

	v2 = a2;
	v3 = a1;
	TitleSnd_1001031F();
	if ( v2 == 1 )
	{
		v4 = GetFocus();
		if ( v4 )
		{
			v5 = GetWindowLongA(v4, -21);
			if ( v5 )
			{
				v6 = *(_DWORD *)(v5 + 12);
				if ( v6 )
					dword_1002A4E4 = *(_DWORD *)(v6 + 4);
			}
		}
	}
	Fade_100073B4();
	Fade_100072BE(10);
	return SDlgEndDialog(v3, v2);
} */
// 10010376: using guessed type int __stdcall SDlgEndDialog(_DWORD, _DWORD);
// 1002A4E4: using guessed type int dword_1002A4E4;

// ref: 0x1000EE78
HWND __fastcall SelModem_1000EE78(HWND hWnd, int a2, int a3) { return 0; }
/* {
	int v3; // ebx
	HWND v4; // esi
	int v5; // ST08_4
	HWND v6; // eax
	int v7; // edx
	HWND result; // eax
	HWND v9; // eax
	HWND v10; // eax
	HWND v11; // eax
	int v12; // eax
	int v13; // eax
	int v14; // eax
	HWND v15; // eax
	HWND v16; // eax
	HWND v17; // eax
	HWND v18; // eax

	v3 = a2;
	v4 = hWnd;
	v5 = a2;
	v6 = GetDlgItem(hWnd, 1056);
	if ( local_10007C3B(v4, v6, v5, a3) )
	{
		v7 = 1;
		return (HWND)SelModem_1000EE29((int)v4, v7);
	}
	v9 = GetDlgItem(v4, 1054);
	if ( local_10007C3B(v4, v9, v3, a3) )
	{
		v7 = 2;
		return (HWND)SelModem_1000EE29((int)v4, v7);
	}
	v10 = GetDlgItem(v4, 1105);
	result = (HWND)local_10007C3B(v4, v10, v3, a3);
	if ( result )
	{
		v11 = GetDlgItem(v4, 1105);
		v12 = Sbar_100099DC(v11, v3, a3) - 1;
		if ( v12 )
		{
			v13 = v12 - 1;
			if ( v13 )
			{
				v14 = v13 - 1;
				if ( v14 )
				{
					result = (HWND)(v14 - 1);
					if ( !result )
					{
						v15 = GetFocus();
						result = SelModem_1000EC0E(v15);
					}
				}
				else
				{
					v16 = GetFocus();
					result = SelModem_1000ECB2(v16);
				}
			}
			else
			{
				v17 = GetFocus();
				result = SelModem_1000ED3B(v17);
			}
		}
		else
		{
			v18 = GetFocus();
			result = SelModem_1000EDBC(v18);
		}
	}
	return result;
} */

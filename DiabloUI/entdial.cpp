// ref: 0x10006C96
int __stdcall EntDial_10006C96(HWND hDlg, UINT Msg, WPARAM wParam, LPARAM lParam) { return 0; }
/* {
	int v4; // edx
	HWND v5; // eax
	HWND v7; // eax
	int savedregs; // [esp+Ch] [ebp+0h]

	v4 = 2;
	if ( Msg == 2 )
	{
		EntDial_10006D78(hDlg);
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
			if ( Msg == 272 )
			{
				lpString = (LPSTR)lParam;
				EntDial_10006DB8(hDlg, (int)&savedregs);
				return 0;
			}
			if ( Msg != 273 )
			{
				if ( Msg != 275 )
				{
					if ( Msg == 513 )
						EntDial_10006F16(hDlg, (unsigned short)lParam, (unsigned int)lParam >> 16);
					return SDlgDefDialogProc(hDlg, Msg, wParam, lParam);
				}
				v5 = GetFocus();
				Focus_100075DC(hDlg, v5);
				return 0;
			}
			if ( (unsigned short)wParam == 1 )
			{
				v4 = 1;
			}
			else if ( (unsigned short)wParam != 2 )
			{
				if ( (unsigned short)wParam == 1116 )
					EntDial_10006EE8(hDlg, wParam, lParam);
				return SDlgDefDialogProc(hDlg, Msg, wParam, lParam);
			}
			EntDial_10006EA7(hDlg, v4);
		}
	}
	return SDlgDefDialogProc(hDlg, Msg, wParam, lParam);
} */
// 1001037C: using guessed type int __stdcall SDlgDefDialogProc(_DWORD, _DWORD, _DWORD, _DWORD);
// 10010382: using guessed type _DWORD __stdcall SDrawGetFrameWindow();

// ref: 0x10006D78
HWND UNKCALL EntDial_10006D78(HWND hDlg) { return 0; }
/* {
	HWND v1; // esi
	HWND v2; // eax

	v1 = hDlg;
	Focus_100076C3();
	Doom_10006C53(v1, (int *)&unk_10022B10);
	Doom_10006C53(v1, (int *)&unk_10022B04);
	Doom_10006C53(v1, (int *)&unk_10022AFC);
	v2 = GetParent(v1);
	return Modem_10008563(v2, 0, 0);
} */

// ref: 0x10006DB8
HWND USERCALL EntDial_10006DB8(HWND hWnd, int a2) { return 0; }
/* {
	HWND v2; // esi
	HWND v3; // eax
	LONG v4; // eax
	HWND v5; // ebx
	HWND v6; // eax
	int v8; // [esp-138h] [ebp-144h]
	int v9; // [esp-38h] [ebp-44h]
	int v10; // [esp+8h] [ebp-4h]

	v2 = hWnd;
	v3 = GetParent(hWnd);
	v4 = GetWindowLongA(v3, -21);
	SetWindowLongA(v2, -21, v4);
	Doom_100068AB(v2, (int *)&unk_10022AFC, 5);
	Doom_1000658C(v2, (int *)&unk_10022B04, 4, 0);
	Doom_10006AB8(v2, (int *)&unk_10022B10, 2);
	Focus_10007719("ui_art\\focus.pcx");
	SDlgSetTimer(v2, 1, 55, 0);
	v5 = GetDlgItem(v2, 1116);
	SendMessageA(v5, 0x401u, 0x1Fu, 0);
	SendMessageA(v5, 0x404u, 0, (LPARAM)"<>%&?");
	v10 = a2;
	LoadStringA(hInstance, 0x3Bu, (LPSTR)&v9, 63);
	LoadStringA(hInstance, 0x3Au, (LPSTR)&v8, 255);
	v6 = GetParent(v2);
	return Modem_10008563(v6, (const char *)&v9, (int)&v8);
} */
// 10006DB8: could not find valid save-restore pair for ebp
// 10010412: using guessed type int __stdcall SDlgSetTimer(_DWORD, _DWORD, _DWORD, _DWORD);

// ref: 0x10006EA7
int __fastcall EntDial_10006EA7(HWND hDlg, int a2) { return 0; }
/* {
	int v2; // edi
	HWND v3; // esi
	CHAR *v4; // ST08_4
	HWND v5; // eax

	v2 = a2;
	v3 = hDlg;
	TitleSnd_1001031F();
	SDlgKillTimer(v3, 1);
	v4 = lpString;
	v5 = GetDlgItem(v3, 1116);
	GetWindowTextA(v5, v4, 32);
	lpString[31] = 0;
	return SDlgEndDialog(v3, v2);
} */
// 10010376: using guessed type int __stdcall SDlgEndDialog(_DWORD, _DWORD);
// 10010418: using guessed type int __stdcall SDlgKillTimer(_DWORD, _DWORD);

// ref: 0x10006EE8
void __fastcall EntDial_10006EE8(HWND hWnd, unsigned int a2, int a3) { return; }
/* {
	int v3; // edx
	HWND v4; // esi
	HWND v5; // eax

	v3 = (a2 >> 16) - 1;
	v4 = hWnd;
	if ( v3 )
	{
		if ( v3 == 2 )
			Doom_10006C08(hWnd, (int *)&unk_10022B10, 2);
	}
	else
	{
		v5 = GetFocus();
		Focus_100075DC(v4, v5);
	}
} */

// ref: 0x10006F16
int __fastcall EntDial_10006F16(HWND hDlg, int a2, int a3) { return 0; }
/* {
	int v3; // ebx
	HWND v4; // esi
	int v5; // ST08_4
	HWND v6; // eax
	int v7; // edx
	HWND v8; // eax
	int result; // eax

	v3 = a2;
	v4 = hDlg;
	v5 = a2;
	v6 = GetDlgItem(hDlg, 1056);
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
	return EntDial_10006EA7(v4, v7);
} */

// ref: 0x10006F71
signed int EntDial_10006F71() { return 0; }
/* {
	signed int result; // eax

	result = 2139095040;
	dword_1002985C = 2139095040;
	return result;
} */
// 1002985C: using guessed type int dword_1002985C;

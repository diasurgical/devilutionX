// ref: 0x10008C62
int __stdcall ModmStat_10008C62(char *a1, int a2, int a3, int a4, int a5) { return 0; }
/* {
	int result; // eax

	strcpy(&byte_1002A264, a1);
	result = 1;
	dword_1002A258 = 1;
	dword_1002A260 = (int (*)(void))a5;
	return result;
} */
// 1002A258: using guessed type int dword_1002A258;
// 1002A260: using guessed type int (*dword_1002A260)(void);

// ref: 0x10008C87
int UNKCALL ModmStat_10008C87(void *arg) { return 0; }
/* {
	return SDlgDialogBoxParam(hInstance, "MODMSTAT_DIALOG", arg, ModmStat_10008CA0, 0);
} */
// 10010370: using guessed type int __stdcall SDlgDialogBoxParam(_DWORD, _DWORD, _DWORD, _DWORD, _DWORD);

// ref: 0x10008CA0
int __stdcall ModmStat_10008CA0(HWND hWnd, UINT Msg, WPARAM wParam, LPARAM lParam) { return 0; }
/* {
	HWND v4; // eax
	HWND v6; // eax

	if ( Msg == 2 )
	{
		ModmStat_10008DB3(hWnd);
	}
	else if ( Msg > 0x103 )
	{
		if ( Msg <= 0x105 )
		{
			v6 = (HWND)SDrawGetFrameWindow();
			SendMessageA(v6, Msg, wParam, lParam);
		}
		else
		{
			switch ( Msg )
			{
				case 0x110u:
					ModmStat_10008DE4(hWnd);
					return 0;
				case 0x111u:
					if ( HIWORD(wParam) == 7 )
					{
						Focus_100075B7(hWnd, (HWND)lParam);
					}
					else if ( HIWORD(wParam) == 6 )
					{
						Focus_10007458((void *)lParam);
						Focus_100075DC(hWnd, (HWND)lParam);
					}
					else if ( (_WORD)wParam == 1 || (_WORD)wParam == 2 )
					{
						ModmStat_10008E89((int)hWnd, 1);
					}
					break;
				case 0x113u:
					if ( dword_1002A258 )
						ModmStat_10008EBF(hWnd);
					if ( Modem_10008653() )
					{
						dword_1002A25C = 1;
						ModmStat_10008E89((int)hWnd, 0);
					}
					v4 = GetFocus();
					Focus_100075DC(hWnd, v4);
					return 0;
			}
		}
	}
	return SDlgDefDialogProc(hWnd, Msg, wParam, lParam);
} */
// 1001037C: using guessed type int __stdcall SDlgDefDialogProc(_DWORD, _DWORD, _DWORD, _DWORD);
// 10010382: using guessed type _DWORD __stdcall SDrawGetFrameWindow();
// 1002A258: using guessed type int dword_1002A258;
// 1002A25C: using guessed type int dword_1002A25C;

// ref: 0x10008DB3
int UNKCALL ModmStat_10008DB3(HWND hDlg) { return 0; }
/* {
	HWND v1; // esi
	_DWORD *v2; // eax

	v1 = hDlg;
	v2 = (_DWORD *)GetWindowLongA(hDlg, -21);
	local_10007F72(v2);
	Focus_100076C3();
	Doom_10006C53(v1, (int *)&unk_10022CB4);
	return Doom_10006C53(v1, (int *)&unk_10022CAC);
} */

// ref: 0x10008DE4
BOOL UNKCALL ModmStat_10008DE4(HWND hWnd) { return 0; }
/* {
	HWND v1; // esi
	int v2; // eax
	int *v3; // edi
	HWND v4; // eax
	BOOL result; // eax

	v1 = hWnd;
	v2 = local_10007F46();
	v3 = (int *)v2;
	if ( v2 )
	{
		SetWindowLongA(v1, -21, v2);
		local_10007944((int)v1, 0, "Popup", -1, 1, (int)"ui_art\\black.pcx", v3, v3 + 1, 1);
	}
	Doom_100068AB(v1, (int *)&unk_10022CAC, 3);
	Doom_1000658C(v1, (int *)&unk_10022CB4, 4, 1);
	Focus_10007719("ui_art\\focus.pcx");
	SDlgSetTimer(v1, 1, 55, 0);
	local_10007DE9(v1, (int *)&unk_10022CB4);
	byte_1002A264 = 0;
	dword_1002A258 = 0;
	dword_1002A260 = 0;
	v4 = GetDlgItem(v1, 2);
	result = ShowWindow(v4, 0);
	dword_1002A25C = 0;
	return result;
} */
// 10010412: using guessed type int __stdcall SDlgSetTimer(_DWORD, _DWORD, _DWORD, _DWORD);
// 1002A258: using guessed type int dword_1002A258;
// 1002A25C: using guessed type int dword_1002A25C;
// 1002A260: using guessed type int (*dword_1002A260)(void);

// ref: 0x10008E89
int __fastcall ModmStat_10008E89(int a1, int a2) { return 0; }
/* {
	int v2; // edi
	int v3; // esi
	int result; // eax

	v2 = a2;
	v3 = a1;
	if ( dword_1002A25C )
	{
		TitleSnd_1001031F();
		SDlgKillTimer(v3, 1);
		if ( v2 )
		{
			if ( dword_1002A260 )
				dword_1002A260();
		}
		result = SDlgEndDialog(v3, 0);
	}
	return result;
} */
// 1002A260: invalid function type has been ignored
// 10010376: using guessed type int __stdcall SDlgEndDialog(_DWORD, _DWORD);
// 10010418: using guessed type int __stdcall SDlgKillTimer(_DWORD, _DWORD);
// 1002A25C: using guessed type int dword_1002A25C;
// 1002A260: using guessed type int (*dword_1002A260)(void);

// ref: 0x10008EBF
void UNKCALL ModmStat_10008EBF(HWND hDlg) { return; }
/* {
	HWND v1; // edi
	HWND v2; // eax
	int v3; // eax
	HWND v4; // eax

	dword_1002A258 = 0;
	v1 = hDlg;
	if ( dword_1002A260 )
	{
		v2 = GetDlgItem(hDlg, 1026);
		if ( v2 )
		{
			v3 = GetWindowLongA(v2, -21);
			local_10007FA4(v3, &byte_1002A264);
			Doom_10006A13(v1, (int *)&unk_10022CAC, 3);
			v4 = GetDlgItem(v1, 2);
			ShowWindow(v4, 1);
			dword_1002A25C = 1;
		}
	}
} */
// 1002A258: using guessed type int dword_1002A258;
// 1002A25C: using guessed type int dword_1002A25C;
// 1002A260: using guessed type int (*dword_1002A260)(void);

// ref: 0x10008F26
signed int ModmStat_10008F26() { return 0; }
/* {
	signed int result; // eax

	result = 2139095040;
	dword_1002A254 = 2139095040;
	return result;
} */
// 1002A254: using guessed type int dword_1002A254;

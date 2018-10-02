// ref: 0x10004C33
void __fastcall CreaDung_SetDelSpin(int a1)
{
	creadung_delspinners = a1;
}
// 100296CC: using guessed type int creadung_delspinners;

// ref: 0x10004C3F
void __cdecl CreaDung_cpp_init()
{
	CreaDung_cpp_float = CreaDung_cpp_float_value;
}
// 1001F400: using guessed type int CreaDung_cpp_float_value;
// 100296C4: using guessed type int CreaDung_cpp_float;

// ref: 0x10004C4A
LRESULT __stdcall CreaDung_WndProc(HWND hWnd, UINT Msg, WPARAM wParam, LPARAM lParam)
{
	HWND v4; // eax
	HWND v6; // eax

	if ( Msg == 2 )
	{
		CreaDung_FreeDungProcs(hWnd);
	}
	else if ( Msg > 0x103 )
	{
		if ( Msg > 0x105 )
		{
			if ( Msg == 272 )
			{
				creadung_dword_100296D8 = lParam;
				CreaDung_LoadDungGFX(hWnd);
			}
			else
			{
				if ( Msg == 273 )
				{
					if ( HIWORD(wParam) == 7 )
					{
						Focus_GetAndBlitSpin(hWnd, lParam);
					}
					else if ( HIWORD(wParam) == 6 )
					{
						Focus_CheckPlayMove(lParam);
						Focus_DoBlitSpinIncFrame(hWnd, (HWND)lParam);
						CreaDung_ParseDungProcs(hWnd, (unsigned short)wParam);
					}
					else if ( HIWORD(wParam) == 5 || (_WORD)wParam == 1 )
					{
						CreaDung_DoAllPlaySnd(hWnd);
					}
					else if ( (_WORD)wParam == 2 )
					{
						CreaDung_PlaySndAndKill(hWnd, 2);
					}
					return (LRESULT)SDlgDefDialogProc(hWnd, Msg, (HDC)wParam, (HWND)lParam);
				}
				if ( Msg != 275 )
				{
					if ( Msg == 513 )
						CreaDung_CheckDlgForSnd(hWnd, (unsigned short)lParam, (unsigned int)lParam >> 16);
					return (LRESULT)SDlgDefDialogProc(hWnd, Msg, (HDC)wParam, (HWND)lParam);
				}
				if ( wParam == 1 )
				{
					v4 = GetFocus();
					Focus_DoBlitSpinIncFrame(hWnd, v4);
				}
			}
			return 0;
		}
		v6 = (HWND)SDrawGetFrameWindow();
		SendMessageA(v6, Msg, wParam, lParam);
	}
	return (LRESULT)SDlgDefDialogProc(hWnd, Msg, (HDC)wParam, (HWND)lParam);
}
// 10010382: using guessed type _DWORD __stdcall SDrawGetFrameWindow();
// 100296D8: using guessed type int creadung_dword_100296D8;

// ref: 0x10004D75
void __fastcall CreaDung_ParseDungProcs(HWND hWnd, int dlg)
{
	HWND v3; // eax
	int v4; // eax
	HWND v5; // eax
	int v6; // eax
	char Buffer[256]; // [esp+4h] [ebp-100h]

	LoadStringA(ghUiInst, dlg - 1055, Buffer, 255);
	v3 = GetDlgItem(hWnd, 1097);
	if ( v3 )
	{
		v4 = GetWindowLongA(v3, -21);
		local_SetWndLongStr(v4, Buffer);
	}
	Doom_ParseWndProc4(hWnd, creadung_msgtbl2, 5);
	LoadStringA(ghUiInst, dlg - 65, Buffer, 255);
	v5 = GetDlgItem(hWnd, 1099);
	if ( v5 )
	{
		v6 = GetWindowLongA(v5, -21);
		local_SetWndLongStr(v6, Buffer);
	}
	Doom_ParseWndProc4(hWnd, creadung_msgtbl3, 1);
}

// ref: 0x10004E2E
void __fastcall CreaDung_FreeDungProcs(HWND hWnd)
{
	void **v2; // eax

	Doom_DeleteFreeProcs(hWnd, creadung_msgtbl5);
	Doom_DeleteFreeProcs(hWnd, creadung_msgtbl4);
	Doom_DeleteFreeProcs(hWnd, creadung_msgtbl3);
	Doom_DeleteFreeProcs(hWnd, creadung_msgtbl2);
	Doom_DeleteFreeProcs(hWnd, creadung_msgtbl1);
	v2 = (void **)GetWindowLongA(hWnd, -21);
	local_FreeMemPtr(v2);
	if ( creadung_delspinners )
		Focus_DeleteSpinners();
}
// 100296CC: using guessed type int creadung_delspinners;

// ref: 0x10004E8B
void __fastcall CreaDung_LoadDungGFX(HWND hWnd)
{
	DWORD *v2; // eax MAPDST

	if ( creadung_delspinners )
		Focus_LoadSpinner("ui_art\\focus16.pcx");
	else
		Focus_ResetSpinToZero();
	SDlgSetTimer((int)hWnd, 1, 55, 0);
	v2 = local_AllocWndLongData();
	if ( v2 )
	{
		SetWindowLongA(hWnd, -21, (LONG)v2);
		local_LoadArtWithPal(hWnd, 0, "popup", -1, 1, "ui_art\\seldiff.pcx", (BYTE **)v2, v2 + 1, 0);
	}
	local_DoUiWndProc(hWnd, (DWORD *)creadung_msgtbl5);
	Doom_ParseWndProc3(hWnd, creadung_msgtbl1, 5);
	Doom_ParseWndProc3(hWnd, creadung_msgtbl2, 5);
	Doom_ParseWndProc3(hWnd, creadung_msgtbl3, 1);
	Doom_ParseWndProcs(hWnd, creadung_msgtbl4, 4, 0);
	Doom_ParseWndProcs(hWnd, creadung_msgtbl5, 2, 1);
}
// 100296CC: using guessed type int creadung_delspinners;

// ref: 0x10004F40
void __fastcall CreaDung_PlaySndAndKill(HWND hWnd, int a2)
{
	TitleSnd_PlaySelectSound();
	SDlgKillTimer((int)hWnd, 1);
	SDlgEndDialog(hWnd, (HANDLE)a2);
}

// ref: 0x10004F5D
void __fastcall CreaDung_DoAllPlaySnd(HWND hWnd)
{
	//int v1; // ebp
	HWND v2; // esi
	HWND v3; // eax
	LONG v4; // eax
	HWND v5; // edi
	HWND v6; // eax
	int v7; // [esp-288h] [ebp-28Ch]
	int v8; // [esp-188h] [ebp-18Ch]
	char *v9; // [esp-108h] [ebp-10Ch]
	char *v10; // [esp-88h] [ebp-8Ch]
	int v11; // [esp-8h] [ebp-Ch]
	char v12; // [esp-4h] [ebp-8h]
	//int v13; // [esp+0h] [ebp-4h]

	/* note: stack is hosed, fix me */
	v2 = hWnd;
	if ( creadung_dword_100296D8 == 1 )
	{
		//v13 = v1;
		v5 = hWnd;
		Connect_CopyPlrDescStrings((char *)&v10, 128, (char *)&v9, 128);
		v6 = GetFocus();
		v12 = GetWindowLongA(v6, -12) - 70;
		Connect_SetDiffString((_gamedata *)&v11, (const char *)&v10, (char *)&v9, (char *)&v8, 128);
		if ( UiAuthCallback(2, (char *)&v10, (char *)&v9, 0, (char *)&v8, (LPSTR)&v7, 256) )
			CreaDung_DoSnetCreaGame(v5);
		else
			SelYesNo_SelOkDialog(v5, (char *)&v7, 0, 0);
	}
	else
	{
		v3 = GetFocus();
		v4 = GetWindowLongA(v3, -12);
		SelHero_SetHeroDifficulty(v4 - 1094);
		CreaDung_PlaySndAndKill(v2, 1);
	}
}
// 10004F5D: could not find valid save-restore pair for ebp
// 100296D8: using guessed type int creadung_dword_100296D8;

// ref: 0x10005037
void __fastcall CreaDung_DoSnetCreaGame(HWND hWnd)
{
	HWND v1; // ebx
	DWORD *v2; // eax
	DWORD v3; // edi
	char a4[256]; // [esp+8h] [ebp-34Ch]
	char v5[128]; // [esp+108h] [ebp-24Ch]
	char Buffer[192]; // [esp+188h] [ebp-1CCh]
	char a3[128]; // [esp+248h] [ebp-10Ch]
	char a2[128]; // [esp+2C8h] [ebp-8Ch]
	_gamedata a1; // [esp+348h] [ebp-Ch]

	v1 = GetFocus();
	if ( hWnd == GetParent(v1) )
	{
		Connect_CopyPlrDescStrings(a2, 128, a3, 128);
		a1.bDiff = GetWindowLongA(v1, -12) - 70;
		Connect_SetDiffString(&a1, a2, a3, a4, 256);
		v2 = crea_somegamestruct;
		if ( crea_somegamestruct[8] >= 8 )
		{
			v3 = crea_somegamestruct[7];
			*(_BYTE *)(v3 + 4) = GetWindowLongA(v1, -12) - 70;
			v2 = crea_somegamestruct;
		}
		if ( SNetCreateGame(
				 creadung_gamename,
				 0,
				 a4,
				 0,
				 (char *)v2[7],
				 v2[8],
				 *(_DWORD *)(creadung_playername + 8),
				 a2,
				 0,
				 creadung_playerID) )
		{
			CreaDung_PlaySndAndKill(hWnd, 1);
		}
		else
		{
			creadung_lasterror = SErrGetLastError();
			if ( creadung_lasterror == 183 )
			{
				LoadStringA(ghUiInst, 0x49u, v5, 127);
				wsprintfA(Buffer, v5, creadung_gamename);
			}
			else
			{
				LoadStringA(ghUiInst, 0x26u, Buffer, 191);
			}
			SelYesNo_SelOkDialog(hWnd, Buffer, 0, 0);
		}
	}
}
// 10010406: using guessed type _DWORD __stdcall SErrGetLastError();
// 100296BC: using guessed type int creadung_playername;
// 100296D4: using guessed type int creadung_lasterror;

// ref: 0x1000517E
void __fastcall CreaDung_CheckDlgForSnd(HWND hWnd, int a2, int a3)
{
	HWND v6; // eax
	HWND v7; // eax

	v6 = GetDlgItem(hWnd, 1056);
	if ( local_GetBottomRect(hWnd, v6, a2, a3) )
	{
		CreaDung_DoAllPlaySnd(hWnd);
	}
	else
	{
		v7 = GetDlgItem(hWnd, 1054);
		if ( local_GetBottomRect(hWnd, v7, a2, a3) )
			CreaDung_PlaySndAndKill(hWnd, 2);
	}
}

// ref: 0x100051D8
BOOL __fastcall CreaDung_SelDungDiff(int a1, int a2, int a3, int a4, int a5, int a6, int a7, int a8)
{
	int v8; // ST10_4
	BOOL result; // eax

	creadung_playername = a1;
	creadung_dword_100296C8 = a3;
	creadung_playerID = (int *)a6;
	creadung_delspinners = a7;
	crea_somegamestruct = (DWORD *)a2;
	creadung_gamename = (char *)a8;
	v8 = SelHero_GetHeroIsGood();
	result = SDlgDialogBoxParam(ghUiInst, "SELDIFF_DIALOG", *(_DWORD *)(a4 + 8), CreaDung_WndProc, v8);
	if ( result != 1 )
	{
		SErrSetLastError(creadung_lasterror);
		result = 0;
	}
	return result;
}
// 1001041E: using guessed type int __stdcall SErrSetLastError(_DWORD);
// 100296BC: using guessed type int creadung_playername;
// 100296C8: using guessed type int creadung_dword_100296C8;
// 100296CC: using guessed type int creadung_delspinners;
// 100296D4: using guessed type int creadung_lasterror;

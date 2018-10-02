// ref: 0x10009D66
LRESULT __stdcall SelClass_WndProc(HWND hWnd, UINT Msg, WPARAM wParam, LPARAM lParam)
{
	HWND v4; // eax
	int v5; // edx
	HWND v6; // eax
	HWND v7; // eax
	HWND v9; // eax

	if ( Msg == 2 )
	{
		SelClass_FreeClassMsgTbl(hWnd);
		return (LRESULT)SDlgDefDialogProc(hWnd, Msg, (HDC)wParam, (HWND)lParam);
	}
	if ( Msg <= 0x103 )
		return (LRESULT)SDlgDefDialogProc(hWnd, Msg, (HDC)wParam, (HWND)lParam);
	if ( Msg <= 0x105 )
	{
		v9 = (HWND)SDrawGetFrameWindow();
		SendMessageA(v9, Msg, wParam, lParam);
		return (LRESULT)SDlgDefDialogProc(hWnd, Msg, (HDC)wParam, (HWND)lParam);
	}
	if ( Msg == 272 )
	{
		SelClass_LoadClassFocus(hWnd);
		return 0;
	}
	if ( Msg != 273 )
	{
		if ( Msg != 275 )
		{
			if ( Msg == 513 )
			{
				v4 = GetDlgItem(hWnd, 1056);
				if ( local_GetBottomRect(hWnd, v4, (unsigned short)lParam, (unsigned int)lParam >> 16) )
				{
					v5 = 1;
LABEL_19:
					SelClass_CheckClassSpawn(hWnd, v5);
					return (LRESULT)SDlgDefDialogProc(hWnd, Msg, (HDC)wParam, (HWND)lParam);
				}
				v6 = GetDlgItem(hWnd, 1054);
				if ( local_GetBottomRect(hWnd, v6, (unsigned short)lParam, (unsigned int)lParam >> 16) )
				{
LABEL_21:
					v5 = 2;
					goto LABEL_19;
				}
			}
			return (LRESULT)SDlgDefDialogProc(hWnd, Msg, (HDC)wParam, (HWND)lParam);
		}
		v7 = GetFocus();
		Focus_DoBlitSpinIncFrame(hWnd, v7);
		return 0;
	}
	if ( HIWORD(wParam) == 7 )
	{
		Focus_GetAndBlitSpin(hWnd, lParam);
	}
	else
	{
		if ( HIWORD(wParam) != 6 )
		{
			v5 = 1;
			if ( HIWORD(wParam) == 5 || (_WORD)wParam == 1 )
				goto LABEL_19;
			if ( (_WORD)wParam != 2 )
				return (LRESULT)SDlgDefDialogProc(hWnd, Msg, (HDC)wParam, (HWND)lParam);
			goto LABEL_21;
		}
		Focus_CheckPlayMove(lParam);
		Focus_DoBlitSpinIncFrame(hWnd, (HWND)lParam);
		SelClass_SetDefaultStats(hWnd, (unsigned short)wParam);
	}
	return (LRESULT)SDlgDefDialogProc(hWnd, Msg, (HDC)wParam, (HWND)lParam);
}
// 10010382: using guessed type _DWORD __stdcall SDrawGetFrameWindow();

// ref: 0x10009EC0
void __fastcall SelClass_FreeClassMsgTbl(HWND hWnd)
{
	HWND v2; // eax

	Focus_DeleteSpinners();
	Doom_DeleteFreeProcs(hWnd, selclass_msgtbl3);
	Doom_DeleteFreeProcs(hWnd, selclass_msgtbl2);
	Doom_DeleteFreeProcs(hWnd, selclass_msgtbl1);
	v2 = GetParent(hWnd);
	SelHero_SetStringWithMsg(v2, 0);
}

// ref: 0x10009EFD
void __fastcall SelClass_LoadClassFocus(HWND hWnd)
{
	HWND v1; // edi
	HWND v2; // esi
	LONG v3; // eax
	char Buffer[32]; // [esp+8h] [ebp-20h]

	v1 = hWnd;
	v2 = GetParent(hWnd);
	if ( SelHero_GetHeroIsGood() == 1 )
		LoadStringA(ghUiInst, 0x20u, Buffer, 31);
	else
		LoadStringA(ghUiInst, 0x1Fu, Buffer, 31);
	SelHero_SetStringWithMsg(v2, Buffer);
	v3 = GetWindowLongA(v2, -21);
	SetWindowLongA(v1, -21, v3);
	local_DoUiWndProc(v1, (DWORD *)selclass_msgtbl3);
	Doom_ParseWndProc3(v1, selclass_msgtbl1, 5);
	Doom_ParseWndProcs(v1, selclass_msgtbl2, 4, 0);
	Doom_ParseWndProcs(v1, selclass_msgtbl3, 2, 1);
	Focus_LoadSpinner("ui_art\\focus.pcx");
	SDlgSetTimer((int)v1, 1, 55, 0);
}

// ref: 0x10009FA2
void __fastcall SelClass_SetDefaultStats(HWND hWnd, int a2)
{
	char v2; // bl
	HWND v4; // eax
	_uiheroinfo pInfo; // [esp+8h] [ebp-34h]
	_uidefaultstats a2a; // [esp+34h] [ebp-8h]

	v2 = a2;
	SelHero_SetClassStats(a2 - 1062, &a2a);
	memset(&pInfo, 0, 0x2Cu);
	pInfo.strength = a2a.strength;
	pInfo.magic = a2a.magic;
	pInfo.dexterity = a2a.dexterity;
	pInfo.vitality = a2a.vitality;
	pInfo.level = 1;
	pInfo.heroclass = v2 - 38;
	v4 = GetParent(hWnd);
	SelHero_PrintHeroInfo(v4, &pInfo);
}

// ref: 0x1000A00D
void __fastcall SelClass_CheckClassSpawn(HWND hWnd, int a2)
{
	HWND v4; // eax
	HWND v5; // eax

	if ( DiabloUI_GetSpawned() && a2 == 1 && (v4 = GetFocus(), GetWindowLongA(v4, -12) != 1062) )
	{
		SelYesNo_SpawnErrDialog(hWnd, 69, 0);
	}
	else
	{
		TitleSnd_PlaySelectSound();
		SDlgKillTimer((int)hWnd, 1);
		if ( a2 == 1 )
		{
			v5 = GetFocus();
			a2 = GetWindowLongA(v5, -12);
		}
		SDlgEndDialog(hWnd, (HANDLE)a2);
	}
}

// ref: 0x1000A077
void __cdecl SelClass_cpp_init()
{
	SelClass_cpp_float = SelClass_cpp_float_value;
}
// 1001F450: using guessed type int SelClass_cpp_float_value;
// 1002A348: using guessed type int SelClass_cpp_float;

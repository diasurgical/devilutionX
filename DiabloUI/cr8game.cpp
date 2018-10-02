// ref: 0x10004339
void __cdecl cr8game_cpp_init()
{
	cr8game_cpp_float = cr8game_cpp_float_value;
}
// 1001F3FC: using guessed type int cr8game_cpp_float_value;
// 1002962C: using guessed type int cr8game_cpp_float;

// ref: 0x10004344
BOOL __fastcall cr8game_GetSnetCreaGame(HWND hWnd)
{
	BOOL result; // eax
	DWORD *v2; // eax
	int *v3; // ST24_4
	int v4; // ST18_4
	int v5; // ST14_4
	char *v6; // ST10_4
	int v7; // eax
	char Buffer[128]; // [esp+Ch] [ebp-308h]
	char Text[256]; // [esp+8Ch] [ebp-288h]
	char a4[128]; // [esp+18Ch] [ebp-188h]
	char a2[128]; // [esp+20Ch] [ebp-108h]
	char str[128]; // [esp+28Ch] [ebp-88h]
	BOOL v13; // [esp+30Ch] [ebp-8h]

	Connect_CopyPlrDescStrings(a2, 128, str, 128);
	Connect_SetDiffString(&cr8_gamedata, a2, str, a4, 128);
	if ( UiAuthCallback(2, a2, str, 0, a4, Text, 256) )
	{
		v2 = cr8_somegamestruct;
		if ( cr8_somegamestruct[8] >= 8 )
		{
			*(_BYTE *)(cr8_somegamestruct[7] + 4) = cr8_gamedata.bDiff;
			v2 = cr8_somegamestruct;
		}
		v3 = cr8game_playerID;
		v4 = *(_DWORD *)(cr8_playercount + 8);
		v5 = v2[8];
		v6 = (char *)v2[7];
		v7 = Connect_GetRankFromLevel(str);
		v13 = SNetCreateGame(cr8_gamename, cr8_gamepassword, a4, v7, v6, v5, v4, a2, 0, v3);
		if ( !v13 )
		{
			if ( SErrGetLastError() == 183 )
			{
				LoadStringA(ghUiInst, 0x40Fu, Buffer, 128);
				sprintf(Text, Buffer, cr8_gamename);
			}
			else
			{
				LoadStringA(ghUiInst, 0x410u, Text, 256);
			}
			UiMessageBoxCallback(hWnd, Text, 0, 0x30u);
		}
		result = v13;
	}
	else
	{
		UiMessageBoxCallback(hWnd, Text, 0, 0x30u);
		result = 0;
	}
	return result;
}
// 10010406: using guessed type _DWORD __stdcall SErrGetLastError();
// 10029630: using guessed type int cr8_playercount;

// ref: 0x100044AA
BOOL __stdcall UiCreateGameCallback(int a1, int a2, int a3, int a4, int a5, int a6)
{
	BOOL v6; // eax

	cr8_playercount = a1;
	cr8_somegamestruct = (DWORD *)a2;
	cr8_dword_10029638 = a3;
	cr8_dword_10029640 = a5;
	cr8_dword_1002963C = a4;
	cr8game_playerID = (int *)a6;
	v6 = SDlgDialogBoxParam(ghUiInst, "DIALOG_CREATE_GAME", *(_DWORD *)(a4 + 8), cr8game_WndProc, 0);
	return v6 != -1 ? v6 : 0;
}
// 10029630: using guessed type int cr8_playercount;
// 10029638: using guessed type int cr8_dword_10029638;
// 1002963C: using guessed type int cr8_dword_1002963C;
// 10029640: using guessed type int cr8_dword_10029640;

// ref: 0x10004506
LRESULT __stdcall cr8game_WndProc(HWND hWnd, UINT Msg, WPARAM wParam, LPARAM lParam)
{
	signed int v4; // ebx
	HWND v6; // eax
	HWND v7; // eax
	HWND v8; // eax
	char Buffer[256]; // [esp+Ch] [ebp-104h]
	int a2; // [esp+10Ch] [ebp-4h]

	v4 = Msg;
	if ( Msg == 2 )
	{
		cr8game_FreeCreaStuff();
		cr8game_FreeMainMem(hWnd);
		if ( cr8game_hobject )
			DeleteObject(cr8game_hobject);
		cr8_sendmsg1 = 0;
		cr8_sendmsg2 = 0;
		return (LRESULT)SDlgDefDialogProc(hWnd, v4, (HDC)wParam, (HWND)lParam);
	}
	if ( Msg > 0x103 )
	{
		if ( Msg > 0x105 )
		{
			if ( Msg == 272 )
			{
				cr8_sendmsg2 = GetDlgItem(hWnd, 1010);
				cr8_sendmsg1 = GetDlgItem(hWnd, 1011);
				cr8game_LoadCreaGFX(hWnd);
				cr8game_AllocMainMem(hWnd);
				cr8game_SendMessageF5(hWnd);
				cr8game_hobject = cr8game_GetCr8Object(hWnd);
				SendMessageA(cr8_sendmsg2, 0xC5u, 0x1Fu, 0);
				SendMessageA(cr8_sendmsg1, 0xC5u, 0x1Fu, 0);
				return 1;
			}
			if ( Msg != 273 )
			{
				if ( Msg == 312 && GetWindowLongA((HWND)lParam, -12) == 1030 )
				{
					local_SetWhiteText((HDC)wParam);
					return (LRESULT)GetStockObject(5);
				}
				return (LRESULT)SDlgDefDialogProc(hWnd, v4, (HDC)wParam, (HWND)lParam);
			}
			if ( (unsigned short)wParam == 1 )
			{
				if ( SendMessageA(cr8_sendmsg2, 0xEu, 0, 0) )
				{
					SendMessageA(cr8_sendmsg2, 0xDu, 0x20u, (LPARAM)cr8_gamename);
					if ( SelHero_IsNameReserved(cr8_gamename)
					  || SelHero_NameHasChar(cr8_gamename, &nullcharacter)
					  || !cr8game_CheckValidGameName(cr8_gamename) )
					{
						LoadStringA(ghUiInst, 0x404u, Buffer, 256);
						OkCancel_DoOkDialog(hWnd, Buffer, 1);
						cr8_gamename[0] = 0;
					}
					else
					{
						cr8_gamepassword[0] = 0;
						if ( SendMessageA(cr8_sendmsg1, 0xEu, 0, 0) )
							SendMessageA(cr8_sendmsg1, 0xDu, 0x20u, (LPARAM)cr8_gamepassword);
						cr8_gamedata.bDiff = cr8_dword_1002966C;
						TitleSnd_PlaySelectSound();
						if ( cr8game_GetSnetCreaGame(hWnd) )
							SDlgEndDialog(hWnd, (void *)HANDLE_FLAG_INHERIT);
					}
				}
				else
				{
					LoadStringA(ghUiInst, 0x3F0u, Buffer, 256);
					OkCancel_DoOkDialog(hWnd, Buffer, 1);
				}
			}
			else
			{
				if ( (unsigned short)wParam != 2 )
				{
					if ( (signed int)(unsigned short)wParam > 1031 && (signed int)(unsigned short)wParam <= 1034 )
					{
						a2 = (unsigned short)wParam - 1032;
						if ( HIWORD(wParam) == 6 )
							SetFocus(cr8_sendmsg2);
						if ( cr8_dword_1002966C != (unsigned short)wParam - 1032 )
						{
							if ( cr8_dword_1002966C != -1 )
							{
								v6 = GetDlgItem(hWnd, cr8_dword_1002966C + 1032);
								SendMessageA(v6, 0xF3u, 0, 0);
								TitleSnd_PlayMoveSound();
							}
							cr8game_BlitCr8Dialog(hWnd, a2);
							v7 = GetDlgItem(hWnd, 1040);
							cr8game_SetWindowStr(v7, 1029, a2);
							v4 = 273;
							cr8_dword_1002966C = a2;
						}
						SendMessageA((HWND)lParam, 0xF3u, 1u, 0);
					}
					return (LRESULT)SDlgDefDialogProc(hWnd, v4, (HDC)wParam, (HWND)lParam);
				}
				TitleSnd_PlaySelectSound();
				SDlgEndDialog(hWnd, 0);
			}
			return 0;
		}
		v8 = (HWND)SDrawGetFrameWindow();
		SendMessageA(v8, Msg, wParam, lParam);
	}
	return (LRESULT)SDlgDefDialogProc(hWnd, v4, (HDC)wParam, (HWND)lParam);
}
// 10010382: using guessed type _DWORD __stdcall SDrawGetFrameWindow();

// ref: 0x10004828
void __cdecl cr8game_FreeCreaStuff()
{
	if ( cr8_creat_bg_ptr )
	{
		SMemFree(cr8_creat_bg_ptr, "C:\\Src\\Diablo\\DiabloUI\\cr8game.cpp", 55, 0);
		cr8_creat_bg_ptr = 0;
	}
	if ( cr8_but_xsm_ptr )
	{
		SMemFree(cr8_but_xsm_ptr, "C:\\Src\\Diablo\\DiabloUI\\cr8game.cpp", 60, 0);
		cr8_but_xsm_ptr = 0;
	}
	if ( cr8_diffbtns_ptr )
	{
		SMemFree(cr8_diffbtns_ptr, "C:\\Src\\Diablo\\DiabloUI\\cr8game.cpp", 65, 0);
		cr8_diffbtns_ptr = 0;
	}
}

// ref: 0x1000487F
BOOL __fastcall cr8game_LoadCreaGFX(HWND hWnd)
{
	int id[3]; // [esp+8h] [ebp-1Ch]
	DWORD a8[2]; // [esp+14h] [ebp-10h]
	DWORD a3[2]; // [esp+1Ch] [ebp-8h]

	id[0] = 0;
	id[0] = 1;
	id[1] = 2;
	local_LoadArtWithPal(hWnd, 0, "Dialog", -1, 1, "ui_art\\creat_bg.pcx", &cr8_creat_bg_ptr, a8, 1);
	local_LoadArtImage("ui_art\\but_xsm.pcx", &cr8_but_xsm_ptr, a3);
	SDlgSetControlBitmaps(hWnd, id, 0, (char *)cr8_but_xsm_ptr, (char *)a3, 1, -1);
	local_LoadArtImage("ui_art\\diffbtns.pcx", &cr8_diffbtns_ptr, cr8diffbtns_size);
	local_SetStaticBmp(hWnd, 1040, cr8_creat_bg_ptr, a8);
	return 1;
}

// ref: 0x10004914
void __fastcall cr8game_FreeMainMem(HWND hWnd)
{
	signed int v2; // edi
	HWND v3; // esi
	void **v4; // eax MAPDST
	void *v6; // eax
	struct tagRECT Rect; // [esp+10h] [ebp-10h]

	v2 = 0;
	do
	{
		v3 = GetDlgItem(hWnd, v2 + 1032);
		GetClientRect(v3, &Rect);
		v4 = (void **)GetWindowLongA(v3, -21);
		if ( v4 )
		{
			v6 = *v4;
			if ( *v4 )
				SMemFree(v6, "C:\\Src\\Diablo\\DiabloUI\\cr8game.cpp", 160, 0);
			SMemFree(v4, "C:\\Src\\Diablo\\DiabloUI\\cr8game.cpp", 162, 0);
		}
		++v2;
	}
	while ( v2 < 3 );
}

// ref: 0x1000497F
void __fastcall cr8game_AllocMainMem(HWND hWnd)
{
	int v1; // ebx
	void **v2; // esi
	HWND v3; // ST1C_4
	LONG v4; // eax
	struct tagRECT Rect; // [esp+Ch] [ebp-18h]
	HWND hWnda; // [esp+20h] [ebp-4h]

	v1 = 0;
	do
	{
		hWnda = GetDlgItem(hWnd, v1 + 1032);
		GetClientRect(hWnda, &Rect);
		v2 = (void **)SMemAlloc(0x110u, "C:\\Src\\Diablo\\DiabloUI\\cr8game.cpp", 177, 0);
		*v2 = SMemAlloc(Rect.right * Rect.bottom, "C:\\Src\\Diablo\\DiabloUI\\cr8game.cpp", 178, 0);
		v3 = hWnda;
		v2[1] = (void *)Rect.right;
		v4 = Rect.bottom;
		v2[3] = 0;
		v2[2] = (void *)v4;
		SetWindowLongA(v3, -21, (LONG)v2);
		SDlgSetBitmapI(hWnda, 0, &nullcharacter, -1, 241, *v2, 0, (int)v2[1], (int)v2[2], -1);
		cr8game_DoAROP3Blit(hWnda, v1++, 0);
	}
	while ( v1 < 3 );
}

// ref: 0x10004A34
void __fastcall cr8game_DoAROP3Blit(HWND hWnd, int frame, int size)
{
	DWORD *v5; // eax

	v5 = (DWORD *)GetWindowLongA(hWnd, -21);
	if ( v5 )
	{
		if ( cr8_diffbtns_ptr )
		{
			SBltROP3(
				(void *)*v5,
				&cr8_diffbtns_ptr[cr8diffbtns_size[0] * v5[2] * (size + 2 * frame)],
				v5[1],
				v5[2],
				v5[1],
				cr8diffbtns_size[0],
				0,
				0xCC0020u);
			InvalidateRect(hWnd, 0, 0);
		}
	}
}

// ref: 0x10004A93
void __fastcall cr8game_SendMessageF5(HWND hWnd)
{
	cr8_dword_1002966C = -1;
	cr8_dword_10029658 = 0;
	cr8_dword_10029668 = 0;
	SendDlgItemMessageA(hWnd, 1032, 0xF5u, 0, 0);
}
// 10029658: using guessed type int cr8_dword_10029658;
// 10029668: using guessed type int cr8_dword_10029668;

// ref: 0x10004ABA
void __fastcall cr8game_BlitCr8Dialog(HWND hWnd, int a2)
{
	HWND v4; // eax
	HWND v5; // eax

	if ( cr8_dword_1002966C != -1 )
	{
		v4 = GetDlgItem(hWnd, cr8_dword_1002966C + 1032);
		cr8game_DoAROP3Blit(v4, cr8_dword_1002966C, 0);
	}
	v5 = GetDlgItem(hWnd, a2 + 1032);
	cr8game_DoAROP3Blit(v5, a2, 1);
}

// ref: 0x10004B02
void __fastcall cr8game_SetWindowStr(HWND hWnd, int dlgitem, int a3)
{
	char Buffer[256]; // [esp+4h] [ebp-100h]

	LoadStringA(ghUiInst, a3 + dlgitem, Buffer, 256);
	SetWindowTextA(hWnd, Buffer);
}

// ref: 0x10004B3F
int __fastcall cr8game_CheckValidGameName(char *name)
{
	signed int v1; // edi
	char v2; // al
	signed int v3; // esi
	int result; // eax
	char v5[32]; // [esp+8h] [ebp-24h]

	strcpy(v5, name);
	v1 = 0;
	if ( v5[0] == 32 )
	{
		v2 = 32;
		while ( v2 )
		{
			v2 = v5[v1++ + 1];
			if ( v2 != 32 )
				goto LABEL_5;
		}
LABEL_9:
		result = 0;
	}
	else
	{
LABEL_5:
		v3 = strlen(v5);
		while ( v5[--v3] == 32 )
		{
			if ( v3 <= v1 )
				goto LABEL_9;
		}
		v5[v3 + 1] = 0;
		strcpy(name, &v5[v1]);
		result = v3 + 1 - v1;
	}
	return result;
}

// ref: 0x10004BA8
HFONT __fastcall cr8game_GetCr8Object(HWND hWnd)
{
	HFONT v2; // eax MAPDST
	int pv[15]; // [esp+8h] [ebp-40h]

	v2 = (HFONT)SendMessageA(hWnd, 0x31u, 0, 0);
	if ( v2 )
	{
		if ( GetObjectA(v2, 60, pv) )
		{
			pv[0] = -MulDiv(12, 96, 72);
			pv[1] = 0;
			v2 = CreateFontIndirectA((const LOGFONTA *)pv);
			if ( v2 )
			{
				SendDlgItemMessageA(hWnd, 1032, 0x30u, (WPARAM)v2, 0);
				SendDlgItemMessageA(hWnd, 1033, 0x30u, (WPARAM)v2, 0);
				SendDlgItemMessageA(hWnd, 1034, 0x30u, (WPARAM)v2, 0);
			}
		}
	}
	return v2;
}

// ref: 0x1000B7A0
_uiheroinfo *__cdecl SelHero_GetCurrentHeroInfo()
{
	return sgpHeroInfo;
}

// ref: 0x1000B7A6
int __cdecl SelHero_GetNumHeroesLeft()
{
	return selhero_numheroesleft;
}
// 1002A428: using guessed type int selhero_numheroesleft;

// ref: 0x1000B7AC
void __fastcall SelHero_SetHeroDifficulty(int diff)
{
	selhero_difficulty = diff;
}
// 1002A420: using guessed type int selhero_difficulty;

// ref: 0x1000B7B3
char *__cdecl SelHero_GetHeroNameStr()
{
	return selhero_heronamestr;
}

// ref: 0x1000B7B9
_uiheroinfo *__cdecl SelHero_AllocHeroInfo()
{
	return (_uiheroinfo *)SMemAlloc(0x2Cu, "C:\\Src\\Diablo\\DiabloUI\\SelHero.cpp", 123, 0);
}

// ref: 0x1000B7CA
int __cdecl SelHero_GetHeroIsGood()
{
	return selhero_is_good;
}

// ref: 0x1000B7D0
int __fastcall SelHero_SetClassStats(int heroclass, _uidefaultstats *pStats)
{
	return selhero_fnstats(heroclass, pStats);
}

// ref: 0x1000B7DE
void __cdecl SelHero_cpp_init()
{
	SelHero_cpp_float = SelHero_cpp_float_value;
}
// 1001F460: using guessed type int SelHero_cpp_float_value;
// 1002A414: using guessed type int SelHero_cpp_float;

// ref: 0x1000B899
void __fastcall SelHero_SetStaticBMP(HWND hWnd, int adjust_size)
{
	HWND v3; // esi
	struct tagRECT Rect; // [esp+8h] [ebp-10h]

	v3 = GetDlgItem(hWnd, 1040);
	InvalidateRect(v3, 0, 0);
	GetClientRect(v3, &Rect);
	local_AdjustRectSize(&Rect, 0, adjust_size * Rect.bottom);
	SDlgSetBitmapI(v3, 0, "Static", -1, 1, selhero_buffer, (int)&Rect, selhero_sizedata[0], selhero_sizedata[1], -1);
}

// ref: 0x1000B905
void __fastcall SelHero_PrintHeroInfo(HWND hWnd, _uiheroinfo *pInfo)
{
	HWND v3; // eax
	int v4; // eax
	HWND v5; // eax
	int v6; // eax
	HWND v7; // eax
	int v8; // eax
	HWND v9; // eax
	int v10; // eax
	HWND v11; // eax
	int v12; // eax
	HWND v15; // ebp
	int v16; // eax
	HWND v17; // ST1C_4
	int v18; // eax
	HWND v19; // ST1C_4
	int v20; // eax
	HWND v21; // ST1C_4
	int v22; // eax
	HWND v23; // ST1C_4
	int v24; // eax

	if ( pInfo->level )
	{
		selhero_hero_hassaved = pInfo->hassaved;
		strcpy(selhero_heronamestr, pInfo->name);
		v15 = GetDlgItem(hWnd, 1014);
		wsprintfA(selhero_herolevel, "%d", pInfo->level);
		v16 = GetWindowLongA(v15, -21);
		local_SetWndLongStr(v16, selhero_herolevel);
		v17 = GetDlgItem(hWnd, 1018);
		wsprintfA(selhero_herostr, "%d", pInfo->strength);
		v18 = GetWindowLongA(v17, -21);
		local_SetWndLongStr(v18, selhero_herostr);
		v19 = GetDlgItem(hWnd, 1017);
		wsprintfA(selhero_heromag, "%d", pInfo->magic);
		v20 = GetWindowLongA(v19, -21);
		local_SetWndLongStr(v20, selhero_heromag);
		v21 = GetDlgItem(hWnd, 1016);
		wsprintfA(selhero_herodex, "%d", pInfo->dexterity);
		v22 = GetWindowLongA(v21, -21);
		local_SetWndLongStr(v22, selhero_herodex);
		v23 = GetDlgItem(hWnd, 1015);
		wsprintfA(selhero_herovit, "%d", pInfo->vitality);
		v24 = GetWindowLongA(v23, -21);
		local_SetWndLongStr(v24, selhero_herovit);
		SelHero_SetStaticBMP(hWnd, pInfo->heroclass);
		Doom_ParseWndProc4(hWnd, selhero_msgtbl_info, 1);
	}
	else
	{
		selhero_hero_hassaved = 0;
		selhero_heronamestr[0] = 0;
		v3 = GetDlgItem(hWnd, 1014);
		v4 = GetWindowLongA(v3, -21);
		local_SetWndLongStr(v4, "--");
		v5 = GetDlgItem(hWnd, 1018);
		v6 = GetWindowLongA(v5, -21);
		local_SetWndLongStr(v6, "--");
		v7 = GetDlgItem(hWnd, 1017);
		v8 = GetWindowLongA(v7, -21);
		local_SetWndLongStr(v8, "--");
		v9 = GetDlgItem(hWnd, 1016);
		v10 = GetWindowLongA(v9, -21);
		local_SetWndLongStr(v10, "--");
		v11 = GetDlgItem(hWnd, 1015);
		v12 = GetWindowLongA(v11, -21);
		local_SetWndLongStr(v12, "--");
		SelHero_SetStaticBMP(hWnd, 3);
		Doom_ParseWndProc4(hWnd, selhero_msgtbl_info, 1);
	}
}
// 1002A424: using guessed type int selhero_hero_hassaved;

// ref: 0x1000BA7B
void __fastcall SelHero_SetStringWithMsg(HWND hWnd, const char *str)
{
	HWND v4; // eax
	int v5; // eax

	v4 = GetDlgItem(hWnd, 1038);
	if ( v4 )
	{
		v5 = GetWindowLongA(v4, -21);
		local_SetWndLongStr(v5, str);
		Doom_ParseWndProc4(hWnd, selhero_msgtbl_string, 5);
	}
}

// ref: 0x1000BAB4
BOOL __fastcall SelHero_IsNameReserved(char *name)
{
	UINT v1; // esi
	BOOL result; // eax
	char SrcStr[128]; // [esp+4h] [ebp-90h]
	char Buffer[16]; // [esp+84h] [ebp-10h]

	strcpy(SrcStr, name);
	_strlwr(SrcStr);
	v1 = 19;
	while ( 1 )
	{
		LoadStringA(ghUiInst, v1, Buffer, 15);
		SelHero_SetLastNamePos(Buffer);
		_strlwr(Buffer);
		result = (BOOL)strstr(SrcStr, Buffer);
		if ( result )
			break;
		if ( (signed int)++v1 > 26 )
			return result;
	}
	return 1;
}

// ref: 0x1000BB26
void __fastcall SelHero_SetLastNamePos(char *name)
{
	while ( *name )
		--*name++;
}

// ref: 0x1000BB34
BOOL __fastcall SelHero_NameHasChar(char *name, char *illegalchrs)
{
	char v5; // al

	if ( strpbrk(name, ",<>%&\\\"?*#/:") || strpbrk(name, illegalchrs) )
		return 1;
	while ( 1 )
	{
		v5 = *name;
		if ( !*name )
			break;
		if ( (unsigned char)v5 < 0x20u || (unsigned char)v5 > 0x7Eu && (unsigned char)v5 < 0xC0u )
			return 1;
		++name;
	}
	return 0;
}

// ref: 0x1000BB75
BOOL __fastcall UiValidPlayerName(char *name)
{
	BOOL v2; // edi

	v2 = 1;
	if ( !strlen(name) )
		v2 = 0;
	if ( selhero_is_good == 1 && (SelHero_IsNameReserved(name) || SelHero_NameHasChar(name, " ")) )
		v2 = 0;
	return v2;
}

// ref: 0x1000BBB4
BOOL __stdcall UiSelHeroMultDialog(BOOL (__stdcall *fninfo)(BOOL (__stdcall *fninfofunc)(_uiheroinfo *)), BOOL (__stdcall *fncreate)(_uiheroinfo *), BOOL (__stdcall *fnremove)(_uiheroinfo *), BOOL (__stdcall *fnstats)(int, _uidefaultstats *), int *dlgresult, int *a6, char *name)
{
	int v7; // eax
	int v8; // eax

	artfont_LoadAllFonts();
	selhero_fninfo = fninfo;
	selhero_fncreate = fncreate;
	selhero_fnremove = fnremove;
	selhero_fnstats = fnstats;
	sgpHeroInfo = 0;
	selhero_is_good = 1;
	selhero_is_created = 0;
	v7 = (int)SDrawGetFrameWindow();
	v8 = SDlgDialogBoxParam(ghUiInst, "SELHERO_DIALOG", v7, SelHero_WndProc, 0);
	if ( dlgresult )
		*dlgresult = v8;
	if ( name )
		strcpy(name, selhero_heronamestr);
	if ( a6 )
		*a6 = selhero_is_created;
	return 1;
}
// 10010382: using guessed type _DWORD __stdcall SDrawGetFrameWindow();
// 1002A45C: using guessed type int selhero_is_created;

// ref: 0x1000BC46
LRESULT __stdcall SelHero_WndProc(HWND hWnd, UINT Msg, WPARAM wParam, LPARAM lParam)
{
	HWND v4; // eax
	int v6; // edx
	HWND v7; // ecx
	signed int v8; // [esp-4h] [ebp-8h]

	if ( Msg > 0xBD2 )
	{
		switch ( Msg )
		{
			case 0xBD3u:
				SelHero_DoSelLoad(hWnd);
				return 0;
			case 0xBD4u:
				SelHero_DoSelDiff(hWnd);
				return 0;
			case 0xBD5u:
				v7 = hWnd;
				if ( selhero_is_good != 1 )
				{
					v8 = 2;
					goto LABEL_30;
				}
				break;
			case 0xBD6u:
				strcpy(selhero_heronamestr, heroinfo_create.name);
				v6 = 1;
				v7 = hWnd;
				if ( selhero_is_good != 1 )
				{
					selhero_difficulty = 0;
LABEL_31:
					SelHero_DoHeroEndFade(v7, v6);
					return 0;
				}
				break;
			case 0xBD7u:
				SelHero_DoStuffWithStrings(hWnd);
				return 0;
			default:
				return (LRESULT)SDlgDefDialogProc(hWnd, Msg, (HDC)wParam, (HWND)lParam);
		}
		v8 = 3;
LABEL_30:
		v6 = v8;
		goto LABEL_31;
	}
	if ( Msg == 3026 )
	{
		SelHero_DoEnterName(hWnd);
		return 0;
	}
	if ( Msg == 2 )
	{
		SelHero_DeleteAndFree(hWnd);
		return (LRESULT)SDlgDefDialogProc(hWnd, Msg, (HDC)wParam, (HWND)lParam);
	}
	if ( Msg <= 0x103 )
		return (LRESULT)SDlgDefDialogProc(hWnd, Msg, (HDC)wParam, (HWND)lParam);
	if ( Msg <= 0x105 )
	{
		v4 = (HWND)SDrawGetFrameWindow();
		SendMessageA(v4, Msg, wParam, lParam);
		return (LRESULT)SDlgDefDialogProc(hWnd, Msg, (HDC)wParam, (HWND)lParam);
	}
	switch ( Msg )
	{
		case 0x110u:
			SelHero_LoadHeroGFX(hWnd);
			PostMessageA(hWnd, 0x7E8u, 0, 0);
			return 0;
		case 0x7E8u:
			if ( !Fade_CheckRange5() )
				Fade_SetFadeTimer((int)hWnd);
			return 0;
		case 0xBD0u:
			SelHero_DoHeroSelList(hWnd);
			return 0;
	}
	if ( Msg != 3025 )
		return (LRESULT)SDlgDefDialogProc(hWnd, Msg, (HDC)wParam, (HWND)lParam);
	SelHero_DoHeroSelClass(hWnd);
	return 0;
}
// 10010382: using guessed type _DWORD __stdcall SDrawGetFrameWindow();
// 1002A420: using guessed type int selhero_difficulty;

// ref: 0x1000BDAD
void __fastcall SelHero_DoStuffWithStrings(HWND hWnd)
{
	_uiheroinfo *v1; // eax
	char dialogstr[80]; // [esp+Ch] [ebp-B4h]
	char string64[64]; // [esp+5Ch] [ebp-64h]
	char Buffer[32]; // [esp+9Ch] [ebp-24h]

	if ( SelHero_GetHeroIsGood() == 1 )
		LoadStringA(ghUiInst, 0x23u, Buffer, 31);
	else
		LoadStringA(ghUiInst, 0x22u, Buffer, 31);
	LoadStringA(ghUiInst, 7u, string64, 63);
	wsprintfA(dialogstr, string64, selhero_heronamestr);
	if ( SelYesNo_YesNoDialog(hWnd, dialogstr, Buffer, 1) != 2 )
	{
		v1 = SelHero_GetHeroSlotFromName(sgpHeroInfo, selhero_heronamestr);
		if ( v1 )
		{
			if ( selhero_fnremove(v1) )
			{
				sgpHeroInfo = SelHero_GetNextHeroFromStr(sgpHeroInfo, selhero_heronamestr);
				--selhero_numheroesleft;
				LoadStringA(ghUiInst, 0x1Eu, string64, 15);
				if ( !strcmp(string64, sgpHeroInfo->name) )
				{
					PostMessageA(hWnd, 0xBD1u, 0, 0);
					return;
				}
				SelHero_PrintHeroInfo(hWnd, sgpHeroInfo);
			}
			else
			{
				LoadStringA(ghUiInst, 0x11u, string64, 63);
				SelYesNo_SelOkDialog(hWnd, string64, Buffer, 1);
			}
		}
	}
	PostMessageA(hWnd, 0xBD0u, 0, 0);
}
// 1002A428: using guessed type int selhero_numheroesleft;

// ref: 0x1000BEDB
_uiheroinfo *__fastcall SelHero_GetNextHeroFromStr(_uiheroinfo *pInfo, char *name)
{
	_uiheroinfo *v2; // ebx
	_uiheroinfo *v3; // ebp
	_uiheroinfo *v4; // edi

	v2 = pInfo;
	v3 = 0;
	v4 = 0;
	if ( pInfo )
	{
		while ( !v4 )
		{
			if ( !strcmp(pInfo->name, name) )
			{
				v4 = pInfo;
			}
			else
			{
				v3 = pInfo;
				pInfo = pInfo->next;
			}
			if ( !pInfo )
			{
				if ( !v4 )
					return v2;
				break;
			}
		}
		if ( v3 )
			v3->next = v4->next;
		else
			v2 = v4->next;
		SelHero_FreeSomeMemory(v4);
	}
	return v2;
}

// ref: 0x1000BF33
void __fastcall SelHero_FreeSomeMemory(void *ptr)
{
	if ( ptr )
		SMemFree(ptr, "C:\\Src\\Diablo\\DiabloUI\\SelHero.cpp", 131, 0);
}

// ref: 0x1000BF4A
_uiheroinfo *__fastcall SelHero_GetHeroSlotFromName(_uiheroinfo *pInfo, const char *name)
{
	_uiheroinfo *i; // esi

	for ( i = pInfo; i && _strcmpi(i->name, name); i = i->next )
		;
	return i;
}

// ref: 0x1000BF6D
void __fastcall SelHero_DoHeroSelList(HWND hWnd)
{
	BOOL v2; // eax
	int v3; // edx

	v2 = SDlgDialogBoxParam(ghUiInst, "SELLIST_DIALOG", (int)hWnd, SelList_WndProc, 0);
	if ( v2 == 1 )
	{
		if ( !strlen(selhero_heronamestr) )
		{
			PostMessageA(hWnd, 0xBD1u, 0, 0);
			return;
		}
		if ( selhero_is_good == 1 )
		{
			PostMessageA(hWnd, 0xBD5u, 0, 0);
			return;
		}
		if ( selhero_hero_hassaved )
		{
			PostMessageA(hWnd, 0xBD3u, 0, 0);
			return;
		}
		selhero_difficulty = 0;
		v3 = 1;
LABEL_13:
		SelHero_DoHeroEndFade(hWnd, v3);
		return;
	}
	if ( v2 != 1006 )
	{
		v3 = 4;
		goto LABEL_13;
	}
	PostMessageA(hWnd, 0xBD7u, 0, 0);
}
// 1002A420: using guessed type int selhero_difficulty;
// 1002A424: using guessed type int selhero_hero_hassaved;

// ref: 0x1000BFF9
void __fastcall SelHero_DoHeroSelClass(HWND hWnd)
{
	BOOL v2; // eax
	int v3; // eax
	char Buffer[32]; // [esp+8h] [ebp-20h]

	v2 = SDlgDialogBoxParam(ghUiInst, "SELCLASS_DIALOG", (int)hWnd, SelClass_WndProc, 0);
	if ( v2 == -1 || v2 == 2 )
	{
		LoadStringA(ghUiInst, 0x1Eu, Buffer, 31);
		if ( !strcmp(Buffer, sgpHeroInfo->name) )
			SelHero_DoHeroEndFade(hWnd, 4);
		else
			PostMessageA(hWnd, 0xBD0u, 0, 0);
	}
	else
	{
		v3 = v2 - 1063;
		if ( v3 )
		{
			if ( v3 == 1 )
				heroinfo_create.heroclass = 2;
			else
				heroinfo_create.heroclass = 0;
		}
		else
		{
			heroinfo_create.heroclass = 1;
		}
		PostMessageA(hWnd, 0xBD2u, 0, 0);
	}
}

// ref: 0x1000C09B
void __fastcall SelHero_DoEnterName(HWND hWnd)
{
	char namestr[16]; // [esp+8h] [ebp-10h]

	if ( SDlgDialogBoxParam(ghUiInst, "ENTERNAME_DIALOG", (int)hWnd, EntName_WndProc, (int)namestr) == 1 )
	{
		namestr[15] = 0;
		if ( SelHero_CreateHero(hWnd, namestr) )
			PostMessageA(hWnd, 0xBD6u, 0, 0);
		else
			PostMessageA(hWnd, 0xBD2u, 0, 0);
	}
	else
	{
		PostMessageA(hWnd, 0xBD1u, 0, 0);
	}
}

// ref: 0x1000C0F9
BOOL __fastcall SelHero_CreateHero(HWND hWnd, char *name)
{
	_uiheroinfo *v2; // edi
	char dialogstr[144]; // [esp+Ch] [ebp-138h]
	char v5[128]; // [esp+9Ch] [ebp-A8h]
	char Buffer[32]; // [esp+11Ch] [ebp-28h]

	if ( SelHero_GetHeroIsGood() == 1 )
		LoadStringA(ghUiInst, 0x20u, Buffer, 31);
	else
		LoadStringA(ghUiInst, 0x1Fu, Buffer, 31);
	if ( !UiValidPlayerName(name) )
	{
		LoadStringA(ghUiInst, 0xFu, v5, 127);
		SelYesNo_SelOkDialog(hWnd, v5, Buffer, 1);
		return 0;
	}
	v2 = SelHero_GetHeroSlotFromName(sgpHeroInfo, name);
	if ( v2 )
	{
		LoadStringA(ghUiInst, 8u, v5, 127);
		wsprintfA(dialogstr, v5, v2->name);
		if ( SelYesNo_YesNoDialog(hWnd, dialogstr, Buffer, 1) == 2 )
			return 0;
	}
	strcpy(heroinfo_create.name, name);
	heroinfo_create.hassaved = 0;
	if ( !selhero_fncreate(&heroinfo_create) )
	{
		LoadStringA(ghUiInst, 0x10u, v5, 127);
		OkCancel_DoOkDialog(hWnd, v5, 1);
		return 0;
	}
	selhero_is_created = 1;
	return 1;
}
// 1002A45C: using guessed type int selhero_is_created;

// ref: 0x1000C21A
void __fastcall SelHero_DoSelLoad(HWND hWnd)
{
	BOOL v2; // eax

	v2 = SDlgDialogBoxParam(ghUiInst, "SELLOAD_DIALOG", (int)hWnd, SelLoad_WndProc, 0);
	if ( v2 == -1 || v2 == 2 )
	{
		PostMessageA(hWnd, 0xBD0u, 0, 0);
	}
	else if ( v2 == 1106 )
	{
		PostMessageA(hWnd, 0xBD5u, 0, 0);
	}
	else
	{
		PostMessageA(hWnd, 0xBD4u, 0, 0);
	}
}

// ref: 0x1000C269
void __fastcall SelHero_DoSelDiff(HWND hWnd)
{
	_uiheroinfo *v3; // eax
	int v4; // eax
	char Buffer[256]; // [esp+4h] [ebp-208h]
	char v6[128]; // [esp+104h] [ebp-108h]
	char v7[128]; // [esp+184h] [ebp-88h]
	_gamedata gameData; // [esp+204h] [ebp-8h]

	if ( !SelHero_GetHeroIsGood() )
	{
		SelHero_SetHeroDifficulty(0);
LABEL_3:
		SelHero_DoHeroEndFade(hWnd, 1);
		return;
	}
	CreaDung_SetDelSpin(1);
	if ( SDlgDialogBoxParam(ghUiInst, "SELDIFF_DIALOG", (int)hWnd, CreaDung_WndProc, selhero_is_good) == 1 )
	{
		v3 = SelHero_GetHeroSlotFromName(sgpHeroInfo, selhero_heronamestr);
		UiCreatePlayerDescription(v3, 'DBLO', v7);
		gameData.bDiff = selhero_difficulty;
		Connect_SetDiffString(&gameData, selhero_heronamestr, v7, v6, 128);
		v4 = UiAuthCallback(2, selhero_heronamestr, v7, 0, v6, Buffer, 256);
		if ( v4 )
			goto LABEL_3;
		SelYesNo_SelOkDialog(hWnd, Buffer, 0, 1);
		PostMessageA(hWnd, 0xBD4u, 0, 0);
	}
	else
	{
		PostMessageA(hWnd, 0xBD3u, 0, 0);
	}
}
// 1002A420: using guessed type int selhero_difficulty;

// ref: 0x1000C364
void __fastcall SelHero_DeleteAndFree(HWND hWnd)
{
	void **v2; // eax

	Doom_DeleteFreeProcs(hWnd, selhero_msgtbl_info);
	Doom_DeleteFreeProcs(hWnd, selhero_msgtbl_3);
	Doom_DeleteFreeProcs(hWnd, selhero_msgtbl_string);
	Title_KillTitleTimer(hWnd);
	SelHero_FreeAllHeroes(sgpHeroInfo);
	if ( selhero_buffer )
	{
		SMemFree(selhero_buffer, "C:\\Src\\Diablo\\DiabloUI\\SelHero.cpp", 744, 0);
		selhero_buffer = 0;
	}
	v2 = (void **)GetWindowLongA(hWnd, -21);
	local_FreeMemPtr(v2);
}

// ref: 0x1000C3CE
void __fastcall SelHero_FreeAllHeroes(_uiheroinfo *pInfo)
{
	_uiheroinfo *v1; // esi

	if ( pInfo )
	{
		do
		{
			v1 = pInfo->next;
			SelHero_FreeSomeMemory(pInfo);
			pInfo = v1;
		}
		while ( v1 );
	}
}

// ref: 0x1000C3E2
void __fastcall SelHero_DoHeroEndFade(HWND hWnd, int a2)
{
	void *v2; // edi

	v2 = (void *)a2;
	Fade_Range5SetZero();
	Fade_UpdatePaletteRange(10);
	SDlgEndDialog(hWnd, v2);
}

// ref: 0x1000C3FF
void __fastcall SelHero_LoadHeroGFX(HWND hWnd)
{
	HWND v1; // eax
	int v2; // eax
	HWND v3; // eax
	int v4; // eax
	HWND v5; // eax
	int v6; // eax
	HWND v7; // eax
	int v8; // eax
	HWND v9; // eax
	int v10; // eax
	DWORD *v12; // eax MAPDST

	SelHero_SelectHeroRegion(hWnd);
	v12 = local_AllocWndLongData();
	if ( v12 )
	{
		SetWindowLongA(hWnd, -21, (LONG)v12);
		local_LoadArtWithPal(hWnd, 0, &nullcharacter, -1, 1, "ui_art\\selhero.pcx", (BYTE **)v12, v12 + 1, 0);
		Fade_NoInputAndArt(hWnd, 1);
	}
	local_LoadArtImage("ui_art\\heros.pcx", &selhero_buffer, selhero_sizedata);
	SetActiveWindow(hWnd);
	Title_LoadImgSetTimer(hWnd, "ui_art\\smlogo.pcx");
	Doom_ParseWndProc3(hWnd, selhero_msgtbl_string, 5);
	Doom_ParseWndProc3(hWnd, selhero_msgtbl_3, 1);
	Doom_ParseWndProc3(hWnd, selhero_msgtbl_info, 1);
	selhero_hero_hassaved = 0;
	selhero_heronamestr[0] = 0;
	v1 = GetDlgItem(hWnd, 1014);
	v2 = GetWindowLongA(v1, -21);
	local_SetWndLongStr(v2, "--");
	v3 = GetDlgItem(hWnd, 1018);
	v4 = GetWindowLongA(v3, -21);
	local_SetWndLongStr(v4, "--");
	v5 = GetDlgItem(hWnd, 1017);
	v6 = GetWindowLongA(v5, -21);
	local_SetWndLongStr(v6, "--");
	v7 = GetDlgItem(hWnd, 1016);
	v8 = GetWindowLongA(v7, -21);
	local_SetWndLongStr(v8, "--");
	v9 = GetDlgItem(hWnd, 1015);
	v10 = GetWindowLongA(v9, -21);
	local_SetWndLongStr(v10, "--");
	SelHero_SetStaticBMP(hWnd, 3);
	Doom_ParseWndProc4(hWnd, selhero_msgtbl_info, 1);
}
// 1002A424: using guessed type int selhero_hero_hassaved;

// ref: 0x1000C49F
void __fastcall SelHero_SelectHeroRegion(HWND hWnd)
{
	_uiheroinfo *v2; // esi
	_uiheroinfo *v3; // [esp+10h] [ebp-44h]
	char Buffer[64]; // [esp+14h] [ebp-40h]

	v2 = SelHero_AllocHeroInfo();
	v2->next = 0;
	LoadStringA(ghUiInst, 0x1Eu, v2->name, 15);
	v2->level = 0;
	sgpHeroInfo = SelRegn_SetNextHero(sgpHeroInfo, v2);
	v3 = sgpHeroInfo;
	selhero_numheroesleft = 1;
	if ( !selhero_fninfo(SelHero_GetHeroInfo) )
	{
		LoadStringA(ghUiInst, 0x12u, Buffer, 64);
		OkCancel_DoOkDialog(hWnd, Buffer, 1);
	}
	if ( v3 == sgpHeroInfo )
		PostMessageA(hWnd, 0xBD1u, 0, 0);
	else
		PostMessageA(hWnd, 0xBD0u, 0, 0);
}
// 1002A428: using guessed type int selhero_numheroesleft;

// ref: 0x1000C541
BOOL __stdcall SelHero_GetHeroInfo(_uiheroinfo *pInfo)
{
	_uiheroinfo *v1; // esi
	_uiheroinfo *v2; // eax

	v1 = SelHero_AllocHeroInfo();
	memcpy(v1, pInfo, 0x2Cu);
	v1->next = 0;
	v2 = SelRegn_SetNextHero(sgpHeroInfo, v1);
	++selhero_numheroesleft;
	sgpHeroInfo = v2;
	return 1;
}
// 1002A428: using guessed type int selhero_numheroesleft;

// ref: 0x1000C57A
BOOL __stdcall UiSelHeroSingDialog(BOOL (__stdcall *fninfo)(BOOL (__stdcall *fninfofunc)(_uiheroinfo *)), BOOL (__stdcall *fncreate)(_uiheroinfo *), BOOL (__stdcall *fnremove)(_uiheroinfo *), BOOL (__stdcall *fnstats)(int, _uidefaultstats *), int *dlgresult, char *name, int *difficulty)
{
	int v7; // eax
	int v8; // edi

	artfont_LoadAllFonts();
	selhero_fninfo = fninfo;
	selhero_fncreate = fncreate;
	selhero_fnremove = fnremove;
	selhero_fnstats = fnstats;
	sgpHeroInfo = 0;
	selhero_is_good = 0;
	v7 = (int)SDrawGetFrameWindow();
	v8 = SDlgDialogBoxParam(ghUiInst, "SELHERO_DIALOG", v7, SelHero_WndProc, 0);
	if ( dlgresult )
		*dlgresult = v8;
	if ( name )
		strcpy(name, selhero_heronamestr);
	if ( difficulty )
		*difficulty = selhero_difficulty;
	if ( v8 != 4 )
		artfont_FreeAllFonts();
	return 1;
}
// 10010382: using guessed type _DWORD __stdcall SDrawGetFrameWindow();
// 1002A420: using guessed type int selhero_difficulty;

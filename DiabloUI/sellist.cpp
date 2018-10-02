// ref: 0x1000D769
void __cdecl SelList_cpp_init()
{
	SelList_cpp_float = SelList_cpp_float_value;
}
// 1001F468: using guessed type int SelList_cpp_float_value;
// 1002A4C0: using guessed type int SelList_cpp_float;

// ref: 0x1000D774
LRESULT __stdcall SelList_WndProc(HWND hWnd, UINT Msg, WPARAM wParam, LPARAM lParam)
{
	HWND v4; // eax
	char *v5; // eax
	int v6; // edx
	HWND v8; // eax
	HWND v9; // eax

	if ( Msg > 0x111 )
	{
		if ( Msg == 275 )
		{
			v9 = GetFocus();
			Focus_DoBlitSpinIncFrame(hWnd, v9);
			return 0;
		}
		if ( Msg != 513 )
		{
			if ( Msg == 514 )
			{
				v8 = GetDlgItem(hWnd, 1105);
				if ( !Sbar_CheckIfNextHero(v8) )
					return (LRESULT)SDlgDefDialogProc(hWnd, Msg, (HDC)wParam, (HWND)lParam);
				goto LABEL_23;
			}
			if ( Msg != 515 )
				return (LRESULT)SDlgDefDialogProc(hWnd, Msg, (HDC)wParam, (HWND)lParam);
		}
		SelList_ChooseDlgFromSize(hWnd, (unsigned short)lParam, (unsigned int)lParam >> 16);
		return (LRESULT)SDlgDefDialogProc(hWnd, Msg, (HDC)wParam, (HWND)lParam);
	}
	switch ( Msg )
	{
		case 0x111u:
			if ( HIWORD(wParam) == 7 )
			{
				Focus_GetAndBlitSpin(hWnd, lParam);
				return (LRESULT)SDlgDefDialogProc(hWnd, Msg, (HDC)wParam, (HWND)lParam);
			}
			if ( HIWORD(wParam) != 6 )
			{
				v6 = 1;
				if ( HIWORD(wParam) != 5 && (_WORD)wParam != 1 )
				{
					v6 = 2;
					if ( (_WORD)wParam != 2 )
						return (LRESULT)SDlgDefDialogProc(hWnd, Msg, (HDC)wParam, (HWND)lParam);
				}
LABEL_25:
				OkCancel_PlaySndEndDlg(hWnd, v6);
				return (LRESULT)SDlgDefDialogProc(hWnd, Msg, (HDC)wParam, (HWND)lParam);
			}
			Focus_CheckPlayMove(lParam);
			Focus_DoBlitSpinIncFrame(hWnd, (HWND)lParam);
			SelList_GetHeroStats(hWnd, (unsigned short)wParam);
LABEL_23:
			SelList_CountHeroList(hWnd);
			return (LRESULT)SDlgDefDialogProc(hWnd, Msg, (HDC)wParam, (HWND)lParam);
		case 2u:
			SelList_DeleteFreeProcs(hWnd);
			return (LRESULT)SDlgDefDialogProc(hWnd, Msg, (HDC)wParam, (HWND)lParam);
		case 6u:
			if ( (_WORD)wParam == 1 || (_WORD)wParam == 2 )
				SelList_LoadFocus16(hWnd);
			else
				SelList_KillFocus16(hWnd);
			return 0;
		case 0x100u:
			if ( wParam != 46 )
				return (LRESULT)SDlgDefDialogProc(hWnd, Msg, (HDC)wParam, (HWND)lParam);
			v5 = SelHero_GetHeroNameStr();
			if ( !strlen(v5) )
				return (LRESULT)SDlgDefDialogProc(hWnd, Msg, (HDC)wParam, (HWND)lParam);
			v6 = 1006;
			goto LABEL_25;
	}
	if ( Msg <= 0x103 )
		return (LRESULT)SDlgDefDialogProc(hWnd, Msg, (HDC)wParam, (HWND)lParam);
	if ( Msg <= 0x105 )
	{
		v4 = (HWND)SDrawGetFrameWindow();
		SendMessageA(v4, Msg, wParam, lParam);
		return (LRESULT)SDlgDefDialogProc(hWnd, Msg, (HDC)wParam, (HWND)lParam);
	}
	if ( Msg != 272 )
		return (LRESULT)SDlgDefDialogProc(hWnd, Msg, (HDC)wParam, (HWND)lParam);
	SelList_ShowListWindow(hWnd);
	return 0;
}
// 10010382: using guessed type _DWORD __stdcall SDrawGetFrameWindow();

// ref: 0x1000D916
void __fastcall SelList_DeleteFreeProcs(HWND hWnd)
{
	HWND v2; // eax

	Sbar_FreeScrollBar(hWnd, 1105);
	Doom_DeleteFreeProcs(hWnd, sellist_msgtbl4);
	Doom_DeleteFreeProcs(hWnd, sellist_msgtbl3);
	Doom_DeleteFreeProcs(hWnd, sellist_msgtbl2);
	Doom_DeleteFreeProcs(hWnd, sellist_msgtbl1);
	v2 = GetParent(hWnd);
	SelHero_SetStringWithMsg(v2, 0);
}

// ref: 0x1000D964
void __fastcall SelList_GetHeroStats(HWND hWnd, int nIDDlgItem)
{
	HWND v2; // ebp
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
	HWND v14; // eax
	LONG v15; // eax
	_uiheroinfo *v16; // edi
	HWND v17; // eax
	HWND v18; // eax

	v14 = GetDlgItem(hWnd, nIDDlgItem);
	if ( v14 )
	{
		v15 = GetWindowLongA(v14, -21);
		if ( v15 )
		{
			v16 = *(_uiheroinfo **)(v15 + 12);
			if ( v16 )
			{
				if ( v16->level )
					Doom_ParseWndProc2(hWnd, sellist_msgtbl3, 4, 0);
				else
					Doom_ParseWndProc2(hWnd, sellist_msgtbl3, 5, 0);
				v17 = GetParent(hWnd);
				SelHero_PrintHeroInfo(v17, v16);
			}
			else
			{
				Doom_ParseWndProc2(hWnd, sellist_msgtbl3, 5, 0);
				v18 = GetParent(hWnd);
				selhero_hero_hassaved = 0;
				selhero_heronamestr[0] = 0;
				v2 = v18;
				v3 = GetDlgItem(v18, 1014);
				v4 = GetWindowLongA(v3, -21);
				local_SetWndLongStr(v4, "--");
				v5 = GetDlgItem(v2, 1018);
				v6 = GetWindowLongA(v5, -21);
				local_SetWndLongStr(v6, "--");
				v7 = GetDlgItem(v2, 1017);
				v8 = GetWindowLongA(v7, -21);
				local_SetWndLongStr(v8, "--");
				v9 = GetDlgItem(v2, 1016);
				v10 = GetWindowLongA(v9, -21);
				local_SetWndLongStr(v10, "--");
				v11 = GetDlgItem(v2, 1015);
				v12 = GetWindowLongA(v11, -21);
				local_SetWndLongStr(v12, "--");
				SelHero_SetStaticBMP(v2, 3);
				Doom_ParseWndProc4(v2, selhero_msgtbl_info, 1);
			}
		}
	}
}
// 1002A424: using guessed type int selhero_hero_hassaved;

// ref: 0x1000D9CF
void __fastcall SelList_CountHeroList(HWND hWnd)
{
	HWND v2; // eax
	int v3; // ST04_4
	int v4; // eax

	v2 = GetFocus();
	v3 = SelList_GetNextHeroLong(v2);
	v4 = SelHero_GetNumHeroesLeft();
	Sbar_DrawScrollBar(hWnd, 1105, v4, v3);
}

// ref: 0x1000D9F4
int __fastcall SelList_GetNextHeroLong(HWND hWnd)
{
	LONG v1; // esi
	_uiheroinfo *v2; // eax
	_uiheroinfo *v3; // esi
	int v5; // ecx

	if ( !hWnd )
		return 0;
	v1 = GetWindowLongA(hWnd, -21);
	if ( !v1 )
		return 0;
	v2 = SelHero_GetCurrentHeroInfo();
	if ( !v2 )
		return 0;
	v3 = *(_uiheroinfo **)(v1 + 12);
	if ( !v3 )
		return 0;
	v5 = 0;
	do
	{
		if ( v2 == v3 )
			break;
		v2 = v2->next;
		++v5;
	}
	while ( v2 );
	return v5;
}

// ref: 0x1000DA2D
void __fastcall SelList_LoadFocus16(HWND hWnd)
{
	Focus_LoadSpinner("ui_art\\focus16.pcx");
	SDlgSetTimer((int)hWnd, 1, 55, 0);
}

// ref: 0x1000DA48
void __fastcall SelList_KillFocus16(HWND hWnd)
{
	SDlgKillTimer((int)hWnd, 1);
	Focus_DeleteSpinners();
}

// ref: 0x1000DA55
void __fastcall SelList_ShowListWindow(HWND hWnd)
{
	HWND v2; // edi
	LONG v3; // eax
	HWND v4; // eax
	char Buffer[32]; // [esp+8h] [ebp-20h]

	v2 = GetParent(hWnd);
	SelList_DoListOldProc(hWnd);
	if ( SelHero_GetHeroIsGood() == 1 )
		LoadStringA(ghUiInst, 0x1Cu, Buffer, 31);
	else
		LoadStringA(ghUiInst, 0x1Du, Buffer, 31);
	SelHero_SetStringWithMsg(v2, Buffer);
	v3 = GetWindowLongA(v2, -21);
	SetWindowLongA(hWnd, -21, v3);
	Doom_ParseWndProc3(hWnd, sellist_msgtbl1, 5);
	Doom_ParseWndProcs(hWnd, sellist_msgtbl2, 4, 0);
	Doom_ParseWndProcs(hWnd, sellist_msgtbl3, 4, 0);
	Doom_ParseWndProcs(hWnd, sellist_msgtbl4, 2, 1);
	sellist_pheroinfo = SelHero_GetCurrentHeroInfo();
	SelList_SetHeroDlgLong(hWnd, sellist_pheroinfo);
	Sbar_LoadScrBarGFX(hWnd, 1105);
	if ( SelHero_GetNumHeroesLeft() <= 6 )
	{
		v4 = GetDlgItem(hWnd, 1105);
		ShowWindow(v4, 0);
	}
}

// ref: 0x1000DB2C
void __fastcall SelList_SetHeroDlgLong(HWND hWnd, _uiheroinfo *pInfo)
{
	int *i; // ebp
	HWND v4; // eax MAPDST
	int v6; // esi

	for ( i = sellist_msgtbl4; *i; ++i )
	{
		v4 = GetDlgItem(hWnd, *i);
		if ( v4 )
		{
			if ( pInfo )
			{
				EnableWindow(v4, 1);
				v6 = GetWindowLongA(v4, -21);
				local_SetWndLongStr(v6, pInfo->name);
				if ( v6 )
					*(_DWORD *)(v6 + 12) = (unsigned int)pInfo;
				pInfo = pInfo->next;
			}
			else
			{
				EnableWindow(v4, 0);
			}
		}
	}
	Doom_ParseWndProc2(hWnd, sellist_msgtbl4, 2, 1);
}

// ref: 0x1000DBAC
void __fastcall SelList_DoListOldProc(HWND hWnd)
{
	int *i; // edi
	HWND v3; // eax MAPDST
	void *v5; // eax

	for ( i = sellist_msgtbl4; *i; ++i )
	{
		v3 = GetDlgItem(hWnd, *i);
		if ( v3 )
		{
			v5 = (void *)GetWindowLongA(v3, -4);
			SetPropA(v3, "UIOLDPROC", v5);
			SetWindowLongA(v3, -4, (LONG)SelList_OldListWndProc);
		}
	}
}

// ref: 0x1000DBFE
LRESULT __stdcall SelList_OldListWndProc(HWND hWnd, UINT Msg, WPARAM wParam, LPARAM lParam)
{
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
			local_DlgDoPaint(hWnd);
			return 0;
		case 0x87u:
			return 4;
		case 0x100u:
			if ( wParam > 0x21 )
			{
				if ( wParam == 34 )
				{
					SelList_HeroesWithBigDialogs(hWnd);
					return 0;
				}
				if ( wParam > 0x24 )
				{
					if ( wParam <= 0x26 )
					{
						SelList_HeroDlgWithSnd2(hWnd);
						return 0;
					}
					if ( wParam <= 0x28 )
					{
						SelList_HeroDlgWithSound(hWnd);
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
						SelList_HeroesWithHugeDlg(hWnd);
						break;
					case 9u:
						if ( GetKeyState(16) >= 0 )
							SelList_ShiftHeroDlgItems(hWnd);
						else
							SelList_ShiftHeroDlgItm2(hWnd);
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
}

// ref: 0x1000DD36
void __fastcall SelList_ShiftHeroDlgItems(HWND hWnd)
{
	HWND v2; // ebx
	int nIDDlgItem[1053]; // [esp+0h] [ebp-1074h]

	nIDDlgItem[1047] = 1048;
	nIDDlgItem[1048] = 1049;
	nIDDlgItem[1049] = 1050;
	nIDDlgItem[1050] = 1051;
	nIDDlgItem[1051] = 1052;
	nIDDlgItem[1052] = 1047;
	v2 = GetParent(hWnd);
	do
	{
		nIDDlgItem[1044] = nIDDlgItem[GetWindowLongA(hWnd, -12)];
		hWnd = GetDlgItem(v2, nIDDlgItem[1044]);
	}
	while ( !IsWindowEnabled(hWnd) );
	SetFocus(hWnd);
}

// ref: 0x1000DDA7
void __fastcall SelList_ShiftHeroDlgItm2(HWND hWnd)
{
	HWND v2; // ebx
	int nIDDlgItem[1053]; // [esp+0h] [ebp-1074h]

	nIDDlgItem[1047] = 1052;
	nIDDlgItem[1048] = 1047;
	nIDDlgItem[1049] = 1048;
	nIDDlgItem[1050] = 1049;
	nIDDlgItem[1051] = 1050;
	nIDDlgItem[1052] = 1051;
	v2 = GetParent(hWnd);
	do
	{
		nIDDlgItem[1044] = nIDDlgItem[GetWindowLongA(hWnd, -12)];
		hWnd = GetDlgItem(v2, nIDDlgItem[1044]);
	}
	while ( !IsWindowEnabled(hWnd) );
	SetFocus(hWnd);
}

// ref: 0x1000DE18
void __fastcall SelList_HeroesWithBigDialogs(HWND hWnd)
{
	HWND v1; // eax MAPDST
	HWND v3; // ebp
	HWND v4; // eax
	LONG v5; // eax
	_uiheroinfo *v6; // eax
	int v7; // esi
	_uiheroinfo *v8; // esi
	int v9; // eax

	v1 = GetParent(hWnd);
	if ( v1 )
	{
		v3 = GetDlgItem(v1, 1047);
		if ( v3 )
		{
			v4 = GetDlgItem(v1, 1052);
			v5 = GetWindowLongA(v4, -21);
			if ( v5 )
			{
				v6 = *(_uiheroinfo **)(v5 + 12);
				if ( v6 && v6->next )
				{
					v7 = SelList_GetNextHeroLong(v3) + 6;
					if ( v7 > SelHero_GetNumHeroesLeft() - 6 )
						v7 = SelHero_GetNumHeroesLeft() - 6;
					v8 = SelList_GetHeroFromNum(v7);
					if ( v8 )
					{
						TitleSnd_PlayMoveSound();
						SelList_SetHeroDlgLong(v1, v8);
						v9 = GetWindowLongA(hWnd, -12);
						SelList_GetHeroStats(v1, v9);
						SelList_CountHeroList(v1);
					}
				}
				else
				{
					SelList_ShiftHeroDlgItm2(v3);
				}
			}
		}
	}
}

// ref: 0x1000DEDD
_uiheroinfo *__fastcall SelList_GetHeroFromNum(int heronum)
{
	_uiheroinfo *result; // eax

	result = SelHero_GetCurrentHeroInfo();
	while ( result && heronum )
	{
		result = result->next;
		--heronum;
	}
	return result;
}

// ref: 0x1000DEF4
void __fastcall SelList_HeroesWithHugeDlg(HWND hWnd)
{
	HWND v1; // eax MAPDST
	HWND v3; // eax MAPDST
	LONG v5; // eax
	_uiheroinfo *v6; // ebp
	HWND v7; // eax
	int v8; // eax
	_uiheroinfo *v9; // edi
	int v10; // eax

	v1 = GetParent(hWnd);
	if ( v1 )
	{
		v3 = GetDlgItem(v1, 1047);
		if ( v3 )
		{
			v5 = GetWindowLongA(v3, -21);
			if ( v5 )
			{
				v6 = *(_uiheroinfo **)(v5 + 12);
				if ( v6 )
				{
					if ( v6 == SelHero_GetCurrentHeroInfo() )
					{
						v7 = GetDlgItem(v1, 1052);
						SelList_ShiftHeroDlgItems(v7);
					}
					else
					{
						v8 = SelList_GetNextHeroLong(v3) - 6;
						if ( v8 < 0 )
							v8 = 0;
						v9 = SelList_GetHeroFromNum(v8);
						if ( v9 )
						{
							TitleSnd_PlayMoveSound();
							SelList_SetHeroDlgLong(v1, v9);
							v10 = GetWindowLongA(hWnd, -12);
							SelList_GetHeroStats(v1, v10);
							SelList_CountHeroList(v1);
						}
					}
				}
			}
		}
	}
}

// ref: 0x1000DFAB
void __fastcall SelList_HeroDlgWithSound(HWND hWnd)
{
	LONG v2; // eax
	_uiheroinfo *v3; // eax
	HWND v4; // eax
	HWND v5; // eax
	LONG v6; // eax
	_uiheroinfo *v7; // ebp
	HWND v8; // eax
	int v9; // ebx
	HWND v10; // eax
	HWND v11; // eax

	v2 = GetWindowLongA(hWnd, -21);
	if ( v2 )
	{
		v3 = *(_uiheroinfo **)(v2 + 12);
		if ( v3 )
		{
			if ( v3->next )
			{
				if ( GetWindowLongA(hWnd, -12) >= 1052 )
				{
					v4 = GetParent(hWnd);
					v5 = GetDlgItem(v4, 1048);
					if ( v5 )
					{
						v6 = GetWindowLongA(v5, -21);
						if ( v6 )
						{
							v7 = *(_uiheroinfo **)(v6 + 12);
							if ( v7 )
							{
								TitleSnd_PlayMoveSound();
								v8 = GetParent(hWnd);
								SelList_SetHeroDlgLong(v8, v7);
								v9 = GetWindowLongA(hWnd, -12);
								v10 = GetParent(hWnd);
								SelList_GetHeroStats(v10, v9);
								v11 = GetParent(hWnd);
								SelList_CountHeroList(v11);
							}
						}
					}
				}
				else
				{
					SelList_ShiftHeroDlgItems(hWnd);
				}
			}
		}
	}
}

// ref: 0x1000E043
void __fastcall SelList_HeroDlgWithSnd2(HWND hWnd)
{
	LONG v2; // eax
	_uiheroinfo *v3; // esi
	_uiheroinfo *v4; // ebx
	HWND v5; // eax
	int v6; // ebx
	HWND v7; // eax
	HWND v8; // eax

	if ( GetWindowLongA(hWnd, -12) <= 1047 )
	{
		v2 = GetWindowLongA(hWnd, -21);
		if ( v2 )
		{
			v3 = *(_uiheroinfo **)(v2 + 12);
			if ( v3 )
			{
				v4 = SelHero_GetCurrentHeroInfo();
				if ( v3 != v4 )
				{
					while ( v4 && v4->next != v3 )
						v4 = v4->next;
					TitleSnd_PlayMoveSound();
					v5 = GetParent(hWnd);
					SelList_SetHeroDlgLong(v5, v4);
					v6 = GetWindowLongA(hWnd, -12);
					v7 = GetParent(hWnd);
					SelList_GetHeroStats(v7, v6);
					v8 = GetParent(hWnd);
					SelList_CountHeroList(v8);
				}
			}
		}
	}
	else
	{
		SelList_ShiftHeroDlgItm2(hWnd);
	}
}

// ref: 0x1000E0CA
void __fastcall SelList_ChooseDlgFromSize(HWND hWnd, int width, int height)
{
	HWND v6; // eax
	int v7; // edx
	HWND v8; // eax
	HWND v9; // eax
	char *v10; // eax
	HWND v11; // eax
	HWND v12; // eax
	int v13; // eax
	int v14; // eax
	int v15; // eax
	HWND v16; // eax
	HWND v17; // eax
	HWND v18; // eax
	HWND v19; // eax

	v6 = GetDlgItem(hWnd, 1056);
	if ( local_GetBottomRect(hWnd, v6, width, height) )
	{
		v7 = 1;
LABEL_3:
		OkCancel_PlaySndEndDlg(hWnd, v7);
		return;
	}
	v8 = GetDlgItem(hWnd, 1054);
	if ( local_GetBottomRect(hWnd, v8, width, height) )
	{
		v7 = 2;
		goto LABEL_3;
	}
	v9 = GetDlgItem(hWnd, 1006);
	if ( local_GetBottomRect(hWnd, v9, width, height) )
	{
		v10 = SelHero_GetHeroNameStr();
		if ( strlen(v10) )
		{
			v7 = 1006;
			goto LABEL_3;
		}
	}
	else
	{
		v11 = GetDlgItem(hWnd, 1105);
		if ( local_GetBottomRect(hWnd, v11, width, height) )
		{
			v12 = GetDlgItem(hWnd, 1105);
			v13 = Sbar_NumScrollLines(v12, width, height) - 1;
			if ( v13 )
			{
				v14 = v13 - 1;
				if ( v14 )
				{
					v15 = v14 - 1;
					if ( v15 )
					{
						if ( v15 == 1 )
						{
							v16 = GetFocus();
							SelList_HeroesWithBigDialogs(v16);
						}
					}
					else
					{
						v17 = GetFocus();
						SelList_HeroesWithHugeDlg(v17);
					}
				}
				else
				{
					v18 = GetFocus();
					SelList_HeroDlgWithSound(v18);
				}
			}
			else
			{
				v19 = GetFocus();
				SelList_HeroDlgWithSnd2(v19);
			}
		}
	}
}

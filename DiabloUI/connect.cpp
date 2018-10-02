// ref: 0x10002E2B
void __cdecl Connect_FreeConnectData()
{
	HANDLE *v0; // edi

	if ( connect_data1 )
	{
		SMemFree(connect_data1, "C:\\Src\\Diablo\\DiabloUI\\Connect.cpp", 124, 0);
		connect_data1 = 0;
	}
	if ( connect_data2 )
	{
		SMemFree(connect_data2, "C:\\Src\\Diablo\\DiabloUI\\Connect.cpp", 129, 0);
		connect_data2 = 0;
	}
	v0 = connect_trans;
	do
	{
		if ( *v0 )
		{
			STransDelete(*v0);
			*v0 = 0;
		}
		++v0;
	}
	while ( (signed int)v0 < (signed int)&connect_trans[10] );
	if ( connect_data3 )
	{
		SMemFree(connect_data3, "C:\\Src\\Diablo\\DiabloUI\\Connect.cpp", 141, 0);
		connect_data3 = 0;
	}
	if ( connect_data4 )
	{
		SMemFree(connect_data4, "C:\\Src\\Diablo\\DiabloUI\\Connect.cpp", 146, 0);
		connect_data4 = 0;
	}
}

// ref: 0x10002EC4
BOOL __cdecl Connect_LoadGFXAndStuff()
{
	HANDLE *v0; // esi
	int v1; // ebx
	int a5[4]; // [esp+4h] [ebp-14h]
	BYTE *a2; // [esp+14h] [ebp-4h]

	a2 = 0;
	if ( !connect_data1 && !connect_trans[0] )
	{
		local_LoadArtImage("ui_art\\heroport.pcx", &connect_data1, heroport_data);
		local_LoadArtImage("ui_art\\spwnport.pcx", &connect_data2, 0);
		local_LoadArtImage("ui_art\\heronum.pcx", &a2, heronum_data);
		local_LoadArtImage("ui_art\\special.pcx", &connect_data3, special_data);
		connect_draw_height = 14;
		heronum_frames = (signed int)heronum_data[1] / 10;
		special_frames = (signed int)special_data[1] / 8;
		heronum_frames2 = (signed int)heronum_data[1] / 10 * heronum_data[0];
		heroport_frames = 14 * heroport_data[0];
		v0 = connect_trans;
		connect_data4 = SMemAlloc(14 * heroport_data[0], "C:\\Src\\Diablo\\DiabloUI\\Connect.cpp", 194, 0);
		memset(connect_trans, 0, 0x28u);
		if ( a2 )
		{
			v1 = 0;
			do
			{
				a5[0] = 0;
				a5[2] = heronum_data[0] - 1;
				a5[1] = v1 * heronum_frames;
				a5[3] = heronum_frames + v1 * heronum_frames - 1;
				STransCreateI(a2, heronum_data[0], heronum_frames, 8, (int)a5, 16777455, v0);
				++v0;
				++v1;
			}
			while ( (signed int)v0 < (signed int)&connect_trans[10] );
			SMemFree(a2, "C:\\Src\\Diablo\\DiabloUI\\Connect.cpp", 218, 0);
		}
	}
	return 1;
}
// 100294A4: using guessed type int special_frames;
// 100295B0: using guessed type int heronum_frames2;
// 100295C0: using guessed type int heroport_frames;

// ref: 0x10003009
BOOL __stdcall UiArtCallback(int game_type, unsigned int art_code, PALETTEENTRY *pPalette, void *pBuffer, DWORD dwBuffersize, DWORD *pdwWidth, DWORD *pdwHeight, DWORD *pdwBpp)
{
	BOOL result; // eax
	char pszFileName[260]; // [esp+8h] [ebp-104h]

	pszFileName[0] = nullcharacter;
	memset(&pszFileName[1], 0, 0x100u);
	*(_WORD *)&pszFileName[257] = 0;
	pszFileName[259] = 0;
	SStrCopy(pszFileName, "ui_art\\", 260);
	if ( game_type == 'BNET' )
	{
		if ( art_code > 0x80000004 )
		{
			switch ( art_code )
			{
				case 0x80000005:
					SStrPack(pszFileName, "bnconnbg.pcx", 0x104u);
					return SBmpLoadImage(pszFileName, pPalette, pBuffer, dwBuffersize, pdwWidth, pdwHeight, pdwBpp);
				case 0x80000006:
					SStrPack(pszFileName, "bnselchn.pcx", 0x104u);
					return SBmpLoadImage(pszFileName, pPalette, pBuffer, dwBuffersize, pdwWidth, pdwHeight, pdwBpp);
				case 0x80000007:
					SStrPack(pszFileName, "bnlogin.pcx", 0x104u);
					return SBmpLoadImage(pszFileName, pPalette, pBuffer, dwBuffersize, pdwWidth, pdwHeight, pdwBpp);
				case 0x80000008:
					SStrPack(pszFileName, "newaccount.pcx", 0x104u);
					return SBmpLoadImage(pszFileName, pPalette, pBuffer, dwBuffersize, pdwWidth, pdwHeight, pdwBpp);
				case 0x80000009:
					SStrPack(pszFileName, "changepassword.pcx", 0x104u);
					return SBmpLoadImage(pszFileName, pPalette, pBuffer, dwBuffersize, pdwWidth, pdwHeight, pdwBpp);
				case 0x8000000A:
					SStrPack(pszFileName, "bnladder.pcx", 0x104u);
					return SBmpLoadImage(pszFileName, pPalette, pBuffer, dwBuffersize, pdwWidth, pdwHeight, pdwBpp);
				case 0x8000000B:
					SStrPack(pszFileName, "badconn.pcx", 0x104u);
					return SBmpLoadImage(pszFileName, pPalette, pBuffer, dwBuffersize, pdwWidth, pdwHeight, pdwBpp);
				case 0x8000000C:
					SStrPack(pszFileName, "welcome.pcx", 0x104u);
					return SBmpLoadImage(pszFileName, pPalette, pBuffer, dwBuffersize, pdwWidth, pdwHeight, pdwBpp);
				case 0x8000000D:
					SStrPack(pszFileName, "lepopup.pcx", 0x104u);
					return SBmpLoadImage(pszFileName, pPalette, pBuffer, dwBuffersize, pdwWidth, pdwHeight, pdwBpp);
				case 0x8000000E:
					SStrPack(pszFileName, "tos.pcx", 0x104u);
					return SBmpLoadImage(pszFileName, pPalette, pBuffer, dwBuffersize, pdwWidth, pdwHeight, pdwBpp);
				default:
					break;
			}
		}
		else
		{
			if ( art_code == 0x80000004 )
			{
				SStrPack(pszFileName, "redlag.pcx", 0x104u);
				return SBmpLoadImage(pszFileName, pPalette, pBuffer, dwBuffersize, pdwWidth, pdwHeight, pdwBpp);
			}
			if ( art_code > 7 )
			{
				switch ( art_code )
				{
					case 8u:
LABEL_48:
						SStrPack(pszFileName, "but_lrg.pcx", 0x104u);
						return SBmpLoadImage(pszFileName, pPalette, pBuffer, dwBuffersize, pdwWidth, pdwHeight, pdwBpp);
					case 0x80000000:
						SStrPack(pszFileName, "bnbuttns.pcx", 0x104u);
						return SBmpLoadImage(pszFileName, pPalette, pBuffer, dwBuffersize, pdwWidth, pdwHeight, pdwBpp);
					case 0x80000001:
						SStrPack(pszFileName, "chat_bkg.pcx", 0x104u);
						return SBmpLoadImage(pszFileName, pPalette, pBuffer, dwBuffersize, pdwWidth, pdwHeight, pdwBpp);
					case 0x80000002:
						SStrPack(pszFileName, "greenlag.pcx", 0x104u);
						return SBmpLoadImage(pszFileName, pPalette, pBuffer, dwBuffersize, pdwWidth, pdwHeight, pdwBpp);
					case 0x80000003:
						SStrPack(pszFileName, "yellolag.pcx", 0x104u);
						return SBmpLoadImage(pszFileName, pPalette, pBuffer, dwBuffersize, pdwWidth, pdwHeight, pdwBpp);
				}
			}
			else
			{
				switch ( art_code )
				{
					case 7u:
LABEL_47:
						SStrPack(pszFileName, "but_med.pcx", 0x104u);
						return SBmpLoadImage(pszFileName, pPalette, pBuffer, dwBuffersize, pdwWidth, pdwHeight, pdwBpp);
					case 0u:
						SStrPack(pszFileName, "bn_bkg.pcx", 0x104u);
						return SBmpLoadImage(pszFileName, pPalette, pBuffer, dwBuffersize, pdwWidth, pdwHeight, pdwBpp);
					case 2u:
						SStrPack(pszFileName, "bnjoinbg.pcx", 0x104u);
						return SBmpLoadImage(pszFileName, pPalette, pBuffer, dwBuffersize, pdwWidth, pdwHeight, pdwBpp);
					case 3u:
						SStrPack(pszFileName, "hpopup.pcx", 0x104u);
						return SBmpLoadImage(pszFileName, pPalette, pBuffer, dwBuffersize, pdwWidth, pdwHeight, pdwBpp);
					case 5u:
LABEL_46:
						SStrPack(pszFileName, "but_xsm.pcx", 0x104u);
						return SBmpLoadImage(pszFileName, pPalette, pBuffer, dwBuffersize, pdwWidth, pdwHeight, pdwBpp);
					case 6u:
LABEL_11:
						SStrPack(pszFileName, "but_sml.pcx", 0x104u);
						return SBmpLoadImage(pszFileName, pPalette, pBuffer, dwBuffersize, pdwWidth, pdwHeight, pdwBpp);
				}
			}
		}
	}
	else if ( game_type == 'IPXN' )
	{
		if ( !art_code )
		{
			SStrPack(pszFileName, "ipx_bkg.pcx", 0x104u);
			return SBmpLoadImage(pszFileName, pPalette, pBuffer, dwBuffersize, pdwWidth, pdwHeight, pdwBpp);
		}
	}
	else if ( !game_type && !art_code )
	{
		SStrPack(pszFileName, "connect.pcx", 0x104u);
		return SBmpLoadImage(pszFileName, pPalette, pBuffer, dwBuffersize, pdwWidth, pdwHeight, pdwBpp);
	}
	switch ( art_code )
	{
		case 0u:
		case 2u:
			SStrPack(pszFileName, "menu.pcx", 0x104u);
			return SBmpLoadImage(pszFileName, pPalette, pBuffer, dwBuffersize, pdwWidth, pdwHeight, pdwBpp);
		case 1u:
		case 8u:
			goto LABEL_48;
		case 3u:
		case 4u:
			SStrPack(pszFileName, "lpopup.pcx", 0x104u);
			return SBmpLoadImage(pszFileName, pPalette, pBuffer, dwBuffersize, pdwWidth, pdwHeight, pdwBpp);
		case 5u:
			goto LABEL_46;
		case 6u:
			goto LABEL_11;
		case 7u:
			goto LABEL_47;
		case 9u:
			SStrPack(pszFileName, "xsmlogo.pcx", 0x104u);
			return SBmpLoadImage(pszFileName, pPalette, pBuffer, dwBuffersize, pdwWidth, pdwHeight, pdwBpp);
		case 0xAu:
			SStrPack(pszFileName, "prog_bg.pcx", 0x104u);
			return SBmpLoadImage(pszFileName, pPalette, pBuffer, dwBuffersize, pdwWidth, pdwHeight, pdwBpp);
		case 0xBu:
			SStrPack(pszFileName, "prog_fil.pcx", 0x104u);
			return SBmpLoadImage(pszFileName, pPalette, pBuffer, dwBuffersize, pdwWidth, pdwHeight, pdwBpp);
		case 0xCu:
			SStrPack(pszFileName, "spopup.pcx", 0x104u);
			return SBmpLoadImage(pszFileName, pPalette, pBuffer, dwBuffersize, pdwWidth, pdwHeight, pdwBpp);
		case 0xDu:
			SStrPack(pszFileName, "scrlarrw.pcx", 0x104u);
			return SBmpLoadImage(pszFileName, pPalette, pBuffer, dwBuffersize, pdwWidth, pdwHeight, pdwBpp);
		case 0xEu:
			SStrPack(pszFileName, "scrlthmb.pcx", 0x104u);
			return SBmpLoadImage(pszFileName, pPalette, pBuffer, dwBuffersize, pdwWidth, pdwHeight, pdwBpp);
		case 0xFu:
			SStrPack(pszFileName, "scrlbar.pcx", 0x104u);
			return SBmpLoadImage(pszFileName, pPalette, pBuffer, dwBuffersize, pdwWidth, pdwHeight, pdwBpp);
		case 0x10u:
			SStrPack(pszFileName, "cmel.pcx", 0x104u);
			return SBmpLoadImage(pszFileName, pPalette, pBuffer, dwBuffersize, pdwWidth, pdwHeight, pdwBpp);
		case 0x11u:
			SStrPack(pszFileName, "cmml.pcx", 0x104u);
			return SBmpLoadImage(pszFileName, pPalette, pBuffer, dwBuffersize, pdwWidth, pdwHeight, pdwBpp);
		case 0x12u:
			SStrPack(pszFileName, "cmbl.pcx", 0x104u);
			return SBmpLoadImage(pszFileName, pPalette, pBuffer, dwBuffersize, pdwWidth, pdwHeight, pdwBpp);
		case 0x13u:
			SStrPack(pszFileName, "cmec.pcx", 0x104u);
			return SBmpLoadImage(pszFileName, pPalette, pBuffer, dwBuffersize, pdwWidth, pdwHeight, pdwBpp);
		case 0x14u:
			SStrPack(pszFileName, "cmmc.pcx", 0x104u);
			return SBmpLoadImage(pszFileName, pPalette, pBuffer, dwBuffersize, pdwWidth, pdwHeight, pdwBpp);
		case 0x15u:
			SStrPack(pszFileName, "cmbc.pcx", 0x104u);
			return SBmpLoadImage(pszFileName, pPalette, pBuffer, dwBuffersize, pdwWidth, pdwHeight, pdwBpp);
		case 0x16u:
			SStrPack(pszFileName, "cmer.pcx", 0x104u);
			return SBmpLoadImage(pszFileName, pPalette, pBuffer, dwBuffersize, pdwWidth, pdwHeight, pdwBpp);
		case 0x17u:
			SStrPack(pszFileName, "cmmr.pcx", 0x104u);
			return SBmpLoadImage(pszFileName, pPalette, pBuffer, dwBuffersize, pdwWidth, pdwHeight, pdwBpp);
		case 0x18u:
			SStrPack(pszFileName, "cmbr.pcx", 0x104u);
			return SBmpLoadImage(pszFileName, pPalette, pBuffer, dwBuffersize, pdwWidth, pdwHeight, pdwBpp);
		case 0x19u:
			SStrPack(pszFileName, "slgray.pcx", 0x104u);
			return SBmpLoadImage(pszFileName, pPalette, pBuffer, dwBuffersize, pdwWidth, pdwHeight, pdwBpp);
		case 0x1Au:
			SStrPack(pszFileName, "slthumb.pcx", 0x104u);
			return SBmpLoadImage(pszFileName, pPalette, pBuffer, dwBuffersize, pdwWidth, pdwHeight, pdwBpp);
		case 0x1Bu:
			SStrPack(pszFileName, "slfocus.pcx", 0x104u);
			return SBmpLoadImage(pszFileName, pPalette, pBuffer, dwBuffersize, pdwWidth, pdwHeight, pdwBpp);
		case 0x1Cu:
			SStrPack(pszFileName, "slleft.pcx", 0x104u);
			return SBmpLoadImage(pszFileName, pPalette, pBuffer, dwBuffersize, pdwWidth, pdwHeight, pdwBpp);
		case 0x1Du:
			SStrPack(pszFileName, "slmiddle.pcx", 0x104u);
			return SBmpLoadImage(pszFileName, pPalette, pBuffer, dwBuffersize, pdwWidth, pdwHeight, pdwBpp);
		case 0x1Eu:
			SStrPack(pszFileName, "slright.pcx", 0x104u);
			return SBmpLoadImage(pszFileName, pPalette, pBuffer, dwBuffersize, pdwWidth, pdwHeight, pdwBpp);
		case 0x20u:
			SStrPack(pszFileName, "but_checkoff.pcx", 0x104u);
			return SBmpLoadImage(pszFileName, pPalette, pBuffer, dwBuffersize, pdwWidth, pdwHeight, pdwBpp);
		case 0x21u:
			SStrPack(pszFileName, "but_checkon.pcx", 0x104u);
			return SBmpLoadImage(pszFileName, pPalette, pBuffer, dwBuffersize, pdwWidth, pdwHeight, pdwBpp);
		default:
			result = 0;
			break;
	}
	return result;
}
// 100103E2: using guessed type unsigned int __stdcall SStrPack(char *, const char *, unsigned int);

// ref: 0x100033D1
void __cdecl Connect_cpp_init()
{
	Connect_cpp_float = Connect_cpp_float_value;
}
// 1001F3F4: using guessed type int Connect_cpp_float_value;
// 100294A0: using guessed type int Connect_cpp_float;

// ref: 0x100033DC
BOOL __stdcall UiGetDataCallback(int game_type, int data_code, void *a3, int a4, int a5)
{
	signed int v5; // edi
	_DWORD *v6; // esi
	HCURSOR v7; // eax

	v5 = 0;
	if ( game_type != 'BNET' || data_code != 0x80000000 )
	{
		switch ( data_code )
		{
			case 1:
				v5 = 16;
				if ( !a3 )
					goto LABEL_24;
				if ( (unsigned int)a4 >= 0x10 )
				{
					memcpy(a3, connect_subnet_ip, 0x10u);
					goto LABEL_24;
				}
				return 0;
			case 2:
				v6 = (unsigned int *)a3;
				v5 = 4;
				if ( !a3 )
					goto LABEL_24;
				if ( (unsigned int)a4 < 4 )
					return 0;
				v7 = LoadCursorA(ghUiInst, "DIABLO_LINKCURSOR");
				break;
			case 3:
				v6 = (unsigned int *)a3;
				v5 = 4;
				if ( !a3 )
					goto LABEL_24;
				if ( (unsigned int)a4 < 4 )
					return 0;
				v7 = LoadCursorA(ghUiInst, "DIABLO_ARROWCURSOR");
				break;
			case 4:
				v6 = (unsigned int *)a3;
				v5 = 4;
				if ( !a3 )
					goto LABEL_24;
				if ( (unsigned int)a4 < 4 )
					return 0;
				v7 = LoadCursorA(ghUiInst, "DIABLOIBEAM");
				break;
			default:
				goto LABEL_24;
		}
		*v6 = (unsigned int)v7;
		if ( v7 )
			goto LABEL_24;
		return 0;
	}
	v5 = 4;
	if ( a3 )
	{
		if ( (unsigned int)a4 >= 4 )
		{
			*(_DWORD *)a3 = 54;
			goto LABEL_24;
		}
		return 0;
	}
LABEL_24:
	if ( a5 )
		*(_DWORD *)a5 = v5;
	return v5 != 0;
}

// ref: 0x100034AB
BOOL __stdcall UiSoundCallback(int a1, int type, int a3)
{
	if ( type )
	{
		if ( type == 1 )
			TitleSnd_PlaySelectSound();
	}
	else
	{
		TitleSnd_PlayMoveSound();
	}
	return 0;
}

// ref: 0x100034C8
BOOL __stdcall UiAuthCallback(int a1, char *a2, char *a3, char a4, char *a5, LPSTR lpBuffer, int cchBufferMax)
{
	size_t v7; // edi
	size_t v8; // ebx
	int v9; // ebx
	char *v10; // ebx
	size_t v11; // eax
	int v12; // eax
	int v14; // eax
	char v15[256]; // [esp+0h] [ebp-434h]
	char a1a[256]; // [esp+100h] [ebp-334h]
	char v17[256]; // [esp+200h] [ebp-234h]
	char Buffer[256]; // [esp+300h] [ebp-134h]
	_uiheroinfo heroinfo; // [esp+400h] [ebp-34h]
	_gamedata GameData; // [esp+42Ch] [ebp-8h]

	*(_DWORD *)&GameData.bDiff = 0;
	if ( cchBufferMax )
		*lpBuffer = 0;
	v7 = strlen(a3) + 1;
	v8 = strlen(a5) + 1;
	if ( v7 > 0x100 || v8 > 0x100 )
	{
		if ( lpBuffer )
			LoadStringA(ghUiInst, 0x413u, lpBuffer, cchBufferMax);
		return 0;
	}
	memcpy(a1a, a3, v7);
	memcpy(v17, a5, v8);
	if ( Connect_GetHeroInfoConc(a1a, &heroinfo) )
	{
		if ( a1 == 1 )
		{
			if ( !(a4 & 9) )
			{
				v9 = 0;
				while ( 1 )
				{
					LoadStringA(ghUiInst, v9 + 4, Buffer, 256);
					if ( strstr(v17, Buffer) )
						break;
					if ( ++v9 >= 3 )
						goto LABEL_16;
				}
				if ( heroinfo.heroclass != v9 )
					goto LABEL_20;
				*(_DWORD *)&GameData.bDiff = 1;
LABEL_16:
				LoadStringA(ghUiInst, 0x408u, Buffer, 256);
				v10 = strstr(v17, Buffer);
				if ( v10 )
				{
					v11 = strlen(Buffer);
					v12 = atoi(&v10[v11]);
					if ( heroinfo.level >= v12 )
						return 1;
				}
				if ( *(_DWORD *)&GameData.bDiff )
					return 1;
LABEL_20:
				if ( lpBuffer )
				{
					LoadStringA(ghUiInst, 0x415u, v15, 256);
					v14 = sprintf(Buffer, v15, v17) + 1;
					if ( cchBufferMax >= v14 )
					{
						memcpy(lpBuffer, Buffer, v14);
					}
					else
					{
						memcpy(lpBuffer, Buffer, cchBufferMax);
						lpBuffer[cchBufferMax - 1] = 0;
					}
				}
				return 0;
			}
		}
		else if ( !(a4 & 8) )
		{
			Connect_DiffFromString(v17, &GameData, 0, 0);
			if ( GameData.bDiff == 1 )
			{
				if ( heroinfo.level < 20u )
				{
					if ( lpBuffer )
						LoadStringA(ghUiInst, 0x411u, lpBuffer, cchBufferMax);
					return 0;
				}
			}
			else if ( GameData.bDiff == 2 && heroinfo.level < 30u )
			{
				if ( lpBuffer )
					LoadStringA(ghUiInst, 0x412u, lpBuffer, cchBufferMax);
				return 0;
			}
		}
		return 1;
	}
	if ( lpBuffer )
		LoadStringA(ghUiInst, 0x414u, lpBuffer, cchBufferMax);
	return 0;
}

// ref: 0x10003710
BOOL __stdcall UiDrawDescCallback(int arg0, COLORREF color, LPCSTR lpString, char *a4, int a5, UINT align, time_t a7, HDC *a8)
{
	HDC *v8; // ebx
	HDC v9; // esi
	int v10; // esi
	size_t v11; // eax
	UINT v13; // eax
	BOOL v14; // esi
	int v15; // eax
	int v16; // eax
	HDC v17; // ST20_4
	struct tm *v18; // eax
	struct tm *v19; // edi
	signed int v20; // eax
	int v21; // eax
	int v22; // eax
	UINT v23; // eax
	int v24; // esi
	int v25; // eax
	int v26; // eax
	signed int v27; // [esp-4h] [ebp-2E8h]
	char a1[256]; // [esp+Ch] [ebp-2D8h]
	char String[128]; // [esp+10Ch] [ebp-1D8h]
	char Buffer[128]; // [esp+18Ch] [ebp-158h]
	CHAR v31[32]; // [esp+20Ch] [ebp-D8h]
	HDC a8a[12]; // [esp+22Ch] [ebp-B8h]
	struct tagTEXTMETRICA tm; // [esp+25Ch] [ebp-88h]
	_uiheroinfo heroinfo; // [esp+294h] [ebp-50h]
	struct tagPOINT pt; // [esp+2C0h] [ebp-24h]
	_gamedata gamedata; // [esp+2C8h] [ebp-1Ch]
	struct tagRECT rc; // [esp+2D0h] [ebp-14h]
	COLORREF v38; // [esp+2E0h] [ebp-4h]
	LPCSTR lpStringa; // [esp+2F4h] [ebp+10h]

	Buffer[0] = nullcharacter;
	v8 = a8;
	memset(&Buffer[1], 0, 0x7Cu);
	v9 = a8[4];
	*(_WORD *)&Buffer[125] = 0;
	Buffer[127] = 0;
	v10 = (unsigned char)v9 & 1;
	v11 = strlen(a4) + 1;
	if ( v11 > 0x100 )
		return 0;
	memcpy(a1, a4, v11);
	if ( color == 1 )
	{
		a8 = 0;
		a4 = 0;
		if ( !Connect_DiffFromString(a1, &gamedata, (int)&a8, (int)&a4) )
			return 0;
		color = SetTextColor(v8[6], 0xFFFFFFu);
		v38 = SetBkColor(v8[6], v10 != 0 ? 0x808080 : 0);
		if ( align & 1 )
		{
			v13 = strlen(lpString);
			ExtTextOutA(v8[6], (int)v8[7], (int)v8[8], 6u, (const RECT *)(v8 + 7), lpString, v13, 0);
		}
		else
		{
			v14 = a8 && a4;
			if ( gamedata.bDiff < 3u )
				LoadStringA(ghUiInst, gamedata.bDiff + 1003, Buffer, 128);
			if ( align & 2 && v14 )
			{
				GetTextMetricsA(v8[6], &tm);
				lpStringa = (LPCSTR)(tm.tmHeight + tm.tmExternalLeading);
				MoveToEx(v8[6], (int)v8[7], (int)v8[8], &pt);
				align = SetTextAlign(v8[6], 1u);
				v15 = strlen(Buffer);
				TextOutA(v8[6], 0, 0, Buffer, v15);
				LoadStringA(ghUiInst, 0x409u, String, 128);
				MoveToEx(v8[6], (int)v8[7], (int)v8[8] + (_DWORD)lpStringa, 0);
				v16 = strlen(String);
				TextOutA(v8[6], 0, 0, String, v16);
				v17 = v8[6];
				qmemcpy(a8a, v8, sizeof(a8a));
				GetCurrentPositionEx(v17, (LPPOINT)&a8a[7]);
				SetTextAlign(v8[6], 0);
				connect_color_text = 1;
				UiDrawDescCallback(arg0, 2u, (LPCSTR)a8, a4, 0, 1u, 0, a8a);
				connect_color_text = 0;
				if ( a7 )
				{
					SetTextAlign(v8[6], 1u);
					LoadStringA(ghUiInst, 0x40Cu, &heroinfo.name[8], 32);
					LoadStringA(ghUiInst, 0x40Du, v31, 32);
					v18 = localtime(&a7);
					v19 = v18;
					if ( v18 )
					{
						LoadStringA(ghUiInst, (v18->tm_hour > 12) + 1034, (LPSTR)&rc.top, 10);
						v20 = v19->tm_hour;
						if ( v20 > 12 )
							v19->tm_hour = v20 - 12;
						if ( !v19->tm_hour )
							v19->tm_hour = 12;
						sprintf(String, &heroinfo.name[8], v31, v19->tm_hour, v19->tm_min, &rc.top);
						MoveToEx(v8[6], (int)v8[7], (int)v8[8] + 2 * (_DWORD)lpStringa, 0);
						v21 = strlen(String);
						TextOutA(v8[6], 0, 0, String, v21);
					}
				}
				MoveToEx(v8[6], pt.x, pt.y, 0);
				SetTextAlign(v8[6], align);
			}
			else
			{
				v22 = strlen(Buffer);
				TextOutA(v8[6], (int)v8[7], (int)v8[8], Buffer, v22);
			}
		}
		goto LABEL_56;
	}
	GetTextMetricsA(v8[6], &tm);
	a4 = (char *)((tm.tmHeight - connect_draw_height) / 2 + 1);
	if ( !connect_color_text )
	{
		color = SetTextColor(v8[6], 0xFFFFu);
		v38 = SetBkColor(v8[6], v10 != 0 ? 0x808080 : 0);
	}
	if ( align & 1 )
	{
		v23 = strlen(lpString);
		ExtTextOutA(v8[6], (int)v8[7] + heroport_data[0] + 2, (int)v8[8], 6u, (const RECT *)(v8 + 7), lpString, v23, 0);
	}
	if ( a5 )
	{
		if ( a5 & 0x20 )
		{
			v27 = 5;
LABEL_45:
			v24 = v27;
			goto LABEL_46;
		}
		if ( a5 & 1 )
		{
			v24 = 0;
LABEL_46:
			SetRect(&rc, 0, 0, special_data[0] - 1, special_frames - 1);
			OffsetRect(&rc, 0, special_frames * v24);
			SDlgBltToWindowI(
				(HWND)v8[5],
				0,
				(char *)v8[7],
				(int)v8[8] + (_DWORD)a4,
				connect_data3,
				&rc,
				(SIZE *)special_data,
				-1,
				0,
				0xCC0020u);
			goto LABEL_55;
		}
		if ( a5 & 2 )
		{
			v27 = 2;
			goto LABEL_45;
		}
		if ( a5 & 4 )
		{
			v27 = 3;
			goto LABEL_45;
		}
		if ( a5 & 8 )
		{
			v25 = (arg0 != 'BNET') - 1;
			_LOBYTE(v25) = v25 & 0xFD;
			v24 = v25 + 4;
			goto LABEL_46;
		}
	}
	if ( *(_DWORD *)a1 == 'CHAT' )
	{
		v27 = 6;
		goto LABEL_45;
	}
	if ( *(_DWORD *)a1 == 'SEXP' || *(_DWORD *)a1 == 'SSHR' || *(_DWORD *)a1 == 'STAR' )
	{
		v27 = 7;
		goto LABEL_45;
	}
	if ( Connect_GetHeroInfoConc(a1, &heroinfo) )
	{
		if ( heroinfo.spawned )
		{
			if ( connect_data2 )
				SBltROP3(
					connect_data4,
					connect_data2,
					heroport_data[0],
					connect_draw_height,
					heroport_data[0],
					heroport_data[0],
					0,
					0xCC0020u);
		}
		else
		{
			SBltROP3(
				connect_data4,
				&connect_data1[heroport_frames * (heroinfo.heroclass + 3 * heroinfo.herorank)],
				heroport_data[0],
				connect_draw_height,
				heroport_data[0],
				heroport_data[0],
				0,
				0xCC0020u);
		}
		v26 = heroinfo.level / 10;
		if ( v26 )
			STransBlt(
				(char *)connect_data4 + 4 * heroport_data[0] + heroport_data[0] + 14,
				0,
				0,
				heroport_data[0],
				connect_trans[v26]);
		STransBlt(
			(char *)connect_data4 + 4 * (heroport_data[0] + 4) + heroport_data[0] + 4,
			0,
			0,
			heroport_data[0],
			connect_trans[heroinfo.level % 10]);
		SetRect(&rc, 0, 0, heroport_data[0] - 1, connect_draw_height - 1);
		SDlgBltToWindowI(
			(HWND)v8[5],
			0,
			(char *)v8[7],
			(int)v8[8] + (_DWORD)a4,
			connect_data4,
			&rc,
			(SIZE *)heroport_data,
			-1,
			0,
			0xCC0020u);
	}
LABEL_55:
	if ( !connect_color_text )
	{
LABEL_56:
		SetTextColor(v8[6], color);
		SetBkColor(v8[6], v38);
	}
	return 1;
}
// 100294A4: using guessed type int special_frames;
// 100295C0: using guessed type int heroport_frames;
// 10029614: using guessed type int connect_color_text;

// ref: 0x10003CE4
BOOL __stdcall UiCategoryCallback(int a1, int a2, int a3, int a4, int a5, _DWORD *a6, _DWORD *a7)
{
	*a7 = 0xFFFF;
	*a6 = Connect_GetRankFromLevel(connect_categorystr);
	return 1;
}

// ref: 0x10003D04
int __fastcall Connect_GetRankFromLevel(char *str)
{
	char a1[512]; // [esp+0h] [ebp-22Ch]
	_uiheroinfo a2; // [esp+200h] [ebp-2Ch]

	strcpy(a1, str);
	if ( !Connect_GetHeroInfoConc(a1, &a2) || a2.level == 1 )
		return 0;
	if ( (signed int)a2.level < 4 )
		return 1;
	if ( (signed int)a2.level < 6 )
		return 2;
	if ( (signed int)a2.level < 8 )
		return 3;
	if ( (signed int)a2.level < 10 )
		return 4;
	if ( (signed int)a2.level < 13 )
		return 5;
	if ( (signed int)a2.level < 17 )
		return 6;
	if ( (signed int)a2.level < 20 )
		return 7;
	if ( (signed int)a2.level < 25 )
		return 8;
	if ( (signed int)a2.level < 30 )
		return 9;
	if ( (signed int)a2.level < 35 )
		return 10;
	if ( (signed int)a2.level < 40 )
		return 11;
	return ((signed int)a2.level >= 48) + 12;
}

// ref: 0x10003DAF
BOOL __fastcall Connect_DiffFromString(char *str, _gamedata *gamedata, int a3, int a4)
{
	unsigned char v6; // al
	char *v8; // eax
	const char *v9; // eax
	char *v10; // eax

	if ( !*str )
		return 0;
	v6 = atoi(str);
	gamedata->bDiff = v6;
	if ( v6 >= 3u )
		return 0;
	v8 = strchr(str, 13);
	if ( v8 )
	{
		*v8 = 0;
		v9 = v8 + 1;
		if ( a3 )
			*(_DWORD *)a3 = (unsigned int)v9;
		v10 = (char *)strchr(v9, 13);
		if ( v10 )
		{
			*v10 = 0;
			if ( a4 )
				*(_DWORD *)a4 = (unsigned int)v10 + 1;
		}
	}
	return 1;
}

// ref: 0x10003E0C
void __fastcall Connect_SetDiffString(_gamedata *gamedata, const char *str1, char *str2, char *str3, int size)
{
	size_t v7; // ebx
	size_t v8; // eax
	unsigned char v9; // zf
	unsigned char v10; // sf
	unsigned char v11; // of
	int v12; // eax

	v7 = strlen(str1);
	v8 = v7 + strlen(str2) + 5;
	v11 = __OFSUB__(size, v8);
	v9 = size == v8;
	v10 = (signed int)(size - v8) < 0;
	v12 = gamedata->bDiff;
	if ( (unsigned char)(v10 ^ v11) | v9 )
		_itoa(v12, str3, 10);
	else
		sprintf(str3, "%d\r%s\r%s", v12, str1, str2);
}

// ref: 0x10003E61
BOOL __fastcall Connect_GetHeroInfoConc(const char *a1, _uiheroinfo *pInfo)
{
	int v4; // eax
	int v5; // edi
	int v6; // edx
	unsigned short v7; // di
	unsigned char v8; // cl
	int v10; // [esp+Ch] [ebp-24h]
	unsigned int v11; // [esp+10h] [ebp-20h]
	unsigned int v12; // [esp+14h] [ebp-1Ch]
	unsigned int v13; // [esp+18h] [ebp-18h]
	int v14; // [esp+1Ch] [ebp-14h]
	int v15; // [esp+20h] [ebp-10h]
	int v16; // [esp+24h] [ebp-Ch]
	int v17; // [esp+28h] [ebp-8h]
	int v18; // [esp+2Ch] [ebp-4h]

	memset(pInfo, 0, 0x2Cu);
	if ( !*a1 )
		return 0;
	v4 = *(_DWORD *)a1;
	if ( *(_DWORD *)a1 != 'DRTL' && v4 != 'DSHR' && v4 != 'DTST' )
		return 0;
	if ( sscanf(a1 + 4, "%d %d %d %d %d %d %d %d %d", &v13, &v12, &v11, &v18, &v17, &v16, &v15, &v10, &v14) != 9 )
		return 0;
	v5 = *(_DWORD *)a1;
	v6 = v14;
	if ( v5 == 'DRTL' )
	{
		if ( v14 )
			return 0;
	}
	if ( v5 == 'DSHR' && !v14 )
		return 0;
	v7 = v13;
	if ( !v13 )
		return 0;
	if ( v13 > 0x63 )
		return 0;
	v8 = v12;
	if ( v12 >= 3 || v11 > 3 || v18 < 0 || v17 < 0 || v16 < 0 || v15 < 0 )
		return 0;
	pInfo->herorank = v11;
	pInfo->level = v7;
	pInfo->strength = (unsigned char)v18;
	pInfo->heroclass = v8;
	pInfo->magic = (unsigned char)v17;
	pInfo->spawned = v6;
	pInfo->dexterity = (unsigned char)v16;
	pInfo->vitality = (unsigned char)v15;
	pInfo->gold = v10;
	return 1;
}

// ref: 0x10003F6F
void __fastcall Connect_MakeDescString(_uiheroinfo *a1, char *name, size_t size)
{
	*(_DWORD *)name = (unsigned int)connect_charname;
	_snprintf(
		name + 4,
		size,
		" %d %d %d %d %d %d %d %d %d",
		a1->level,
		a1->heroclass,
		a1->herorank,
		a1->strength,
		a1->magic,
		a1->dexterity,
		a1->vitality,
		a1->gold,
		a1->spawned);
}

// ref: 0x10003FB7
void __stdcall UiCreateGameCriteria(_uiheroinfo *pInfo, char *str)
{
	sprintf(str, "#%d?%d", 3, pInfo->level);
}

// ref: 0x10003FD6
BOOL __stdcall UiCreatePlayerDescription(_uiheroinfo *info, int mode, char *desc)
{
	connect_charname = (char *)mode;
	Connect_MakeDescString(info, desc, 0x80u);
	return 1;
}

// ref: 0x10003FF7
void __stdcall UiSetupPlayerInfo(char *infostr, _uiheroinfo *pInfo, int type)
{
	connect_charname = (char *)type;
	SStrCopy(connect_plrinfostr, infostr, 128);
	Connect_MakeDescString(pInfo, connect_categorystr, 0x80u);
}

// ref: 0x10004028
void __fastcall Connect_CopyPlrDescStrings(char *str1, int size1, char *str2, int size2)
{
	if ( str1 )
		SStrCopy(str1, connect_plrinfostr, size1);
	if ( str2 )
		SStrCopy(str2, connect_categorystr, size2);
}

//HEADER_GOES_HERE

#include "../types.h"



// 20 frames per second
// if increased for smoother mouse movement, the animation rate has to be throttled
#define MENU_FRAME_RATE 20



char chr_name_str[16];
int playmovie = 0;

// prevent calling from the DLL
void __cdecl UiDestroy() { return; }
BOOL __stdcall UiTitleDialog(int a1) { return 0; }
void __cdecl UiInitialize() { return ; }
BOOL __stdcall UiCopyProtError(int *pdwResult) { return 0; }
void __stdcall UiAppActivate(BOOL bActive) { return; }
BOOL __fastcall UiValidPlayerName(char *name) { return 0; } /* check __stdcall */
BOOL __stdcall UiSelHeroMultDialog(BOOL (__stdcall *fninfo)(BOOL (__stdcall *fninfofunc)(_uiheroinfo *)), BOOL (__stdcall *fncreate)(_uiheroinfo *), BOOL (__stdcall *fnremove)(_uiheroinfo *), BOOL (__stdcall *fnstats)(int, _uidefaultstats *), int *dlgresult, int *a6, char *name) { return 0; }
BOOL __stdcall UiSelHeroSingDialog(BOOL (__stdcall *fninfo)(BOOL (__stdcall *fninfofunc)(_uiheroinfo *)), BOOL (__stdcall *fncreate)(_uiheroinfo *), BOOL (__stdcall *fnremove)(_uiheroinfo *), BOOL (__stdcall *fnstats)(int, _uidefaultstats *), int *dlgresult, char *name, int *difficulty) { return 0; }
BOOL __stdcall UiCreditsDialog(int a1) { return 0; }
BOOL __stdcall UiMainMenuDialog(char *name, int *pdwResult, void (__stdcall *fnSound)(char *file), int a4) { return 0; }
int __stdcall UiProgressDialog(HWND window, char *msg, int a3, void *fnfunc, int a5) { return 0; }
int __cdecl UiProfileGetString() { return 0; }
void __cdecl UiProfileCallback() { return ; }
void __cdecl UiProfileDraw() { return ; }
BOOL __stdcall UiCategoryCallback(int a1, int a2, int a3, int a4, int a5, _DWORD *a6, _DWORD *a7) { return 0; }
BOOL __stdcall UiGetDataCallback(int game_type, int data_code, void *a3, int a4, int a5) { return 0; }
BOOL __stdcall UiAuthCallback(int a1, char *a2, char *a3, char a4, char *a5, LPSTR lpBuffer, int cchBufferMax) { return 0; }
BOOL __stdcall UiSoundCallback(int a1, int type, int a3) { return 0; }
void __stdcall UiMessageBoxCallback(HWND hWnd, char *lpText, LPCSTR lpCaption, UINT uType) { return ; }
BOOL __stdcall UiDrawDescCallback(int arg0, COLORREF color, LPCSTR lpString, char *a4, int a5, UINT align, time_t a7, HDC *a8) { return 0; }
BOOL __stdcall UiCreateGameCallback(int a1, int a2, int a3, int a4, int a5, int a6) { return 0; }
BOOL __stdcall UiArtCallback(int game_type, unsigned int art_code, PALETTEENTRY *pPalette, void *pBuffer, DWORD dwBuffersize, DWORD *pdwWidth, DWORD *pdwHeight, DWORD *pdwBpp) { return 0; }
int __stdcall UiSelectGame(int a1, _SNETPROGRAMDATA *client_info, _SNETPLAYERDATA *user_info, _SNETUIDATA *ui_info, _SNETVERSIONDATA *file_info, int *a6) { return 0; }
int __stdcall UiSelectProvider(int a1, _SNETPROGRAMDATA *client_info, _SNETPLAYERDATA *user_info, _SNETUIDATA *ui_info, _SNETVERSIONDATA *file_info, int *type) { return 0; }
BOOL __stdcall UiCreatePlayerDescription(_uiheroinfo *info, int mode, char *desc) { return 0; }
void __stdcall UiSetupPlayerInfo(char *infostr, _uiheroinfo *pInfo, int type) { return ; }
void __stdcall UiCreateGameCriteria(_uiheroinfo *pInfo, char *str) { return ; }
BOOL __stdcall UiGetDefaultStats(int pclass, _uidefaultstats *pStats) { return 0; }
BOOL __stdcall UiBetaDisclaimer(int a1) { return 0; }

/*
*
*   custom palettes
*   diablo happens to lack my three favorite colors :P
*
*   overwrite palette array to use, and shift the string color
*
*
*/
/*


BYTE GreenPal[8][3] =
{
	{ 0xBE, 0xFD, 0x9F },
	{ 0x99, 0xF9, 0x6A },
	{ 0x75, 0xF4, 0x35 },
	{ 0x50, 0xF0, 0x00 },
	{ 0x3F, 0xBC, 0x00 },
	{ 0x2D, 0x87, 0x00 },
	{ 0x1C, 0x53, 0x00 },
	{ 0x0A, 0x1E, 0x00 }
};

BYTE PinkPal[8][3] =
{
	{ 0xFD, 0x9F, 0xBE },
	{ 0xF9, 0x6A, 0x99 },
	{ 0xF4, 0x35, 0x75 },
	{ 0xF0, 0x00, 0x50 },
	{ 0xBC, 0x00, 0x3F },
	{ 0x87, 0x00, 0x2D },
	{ 0x53, 0x00, 0x1C },
	{ 0x1E, 0x00, 0x0A }
};

BYTE PurplePal[8][3] =
{
	{ 0xBE, 0x9F, 0xDD },
	{ 0x99, 0x6A, 0xD9 },
	{ 0x75, 0x35, 0xD4 },
	{ 0x50, 0x00, 0xD0 },
	{ 0x3F, 0x00, 0x9C },
	{ 0x2D, 0x00, 0x67 },
	{ 0x1C, 0x00, 0x33 },
	{ 0x0A, 0x00, 0x0E }
};*/

int __fastcall TEMP_diablo_init_menu(int a1, int bSinglePlayer)
{
	int v2; // esi
	int v3; // edi
	int v4; // ecx
	int pfExitProgram; // [esp+Ch] [ebp-4h]

	v2 = bSinglePlayer;
	v3 = a1;
	byte_678640 = 1;
	while ( 1 )
	{
		pfExitProgram = 0;
		dword_5256E8 = 0;
		if ( !NetInit(v2, &pfExitProgram) )
			break;
		byte_678640 = 0;
		if ( v3 )
		{
			InitLevels();
			InitQuests();
			InitPortals();
			InitDungMsgs(myplr);
			v4 = WM_DIABLOADGAME;
		}
		else
		{
			v4 = WM_DIABNEWGAME;
		}
		run_game_loop(v4);
		NetClose();
		pfile_create_player_description(0, 0);
		if ( !gbRunGameResult )
			goto LABEL_11;
		break;
	}
	gbRunGameResult = pfExitProgram == 0;
LABEL_11:
	SNetDestroy();
	return gbRunGameResult;
}

///////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////

int gdwLogoWidth;
int gdwLogoHeight;
void *pPcxLogoImage;

int gdwFontWidth;
int gdwFontHeight;
void *pPcxFontImage;
unsigned char *pFont;

int gdwFont2Width;
int gdwFont2Height;
void *pPcxFont2Image;
unsigned char *pFont2;

int gdwFont3Width;
int gdwFont3Height;
void *pPcxFont3Image;
unsigned char *pFont3;

int gdwTitleWidth;
int gdwTitleHeight;
void *pPcxTitleImage;

int gdwHeroWidth;
int gdwHeroHeight;
void *pPcxHeroImage;

int gdwSHeroWidth;
int gdwSHeroHeight;
void *pPcxSHeroImage;

int gdwCreditsWidth;
int gdwCreditsHeight;
void *pPcxCreditsImage;

int gdwCursorWidth;
int gdwCursorHeight;
void *pPcxCursorImage;

///////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////
//////// PCX HELPER FUNCTIONS /////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////
int bloaded = 0;

PALETTEENTRY pcxPal[256];

void __fastcall LoadPalInMem(PALETTEENTRY *pPal)
{
	int i; // eax

	for(i = 0; i < 256; i++)
	{
		orig_palette[i].peFlags = 0;
		orig_palette[i].peRed = pPal[i].peRed;
		orig_palette[i].peGreen = pPal[i].peGreen;
		orig_palette[i].peBlue = pPal[i].peBlue;
	}
}

BOOL __cdecl LoadArtImage(char *pszFile, void **pBuffer, int frames, DWORD *data)
{
    DWORD width; // [esp+44h] [ebp-8h]
    DWORD height; // [esp+48h] [ebp-4h] MAPDST

	*pBuffer = NULL;

    if ( !SBmpLoadImage(pszFile, 0, 0, 0, &width, &height, 0) )
        return 0;
    *pBuffer = SMemAlloc(height * width, "U:\\DiabloUI\\Ui\\local.cpp", 88);
    if ( !SBmpLoadImage(pszFile, NULL, *pBuffer, height * width, 0, 0, 0) )
        return 0;
    if ( pBuffer && data )
    {
        data[0] = width;
        data[1] = height / frames;
    }
    return 1;
}
/*


// Test sample.pcx to see if it is a PCX
SDL_RWops *rwop;
rwop=SDL_RWFromFile("sample.pcx", "rb");
if(IMG_isPCX(rwop))
printf("sample.pcx is a PCX file.\n");
else
printf("sample.pcx is not a PCX file, or PCX support is not available.\n");






*/
BOOL __cdecl LoadArtWithPal(char *pszFile, void **pBuffer, int frames, DWORD *data)
{
    DWORD width; // [esp+44h] [ebp-8h]
    DWORD height; // [esp+48h] [ebp-4h] MAPDST

	*pBuffer = NULL;

    if ( !SBmpLoadImage(pszFile, 0, 0, 0, &width, &height, 0) )
        return 0;
    *pBuffer = SMemAlloc(height * width, "U:\\DiabloUI\\Ui\\local.cpp", 88);
    if ( !SBmpLoadImage(pszFile, pcxPal, *pBuffer, height * width, 0, 0, 0) )
        return 0;

	LoadPalInMem(pcxPal);
	lpDDPalette->SetEntries(0, 0, 256, orig_palette);

    if ( pBuffer && data )
    {
        data[0] = width;
        data[1] = height / frames;
    }
    return 1;
}



void DrawArtImage(int SX, int SY, int SW, int SH, int nFrame, void *pBuffer)
{
	BYTE *src = (BYTE *)pBuffer+(SW * SH * nFrame);
	BYTE *dst = (BYTE *)&gpBuffer->row[SY].pixels[SX];

	for(int i = 0; i < SH; i++, src += SW, dst += 768)
	{
		for(int j = 0; j < SW; j++)
		{
			dst[j] = src[j];
		}
	}
}

void DrawArtWithMask(int SX, int SY, int SW, int SH, int nFrame, BYTE bMask, void *pBuffer)
{
	BYTE *src = (BYTE *)pBuffer+(SW * SH * nFrame);
	BYTE *dst = (BYTE *)&gpBuffer->row[SY].pixels[SX];

	for(int i = 0; i < SH; i++, src += SW, dst += 768)
	{
		for(int j = 0; j < SW; j++)
		{
			if(src[j] != bMask)
				dst[j] = src[j];
		}
	}
}

int __fastcall GetPCXFontWidth(char *str, BYTE *font)
{
    int len; // eax
    unsigned __int8 i; // bl
    BYTE chr; // bl
    int width; // esi

    len = 0;
    for ( i = *str; *str; i = *str )
    {
        chr = font[i + 2];
        if ( chr )
            width = chr;
        else
            width = *font;
        len += width;
        ++str;
    }
    return len;
}

void DrawPCXString(int x, int y, int w, int h, char *str, BYTE *font, void *pBuff)
{
	int i;
	int len = 0;
	for(i = 0; i < strlen(str); i++)
	{
		DrawArtWithMask(x+len, y, w, h, str[i], 32, pBuff);
		//DrawPCX(x+len, y, str[i], 32);
		if(font[str[i]+2])
			len += font[str[i]+2];
		else
			len += *font;
	}
}

void ShiftStrColor(int w, int h, int frames, void *pBuff, BYTE bMask, BYTE bShift, BOOL b4BPP)
{
	BYTE *dst = (BYTE*)pBuff;
	for(int i = 0; i < w*h*frames; i++)
	{
		if(*dst != bMask) {
			if(b4BPP) {
				BYTE c = *dst & 0xF;
				c /= 2;
				*dst -= c+1;
			}
			*dst += bShift;
		}
		dst++;
	}
}
////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////




int menu_music_track_id = 5; // idb
LRESULT CALLBACK GM_Title(HWND hWnd, UINT Msg, WPARAM wParam, LPARAM lParam);
void __cdecl mainmenu_refresh_music()
{
	int v0; // eax

	music_start((GetTickCount() % 5)+1);
	v0 = menu_music_track_id;
	do
	{
		if ( ++v0 == 6 )
			v0 = 0;
	}
	while ( !v0 || v0 == 1 );
	menu_music_track_id = v0;
}

void __stdcall mainmenu_create_hero(char *a1, char *a2)
{
	// char *v2; // [esp-14h] [ebp-14h]

	//if ( UiValidPlayerName(a1) ) /* v2 */
		pfile_create_save_file(a1, a2);
}

int __stdcall mainmenu_select_hero_dialog(const struct _SNETPROGRAMDATA *u1,const struct _SNETPLAYERDATA *u2,const struct _SNETUIDATA *u3,const struct _SNETVERSIONDATA *u4,DWORD mode,char *cname, DWORD clen, char *cdesc, DWORD cdlen,BOOL *multi)
{
	int v10; // eax
	int a6; // [esp+8h] [ebp-8h]
	int a5; // [esp+Ch] [ebp-4h]

	a6 = 1;
	a5 = 0;
	// if ( gbMaxPlayers == 1 )
	// {
		// if ( !UiSelHeroSingDialog(
				  // pfile_ui_set_hero_infos,
				  // pfile_ui_save_create,
				  // pfile_delete_save,
				  // pfile_ui_set_class_stats,
				  // &a5,
				  // chr_name_str,
				  // &gnDifficulty) )
			// TermMsg("Unable to display SelHeroSing");
		// if ( a5 == 2 )
		// {
			// dword_5256E8 = 1;
			// goto LABEL_6;
		// }
		// dword_5256E8 = 0;
	// }
	// else if ( !UiSelHeroMultDialog(
				   // pfile_ui_set_hero_infos,
				   // pfile_ui_save_create,
				   // pfile_delete_save,
				   // pfile_ui_set_class_stats,
				   // &a5,
				   // &a6,
				   // chr_name_str) )
	// {
		// TermMsg("Can't load multiplayer dialog");
	// }
	if ( a5 == 4 )
	{
		SErrSetLastError(1223);
		return 0;
	}
LABEL_6:
	pfile_create_player_description(cdesc, cdlen);
	if ( multi )
	{
		if ( mode == 'BNET' )
			v10 = a6 || !plr[myplr].pBattleNet;
		else
			v10 = a6;
		*multi = v10;
	}
	if ( cname )
	{
		if ( clen )
			SStrCopy(cname, chr_name_str, clen);
	}
	return 1;
}

/*
"Gendata\\Titlgray.CEL"
"Gendata\\Titltext.CEL"
"Gendata\\Pentitle.CEL"
"Gendata\\Titlqtxt.CEL"
"Gendata\\Diabfr.CEL"
"Gendata\\Title.pal"
*/
_uiheroinfo heroarray[10];
int fuckcount = 0;
char gb_title_option;
unsigned char is_typing_plr_name;
unsigned char title_cel_is_loaded;
int diablogo_cel_frame;
void *pTitlgrayCel_sgpBackCel;
void *pTitltextCel;
void *pPentitleCel;
void *pTitlqtxtCel;
void *pPentSmall;
void *pDiabfrCel;
void *p_reserved_title;
//void *pPlayerGFX;
int gb_Lfont_str_len;
char byte_5DE90C; //unused
int gb_Lfont_pix_width;
int the_pcx_frame;

int sgbMenuOption;
int gbProcessFrame;
int new_timer_2_int;

int gNewMSG;
int gbProcType;
int some_usless_adder_int = 0;
/*
void __cdecl load_title_game00()
{
	char str[52]; // [esp+Ch] [ebp-34h]

	some_save_flag = 0;
	sprintf(gszSaveFilePath, "%s%s", gszSavePath64_not_long, "\\Game00.sav");
	_searchenv(gszSaveFilePath, 0, str);
	if ( str[0] )
		some_save_flag |= 0x80u;
}
*/

void __fastcall print_title_str_large(int a1, int a2, char *a3)
{
	int screen_y; // [esp+Ch] [ebp-14h]
	int screen_x; // [esp+10h] [ebp-10h]
	char v5; // [esp+14h] [ebp-Ch]
	signed int i; // [esp+18h] [ebp-8h]
	signed int v7; // [esp+1Ch] [ebp-4h]

	screen_y = a2;
	screen_x = a1;
	v7 = strlen(a3);
	for ( i = 0; v7 > i; ++i )
	{
		v5 = lfontframe[fontidx[a3[i]]];
		if ( v5 )
			CelDecodeOnly(screen_x, screen_y, pTitltextCel, v5, 46);
		screen_x += lfontkern[v5] + 2;
	}
	gb_Lfont_pix_width = screen_x;
	gb_Lfont_str_len = v7;
}

void __fastcall print_title_str_small(int a1, int a2, char *a3)
{
	int screen_y; // [esp+Ch] [ebp-14h]
	int screen_x; // [esp+10h] [ebp-10h]
	char v5; // [esp+14h] [ebp-Ch]
	signed int i; // [esp+18h] [ebp-8h]
	signed int v7; // [esp+1Ch] [ebp-4h]

	screen_y = a2;
	screen_x = a1;
	v7 = strlen(a3);
	for ( i = 0; i < v7; ++i )
	{
		v5 = mfontframe[fontidx[a3[i]]];
		if ( v5 )
			CelDecodeOnly(screen_x, screen_y, pTitlqtxtCel, v5, 22);
		screen_x += mfontkern[v5] + 2;
	}
}

int __fastcall gmenu_get_lfont2(char *pItem)
{
	char *v2; // eax
	int i; // edx
	unsigned char v4; // cl

	v2 = pItem;
	for ( i = 0; ; i += lfontkern[lfontframe[fontidx[v4]]] + 2 )
	{
		v4 = *v2;
		if ( !*v2 )
			break;
		++v2;
	}
	return i - 2;
}
int thehero = 0;
int __fastcall gmenu_get_sfont2(char *pItem)
{
	char *v2; // eax
	int i; // edx
	unsigned char v4; // cl

	v2 = pItem;
	for ( i = 0; ; i += fontkern[fontframe[fontidx[v4]]] + 2 )
	{
		v4 = *v2;
		if ( !*v2 )
			break;
		++v2;
	}
	return i - 2;
}
int thefucker = 1;
int maxheroes = 0;
void __cdecl LoadNames(int fuckthem)
{
	int item_basex[3]; // [esp+Ch] [ebp-48h]
	int y; // [esp+18h] [ebp-3Ch]
	int i; // [esp+1Ch] [ebp-38h]
	int x; // [esp+20h] [ebp-34h]
	char strtbl[5][16]; // [esp+24h] [ebp-30h]
	char pclass[32];
	if(!fuckthem) {
	ClearScreenBuffer();
	//CelDecodeOnly(64, 639, pTitlgrayCel_sgpBackCel, 1, 640);

	}
	//DrawArtImage(0, 0, gdwTitleWidth, gdwTitleHeight, 0, pPcxTitleImage);
	DrawArtImage(0, 0, gdwSHeroWidth, gdwSHeroHeight, 0, pPcxSHeroImage);
	DrawArtWithMask(320-(gdwLogoWidth/2), 0, gdwLogoWidth, gdwLogoHeight, the_pcx_frame, 250, pPcxLogoImage);
	//CelDecodeOnly(236, 282, pDiabfrCel, 1, 296);//CelDecodeOnly(64, 377, pDiabfrCel, diablogo_cel_frame, 640);
	// if ( demo_mode )
		// strcpy(tempstr, "Pre-release Demo");
	// else
//		strcpy(tempstr, "V1.0");
//	print_title_str_small(80, 624, tempstr);

	bool empty = 0;
	//CelDecodeOnly(408, 600, pSTextBoxCels, 1, 271);
	for ( i = 0; i < 10; ++i )
	{
		if(heroarray[i].name) {
			if(((!fuckthem && sgdwProgress == i) || (fuckthem && i == thehero)) && heroarray[i].level)
			{
				DrawArtImage(30, 211, gdwHeroWidth, gdwHeroHeight, heroarray[i].heroclass, pPcxHeroImage);

				int add_this_to_the_ypos = 320;
				sprintf(tempstr, "     Level: %i", heroarray[i].level);
				DrawPCXString(46, add_this_to_the_ypos, gdwFont3Width, gdwFont3Height, tempstr, pFont3, pPcxFont3Image);
				add_this_to_the_ypos += pFont3[1]*2;
				sprintf(tempstr, "  Strength: %i", heroarray[i].strength);
				DrawPCXString(50, add_this_to_the_ypos, gdwFont3Width, gdwFont3Height, tempstr, pFont3, pPcxFont3Image);
				add_this_to_the_ypos += pFont3[1];
				sprintf(tempstr, "    Magic: %i", heroarray[i].magic);
				DrawPCXString(52, add_this_to_the_ypos, gdwFont3Width, gdwFont3Height, tempstr, pFont3, pPcxFont3Image);
				add_this_to_the_ypos += pFont3[1];
				sprintf(tempstr, " Dexterity: %i", heroarray[i].dexterity);
				DrawPCXString(50, add_this_to_the_ypos, gdwFont3Width, gdwFont3Height, tempstr, pFont3, pPcxFont3Image);
				add_this_to_the_ypos += pFont3[1];
				sprintf(tempstr, "  Vitality: %i", heroarray[i].vitality);
				DrawPCXString(50, add_this_to_the_ypos, gdwFont3Width, gdwFont3Height, tempstr, pFont3, pPcxFont3Image);
			}

			sprintf(tempstr, "%s", heroarray[i].name);
			char col = COL_GOLD;
			if(!heroarray[i].level && !empty) {
				if(i == sgdwProgress) DrawArtImage(30, 211, gdwHeroWidth, gdwHeroHeight, 3, pPcxHeroImage);
				strcpy(tempstr, "New Hero");
				maxheroes = i;
				empty = 1;
				col = COL_RED;
			}
			x = 424 - (GetPCXFontWidth(tempstr, pFont2) / 2);
			y = pFont2[1] * i + 256;

			DrawPCXString(x, y, gdwFont2Width, gdwFont2Height, tempstr, pFont2, pPcxFont2Image);
			y += 160+20;
			x = 400;

			if(fuckthem && i == thehero)
			{
				CelDecodeOnly(x-20, y, pPentSmall, fuckthem ? 1 : progress_id, 12);
				CelDecodeOnly(x+175, y, pPentSmall, fuckthem ? 1 : progress_id, 12);
			}
			else if ( !fuckthem && i == sgdwProgress )
			{
				CelDecodeOnly(x-20, y, pPentSmall, fuckthem ? 1 : progress_id, 12);
				CelDecodeOnly(x+175, y, pPentSmall, fuckthem ? 1 : progress_id, 12);
			}
		}
	}
}

void __cdecl paint_diff_screen()
{
	int item_basex[3]; // [esp+Ch] [ebp-48h]
	int y; // [esp+18h] [ebp-3Ch]
	int i; // [esp+1Ch] [ebp-38h]
	int x; // [esp+20h] [ebp-34h]
	char strtbl[5][16]; // [esp+24h] [ebp-30h]

	ClearScreenBuffer();
	LoadNames(1);
	strcpy(strtbl[4], "Select Difficulty");//&hero_names[0*32]);
	strcpy(strtbl[0], " Arena Match");
	strcpy(strtbl[1], "Normal");
	strcpy(strtbl[2], "Nightmare");
	strcpy(strtbl[3], "Hell");

	item_basex[0] = 86;
	item_basex[1] = 88;
	item_basex[2] = 113;
	item_basex[3] = 158;
	item_basex[4] = 113;
	//CelDecodeOnly(64, 639, pTitlgrayCel_sgpBackCel, 1, 640);
	//CelDecodeOnly(236, 282, pDiabfrCel, 1, 296);//CelDecodeOnly(64, 377, pDiabfrCel, diablogo_cel_frame, 640);
	// if ( demo_mode )
		// strcpy(tempstr, "Pre-release Demo");
	// else
		strcpy(tempstr, "V1.0");
	print_title_str_small(80, 624, tempstr);

	x = 100;
	y = 45 * (0) + 340;
	ADD_PlrStringXY(x, y-160, x+(gmenu_get_sfont2(strtbl[4])), strtbl[4], COL_GOLD);

	for ( i = 0; i < 4; ++i )
	{
		x = 100;
		y = 18 * (i+1) + 350;
		ADD_PlrStringXY(x, y-160, x+(gmenu_get_sfont2(strtbl[4])), strtbl[i], i == 0 ? COL_BLUE : COL_WHITE);
		//print_title_str_small(x, y, strtbl[i]);
		if ( i == sgdwProgress )
		{
			CelDecodeOnly(172-10, y + 1, pPentSmall, progress_id, 12);
			CelDecodeOnly(300+10, y + 1, pPentSmall, progress_id, 12);
			//CelDecodeOnly(155, y + 1, pPentitleCel, progress_id, 48);
			//CelDecodeOnly(563, y + 1, pPentitleCel, progress_id, 48);
		}
	}
	if(!sgdwProgress) {
	ADD_PlrStringXY(0, 450-160, 350, "Must complete Hell to unlock", COL_RED);
	ADD_PlrStringXY(0, 462-160, 350, "Fight endless hoards of demons", COL_WHITE);
	ADD_PlrStringXY(0, 474-160, 350, "until death or time runs out!", COL_WHITE);
	}
	//CelDrawHdrOnly(164, 364, (char *)pCursCels, some_usless_adder_int+12, InvItemWidth[some_usless_adder_int+12], 0, 8);
	//CelDecodeOnly(64, 64, pCursCels, 11, 28);
}

void __cdecl paint_title_screen()
{
	int item_basex[3]; // [esp+Ch] [ebp-48h]
	int y; // [esp+18h] [ebp-3Ch]
	int i; // [esp+1Ch] [ebp-38h]
	int x; // [esp+20h] [ebp-34h]
	char strtbl[5][16]; // [esp+24h] [ebp-30h]

	ClearScreenBuffer();
	//DrawPCXString(x, y, "fucker");
	DrawArtImage(0, 0, gdwTitleWidth, gdwTitleHeight, 0, pPcxTitleImage);
	DrawArtWithMask(320-(gdwLogoWidth/2), 0, gdwLogoWidth, gdwLogoHeight, the_pcx_frame, 250, pPcxLogoImage);
	strcpy(strtbl[0], "Single Player");//&hero_names[0*32]);
	strcpy(strtbl[1], "Multi Player");
	strcpy(strtbl[2], "Replay Intro");
	strcpy(strtbl[3], "Show Credits");
	strcpy(strtbl[4], "Exit Diablo");

	item_basex[0] = 86;
	item_basex[1] = 88;
	item_basex[2] = 113;
	item_basex[3] = 158;
	item_basex[4] = 113;
	//CelDecodeOnly(64, 639, pTitlgrayCel_sgpBackCel, 1, 640);
	//CelDecodeOnly(236, 282, pDiabfrCel, 1, 296);//CelDecodeOnly(64, 377, pDiabfrCel, diablogo_cel_frame, 640);

	print_title_str_small(80, 624, gszProductName);
	//print_title_str_small(120, 630, "Copyright (C) 1996 Blizzard Entertainment");
	//ADD_PlrStringXY(0, 600-160, 640, "Copyright (C) 1996 Blizzard Entertainment", 0);
	for ( i = 0; i < 5; ++i )
	{
		//x = 384 - (gmenu_get_lfont2(strtbl[i]) >> 1);
		x = 320 - (GetPCXFontWidth(strtbl[i], pFont) / 2) - 1;
		y = pFont[1] * i + 192; // 42 large font, 30 or 16
		//print_title_str_large(x, y, strtbl[i]);
		DrawPCXString(x, y, gdwFontWidth, gdwFontHeight, strtbl[i], pFont, pPcxFontImage);
		if ( i == sgdwProgress )
		{
			CelDecodeOnly(155, y + 208, pPentitleCel, progress_id, 48);
			CelDecodeOnly(563, y + 208, pPentitleCel, progress_id, 48);
			//DrawPCX(155-64, (y-160-45) + 1, progress_id-1);
			//DrawPCX(563-64, (y-160-45) + 1, progress_id-1);
		}
	}
	//CelDrawHdrOnly(164, 364, (char *)pCursCels, some_usless_adder_int+12, InvItemWidth[some_usless_adder_int+12], 0, 8);
	//CelDecodeOnly(64, 64, pCursCels, 11, 28);

}

char *the_long_ass_credits[] = 
{
	"","","","","","","","","","","","",
	"$-- Devilution --",
	"Reversed by Andi \"GalaXyHaXz\"",
	"",
	"$-- Meet the Team --",
	"seritools",
	"mewmew",
	"nomdenom",
	"AJenbo",
	"ApertureSecurity",
	"",
	"$-- Special Thanks --",
	"The Sanctuary preservation project",
	"Jarulf's guide to the Devil himself",
	"",
	"",
	"$...The Devil is in the details",
	"",
	"",
	"$Game Design",
	"Blizzard North",
	"",
	"$Senior Designers",
	"Erich Schaefer, David Brevik",
	"",
	"$Additional Design",
	"Max Schaefer, Eric Sexton, Kenneth Williams",
	"",
	"$Lead Programmer",
	"David Brevik",
	"",
	"$Senior Programmers",
	"Richard Seis, Doron Gartner,",
	"Patrick Wyatt, Mike O'Brien",
	"",
	"$Programming",
	"Peter Brevik, Robin van der Wel, Jon Morin",
	"",
	"$Special Guest Programmers",
	"Collin Murray, Frank Pearce",
	"",
	"$Battle.net Programming",
	"Mike O'Brien, Mike Morhaime, Dan Liebgold",
	"",
	"$Serial Communications Programming",
	"Jeff Strain, Mike O'Brien",
	"",
	"$Installer Programming",
	"Bob Fitch, Patrick Wyatt",
	"",
	"$Art Directors",
	"Erich Schaefer, Samwise Didier",
	"",
	"$Artwork",
	"Michio Okamura, Eric Sexton, Ben Haas, Patrick Tougas,",
	"Kelly Johnson, Erich Schaefer, Max Schaefer, Hugh Tom",
	"Byrne, Roman Kenney, Samwise Didier, Dave Berggren,",
	"Micky Neilson, Eric Flannum, Justin Thavirat, Brian",
	"Sousa, Stu Rose, Nick Carpenter, Joeyray Hall",
	"",
	"$Technical Artwork",
	"Christopher Root, Mark Sutherland,",
	"Robert Djordjevich, Jeffrey Vaughn",
	"",
	"$Cinematic Art Directors",
	"Duane Stinnett, Matt Samia",
	"",
	"$3D Cinematic Artwork",
	"Duane Stinnett, Matt Samia, Rob McNaughton,",
	"Eric Flannum, Dave Pursley, Micky Neilson,",
	"Maxx Marshall, Trevor Jacobs, Brian Sousa,",
	"Samwise Didier, Ron Smorynski, Ben Haas, Patrick Tougas",
	"",
	"$Cinematic Technical Artwork",
	"Joeyray Hall ",
	"",
	"$Executive Producer",
	"Allen Adham",
	"",
	"$Producer",
	"Bill Roper",
	"",
	"$Associate Producer",
	"James Phinney",
	"",
	"$Diablo Strike Team",
	"Allen Adham, Ron Millar, Mike O'Brien,",
	"James Phinney, Bill Roper, Patrick Wyatt",
	"",
	"$Music",
	"Matt Uelmen",
	"",
	"$Sound Design",
	"Matt Uelmen, Glenn Stafford",
	"",
	"$Cinematic Music & Sound",
	"Glenn Stafford, Matt Uelmen",
	"",
	"$Voice Production, Direction & Casting",
	"Mark Schwarz, Glenn Stafford, Bill Roper",
	"",
	"$Script & Story",
	"Erich Schaefer, Eric Sexton,",
	"Chris Metzen, Bill Roper",
	"",
	"$Voice Editing",
	"Glenn Stafford",
	"",
	"$Voices",
	"Michael Haley, Glynnis Talken, Michael Gough,",
	"Paul Eiding, Lani Minella, Steve Brodie, Bill Roper,",
	"Mark Schwarz, Glenn Stafford, Chris Metzen, Max Schaefer",
	"",
	"$Recording Engineer",
	"Robert Koenigsberg",
	"",
	"$Manual Design & Layout",
	"Peter Underwood, Jeffrey Vaughn,",
	"Chris Metzen, Bill Roper",
	"",
	"$Manual Artwork",
	"Samwise Didier, Chris Metzen,",
	"Roman Kenney, Maxx Marshall",
	"",
	"$Provisional Director of QA (Lead Tester)",
	"Shane Dabiri",
	"",
	"$QA Assault Team (Testers)",
	"Christopher Cross, Mike Givens, Dave Hale,",
	"Brian \"Doc\" Love, Hung Nguyen, Dean Shipley,",
	"Chris Sigaty, Ian Welke, Jamie Wiggs",
	"",
	"$QA Special Ops Team (Compatibility Testers)",
	"Adam Maxwell, Russell \"Rug\" Miller",
	"",
	"$QA Artillery Support (Additional Testers) ",
	"Rich Alcazar, Mike Jones, Rev. Dan Moore, Matt Morris,",
	"Mark Pfeiffer, Harish \"Pepe the Enforcer\" Rao, Chris Millar",
	"",
	"$QA Counterintelligence",
	"Alan Dabiri, Jesse McReynolds, Walter Takata",
	"",
	"$Order of Network Information Services",
	"Robert Bridenbecker, Jason \"Brother Shaggy\" Schmit,",
	"Jamie \"Brother Gug\" Steigerwald, Richard Seis ",
	"",
	"$Customer Support",
	"John Schwartz, Vic Larson, Chad Chaudry, Mick Yanko,",
	"Brandon Riseling, Kirk Mahony, Tony Hardy, Richard",
	"Amtower, Collin Smith, Andrew Edwards, Roger Eberhart",
	"",
	"$Sales",
	"Jorge Rivero, Rob Beatie",
	"",
	"$Dunsel",
	"Alan Dabiri",
	"",
	"$Mr. Dabiri's Background Vocalists",
	"Karin Colenzo, Christina Cade,",
	"Kim Farrington, Melissa Edwards",
	"",
	"$Public Relations",
	"Linda Duttenhaver, Susan Wooley",
	"",
	"$Marketing",
	"John Patrick, Steve Huot, Neal Hubbard, Kathy Carter",
	"",
	"$International Sales",
	"Ralph Becker, Chris Yoshimura",
	"",
	"$U.S. Sales",
	"Todd Coyle, Danny Kearns",
	"",
	"$Manufacturing",
	"John Goodman, Tom Bryan",
	"",
	"$Legal & Business",
	"Paula Duffy, Eric Roeder, Paul Sams, Kenneth Williams",
	"",
	"$Special Thanks To",
	"Primenet, Earthlink, AOL, Compuserve, ATI, Matrox, STB, ",
	"Creative Labs, Logitech, U.S. Robotics, Gravis, Cyrix",
	"",
	"$Thanks To",
	"Ian Colquhoun, Rod Shean, Brian Fitzgerald, Sharon Schafer, Todd ",
	"Schafer, Richard and Sandra Schaefer, Rick Bowles, Greg Bogden, ",
	"Cindy Sievert, Brad Mason, Chuck Goldman, Karin Colenzo, Wendy ",
	"Brevik, Jennie Brevik, Hanna Carroll, Katie Newell, Karen Weiss, ",
	"Kurt Beaver, Stan McDonald, Brian Sexton, Michael Wan, Megan ",
	"Williams, Jessica Gensley, Beth Ann Parks, Brian Piltin, Johnathan ",
	"Root, Brett Supernaw, Sabeer Bhatia, Mark Rearick, Brad Mason, ",
	"Diane Kodama, Bernadette Sexton, Barbara Uhlmann, Patricia",
	"McDonald, Kris Kelley, Alissa Vaage, Denise Hernandez, Paula ",
	"White, Rachel Marie Hawley, Laura Gaber, Isaac Matarasso,",
	"Susan Stafford, Gretchen Witte, Jason Sailor, Eufemia Palomino, ",
	"Nathalie Didier, Nicole Welke, Dawn Caddel, Anna-Marcelle ",
	"Okamura, Amber Okamura, Megan Okamura Tracey McNaughton, ",
	"Trisha Ann Ochoa, Annie Strain, Dr. Gregory T. Street, Ray the Soda ",
	"Guy, Sam Raimi, A. Guinness Brewing Co., John Woo, Chow Yun Fat, ",
	"Jackie Chan,Proposition 215, Rumiko Takahashi, Bunchy & Mason, ",
	"The Friends of Stan, The Zoo Crew, Team Hamro, Brazil 2001, RUSH, ",
	"Cornholio, THE BROS., Dar, Emeril Lagasse, Tom Waits, Ice-Cube, ",
	"The Strike Team Deflectors, Tony Levin, Big Jim and the Twins, ",
	"Jag`rmeister, The Megasphere, Condor, The Allower, The Sunday ",
	"Night Group, Gravitar, Steinway Pianos, Round Table Pizza, The Poxy ",
	"Boggards, Urban Mystic & Co., Caffeine, Hunter Rose, Marly ",
	"mediums in the box, sweet Poteen, Dave Scarpitti, TheByter, Bim ",
	"Man, Krissann for color, Patty at Avalon Tattoo, James, Whaleboy, ",
	"Dunsel Training Institute, monkeys, Dob Bole, Rootes Group, Porshe, ",
	"Bongo, Scarlett, Apollo, The House of Mediocrity, Amelias, The King, ",
	"Saag and all the folks at Davidson",
	"",
	"$In memory of",
	"Lawrence and Barbara Williams",
	"David A. Hargrave",
	"Tony \"Xatre\" Collin",
	"Thomas H Sexton",
	"",
	"$Very Special Thanks to",
	"Bob and Jan Davidson",
	"Walter Forbes",
	"",
	"$The Ring of One Thousand",
	"Andrew Abernathy, Christopher Abramo, David Adams, David ",
	"Adcox, Marko Agterberg, Cory Aiken, Judah Altaras, John ",
	"Alvarez, Jose Alvarez, Richard Amable, Alexander Amaral, ",
	"Scott Amis, Vincent Amoroso, Mark An, David Andersen, Jason ",
	"Andrew Abernathy, Christopher Abramo, David Adams, David ",
	"Adcox, Marko Agterberg, Cory Aiken, Judah Altaras, John ",
	"Alvarez, Jose Alvarez, Richard Amable, Alexander Amaral, ",
	"Scott Amis, Vincent Amoroso, Mark An, David Andersen, Jason ",
	"Andersen, Aaron Anderson, George Anderson, Matthew ",
	"Anderson, Michael Anderson, Sean Anderson, Robert Andrade, ",
	"Cerny Andre, Michael Andreev, Devin Angle, Brian Apple, ",
	"Brian Arbuthnot, Billy Arden, Dorian Arnold, Andre Arsenault, ",
	"Erik Asplund, Mark Assam, John Athey, Jason Attard, Jeff ",
	"Atwood, Ricky Au, Scott Avery, Rand Babcock, Steve Babel, ",
	"Raymond Bacalso, Ed Bachta, Steven Back, Scott Baeder, Alex ",
	"Baevski, Scott Bailey, Kenneth Baird, Thomas Baker, Todd ",
	"Bala, Jan Ball, Greg Baltz, Blake Baltzley, Doug Bambrick, Wes ",
	"Bangerter, Paul Barfuss, Chris Barghout, Dave Barnebey, Jon ",
	"Barnhart, Terje Barth, Nicole Baskin, Bernard Baylen, Ryan ",
	"Bear, Phil Bedard, Todd Belcher, Chip Bell, Erez Ben-Aharon, ",
	"Jonathan Bender, Nick Bennett, Ireney Berezniak, Ted Berg, ",
	"Gunnar Bergem, Russell Beuker, Ed Bickford, Stephen Biles, ",
	"John Billdt, Gerald Binder, John Bird, Hannah Blackerby, Tom ",
	"Blackerby, Clayton Blackwell, Thomas Blake, Shawn Blaszak, ",
	"Daniel Bliss, Fred Bliss, Jeff Bliss, Jon Blum, Rune Boersjoe, ",
	"Andrew Boggs, Dave Boisvenu, Joe Bolt, John Bonds, Jeff ",
	"Borenstein, Dorian Borin, Ed Boris, Bartholomew Botta, ",
	"Michael Boutin, Michael Boyd, Charles Boyer, Mike ",
	"Boyersmith, Michael Boyko, Eric Bradberry, John Brandstetter, ",
	"Ryan Breding, Paul Brinkmann, Patrick Briscoe, Scott Brisko, ",
	"Jeremy Britton, Adrian Broadhead, Glenn Brouwer, Joe Brown, ",
	"Sebastien Brulotte, Darrell Brunsch, William Bryan, Jason ",
	"Bucher, Chris Buchinger, Clayton Buckingham, John Buckles, ",
	"David Bugay, Ed Bujone, Erik Burgess, Gabriel Burkett, Chris ",
	"Burnes, Richard Butler, Jeffrey Bye, Dougall Campbell, Donnie ",
	"Cannon, Shane Cantrell, Connor Caple, Daniel Carey, James ",
	"Carlton, Michael Carmel, Mike Carpenter, Ronald Carruthers, ",
	"Phil Carter, Bryce Cartmill, Eric Cartwright, Anthony Caruso, ",
	"Lon Casey, Tim Caster, Aaron Chan, Otto Chan, Gene Chang, ",
	"Hsiao-Lung Chang, William Chang, George Chappel, Larry ",
	"Charbonneau, Troy Chase, Bruce Chen, Chun Hsien Chen, Sam ",
	"Chen, Tzu-Mainn Chen, Mike Cheng, Chris Chiapusio, Damien ",
	"Chiesa, Nick Chin, Nim Ching, Jonathan Chinn, Michael Chinn, ",
	"Philander Chiu, Thayne Christiansen, Philip Chui, Steve ",
	"Cintamani, Richard Ciordia, Colin Clark, Steve Clark, Samuel ",
	"Clements, Meredith Clifton, Jeff Cohen, Dale Colton, Dax ",
	"Combe, Matt Compton, Jacob Conklin, Richard Conn, Zac ",
	"Cook, Tim Coolong, Gregory Coomer, Dennis Cosgrove, Kelly ",
	"Couch, Andrew Coulter, Eric Coutinho, David Coutts, James ",
	"Craig, John Craig, Kazial Craig, John Crawford, Marcelo ",
	"Crespo, Orville Crews, Tim Cristy, Elmer Crosby III, Russell ",
	"Cullison, Ryan Cupples, Andrew Dagley, Steve Dallaire, Richard ",
	"Dalton, David Dandar, Pro Daulo, Rob Dautermann, Mike ",
	"Davies, Kalieb Davis, Marshall Davis, Ronald Davis, Danny De ",
	"Bie, Marc De Filippis, Myles Deighton, Kent Dejarnett, Anthony ",
	"Delarosa, Neil Dempster, Peter Denitto, Joaquim Dentz, Scott ",
	"Dewar, Anish Dhingra, Philip Dhingra, David Diaz, Stewart ",
	"Dicks, Brad Dietz, Josh Dietz, Colleen Diggins, Mike Ditchburn, ",
	"Eric Dittman, Allen Do, Huy Doan, Matthew Dolman, Antoine ",
	"Dongois, Eamonn Donohoe, Bill Dorell, Mark Dorison, Dan ",
	"Dorsett, Chris Dorsey, Jim Dose, Willis Doss, Chin Du, William ",
	"Dubis, Timothy Duewell, Brandon Dunn, Andrew Durham, Don ",
	"Duvall, Kevin Dvojack, Daniel Eaton, Paul Eberting, Eric ",
	"Echelbarger, Lance Eddleman, Ben Eggers, David Eggum, John ",
	"Ehde, Brian Eikenberry, Patrick Elven, Peter Engdahl, Philip ",
	"Engdahl, Michael Ennis, Darren Eslinger, Eric Ezell, Darren ",
	"Falslev, Stephen Feather, Tony Fenn, Ben Ferguson, Mike ",
	"Fernandez, Gwendal Feuillet, Guy Fietz, Julian Figueroa, Dale ",
	"Fillpot, Stan Finchem, Michael Finley, Nick Fisher, William ",
	"Fisher, Mark Fitlin, Dave Flatt, Joel J. Flores, John Folkers, ",
	"Steven Forgie, Tom Forsythe, Matthew Foster, Scott Francis, ",
	"Jim Frank, Paulo Fraser, Glenn French, Kurt Frerichs, Chris ",
	"Frey, Mark Friedman, Charles Friedmann, Dan Friend, Kirk ",
	"Fry, Aaron Fu, Erik Gaalema, Brandon Gabbard, Phil Gagner, ",
	"Tommy Gannon, David Gappmayer, Chris Garrison, Tony ",
	"Garrison, David Gasca, Jeremy Gasser, Michael Geist, Michael ",
	"Genereux, Daniel Genovese, Josh Gerwin, Paul Gibson, William ",
	"Gilchrist, Gabriel Gils Carbo, Chad Glendenin, Ryan Glinski, ",
	"Dean Gobrecht, Andrew Goldfinch, David Goodman, Mark ",
	"Goodson, Matt Gordon, Frank Gorgenyi, Sean Gould, Perry ",
	"Goutsos, Ed Govednik, Michael Grayson, Chris Green, Justin ",
	"Grenier, Jeff Greulich, Don Grey, Rob Griesbeck, Don Griffes, ",
	"Kimberly Griffeth, Jay Grizzard, Don Gronlund, Joe Gross, ",
	"Troy Growden, Greg Guilford, David Gusovsky, Jeremy ",
	"Guthrie, Adam Gutierrez, James Guzicki, Matthew Haas, Matt ",
	"Hadley, Ryan Hagelstrom, Bobby Hagen, Ben Hall, Brian Hall, ",
	"Kris Hall, Calvin Hamilton, Kris Hamilton, Bo Hammil, Dave ",
	"Hans, Rick Hansen, Robert Harlan, Travis Harlan, Seth ",
	"Harman, Jeff Harris, Shawn Hartford, Adam Hartsell, Neil ",
	"Harvey, Ray Hayes, John Hein, Chris Heinonen, Christer ",
	"Helsing, Chris Hempel, Dustin Hempel, Mathieu Henaire, Matt ",
	"Henry, Chuck Herb, Michael Herron, Sage Herron, Thomas ",
	"Herschbach, Cliff Hicks, Nelson Hicks, Paul Hierling, William ",
	"Hiers, Mike Higdon, Tim Hildebrand, Casey Hinkle, Ryan ",
	"Hitchings, Wes Hix, Alan Ho, Jenson Ho, Alan Hoffman, Jeff ",
	"Hoffman, Eleanor Hoffmann, Steve Hogg, Richard Holler, Brian ",
	"Homolya, Wade Hone, Joe Horvath, Jeff Howe, Eric Hudson, ",
	"Glen Huey, Chris Hufnagel, Joshua Hughes, Melissa Hughes, ",
	"Arief Hujaya, Thomas Hulen, Ryan Hupp, Justin Hurst, Rick ",
	"Hutchins, Steve Iams, Mike Iarossi, Bjorn Idren, Johan Idren, ",
	"Micah Imparato, Joe Ingersoll, David Ingram, Greg Ipp, Rodney ",
	"Irvin, Darin Isola, Justin Itoh, Mario Ivan, Fredrik Ivarsson, ",
	"Dax Jacobson, Michael Jacques, Stevens Jacques, Duane Jahnke, ",
	"William Jambrosek, Daniel Janick, Narciso Jaramillo, Neil ",
	"Jariwala, Harvie Jarriell, Scott Javadi, Joe Jenkins, Bart ",
	"Jennings, Paul Jennings, Julien Jenny, Jason Jensen, Martin ",
	"Jeremy, Mark Jeschke, Andy Johnson, James Johnson, Leigh ",
	"Johnson, Mark Johnson, Rupert Johnson, Clyde Jones, Michael ",
	"Jones, Tim Jordan, Ben Judy, Michael Kaae, Steve Kaczkowski, ",
	"Neville Kadwa, Brian Kaisner, Yoshihisa Kameyama, Michael ",
	"Kanemura, Daniel Kao, Eric Karabin, Ben Katz, Christopher ",
	"Kawamura, Erick Kayser, Craig Keddie, Kevin Kelley, Bryan ",
	"Kemp, Michael Kendrigan, Dan Kerber, Timothy Kerber, Tomi ",
	"Keski-Heikkila, Greg Kettering, Nathan Kilber, Howard Kim, ",
	"Orrin Kinion, Jon Kirst, David Kitch, John Klingbeil, Neil ",
	"Klopfenstein, Kerry Knouse, David Knox, Said Kobeissi, Jeff ",
	"Koches, Hades Kong, Jeff Kong, Kevin Konkle, Steve Koon, ",
	"David Koontz, Dan Koopmann, Steve Koskela, Kuan Kou, ",
	"Cameron Kracke, Jensen Krage, York Kramer, Cedar Kraus, ",
	"Jason Kraus, Bobby Krimen, Melissa Krispli, Steven Krispli, ",
	"James Kruger, Charles Kubasta, Kimmo Kulonen, Frank ",
	"Lackaff, Michael Lacour, Matt Lake, Jason Landry, Hans Erik ",
	"Lange, Michael Laramee, Brad Lascelle, Pat Laschinger, Alan ",
	"Lau, Sean Laurence, Anthony Lavey, Jr., Gary Le, Huey Le, ",
	"Stephane Le Roy Audy, Lim Leandro, Charles Lee, Conroy Lee, ",
	"Mike Lee, Shih-Hang Lee, Jonathan Leipert, Jason Lemann, ",
	"Ron Lenzi, Mitchell Leon, Stephanie Lesniewski, Brendan ",
	"Lewis, Robert Lewis, Sam Liao, Tom Liem, Adam Ligas, Steven ",
	"Liggett, Roger Lilley, Benjamin Lim, Jeff Lindholm, Johnson ",
	"Linwood, David Litchman, Bruce Lithimane, William Liu, ",
	"Wilson Liu, Robert Lobdell, Chris Logan, Razvan Loghin, Jack ",
	"Loh, George Loo, Russell Love, Juan Loyola, Ricardo Lozano, ",
	"Mike Luban, Tim Luc, Henry Luciano, Dianne Ludwig, Charles ",
	"Lueras, Derek Lung, Phong Ly, Scott MacGillivray, Dave Mack, ",
	"Alec Mak, Steve Mamayek, Michael Mancini, Daniel Mann, ",
	"Michael Mann, Chris Manofsky, Abdullah Marafie, Nicholas ",
	"Marcy, Piompino Mariano, Bob Marius, Trey Marshall, Dane ",
	"Martin, Gregg Martin, Renard Martin, Rich Martin, Scott ",
	"Martin, Thomas Martin, Jon Masters, Christopher Mathews, ",
	"Jay Mathis, Marc Matthews, Chris Mazur, Doug McBride, ",
	"Mackey McCandlish, Robin McCollum, Steven McCombie, ",
	"Andy McConnell, Michael McCourt, Bill McCoy, Doug ",
	"McCracken, Michael McDeed, Robert McDonald, Steve ",
	"McEachron, Craig McGee, Ryan McKenzie, Michael McKeown, ",
	"Daniel McMahon, Colin McMillan, Ian McWilliam, Mark ",
	"McWilliams, Khann Mean, Bryan Meason, Kenneth Medley, ",
	"Jeff Meek, John Mehr, Christopher Mende, Brian Mendenhall, ",
	"Peter Mengel, Michael Mersic, Mike Messom, Don Metcalf, ",
	"Gary Metzker, Scott Meyer, Joseph Michaud, Andrew Mielke, ",
	"Travis Mikalson, Troy Milburn, Ike Miller, Ronnie Miller, Sean ",
	"Miller, Steve Miller, Arthur Min, David Minniti, Brenda ",
	"Mirsberger, Bill Misek, David Mitchell, Joseph Mobley, Robert ",
	"Mollard, Will Mooar, Curtis Moore, Matthew Moore, Al ",
	"Morales, Ryan Moran, Lance Mortensen, Karel Mrazek, Ward ",
	"Mullee, William Munoz, Kirk Munro, Craig Murray, Shawn P. ",
	"Murray, Travis Murray, Michael Mushrush, Tom Mustaine, ",
	"David Myers, Joseph Myett, Morgan Najar, Kenta Nakamura, ",
	"Damian Nastri, Joshua Naumann, Nick Navarro, Douglas ",
	"Neitzel, Arnold Ng, Anthony Nguyen, Steve Nguyen, Joseph ",
	"Nicholas, Charles Nickolaus, Jon Nisbet, Patrick Nomee, David ",
	"Norling-Christensen, Bobby Norton, Joseph Nottingham, Frank ",
	"O'Connor, Jon Oden, David Oester, Lavern Ogden, Zach ",
	"Oglesby, Lucas Oldfield, Toby Olsson, Aaron Ondek, Sean ",
	"O'Neill, John Orlando, Samuel Orlando, Donovan Orloski, ",
	"David Pai, Nikolas Paldan, David Palek, John Palmieri, Anthony ",
	"Palmisano, Sanjay Pandit, Jesse Park, Alex Parker, Jimmy ",
	"Pasher, Lukasz Paszek, Andy Patterson, William Pelletier, ",
	"Duane Pemberton, Ivan Pemic, Kelly Pendergast, Mike ",
	"Penezich, Jon Penk, Willie Penley, Ron Penna, Matthew ",
	"Pennington, Kevin Pereira, Ross Perez, Ken Perkins, Brian ",
	"Peterik, Kelly Peterson, Chris Phillips, Rod Pickett, Cameron ",
	"Pierce, Reuben Pierce, Tim Pilger, Billy Pippin, Brad Plank, ",
	"Brian Plant, Craig Platt, David Plunkett, Michael Politi, Albert ",
	"Portillo, Brian Powell, David Powell, Franklin Powers Jr., Alan ",
	"Precourt, Michael Pronchick, Julian Quintana, Justin Radziej, ",
	"Steven Rajewski, Shawn Rawles, Ian Reardon, Marc Reed, Ric ",
	"Reichelt, Judd Reiffin, David Reilly, Garry Reisky, Drew ",
	"Ressler, Robert Reynolds, Walter Reynolds, Michael Rice, Ian ",
	"Richards, James Richards, Raymond Richmond, Dustin Riggs, ",
	"Keith Riskey, Brian Ro, Scott Roberts, Jorge Rodriguez, Chad ",
	"Rogers, Clint Rogers, Robert Rogers, Steve Rogers, Ethan Roots, ",
	"Ron Roque, William Ross, Sebastian Rossi, Jeff Rostis, Ben ",
	"Roth, Demar Roth, Rich Rouse, Oleg Rovner, Jonathan Roy, ",
	"Drew Rozema, Mike Ruggles, Mathias Russ, James Russell, Jim ",
	"Rutledge, James Rutter, Dave Ryder, Chris Salvadras, Anders ",
	"Samnerud, Nick Sanders, Jakob Sandgren, Joe Sapinsky, Tyler ",
	"Sargent, Jonas Saunders, Mark Savage, Scott Sawyer, Robert ",
	"Scanlon, Trevor Schaben, Aaron Schmidt, Chris Schmidt, Greg ",
	"Schultz, Nicholas Schumacher, Scott Schumacher, Kevin Scott, ",
	"Rastislav Seffer, Robert Seidler, Corey Sellers, Justin Sellers, ",
	"Marc Senecal, George Shannon, Ian Sheffield, Anoop Shekar, ",
	"Sandeep Shekar, Kevin Shelton, Leon Shephard, Eric Shepperd, ",
	"Jeffrey Shneidman, Samuel Shockey, Mark Shoemaker, Mike ",
	"Shupe, Sean Sibbet, Brian Sidharta, Jimmy Sieben, Eric ",
	"Siemens, William Silva, Jody Simpson, Jatinder Singh, Sonia ",
	"Siu, Omar Skarsvaag, Tom Skiba, Carl Skow, David Skuse, ",
	"Robert Slifka, Brent Smith, C. Eric Smith, Jared Smith, Jeffrey ",
	"Smith, Owen Smith, Shannon Smith, Steven Smith, Edward Smola, ",
	"Matthew Sneep, Eric Snow, Brad Sobel, Jean-Pierre Solignac, Rasmus ",
	"Sorensen, Andrew Sorg, Poppy Southcott, Ross Specter, Erik Spencer, ",
	"Keith Spencer, Chris Springer, Erikson Squier, Dean St. Onge, Stewart ",
	"Stanfield, John Stanley, Terrence Staton, Benjamin Stein, James ",
	"Steiner, David Steyer, William Stickney, Chris Stiff, James Stofer, ",
	"Norm Storch, Patrick Stovall, Brandon Sturgeon, Sean Stutler, Anne ",
	"Sukprasert, Jamal Sullivan, Bruce Sully, Jay Sung, Park Sung ",
	"Joon, Stein Sunnarvik, Nathan Surginer, Robert Swaringen, Lee ",
	"Sweeney, David Szabo, Scott Taft, Christian Takvam, Patrick ",
	"Talaska, Tadashi Tamaki, Simon Tan, Mark Taraba, Khon-",
	"Whey Tay, John Taylor, Keith Taylor, Donald Temean, Phillip ",
	"Tesar, Pete Thao, Brian Thomas, Keith Thomas, Speed Thomas, ",
	"Miles Thorpe, Paul Thurrott, Mike Timbol, Nicholas Timmins, ",
	"Tom Tobin, Robert Towster, Hien Tran, Timothy Traviss, Toby ",
	"Traylor, Tony Treadwell, George Tremoulis, Paul Trinh, Thanh ",
	"Trinh, Chris Tristan, Brad Truswell, Jason Tryon, Mike Tu, ",
	"Gernel Tuazon, Eric Tuggle, Mike Turnbull, Lyle Ubben, ",
	"Amilcar Ubiera, Robert Ulozas, Arie Upton, Mark Van Noy, ",
	"Matthew Van Sickler, Jake Vantlin, Tony Vasquez, Brady ",
	"Vauclin, Gianpiero Vecchi, Chad Verrall, Chris Vicente, Brett ",
	"Vickers, David Vickery, Jonathan Vilante, Simon Vince, Ben ",
	"Vinson, David Voelkert, Paul Vogt, Nicholas Voorhies, Robert ",
	"Vreeland, Mike Vrooman, Rick Vuong, Brian Wachhaus, Todd ",
	"Wachhaus, Caine Wade, Mathew Wadstein, Kenneth Wagenius, ",
	"Trevor Wagner, John Wagstaff, Asad Wahid, Richard ",
	"Wahlberg, Helio Wakasugui, Richard Walker, Wilkins Walker, ",
	"Matthew Wallace, Daniel Walsh, Joel Walters, Andrew Waltz, ",
	"Tom Wang, Tony Wang, Jay Ward, Jonathan Ward, John ",
	"Warner, Mark Warren, Matt Washer, Mike Watson, Sean ",
	"Wattles, Mike Wayne, Christian Wehba, Benjamin Wei, Richard ",
	"Weight, Cary Wells, David Wenck, Bill Werring, Leonard ",
	"Wesley, Marlon West, Mikael Westerbacka, Brian Wharry, ",
	"Chris White, Chris White, Jeremy White, Greg Whitlock, Gary ",
	"Widener, Marty Wilfried, Israel Wilkinson, Michael Willams, ",
	"Derek Williams, Sean Willson, Nitzan Wilnai, Jim Wilson, ",
	"Karsten Wilson, William Wilt, Tim Winn, Brian Winzeler, Matt ",
	"Wise, Lee Wissmiller, Brendan Wolfe, Daniel Wolpert, Felix ",
	"Wong, Power Wong, Tony Wong, Tim Wood, Timothy Wood, ",
	"Jeremy Woods, Michael Woods, Bill Wright, Keith Wright, ",
	"Patrick Wu, Gang Xie, Robert Yao, Sun Lim Yap, Stephen Yau, ",
	"Erek Yedwabnick, Christopher Yee, Nick Yee, Juan Yip, David ",
	"Young, Rob Young, Seth Young, Alex Yu, Terry Zahn, Jia-Ning ",
	"Zhang, Eric Zieg, Jordan Zielin, Clint Zimmerman, Matt Zinke ",
	"",
	"",
	"",
	"",
	"$No souls were sold in the making of this game.",
	"",
	"",
	" ",
	" ",
	" ",
	NULL
};

int creditline = 0;
int ybase = 0;
int linecount = 24;
int nottheend = 1;

void pain_credts()
{
	ClearScreenBuffer();

	DWORD dwData[2];

	if(bloaded) {
	LoadArtWithPal("ui_art\\credits.pcx", &pPcxCreditsImage, 1, dwData);
	gdwCreditsWidth = dwData[0];
	gdwCreditsHeight = dwData[1];
	bloaded = 0;
	}

	ybase += 1;
	if(ybase >= pFont2[1])
	{
		ybase = 0;
		if(!nottheend)
		{
			linecount--;
			creditline++;
		}
		else if(the_long_ass_credits[creditline+24] == NULL)
		{
			nottheend = 0;
			//linecount--;
		}
		else
			creditline++;
	}

	DrawArtImage(0, 0, gdwCreditsWidth, gdwCreditsHeight, 0, pPcxCreditsImage);
	for(int i = 0; i < linecount; i++) {
		if(*the_long_ass_credits[creditline+i] == '$')
			DrawPCXString(	320-(GetPCXFontWidth(the_long_ass_credits[creditline+i]+1, pFont2)/2),
							50+(i*pFont2[1])-ybase,
							gdwFont2Width,
							gdwFont2Height,
							the_long_ass_credits[creditline+i]+1,
							pFont2,
							pPcxFont2Image);
		else
			DrawPCXString(	320-(GetPCXFontWidth(the_long_ass_credits[creditline+i], pFont3)/2),
							50+(i*pFont3[1])-ybase,
							gdwFont3Width,
							gdwFont3Height,
							the_long_ass_credits[creditline+i],
							pFont3,
							pPcxFont3Image);
	}

	if(bloaded) {
		//bloaded = 0;
	//mem_free_dbg(pPcxCreditsImage);
	}
}

void __cdecl paint_select_class()
{
	char names[5][16]; // [esp+Ch] [ebp-88h]
	int width[5]; // [esp+5Ch] [ebp-38h]
	int a2; // [esp+70h] [ebp-24h]
	int j; // [esp+74h] [ebp-20h]
	int v4; // [esp+78h] [ebp-1Ch]
	int i; // [esp+7Ch] [ebp-18h]
	int a1; // [esp+80h] [ebp-14h]
	int v7; // [esp+84h] [ebp-10h]
	int num_lines; // [esp+88h] [ebp-Ch]
	int start_line; // [esp+8Ch] [ebp-8h]
	int v10; // [esp+90h] [ebp-4h]
	ClearScreenBuffer();
	DrawArtImage(0, 0, gdwTitleWidth, gdwTitleHeight, 0, pPcxTitleImage);
	DrawArtWithMask(320-(gdwLogoWidth/2), 0, gdwLogoWidth, gdwLogoHeight, the_pcx_frame, 250, pPcxLogoImage);
	strcpy(names[0], "Enter Name");
	// *&names[0][11] = 0;
	// names[0][15] = 0;
	strcpy(names[1], "Choose Class");
	// *&names[1][13] = 0;
	// names[1][15] = 0;
	strcpy(names[2], "Warrior");
	// *&names[2][8] = 0i64;
	strcpy(names[3], "Rogue");
	// *&names[3][6] = 0.0;
	strcpy(names[4], "Sorceror");
	// *&names[4][9] = 0;
	// *&names[4][13] = 0;
	// names[4][15] = 0;
	width[0] = 272;
	width[1] = 312;
	width[2] = 218;
	width[3] = 149;
	width[4] = 227;
	//CelDecodeOnly(64, 639, pTitlgrayCel_sgpBackCel, 1, 640);
	//CelDecodeOnly(236, 282, pDiabfrCel, 1, 296);//CelDecodeOnly(64, 377, pDiabfrCel, diablogo_cel_frame, 640);
	if ( (is_typing_plr_name & 0x80u) == 0 )
	{
		start_line = 1;
		num_lines = 5;
	}
	else
	{
		start_line = 0;
		num_lines = 1;
		sgdwProgress = 0;
	}
	for ( i = start_line; i < num_lines; ++i )
	{
		a1 = ((640 - width[i]) >> 1) + 64;
		a2 = 45 * (i - start_line) + 350;
		print_title_str_large(a1, a2, names[i]);
		if ( num_lines == 1 )
		{
			v10 = 0;
			v7 = strlen(chr_name_str);
			for ( j = 0; v7 > j; ++j )
			{
				v4 = lfontframe[chr_name_str[j]];
				v10 += lfontkern[v4] + 2;
			}
			a1 = 374 - (v10 >> 1);
			a2 = 9 * (5 * (i - start_line) + 5) + 450;
			print_title_str_large(374 - (v10 >> 1), a2, chr_name_str);
		}
		if ( i == sgdwProgress )
		{
			CelDecodeOnly(155, a2 + 1, pPentitleCel, progress_id, 48);
			CelDecodeOnly(563, a2 + 1, pPentitleCel, progress_id, 48);
			// if ( demo_mode )
			// {
				// if ( sgdwProgress >= 3 )
				// {
					// a1 = 244;
					// print_title_str_small(244, 624, "Not available in demo");
				// }
			// }
		}
	}
}

void __cdecl paint_title_cel()
{
	switch ( sgdwProgress )
	{
		case 1:
			CelDecodeOnly(64, 639, pTitlgrayCel_sgpBackCel, 1, 640);
			break;
		case 2:
			CelDecodeOnly(64, 639, pTitltextCel, 1, 640);
			break;
		case 3:
			CelDecodeOnly(64, 639, pPentitleCel, 1, 640);
			break;
		case 4:
			CelDecodeOnly(64, 639, pTitlqtxtCel, 1, 640);
			break;
		case 5:
			CelDecodeOnly(64, 639, pDiabfrCel, 1, 640);
			break;
		case 6:
			CelDecodeOnly(64, 639, p_reserved_title, 1, 640);
			break;
		default:
			return;
	}
}

void __fastcall switch_title_option(int type)
{
	byte_5DE90C = 0;
	lock_buf_priv();
	switch ( type )
	{
		case 1:
			paint_title_screen();
			break;
		case 2:
			paint_select_class();
			break;
		case 3:
			ClearScreenBuffer();
			paint_title_cel();
			break;
		case 4:
			DrawCutscene();
			break;
		case 5:
			LoadNames(0);
			break;
		case 6:
			paint_diff_screen();
			break;
		case 7:
			pain_credts();
			break;
		default:
			break;
	}
	unlock_buf_priv();
	drawpanflag = 255;
}

void free_title_shit();
void interfac_init_title_play_music();

void DrawMouse()
{
	int lines = gdwCursorHeight;
	if(MouseY > 480-gdwCursorHeight)
		lines -= (MouseY - (480-gdwCursorHeight));
	int mx = MouseX;
	if(mx < 0) mx = 0;
	if(mx >639) mx = 639;
	int my = MouseY;
	if(my < 0) my = 0;
	if(my > 480) my = 480;

	lock_buf_priv();
	DrawArtWithMask(mx, my, gdwCursorWidth, lines, 0, 0, pPcxCursorImage);
	unlock_buf_priv();
}
int gbStopGame = 0;
void __fastcall interface_pump_title_message(HWND hWnd)
{
	while ( gbProcessFrame )
		;
	gbProcessFrame = 1;
	if(playmovie)
	{
		playmovie = 0;
		music_stop();
		//play_movie("gendata\\diablo1.smk", 1);
		mainmenu_refresh_music();
		LoadPalInMem(pcxPal);
		lpDDPalette->SetEntries(0, 0, 256, orig_palette);
	}
	if ( sgbMenuOption )
	{
		if ( sgbMenuOption == 1 )
		{
			PaletteFadeIn(8); //DoFadeIn();
			if ( !sgbMenuOption )
				PostMessageA(hWnd, 0x408u, 0, 0);
		}
		if ( sgbMenuOption == 2 )
		{
			if ( title_cel_is_loaded & 0x40 )
				sgbMenuOption = 0;
			else
				PaletteFadeOut(8); //DoFadeOut();
			if ( !sgbMenuOption )
				PostMessageA(hWnd, gNewMSG, 0, 0);
		}
		if(sgbMenuOption == 7)
		{
			free_title_shit();
			if(!TEMP_diablo_init_menu(GetKeyState(VK_SHIFT) < 0, 1))
			{
				gbStopGame = 2;//PostMessage(ghMainWnd, WM_CLOSE, 0, 0);
				return;
			}
			else
			{
				interfac_init_title_play_music();
				SetWindowProc(GM_Title);
				LoadPalInMem(pcxPal);
				mainmenu_refresh_music();
				gb_title_option = 1;
				sgdwProgress = 0;
				sgbMenuOption = 0;
				//InitCursor();
				//InitLevelCursor();
			}
		}
		lpDDPalette->SetEntries(0, 0, 256, orig_palette);
	}
	// lock_buf_priv();
	// scrollrt_draw_cursor_back_buffer();
	// unlock_buf_priv();
	switch_title_option(gb_title_option);
	// if ( (is_typing_plr_name & 0x80u) == 0 ) {
		// lock_buf_priv();
		// scrollrt_draw_cursor_item();
		// unlock_buf_priv();
	// }
	DrawMouse();

	scrollrt_draw_game_screen(0); // set to 1 to draw mouse through engine instead of PCX

	if ( ++progress_id == 9 )
		progress_id = 1;
	if ( ++diablogo_cel_frame == 29 )
		diablogo_cel_frame = 1;
	if ( ++the_pcx_frame == 15)
		the_pcx_frame = 0;
	new_timer_2_int = 0;
	gbProcessFrame = 0;
}

void __fastcall diablo_procwait_4882F9(int a1)
{
	while ( gbProcessFrame )
		;
	new_timer_2_int = a1;
	gbProcessFrame = 0;
}

void __cdecl interfac_set_player_some()
{
	chr_name_str[0] = '-';
	chr_name_str[1] = 0;
	gb_title_option = 2;
	is_typing_plr_name = 0;
	sgdwProgress = 2;
}

void __cdecl interfac_init_title_play_music()
{
	//ui setup
	DWORD dwData[2];

	LoadArtWithPal("ui_art\\mainmenu.pcx", &pPcxTitleImage, 1, dwData);
	gdwTitleWidth = dwData[0];
	gdwTitleHeight = dwData[1];

	LoadArtImage("ui_art\\smlogo.pcx", &pPcxLogoImage, 15, dwData);
	gdwLogoWidth = dwData[0];
	gdwLogoHeight = dwData[1];

	LoadArtImage("ui_art\\font42g.pcx", &pPcxFontImage, 256, dwData);
	gdwFontWidth = dwData[0];
	gdwFontHeight = dwData[1];
	pFont = LoadFileInMem("ui_art\\font42.bin", 0);

	LoadArtImage("ui_art\\heros.pcx", &pPcxHeroImage, 4, dwData);
	gdwHeroWidth = dwData[0];
	gdwHeroHeight = dwData[1];

	LoadArtImage("ui_art\\selhero.pcx", &pPcxSHeroImage, 1, dwData);
	gdwSHeroWidth = dwData[0];
	gdwSHeroHeight = dwData[1];

	LoadArtImage("ui_art\\cursor.pcx", &pPcxCursorImage, 1, dwData);
	gdwCursorWidth = dwData[0];
	gdwCursorHeight = dwData[1];

	LoadArtImage("ui_art\\font16g.pcx", &pPcxFont2Image, 256, dwData);
	gdwFont2Width = dwData[0];
	gdwFont2Height = dwData[1];
	pFont2 = LoadFileInMem("ui_art\\font16.bin", 0);

	ShiftStrColor(gdwFont2Width, gdwFont2Height, 256, pPcxFont2Image, 32, -56, 1);

	LoadArtImage("ui_art\\font16s.pcx", &pPcxFont3Image, 256, dwData);
	gdwFont3Width = dwData[0];
	gdwFont3Height = dwData[1];
	pFont3 = LoadFileInMem("ui_art\\font16.bin", 0);

	//ShiftStrColor(gdwFont3Width, gdwFont3Height, 256, pPcxFont3Image, 32, 48, 1);

	title_cel_is_loaded = 0;
	is_typing_plr_name = 0;
	/* if ( !dword_4BC180 && debug_mode_music_enabled )
	{
		SFileOpenFile("Music\\Dintro.WAV", &dword_4BC180);
		SFileDdaBegin(dword_4BC180, 0x40000, 0x40000);
	} */
	pTitlgrayCel_sgpBackCel = LoadFileInMem("Gendata\\Titlgray.CEL", 0);
	//pPlayerGFX = LoadFileInMem("PlrGFX\\warrior\\whn\\whnat.CL2", 0);
	pPentSmall = LoadFileInMem("Data\\PentSpn2.CEL", 0);
	pPanelText = LoadFileInMem("CtrlPan\\SmalText.CEL", 0);
	pTitltextCel = LoadFileInMem("Data\\BigTGold.CEL", 0);
	pPentitleCel = LoadFileInMem("Data\\PentSpin.CEL", 0);
	pSTextBoxCels = LoadFileInMem("Data\\TextBox2.CEL", 0);
	pTitlqtxtCel = LoadFileInMem("Data\\MedTextS.CEL", 0);
	pDiabfrCel = LoadFileInMem("Data\\Diabsmal.CEL", 0);
	sgdwProgress = 0;
	progress_id = 1;
	the_pcx_frame = 0;
	diablogo_cel_frame = 1;
	/* load_title_game00();
	if ( some_save_flag )
		dword_603F10 = 1; */
	gb_title_option = 1;
	switch_title_option(1);
	//SetCursor(CURSOR_HAND);


}

void free_title_shit()
{
	//pPlayerGFX = 0;
	//mem_free_dbg(pPlayerGFX);
	pSTextBoxCels = 0;
	mem_free_dbg(pSTextBoxCels);
	pPanelText = 0;
	mem_free_dbg(pPanelText);
	pPentSmall = 0;
	mem_free_dbg(pPentSmall);
	pTitlgrayCel_sgpBackCel = 0;
	mem_free_dbg(pTitlgrayCel_sgpBackCel);
	pTitltextCel = 0;
	mem_free_dbg(pTitltextCel);
	pPentitleCel = 0;
	mem_free_dbg(pPentitleCel);
	pTitlqtxtCel = 0;
	mem_free_dbg(pTitlqtxtCel);
	pDiabfrCel = 0;
	mem_free_dbg(pDiabfrCel);

	mem_free_dbg(pPcxLogoImage);
	mem_free_dbg(pPcxFontImage);
	mem_free_dbg(pFont);
	mem_free_dbg(pPcxCursorImage);
	mem_free_dbg(pPcxFont2Image);
	mem_free_dbg(pFont2);
	mem_free_dbg(pPcxFont3Image);
	mem_free_dbg(pFont3);
	mem_free_dbg(pPcxTitleImage);
	mem_free_dbg(pPcxHeroImage);
	mem_free_dbg(pPcxSHeroImage);
}

LRESULT __fastcall PROC2_GM_ClassMenu(HWND hWnd, UINT Msg, WPARAM wParam, LPARAM lParam)
{
	LRESULT result; // eax
	//void *p; // eax MAPDST
	char c; // [esp+1Ch] [ebp-4h]

	if ( Msg > WM_KEYDOWN )
	{
		if ( Msg > WM_MOUSEMOVE )
		{
			if ( Msg > WM_USER )
			{
				if ( Msg == 0x408 )
					return 0;
				if ( Msg != 0x409 )
				{
					if ( Msg == 0x40C )
						return 0;
					return DefWindowProcA(hWnd, Msg, wParam, lParam);
				}

				free_title_shit();
				if ( is_typing_plr_name & 0x40 )
				{
					gNewMSG = 0x40C;
					//InitCutscene();
					sgbMenuOption = 2;
					gbProcType = 14;
					//TEMP_diablo_init_menu(0, 1);
				}
				else
				{
					interfac_init_title_play_music();
					LoadPalette("Gendata\\Title.pal");
					gbProcType = 1;
					is_typing_plr_name = 0;
					//CopyPalette(logical_palette, orig_palette);
					PaletteFadeIn(32);
					drawpanflag = 4;
				}
				result = 0;
			}
			else
			{
				if ( Msg == WM_USER )
				{
					interface_pump_title_message(hWnd);
					return 0;
				}
				if ( Msg != WM_LBUTTONDOWN || sgbMenuOption )
					return DefWindowProcA(hWnd, Msg, wParam, lParam);
				if ( (is_typing_plr_name & 0x80u) != 0 )
				{
					if ( chr_name_str[0] != '-' )
					{
						PlaySFX(IS_TITLSLCT);
						chr_name_str[gb_Lfont_str_len] = 0;
						//InitQuests();
						PaletteFadeOut(32);
						title_cel_is_loaded &= 0xBFu;
						is_typing_plr_name |= 0x40u;
						gNewMSG = 1033;
					}
				}
				else
				{
					sgdwProgress = 2;
					if ( MouseY >= 335 )
						sgdwProgress = 3;
					if ( MouseY >= 380 )
						sgdwProgress = 4;
					PlaySFX(IS_TITLSLCT);
					//if ( demo_mode && sgdwProgress >= 3 )
					//	return 0;
					if ( sgdwProgress == 2 )
						c = 0;
					if ( sgdwProgress == 3 )
						c = 1;
					if ( sgdwProgress == 4 )
						c = 2;
					CreatePlayer(myplr, c);
					is_typing_plr_name |= 0x80u;
				}
				result = 0;
			}
		}
		else if ( Msg == WM_MOUSEMOVE )
		{
			if ( (is_typing_plr_name & 0x80u) == 0 )
			{
				MouseX = lParam & 0xFFFF;
				MouseY = lParam >> 16;
			}
			result = 0;
		}
		else
		{
			if ( Msg != WM_CHAR || sgbMenuOption )
				return DefWindowProcA(hWnd, Msg, wParam, lParam);
			if ( (is_typing_plr_name & 0x80u) != 0 )
			{
				if ( wParam == VK_BACK && gb_Lfont_str_len > 0 )
				{
					chr_name_str[gb_Lfont_str_len-1] = 0;
					if ( gb_Lfont_str_len == 1 )
					{
						chr_name_str[0] = '-';
						chr_name_str[1] = 0;
					}
				}
				if ( wParam >= 'a' && wParam <= 'z' )
					wParam -= ' ';
				if ( wParam >= ' ' && wParam <= ']' && gb_Lfont_pix_width <= 535 )
				{
					if ( chr_name_str[gb_Lfont_str_len-1] == '-' )
						--gb_Lfont_str_len;
					chr_name_str[gb_Lfont_str_len] = wParam;
					chr_name_str[gb_Lfont_str_len + 1] = 0;
				}
				if ( wParam == VK_RETURN && chr_name_str[0] != '-' )
				{
					PlaySFX(IS_TITLSLCT);
					chr_name_str[gb_Lfont_str_len] = 0;
					//InitQuests();
					PaletteFadeOut(32);
					title_cel_is_loaded &= 0xBFu;
					is_typing_plr_name |= 0x40u;
					sgbMenuOption = 7;
					return 0;
					TEMP_diablo_init_menu(0, 1);
					//gNewMSG = 0x409;
				}
			}
			result = 0;
		}
	}
	else
	{
		if ( Msg == WM_KEYDOWN )
		{
			if ( (is_typing_plr_name & 0x80u) == 0 )
			{
				switch ( wParam )
				{
					case VK_RETURN:
						PlaySFX(IS_TITLSLCT);
						//if ( demo_mode && sgdwProgress >= 3 )
						//	return 0;
						if ( sgdwProgress == 2 )
							c = 0;
						if ( sgdwProgress == 3 )
							c = 1;
						if ( sgdwProgress == 4 )
							c = 2;
						CreatePlayer(myplr, c);
						is_typing_plr_name |= 0x80u;
						return 0;
					case VK_UP:
						PlaySFX(IS_TITLEMOV);
						if ( sgdwProgress == 2 )
							sgdwProgress = 4;
						else
							--sgdwProgress;
						return 0;
					case VK_DOWN:
						PlaySFX(IS_TITLEMOV);
						if ( ++sgdwProgress > 4 )
							sgdwProgress = 2;
						return 0;
				}
			}
			if ( wParam == VK_ESCAPE )
			{
				//PaletteFadeOut(32);
				title_cel_is_loaded &= 0xBFu;
				gbProcType = 1;
				gb_title_option = 1;
				sgdwProgress = 0;
				//gNewMSG = 0x409;
			}
			return 0;
		}
		if ( Msg != WM_ACTIVATEAPP )
			return DefWindowProcA(hWnd, Msg, wParam, lParam);
		gbActive = wParam;
		if ( wParam )
			palette_update();
		result = 0;
	}
	return result;
}

void __fastcall MenuKeyDown(WPARAM wParam)
{
	int max;
			if(gb_title_option == 6)
				max = 3;
			else if(gb_title_option == 5)
				max = maxheroes;
			else
				max = 4;

	switch ( wParam )
	{
		case VK_ESCAPE:
			PlaySFX(IS_TITLSLCT);
			if(gb_title_option == 6)
			{
				sgdwProgress = thehero;
			}
			else {
				sgdwProgress = 0;
			}
			if(gb_title_option == 6)
			{
				gb_title_option = 5;
				break;
			}
			if(gb_title_option == 5 || gb_title_option == 7)
			{
				gb_title_option = 1;
				break;
			}
			gbStopGame = 1;//PostMessage(ghMainWnd, WM_CLOSE, 0, 0);
			break;
		case VK_RETURN:
		//interface_pump_title_message(hWnd);
			PlaySFX(IS_TITLSLCT);
			if(gb_title_option == 5)
			{
				if(!heroarray[sgdwProgress].level)
					return;
				strcpy(chr_name_str, heroarray[sgdwProgress].name);
				thehero = sgdwProgress;
				sgdwProgress = 0;
				gb_title_option = 6;
				return;
			}
			if(gb_title_option == 6)
			{
				if(!heroarray[thehero].level)
					return;
				if(sgdwProgress == 0 || sgdwProgress == 1)
					gnDifficulty = DIFF_NORMAL;
				else if(sgdwProgress == 2)
					gnDifficulty = DIFF_NIGHTMARE;
				else if(sgdwProgress == 3)
					gnDifficulty = DIFF_HELL;
				sgbMenuOption = 7;
				return;
			}
			else
			{
				if ( sgdwProgress == 4 )
				{
					PaletteFadeOut(32);
					gbStopGame = 1;//PostMessage(ghMainWnd, WM_CLOSE, 0, 0);
				}
				else if (sgdwProgress == 0)
				{
					sgdwProgress = 0;
					gb_title_option = 5;
				}
				else if(sgdwProgress == 2)
				{
					playmovie = 1;
				}
				else if (sgdwProgress == 1)
				{
					sgdwProgress = 0;
					title_cel_is_loaded |= 0x40u;
					sgbMenuOption = 2;
					gNewMSG = 0x409;
				}
				else if (sgdwProgress == 3)
				{
					sgdwProgress = 0;
					gb_title_option = 7;
					bloaded = 1;
				}
			}
			break;
		case VK_UP:
			PlaySFX(IS_TITLEMOV);
			if ( sgdwProgress )
				--sgdwProgress;
			else
				sgdwProgress = max;
			//if ( sgdwProgress == 1 ) //&& !dword_603F10 )
			//	--sgdwProgress;
			break;
		case VK_DOWN:

			PlaySFX(IS_TITLEMOV);
			//if ( ++sgdwProgress == 1 ) //&& !dword_603F10 )
				++sgdwProgress;
			if ( sgdwProgress > max )
				sgdwProgress = 0;
			break;
	}
}

void MenuMouseDown()
{
	int prog_old = sgdwProgress;
	sgdwProgress = 0;
	if ( MouseY >= 190 && MouseY < 231 )
	{
		/*if ( dword_603F10 == 1 )
			sgdwProgress = 1;
		else */
			sgdwProgress = prog_old;
			PlaySFX(IS_TITLSLCT);
			gb_title_option = 5;
			return;
	}
	if ( MouseY >= 232 && MouseY < 280 ) {
		sgdwProgress = 2;
		gbStopGame = 1;//PostMessage(ghMainWnd, WM_CLOSE, 0, 0);
	}
	if ( MouseY >= 140 && MouseY <= 190) {
		PlaySFX(IS_TITLSLCT);

		if ( sgdwProgress && sgdwProgress != 1 )
			PaletteFadeOut(32);
		else
			title_cel_is_loaded |= 0x40u;
		sgbMenuOption = 2;
		gNewMSG = 0x409;
	}
}

LRESULT CALLBACK GM_Title(HWND hWnd, UINT Msg, WPARAM wParam, LPARAM lParam)
{
	switch(Msg)
	{
		case WM_ACTIVATEAPP:
			gbActive = wParam;
			if ( wParam )
				palette_update();
			break;
		case WM_USER:
			interface_pump_title_message(hWnd);
			return 0;
		case WM_LBUTTONDOWN:
			sgbMouseDown = 1;
			SetCapture(hWnd);
			if(!sgbMenuOption)
				MenuMouseDown();
			break;
		case WM_LBUTTONUP:
			sgbMouseDown = 0;
			ReleaseCapture();
			break;
		case WM_DESTROY:
			// dword_61B754 = 1;
			// timeEndPeriod(uPeriod);
			// free_title_shit();
			ShowCursor(1);
			PostQuitMessage(0);
			break;
		case WM_MOUSEMOVE:
			MouseX = lParam & 0xFFFF;
			MouseY = lParam >> 16;
			break;
		case WM_KEYDOWN:
			MenuKeyDown(wParam);
			return 0;
		case 0x408:
			drawpanflag = 255;
			diablo_procwait_4882F9(0);
			return 0;
		case 0x409:
			if ( !(title_cel_is_loaded & 0x40) )
			{
				free_title_shit();
			}
			if ( sgdwProgress == 0 )
			{
				interfac_set_player_some();
				gbProcType = 2;
				drawpanflag = 255;
				diablo_procwait_4882F9(0);
			}
			else if ( sgdwProgress == 1 )
			{
				gNewMSG = 0x40D;
				//InitCutscene();
				sgbMenuOption = 2;
				gbProcType = 14;
			}
			else if ( sgdwProgress == 2 )
			{
				//interfac_init_exit_screens(hWnd, Msg);
				//LoadPalette("Gendata\\Screen01.pal");
				sgbMenuOption = 2;
				gNewMSG = 0x409;
				gbProcType = 13;
			}
			drawpanflag = 255;
			return 0;
	}

	return DefWindowProcA(hWnd, Msg, wParam, lParam);
}

struct timecaps_tag ptc;
unsigned int uPeriod;
unsigned int uDelay;
int gdwCurTime;
unsigned int guTimerID;
int gbFuckThis = 0;

void __stdcall TimeProc(UINT uID, UINT uMsg, DWORD dwUser, DWORD dw1, DWORD dw2)
{
	long dwTime; // [esp+Ch] [ebp-4h]

	if ( !gbProcessFrame )
	{
		gbProcessFrame = 1;
		dwTime = timeGetTime();
		if ( dwTime - gdwCurTime >= 1000/MENU_FRAME_RATE && !new_timer_2_int ) // 20 frames per second (50 milisecs)
		{
			new_timer_2_int = 1;
			if ( gbFuckThis )
				PostMessageA((HWND)dwUser, WM_USER, 0, 0);
			gdwCurTime = dwTime;
		}
		if ( gbFuckThis == 2 )
		{
			timeKillEvent(guTimerID);
			ExitThread(0);
		}
		gbProcessFrame = 0;
	}
}

LRESULT __stdcall MenuProc(HWND hWnd, UINT Msg, WPARAM wParam, LPARAM lParam)
{
	LRESULT new_proc; // eax

	switch ( gbProcType )
	{
		case 0:
		//	new_proc = PROC0_GM_Logo(hWnd, Msg, wParam, lParam);
		//	break;
		case 1:
			new_proc = GM_Title(hWnd, Msg, wParam, lParam);
			break;
		case 2:
			new_proc = PROC2_GM_ClassMenu(hWnd, Msg, wParam, lParam);
			break;
		case 7:
			//new_proc = PROC3_GM_Game(hWnd, Msg, wParam, lParam);
			//break;
		case 12:
			//new_proc = PROC12_GM_Intro(hWnd, Msg, wParam, lParam);
			//break;
		case 13:
			//new_proc = PROC13_GM_Exit(hWnd, Msg, wParam, lParam);
			//break;
		case 14:
			//new_proc = PROC14_GM_Progress(hWnd, Msg, wParam, lParam);
			//break;
		default:
			new_proc = DefWindowProcA(hWnd, Msg, wParam, lParam);
			break;
	}
	return new_proc;
}

int __stdcall ui_add_hero_infos(_uiheroinfo *a1)
{
	memcpy(&heroarray[fuckcount], a1, sizeof(_uiheroinfo));
	fuckcount++;
	return 0;
}

void __cdecl mainmenu_loop()
{
	printf("MainMenu_Loop? \n\n");
	WNDPROC saveProc; // edi
	MSG Msg; // [esp+8h] [ebp-24h]

	gbFuckThis = 1;

	sgbMenuOption = 0;
	//lpDDPalette->SetEntries(0, 0, 256, orig_palette);


	//timer
	new_timer_2_int = 0;
	gbProcessFrame = 0;

	diablo_procwait_4882F9(0);
	timeGetDevCaps(&ptc, 8u);
	uPeriod = ptc.wPeriodMin;
	uDelay = ptc.wPeriodMin;
	if ( ptc.wPeriodMin <= 5 )
		uDelay = 5;

	timeBeginPeriod(uPeriod);
	gdwCurTime = timeGetTime();
	guTimerID = timeSetEvent(uDelay, uPeriod, TimeProc, (long)ghMainWnd, TIME_PERIODIC|TIME_CALLBACK_FUNCTION);


	//hero
	pfile_ui_set_hero_infos(ui_add_hero_infos);

	ClearScreenBuffer();
	//InitCursor();
	//InitLevelCursor();
	interfac_init_title_play_music();

	//main proc
	saveProc = SetWindowProc(MenuProc);

	PaletteFadeIn(8);
	drawpanflag = 255;
	mainmenu_refresh_music();

	//pfile_ui_set_hero_infos(ui_add_hero_infos);

	while( 1 )
	{

		if( PeekMessage( &Msg, NULL, 0, 0, PM_NOREMOVE ) )
		{
			if( !GetMessage( &Msg, NULL, 0, 0 ) )
			{
				break;
			}
			TranslateMessage(&Msg); 
			DispatchMessage(&Msg);
		}
		if(gbStopGame) {
			PostMessage(ghMainWnd, WM_CLOSE, 0, 0);
			break;
		}

		Sleep(1);
	}

	PaletteFadeOut(8);
	timeEndPeriod(uPeriod);
	gbFuckThis = 2;
	SetWindowProc(saveProc);
	music_stop();
	if(gbStopGame == 1)
		free_title_shit();
	init_cleanup(0);
}

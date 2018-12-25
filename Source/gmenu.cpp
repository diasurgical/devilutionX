//HEADER_GOES_HERE

#include "../types.h"
#include "miniwin_sdl.h"

#ifndef NO_GLOBALS
void *optbar_cel;
bool byte_634464; // weak
void *PentSpin_cel;
TMenuItem *sgpCurrItem;

int dword_634474; // weak
char byte_634478; // weak
void(__cdecl *dword_63447C)();
TMenuItem *dword_634480; // idb
void *option_cel;
void *sgpLogo;
int dword_63448C; // weak

void *pPentSmall;
void *pTitlqtxtCel;
void *pDiabfrCel;
void *BigTGold_cel;

//int gb_Lfont_pix_width;
//int gb_Lfont_str_len;

int gdwLogoWidth;
int gdwLogoHeight;
void *pPcxLogoImage;
void *pTitlgrayCel_sgpBackCel;

int gdwTitleWidth;
int gdwTitleHeight;
void *pPcxTitleImage;

int gdwCursorWidth;
int gdwCursorHeight;
void *pPcxCursorImage;

int gdwHeroHeight;
int gdwHeroWidth;
void *pPcxHeroImage;

int gdwSHeroHeight;
int gdwSHeroWidth;
void *pPcxSHeroImage;

void *pPcxGameImage;


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


void *GameTitle;
int GameTitleHeight;
int GameTitleWidth;

void * TitleMenuText; 
void * MenuPentegram;
#endif

const unsigned char lfontframe[127] =
	{
		0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
		0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
		0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
		0, 0, 0, 37, 49, 38, 0, 39, 40, 47,
		42, 43, 41, 45, 52, 44, 53, 55, 36, 27,
		28, 29, 30, 31, 32, 33, 34, 35, 51, 50,
		0, 46, 0, 54, 0, 1, 2, 3, 4, 5,
		6, 7, 8, 9, 10, 11, 12, 13, 14, 15,
		16, 17, 18, 19, 20, 21, 22, 23, 24, 25,
		26, 42, 0, 43, 0, 0, 0, 1, 2, 3,
		4, 5, 6, 7, 8, 9, 10, 11, 12, 13,
		14, 15, 16, 17, 18, 19, 20, 21, 22, 23,
		24, 25, 26, 20, 0, 21, 0};
const unsigned char lfontkern[56] =
	{
		18, 33, 21, 26, 28, 19, 19, 26, 25, 11,
		12, 25, 19, 34, 28, 32, 20, 32, 28, 20,
		28, 36, 35, 46, 33, 33, 24, 11, 23, 22,
		22, 21, 22, 21, 21, 21, 32, 10, 20, 36,
		31, 17, 13, 12, 13, 18, 16, 11, 20, 21,
		11, 10, 12, 11, 21, 23};

void __cdecl gmenu_draw_pause()
{
	if (currlevel)
		RedBack();
	if (!dword_634480)
	{
		light_table_index = 0;
		gmenu_print_text(316, 336, "Pause");
	}
}
// 69BEF8: using guessed type int light_table_index;


PALETTEENTRY pcxPal[256];

void __fastcall LoadPalInMem(PALETTEENTRY *pPal)
{
	int i; // eax

	for (i = 0; i < 256; i++)
	{
		orig_palette[i].peFlags = 0;
		orig_palette[i].peRed = pPal[i].peRed;
		orig_palette[i].peGreen = pPal[i].peGreen;
		orig_palette[i].peBlue = pPal[i].peBlue;
	}
}

BOOL __cdecl LoadArtImage(char *pszFile, void **pBuffer, int frames, DWORD *data)
{
	DWORD width;  // [esp+44h] [ebp-8h]
	DWORD height; // [esp+48h] [ebp-4h] MAPDST

	*pBuffer = NULL;

	if (!SBmpLoadImage(pszFile, 0, 0, 0, &width, &height, 0))
		return 0;
	*pBuffer = SMemAlloc(height * width, "U:\\DiabloUI\\Ui\\local.cpp", 88);
	if (!SBmpLoadImage(pszFile, NULL, *pBuffer, height * width, 0, 0, 0))
		return 0;
	if (pBuffer && data)
	{
		data[0] = width;
		data[1] = height / frames;
	}
	return 1;
}

BOOL __cdecl LoadArtWithPal(char *pszFile, void **pBuffer, int frames, DWORD *data)
{
	DWORD width;  // [esp+44h] [ebp-8h]
	DWORD height; // [esp+48h] [ebp-4h] MAPDST

	*pBuffer = NULL;

	if (!SBmpLoadImage(pszFile, 0, 0, 0, &width, &height, 0))
		return 0;
	*pBuffer = SMemAlloc(height * width, "U:\\DiabloUI\\Ui\\local.cpp", 88);
	if (!SBmpLoadImage(pszFile, pcxPal, *pBuffer, height * width, 0, 0, 0))
		return 0;

	LoadPalInMem(pcxPal);
	



	//	lpDDPalette->SetEntries(0, 0, 256, orig_palette);

	if (pBuffer && data)
	{
		data[0] = width;
		data[1] = height / frames;
	}
	return 1;
}

BOOL __stdcall SBmpLoadImage(const char *pszFileName, PALETTEENTRY *pPalette, void *pBuffer, DWORD dwBuffersize, DWORD *pdwWidth, DWORD *dwHeight, DWORD *pdwBpp)
{
	char *v7;					   // ebx
	unsigned char *v8;			   // edi
	PALETTEENTRY *v9;			   // esi
	int v10;					   // esi
	signed int v11;				   // ebx
	int v12;					   // ebp
	size_t v13;					   // ebp
	unsigned char *v14;			   // eax
	unsigned char *v15;			   // edx
	int v16;					   // ebp
	unsigned char v17;			   // cl
	unsigned char v18;			   // al
	int v19;					   // ecx
	bool v20;					   // zf
	bool v21;					   // sf
	unsigned char(*v22)[3];		   // eax
	BYTE v23;					   // cl
	unsigned char *Memory;		   // [esp+14h] [ebp-38Ch]
	HANDLE hFile;				   // [esp+18h] [ebp-388h] MAPDST
	DWORD v28;					   // [esp+1Ch] [ebp-384h]
	PCXHeader pcxhdr;			   // [esp+20h] [ebp-380h]
	unsigned char paldata[256][3]; // [esp+A0h] [ebp-300h]
	int z;

	if (pdwWidth)
		*pdwWidth = 0;
	if (dwHeight)
		*dwHeight = 0;
	if (pdwBpp)
		*pdwBpp = 0;
	v7 = (char *)pszFileName;
	if (pszFileName)
	{
		if (*pszFileName)
		{
			v8 = (unsigned char *)pBuffer;
			if (!pBuffer || dwBuffersize)
			{
				v9 = pPalette;
				if (pPalette || pBuffer || pdwWidth || dwHeight)
				{
					if (SFileOpenFile(pszFileName, &hFile))
					{
						if (strchr(pszFileName, 92))
						{
							do
								v7 = strchr(v7, 92) + 1;
							while (strchr(v7, 92));
						}
						for (; strchr(v7 + 1, 46); v7 = strchr(v7, 46))
							;
						if (!v7 || _strcmpi(v7, ".pcx")) // omit all types except PCX
						{
							return 0;
							//v10 = sub_15001C70(hFile, pPalette, pBuffer, dwBuffersize, pdwWidth, dwHeight, pdwBpp);
							//goto LABEL_51;
						}
						if (!SFileReadFile(hFile, &pcxhdr, 128u, 0, 0))
						{
							v10 = 0;
						LABEL_51:
							SFileCloseFile(hFile);
							return v10;
						}
						*(_DWORD *)&paldata[0][0] = pcxhdr.xmax - pcxhdr.xmin + 1;
						v11 = pcxhdr.ymax - pcxhdr.ymin + 1;
						v28 = pcxhdr.bitsPerPixel;
						if (pdwWidth)
							*pdwWidth = *(_DWORD *)&paldata[0][0];
						if (dwHeight)
							*dwHeight = v11;
						if (pdwBpp)
							*pdwBpp = v28;
						if (!pBuffer)
						{
							SFileSetFilePointer(hFile, 0, 0, 2);
							goto LABEL_45;
						}
						v12 = SFileGetFileSize(hFile, 0);
						v13 = v12 - SFileSetFilePointer(hFile, 0, 0, 1);
						v14 = (unsigned char *)SMemAlloc(v13, "SBMP.CPP", 171);
						Memory = v14;
						if (!v14)
						{
						LABEL_45:
							if (pPalette && v28 == 8)
							{
								SFileSetFilePointer(hFile, -768, 0, 1);
								SFileReadFile(hFile, paldata, 768u, 0, 0);
								v22 = paldata;
								for (z = 0; z < 256; z++)
								{
									v23 = *(_BYTE *)v22;
									++v9;
									++v22;
									v9[-1].peRed = v23;
									v9[-1].peGreen = (*v22)[-2];
									v9[-1].peBlue = (*v22)[-1];
									v9[-1].peFlags = 0;
								}
							}
							v10 = 1;
							goto LABEL_51;
						}
						SFileReadFile(hFile, v14, v13, 0, 0);
						v15 = Memory;
						if (v11 <= 0)
							goto LABEL_43;
					LABEL_33:
						v16 = *(_DWORD *)&paldata[0][0];
						while (1)
						{
							v17 = *v15++;
							if (v17 < 0xC0u)
							{
								*v8++ = v17;
								--v16;
							}
							else
							{
								v18 = *v15;
								v19 = v17 & 0x3F;
								++v15;
								for (; v19; --v16)
								{
									v20 = v16 == 0;
									v21 = v16 < 0;
									if (!v16)
										goto LABEL_41;
									*v8++ = v18;
									--v19;
								}
							}
							v20 = v16 == 0;
							v21 = v16 < 0;
						LABEL_41:
							if (v21 || v20)
							{
								if (!--v11)
								{
								LABEL_43:
									SMemFree(Memory, "SBMP.CPP", 178);
									goto LABEL_45;
								}
								goto LABEL_33;
							}
						}
					}
				}
			}
		}
	}
	return 0;
}

void __fastcall gmenu_print_text(int x, int y, char *pszStr)
{
	char *v3;		  // edi
	int v4;			  // ebp
	int v5;			  // esi
	unsigned char i;  // al
	unsigned char v7; // bl

	//printf("X:%d Y: %d \n", x, y );
	v3 = pszStr;
	v4 = y;
	v5 = x;


	// for(int z = 0; z < 30; z++){
	// 	char * A = TitleMenuText;
	// 	printf("BIG TEXT %c\n", A[z]);
	// }

	for (i = *pszStr; *v3; i = *v3)
	{
		++v3;
		v7 = lfontframe[fontidx[i]];
		if (v7)
			if(gbRunGame == 1){
				CelDecodeLightOnly(v5, v4, (char *)BigTGold_cel, v7, 46);
			}
			else{
				CelDecodeLightOnly(v5, v4, (char *)TitleMenuText, v7, 46);
			}
			
		v5 += lfontkern[v7] + 2;
	}
}

void __cdecl FreeGMenu()
{
	void *v0; // ecx
	void *v1; // ecx
	void *v2; // ecx
	void *v3; // ecx
	void *v4; // ecx

	v0 = sgpLogo;
	sgpLogo = 0;
	mem_free_dbg(v0);
	v1 = BigTGold_cel;
	BigTGold_cel = 0;
	mem_free_dbg(v1);
	v2 = PentSpin_cel;
	PentSpin_cel = 0;
	mem_free_dbg(v2);
	v3 = option_cel;
	option_cel = 0;
	mem_free_dbg(v3);
	v4 = optbar_cel;
	optbar_cel = 0;
	mem_free_dbg(v4);
}

void __cdecl gmenu_init_menu()
{
	DWORD dwData[2];
	byte_634478 = 1;
	dword_634480 = 0;
	sgpCurrItem = 0;
	dword_63447C = 0;
	dword_63448C = 0;
	byte_634464 = 0;

	// LoadArtImage("ui_art\\cursor.pcx", &pPcxCursorImage, 1, dwData);
	// gdwCursorWidth = dwData[0];
	// gdwCursorHeight = dwData[1];
	// printf("%d\n",gdwCursorWidth);

	

	LoadArtWithPal("ui_art\\mainmenu.pcx", &pPcxTitleImage, 1, dwData);
	gdwTitleWidth = dwData[0];
	gdwTitleHeight = dwData[1];

	LoadArtImage("ui_art\\smlogo.pcx", &pPcxLogoImage, 15, dwData);
	gdwLogoWidth = dwData[0];
	gdwLogoHeight = dwData[1];

	LoadArtImage("ui_art\\heros.pcx", &pPcxHeroImage, 4, dwData);
	gdwHeroWidth = dwData[0];
	gdwHeroHeight = dwData[1];

	LoadArtImage("ui_art\\selhero.pcx", &pPcxSHeroImage, 1, dwData);
	gdwSHeroWidth = dwData[0];
	gdwSHeroHeight = dwData[1];

	LoadArtImage("ui_art\\selgame.pcx", &pPcxGameImage, 1, dwData);
	gdwSHeroWidth = dwData[0];
	gdwSHeroHeight = dwData[1];

	LoadArtWithPal("ui_art\\credits.pcx", &pPcxCreditsImage, 1, dwData);
	gdwCreditsWidth = dwData[0];
	gdwCreditsHeight = dwData[1];

	LoadArtImage("ui_art\\font42g.pcx", &pPcxFontImage, 256, dwData);
	gdwFontWidth = dwData[0];
	gdwFontHeight = dwData[1];
	pFont = LoadFileInMem("ui_art\\font42.bin", 0);

	LoadArtImage("ui_art\\font16g.pcx", &pPcxFont2Image, 256, dwData);
	gdwFont2Width = dwData[0];
	gdwFont2Height = dwData[1];
	pFont2 = LoadFileInMem("ui_art\\font16.bin", 0);

	LoadArtImage("ui_art\\font16s.pcx", &pPcxFont3Image, 256, dwData);
	gdwFont3Width = dwData[0];
	gdwFont3Height = dwData[1];
	pFont3 = LoadFileInMem("ui_art\\font16.bin", 0);


	MenuPentegram = LoadFileInMem("Gendata\\Pentitle.CEL",0);
	pTitlgrayCel_sgpBackCel = LoadFileInMem("Gendata\\Titlgray.CEL", 0);
	TitleMenuText = LoadFileInMem("Gendata\\Titltext.CEL",0);
	GameTitle = LoadFileInMem("Gendata\\Diabfr.CEL",0);
	sgpLogo = LoadFileInMem("Data\\Diabsmal.CEL", 0);
	BigTGold_cel = LoadFileInMem("Data\\BigTGold.CEL", 0);
	PentSpin_cel = LoadFileInMem("Data\\PentSpin.CEL", 0);
	option_cel = LoadFileInMem("Data\\option.CEL", 0);
	optbar_cel = LoadFileInMem("Data\\optbar.CEL", 0);
	pPentSmall = LoadFileInMem("Data\\PentSpn2.CEL", 0);
	pPanelText = LoadFileInMem("CtrlPan\\SmalText.CEL", 0);
	pSTextBoxCels = LoadFileInMem("Data\\TextBox2.CEL", 0);
	pMedTextCels = LoadFileInMem("Gendata\\Titlqtxt.CEL", 0);
	pDiabfrCel = LoadFileInMem("Data\\Diabsmal.CEL", 0);
}
// 634464: using guessed type char byte_634464;
// 634478: using guessed type char byte_634478;
// 63448C: using guessed type int dword_63448C;

bool __cdecl gmenu_exception()
{
	return dword_634480 != 0;
}

void __fastcall gmenu_call_proc(TMenuItem *pItem, void(__cdecl *gmFunc)())
{
	TMenuItem *v2;		   // eax
	int v3;				   // ecx
	void(__cdecl * *v4)(); // edx

	PauseMode = 0;
	byte_634464 = 0;
	v2 = pItem;
	dword_63447C = gmFunc;
	dword_634480 = pItem;
	if (gmFunc)
	{
		gmFunc();
		v2 = dword_634480;
	}
	v3 = 0;
	dword_63448C = 0;
	if (v2)
	{
		v4 = &v2->fnMenu;
		while (*v4)
		{
			++v3;
			v4 += 3;
			dword_63448C = v3;
		}
	}
	sgpCurrItem = &v2[v3 - 1];
	gmenu_up_down(1);
}
// 525740: using guessed type int PauseMode;
// 634464: using guessed type char byte_634464;
// 63448C: using guessed type int dword_63448C;

void __fastcall gmenu_up_down(int a1)
{
	TMenuItem *v1; // eax
	int v2;		   // edi

	v1 = sgpCurrItem;
	if (sgpCurrItem)
	{
		byte_634464 = 0;
		v2 = dword_63448C;
		while (v2)
		{
			--v2;
			if (a1)
			{
				++v1;
				sgpCurrItem = v1;
				if (v1->fnMenu)
					goto LABEL_10;
				v1 = dword_634480;
			}
			else
			{
				if (v1 == dword_634480)
					v1 = &dword_634480[dword_63448C];
				--v1;
			}
			sgpCurrItem = v1;
		LABEL_10:
			if ((v1->dwFlags & 0x80000000) != 0)
			{
				if (v2)
					PlaySFX(IS_TITLEMOV);
				return;
			}
		}
	}
}
// 634464: using guessed type char byte_634464;
// 63448C: using guessed type int dword_63448C;

void __cdecl gmenu_draw()
{
	int v0;		  // edi
	TMenuItem *i; // esi
	DWORD v2;	 // eax

	if (dword_634480)
	{
		if (dword_63447C)
			dword_63447C();
		//	CelDecodeOnly(236, 262, sgpLogo, 1, 296);
		v0 = 320;
		for (i = dword_634480; i->fnMenu; v0 += 45)
		{
			gmenu_draw_menu_item(i, v0);
			++i;
		}
		v2 = GetTickCount();
		if ((signed int)(v2 - dword_634474) > 25)
		{
			if (++byte_634478 == 9)
				byte_634478 = 1;
			dword_634474 = v2;
		}
	}
}
// 634474: using guessed type int dword_634474;
// 634478: using guessed type char byte_634478;

void __fastcall gmenu_draw_menu_item(TMenuItem *pItem, int a2)
{
	int v2;			  // edi
	TMenuItem *v3;	// ebx
	unsigned int v4;  // eax
	unsigned int v5;  // ebp
	int v6;			  // esi
	unsigned int v7;  // ecx
	unsigned int v8;  // eax
	int v9;			  // ecx
	unsigned int v10; // ebp
	int v11;		  // esi
	int v12;		  // eax
	int v13;		  // edi
	unsigned int v14; // [esp+10h] [ebp-4h]

	int diablogo_cel_frame = 1;

	int MyPcxDelay = 60;
	int MyPcxFRAME = (SDL_GetTicks() / MyPcxDelay) % 15;
	if (++MyPcxFRAME == 15)
	{
		MyPcxFRAME = 1;
	}

	//DrawArtWithMask(320 - (gdwLogoWidth / 2), -50, gdwLogoWidth, gdwLogoHeight, MyPcxFRAME, 250, pPcxLogoImage);
	CelDecodeOnly(64, 275, GameTitle, MyPcxFRAME, 640);


	if (++diablogo_cel_frame == 29)
	{
		diablogo_cel_frame = 1;
	}

	v2 = a2;
	v3 = pItem;
	v4 = gmenu_get_lfont(pItem);
	v5 = v4;
	v14 = v4;
	if (v3->dwFlags & 0x40000000)
	{
		v6 = (v4 >> 1) + 80;
		CelDecodeOnly(v6, v2 - 10, optbar_cel, 1, 287);
		v7 = (v3->dwFlags >> 12) & 0xFFF;
		if (v7 < 2)
			v7 = 2;
		v8 = ((v3->dwFlags & 0xFFF) << 8) / v7;
		v9 = (v5 >> 1) + 82;
		v10 = v8;
		gmenu_clear_buffer(v9, v2 - 12, v8 + 13, 28);
		CelDecodeOnly(v6 + v10 + 2, v2 - 12, option_cel, 1, 27);
		v5 = v14;
	}
	v11 = 384 - (v5 >> 1);
	v12 = -((v3->dwFlags & 0x80000000) != 0);
	_LOBYTE(v12) = v12 & 0xF1;
	light_table_index = v12 + 15;
	gmenu_print_text(384 - (v5 >> 1), v2, v3->pszStr);
	if (v3 == sgpCurrItem)
	{
		v13 = v2 + 1;

		CelDecodeOnly(v11 - 54, v13, PentSpin_cel, (unsigned char)byte_634478, 48);
		CelDecodeOnly(v11 + v5 + 4, v13, PentSpin_cel, (unsigned char)byte_634478, 48);
	}
}
// 634478: using guessed type char byte_634478;
// 69BEF8: using guessed type int light_table_index;

void __fastcall gmenu_clear_buffer(int x, int y, int width, int height)
{
	int v4;  // edi
	char *i; // esi

	v4 = height;
	for (i = (char *)gpBuffer + screen_y_times_768[y] + x; v4; --v4)
	{
		memset(i, 205, width);
		i -= 768;
	}
}

int __fastcall gmenu_get_lfont(TMenuItem *pItem)
{
	char *v2;		  // eax
	int i;			  // edx
	unsigned char v4; // cl

	if (pItem->dwFlags & 0x40000000)
		return 490;
	v2 = pItem->pszStr;
	for (i = 0;; i += lfontkern[lfontframe[fontidx[v4]]] + 2)
	{
		v4 = *v2;
		if (!*v2)
			break;
		++v2;
	}
	return i - 2;
}

int __fastcall gmenu_presskeys(int a1)
{
	int v1; // ecx
	int v2; // ecx

	if (!dword_634480)
		return 0;
	switch (a1)
	{
	case VK_RETURN:
		if ((sgpCurrItem->dwFlags & 0x80000000) != 0)
		{
			PlaySFX(IS_TITLEMOV);
			((void(__fastcall *)(signed int))sgpCurrItem->fnMenu)(1);
		}
		return 1;
	case VK_ESCAPE:
		PlaySFX(IS_TITLEMOV);
		gmenu_call_proc(0, 0);
		return 1;
	case VK_SPACE:
		return 0;
	case VK_LEFT:
		v2 = 0;
		goto LABEL_12;
	case VK_UP:
		v1 = 0;
		goto LABEL_10;
	case VK_RIGHT:
		v2 = 1;
	LABEL_12:
		gmenu_left_right(v2);
		return 1;
	case VK_DOWN:
		v1 = 1;
	LABEL_10:
		gmenu_up_down(v1);
		break;
	}
	return 1;
}

void __fastcall gmenu_left_right(int a1)
{
	signed int v1;   // edx
	unsigned int v2; // eax
	int v3;			 // eax

	v1 = sgpCurrItem->dwFlags;
	if (sgpCurrItem->dwFlags & 0x40000000)
	{
		v2 = sgpCurrItem->dwFlags & 0xFFF;
		if (a1)
		{
			if (v2 == ((v1 >> 12) & 0xFFF))
				return;
			v3 = v2 + 1;
		}
		else
		{
			if (!(v1 & 0xFFF))
				return;
			v3 = v2 - 1;
		}
		_LOWORD(v1) = v1 & 0xF000;
		sgpCurrItem->dwFlags = v1;
		sgpCurrItem->dwFlags |= v3;
		((void(__fastcall *)(_DWORD))sgpCurrItem->fnMenu)(0);
	}
}

int __fastcall gmenu_on_mouse_move(LPARAM lParam)
{
	int v2; // edx
	int a1; // [esp+0h] [ebp-4h]

	a1 = lParam;
	if (!byte_634464)
		return 0;
	gmenu_valid_mouse_pos(&a1);
	v2 = a1 * ((sgpCurrItem->dwFlags >> 12) & 0xFFF) % 256;
	a1 = a1 * ((sgpCurrItem->dwFlags >> 12) & 0xFFF) / 256;
	_LOWORD(sgpCurrItem->dwFlags) &= 0xF000u;
	sgpCurrItem->dwFlags |= a1;
	((void(__fastcall *)(_DWORD, int))sgpCurrItem->fnMenu)(0, v2);
	return 1;
}
// 41A37A: could not find valid save-restore pair for esi
// 634464: using guessed type char byte_634464;

bool __fastcall gmenu_valid_mouse_pos(int *plOffset)
{
	*plOffset = 282;
	if (MouseX < 282)
	{
		*plOffset = 0;
		return 0;
	}
	if (MouseX > 538)
	{
		*plOffset = 256;
		return 0;
	}
	*plOffset = MouseX - 282;
	return 1;
}

int __fastcall gmenu_left_mouse(int a1)
{
	int result;		 // eax
	unsigned int v2; // eax
	unsigned int v3; // eax
	TMenuItem *v4;   // esi
	unsigned int v5; // eax
	//LPARAM v6; // ecx
	int a1a; // [esp+4h] [ebp-4h]

	if (a1)
	{
		if (!dword_634480 || MouseY >= 352)
			return 0;
		if (MouseY - 117 >= 0)
		{
			v2 = (MouseY - 117) / 45;
			if (v2 < dword_63448C)
			{
				v3 = v2;
				v4 = &dword_634480[v3];
				if ((v4->dwFlags & 0x80000000) != 0)
				{
					v5 = (unsigned int)gmenu_get_lfont(&dword_634480[v3]) >> 1;
					if (MouseX >= 320 - v5 && MouseX <= v5 + 320)
					{
						sgpCurrItem = v4;
						PlaySFX(IS_TITLEMOV);
						if (v4->dwFlags & 0x40000000)
						{
							byte_634464 = gmenu_valid_mouse_pos(&a1a);
							gmenu_on_mouse_move(a1); /* v6 */
						}
						else
						{
							((void(__fastcall *)(signed int))sgpCurrItem->fnMenu)(1);
						}
					}
				}
			}
		}
	}
	else
	{
		result = 0;
		if (!byte_634464)
			return result;
		byte_634464 = 0;
	}
	return 1;
}
// 634464: using guessed type char byte_634464;
// 63448C: using guessed type int dword_63448C;

void __fastcall gmenu_enable(TMenuItem *pMenuItem, BOOL enable)
{
	if (enable)
		pMenuItem->dwFlags |= 0x80000000;
	else
		pMenuItem->dwFlags &= 0x7F000000;
}

void __fastcall gmenu_slider_1(TMenuItem *pItem, int min, int max, int gamma)
{
	unsigned int v4; // esi
	int v5;			 // eax

	v4 = pItem->dwFlags;
	v5 = (pItem->dwFlags >> 12) & 0xFFF;
	if (v5 < 2)
		v5 = 2;
	_LOWORD(v4) = v4 & 0xF000;
	pItem->dwFlags = v4 | (v5 * (gamma - min) + (max - min - 1) / 2) / (max - min);
}

int __fastcall gmenu_slider_get(TMenuItem *pItem, int min, int max)
{
	int v3;			 // eax
	unsigned int v4; // ecx

	v3 = (pItem->dwFlags >> 12) & 0xFFF;
	v4 = pItem->dwFlags & 0xFFF;
	if (v3 < 2)
		v3 = 2;
	return min + (v4 * (max - min) + (v3 - 1) / 2) / v3;
}

void __fastcall gmenu_slider_3(TMenuItem *pItem, int dwTicks)
{
	pItem->dwFlags ^= (pItem->dwFlags ^ (dwTicks << 12)) & 0xFFF000;
}

// ref: 0x10001058
void __fastcall artfont_SetArtFont(int nFont)
{
	switch ( nFont )
	{
		case 0:
			sgpCurrFont = &font16g;
			break;
		case 2:
			sgpCurrFont = &font24g;
			break;
		case 3:
			sgpCurrFont = &font24s;
			break;
		case 4:
			sgpCurrFont = &font30g;
			break;
		case 5:
			sgpCurrFont = &font30s;
			break;
		case 6:
			sgpCurrFont = &font42g;
			break;
		case 7:
			sgpCurrFont = &font42y;
			break;
		default:
			sgpCurrFont = &font16s;
			break;
	}
}

// ref: 0x10001098
void __cdecl artfont_InitAllFonts()
{
	font42g.active = 0;
	font42y.active = 0;
	font30g.active = 0;
	font30s.active = 0;
	font24g.active = 0;
	font24s.active = 0;
	font16g.active = 0;
	font16s.active = 0;
	sgpCurrFont = 0;
}

// ref: 0x100010C8
void __cdecl artfont_FreeAllFonts()
{
	artfont_FreeArtFont(&font42g);
	artfont_FreeArtFont(&font42y);
	artfont_FreeArtFont(&font30g);
	artfont_FreeArtFont(&font30s);
	artfont_FreeArtFont(&font24g);
	artfont_FreeArtFont(&font24s);
	artfont_FreeArtFont(&font16g);
	artfont_FreeArtFont(&font16s);
	sgpCurrFont = 0;
}

// ref: 0x10001120
void __fastcall artfont_FreeArtFont(FontStruct *pFont)
{
	HANDLE *v2; // esi
	signed int v3; // ebx

	if ( pFont->active )
	{
		v2 = pFont->fonttrans;
		v3 = 256;
		do
		{
			if ( *v2 )
			{
				STransDelete(*v2);
				*v2 = 0;
			}
			++v2;
			--v3;
		}
		while ( v3 );
		pFont->active = 0;
	}
}

// ref: 0x10001159
BOOL __cdecl artfont_LoadAllFonts()
{
	artfont_LoadArtFont(&font30g, "ui_art\\font30.bin", "ui_art\\font30g.pcx");
	artfont_LoadArtFont(&font30s, "ui_art\\font30.bin", "ui_art\\font30s.pcx");
	artfont_LoadArtFont(&font24g, "ui_art\\font24.bin", "ui_art\\font24g.pcx");
	artfont_LoadArtFont(&font24s, "ui_art\\font24.bin", "ui_art\\font24s.pcx");
	artfont_LoadArtFont(&font16g, "ui_art\\font16.bin", "ui_art\\font16g.pcx");
	artfont_LoadArtFont(&font16s, "ui_art\\font16.bin", "ui_art\\font16s.pcx");
	artfont_LoadArtFont(&font42g, "ui_art\\font42.bin", "ui_art\\font42g.pcx");
	artfont_LoadArtFont(&font42y, "ui_art\\font42.bin", "ui_art\\font42y.pcx");
	return 1;
}

// ref: 0x100011FB
void __fastcall artfont_LoadArtFont(FontStruct *pFont, const char *pszBinFile, const char *pszFileName)
{
	LONG v4; // eax
	signed int v5; // edi
	unsigned char v6; // al
	int v7; // ecx
	int a5[4]; // [esp+8h] [ebp-20h]
	DWORD size[2]; // [esp+18h] [ebp-10h]
	BYTE *pBuffer; // [esp+20h] [ebp-8h]
	HANDLE phFile; // [esp+24h] [ebp-4h]
	HANDLE *a1a; // [esp+30h] [ebp+8h]

	if ( !pFont->active && SFileOpenFile(pszBinFile, &phFile) )
	{
		v4 = SFileGetFileSize(phFile, 0);
		if ( SFileReadFile(phFile, pFont, v4, 0, 0) )
		{
			SFileCloseFile(phFile);
			local_LoadArtImage(pszFileName, &pBuffer, size);
			memset(pFont->fonttrans, 0, 0x400u);
			if ( pBuffer )
			{
				v5 = 0;
				a1a = pFont->fonttrans;
				do
				{
					v6 = pFont->fontbin[v5 + 2];
					if ( v6 )
					{
						v7 = pFont->fontbin[1];
						a5[2] = v6;
						a5[1] = v5 * v7;
						a5[0] = 0;
						a5[3] = v7 + v5 * v7 - 1;
						STransCreateI(pBuffer, size[0], size[1], 8, (int)a5, 16777248, a1a);
					}
					++a1a;
					++v5;
				}
				while ( v5 <= 256 );
				pFont->active = 1;
				SMemFree(pBuffer, "C:\\Src\\Diablo\\DiabloUI\\artfont.cpp", 206, 0);
			}
		}
		else
		{
			SFileCloseFile(phFile);
		}
	}
}

// ref: 0x100012F6
int __cdecl artfont_GetFontMaxHeight()
{
	int result; // eax

	if ( sgpCurrFont && sgpCurrFont->active )
		result = sgpCurrFont->fontbin[1];
	else
		result = 0;
	return result;
}

// ref: 0x10001310
int __cdecl artfont_GetFontDefWidth()
{
	int result; // eax

	if ( sgpCurrFont && sgpCurrFont->active )
		result = sgpCurrFont->fontbin[0];
	else
		result = 0;
	return result;
}

// ref: 0x10001329
int __fastcall artfont_GetFontWidth(char *str)
{
	int result; // eax
	unsigned char i; // bl
	unsigned char v3; // bl
	int v4; // esi

	result = 0;
	if ( !sgpCurrFont || !sgpCurrFont->active )
		return 0;
	for ( i = *str; *str; i = *str )
	{
		v3 = sgpCurrFont->fontbin[i + 2];
		if ( v3 )
			v4 = v3;
		else
			v4 = sgpCurrFont->fontbin[0];
		result += v4;
		++str;
	}
	return result;
}

// ref: 0x1000136C
void __cdecl artfont_cpp_init()
{
	artfont_cpp_float = 2139095040;
}
// 10026BB0: using guessed type int artfont_cpp_float;

// ref: 0x10001377
int __fastcall artfont_GetFontBreak(char *str)
{
	int result; // eax
	unsigned char v2; // dl
	unsigned char v3; // dl

	result = 0;
	if ( !sgpCurrFont || !sgpCurrFont->active )
		return 0;
	while ( 1 )
	{
		v3 = *str;
		if ( !*str )
			break;
		if ( v3 == '\n' )
			break;
		if ( v3 == ' ' )
			break;
		v2 = sgpCurrFont->fontbin[v3 + 2];
		if ( !v2 )
			break;
		result += v2;
		++str;
	}
	return result;
}

// ref: 0x100013B3
void __cdecl artfont_delete_operator(void *ptr)
{
	if ( ptr )
		SMemFree(ptr, "delete", -1, 0);
}

// ref: 0x100013CD
void __fastcall artfont_PrintFontStr(char *str, DWORD **pSurface, int sx, int sy)
{
	FontStruct *v5; // esi
	unsigned char v6; // cl
	int v7; // edi
	unsigned char v8; // dl
	int v9; // edi
	DWORD *v10; // ecx
	HANDLE hTrans; // [esp+Ch] [ebp-8h]
	HANDLE hTransa; // [esp+Ch] [ebp-8h]

	if ( pSurface )
	{
		if ( *pSurface )
		{
			v5 = sgpCurrFont;
			if ( sgpCurrFont )
			{
				if ( sgpCurrFont->active )
				{
					if ( sx < 0 )
						sx = 0;
					if ( sy < 0 )
						sy = 0;
					v6 = *str;
					if ( *str )
					{
						while ( 1 )
						{
							hTrans = (HANDLE)(sy + v5->fontbin[1]);
							if ( sy + v5->fontbin[1] > (signed int)pSurface[2] )
								return;
							if ( v6 == '\n' )
								break;
							v7 = v6;
							v8 = v5->fontbin[v6 + 2];
							if ( !v8 )
							{
								v9 = v5->fontbin[0];
								if ( sx + v9 + artfont_GetFontBreak(++str) < (signed int)pSurface[1] )
								{
									sx += v9;
								}
								else
								{
									sx = 0;
									sy = (int)hTrans;
								}
								goto LABEL_23;
							}
							hTransa = v5->fonttrans[v6];
							if ( v5->fonttrans[v6] )
							{
								v10 = pSurface[1];
								if ( sx + v8 <= (signed int)v10 )
								{
									STransBlt(*pSurface, sx, sy, (int)v10, hTransa);
									v5 = sgpCurrFont;
									sx += sgpCurrFont->fontbin[v7 + 2];
									goto LABEL_22;
								}
								sx = 0;
								sy += v5->fontbin[1];
							}
LABEL_23:
							v6 = *str;
							if ( !*str )
								return;
						}
						sx = 0;
						sy += v5->fontbin[1];
LABEL_22:
						++str;
						goto LABEL_23;
					}
				}
			}
		}
	}
}

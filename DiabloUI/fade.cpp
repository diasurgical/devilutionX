// ref: 0x1000722B
void __fastcall Fade_ApplyPaletteRange(int range1, int range2)
{
	tagPALETTEENTRY *v4; // eax MAPDST
	BYTE *v6; // esi
	BYTE v7; // al

	v4 = local_GetArtPalEntry(0);
	if ( range1 == range2 )
	{
		memcpy(fadepal, v4, 0x400u);
	}
	else if ( range2 )
	{
		v6 = &fadepal[0].peGreen;
		do
		{
			v7 = range2 * v4->peRed / range1;
			++v4;
			*(v6 - 1) = v7;
			*v6 = range2 * v4[-1].peGreen / range1;
			v6 += 4;
			*(v6 - 3) = range2 * v4[-1].peBlue / range1;
		}
		while ( (signed int)v6 < (signed int)&fadepal[256].peGreen );
	}
	else
	{
		memcpy(fadepal, v4, 0x400u);
		local_ClearPalette(fadepal);
	}
	SDrawUpdatePalette(0, 0x100u, fadepal, 1);
}

// ref: 0x100072BE
void __fastcall Fade_UpdatePaletteRange(int range)
{
	tagPALETTEENTRY *v2; // eax
	tagPALETTEENTRY *v3; // edi
	BYTE *v4; // ecx
	BYTE v5; // al
	HPALETTE v6; // ebx
	int v7; // [esp+10h] [ebp-4h]

	v2 = local_GetArtPalEntry(0);
	memcpy(fadepal, v2, 0x400u);
	if ( range > 0 )
	{
		v7 = range;
		do
		{
			v3 = local_GetArtPalEntry(0);
			v4 = &fadepal[0].peGreen;
			do
			{
				v5 = v3->peRed / range;
				++v3;
				*(v4 - 1) -= v5;
				*v4 -= v3[-1].peGreen / range;
				v4 += 4;
				*(v4 - 3) -= v3[-1].peBlue / range;
			}
			while ( (signed int)v4 < (signed int)&fadepal[256].peGreen );
			SDrawUpdatePalette(0, 0x100u, fadepal, 1);
			--v7;
		}
		while ( v7 );
	}
	local_ClearPalette(fadepal);
	SDrawUpdatePalette(0, 0x100u, fadepal, 1);
	local_SetCursorDefault();
	SDrawClearSurface(0);
	v6 = (HPALETTE)GetStockObject(15);
	GetPaletteEntries(v6, 0, 0xAu, fadepal);
	GetPaletteEntries(v6, 0xAu, 0xAu, &fadepal[246]);
	SDrawUpdatePalette(0, 0x100u, fadepal, 1);
}
// 1001043C: using guessed type int __stdcall SDrawClearSurface(_DWORD);

// ref: 0x1000739F
BOOL __cdecl Fade_CheckRange5()
{
	BOOL result; // eax

	result = 0;
	if ( sgbIsFading )
	{
		if ( sgbFadeRange <= 5 )
			result = 1;
	}
	return result;
}
// 10029C70: using guessed type int sgbIsFading;

// ref: 0x100073B4
void __cdecl Fade_Range5SetZero()
{
	if ( Fade_CheckRange5() )
		sgbIsFading = 0;
}
// 10029C70: using guessed type int sgbIsFading;

// ref: 0x100073C5
void __fastcall Fade_NoInputAndArt(HWND hWnd, BOOL bShowCurs)
{
	HWND v3; // eax

	v3 = GetParent(hWnd);
	local_DisableKeyWaitMouse(v3);
	if ( bShowCurs )
		local_SetCursorArt();
	sgbIsFading = 0;
	sgbFadeRange = 0;
}
// 10029C70: using guessed type int sgbIsFading;

// ref: 0x100073EF
void __fastcall Fade_SetInputWindow(HWND hWnd)
{
	HWND v1; // eax

	v1 = GetParent(hWnd);
	local_DisableKeyWaitMouse(v1);
}

// ref: 0x100073FD
void __fastcall Fade_SetFadeTimer(int nTime)
{
	if ( !sgbIsFading )
	{
		SDlgSetTimer(nTime, 16, 50, Fade_TimerFunctionDlg);
		sgbIsFading = 1;
	}
}
// 10029C70: using guessed type int sgbIsFading;

// ref: 0x10007420
void __stdcall Fade_TimerFunctionDlg(int a1, int a2, int a3, int a4)
{
	if ( sgbFadeRange > 5 )
	{
		SDlgKillTimer(a1, 16);
	}
	else
	{
		Fade_ApplyPaletteRange(5, sgbFadeRange);
		++sgbFadeRange;
	}
}

// ref: 0x1000744D
void __cdecl Fade_cpp_init()
{
	fade_cpp_float = fade_cpp_float_value;
}
// 1001F428: using guessed type int fade_cpp_float_value;
// 10029868: using guessed type int fade_cpp_float;

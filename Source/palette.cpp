//HEADER_GOES_HERE

#include "../types.h"

DEVILUTION_BEGIN_NAMESPACE

PALETTEENTRY logical_palette[256];
PALETTEENTRY system_palette[256];
PALETTEENTRY orig_palette[256];
UINT gdwPalEntries;

/* data */

int gamma_correction = 100; // idb
BOOL color_cycling_enabled = TRUE;
BOOLEAN sgbFadedIn = 1;

void __cdecl SaveGamma()
{
	SRegSaveValue("Diablo", "Gamma Correction", 0, gamma_correction);
	SRegSaveValue("Diablo", "Color Cycling", FALSE, color_cycling_enabled);
}

void __cdecl palette_init()
{
	int v0; // eax
	int v1; // eax

	LoadGamma();
	memcpy(system_palette, orig_palette, 0x400u);
	LoadSysPal();
#ifdef __cplusplus
	v0 = lpDDInterface->CreatePalette(DDPCAPS_ALLOW256 | DDPCAPS_8BIT, system_palette, &lpDDPalette, NULL);
#else
	v0 = lpDDInterface->lpVtbl->CreatePalette(lpDDInterface, DDPCAPS_ALLOW256 | DDPCAPS_8BIT, system_palette, &lpDDPalette, NULL);
#endif
	if (v0)
		ErrDlg(IDD_DIALOG8, v0, "C:\\Src\\Diablo\\Source\\PALETTE.CPP", 143);
#ifdef __cplusplus
	v1 = lpDDSPrimary->SetPalette(lpDDPalette);
#else
	v1 = lpDDSPrimary->lpVtbl->SetPalette(lpDDSPrimary, lpDDPalette);
#endif
	if (v1)
		ErrDlg(IDD_DIALOG8, v1, "C:\\Src\\Diablo\\Source\\PALETTE.CPP", 146);
}

void __cdecl LoadGamma()
{
	int v3;    // eax
	int value; // [esp+8h] [ebp-4h]

	value = gamma_correction;
	if (!SRegLoadValue("Diablo", "Gamma Correction", 0, &value))
		value = 100;
	if (value >= 30) {
		if (value > 100)
			value = 100;
	} else {
		value = 30;
	}
	gamma_correction = value - value % 5;
	if (SRegLoadValue("Diablo", "Color Cycling", 0, &value))
		v3 = value;
	else
		v3 = TRUE;
	color_cycling_enabled = v3;
}

void __cdecl LoadSysPal()
{
	HDC hDC;         // ebx
	int i;           // ecx
	int iStartIndex; // edi

	for (i = 0; i < 256; i++)
		system_palette[i].peFlags = PC_NOCOLLAPSE | PC_RESERVED;

	if (!fullscreen) {
		hDC = GetDC(NULL);
		gdwPalEntries = GetDeviceCaps(hDC, NUMRESERVED) / 2;
		GetSystemPaletteEntries(hDC, 0, gdwPalEntries, system_palette);
		for (i = 0; i < gdwPalEntries; i++)
			system_palette[i].peFlags = 0;
		iStartIndex = 256 - gdwPalEntries;
		GetSystemPaletteEntries(hDC, iStartIndex, gdwPalEntries, &system_palette[iStartIndex]);
		if (iStartIndex < 256) {
			for (i = iStartIndex; i < 256; i++)
				system_palette[i].peFlags = 0;
		}
		ReleaseDC(NULL, hDC);
	}
}

void __fastcall LoadPalette(char *pszFileName)
{
	int i;
	void *pBuf;
	BYTE PalData[256][3];

	/// ASSERT: assert(pszFileName);

	WOpenFile(pszFileName, &pBuf, 0);
	WReadFile(pBuf, (char *)PalData, sizeof(PalData));
	WCloseFile(pBuf);

	for (i = 0; i < 256; i++) {
		orig_palette[i].peRed = PalData[i][0];
		orig_palette[i].peGreen = PalData[i][1];
		orig_palette[i].peBlue = PalData[i][2];
		orig_palette[i].peFlags = 0;
	}
}

void __fastcall LoadRndLvlPal(int l)
{
	char szFileName[MAX_PATH];

	if (l == DTYPE_TOWN) {
		LoadPalette("Levels\\TownData\\Town.pal");
	} else {
		sprintf(szFileName, "Levels\\L%iData\\L%i_%i.PAL", l, l, random(0, 4) + 1);
		LoadPalette(szFileName);
	}
}

void __cdecl ResetPal()
{
	if (!lpDDSPrimary
#ifdef __cplusplus
	    || lpDDSPrimary->IsLost() != DDERR_SURFACELOST
	    || !lpDDSPrimary->Restore()) {
#else
	    || lpDDSPrimary->lpVtbl->IsLost(lpDDSPrimary) != DDERR_SURFACELOST
	    || !lpDDSPrimary->lpVtbl->Restore(lpDDSPrimary)) {
#endif
		SDrawRealizePalette();
	}
}

void __cdecl IncreaseGamma()
{
	if (gamma_correction < 100) {
		gamma_correction += 5;
		if (gamma_correction > 100)
			gamma_correction = 100;
		ApplyGamma(system_palette, logical_palette, 256);
		palette_update();
	}
}

void __cdecl palette_update()
{
	int v0; // ecx
	int v1; // eax

	if (lpDDPalette) {
		v0 = 0;
		v1 = 256;
		if (!fullscreen) {
			v0 = gdwPalEntries;
			v1 = 2 * (128 - gdwPalEntries);
		}
		SDrawUpdatePalette(v0, v1, &system_palette[v0], 0);
	}
}

void __fastcall ApplyGamma(PALETTEENTRY *dst, PALETTEENTRY *src, int n)
{
	int i;
	double g;

	g = gamma_correction / 100.0;

	for (i = 0; i < n; i++) {
		dst->peRed = pow(src->peRed / 256.0, g) * 256.0;
		dst->peGreen = pow(src->peGreen / 256.0, g) * 256.0;
		dst->peBlue = pow(src->peBlue / 256.0, g) * 256.0;
		dst++;
		src++;
	}
}

void __cdecl DecreaseGamma()
{
	if (gamma_correction > 30) {
		gamma_correction -= 5;
		if (gamma_correction < 30)
			gamma_correction = 30;
		ApplyGamma(system_palette, logical_palette, 256);
		palette_update();
	}
}

int __fastcall UpdateGamma(int gamma)
{
	if (gamma) {
		gamma_correction = 130 - gamma;
		ApplyGamma(system_palette, logical_palette, 256);
		palette_update();
	}
	return 130 - gamma_correction;
}

void __cdecl BlackPalette()
{
	SetFadeLevel(0);
}

void __fastcall SetFadeLevel(int fadeval)
{
	int i; // eax

	if (lpDDInterface) {
		for (i = 0; i < 255; i++) {
			system_palette[i].peRed = (fadeval * logical_palette[i].peRed) >> 8;
			system_palette[i].peGreen = (fadeval * logical_palette[i].peGreen) >> 8;
			system_palette[i].peBlue = (fadeval * logical_palette[i].peBlue) >> 8;
		}
		Sleep(3);
#ifdef __cplusplus
		lpDDInterface->WaitForVerticalBlank(DDWAITVB_BLOCKBEGIN, NULL);
#else
		lpDDInterface->lpVtbl->WaitForVerticalBlank(lpDDInterface, DDWAITVB_BLOCKBEGIN, NULL);
#endif
		palette_update();
	}
}

void __fastcall PaletteFadeIn(int fr)
{
	int i; // ebp

	ApplyGamma(logical_palette, orig_palette, 256);

	for (i = 0; i < 256; i += fr)
		SetFadeLevel(i);

	SetFadeLevel(256);
	memcpy(logical_palette, orig_palette, 0x400u);
	sgbFadedIn = 1;
}

void __fastcall PaletteFadeOut(int fr)
{
	int i; // esi

	if (sgbFadedIn) {
		for (i = 256; i > 0; i -= fr)
			SetFadeLevel(i);

		SetFadeLevel(0);
		sgbFadedIn = 0;
	}
}

void __cdecl palette_update_caves()
{
	int i;
	PALETTEENTRY col;

	col = system_palette[1];
	for (i = 1; i < 31; i++) {
		system_palette[i].peRed = system_palette[i + 1].peRed;
		system_palette[i].peGreen = system_palette[i + 1].peGreen;
		system_palette[i].peBlue = system_palette[i + 1].peBlue;
	}
	system_palette[i].peRed = col.peRed;
	system_palette[i].peGreen = col.peGreen;
	system_palette[i].peBlue = col.peBlue;

	palette_update();
}

void __fastcall palette_update_quest_palette(int n)
{
	int i; // eax

	for (i = 32 - n; i >= 0; --i)
		logical_palette[i] = orig_palette[i];
	ApplyGamma(system_palette, logical_palette, 32);
	palette_update();
}

BOOLEAN __cdecl palette_get_colour_cycling()
{
	return color_cycling_enabled;
}

void __fastcall palette_set_color_cycling(BOOLEAN enabled)
{
	color_cycling_enabled = enabled;
}

DEVILUTION_END_NAMESPACE

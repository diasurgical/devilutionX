#include "diablo.h"
#include "../3rdParty/Storm/Source/storm.h"

DEVILUTION_BEGIN_NAMESPACE

PALETTEENTRY logical_palette[256];
PALETTEENTRY system_palette[256];
PALETTEENTRY orig_palette[256];
int gdwPalEntries;

/* data */

int gamma_correction = 100;
BOOL color_cycling_enabled = TRUE;
BOOLEAN sgbFadedIn = TRUE;

void SaveGamma()
{
	SRegSaveValue("Diablo", "Gamma Correction", 0, gamma_correction);
	SRegSaveValue("Diablo", "Color Cycling", FALSE, color_cycling_enabled);
}

void palette_init()
{
	DWORD error_code;

	LoadGamma();
	memcpy(system_palette, orig_palette, sizeof(orig_palette));
	LoadSysPal();
#ifdef __cplusplus
	error_code = lpDDInterface->CreatePalette(DDPCAPS_ALLOW256 | DDPCAPS_8BIT, system_palette, &lpDDPalette, NULL);
#else
	error_code = lpDDInterface->lpVtbl->CreatePalette(lpDDInterface, DDPCAPS_ALLOW256 | DDPCAPS_8BIT, system_palette, &lpDDPalette, NULL);
#endif
	if (error_code)
		ERR_DLG(IDD_DIALOG8, error_code);
#ifdef __cplusplus
	error_code = lpDDSPrimary->SetPalette(lpDDPalette);
#else
	error_code = lpDDSPrimary->lpVtbl->SetPalette(lpDDSPrimary, lpDDPalette);
#endif
#ifndef RGBMODE
	if (error_code)
		ERR_DLG(IDD_DIALOG8, error_code);
#endif
}

void LoadGamma()
{
	int gamma_value;
	int value;

	value = gamma_correction;
	if (!SRegLoadValue("Diablo", "Gamma Correction", 0, &value))
		value = 100;
	gamma_value = value;
	if (value < 30) {
		gamma_value = 30;
	} else if (value > 100) {
		gamma_value = 100;
	}
	gamma_correction = gamma_value - gamma_value % 5;
	if (!SRegLoadValue("Diablo", "Color Cycling", 0, &value))
		value = 1;
	color_cycling_enabled = value;
}

void LoadSysPal()
{
	HDC hDC;
	int i, iStartIndex;

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

void LoadPalette(char *pszFileName)
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

void LoadRndLvlPal(int l)
{
	char szFileName[MAX_PATH];

	if (l == DTYPE_TOWN) {
		LoadPalette("Levels\\TownData\\Town.pal");
	} else {
		sprintf(szFileName, "Levels\\L%iData\\L%i_%i.PAL", l, l, random(0, 4) + 1);
		LoadPalette(szFileName);
	}
}

void ResetPal()
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

void IncreaseGamma()
{
	if (gamma_correction < 100) {
		gamma_correction += 5;
		if (gamma_correction > 100)
			gamma_correction = 100;
		ApplyGamma(system_palette, logical_palette, 256);
		palette_update();
	}
}

void palette_update()
{
	int nentries;
	int max_entries;

	if (lpDDPalette) {
		nentries = 0;
		max_entries = 256;
		if (!fullscreen) {
			nentries = gdwPalEntries;
			max_entries = 2 * (128 - gdwPalEntries);
		}
		SDrawUpdatePalette(nentries, max_entries, &system_palette[nentries], 0);
	}
}

void ApplyGamma(PALETTEENTRY *dst, PALETTEENTRY *src, int n)
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

void DecreaseGamma()
{
	if (gamma_correction > 30) {
		gamma_correction -= 5;
		if (gamma_correction < 30)
			gamma_correction = 30;
		ApplyGamma(system_palette, logical_palette, 256);
		palette_update();
	}
}

int UpdateGamma(int gamma)
{
	if (gamma) {
		gamma_correction = 130 - gamma;
		ApplyGamma(system_palette, logical_palette, 256);
		palette_update();
	}
	return 130 - gamma_correction;
}

void BlackPalette()
{
	SetFadeLevel(0);
}

void SetFadeLevel(DWORD fadeval)
{
	int i;
	RECT SrcRect;

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

		// Workaround for flickering mouse in caves https://github.com/diasurgical/devilutionX/issues/7
		SrcRect.left = 64;
		SrcRect.top = 160;
		SrcRect.right = BUFFER_WIDTH;
		SrcRect.bottom = BUFFER_HEIGHT; // menu isn't offset so make sure we copy all of it
		lpDDSPrimary->BltFast(0, 0, lpDDSBackBuf, &SrcRect, DDBLTFAST_WAIT);
		lpDDSPrimary->Unlock(NULL);
	}
}

void PaletteFadeIn(int fr)
{
	int i;

	ApplyGamma(logical_palette, orig_palette, 256);
	for (i = 0; i < 256; i += fr) {
		SetFadeLevel(i);
	}
	SetFadeLevel(256);
	memcpy(logical_palette, orig_palette, sizeof(orig_palette));
	sgbFadedIn = TRUE;
}

void PaletteFadeOut(int fr)
{
	int i;

	if (sgbFadedIn) {
		for (i = 256; i > 0; i -= fr) {
			SetFadeLevel(i);
		}
		SetFadeLevel(0);
		sgbFadedIn = FALSE;
	}
}

void palette_update_caves()
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

#ifndef SPAWN
void palette_update_quest_palette(int n)
{
	int i;

	for (i = 32 - n; i >= 0; i--) {
		logical_palette[i] = orig_palette[i];
	}
	ApplyGamma(system_palette, logical_palette, 32);
	palette_update();
}
#endif

BOOL palette_get_colour_cycling()
{
	return color_cycling_enabled;
}

BOOL palette_set_color_cycling(BOOL enabled)
{
	color_cycling_enabled = enabled;
	return enabled;
}

DEVILUTION_END_NAMESPACE

/**
 * @file palette.cpp
 *
 * Implementation of functions for handling the engines color palette.
 */
#include "all.h"
#include "../SourceX/display.h"
#include "../3rdParty/Storm/Source/storm.h"

DEVILUTION_BEGIN_NAMESPACE

SDL_Color logical_palette[256];
SDL_Color system_palette[256];
SDL_Color orig_palette[256];
BYTE palette_transparency_lookup[256][256]; //Fluffy

/* data */

/** Specifies the gamma correction level. */
int gamma_correction = 100;
/** Specifies whether colour cycling is enabled. */
BOOL color_cycling_enabled = TRUE;
/** Specifies whether the palette has max brightness. */
BOOLEAN sgbFadedIn = TRUE;

void palette_update()
{
	assert(palette);
	if (SDLC_SetSurfaceAndPaletteColors(pal_surface, palette, system_palette, 0, 256) < 0) {
		ErrSdl();
	}
	pal_surface_palette_version++;
}

void ApplyGamma(SDL_Color *dst, const SDL_Color *src, int n)
{
	int i;
	double g;

	g = gamma_correction / 100.0;

	for (i = 0; i < n; i++) {
		dst[i].r = pow(src[i].r / 256.0, g) * 256.0;
		dst[i].g = pow(src[i].g / 256.0, g) * 256.0;
		dst[i].b = pow(src[i].b / 256.0, g) * 256.0;
	}
	force_redraw = 255;
}

void SaveGamma()
{
	SRegSaveValue("Diablo", "Gamma Correction", 0, gamma_correction);
	SRegSaveValue("Diablo", "Color Cycling", FALSE, color_cycling_enabled);
}

static void LoadGamma()
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

void palette_init()
{
	LoadGamma();
	memcpy(system_palette, orig_palette, sizeof(orig_palette));
	InitPalette();
}

void LoadPalette(const char *pszFileName)
{
	int i;
	void *pBuf;
	BYTE PalData[256][3];

	assert(pszFileName);

	SFileOpenFile(pszFileName, &pBuf);
	SFileReadFile(pBuf, (char *)PalData, sizeof(PalData), NULL, NULL);
	SFileCloseFile(pBuf);

	for (i = 0; i < 256; i++) {
		orig_palette[i].r = PalData[i][0];
		orig_palette[i].g = PalData[i][1];
		orig_palette[i].b = PalData[i][2];
#ifndef USE_SDL1
		orig_palette[i].a = SDL_ALPHA_OPAQUE;
#endif
	}

	/* Fluffy: Generate lookup table for transparency
	*
	* Explanation for how this works: To mimic 50% transparency we figure out what colours in the existing palette are the best match for the combination of any 2 colours.
	* We save this info in a lookup table we use during rendering for whenever we want this kind of transparency.
	* 
	*/
	if (options_transparency == 1) {
		for (int i = 0; i < 256; i++) {
			for (int j = 0; j < 256; j++) {
				if (i == j) { //No need to calculate transparency between 2 identical colours
					palette_transparency_lookup[i][j] = j;
					continue;
				} else if (i > j) { //Since there's a lot of redundancy ([i][j] will always have the same value as [j][i]), we skip calculating combinations which have already been calculated
					palette_transparency_lookup[i][j] = palette_transparency_lookup[j][i];
					continue;
				}

				Uint8 r = ((int)orig_palette[i].r + (int)orig_palette[j].r) / 2;
				Uint8 g = ((int)orig_palette[i].g + (int)orig_palette[j].g) / 2;
				Uint8 b = ((int)orig_palette[i].b + (int)orig_palette[j].b) / 2;
				BYTE best;
				int bestDiff = 255 * 3;
				for (int k = 0; k < 256; k++) {
					int diffr = orig_palette[k].r - r;
					int diffg = orig_palette[k].g - g;
					int diffb = orig_palette[k].b - b;
					int diff = diffr * diffr + diffg * diffg + diffb * diffb;

					if (k == 0 || bestDiff > diff) {
						best = k;
						bestDiff = diff;
					}
				}
				palette_transparency_lookup[i][j] = best;
			}
		}
	}
}

void LoadRndLvlPal(int l)
{
	int rv;
	char szFileName[MAX_PATH];

	if (l == DTYPE_TOWN) {
		LoadPalette("Levels\\TownData\\Town.pal");
	} else {
		rv = random_(0, 4) + 1;
		sprintf(szFileName, "Levels\\L%iData\\L%i_%i.PAL", l, l, rv);
		if (l == 5) {
			sprintf(szFileName, "NLevels\\L5Data\\L5Base.PAL");
		}
		if (l == 6) {
			if (!UseNestArt) {
				rv++;
			}
			sprintf(szFileName, "NLevels\\L%iData\\L%iBase%i.PAL", 6, 6, rv);
		}
		LoadPalette(szFileName);
	}
}

void ResetPal()
{
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
	SaveGamma();
	return 130 - gamma_correction;
}

void SetFadeLevel(DWORD fadeval)
{
	int i;

	for (i = 0; i < 256; i++) { // BUGFIX: should be 256 (fixed)
		system_palette[i].r = (fadeval * logical_palette[i].r) >> 8;
		system_palette[i].g = (fadeval * logical_palette[i].g) >> 8;
		system_palette[i].b = (fadeval * logical_palette[i].b) >> 8;
	}
	palette_update();
}

void BlackPalette()
{
	SetFadeLevel(0);
}

void PaletteFadeIn(int fr)
{
	int i;

	ApplyGamma(logical_palette, orig_palette, 256);
	DWORD tc = SDL_GetTicks();
	for (i = 0; i < 256; i = (SDL_GetTicks() - tc) / 2.083) { // 32 frames @ 60hz
		SetFadeLevel(i);
		SDL_Rect SrcRect = { SCREEN_X, SCREEN_Y, SCREEN_WIDTH, SCREEN_HEIGHT };
		BltFast(&SrcRect, NULL);
		RenderPresent();
	}
	SetFadeLevel(256);
	memcpy(logical_palette, orig_palette, sizeof(orig_palette));
	sgbFadedIn = TRUE;
}

void PaletteFadeOut(int fr)
{
	int i;

	if (sgbFadedIn) {
		DWORD tc = SDL_GetTicks();
		for (i = 256; i > 0; i = 256 - (SDL_GetTicks() - tc) / 2.083) { // 32 frames @ 60hz
			SetFadeLevel(i);
			SDL_Rect SrcRect = { SCREEN_X, SCREEN_Y, SCREEN_WIDTH, SCREEN_HEIGHT };
			BltFast(&SrcRect, NULL);
			RenderPresent();
		}
		SetFadeLevel(0);
		sgbFadedIn = FALSE;
	}
}

void palette_update_caves()
{
	int i;
	SDL_Color col;

	col = system_palette[1];
	for (i = 1; i < 31; i++) {
		system_palette[i] = system_palette[i + 1];
	}
	system_palette[i] = col;

	palette_update();
}

int dword_6E2D58;
int dword_6E2D54;
void palette_update_crypt()
{
	int i;
	SDL_Color col;

	if (dword_6E2D58 > 1) {
		col = system_palette[15];
		for (i = 15; i > 1; i--) {
			system_palette[i].r = system_palette[i - 1].r;
			system_palette[i].g = system_palette[i - 1].g;
			system_palette[i].b = system_palette[i - 1].b;
		}
		system_palette[i].r = col.r;
		system_palette[i].g = col.g;
		system_palette[i].b = col.b;

		dword_6E2D58 = 0;
	} else {
		dword_6E2D58++;
	}
	if (dword_6E2D54 > 0) {
		col = system_palette[31];
		for (i = 31; i > 16; i--) {
			system_palette[i].r = system_palette[i - 1].r;
			system_palette[i].g = system_palette[i - 1].g;
			system_palette[i].b = system_palette[i - 1].b;
		}
		system_palette[i].r = col.r;
		system_palette[i].g = col.g;
		system_palette[i].b = col.b;
		palette_update();
		dword_6E2D54++;
	} else {
		dword_6E2D54 = 1;
	}
}

int dword_6E2D5C;
int dword_6E2D60;
void palette_update_hive()
{
	int i;
	SDL_Color col;

	if (dword_6E2D60 == 2) {
		col = system_palette[8];
		for (i = 8; i > 1; i--) {
			system_palette[i].r = system_palette[i - 1].r;
			system_palette[i].g = system_palette[i - 1].g;
			system_palette[i].b = system_palette[i - 1].b;
		}
		system_palette[i].r = col.r;
		system_palette[i].g = col.g;
		system_palette[i].b = col.b;
		dword_6E2D60 = 0;
	} else {
		dword_6E2D60++;
	}
	if (dword_6E2D5C == 2) {
		col = system_palette[15];
		for (i = 15; i > 9; i--) {
			system_palette[i].r = system_palette[i - 1].r;
			system_palette[i].g = system_palette[i - 1].g;
			system_palette[i].b = system_palette[i - 1].b;
		}
		system_palette[i].r = col.r;
		system_palette[i].g = col.g;
		system_palette[i].b = col.b;
		palette_update();
		dword_6E2D5C = 0;
	} else {
		dword_6E2D5C++;
	}
}

void palette_update_quest_palette(int n)
{
	int i;

	for (i = 32 - n; i >= 0; i--) {
		logical_palette[i] = orig_palette[i];
	}
	ApplyGamma(system_palette, logical_palette, 32);
	palette_update();
}

BOOL palette_get_color_cycling()
{
	return color_cycling_enabled;
}

BOOL palette_set_color_cycling(BOOL enabled)
{
	color_cycling_enabled = enabled;
	return enabled;
}

DEVILUTION_END_NAMESPACE

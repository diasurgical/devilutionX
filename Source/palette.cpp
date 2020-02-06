#include "diablo.h"
#include "../3rdParty/Storm/Source/storm.h"

DEVILUTION_BEGIN_NAMESPACE

SDL_Color logical_palette[256];
SDL_Color system_palette[256];
SDL_Color orig_palette[256];
int gdwPalEntries;

/* data */

int gamma_correction = 100;
BOOL color_cycling_enabled = TRUE;
BOOLEAN sgbFadedIn = TRUE;

static void palette_update()
{
	int nentries;
	int max_entries;

	if (1) {
		nentries = 0;
		max_entries = 256;
		if (!fullscreen) {
			nentries = gdwPalEntries;
			max_entries = 2 * (128 - gdwPalEntries);
		}
		SDrawUpdatePalette(nentries, max_entries, &system_palette[nentries], 0);
	}
}

void ApplyGamma(SDL_Color *dst, const SDL_Color *src, int n)
{
	int i;
	double g;

	g = gamma_correction / 100.0;

	for (i = 0; i < n; i++) {
		dst->r = pow(src->r / 256.0, g) * 256.0;
		dst->g = pow(src->g / 256.0, g) * 256.0;
		dst->b = pow(src->b / 256.0, g) * 256.0;
		dst++;
		src++;
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
	CreatePalette();
}

void LoadPalette(char *pszFileName)
{
	int i;
	void *pBuf;
	BYTE PalData[256][3];

	assert(pszFileName);

	WOpenFile(pszFileName, &pBuf, FALSE);
	WReadFile(pBuf, (char *)PalData, sizeof(PalData), pszFileName);
	WCloseFile(pBuf);

	for (i = 0; i < 256; i++) {
		orig_palette[i].r = PalData[i][0];
		orig_palette[i].g = PalData[i][1];
		orig_palette[i].b = PalData[i][2];
#ifndef USE_SDL1
		orig_palette[i].a = SDL_ALPHA_OPAQUE;
#endif
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

	for (i = 0; i < 255; i++) {
		system_palette[i].r = (fadeval * logical_palette[i].r) >> 8;
		system_palette[i].g = (fadeval * logical_palette[i].g) >> 8;
		system_palette[i].b = (fadeval * logical_palette[i].b) >> 8;
	}
	palette_update();

	// Workaround for flickering mouse in caves https://github.com/diasurgical/devilutionX/issues/7
	SDL_Rect SrcRect = {
		SCREEN_X,
		SCREEN_Y,
		SCREEN_WIDTH,
		SCREEN_HEIGHT,
	};
	SDL_Rect DstRect = {
		0,
		0,
		SCREEN_WIDTH,
		SCREEN_HEIGHT,
	};

	BltFast(&SrcRect, &DstRect);
	RenderPresent();
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

/**
 * @file palette.cpp
 *
 * Implementation of functions for handling the engines color palette.
 */
#include "engine/palette.h"

#include <fmt/compile.h>

#include "engine/dx.h"
#include "engine/load_file.hpp"
#include "engine/random.hpp"
#include "hwcursor.hpp"
#include "options.h"
#include "utils/display.h"
#include "utils/sdl_compat.h"

namespace devilution {

SDL_Color logical_palette[256];
SDL_Color system_palette[256];
SDL_Color orig_palette[256];
Uint8 paletteTransparencyLookup[256][256];

#if DEVILUTIONX_PALETTE_TRANSPARENCY_BLACK_16_LUT
uint16_t paletteTransparencyLookupBlack16[65536];
#endif

namespace {

/** Specifies whether the palette has max brightness. */
bool sgbFadedIn = true;

void LoadGamma()
{
	int gammaValue = *sgOptions.Graphics.gammaCorrection;
	gammaValue = clamp(gammaValue, 30, 100);
	sgOptions.Graphics.gammaCorrection.SetValue(gammaValue - gammaValue % 5);
}

Uint8 FindBestMatchForColor(SDL_Color *palette, SDL_Color color, int skipFrom, int skipTo)
{
	Uint8 best;
	Uint32 bestDiff = SDL_MAX_UINT32;
	for (int i = 0; i < 256; i++) {
		if (i >= skipFrom && i <= skipTo)
			continue;
		int diffr = palette[i].r - color.r;
		int diffg = palette[i].g - color.g;
		int diffb = palette[i].b - color.b;
		Uint32 diff = diffr * diffr + diffg * diffg + diffb * diffb;

		if (bestDiff > diff) {
			best = i;
			bestDiff = diff;
		}
	}
	return best;
}

/**
 * @brief Generate lookup table for transparency
 *
 * This is based of the same technique found in Quake2.
 *
 * To mimic 50% transparency we figure out what colors in the existing palette are the best match for the combination of any 2 colors.
 * We save this into a lookup table for use during rendering.
 *
 * @param palette The colors to operate on
 * @param skipFrom Do not use colors between this index and skipTo
 * @param skipTo Do not use colors between skipFrom and this index
 * @param toUpdate Only update the first n colors
 */
void GenerateBlendedLookupTable(SDL_Color *palette, int skipFrom, int skipTo, int toUpdate = 256)
{
	for (int i = 0; i < 256; i++) {
		for (int j = 0; j < 256; j++) {
			if (i == j) { // No need to calculate transparency between 2 identical colors
				paletteTransparencyLookup[i][j] = j;
				continue;
			}
			if (i > j) { // Half the blends will be mirror identical ([i][j] is the same as [j][i]), so simply copy the existing combination.
				paletteTransparencyLookup[i][j] = paletteTransparencyLookup[j][i];
				continue;
			}
			if (i > toUpdate && j > toUpdate) {
				continue;
			}

			SDL_Color blendedColor;
			blendedColor.r = ((int)palette[i].r + (int)palette[j].r) / 2;
			blendedColor.g = ((int)palette[i].g + (int)palette[j].g) / 2;
			blendedColor.b = ((int)palette[i].b + (int)palette[j].b) / 2;
			Uint8 best = FindBestMatchForColor(palette, blendedColor, skipFrom, skipTo);
			paletteTransparencyLookup[i][j] = best;
		}
	}

#if DEVILUTIONX_PALETTE_TRANSPARENCY_BLACK_16_LUT
	for (unsigned i = 0; i < 256; ++i) {
		for (unsigned j = 0; j < 256; ++j) {
#if SDL_BYTEORDER == SDL_LIL_ENDIAN
			const std::uint16_t index = i | (j << 8);
#else
			const std::uint16_t index = j | (i << 8);
#endif
			paletteTransparencyLookupBlack16[index] = paletteTransparencyLookup[0][i] | (paletteTransparencyLookup[0][j] << 8);
		}
	}
#endif
}

/**
 * @brief Cycle the given range of colors in the palette
 * @param from First color index of the range
 * @param to First color index of the range
 */
void CycleColors(int from, int to)
{
	{
		SDL_Color col = system_palette[from];
		for (int i = from; i < to; i++) {
			system_palette[i] = system_palette[i + 1];
		}
		system_palette[to] = col;
	}

	for (auto &palette : paletteTransparencyLookup) {
		Uint8 col = palette[from];
		for (int j = from; j < to; j++) {
			palette[j] = palette[j + 1];
		}
		palette[to] = col;
	}

	Uint8 colRow[256];
	memcpy(colRow, &paletteTransparencyLookup[from], sizeof(*paletteTransparencyLookup));
	for (int i = from; i < to; i++) {
		memcpy(&paletteTransparencyLookup[i], &paletteTransparencyLookup[i + 1], sizeof(*paletteTransparencyLookup));
	}
	memcpy(&paletteTransparencyLookup[to], colRow, sizeof(colRow));
}

/**
 * @brief Cycle the given range of colors in the palette in reverse direction
 * @param from First color index of the range
 * @param to Last color index of the range
 */
void CycleColorsReverse(int from, int to)
{
	{
		SDL_Color col = system_palette[to];
		for (int i = to; i > from; i--) {
			system_palette[i] = system_palette[i - 1];
		}
		system_palette[from] = col;
	}

	for (auto &palette : paletteTransparencyLookup) {
		Uint8 col = palette[to];
		for (int j = to; j > from; j--) {
			palette[j] = palette[j - 1];
		}
		palette[from] = col;
	}

	Uint8 colRow[256];
	memcpy(colRow, &paletteTransparencyLookup[to], sizeof(*paletteTransparencyLookup));
	for (int i = to; i > from; i--) {
		memcpy(&paletteTransparencyLookup[i], &paletteTransparencyLookup[i - 1], sizeof(*paletteTransparencyLookup));
	}
	memcpy(&paletteTransparencyLookup[from], colRow, sizeof(colRow));
}

} // namespace

void palette_update(int first, int ncolor)
{
	if (HeadlessMode)
		return;

	assert(Palette);
	if (SDLC_SetSurfaceAndPaletteColors(PalSurface, Palette.get(), system_palette, first, ncolor) < 0) {
		ErrSdl();
	}
	pal_surface_palette_version++;
}

void ApplyGamma(SDL_Color *dst, const SDL_Color *src, int n)
{
	double g = *sgOptions.Graphics.gammaCorrection / 100.0;

	for (int i = 0; i < n; i++) {
		dst[i].r = static_cast<Uint8>(pow(src[i].r / 256.0, g) * 256.0);
		dst[i].g = static_cast<Uint8>(pow(src[i].g / 256.0, g) * 256.0);
		dst[i].b = static_cast<Uint8>(pow(src[i].b / 256.0, g) * 256.0);
	}
	force_redraw = 255;
}

void palette_init()
{
	LoadGamma();
	memcpy(system_palette, orig_palette, sizeof(orig_palette));
	InitPalette();
}

void LoadPalette(const char *pszFileName, bool blend /*= true*/)
{
	assert(pszFileName);

	if (HeadlessMode)
		return;

	struct Color {
		uint8_t r;
		uint8_t g;
		uint8_t b;
	};

	std::array<Color, 256> palData;

	LoadFileInMem(pszFileName, palData);

	for (unsigned i = 0; i < palData.size(); i++) {
		orig_palette[i].r = palData[i].r;
		orig_palette[i].g = palData[i].g;
		orig_palette[i].b = palData[i].b;
#ifndef USE_SDL1
		orig_palette[i].a = SDL_ALPHA_OPAQUE;
#endif
	}

	if (blend) {
		if (leveltype == DTYPE_CAVES || leveltype == DTYPE_CRYPT) {
			GenerateBlendedLookupTable(orig_palette, 1, 31);
		} else if (leveltype == DTYPE_NEST) {
			GenerateBlendedLookupTable(orig_palette, 1, 15);
		} else {
			GenerateBlendedLookupTable(orig_palette, -1, -1);
		}
	}
}

void LoadRndLvlPal(dungeon_type l)
{
	if (HeadlessMode)
		return;

	if (l == DTYPE_TOWN) {
		LoadPalette("levels\\towndata\\town.pal");
		return;
	}

	int rv = GenerateRnd(4) + 1;
	if (l == DTYPE_CRYPT) {
		LoadPalette("nlevels\\l5data\\l5base.pal");
		return;
	}

	char szFileName[27];
	if (l == DTYPE_NEST) {
		if (!*sgOptions.Graphics.alternateNestArt) {
			rv++;
		}
		*fmt::format_to(szFileName, FMT_COMPILE(R"(nlevels\l{0}data\l{0}base{1}.pal)"), 6, rv) = '\0';
	} else {
		*fmt::format_to(szFileName, FMT_COMPILE(R"(levels\l{0}data\l{0}_{1}.pal)"), l, rv) = '\0';
	}
	LoadPalette(szFileName);
}

void IncreaseGamma()
{
	int gammaValue = *sgOptions.Graphics.gammaCorrection;
	if (gammaValue < 100) {
		sgOptions.Graphics.gammaCorrection.SetValue(std::min(gammaValue + 5, 100));
		ApplyGamma(system_palette, logical_palette, 256);
		palette_update();
	}
}

void DecreaseGamma()
{
	int gammaValue = *sgOptions.Graphics.gammaCorrection;
	if (gammaValue > 30) {
		sgOptions.Graphics.gammaCorrection.SetValue(std::max(gammaValue - 5, 30));
		ApplyGamma(system_palette, logical_palette, 256);
		palette_update();
	}
}

int UpdateGamma(int gamma)
{
	if (gamma > 0) {
		sgOptions.Graphics.gammaCorrection.SetValue(130 - gamma);
		ApplyGamma(system_palette, logical_palette, 256);
		palette_update();
	}
	return 130 - *sgOptions.Graphics.gammaCorrection;
}

void SetFadeLevel(int fadeval)
{
	if (HeadlessMode)
		return;

	for (int i = 0; i < 256; i++) {
		system_palette[i].r = (fadeval * logical_palette[i].r) / 256;
		system_palette[i].g = (fadeval * logical_palette[i].g) / 256;
		system_palette[i].b = (fadeval * logical_palette[i].b) / 256;
	}
	palette_update();
	if (IsHardwareCursor()) {
		ReinitializeHardwareCursor();
	}
}

void BlackPalette()
{
	SetFadeLevel(0);
}

void PaletteFadeIn(int fr)
{
	if (HeadlessMode)
		return;

	ApplyGamma(logical_palette, orig_palette, 256);

	const uint32_t tc = SDL_GetTicks();
	fr *= 3;

	uint32_t prevFadeValue = 255;
	for (uint32_t i = 0; i < 256; i = fr * (SDL_GetTicks() - tc) / 50) {
		if (i != prevFadeValue) {
			SetFadeLevel(i);
			prevFadeValue = i;
		}
		BltFast(nullptr, nullptr);
		RenderPresent();
	}
	SetFadeLevel(256);

	memcpy(logical_palette, orig_palette, sizeof(orig_palette));

	sgbFadedIn = true;
}

void PaletteFadeOut(int fr)
{
	if (!sgbFadedIn || HeadlessMode)
		return;

	const uint32_t tc = SDL_GetTicks();
	fr *= 3;

	uint32_t prevFadeValue = 0;
	for (uint32_t i = 0; i < 256; i = fr * (SDL_GetTicks() - tc) / 50) {
		if (i != prevFadeValue) {
			SetFadeLevel(256 - i);
			prevFadeValue = i;
		}
		BltFast(nullptr, nullptr);
		RenderPresent();
	}
	SetFadeLevel(0);

	sgbFadedIn = false;
}

void palette_update_caves()
{
	CycleColors(1, 31);
	palette_update(0, 31);
}

/**
 * @brief Cycle the lava every other frame, and glow every frame
 * Lava has 15 colors and the glow 16, so the full animation has 240 frames before it loops
 */
void palette_update_crypt()
{
	static bool delayLava = false;

	if (!delayLava) {
		CycleColorsReverse(1, 15);
		delayLava = false;
	}

	CycleColorsReverse(16, 31);
	palette_update(0, 31);
	delayLava = !delayLava;
}

/**
 * @brief Cycle the pond waves and bubles colors every 3rd frame
 * Bubles have 8 colors and waves 7, so the full animation has 56 frames before it loops
 */
void palette_update_hive()
{
	static uint8_t delay = 0;

	if (delay != 2) {
		delay++;
		return;
	}

	CycleColorsReverse(1, 8);
	CycleColorsReverse(9, 15);
	palette_update(0, 15);
	delay = 0;
}

void palette_update_quest_palette(int n)
{
	int i = 32 - n;
	logical_palette[i] = orig_palette[i];
	ApplyGamma(system_palette, logical_palette, 32);
	palette_update(0, 31);
	// Update blended transparency, but only for the color that was updated
	for (int j = 0; j < 256; j++) {
		if (i == j) { // No need to calculate transparency between 2 identical colors
			paletteTransparencyLookup[i][j] = j;
			continue;
		}
		SDL_Color blendedColor;
		blendedColor.r = ((int)logical_palette[i].r + (int)logical_palette[j].r) / 2;
		blendedColor.g = ((int)logical_palette[i].g + (int)logical_palette[j].g) / 2;
		blendedColor.b = ((int)logical_palette[i].b + (int)logical_palette[j].b) / 2;
		Uint8 best = FindBestMatchForColor(logical_palette, blendedColor, 1, 31);
		paletteTransparencyLookup[i][j] = paletteTransparencyLookup[j][i] = best;
	}
}

} // namespace devilution

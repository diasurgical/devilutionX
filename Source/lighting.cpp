/**
 * @file lighting.cpp
 *
 * Implementation of light and vision.
 */
#include "lighting.h"

#include <algorithm>

#include "automap.h"
#include "diablo.h"
#include "engine/load_file.hpp"
#include "player.h"

namespace devilution {

Light VisionList[MAXVISION];
int VisionCount;
int VisionId;
Light Lights[MAXLIGHTS];
uint8_t ActiveLights[MAXLIGHTS];
int ActiveLightCount;
std::array<uint8_t, LIGHTSIZE> LightTables;
bool DisableLighting;
bool UpdateLighting;

namespace {

/*
 * X- Y-coordinate offsets of lighting visions.
 * The last entry-pair is only for alignment.
 */
const DisplacementOf<int8_t> VisionCrawlTable[23][15] = {
	// clang-format off
	{ { 1, 0 }, { 2, 0 }, { 3, 0 }, { 4, 0 }, { 5, 0 }, { 6, 0 }, { 7, 0 }, { 8, 0 }, { 9, 0 }, { 10,  0 }, { 11,  0 }, { 12,  0 }, { 13,  0 }, { 14,  0 }, { 15,  0 } },
	{ { 1, 0 }, { 2, 0 }, { 3, 0 }, { 4, 0 }, { 5, 0 }, { 6, 0 }, { 7, 0 }, { 8, 1 }, { 9, 1 }, { 10,  1 }, { 11,  1 }, { 12,  1 }, { 13,  1 }, { 14,  1 }, { 15,  1 } },
	{ { 1, 0 }, { 2, 0 }, { 3, 0 }, { 4, 1 }, { 5, 1 }, { 6, 1 }, { 7, 1 }, { 8, 1 }, { 9, 1 }, { 10,  1 }, { 11,  1 }, { 12,  2 }, { 13,  2 }, { 14,  2 }, { 15,  2 } },
	{ { 1, 0 }, { 2, 0 }, { 3, 1 }, { 4, 1 }, { 5, 1 }, { 6, 1 }, { 7, 1 }, { 8, 2 }, { 9, 2 }, { 10,  2 }, { 11,  2 }, { 12,  2 }, { 13,  3 }, { 14,  3 }, { 15,  3 } },
	{ { 1, 0 }, { 2, 1 }, { 3, 1 }, { 4, 1 }, { 5, 1 }, { 6, 2 }, { 7, 2 }, { 8, 2 }, { 9, 3 }, { 10,  3 }, { 11,  3 }, { 12,  3 }, { 13,  4 }, { 14,  4 }, {  0,  0 } },
	{ { 1, 0 }, { 2, 1 }, { 3, 1 }, { 4, 1 }, { 5, 2 }, { 6, 2 }, { 7, 3 }, { 8, 3 }, { 9, 3 }, { 10,  4 }, { 11,  4 }, { 12,  4 }, { 13,  5 }, { 14,  5 }, {  0,  0 } },
	{ { 1, 0 }, { 2, 1 }, { 3, 1 }, { 4, 2 }, { 5, 2 }, { 6, 3 }, { 7, 3 }, { 8, 3 }, { 9, 4 }, { 10,  4 }, { 11,  5 }, { 12,  5 }, { 13,  6 }, { 14,  6 }, {  0,  0 } },
	{ { 1, 1 }, { 2, 1 }, { 3, 2 }, { 4, 2 }, { 5, 3 }, { 6, 3 }, { 7, 4 }, { 8, 4 }, { 9, 5 }, { 10,  5 }, { 11,  6 }, { 12,  6 }, { 13,  7 }, {  0,  0 }, {  0,  0 } },
	{ { 1, 1 }, { 2, 1 }, { 3, 2 }, { 4, 2 }, { 5, 3 }, { 6, 4 }, { 7, 4 }, { 8, 5 }, { 9, 6 }, { 10,  6 }, { 11,  7 }, { 12,  7 }, { 12,  8 }, { 13,  8 }, {  0,  0 } },
	{ { 1, 1 }, { 2, 2 }, { 3, 2 }, { 4, 3 }, { 5, 4 }, { 6, 5 }, { 7, 5 }, { 8, 6 }, { 9, 7 }, { 10,  7 }, { 10,  8 }, { 11,  8 }, { 12,  9 }, {  0,  0 }, {  0,  0 } },
	{ { 1, 1 }, { 2, 2 }, { 3, 3 }, { 4, 4 }, { 5, 5 }, { 6, 5 }, { 7, 6 }, { 8, 7 }, { 9, 8 }, { 10,  9 }, { 11,  9 }, { 11, 10 }, {  0,  0 }, {  0,  0 }, {  0,  0 } },
	{ { 1, 1 }, { 2, 2 }, { 3, 3 }, { 4, 4 }, { 5, 5 }, { 6, 6 }, { 7, 7 }, { 8, 8 }, { 9, 9 }, { 10, 10 }, { 11, 11 }, {  0,  0 }, {  0,  0 }, {  0,  0 }, {  0,  0 } },
	{ { 1, 1 }, { 2, 2 }, { 3, 3 }, { 4, 4 }, { 5, 5 }, { 5, 6 }, { 6, 7 }, { 7, 8 }, { 8, 9 }, {  9, 10 }, {  9, 11 }, { 10, 11 }, {  0,  0 }, {  0,  0 }, {  0,  0 } },
	{ { 1, 1 }, { 2, 2 }, { 2, 3 }, { 3, 4 }, { 4, 5 }, { 5, 6 }, { 5, 7 }, { 6, 8 }, { 7, 9 }, {  7, 10 }, {  8, 10 }, {  8, 11 }, {  9, 12 }, {  0,  0 }, {  0,  0 } },
	{ { 1, 1 }, { 1, 2 }, { 2, 3 }, { 2, 4 }, { 3, 5 }, { 4, 6 }, { 4, 7 }, { 5, 8 }, { 6, 9 }, {  6, 10 }, {  7, 11 }, {  7, 12 }, {  8, 12 }, {  8, 13 }, {  0,  0 } },
	{ { 1, 1 }, { 1, 2 }, { 2, 3 }, { 2, 4 }, { 3, 5 }, { 3, 6 }, { 4, 7 }, { 4, 8 }, { 5, 9 }, {  5, 10 }, {  6, 11 }, {  6, 12 }, {  7, 13 }, {  0,  0 }, {  0,  0 } },
	{ { 0, 1 }, { 1, 2 }, { 1, 3 }, { 2, 4 }, { 2, 5 }, { 3, 6 }, { 3, 7 }, { 3, 8 }, { 4, 9 }, {  4, 10 }, {  5, 11 }, {  5, 12 }, {  6, 13 }, {  6, 14 }, {  0,  0 } },
	{ { 0, 1 }, { 1, 2 }, { 1, 3 }, { 1, 4 }, { 2, 5 }, { 2, 6 }, { 3, 7 }, { 3, 8 }, { 3, 9 }, {  4, 10 }, {  4, 11 }, {  4, 12 }, {  5, 13 }, {  5, 14 }, {  0,  0 } },
	{ { 0, 1 }, { 1, 2 }, { 1, 3 }, { 1, 4 }, { 1, 5 }, { 2, 6 }, { 2, 7 }, { 2, 8 }, { 3, 9 }, {  3, 10 }, {  3, 11 }, {  3, 12 }, {  4, 13 }, {  4, 14 }, {  0,  0 } },
	{ { 0, 1 }, { 0, 2 }, { 1, 3 }, { 1, 4 }, { 1, 5 }, { 1, 6 }, { 1, 7 }, { 2, 8 }, { 2, 9 }, {  2, 10 }, {  2, 11 }, {  2, 12 }, {  3, 13 }, {  3, 14 }, {  3, 15 } },
	{ { 0, 1 }, { 0, 2 }, { 0, 3 }, { 1, 4 }, { 1, 5 }, { 1, 6 }, { 1, 7 }, { 1, 8 }, { 1, 9 }, {  1, 10 }, {  1, 11 }, {  2, 12 }, {  2, 13 }, {  2, 14 }, {  2, 15 } },
	{ { 0, 1 }, { 0, 2 }, { 0, 3 }, { 0, 4 }, { 0, 5 }, { 0, 6 }, { 0, 7 }, { 1, 8 }, { 1, 9 }, {  1, 10 }, {  1, 11 }, {  1, 12 }, {  1, 13 }, {  1, 14 }, {  1, 15 } },
	{ { 0, 1 }, { 0, 2 }, { 0, 3 }, { 0, 4 }, { 0, 5 }, { 0, 6 }, { 0, 7 }, { 0, 8 }, { 0, 9 }, {  0, 10 }, {  0, 11 }, {  0, 12 }, {  0, 13 }, {  0, 14 }, {  0, 15 } },
	// clang-format on
};

uint8_t lightradius[16][128];
bool dovision;
uint8_t lightblock[64][16][16];

/** RadiusAdj maps from VisionCrawlTable index to lighting vision radius adjustment. */
const uint8_t RadiusAdj[23] = { 0, 0, 0, 0, 1, 1, 1, 2, 2, 2, 3, 4, 3, 2, 2, 2, 1, 1, 1, 0, 0, 0, 0 };

void RotateRadius(int *x, int *y, int *dx, int *dy, int *lx, int *ly, int *bx, int *by)
{
	*bx = 0;
	*by = 0;

	int swap = *dx;
	*dx = 7 - *dy;
	*dy = swap;
	swap = *lx;
	*lx = 7 - *ly;
	*ly = swap;

	*x = *dx - *lx;
	*y = *dy - *ly;

	if (*x < 0) {
		*x += 8;
		*bx = 1;
	}
	if (*y < 0) {
		*y += 8;
		*by = 1;
	}
}

void SetLight(Point position, char v)
{
	if (LoadingMapObjects)
		dPreLight[position.x][position.y] = v;
	else
		dLight[position.x][position.y] = v;
}

char GetLight(Point position)
{
	if (LoadingMapObjects)
		return dPreLight[position.x][position.y];

	return dLight[position.x][position.y];
}

void DoUnLight(int nXPos, int nYPos, int nRadius)
{
	nRadius++;

	int minX = nXPos - nRadius;
	int maxX = nXPos + nRadius;
	int minY = nYPos - nRadius;
	int maxY = nYPos + nRadius;

	minX = std::max(minX, 0);
	maxX = std::max(maxX, MAXDUNX);
	minY = std::max(minY, 0);
	maxY = std::max(maxY, MAXDUNY);

	for (int y = minY; y < maxY; y++) {
		for (int x = minX; x < maxX; x++) {
			if (InDungeonBounds({ x, y }))
				dLight[x][y] = dPreLight[x][y];
		}
	}
}

bool CrawlFlipsX(Displacement mirrored, tl::function_ref<bool(Displacement)> function)
{
	for (const Displacement displacement : { mirrored.flipX(), mirrored }) {
		if (!function(displacement))
			return false;
	}
	return true;
}

bool CrawlFlipsY(Displacement mirrored, tl::function_ref<bool(Displacement)> function)
{
	for (const Displacement displacement : { mirrored, mirrored.flipY() }) {
		if (!function(displacement))
			return false;
	}
	return true;
}

bool CrawlFlipsXY(Displacement mirrored, tl::function_ref<bool(Displacement)> function)
{
	for (const Displacement displacement : { mirrored.flipX(), mirrored, mirrored.flipXY(), mirrored.flipY() }) {
		if (!function(displacement))
			return false;
	}
	return true;
}

bool TileAllowsLight(Point position)
{
	if (!InDungeonBounds(position))
		return false;
	return !TileHasAny(dPiece[position.x][position.y], TileProperties::BlockLight);
}

void DoVisionFlags(Point position, MapExplorationType doAutomap, bool visible)
{
	if (doAutomap != MAP_EXP_NONE) {
		if (dFlags[position.x][position.y] != DungeonFlag::None)
			SetAutomapView(position, doAutomap);
		dFlags[position.x][position.y] |= DungeonFlag::Explored;
	}
	if (visible)
		dFlags[position.x][position.y] |= DungeonFlag::Lit;
	dFlags[position.x][position.y] |= DungeonFlag::Visible;
}

} // namespace

bool DoCrawl(unsigned radius, tl::function_ref<bool(Displacement)> function)
{
	if (radius == 0)
		return function(Displacement { 0, 0 });

	if (!CrawlFlipsY({ 0, static_cast<int>(radius) }, function))
		return false;
	for (unsigned i = 1; i < radius; i++) {
		if (!CrawlFlipsXY({ static_cast<int>(i), static_cast<int>(radius) }, function))
			return false;
	}
	if (radius > 1) {
		if (!CrawlFlipsXY({ static_cast<int>(radius) - 1, static_cast<int>(radius) - 1 }, function))
			return false;
	}
	if (!CrawlFlipsX({ static_cast<int>(radius), 0 }, function))
		return false;
	for (unsigned i = 1; i < radius; i++) {
		if (!CrawlFlipsXY({ static_cast<int>(radius), static_cast<int>(i) }, function))
			return false;
	}
	return true;
}

bool DoCrawl(unsigned minRadius, unsigned maxRadius, tl::function_ref<bool(Displacement)> function)
{
	for (unsigned i = minRadius; i <= maxRadius; i++) {
		if (!DoCrawl(i, function))
			return false;
	}
	return true;
}

void DoLighting(Point position, int nRadius, int lnum)
{
	int xoff = 0;
	int yoff = 0;
	int lightX = 0;
	int lightY = 0;
	int blockX = 0;
	int blockY = 0;

	if (lnum >= 0) {
		Light &light = Lights[lnum];
		xoff = light.position.offset.deltaX;
		yoff = light.position.offset.deltaY;
		if (xoff < 0) {
			xoff += 8;
			position -= { 1, 0 };
		}
		if (yoff < 0) {
			yoff += 8;
			position -= { 0, 1 };
		}
	}

	int distX = xoff;
	int distY = yoff;

	int minX = 15;
	if (position.x - 15 < 0) {
		minX = position.x + 1;
	}
	int maxX = 15;
	if (position.x + 15 > MAXDUNX) {
		maxX = MAXDUNX - position.x;
	}
	int minY = 15;
	if (position.y - 15 < 0) {
		minY = position.y + 1;
	}
	int maxY = 15;
	if (position.y + 15 > MAXDUNY) {
		maxY = MAXDUNY - position.y;
	}

	if (InDungeonBounds(position)) {
		if (IsNoneOf(leveltype, DTYPE_NEST, DTYPE_CRYPT)) {
			SetLight(position, 0);
		} else if (GetLight(position) > lightradius[nRadius][0]) {
			SetLight(position, lightradius[nRadius][0]);
		}
	}

	for (int i = 0; i < 4; i++) {
		int mult = xoff + 8 * yoff;
		int yBound = i > 0 && i < 3 ? maxY : minY;
		int xBound = i < 2 ? maxX : minX;
		for (int y = 0; y < yBound; y++) {
			for (int x = 1; x < xBound; x++) {
				int radiusBlock = lightblock[mult][y + blockY][x + blockX];
				if (radiusBlock >= 128)
					continue;
				Point temp = position + (Displacement { x, y }).Rotate(-i);
				int8_t v = lightradius[nRadius][radiusBlock];
				if (!InDungeonBounds(temp))
					continue;
				if (v < GetLight(temp))
					SetLight(temp, v);
			}
		}
		RotateRadius(&xoff, &yoff, &distX, &distY, &lightX, &lightY, &blockX, &blockY);
	}
}

void DoUnVision(Point position, int nRadius)
{
	nRadius++;
	nRadius++; // increasing the radius even further here prevents leaving stray vision tiles behind and doesn't seem to affect monster AI - applying new vision happens in the same tick
	int x1 = std::max(position.x - nRadius, 0);
	int y1 = std::max(position.y - nRadius, 0);
	int x2 = std::min(position.x + nRadius, MAXDUNX);
	int y2 = std::min(position.y + nRadius, MAXDUNY);

	for (int i = x1; i < x2; i++) {
		for (int j = y1; j < y2; j++) {
			dFlags[i][j] &= ~(DungeonFlag::Visible | DungeonFlag::Lit);
		}
	}
}

void DoVision(Point position, int radius, MapExplorationType doAutomap, bool visible)
{
	DoVisionFlags(position, doAutomap, visible);

	static const Displacement factors[] = { { 1, 1 }, { -1, 1 }, { 1, -1 }, { -1, -1 } };
	for (auto factor : factors) {
		for (int j = 0; j < 23; j++) {
			int lineLen = radius - RadiusAdj[j];
			for (int k = 0; k < lineLen; k++) {
				Point crawl = position + VisionCrawlTable[j][k] * factor;
				if (!InDungeonBounds(crawl))
					break;
				bool blockerFlag = TileHasAny(dPiece[crawl.x][crawl.y], TileProperties::BlockLight);
				bool tileOK = !blockerFlag;

				if (VisionCrawlTable[j][k].deltaX > 0 && VisionCrawlTable[j][k].deltaY > 0) {
					tileOK = tileOK || TileAllowsLight(crawl + Displacement { -factor.deltaX, 0 });
					tileOK = tileOK || TileAllowsLight(crawl + Displacement { 0, -factor.deltaY });
				}

				if (!tileOK)
					break;

				DoVisionFlags(crawl, doAutomap, visible);

				if (blockerFlag)
					break;

				int8_t trans = dTransVal[crawl.x][crawl.y];
				if (trans != 0)
					TransList[trans] = true;
			}
		}
	}
}

void MakeLightTable()
{
	uint8_t *tbl = LightTables.data();
	int shade = 0;
	int lights = 15;

	for (int i = 0; i < lights; i++) {
		*tbl++ = 0;
		for (int j = 0; j < 8; j++) {
			uint8_t col = 16 * j + shade;
			uint8_t max = 16 * j + 15;
			for (int k = 0; k < 16; k++) {
				if (k != 0 || j != 0) {
					*tbl++ = col;
				}
				if (col < max) {
					col++;
				} else {
					max = 0;
					col = 0;
				}
			}
		}
		for (int j = 16; j < 20; j++) {
			uint8_t col = 8 * j + (shade >> 1);
			uint8_t max = 8 * j + 7;
			for (int k = 0; k < 8; k++) {
				*tbl++ = col;
				if (col < max) {
					col++;
				} else {
					max = 0;
					col = 0;
				}
			}
		}
		for (int j = 10; j < 16; j++) {
			uint8_t col = 16 * j + shade;
			uint8_t max = 16 * j + 15;
			for (int k = 0; k < 16; k++) {
				*tbl++ = col;
				if (col < max) {
					col++;
				} else {
					max = 0;
					col = 0;
				}
				if (col == 255) {
					max = 0;
					col = 0;
				}
			}
		}
		shade++;
	}

	for (int i = 0; i < 256; i++) {
		*tbl++ = 0;
	}

	if (leveltype == DTYPE_HELL) {
		uint8_t blood[16];
		tbl = LightTables.data();
		for (int i = 0; i < lights; i++) {
			int l1 = lights - i;
			int l2 = l1;
			int div = lights / l1;
			int rem = lights % l1;
			int cnt = 0;
			blood[0] = 0;
			uint8_t col = 1;
			for (int j = 1; j < 16; j++) {
				blood[j] = col;
				l2 += rem;
				if (l2 > l1 && j < 15) {
					j++;
					blood[j] = col;
					l2 -= l1;
				}
				cnt++;
				if (cnt == div) {
					col++;
					cnt = 0;
				}
			}
			*tbl++ = 0;
			for (int j = 1; j <= 15; j++) {
				*tbl++ = blood[j];
			}
			for (int j = 15; j > 0; j--) {
				*tbl++ = blood[j];
			}
			*tbl++ = 1;
			tbl += 224;
		}
		*tbl++ = 0;
		for (int j = 0; j < 31; j++) {
			*tbl++ = 1;
		}
		tbl += 224;
	}
	if (IsAnyOf(leveltype, DTYPE_NEST, DTYPE_CRYPT)) {
		tbl = LightTables.data();
		for (int i = 0; i < lights; i++) {
			*tbl++ = 0;
			for (int j = 1; j < 16; j++)
				*tbl++ = j;
			tbl += 240;
		}
		*tbl++ = 0;
		for (int j = 1; j < 16; j++)
			*tbl++ = 1;
		tbl += 240;
	}

	LoadFileInMem("plrgfx\\infra.trn", tbl, 256);
	tbl += 256;

	LoadFileInMem("plrgfx\\stone.trn", tbl, 256);
	tbl += 256;

	for (int i = 0; i < 8; i++) {
		for (uint8_t col = 226; col < 239; col++) {
			if (i != 0 || col != 226) {
				*tbl++ = col;
			} else {
				*tbl++ = 0;
			}
		}
		*tbl++ = 0;
		*tbl++ = 0;
		*tbl++ = 0;
	}
	for (int i = 0; i < 4; i++) {
		uint8_t col = 224;
		for (int j = 224; j < 239; j += 2) {
			*tbl++ = col;
			col += 2;
		}
	}
	for (int i = 0; i < 6; i++) {
		for (uint8_t col = 224; col < 239; col++) {
			*tbl++ = col;
		}
		*tbl++ = 0;
	}

	for (int j = 0; j < 16; j++) {
		for (int i = 0; i < 128; i++) {
			if (i > (j + 1) * 8) {
				lightradius[j][i] = 15;
			} else {
				double fs = (double)15 * i / ((double)8 * (j + 1));
				lightradius[j][i] = static_cast<uint8_t>(fs + 0.5);
			}
		}
	}

	if (IsAnyOf(leveltype, DTYPE_NEST, DTYPE_CRYPT)) {
		for (int j = 0; j < 16; j++) {
			double fa = (sqrt((double)(16 - j))) / 128;
			fa *= fa;
			for (int i = 0; i < 128; i++) {
				lightradius[15 - j][i] = 15 - static_cast<uint8_t>(fa * (double)((128 - i) * (128 - i)));
				if (lightradius[15 - j][i] > 15)
					lightradius[15 - j][i] = 0;
				lightradius[15 - j][i] = lightradius[15 - j][i] - static_cast<uint8_t>((15 - j) / 2);
				if (lightradius[15 - j][i] > 15)
					lightradius[15 - j][i] = 0;
			}
		}
	}
	for (int j = 0; j < 8; j++) {
		for (int i = 0; i < 8; i++) {
			for (int k = 0; k < 16; k++) {
				for (int l = 0; l < 16; l++) {
					int a = (8 * l - j);
					int b = (8 * k - i);
					lightblock[j * 8 + i][k][l] = static_cast<uint8_t>(sqrt(a * a + b * b));
				}
			}
		}
	}
}

#ifdef _DEBUG
void ToggleLighting()
{
	DisableLighting = !DisableLighting;

	if (DisableLighting) {
		memset(dLight, 0, sizeof(dLight));
		return;
	}

	memcpy(dLight, dPreLight, sizeof(dLight));
	for (const Player &player : Players) {
		if (player.plractive && player.isOnActiveLevel()) {
			DoLighting(player.position.tile, player._pLightRad, -1);
		}
	}
}
#endif

void InitLighting()
{
	ActiveLightCount = 0;
	UpdateLighting = false;
	DisableLighting = false;

	for (int i = 0; i < MAXLIGHTS; i++) {
		ActiveLights[i] = i;
	}
}

int AddLight(Point position, int r)
{
	int lid;

	if (DisableLighting) {
		return NO_LIGHT;
	}

	lid = NO_LIGHT;

	if (ActiveLightCount < MAXLIGHTS) {
		lid = ActiveLights[ActiveLightCount++];
		Light &light = Lights[lid];
		light.position.tile = position;
		light._lradius = r;
		light.position.offset = { 0, 0 };
		light._ldel = false;
		light._lunflag = false;
		UpdateLighting = true;
	}

	return lid;
}

void AddUnLight(int i)
{
	if (DisableLighting || i == NO_LIGHT) {
		return;
	}

	Lights[i]._ldel = true;
	UpdateLighting = true;
}

void ChangeLightRadius(int i, int r)
{
	if (DisableLighting || i == NO_LIGHT) {
		return;
	}

	Light &light = Lights[i];
	light._lunflag = true;
	light.position.old = light.position.tile;
	light.oldRadius = light._lradius;
	light._lradius = r;
	UpdateLighting = true;
}

void ChangeLightXY(int i, Point position)
{
	if (DisableLighting || i == NO_LIGHT) {
		return;
	}

	Light &light = Lights[i];
	light._lunflag = true;
	light.position.old = light.position.tile;
	light.oldRadius = light._lradius;
	light.position.tile = position;
	UpdateLighting = true;
}

void ChangeLightOffset(int i, Displacement offset)
{
	if (DisableLighting || i == NO_LIGHT) {
		return;
	}

	Light &light = Lights[i];
	light._lunflag = true;
	light.position.old = light.position.tile;
	light.oldRadius = light._lradius;
	light.position.offset = offset;
	UpdateLighting = true;
}

void ChangeLight(int i, Point position, int r)
{
	if (DisableLighting || i == NO_LIGHT) {
		return;
	}

	Light &light = Lights[i];
	light._lunflag = true;
	light.position.old = light.position.tile;
	light.oldRadius = light._lradius;
	light.position.tile = position;
	light._lradius = r;
	UpdateLighting = true;
}

void ProcessLightList()
{
	if (DisableLighting) {
		return;
	}

	if (UpdateLighting) {
		for (int i = 0; i < ActiveLightCount; i++) {
			Light &light = Lights[ActiveLights[i]];
			if (light._ldel) {
				DoUnLight(light.position.tile.x, light.position.tile.y, light._lradius);
			}
			if (light._lunflag) {
				DoUnLight(light.position.old.x, light.position.old.y, light.oldRadius);
				light._lunflag = false;
			}
		}
		for (int i = 0; i < ActiveLightCount; i++) {
			int j = ActiveLights[i];
			Light &light = Lights[j];
			if (!light._ldel) {
				DoLighting(light.position.tile, light._lradius, j);
			}
		}
		int i = 0;
		while (i < ActiveLightCount) {
			if (Lights[ActiveLights[i]]._ldel) {
				ActiveLightCount--;
				std::swap(ActiveLights[ActiveLightCount], ActiveLights[i]);
			} else {
				i++;
			}
		}
	}

	UpdateLighting = false;
}

void SavePreLighting()
{
	memcpy(dPreLight, dLight, sizeof(dPreLight));
}

void InitVision()
{
	VisionCount = 0;
	dovision = false;
	VisionId = 1;

	for (int i = 0; i < TransVal; i++) {
		TransList[i] = false;
	}
}

int AddVision(Point position, int r, bool mine)
{
	if (VisionCount >= MAXVISION)
		return -1;

	auto &vision = VisionList[VisionCount];
	vision.position.tile = position;
	vision._lradius = r;
	vision._lid = VisionId;
	vision._ldel = false;
	vision._lunflag = false;
	vision._lflags = mine;

	VisionId++;
	VisionCount++;
	dovision = true;

	return vision._lid;
}

void ChangeVisionRadius(int id, int r)
{
	for (int i = 0; i < VisionCount; i++) {
		auto &vision = VisionList[i];
		if (vision._lid != id)
			continue;

		vision._lunflag = true;
		vision.position.old = vision.position.tile;
		vision.oldRadius = vision._lradius;
		vision._lradius = r;
		dovision = true;
	}
}

void ChangeVisionXY(int id, Point position)
{
	for (int i = 0; i < VisionCount; i++) {
		auto &vision = VisionList[i];
		if (vision._lid != id)
			continue;

		vision._lunflag = true;
		vision.position.old = vision.position.tile;
		vision.oldRadius = vision._lradius;
		vision.position.tile = position;
		dovision = true;
	}
}

void ProcessVisionList()
{
	if (!dovision)
		return;

	for (int i = 0; i < VisionCount; i++) {
		auto &vision = VisionList[i];
		if (vision._ldel) {
			DoUnVision(vision.position.tile, vision._lradius);
		}
		if (vision._lunflag) {
			DoUnVision(vision.position.old, vision.oldRadius);
			vision._lunflag = false;
		}
	}
	for (int i = 0; i < TransVal; i++) {
		TransList[i] = false;
	}
	for (int i = 0; i < VisionCount; i++) {
		auto &vision = VisionList[i];
		if (vision._ldel)
			continue;

		MapExplorationType doautomap = MAP_EXP_SELF;
		if (!vision._lflags) {
			doautomap = MAP_EXP_OTHERS;
			for (const Player &player : Players) {
				// Find player for this vision
				if (!player.plractive || !player.isOnActiveLevel() || player._pvid != vision._lid)
					continue;
				// Check that player allows automap sharing
				if (!player.friendlyMode)
					doautomap = MAP_EXP_NONE;
				break;
			}
		}
		DoVision(
		    vision.position.tile,
		    vision._lradius,
		    doautomap,
		    vision._lflags);
	}
	bool delflag;
	do {
		delflag = false;
		for (int i = 0; i < VisionCount; i++) {
			auto &vision = VisionList[i];
			if (!vision._ldel)
				continue;

			VisionCount--;
			if (VisionCount > 0 && i != VisionCount) {
				vision = VisionList[VisionCount];
			}
			delflag = true;
		}
	} while (delflag);

	dovision = false;
}

void lighting_color_cycling()
{
	if (leveltype != DTYPE_HELL) {
		return;
	}

	uint8_t *tbl = LightTables.data();

	for (int j = 0; j < 16; j++) {
		tbl++;
		uint8_t col = *tbl;
		for (int i = 0; i < 30; i++) {
			tbl[0] = tbl[1];
			tbl++;
		}
		*tbl = col;
		tbl += 225;
	}
}

} // namespace devilution

/**
 * @file lighting.cpp
 *
 * Implementation of light and vision.
 */
#include "lighting.h"

#include <algorithm>
#include <cstdint>
#include <numeric>

#include "automap.h"
#include "diablo.h"
#include "engine/load_file.hpp"
#include "engine/points_in_rectangle_range.hpp"
#include "player.h"
#include "utils/attributes.h"

namespace devilution {

std::array<bool, MAXVISION> VisionActive;
Light VisionList[MAXVISION];
Light Lights[MAXLIGHTS];
std::array<uint8_t, MAXLIGHTS> ActiveLights;
int ActiveLightCount;
std::array<std::array<uint8_t, 256>, NumLightingLevels> LightTables;
std::array<uint8_t, 256> InfravisionTable;
std::array<uint8_t, 256> StoneTable;
std::array<uint8_t, 256> PauseTable;
#ifdef _DEBUG
bool DisableLighting;
#endif
bool UpdateLighting;
constexpr size_t NumLightRadiuses2 = 32;
uint8_t LightFalloffs2[NumLightRadiuses2][128 * 2] = {};
uint8_t LightConeInterpolations2[8][8][32][32] = {};

namespace {

constexpr int NumVisionAngles = 72;

// Precalculated cosine used for the vision cone, moving in steps of 5 degrees
// std::cos((i * (360.0f / 72) * PI / 180.0f);
const float VisionDeltaX[NumVisionAngles] = {
	1.0, 0.9961946980917455, 0.984807753012208, 0.9659258262890683, 0.9396926207859084, 0.9063077870366499,
	0.8660254037844387, 0.8191520442889918, 0.766044443118978, 0.7071067811865476, 0.6427876096865394,
	0.5735764363510462, 0.5, 0.42261826174069944, 0.3420201433256688, 0.25881904510252074, 0.17364817766693041,
	0.08715574274765814, 0.0, -0.08715574274765824, -0.1736481776669303, -0.25881904510252085, -0.3420201433256687,
	-0.42261826174069933, -0.5, -0.5735764363510462, -0.6427876096865394, -0.7071067811865475, -0.7660444431189779,
	-0.8191520442889919, -0.8660254037844387, -0.9063077870366499, -0.9396926207859083, -0.9659258262890682,
	-0.984807753012208, -0.9961946980917455, -1.0, -0.9961946980917455, -0.984807753012208, -0.9659258262890683,
	-0.9396926207859084, -0.90630778703665, -0.8660254037844386, -0.8191520442889918, -0.766044443118978,
	-0.7071067811865477, -0.6427876096865395, -0.5735764363510464, -0.5, -0.42261826174069916, -0.34202014332566855,
	-0.25881904510252063, -0.17364817766693033, -0.08715574274765825, -1.8369701987210297e-16, 0.08715574274765789,
	0.17364817766692997, 0.2588190451025203, 0.342020143325669, 0.4226182617406996, 0.5, 0.573576436351046,
	0.6427876096865393, 0.7071067811865474, 0.7660444431189778, 0.8191520442889916, 0.8660254037844384, 0.90630778703665,
	0.9396926207859084, 0.9659258262890683, 0.984807753012208, 0.9961946980917455
};

// Precalculated sine used for the vision cone, moving in steps of 5 degrees
// std::sin((i * (360.0f / 72) * PI / 180.0f);
const float VisionDeltaY[NumVisionAngles] = {
	0.0, 0.08715574274765817, 0.17364817766693033, 0.25881904510252074, 0.3420201433256687, 0.42261826174069944,
	0.5, 0.573576436351046, 0.6427876096865393, 0.7071067811865475, 0.766044443118978, 0.8191520442889918,
	0.8660254037844386, 0.9063077870366499, 0.9396926207859083, 0.9659258262890683, 0.984807753012208,
	0.9961946980917455, 1.0, 0.9961946980917455, 0.984807753012208, 0.9659258262890683, 0.9396926207859084,
	0.90630778703665, 0.8660254037844387, 0.8191520442889917, 0.766044443118978, 0.7071067811865476, 0.6427876096865395,
	0.5735764363510459, 0.5, 0.4226182617406994, 0.3420201433256688, 0.25881904510252074, 0.17364817766693041,
	0.08715574274765814, 0.0, -0.08715574274765824, -0.1736481776669303, -0.25881904510252085, -0.3420201433256687,
	-0.42261826174069933, -0.5, -0.5735764363510462, -0.6427876096865394, -0.7071067811865475, -0.7660444431189779,
	-0.8191520442889919, -0.8660254037844387, -0.9063077870366499, -0.9396926207859083, -0.9659258262890682,
	-0.984807753012208, -0.9961946980917455, -1.0, -0.9961946980917455, -0.984807753012208, -0.9659258262890683,
	-0.9396926207859084, -0.90630778703665, -0.8660254037844386, -0.8191520442889918, -0.766044443118978,
	-0.7071067811865477, -0.6427876096865395, -0.5735764363510464, -0.5, -0.42261826174069916, -0.34202014332566855,
	-0.25881904510252063, -0.17364817766693033, -0.08715574274765825
};

/** @brief Number of supported light radiuses (first radius starts with 0) */
constexpr size_t NumLightRadiuses = 16;
/** Falloff tables for the light cone */
uint8_t LightFalloffs[NumLightRadiuses][128];
bool UpdateVision;
/** interpolations of a 32x32 (16x16 mirrored) light circle moving between tiles in steps of 1/8 of a tile */
uint8_t LightConeInterpolations[8][8][16][16];

void RotateRadius(DisplacementOf<int8_t> &offset, DisplacementOf<int8_t> &dist, DisplacementOf<int8_t> &light, DisplacementOf<int8_t> &block)
{
	dist = { static_cast<int8_t>(7 - dist.deltaY), dist.deltaX };
	light = { static_cast<int8_t>(7 - light.deltaY), light.deltaX };
	offset = { static_cast<int8_t>(dist.deltaX - light.deltaX), static_cast<int8_t>(dist.deltaY - light.deltaY) };

	block.deltaX = 0;
	if (offset.deltaX < 0) {
		offset.deltaX += 8;
		block.deltaX = 1;
	}
	block.deltaY = 0;
	if (offset.deltaY < 0) {
		offset.deltaY += 8;
		block.deltaY = 1;
	}
}

DVL_ALWAYS_INLINE void SetLight(Point position, uint8_t v)
{
	if (LoadingMapObjects)
		dPreLight[position.x][position.y] = v;
	else
		dLight[position.x][position.y] = v;
}

DVL_ALWAYS_INLINE uint8_t GetLight(Point position)
{
	if (LoadingMapObjects)
		return dPreLight[position.x][position.y];

	return dLight[position.x][position.y];
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

void DoUnLight(Point position, uint8_t radius)
{
	radius++;
	radius++; // If lights moved at a diagonal it can result in some extra tiles being lit

	auto searchArea = PointsInRectangle(WorldTileRectangle { position, radius });

	for (WorldTilePosition targetPosition : searchArea) {
		if (InDungeonBounds(targetPosition))
			dLight[targetPosition.x][targetPosition.y] = dPreLight[targetPosition.x][targetPosition.y];
	}
}

void DoLighting(Point position, uint8_t radius, DisplacementOf<int8_t> offset)
{
	assert(radius >= 0 && radius <= NumLightRadiuses);
	assert(InDungeonBounds(position));

	DisplacementOf<int8_t> light = {};
	DisplacementOf<int8_t> block = {};

	if (offset.deltaX < 0) {
		offset.deltaX += 8;
		position -= { 1, 0 };
	}
	if (offset.deltaY < 0) {
		offset.deltaY += 8;
		position -= { 0, 1 };
	}

	DisplacementOf<int8_t> dist = offset;

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

	// Allow for dim lights in crypt and nest
	if (IsAnyOf(leveltype, DTYPE_NEST, DTYPE_CRYPT)) {
		if (GetLight(position) > LightFalloffs[radius][0])
			SetLight(position, LightFalloffs[radius][0]);
	} else {
		SetLight(position, 0);
	}

	for (int i = 0; i < 4; i++) {
		int yBound = i > 0 && i < 3 ? maxY : minY;
		int xBound = i < 2 ? maxX : minX;
		for (int y = 0; y < yBound; y++) {
			for (int x = 1; x < xBound; x++) {
				int linearDistance = LightConeInterpolations[offset.deltaX][offset.deltaY][x + block.deltaX][y + block.deltaY];
				if (linearDistance >= 128)
					continue;
				Point temp = position + (Displacement { x, y }).Rotate(-i);
				uint8_t v = LightFalloffs[radius][linearDistance];
				if (!InDungeonBounds(temp))
					continue;
				if (v < GetLight(temp))
					SetLight(temp, v);
			}
		}
		RotateRadius(offset, dist, light, block);
	}
}

void DoUnVision(Point position, uint8_t radius)
{
	radius++;
	radius++; // increasing the radius even further here prevents leaving stray vision tiles behind and doesn't seem to affect monster AI - applying new vision happens in the same tick

	auto searchArea = PointsInRectangle(WorldTileRectangle { position, radius });

	for (WorldTilePosition targetPosition : searchArea) {
		if (InDungeonBounds(targetPosition))
			dFlags[targetPosition.x][targetPosition.y] &= ~(DungeonFlag::Visible | DungeonFlag::Lit);
	}
}

void DoVision(Point position, uint8_t radius, MapExplorationType doAutomap, bool visible)
{
	DoVisionFlags(position, doAutomap, visible);

	for (int i = 0; i < NumVisionAngles; i++) {
		float dx = VisionDeltaX[i];
		float dy = VisionDeltaY[i];

		for (int distance = 1; distance <= radius; distance++) {
			float x = (position.x + 0.5f) * TILE_WIDTH + dx * distance * TILE_WIDTH;
			float y = (position.y + 0.5f) * TILE_HEIGHT + dy * distance * TILE_HEIGHT;

			int tileX = static_cast<int>(x / TILE_WIDTH);
			int tileY = static_cast<int>(y / TILE_HEIGHT);

			if (!InDungeonBounds({ tileX, tileY }))
				break;

			Point target { tileX, tileY };
			DoVisionFlags(target, doAutomap, visible);

			if (TileHasAny(dPiece[tileX][tileY], TileProperties::BlockLight))
				break;

			int8_t trans = dTransVal[tileX][tileY];
			if (trans != 0)
				TransList[trans] = true;
		}
	}
}

void MakeCustomLightTable()
{
	const float maxDarkness = 15;
	const float maxBrightness = 0;
	for (unsigned radius = 0; radius < NumLightRadiuses2; radius++) {
		const unsigned maxDistance = (radius + 1) * 8; // Compute the maximum effective distance for this radius

		// Define the threshold for starting the falloff (75% of the way)
		const unsigned falloffStartDistance = static_cast<unsigned>(maxDistance * 0.50);

		for (unsigned distance = 0; distance < 256; distance++) {
			if (distance > maxDistance) {
				// Beyond the effective range of the light, set to maximum darkness
				LightFalloffs2[radius][distance] = static_cast<uint8_t>(maxDarkness);
			} else if (distance <= falloffStartDistance) {
				// Within the 75% range where there's no falloff
				LightFalloffs2[radius][distance] = static_cast<uint8_t>(maxBrightness);
			} else {
				// Calculate a smoother falloff for the last 25%
				float factor = static_cast<float>(distance - falloffStartDistance) / (static_cast<float>(maxDistance) - static_cast<float>(falloffStartDistance));
				// Directly scale the darkness based on distance
				float scaled = maxDarkness * factor;
				LightFalloffs2[radius][distance] = static_cast<uint8_t>(std::round(scaled));
			}
		}
	}

	// Generate the light cone interpolations
	for (int offsetY = 0; offsetY < 8; offsetY++) {
		for (int offsetX = 0; offsetX < 8; offsetX++) {
			for (int y = 0; y < 32; y++) {
				for (int x = 0; x < 32; x++) {
					int a = (8 * x - offsetY);
					int b = (8 * y - offsetX);
					LightConeInterpolations2[offsetX][offsetY][x][y] = static_cast<uint8_t>(sqrt(a * a + b * b));
				}
			}
		}
	}
}

void MakeLightTable()
{
	// Generate 16 gradually darker translation tables for doing lighting
	uint8_t shade = 0;
	constexpr uint8_t black = 0;
	constexpr uint8_t white = 255;
	for (auto &lightTable : LightTables) {
		uint8_t colorIndex = 0;
		for (uint8_t steps : { 16, 16, 16, 16, 16, 16, 16, 16, 8, 8, 8, 8, 16, 16, 16, 16, 16, 16 }) {
			const uint8_t shading = shade * steps / 16;
			const uint8_t shadeStart = colorIndex;
			const uint8_t shadeEnd = shadeStart + steps - 1;
			for (uint8_t step = 0; step < steps; step++) {
				if (colorIndex == black) {
					lightTable[colorIndex++] = black;
					continue;
				}
				int color = shadeStart + step + shading;
				if (color > shadeEnd || color == white)
					color = black;
				lightTable[colorIndex++] = color;
			}
		}
		shade++;
	}

	LightTables[15] = {}; // Make last shade pitch black

	if (leveltype == DTYPE_HELL) {
		// Blood wall lighting
		const auto shades = static_cast<int>(LightTables.size() - 1);
		for (int i = 0; i < shades; i++) {
			auto &lightTable = LightTables[i];
			constexpr int range = 16;
			for (int j = 0; j < range; j++) {
				uint8_t color = ((range - 1) << 4) / shades * (shades - i) / range * (j + 1);
				color = 1 + (color >> 4);
				lightTable[j + 1] = color;
				lightTable[31 - j] = color;
			}
		}
	} else if (IsAnyOf(leveltype, DTYPE_NEST, DTYPE_CRYPT)) {
		// Make the lava fully bright
		for (auto &lightTable : LightTables)
			std::iota(lightTable.begin(), lightTable.begin() + 16, uint8_t { 0 });
		LightTables[15][0] = 0;
		std::fill_n(LightTables[15].begin() + 1, 15, 1);
	}

	LoadFileInMem("plrgfx\\infra.trn", InfravisionTable);
	LoadFileInMem("plrgfx\\stone.trn", StoneTable);
	LoadFileInMem("gendata\\pause.trn", PauseTable);

	// Generate light falloffs ranges
	const float maxDarkness = 15;
	const float maxBrightness = 0;
	for (unsigned radius = 0; radius < NumLightRadiuses; radius++) {
		const unsigned maxDistance = (radius + 1) * 8;
		for (unsigned distance = 0; distance < 128; distance++) {
			if (distance > maxDistance) {
				LightFalloffs[radius][distance] = 15;
			} else {
				const float factor = static_cast<float>(distance) / static_cast<float>(maxDistance);
				float scaled;
				if (IsAnyOf(leveltype, DTYPE_NEST, DTYPE_CRYPT)) {
					// quardratic falloff with over exposure
					const float brightness = static_cast<float>(radius) * 1.25F;
					scaled = factor * factor * brightness + (maxDarkness - brightness);
					scaled = std::max(maxBrightness, scaled);
				} else {
					// Leaner falloff
					scaled = factor * maxDarkness;
				}
				LightFalloffs[radius][distance] = static_cast<uint8_t>(scaled + 0.5F); // round up
			}
		}
	}

	// Generate the light cone interpolations
	for (int offsetY = 0; offsetY < 8; offsetY++) {
		for (int offsetX = 0; offsetX < 8; offsetX++) {
			for (int y = 0; y < 16; y++) {
				for (int x = 0; x < 16; x++) {
					int a = (8 * x - offsetY);
					int b = (8 * y - offsetX);
					LightConeInterpolations[offsetX][offsetY][x][y] = static_cast<uint8_t>(sqrt(a * a + b * b));
				}
			}
		}
	}
	MakeCustomLightTable();
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
			DoLighting(player.position.tile, player._pLightRad, {});
		}
	}
}
#endif

void InitLighting()
{
	ActiveLightCount = 0;
	UpdateLighting = false;
	UpdateVision = false;
#ifdef _DEBUG
	DisableLighting = false;
#endif

	std::iota(ActiveLights.begin(), ActiveLights.end(), uint8_t { 0 });
	VisionActive = {};
	TransList = {};
}

int AddLight(Point position, uint8_t radius)
{
#ifdef _DEBUG
	if (DisableLighting)
		return NO_LIGHT;
#endif
	if (ActiveLightCount >= MAXLIGHTS)
		return NO_LIGHT;

	int lid = ActiveLights[ActiveLightCount++];
	Light &light = Lights[lid];
	light.position.tile = position;
	light.radius = radius;
	light.position.offset = { 0, 0 };
	light.isInvalid = false;
	light.hasChanged = false;

	UpdateLighting = true;

	return lid;
}

void AddUnLight(int i)
{
#ifdef _DEBUG
	if (DisableLighting)
		return;
#endif
	if (i == NO_LIGHT)
		return;

	Lights[i].isInvalid = true;

	UpdateLighting = true;
}

void ChangeLightRadius(int i, uint8_t radius)
{
#ifdef _DEBUG
	if (DisableLighting)
		return;
#endif
	if (i == NO_LIGHT)
		return;

	Light &light = Lights[i];
	light.hasChanged = true;
	light.position.old = light.position.tile;
	light.oldRadius = light.radius;
	light.radius = radius;

	UpdateLighting = true;
}

void ChangeLightXY(int i, Point position)
{
#ifdef _DEBUG
	if (DisableLighting)
		return;
#endif
	if (i == NO_LIGHT)
		return;

	Light &light = Lights[i];
	light.hasChanged = true;
	light.position.old = light.position.tile;
	light.oldRadius = light.radius;
	light.position.tile = position;

	UpdateLighting = true;
}

void ChangeLightOffset(int i, DisplacementOf<int8_t> offset)
{
#ifdef _DEBUG
	if (DisableLighting)
		return;
#endif
	if (i == NO_LIGHT)
		return;

	Light &light = Lights[i];
	if (light.position.offset == offset)
		return;

	light.hasChanged = true;
	light.position.old = light.position.tile;
	light.oldRadius = light.radius;
	light.position.offset = offset;

	UpdateLighting = true;
}

void ChangeLight(int i, Point position, uint8_t radius)
{
#ifdef _DEBUG
	if (DisableLighting)
		return;
#endif
	if (i == NO_LIGHT)
		return;

	Light &light = Lights[i];
	light.hasChanged = true;
	light.position.old = light.position.tile;
	light.oldRadius = light.radius;
	light.position.tile = position;
	light.radius = radius;

	UpdateLighting = true;
}

void ProcessLightList()
{
#ifdef _DEBUG
	if (DisableLighting)
		return;
#endif
	if (!UpdateLighting)
		return;
	for (int i = 0; i < ActiveLightCount; i++) {
		Light &light = Lights[ActiveLights[i]];
		if (light.isInvalid) {
			DoUnLight(light.position.tile, light.radius);
		}
		if (light.hasChanged) {
			DoUnLight(light.position.old, light.oldRadius);
			light.hasChanged = false;
		}
	}
	for (int i = 0; i < ActiveLightCount; i++) {
		const Light &light = Lights[ActiveLights[i]];
		if (light.isInvalid) {
			ActiveLightCount--;
			std::swap(ActiveLights[ActiveLightCount], ActiveLights[i]);
			i--;
			continue;
		}
		if (TileHasAny(dPiece[light.position.tile.x][light.position.tile.y], TileProperties::Solid))
			continue; // Monster hidden in a wall, don't spoil the surprise
		DoLighting(light.position.tile, light.radius, light.position.offset);
	}

	UpdateLighting = false;
}

void SavePreLighting()
{
	memcpy(dPreLight, dLight, sizeof(dPreLight));
}

void ActivateVision(Point position, int r, size_t id)
{
	auto &vision = VisionList[id];
	vision.position.tile = position;
	vision.radius = r;
	vision.isInvalid = false;
	vision.hasChanged = false;
	VisionActive[id] = true;

	UpdateVision = true;
}

void ChangeVisionRadius(size_t id, int r)
{
	auto &vision = VisionList[id];
	vision.hasChanged = true;
	vision.position.old = vision.position.tile;
	vision.oldRadius = vision.radius;
	vision.radius = r;
	UpdateVision = true;
}

void ChangeVisionXY(size_t id, Point position)
{
	auto &vision = VisionList[id];
	vision.hasChanged = true;
	vision.position.old = vision.position.tile;
	vision.oldRadius = vision.radius;
	vision.position.tile = position;
	UpdateVision = true;
}

void ProcessVisionList()
{
	if (!UpdateVision)
		return;

	TransList = {};

	for (const Player &player : Players) {
		const size_t id = player.getId();
		if (!VisionActive[id])
			continue;
		Light &vision = VisionList[id];
		if (!player.plractive || !player.isOnActiveLevel()) {
			DoUnVision(vision.position.tile, vision.radius);
			VisionActive[id] = false;
			continue;
		}
		if (vision.hasChanged) {
			DoUnVision(vision.position.old, vision.oldRadius);
			vision.hasChanged = false;
		}
	}
	for (const Player &player : Players) {
		const size_t id = player.getId();
		if (!VisionActive[id])
			continue;
		Light &vision = VisionList[id];
		MapExplorationType doautomap = MAP_EXP_SELF;
		if (&player != MyPlayer)
			doautomap = player.friendlyMode ? MAP_EXP_OTHERS : MAP_EXP_NONE;
		DoVision(
		    vision.position.tile,
		    vision.radius,
		    doautomap,
		    &player == MyPlayer);
	}

	UpdateVision = false;
}

void lighting_color_cycling()
{
	for (auto &lightTable : LightTables) {
		// shift elements between indexes 1-31 to left
		std::rotate(lightTable.begin() + 1, lightTable.begin() + 2, lightTable.begin() + 32);
	}
}

} // namespace devilution

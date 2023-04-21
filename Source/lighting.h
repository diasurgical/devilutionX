/**
 * @file lighting.h
 *
 * Interface of light and vision.
 */
#pragma once

#include <array>
#include <vector>

#include <function_ref.hpp>

#include "automap.h"
#include "engine.h"
#include "engine/point.hpp"
#include "utils/attributes.h"
#include "utils/stdcompat/invoke_result_t.hpp"
#include "utils/stdcompat/optional.hpp"

namespace devilution {

#define MAXLIGHTS 32
#define MAXVISION 32
/** @brief Number of supported light levels */
constexpr size_t NumLightingLevels = 16;
#define NO_LIGHT -1

struct LightPosition {
	WorldTilePosition tile;
	/** Pixel offset from tile. */
	DisplacementOf<int8_t> offset;
	/** Prevous position. */
	WorldTilePosition old;
};

struct Light {
	LightPosition position;
	uint8_t radius;
	uint8_t oldRadius;
	bool isInvalid;
	bool hasChanged;
	int _lid;
	bool _lflags;
};

extern Light VisionList[MAXVISION];
extern int VisionCount;
extern int VisionId;
extern Light Lights[MAXLIGHTS];
extern uint8_t ActiveLights[MAXLIGHTS];
extern int ActiveLightCount;
constexpr char LightsMax = 15;
extern std::array<std::array<uint8_t, 256>, NumLightingLevels> LightTables;
extern std::array<uint8_t, 256> InfravisionTable;
extern std::array<uint8_t, 256> StoneTable;
extern std::array<uint8_t, 256> PauseTable;
extern bool DisableLighting;
extern bool UpdateLighting;

void DoLighting(Point position, uint8_t radius, int Lnum);
void DoUnVision(Point position, uint8_t radius);
void DoVision(Point position, uint8_t radius, MapExplorationType doAutomap, bool visible);
void MakeLightTable();
#ifdef _DEBUG
void ToggleLighting();
#endif
void InitLighting();
int AddLight(Point position, uint8_t radius);
void AddUnLight(int i);
void ChangeLightRadius(int i, uint8_t radius);
void ChangeLightXY(int i, Point position);
void ChangeLightOffset(int i, Displacement offset);
void ChangeLight(int i, Point position, uint8_t radius);
void ProcessLightList();
void SavePreLighting();
int AddVision(Point position, int r, bool mine);
void ChangeVisionRadius(int id, int r);
void ChangeVisionXY(int id, Point position);
void ProcessVisionList();
void lighting_color_cycling();

constexpr int MaxCrawlRadius = 18;

/**
 * CrawlTable specifies X- and Y-coordinate deltas from a missile target coordinate.
 *
 * n=4
 *
 *    y
 *    ^
 *    |  1
 *    | 3#4
 *    |  2
 *    +-----> x
 *
 * n=16
 *
 *    y
 *    ^
 *    |  314
 *    | B7 8C
 *    | F # G
 *    | D9 AE
 *    |  526
 *    +-------> x
 */

bool DoCrawl(unsigned radius, tl::function_ref<bool(Displacement)> function);
bool DoCrawl(unsigned minRadius, unsigned maxRadius, tl::function_ref<bool(Displacement)> function);

template <typename F>
auto Crawl(unsigned radius, F function) -> invoke_result_t<decltype(function), Displacement>
{
	invoke_result_t<decltype(function), Displacement> result;
	DoCrawl(radius, [&result, &function](Displacement displacement) -> bool {
		result = function(displacement);
		return !result;
	});
	return result;
}

template <typename F>
auto Crawl(unsigned minRadius, unsigned maxRadius, F function) -> invoke_result_t<decltype(function), Displacement>
{
	invoke_result_t<decltype(function), Displacement> result;
	DoCrawl(minRadius, maxRadius, [&result, &function](Displacement displacement) -> bool {
		result = function(displacement);
		return !result;
	});
	return result;
}

} // namespace devilution

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
/** 16 light levels + infravision + stone curse + red for pause/death screen */
#define LIGHTSIZE (19 * 256)
#define NO_LIGHT -1

struct LightPosition {
	Point tile;
	/** Pixel offset from tile. */
	Displacement offset;
	/** Prevous position. */
	Point old;
};

struct Light {
	LightPosition position;
	int _lradius;
	int _lid;
	bool _ldel;
	bool _lunflag;
	int oldRadius;
	bool _lflags;
};

extern Light VisionList[MAXVISION];
extern int VisionCount;
extern int VisionId;
extern Light Lights[MAXLIGHTS];
extern uint8_t ActiveLights[MAXLIGHTS];
extern int ActiveLightCount;
constexpr char LightsMax = 15;
extern std::array<uint8_t, LIGHTSIZE> LightTables;
extern bool DisableLighting;
extern bool UpdateLighting;

void DoLighting(Point position, int nRadius, int Lnum);
void DoUnVision(Point position, int nRadius);
void DoVision(Point position, int radius, MapExplorationType doAutomap, bool visible);
void MakeLightTable();
#ifdef _DEBUG
void ToggleLighting();
#endif
void InitLighting();
int AddLight(Point position, int r);
void AddUnLight(int i);
void ChangeLightRadius(int i, int r);
void ChangeLightXY(int i, Point position);
void ChangeLightOffset(int i, Displacement offset);
void ChangeLight(int i, Point position, int r);
void ProcessLightList();
void SavePreLighting();
void InitVision();
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

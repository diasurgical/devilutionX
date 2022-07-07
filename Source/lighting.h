/**
 * @file lighting.h
 *
 * Interface of light and vision.
 */
#pragma once

#include <array>
#include <vector>

#include "automap.h"
#include "engine.h"
#include "engine/point.hpp"
#include "miniwin/miniwin.h"
#include "utils/attributes.h"
#include "utils/stdcompat/invoke_result_t.hpp"

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
void DoVision(Point position, int nRadius, MapExplorationType doautomap, bool visible);
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

template <typename F>
auto CrawlFlipsX(Displacement mirrored, F function) -> invoke_result_t<decltype(function), Displacement>
{
	const Displacement Flips[] = { mirrored.flipX(), mirrored };
	for (auto displacement : Flips) {
		auto ret = function(displacement);
		if (ret)
			return ret;
	}
	return {};
}

template <typename F>
auto CrawlFlipsY(Displacement mirrored, F function) -> invoke_result_t<decltype(function), Displacement>
{
	const Displacement Flips[] = { mirrored, mirrored.flipY() };
	for (auto displacement : Flips) {
		auto ret = function(displacement);
		if (ret)
			return ret;
	}
	return {};
}

template <typename F>
auto CrawlFlipsXY(Displacement mirrored, F function) -> invoke_result_t<decltype(function), Displacement>
{
	const Displacement Flips[] = { mirrored.flipX(), mirrored, mirrored.flipXY(), mirrored.flipY() };
	for (auto displacement : Flips) {
		auto ret = function(displacement);
		if (ret)
			return ret;
	}
	return {};
}

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

template <typename F>
auto Crawl(unsigned radius, F function) -> invoke_result_t<decltype(function), Displacement>
{
	assert(radius <= MaxCrawlRadius);

	if (radius == 0)
		return function(Displacement { 0, 0 });

	auto ret = CrawlFlipsY({ 0, static_cast<int>(radius) }, function);
	if (ret)
		return ret;
	for (unsigned i = 1; i < radius; i++) {
		ret = CrawlFlipsXY({ static_cast<int>(i), static_cast<int>(radius) }, function);
		if (ret)
			return ret;
	}
	if (radius > 1) {
		ret = CrawlFlipsXY({ static_cast<int>(radius) - 1, static_cast<int>(radius) - 1 }, function);
		if (ret)
			return ret;
	}
	ret = CrawlFlipsX({ static_cast<int>(radius), 0 }, function);
	if (ret)
		return ret;
	for (unsigned i = 1; i < radius; i++) {
		ret = CrawlFlipsXY({ static_cast<int>(radius), static_cast<int>(i) }, function);
		if (ret)
			return ret;
	}
	return {};
}

template <typename F>
auto Crawl(unsigned minRadius, unsigned maxRadius, F function) -> invoke_result_t<decltype(function), Displacement>
{
	for (unsigned i = minRadius; i <= maxRadius; i++) {
		auto displacement = Crawl(i, function);
		if (displacement)
			return displacement;
	}
	return {};
}

/* rdata */

extern const uint8_t VisionCrawlTable[23][30];

} // namespace devilution

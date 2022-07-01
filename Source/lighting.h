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

/* rdata */

constexpr size_t CrawlTableSize = 19;
extern DVL_API_FOR_TEST const std::array<std::vector<DisplacementOf<int8_t>>, CrawlTableSize> CrawlTable;
extern const uint8_t VisionCrawlTable[23][30];

} // namespace devilution

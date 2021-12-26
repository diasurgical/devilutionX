/**
 * @file lighting.h
 *
 * Interface of light and vision.
 */
#pragma once

#include <array>

#include "automap.h"
#include "engine.h"
#include "engine/point.hpp"
#include "miniwin/miniwin.h"
#include "utils/attributes.h"

namespace devilution {

#define MAXLIGHTS 32
#define MAXVISION 32
#define LIGHTSIZE (27 * 256)
#define NO_LIGHT -1

struct LightPosition {
	Point tile;
	/** Pixel offset from tile. */
	Point offset;
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
extern char LightsMax;
extern std::array<uint8_t, LIGHTSIZE> LightTables;
extern DVL_API_FOR_TEST bool DisableLighting;
extern bool UpdateLighting;

void DoLighting(Point position, int nRadius, int Lnum);
void DoUnVision(Point position, int nRadius);
void DoVision(Point position, int nRadius, MapExplorationType doautomap, bool visible);
void MakeLightTable();
#ifdef _DEBUG
void ToggleLighting();
#endif
void InitLightMax();
void InitLighting();
int AddLight(Point position, int r);
void AddUnLight(int i);
void ChangeLightRadius(int i, int r);
void ChangeLightXY(int i, Point position);
void ChangeLightOffset(int i, Point position);
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

extern DVL_API_FOR_TEST const int8_t CrawlTable[2749];
extern DVL_API_FOR_TEST const int CrawlNum[19];
extern const uint8_t VisionCrawlTable[23][30];

} // namespace devilution

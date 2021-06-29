/**
 * @file lighting.h
 *
 * Interface of light and vision.
 */
#pragma once

#include <array>

#include "engine.h"
#include "engine/point.hpp"
#include "miniwin/miniwin.h"

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

struct LightListStruct {
	LightPosition position;
	int _lradius;
	int _lid;
	bool _ldel;
	bool _lunflag;
	int oldRadius;
	bool _lflags;
};

extern LightListStruct VisionList[MAXVISION];
extern uint8_t lightactive[MAXLIGHTS];
extern LightListStruct LightList[MAXLIGHTS];
extern int numlights;
extern int numvision;
extern char lightmax;
extern bool dolighting;
extern int visionid;
extern std::array<BYTE, LIGHTSIZE> pLightTbl;
extern bool lightflag;

void DoLighting(Point position, int nRadius, int Lnum);
void DoUnVision(Point position, int nRadius);
void DoVision(Point position, int nRadius, bool doautomap, bool visible);
void FreeLightTable();
void InitLightTable();
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
void ChangeLightOff(int i, Point position);
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

extern const char CrawlTable[2749];
extern const BYTE vCrawlTable[23][30];

} // namespace devilution

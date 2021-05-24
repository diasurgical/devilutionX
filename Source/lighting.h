/**
 * @file lighting.h
 *
 * Interface of light and vision.
 */
#pragma once

#include <array>

#include "engine.h"
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
	int oldRadious;
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

void DoLighting(int nXPos, int nYPos, int nRadius, int Lnum);
void DoUnVision(int nXPos, int nYPos, int nRadius);
void DoVision(int nXPos, int nYPos, int nRadius, bool doautomap, bool visible);
void FreeLightTable();
void InitLightTable();
void MakeLightTable();
#ifdef _DEBUG
void ToggleLighting();
#endif
void InitLightMax();
void InitLighting();
int AddLight(int x, int y, int r);
void AddUnLight(int i);
void ChangeLightRadius(int i, int r);
void ChangeLightXY(int i, Point position);
void ChangeLightOff(int i, int x, int y);
void ChangeLight(int i, int x, int y, int r);
void ProcessLightList();
void SavePreLighting();
void InitVision();
int AddVision(int x, int y, int r, bool mine);
void ChangeVisionRadius(int id, int r);
void ChangeVisionXY(int id, int x, int y);
void ProcessVisionList();
void lighting_color_cycling();

/* rdata */

extern const char CrawlTable[2749];
extern const BYTE vCrawlTable[23][30];

} // namespace devilution

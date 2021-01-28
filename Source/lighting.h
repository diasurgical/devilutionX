/**
 * @file lighting.h
 *
 * Interface of light and vision.
 */
#ifndef __LIGHTING_H__
#define __LIGHTING_H__

DEVILUTION_BEGIN_NAMESPACE

#ifdef __cplusplus
extern "C" {
#endif

typedef struct LightListStruct {
	int _lx;
	int _ly;
	int _lradius;
	int _lid;
	int _ldel;
	int _lunflag;
	int field_18;
	int _lunx;
	int _luny;
	int _lunr;
	int _xoff;
	int _yoff;
	int _lflags;
} LightListStruct;

extern LightListStruct VisionList[MAXVISION];
extern BYTE lightactive[MAXLIGHTS];
extern LightListStruct LightList[MAXLIGHTS];
extern int numlights;
extern BYTE lightradius[16][128];
extern BOOL dovision;
extern int numvision;
extern char lightmax;
extern BOOL dolighting;
extern int visionid;
extern BYTE *pLightTbl;
extern BOOL lightflag;

void DoLighting(int nXPos, int nYPos, int nRadius, int Lnum);
void DoUnVision(int nXPos, int nYPos, int nRadius);
void DoVision(int nXPos, int nYPos, int nRadius, BOOL doautomap, BOOL visible);
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
void ChangeLightXY(int i, int x, int y);
void ChangeLightOff(int i, int x, int y);
void ChangeLight(int i, int x, int y, int r);
void ProcessLightList();
void SavePreLighting();
void InitVision();
int AddVision(int x, int y, int r, BOOL mine);
void ChangeVisionRadius(int id, int r);
void ChangeVisionXY(int id, int x, int y);
void ProcessVisionList();
void lighting_color_cycling();

/* rdata */

extern char CrawlTable[2749];
extern BYTE vCrawlTable[23][30];

#ifdef __cplusplus
}
#endif

DEVILUTION_END_NAMESPACE

#endif /* __LIGHTING_H__ */

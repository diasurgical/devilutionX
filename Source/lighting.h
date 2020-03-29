/**
 * @file lighting.h
 *
 * Interface of light and vision.
 */
#ifndef __LIGHTING_H__
#define __LIGHTING_H__

extern LightListStruct VisionList[MAXVISION];
extern BYTE lightactive[MAXLIGHTS];
extern LightListStruct LightList[MAXLIGHTS];
extern int numlights;
extern BYTE lightradius[16][128];
extern BOOL dovision;
extern int numvision;
extern char lightmax;
extern BOOL dolighting;
extern BYTE lightblock[64][16][16];
extern int visionid;
extern BYTE *pLightTbl;
extern BOOL lightflag;

void RotateRadius(int *x, int *y, int *dx, int *dy, int *lx, int *ly, int *bx, int *by);
void DoLighting(int nXPos, int nYPos, int nRadius, int Lnum);
void DoUnLight(int nXPos, int nYPos, int nRadius);
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
extern char *pCrawlTable[19];
extern BYTE vCrawlTable[23][30];
extern BYTE byte_49463C[18][18];
extern BYTE RadiusAdj[23];

#endif /* __LIGHTING_H__ */

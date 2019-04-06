//HEADER_GOES_HERE
#ifndef __LIGHTING_H__
#define __LIGHTING_H__

extern LightListStruct VisionList[MAXVISION];
extern unsigned char lightactive[MAXLIGHTS];
extern LightListStruct LightList[MAXLIGHTS];
extern int numlights;
extern BYTE lightradius[16][128];
extern int dovision;               // weak
extern int numvision;
extern char lightmax;             // weak
extern int dolighting;            // weak
extern BYTE lightblock[8][8][16][16];
extern int visionid;
extern BYTE *pLightTbl;
extern BOOL lightflag;

void __fastcall RotateRadius(int *x, int *y, int *dx, int *dy, int *lx, int *ly, int *bx, int *by);
void __fastcall DoLighting(int nXPos, int nYPos, int nRadius, int Lnum);
void __fastcall DoUnLight(int nXPos, int nYPos, int nRadius);
void __fastcall DoUnVision(int nXPos, int nYPos, int nRadius);
void __fastcall DoVision(int nXPos, int nYPos, int nRadius, BOOL doautomap, BOOL visible);
void __cdecl FreeLightTable();
void __cdecl InitLightTable();
void __cdecl MakeLightTable();
#ifdef _DEBUG
void __cdecl ToggleLighting_2();
void __cdecl ToggleLighting();
#endif
void __cdecl InitLightMax();
void __cdecl InitLighting();
int __fastcall AddLight(int x, int y, int r);
void __fastcall AddUnLight(int i);
void __fastcall ChangeLightRadius(int i, int r);
void __fastcall ChangeLightXY(int i, int x, int y);
void __fastcall ChangeLightOff(int i, int x, int y);
void __fastcall ChangeLight(int i, int x, int y, int r);
void __cdecl ProcessLightList();
void __cdecl SavePreLighting();
void __cdecl InitVision();
int __fastcall AddVision(int x, int y, int r, BOOL mine);
void __fastcall ChangeVisionRadius(int id, int r);
void __fastcall ChangeVisionXY(int id, int x, int y);
void __cdecl ProcessVisionList();
void __cdecl lighting_color_cycling();

/* rdata */

extern char CrawlTable[2749];
extern char *pCrawlTable[19];
extern unsigned char vCrawlTable[23][30];
extern unsigned char byte_49463C[18][18];
extern unsigned char RadiusAdj[23];

#endif /* __LIGHTING_H__ */

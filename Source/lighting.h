//HEADER_GOES_HERE
#ifndef __LIGHTING_H__
#define __LIGHTING_H__

extern LightListStruct VisionList[32];
extern char lightactive[32];
extern LightListStruct LightList[32];
extern int numlights;
extern char dung_map_radius[2048]; /* char [16][128] */
extern int dovision; // weak
extern int numvision;
extern char lightmax; // weak
extern int dolighting; // weak
extern char dung_map_rgba[16384]; /* int [64][64] short [64][128] char [64][256] */
extern int visionid;
extern char *pLightTbl; /* todo: struct? */
extern BOOL lightflag;

void __fastcall SetLightFX(int *x, int *y, short *s_r, short *s_g, int *s_b, int *d_r, int *d_g, int *d_b);
void __fastcall DoLighting(int nXPos, int nYPos, int nRadius, int Lnum);
void __fastcall DoUnLight(int nXPos, int nYPos, int nRadius);
void __fastcall DoUnVision(int nXPos, int nYPos, int nRadius);
void __fastcall DoVision(int nXPos, int nYPos, int nRadius, unsigned char doautomap, int visible);
void __cdecl FreeLightTable();
void __cdecl InitLightTable();
void __cdecl MakeLightTable();
#ifdef _DEBUG
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

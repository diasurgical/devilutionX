//HEADER_GOES_HERE
#ifndef __AUTOMAP_H__
#define __AUTOMAP_H__

extern short automaptype[512];
extern int AMdword_4B7E40; // weak
extern int AMdword_4B7E44; // weak
extern bool automapflag; // idb
extern char AMbyte_4B7E4C[32];
extern char automapview[DMAXX][DMAXY];
extern int AutoMapScale; // idb
extern int AutoMapXOfs; // weak
extern int AutoMapYOfs; // weak
extern int AutoMapPosBits; // weak
extern int AutoMapXPos; // weak
extern int AutoMapYPos; // weak
extern int AMPlayerX; // weak
extern int AMPlayerY; // weak

void __cdecl InitAutomapOnce();
void __cdecl InitAutomap();
void __cdecl StartAutomap();
void __cdecl AutomapUp();
void __cdecl AutomapDown();
void __cdecl AutomapLeft();
void __cdecl AutomapRight();
void __cdecl AutomapZoomIn();
void __cdecl AutomapZoomOut();
void __cdecl DrawAutomap();
void __fastcall DrawAutomapType(int screen_x, int screen_y, short automap_type);
void __cdecl DrawAutomapPlr();
short __fastcall GetAutomapType(int x, int y, BOOL view);
void __cdecl DrawAutomapGame();
void __fastcall SetAutomapView(int x, int y);
void __cdecl AutomapZoomReset();

#endif /* __AUTOMAP_H__ */

//HEADER_GOES_HERE
#ifndef __AUTOMAP_H__
#define __AUTOMAP_H__

extern WORD automaptype[512];
extern BOOL automapflag;
extern char AmShiftTab[32];
extern BOOLEAN automapview[DMAXX][DMAXY];
extern int AutoMapScale;
extern int AutoMapXOfs;
extern int AutoMapYOfs;
extern int AutoMapPosBits;
extern int AutoMapXPos;
extern int AutoMapYPos;
extern int AMPlayerX;
extern int AMPlayerY;

void InitAutomapOnce();
void InitAutomap();
void StartAutomap();
void AutomapUp();
void AutomapDown();
void AutomapLeft();
void AutomapRight();
void AutomapZoomIn();
void AutomapZoomOut();
void DrawAutomap();
void DrawAutomapType(int screen_x, int screen_y, WORD automap_type);
void DrawAutomapPlr();
WORD GetAutomapType(int x, int y, BOOL view);
void DrawAutomapGame();
void SetAutomapView(int x, int y);
void AutomapZoomReset();

#endif /* __AUTOMAP_H__ */

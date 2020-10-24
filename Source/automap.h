/**
 * @file automap.h
 *
 * Interface of the in-game map overlay.
 */
#ifndef __AUTOMAP_H__
#define __AUTOMAP_H__

DEVILUTION_BEGIN_NAMESPACE

#ifdef __cplusplus
extern "C" {
#endif

extern WORD automaptype[512];
extern BOOL automapflag;
extern char AmShiftTab[32];
extern BOOLEAN automapview[DMAXX][DMAXY];
extern int AutoMapScale;
extern int AutoMapXOfs;
extern int AutoMapYOfs;
extern int AmLine64;
extern int AmLine32;
extern int AmLine16;
extern int AmLine8;
extern int AmLine4;

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
void DrawAutomapTile(int screen_x, int screen_y, WORD automap_type);
#ifdef HELLFIRE
void SearchAutomapItem();
void DrawAutomapItem(int x, int y, BYTE color);
#endif
void DrawAutomapPlr();
WORD GetAutomapType(int x, int y, BOOL view);
void DrawAutomapText();
void SetAutomapView(int x, int y);
void AutomapZoomReset();

#ifdef __cplusplus
}
#endif

DEVILUTION_END_NAMESPACE

#endif /* __AUTOMAP_H__ */

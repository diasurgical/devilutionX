/**
 * @file qol.h
 *
 * Quality of life features
 */
#ifndef __QOL_H__
#define __QOL_H__

DEVILUTION_BEGIN_NAMESPACE

extern bool altPressed;
extern bool drawXPBar;
extern bool drawHPBar;
extern bool isGeneratingLabels;
extern bool isLabelHighlighted;

extern BYTE *qolbuff;

void QOLCopyBytes(const void *src, const int n, void *dst);
void QOLCopyInt(const void *src, void *dst);
void QOLCopyInts(const void *src, const int n, void *dst);

void DrawMonsterHealthBar();
void DrawXPBar();
void UpdateLabels(BYTE *dst, int width);
void GenerateLabelOffsets();
void AddItemToDrawQueue(int x, int y, int id);
void HighlightItemsNameOnMap();
void diablo_parse_config();
void SaveHotkeys();
void LoadHotkeys();
void RepeatClicks();
void AutoPickGold(int pnum);

DEVILUTION_END_NAMESPACE

#endif /* __QOL_H__ */

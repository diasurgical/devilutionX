/**
 * @file qol.h
 *
 * Quality of life features
 */
#ifndef __QOL_H__
#define __QOL_H__

DEVILUTION_BEGIN_NAMESPACE

extern int drawMinX;
extern int drawMaxX;
extern bool altPressed;
extern bool isGeneratingLabels;
extern bool isLabelHighlighted;

void DrawMonsterHealthBar();
void DrawXPBar();
void AutoGoldPickup(int pnum);
void UpdateLabels(BYTE *dst, int width);
void GenerateLabelOffsets();
void AddItemToDrawQueue(int x, int y, int id);
void HighlightItemsNameOnMap();
void SaveHotkeys();
void RepeatClicks();
void AutoPickGold(int pnum);

DEVILUTION_END_NAMESPACE

#endif /* __QOL_H__ */

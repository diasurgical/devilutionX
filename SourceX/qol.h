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
extern int highlightItemsMode;

void DrawMonsterHealthBar(int monsterID);
void DrawXPBar();
void AddItemToDrawQueue(int x, int y, int id);
void HighlightItemsNameOnMap();
void diablo_parse_config();
void SaveHotkeys();
void LoadHotkeys();
DEVILUTION_END_NAMESPACE

#endif /* __QOL_H__ */

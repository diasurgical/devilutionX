/**
 * @file control.h
 *
 * Interface of the character and main control panels
 */
#ifndef __CONTROL_H__
#define __CONTROL_H__

#include "engine.h"

DEVILUTION_BEGIN_NAMESPACE

#ifdef __cplusplus
extern "C" {
#endif

typedef struct RECT32 {
	int x;
	int y;
	int w;
	int h;
} RECT32;

extern BOOL drawhpflag;
extern BOOL dropGoldFlag;
extern BOOL chrbtn[4];
extern BOOL lvlbtndown;
extern int dropGoldValue;
extern BOOL drawmanaflag;
extern BOOL chrbtnactive;
extern BYTE *pPanelText;
extern int pnumlines;
extern BOOL pinfoflag;
extern int pSpell;
extern char infoclr;
extern char tempstr[256];
extern int sbooktab;
extern int pSplType;
extern int initialDropGoldIndex;
extern BOOL talkflag;
extern BOOL sbookflag;
extern BOOL chrflag;
extern BOOL drawbtnflag;
extern char infostr[64];
extern BOOL panelflag;
extern int initialDropGoldValue;
extern BOOL panbtndown;
extern BOOL spselflag;

void DrawSpellList();
void SetSpell();
void SetSpeedSpell(int slot);
void ToggleSpell(int slot);

/**
 * @brief Print letter to the given buffer
 * @param out The buffer to print to
 * @param sx Backbuffer offset
 * @param sy Backbuffer offset
 * @param nCel Number of letter in Windows-1252
 * @param col text_color color value
 */
void PrintChar(CelOutputBuffer out, int sx, int sy, int nCel, char col);

void AddPanelString(const char *str, BOOL just);
void ClearPanel();
void DrawPanelBox(CelOutputBuffer out, int x, int y, int w, int h, int sx, int sy);
void DrawLifeFlask();

/**
 * Controls the drawing of the area of the life flask within the control panel.
 * First sets the fill amount then draws the empty flask cel portion then the filled
 * flask portion.
 */
void UpdateLifeFlask(CelOutputBuffer out);

void DrawManaFlask();
void control_update_life_mana();

/**
 * Controls the drawing of the area of the life flask within the control panel.
 * Also for some reason draws the current right mouse button spell.
 */
void UpdateManaFlask(CelOutputBuffer out);

void InitControlPan();
void DrawCtrlPan(CelOutputBuffer out);

/**
 * Draws the control panel buttons in their current state. If the button is in the default
 * state draw it from the panel cel(extract its sub-rect). Else draw it from the buttons cel.
 */
void DrawCtrlBtns(CelOutputBuffer out);

void DoSpeedBook();
void DoPanBtn();
void control_check_btn_press();
void DoAutoMap();
void CheckPanelInfo();
void CheckBtnUp();
void FreeControlPan();
BOOL control_WriteStringToBuffer(BYTE *str);

/**
 * Sets a string to be drawn in the info box and then draws it.
 */
void DrawInfoBox(CelOutputBuffer out);

void PrintGameStr(CelOutputBuffer out, int x, int y, const char *str, int color);
void DrawChr(CelOutputBuffer out);
void CheckLvlBtn();
void ReleaseLvlBtn();
void DrawLevelUpIcon(CelOutputBuffer out);
void CheckChrBtns();
void ReleaseChrBtns(bool addAllStatPoints);
void DrawDurIcon();
void RedBack();
void DrawSpellBook(CelOutputBuffer out);
void CheckSBook();
const char *get_pieces_str(int nGold);
void DrawGoldSplit(CelOutputBuffer out, int amount);
void control_drop_gold(char vkey);
void control_remove_gold(int pnum, int gold_index);
void control_set_gold_curs(int pnum);
void DrawTalkPan(CelOutputBuffer out);
BOOL control_check_talk_btn();
void control_release_talk_btn();
void control_type_message();
void control_reset_talk();
BOOL control_talk_last_key(int vkey);
BOOL control_presskeys(int vkey);

/* rdata */
extern const BYTE fontframe[128];
extern const BYTE fontkern[68];
extern const BYTE gbFontTransTbl[256];

/* data */

extern RECT32 ChrBtnsRect[4];

#ifdef __cplusplus
}
#endif

DEVILUTION_END_NAMESPACE

#endif /* __CONTROL_H__ */

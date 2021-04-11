/**
 * @file control.h
 *
 * Interface of the character and main control panels
 */
#pragma once

#include <stdint.h>

#include "engine.h"
#include "spelldat.h"
#include "spells.h"

namespace devilution {

enum text_color : uint8_t {
	COL_WHITE,
	COL_BLUE,
	COL_RED,
	COL_GOLD,
	COL_BLACK,
};

struct RECT32 {
	int x;
	int y;
	int w;
	int h;
};

extern bool drawhpflag;
extern bool dropGoldFlag;
extern bool chrbtn[4];
extern bool lvlbtndown;
extern int dropGoldValue;
extern bool drawmanaflag;
extern bool chrbtnactive;
extern BYTE *pPanelText;
extern int pnumlines;
extern bool pinfoflag;
extern spell_id pSpell;
extern text_color infoclr;
extern char tempstr[256];
extern int sbooktab;
extern spell_type pSplType;
extern int initialDropGoldIndex;
extern bool talkflag;
extern bool sbookflag;
extern bool chrflag;
extern bool drawbtnflag;
extern char infostr[64];
extern bool panelflag;
extern int initialDropGoldValue;
extern bool panbtndown;
extern bool spselflag;

void DrawSpellList(CelOutputBuffer out);
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
void PrintChar(CelOutputBuffer out, int sx, int sy, int nCel, text_color col);

void AddPanelString(const char *str, bool just);
void ClearPanel();
void DrawPanelBox(CelOutputBuffer out, int x, int y, int w, int h, int sx, int sy);

/**
 * Draws the top dome of the life flask (that part that protrudes out of the control panel).
 * First it draws the empty flask cel and then draws the filled part on top if needed.
 */
void DrawLifeFlask(CelOutputBuffer out);

/**
 * Controls the drawing of the area of the life flask within the control panel.
 * First sets the fill amount then draws the empty flask cel portion then the filled
 * flask portion.
 */
void UpdateLifeFlask(CelOutputBuffer out);

void DrawManaFlask(CelOutputBuffer out);
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
bool control_WriteStringToBuffer(BYTE *str);

/**
 * Sets a string to be drawn in the info box and then draws it.
 */
void DrawInfoBox(CelOutputBuffer out);

void PrintGameStr(CelOutputBuffer out, int x, int y, const char *str, text_color color);
void DrawChr(CelOutputBuffer out);
void CheckLvlBtn();
void ReleaseLvlBtn();
void DrawLevelUpIcon(CelOutputBuffer out);
void CheckChrBtns();
void ReleaseChrBtns(bool addAllStatPoints);
void DrawDurIcon(CelOutputBuffer out);
void RedBack(CelOutputBuffer out);
void DrawSpellBook(CelOutputBuffer out);
void CheckSBook();
const char *get_pieces_str(int nGold);
void DrawGoldSplit(CelOutputBuffer out, int amount);
void control_drop_gold(char vkey);
void control_remove_gold(int pnum, int gold_index);
void control_set_gold_curs(int pnum);
void DrawTalkPan(CelOutputBuffer out);
bool control_check_talk_btn();
void control_release_talk_btn();
void control_type_message();
void control_reset_talk();
bool control_talk_last_key(int vkey);
bool control_presskeys(int vkey);

/* rdata */
extern const BYTE fontframe[128];
extern const BYTE fontkern[68];
extern const BYTE gbFontTransTbl[256];

/* data */

extern RECT32 ChrBtnsRect[4];

}

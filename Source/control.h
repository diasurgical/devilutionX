/**
 * @file control.h
 *
 * Interface of the character and main control panels
 */
#pragma once

#include <cstdint>

#include "engine.h"
#include "engine/render/text_render.hpp"
#include "spelldat.h"
#include "spells.h"
#include "utils/ui_fwd.h"
#include "utils/stdcompat/optional.hpp"

namespace devilution {

#define PANEL_WIDTH 640
#define PANEL_HEIGHT 128
#define PANEL_TOP (gnScreenHeight - PANEL_HEIGHT)
#define PANEL_LEFT (gnScreenWidth - PANEL_WIDTH) / 2
#define PANEL_X PANEL_LEFT
#define PANEL_Y PANEL_TOP

#define SPANEL_WIDTH 320
#define SPANEL_HEIGHT 352
#define RIGHT_PANEL (gnScreenWidth - SPANEL_WIDTH)
#define RIGHT_PANEL_X RIGHT_PANEL

extern bool drawhpflag;
extern bool dropGoldFlag;
extern bool chrbtn[4];
extern bool lvlbtndown;
extern int dropGoldValue;
extern bool drawmanaflag;
extern bool chrbtnactive;
extern int pnumlines;
extern bool pinfoflag;
extern spell_id pSpell;
extern uint16_t infoclr;
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

/**
 * @brief Check if the UI can cover the game area entierly
 */
inline bool CanPanelsCoverView()
{
	return gnScreenWidth <= PANEL_WIDTH && gnScreenHeight <= SPANEL_HEIGHT + PANEL_HEIGHT;
}

void DrawSpellList(const CelOutputBuffer &out);
void SetSpell();
void SetSpeedSpell(int slot);
void ToggleSpell(int slot);

void AddPanelString(const char *str);
void ClearPanel();
void DrawPanelBox(const CelOutputBuffer &out, SDL_Rect srcRect, Point targetPosition);

/**
 * Draws the top dome of the life flask (that part that protrudes out of the control panel).
 * First it draws the empty flask cel and then draws the filled part on top if needed.
 */
void DrawLifeFlask(const CelOutputBuffer &out);

/**
 * Controls the drawing of the area of the life flask within the control panel.
 * First sets the fill amount then draws the empty flask cel portion then the filled
 * flask portion.
 */
void UpdateLifeFlask(const CelOutputBuffer &out);

void DrawManaFlask(const CelOutputBuffer &out);
void control_update_life_mana();

/**
 * Controls the drawing of the area of the life flask within the control panel.
 * Also for some reason draws the current right mouse button spell.
 */
void UpdateManaFlask(const CelOutputBuffer &out);

void InitControlPan();
void DrawCtrlPan(const CelOutputBuffer &out);

/**
 * Draws the control panel buttons in their current state. If the button is in the default
 * state draw it from the panel cel(extract its sub-rect). Else draw it from the buttons cel.
 */
void DrawCtrlBtns(const CelOutputBuffer &out);

void DoSpeedBook();
void DoPanBtn();
void control_check_btn_press();
void DoAutoMap();
void CheckPanelInfo();
void CheckBtnUp();
void FreeControlPan();

/**
 * Sets a string to be drawn in the info box and then draws it.
 */
void DrawInfoBox(const CelOutputBuffer &out);
void DrawChr(const CelOutputBuffer &out);
void CheckLvlBtn();
void ReleaseLvlBtn();
void DrawLevelUpIcon(const CelOutputBuffer &out);
void CheckChrBtns();
void ReleaseChrBtns(bool addAllStatPoints);
void DrawDurIcon(const CelOutputBuffer &out);
void RedBack(const CelOutputBuffer &out);
void DrawSpellBook(const CelOutputBuffer &out);
void CheckSBook();
void DrawGoldSplit(const CelOutputBuffer &out, int amount);
void control_drop_gold(char vkey);
void control_remove_gold(int pnum, int goldIndex);
void DrawTalkPan(const CelOutputBuffer &out);
bool control_check_talk_btn();
void control_release_talk_btn();
void control_type_message();
void control_reset_talk();
bool control_talk_last_key(int vkey);
bool control_presskeys(int vkey);

extern Rectangle ChrBtnsRect[4];

} // namespace devilution

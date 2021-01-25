/**
 * @file control.h
 *
 * Interface of the character and main control panels
 */
#ifndef __CONTROL_H__
#define __CONTROL_H__

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
extern BOOL panbtn[8];
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
extern BOOLEAN whisper[MAX_PLRS];
extern int sbooktab;
extern int pSplType;
extern int initialDropGoldIndex;
extern BOOL talkflag;
extern BOOL sbookflag;
extern BOOL chrflag;
extern BOOL drawbtnflag;
extern char infostr[256];
extern char panelstr[4][64];
extern BOOL panelflag;
extern int initialDropGoldValue;
extern BOOL panbtndown;
extern BOOL spselflag;

void DrawSpellList();
void SetSpell();
void SetSpeedSpell(int slot);
void ToggleSpell(int slot);
void PrintChar(int sx, int sy, int nCel, char col);
void AddPanelString(const char *str, BOOL just);
void ClearPanel();
void DrawPanelBox(int x, int y, int w, int h, int sx, int sy);
void DrawLifeFlask();
void UpdateLifeFlask();
void DrawManaFlask();
void control_update_life_mana();
void UpdateManaFlask();
void InitControlPan();
void DrawCtrlPan();
void DrawCtrlBtns();
void DoSpeedBook();
void DoPanBtn();
void control_check_btn_press();
void DoAutoMap();
void CheckPanelInfo();
void CheckBtnUp();
void FreeControlPan();
BOOL control_WriteStringToBuffer(BYTE *str);
void DrawInfoBox();
void PrintGameStr(int x, int y, const char *str, int color);
void DrawChr();
void CheckLvlBtn();
void ReleaseLvlBtn();
void DrawLevelUpIcon();
void CheckChrBtns();
void ReleaseChrBtns();
void DrawDurIcon();
void RedBack();
void DrawSpellBook();
void CheckSBook();
const char *get_pieces_str(int nGold);
void DrawGoldSplit(int amount);
void control_drop_gold(char vkey);
void control_remove_gold(int pnum, int gold_index);
void control_set_gold_curs(int pnum);
void DrawTalkPan();
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

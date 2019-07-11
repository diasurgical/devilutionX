//HEADER_GOES_HERE
#ifndef __CONTROL_H__
#define __CONTROL_H__

extern BYTE *pDurIcons;
extern BYTE *pChrButtons;
extern BOOL drawhpflag; // idb
extern BOOL dropGoldFlag;
extern int panbtn[8];
extern int chrbtn[4];
extern BYTE *pMultiBtns;
extern BYTE *pPanelButtons;
extern BYTE *pChrPanel;
extern int lvlbtndown;    // weak
extern int dropGoldValue; // idb
extern BOOL drawmanaflag; // idb
extern BOOL chrbtnactive;
extern BYTE *pPanelText;
extern int nGoldFrame;
extern BYTE *pLifeBuff;
extern BYTE *pBtmBuff;
extern BYTE *pTalkBtns;
extern int pstrjust[4];
extern int pnumlines; // idb
extern BOOL pinfoflag;
extern BOOL talkbtndown[3];
extern int pSpell; // weak
extern BYTE *pManaBuff;
extern char infoclr; // weak
extern BYTE *pGBoxBuff;
extern BYTE *pSBkBtnCel;
extern char tempstr[256];
extern BOOLEAN whisper[MAX_PLRS];
extern int sbooktab;             // weak
extern int pSplType;             // weak
extern int frame;                // idb
extern int initialDropGoldIndex; // idb
extern int talkflag;             // weak
extern BYTE *pSBkIconCels;
extern int sbookflag; // weak
extern int chrflag;
extern BOOL drawbtnflag;
extern BYTE *pSpellBkCel;
extern char infostr[MAX_PATH];
extern int numpanbtns; // weak
extern BYTE *pStatusPanel;
extern char panelstr[256];
extern int panelflag; // weak
extern unsigned char SplTransTbl[256];
extern int initialDropGoldValue; // idb
extern BYTE *pSpellCels;
extern BOOL panbtndown;
extern BYTE *pTalkPanel;
extern int spselflag; // weak

void DrawSpellCel(int xp, int yp, BYTE *Trans, int nCel, int w);
void SetSpellTrans(char t);
void DrawSpell();
void DrawSpellList();
void SetSpell();
void SetSpeedSpell(int slot);
void ToggleSpell(int slot);
void CPrintString(int nOffset, int nCel, char col);
void AddPanelString(char *str, BOOL just);
void ClearPanel();
void DrawPanelBox(int x, int y, int w, int h, int sx, int sy);
void InitPanelStr();
void SetFlaskHeight(BYTE *pCelBuff, int min, int max, int c, int r);
void DrawFlask(BYTE *pCelBuff, int w, int nSrcOff, BYTE *pBuff, int nDstOff, int h);
void DrawLifeFlask();
void UpdateLifeFlask();
void DrawManaFlask();
void control_update_life_mana();
void UpdateManaFlask();
void InitControlPan();
void ClearCtrlPan();
void DrawCtrlPan();
void DoSpeedBook();
void DoPanBtn();
void control_set_button_down(int btn_id);
void control_check_btn_press();
void DoAutoMap();
void CheckPanelInfo();
void CheckBtnUp();
void FreeControlPan();
BOOL control_WriteStringToBuffer(BYTE *str);
void DrawInfoBox();
void control_draw_info_str();
void control_print_info_str(int y, char *str, BOOL center, int lines);
void PrintGameStr(int x, int y, char *str, int color);
void DrawChr();
void ADD_PlrStringXY(int x, int y, int width, char *pszStr, char col);
void MY_PlrStringXY(int x, int y, int width, char *pszStr, char col, int base);
void CheckLvlBtn();
void ReleaseLvlBtn();
void DrawLevelUpIcon();
void CheckChrBtns();
void ReleaseChrBtns();
void DrawDurIcon();
int DrawDurIcon4Item(ItemStruct *pItem, int x, int c);
void RedBack();
char GetSBookTrans(int ii, BOOL townok);
void DrawSpellBook();
void PrintSBookStr(int x, int y, BOOL cjustflag, char *pszStr, char col);
void CheckSBook();
char *get_pieces_str(int nGold);
void DrawGoldSplit(int amount);
void control_drop_gold(char vkey);
void control_remove_gold(int pnum, int gold_index);
void control_set_gold_curs(int pnum);
void DrawTalkPan();
char *control_print_talk_msg(char *msg, int x, int y, int *a4, int just);
BOOL control_check_talk_btn();
void control_release_talk_btn();
void control_reset_talk_msg();
void control_type_message();
void control_reset_talk();
BOOL control_talk_last_key(int vkey);
BOOL control_presskeys(int vkey);
void control_press_enter();
void control_up_down(int v);

/* rdata */
extern const unsigned char fontframe[128];
extern const unsigned char fontkern[68];
extern const int lineoffset[25];
extern const unsigned char gbFontTransTbl[256];

/* data */

extern char SpellITbl[MAX_SPELLS];
extern int PanBtnPos[8][5];
extern char *PanBtnHotKey[8];
extern char *PanBtnStr[8];
extern RECT32 attribute_inc_rects[4];
extern int SpellPages[6][7];

#endif /* __CONTROL_H__ */

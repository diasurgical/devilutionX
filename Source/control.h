//HEADER_GOES_HERE
#ifndef __CONTROL_H__
#define __CONTROL_H__

extern char sgbNextTalkSave; // weak
extern char sgbTalkSavePos; // weak
extern void *pDurIcons;
extern void *pChrButtons;
extern int drawhpflag; // idb
extern int dropGoldFlag; // weak
extern int panbtn[8];
extern int chrbtn[4];
extern void *pMultiBtns;
extern void *pPanelButtons;
extern void *pChrPanel;
extern int lvlbtndown; // weak
extern char sgszTalkSave[8][80];
extern int dropGoldValue; // idb
extern int drawmanaflag; // idb
extern int chrbtnactive; // weak
extern char sgszTalkMsg[80];
extern void *pPanelText;
extern int frame_4B8800; // idb
extern void *pLifeBuff;
extern void *pBtmBuff;
extern void *pTalkBtns;
extern int pstrjust[4];
extern int pnumlines; // idb
extern int pinfoflag; // weak
extern int talkbtndown[3];
extern int pSpell; // weak
extern void *pManaBuff;
extern int infoclr; // weak
extern int sgbPlrTalkTbl; // weak // should be char [4]
extern void *pGBoxBuff;
extern void *pSBkBtnCel;
extern char tempstr[260];
extern int sbooktab; // weak
extern int pSplType; // weak
extern int frame; // idb
extern int initialDropGoldIndex; // idb
extern int talkflag; // weak
extern void *pSBkIconCels;
extern int sbookflag; // weak
extern int chrflag;
extern int drawbtnflag; // idb
extern void *pSpellBkCel;
extern char infostr[260];
extern int numpanbtns; // weak
extern void *pStatusPanel;
extern char panelstr[256];
extern int panelflag; // weak
extern char byte_4B8B88[256];
extern int initialDropGoldValue; // idb
extern void *pSpellCels;
extern int panbtndown; // weak
extern void *pTalkPanel; // idb
extern int spselflag; // weak

void __fastcall DrawSpellCel(int xp, int yp, char *Trans, int nCel, int w);
void __fastcall SetSpellTrans(char t);
void __cdecl DrawSpell();
void __cdecl DrawSpellList();
void __cdecl SetSpell();
void __fastcall SetSpeedSpell(int slot);
void __fastcall ToggleSpell(int slot);
void __fastcall CPrintString(int No, unsigned char pszStr, int Just); /* check arg names */
void __fastcall AddPanelString(char *str, int just);
void __cdecl ClearPanel();
void __fastcall DrawPanelBox(int x, int y, int w, int h, int sx, int sy);
void __cdecl InitPanelStr();
void __fastcall SetFlaskHeight(char *buf, int min, int max, int c, int r);
void __fastcall DrawFlask(void *a1, int a2, int a3, void *a4, int a5, int a6);
void __cdecl DrawLifeFlask();
void __cdecl UpdateLifeFlask();
void __cdecl DrawManaFlask();
void __cdecl control_update_life_mana();
void __cdecl UpdateManaFlask();
void __cdecl InitControlPan();
void __cdecl ClearCtrlPan();
void __cdecl DrawCtrlPan();
void __cdecl DoSpeedBook();
void __cdecl DoPanBtn();
void __fastcall control_set_button_down(int btn_id);
void __cdecl control_check_btn_press();
void __cdecl DoAutoMap();
void __cdecl CheckPanelInfo();
void __cdecl CheckBtnUp();
void __cdecl FreeControlPan();
int __fastcall control_WriteStringToBuffer(char *str);
void __cdecl DrawInfoBox();
void __fastcall control_print_info_str(int y, char *str, bool center, int lines);
void __fastcall PrintGameStr(int x, int y, char *str, int color);
void __cdecl DrawChr();
void __fastcall ADD_PlrStringXY(int x, int y, int width, char *pszStr, char col);
void __fastcall MY_PlrStringXY(int x, int y, int width, char *pszStr, char col, int base);
void __cdecl CheckLvlBtn();
void __cdecl ReleaseLvlBtn();
void __cdecl DrawLevelUpIcon();
void __cdecl CheckChrBtns();
void __cdecl ReleaseChrBtns();
void __cdecl DrawDurIcon();
int __fastcall DrawDurIcon4Item(ItemStruct *pItem, int x, int c);
void __cdecl RedBack();
int __fastcall GetSBookTrans(int ii, unsigned char townok);
void __cdecl DrawSpellBook();
void __fastcall PrintSBookStr(int x, int y, bool cjustflag, char *pszStr, int bright);
void __cdecl CheckSBook();
char *__fastcall get_pieces_str(int nGold);
void __fastcall DrawGoldSplit(int amount);
void __fastcall control_drop_gold(int vkey);
void __fastcall control_remove_gold(int pnum, int gold_index);
void __fastcall control_set_gold_curs(int pnum);
void __cdecl DrawTalkPan();
char *__fastcall control_print_talk_msg(char *msg, int x, int y, int *a4, int just);
int __cdecl control_check_talk_btn();
void __cdecl control_release_talk_btn();
void __cdecl control_reset_talk_msg();
void __cdecl control_type_message();
void __cdecl control_reset_talk();
int __fastcall control_talk_last_key(int a1);
int __fastcall control_presskeys(int a1);
void __cdecl control_press_enter();
void __fastcall control_up_down(char a1);

/* rdata */
extern const unsigned char fontframe[127];
extern const unsigned char fontkern[68];
extern const int lineoffset[25];
extern const unsigned char fontidx[256];

/* data */

extern unsigned char SpellITbl[37];
extern int PanBtnPos[8][5];
extern char *PanBtnHotKey[8];
extern char *PanBtnStr[8];
extern int attribute_inc_rects[4][4];
extern int SpellPages[6][7];

#endif /* __CONTROL_H__ */

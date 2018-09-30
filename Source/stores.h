//HEADER_GOES_HERE
#ifndef __STORES_H__
#define __STORES_H__

extern int stextup; // weak
extern int storenumh; // weak
extern int stextlhold; // weak
extern ItemStruct boyitem;
extern int stextshold; // idb
extern ItemStruct premiumitem[6];
extern void *pSTextBoxCels;
extern int premiumlevel; // idb
extern int talker; // weak
extern STextStruct stext[24];
extern char stextsize; // weak
extern int stextsmax; // weak
extern int InStoreFlag; // idb
extern ItemStruct storehold[48];
extern int gossipstart; // weak
extern ItemStruct witchitem[20];
extern int stextscrl; // weak
extern int numpremium; // idb
extern ItemStruct healitem[20];
extern ItemStruct golditem;
extern char storehidx[48];
extern void *pSTextSlidCels;
extern int stextvhold; // weak
extern int stextsel; // weak
extern char stextscrldbtn; // weak
extern int gossipend; // weak
extern void *pCelBuff;
extern int stextsval; // idb
extern int boylevel; // weak
extern ItemStruct smithitem[20];
extern int stextdown; // weak
extern char stextscrlubtn; // weak
extern char stextflag; // weak

void __cdecl InitStores();
void __cdecl SetupTownStores();
void __cdecl FreeStoreMem();
void __cdecl DrawSTextBack();
void __fastcall PrintSString(int x, int y, unsigned char cjustflag, char *str, int col, int val);
void __fastcall DrawSLine(int y);
void __fastcall DrawSArrows(int y1, int y2);
void __cdecl DrawSTextHelp();
void __fastcall ClearSText(int s, int e);
void __fastcall AddSLine(int y);
void __fastcall AddSTextVal(int y, int val);
void __fastcall OffsetSTextY(int y, int yo);
void __fastcall AddSText(int x, int y, unsigned char j, char *str, int clr, int sel);
void __cdecl StoreAutoPlace();
void __cdecl S_StartSmith();
void __fastcall S_ScrollSBuy(int idx);
void __fastcall PrintStoreItem(ItemStruct *x, int l, char iclr);
void __cdecl S_StartSBuy();
void __fastcall S_ScrollSPBuy(int idx);
bool __cdecl S_StartSPBuy();
bool __fastcall SmithSellOk(int i);
void __fastcall S_ScrollSSell(int idx);
void __cdecl S_StartSSell();
bool __fastcall SmithRepairOk(int i);
void __cdecl S_StartSRepair();
void __fastcall AddStoreHoldRepair(ItemStruct *itm, int i);
void __cdecl S_StartWitch();
void __fastcall S_ScrollWBuy(int idx);
void __cdecl S_StartWBuy();
bool __fastcall WitchSellOk(int i);
void __cdecl S_StartWSell();
bool __fastcall WitchRechargeOk(int i);
void __fastcall AddStoreHoldRecharge(ItemStruct itm, int i);
void __cdecl S_StartWRecharge();
void __cdecl S_StartNoMoney();
void __cdecl S_StartNoRoom();
void __cdecl S_StartConfirm();
void __cdecl S_StartBoy();
void __cdecl S_StartBBoy();
void __cdecl S_StartHealer();
void __fastcall S_ScrollHBuy(int idx);
void __cdecl S_StartHBuy();
void __cdecl S_StartStory();
bool __fastcall IdItemOk(ItemStruct *i);
void __fastcall AddStoreHoldId(ItemStruct itm, int i);
void __cdecl S_StartSIdentify();
void __cdecl S_StartIdShow();
void __cdecl S_StartTalk();
void __cdecl S_StartTavern();
void __cdecl S_StartBarMaid();
void __cdecl S_StartDrunk();
void __fastcall StartStore(char s);
void __cdecl DrawSText();
void __cdecl STextESC();
void __cdecl STextUp();
void __cdecl STextDown();
void __cdecl STextPrior();
void __cdecl STextNext();
void __cdecl S_SmithEnter();
void __fastcall SetGoldCurs(int pnum, int i);
void __fastcall SetSpdbarGoldCurs(int pnum, int i);
void __fastcall TakePlrsMoney(int cost);
void __cdecl SmithBuyItem();
void __cdecl S_SBuyEnter();
void __cdecl SmithBuyPItem();
void __cdecl S_SPBuyEnter();
bool __fastcall StoreGoldFit(int idx);
void __fastcall PlaceStoreGold(int v);
void __cdecl StoreSellItem();
void __cdecl S_SSellEnter();
void __cdecl SmithRepairItem();
void __cdecl S_SRepairEnter();
void __cdecl S_WitchEnter();
void __cdecl WitchBuyItem();
void __cdecl S_WBuyEnter();
void __cdecl S_WSellEnter();
void __cdecl WitchRechargeItem();
void __cdecl S_WRechargeEnter();
void __cdecl S_BoyEnter();
void __cdecl BoyBuyItem();
void __cdecl HealerBuyItem();
void __cdecl S_BBuyEnter();
void __cdecl StoryIdItem();
void __cdecl S_ConfirmEnter();
void __cdecl S_HealerEnter();
void __cdecl S_HBuyEnter();
void __cdecl S_StoryEnter();
void __cdecl S_SIDEnter();
void __cdecl S_TalkEnter();
void __cdecl S_TavernEnter();
void __cdecl S_BarmaidEnter();
void __cdecl S_DrunkEnter();
void __cdecl STextEnter();
void __cdecl CheckStoreBtn();
void __cdecl ReleaseStoreBtn();

/* rdata */

extern int SStringY[24];
extern char *talkname[9];

#endif /* __STORES_H__ */

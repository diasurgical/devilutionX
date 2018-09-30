//HEADER_GOES_HERE
#ifndef __ITEMS_H__
#define __ITEMS_H__

extern int itemactive[MAXITEMS];
extern int uitemflag;
extern int itemavail[MAXITEMS];
extern ItemStruct curruitem;
extern ItemGetRecordStruct itemrecord[MAXITEMS];
extern ItemStruct item[MAXITEMS+1];
extern BOOL itemhold[3][3];
extern unsigned char *Item2Frm[35];
extern int UniqueItemFlag[128];
extern int numitems;
extern int gnNumGetRecords;

void __cdecl InitItemGFX();
bool __fastcall ItemPlace(int xp, int yp);
void __cdecl AddInitItems();
void __cdecl InitItems();
void __fastcall CalcPlrItemVals(int p, BOOL Loadgfx);
void __fastcall CalcPlrScrolls(int p);
void __fastcall CalcPlrStaff(int pnum);
void __fastcall CalcSelfItems(int pnum);
void __fastcall CalcPlrItemMin(int pnum);
bool __fastcall ItemMinStats(PlayerStruct *p, ItemStruct *x);
void __fastcall CalcPlrBookVals(int p);
void __fastcall CalcPlrInv(int p, BOOL Loadgfx);
void __fastcall SetPlrHandItem(ItemStruct *h, int idata);
void __fastcall GetPlrHandSeed(ItemStruct *h);
void __fastcall GetGoldSeed(int pnum, ItemStruct *h);
void __fastcall SetPlrHandSeed(ItemStruct *h, int iseed);
void __fastcall SetPlrHandGoldCurs(ItemStruct *h);
void __fastcall CreatePlrItems(int p);
BOOL __fastcall ItemSpaceOk(int i, int j);
bool __fastcall GetItemSpace(int x, int y, char inum);
void __fastcall GetSuperItemSpace(int x, int y, char inum);
void __fastcall GetSuperItemLoc(int x, int y, int *xx, int *yy);
void __fastcall CalcItemValue(int i);
void __fastcall GetBookSpell(int i, int lvl);
void __fastcall GetStaffPower(int i, int lvl, int bs, unsigned char onlygood);
void __fastcall GetStaffSpell(int i, int lvl, unsigned char onlygood);
void __fastcall GetItemAttrs(int i, int idata, int lvl);
int __fastcall RndPL(int param1, int param2);
int __fastcall PLVal(int pv, int p1, int p2, int minv, int maxv);
void __fastcall SaveItemPower(int i, int power, int param1, int param2, int minval, int maxval, int multval);
void __fastcall GetItemPower(int i, int minlvl, int maxlvl, int flgs, int onlygood);
void __fastcall GetItemBonus(int i, int idata, int minlvl, int maxlvl, int onlygood);
void __fastcall SetupItem(int i);
int __fastcall RndItem(int m);
int __fastcall RndUItem(int m);
int __cdecl RndAllItems();
int __fastcall RndTypeItems(int itype, int imid);
int __fastcall CheckUnique(int i, int lvl, int uper, bool recreate);
void __fastcall GetUniqueItem(int i, int uid);
void __fastcall SpawnUnique(int uid, int x, int y);
void __fastcall ItemRndDur(int ii);
void __fastcall SetupAllItems(int ii, int idx, int iseed, int lvl, int uper, int onlygood, int recreate, int pregen);
void __fastcall SpawnItem(int m, int x, int y, unsigned char sendmsg);
void __fastcall CreateItem(int uid, int x, int y);
void __fastcall CreateRndItem(int x, int y, unsigned char onlygood, unsigned char sendmsg, int delta);
void __fastcall SetupAllUseful(int ii, int iseed, int lvl);
void __fastcall CreateRndUseful(int pnum, int x, int y, unsigned char sendmsg);
void __fastcall CreateTypeItem(int x, int y, unsigned char onlygood, int itype, int imisc, int sendmsg, int delta);
void __fastcall RecreateItem(int ii, int idx, unsigned short ic, int iseed, int ivalue);
void __fastcall RecreateEar(int ii, unsigned short ic, int iseed, unsigned char Id, int dur, int mdur, int ch, int mch, int ivalue, int ibuff);
void __fastcall SpawnQuestItem(int itemid, int x, int y, int randarea, int selflag);
void __cdecl SpawnRock();
void __fastcall RespawnItem(int i, bool FlipFlag);
void __fastcall DeleteItem(int ii, int i);
void __cdecl ItemDoppel();
void __cdecl ProcessItems();
void __cdecl FreeItemGFX();
void __fastcall GetItemFrm(int i);
void __fastcall GetItemStr(int i);
void __fastcall CheckIdentify(int pnum, int cii);
void __fastcall DoRepair(int pnum, int cii);
void __fastcall RepairItem(ItemStruct *i, int lvl);
void __fastcall DoRecharge(int pnum, int cii);
void __fastcall RechargeItem(ItemStruct *i, int r);
void __fastcall PrintItemOil(char IDidx);
void __fastcall PrintItemPower(char plidx, ItemStruct *x);
void __cdecl DrawUBack();
void __fastcall PrintUString(int x, int y, int cjustflag, char *str, int col);
void __fastcall DrawULine(int y);
void __cdecl DrawUniqueInfo();
void __fastcall PrintItemMisc(ItemStruct *x);
void __fastcall PrintItemDetails(ItemStruct *x);
void __fastcall PrintItemDur(ItemStruct *x);
void __fastcall UseItem(int p, int Mid, int spl);
bool __fastcall StoreStatOk(ItemStruct *h);
bool __fastcall SmithItemOk(int i);
int __fastcall RndSmithItem(int lvl);
void __fastcall BubbleSwapItem(ItemStruct *a, ItemStruct *b);
void __cdecl SortSmith();
void __fastcall SpawnSmith(int lvl);
bool __fastcall PremiumItemOk(int i);
int __fastcall RndPremiumItem(int minlvl, int maxlvl);
void __fastcall SpawnOnePremium(int i, int plvl);
void __fastcall SpawnPremium(int lvl);
bool __fastcall WitchItemOk(int i);
int __fastcall RndWitchItem(int lvl);
void __cdecl SortWitch();
void __fastcall WitchBookLevel(int ii);
void __fastcall SpawnWitch(int lvl);
int __fastcall RndBoyItem(int lvl);
void __fastcall SpawnBoy(int lvl);
bool __fastcall HealerItemOk(int i);
int __fastcall RndHealerItem(int lvl);
void __cdecl SortHealer();
void __fastcall SpawnHealer(int lvl);
void __cdecl SpawnStoreGold();
void __fastcall RecreateSmithItem(int ii, int idx, int plvl, int iseed);
void __fastcall RecreatePremiumItem(int ii, int idx, int lvl, int iseed);
void __fastcall RecreateBoyItem(int ii, int idx, int lvl, int iseed);
void __fastcall RecreateWitchItem(int ii, int idx, int lvl, int iseed);
void __fastcall RecreateHealerItem(int ii, int idx, int lvl, int iseed);
void __fastcall RecreateTownItem(int ii, int idx, unsigned short icreateinfo, int iseed, int ivalue);
void __cdecl RecalcStoreStats();
int __cdecl ItemNoFlippy();
void __fastcall CreateSpellBook(int x, int y, int ispell, bool sendmsg, int delta);
void __fastcall CreateMagicItem(int x, int y, int imisc, int icurs, int sendmsg, int delta);
bool __fastcall GetItemRecord(int dwSeed, int CI, int indx);
void __fastcall NextItemRecord(int i);
void __fastcall SetItemRecord(int dwSeed, int CI, int indx);
void __fastcall PutItemRecord(int seed, int ci, int index);

/* rdata */

extern const PLStruct PL_Prefix[84];
extern const PLStruct PL_Suffix[96];
extern const UItemStruct UniqueItemList[91];

/* data */


extern ItemDataStruct AllItemsList[157];
extern unsigned char ItemCAnimTbl[169];
extern char *ItemDropStrs[35];
extern unsigned char ItemAnimLs[35];
extern int ItemDropSnds[35];
extern int ItemInvSnds[35];
extern int idoppely; // weak
extern int premiumlvladd[6];

#endif /* __ITEMS_H__ */

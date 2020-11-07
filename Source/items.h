/**
 * @file items.h
 *
 * Interface of item functionality.
 */
#ifndef __ITEMS_H__
#define __ITEMS_H__

DEVILUTION_BEGIN_NAMESPACE

#ifdef __cplusplus
extern "C" {
#endif

extern int itemactive[MAXITEMS];
extern BOOL uitemflag;
extern int itemavail[MAXITEMS];
extern ItemGetRecordStruct itemrecord[MAXITEMS];
extern ItemStruct item[MAXITEMS + 1];
#ifdef HELLFIRE
extern CornerStoneStruct CornerStone;
#endif
extern BOOL UniqueItemFlag[128];
#ifdef HELLFIRE
extern int auricGold;
#endif
extern int numitems;

#ifdef HELLFIRE
int items_4231CA(int i);
int items_423230(int i);
int items_423296(int i);
int items_4232FC(int i);
int items_423362(int i);
int items_4233C8(int i);
int items_42342E(int i);
int items_4234B2(int i);
int items_423518(int i);
int items_42357E(int i);
#endif
void InitItemGFX();
void InitItems();
void CalcPlrItemVals(int p, BOOL Loadgfx);
void CalcPlrScrolls(int p);
void CalcPlrStaff(int p);
void CalcPlrInv(int p, BOOL Loadgfx);
void SetPlrHandItem(ItemStruct *h, int idata);
void GetPlrHandSeed(ItemStruct *h);
void GetGoldSeed(int pnum, ItemStruct *h);
void SetPlrHandGoldCurs(ItemStruct *h);
void CreatePlrItems(int p);
BOOL ItemSpaceOk(int i, int j);
void GetSuperItemLoc(int x, int y, int *xx, int *yy);
void GetItemAttrs(int i, int idata, int lvl);
void SaveItemPower(int i, int power, int param1, int param2, int minval, int maxval, int multval);
void GetItemPower(int i, int minlvl, int maxlvl, int flgs, BOOL onlygood);
void SetupItem(int i);
int RndItem(int m);
void SpawnUnique(int uid, int x, int y);
void SpawnItem(int m, int x, int y, BOOL sendmsg);
void CreateItem(int uid, int x, int y);
void CreateRndItem(int x, int y, BOOL onlygood, BOOL sendmsg, BOOL delta);
void CreateRndUseful(int pnum, int x, int y, BOOL sendmsg);
void CreateTypeItem(int x, int y, BOOL onlygood, int itype, int imisc, BOOL sendmsg, BOOL delta);
void RecreateItem(int ii, int idx, WORD icreateinfo, int iseed, int ivalue);
void RecreateEar(int ii, WORD ic, int iseed, int Id, int dur, int mdur, int ch, int mch, int ivalue, int ibuff);
#ifdef HELLFIRE
void items_427A72();
void items_427ABA(int x, int y);
#endif
void SpawnQuestItem(int itemid, int x, int y, int randarea, int selflag);
void SpawnRock();
#ifdef HELLFIRE
void SpawnRewardItem(int itemid, int xx, int yy);
void SpawnMapOfDoom(int xx, int yy);
void SpawnRuneBomb(int xx, int yy);
void SpawnTheodore(int xx, int yy);
#endif
void RespawnItem(int i, BOOL FlipFlag);
void DeleteItem(int ii, int i);
void ProcessItems();
void FreeItemGFX();
void GetItemFrm(int i);
void GetItemStr(int i);
void CheckIdentify(int pnum, int cii);
void DoRepair(int pnum, int cii);
void DoRecharge(int pnum, int cii);
#ifdef HELLFIRE
void DoOil(int pnum, int cii);
#endif
void PrintItemPower(char plidx, ItemStruct *x);
void DrawUniqueInfo();
void PrintItemDetails(ItemStruct *x);
void PrintItemDur(ItemStruct *x);
void UseItem(int p, int Mid, int spl);
BOOL StoreStatOk(ItemStruct *h);
void SpawnSmith(int lvl);
#ifdef HELLFIRE
void SpawnPremium(int pnum);
#else
void SpawnPremium(int lvl);
#endif
void WitchBookLevel(int ii);
void SpawnWitch(int lvl);
void SpawnBoy(int lvl);
void SpawnHealer(int lvl);
void SpawnStoreGold();
void RecreateTownItem(int ii, int idx, WORD icreateinfo, int iseed, int ivalue);
void RecalcStoreStats();
int ItemNoFlippy();
void CreateSpellBook(int x, int y, int ispell, BOOL sendmsg, BOOL delta);
void CreateMagicArmor(int x, int y, int imisc, int icurs, BOOL sendmsg, BOOL delta);
#ifdef HELLFIRE
void CreateAmulet(int x, int y, int curlv, BOOL sendmsg, BOOL delta);
#endif
void CreateMagicWeapon(int x, int y, int imisc, int icurs, BOOL sendmsg, BOOL delta);
BOOL GetItemRecord(int nSeed, WORD wCI, int nIndex);
void SetItemRecord(int nSeed, WORD wCI, int nIndex);
void PutItemRecord(int nSeed, WORD wCI, int nIndex);

/* data */

#ifdef HELLFIRE
extern int MaxGold;
#endif

extern BYTE ItemCAnimTbl[];
extern int ItemInvSnds[];

#ifdef __cplusplus
}
#endif

DEVILUTION_END_NAMESPACE

#endif /* __ITEMS_H__ */

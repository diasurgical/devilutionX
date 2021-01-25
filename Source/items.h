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

typedef struct CornerStoneStruct {
	int x;
	int y;
	bool activated;
	ItemStruct item;
} CornerStoneStruct;

extern int itemactive[MAXITEMS];
extern BOOL uitemflag;
extern int itemavail[MAXITEMS];
extern ItemGetRecordStruct itemrecord[MAXITEMS];
extern ItemStruct item[MAXITEMS + 1];
extern CornerStoneStruct CornerStone;
extern BOOL UniqueItemFlag[128];
extern int auricGold;
extern int numitems;

int get_ring_max_value(int i);
int get_bow_max_value(int i);
int get_staff_max_value(int i);
int get_sword_max_value(int i);
int get_helm_max_value(int i);
int get_shield_max_value(int i);
int get_armor_max_value(int i);
int get_mace_max_value(int i);
int get_amulet_max_value(int i);
int get_axe_max_value(int i);
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
void items_427A72();
void items_427ABA(int x, int y);
void SpawnQuestItem(int itemid, int x, int y, int randarea, int selflag);
void SpawnRock();
void SpawnRewardItem(int itemid, int xx, int yy);
void SpawnMapOfDoom(int xx, int yy);
void SpawnRuneBomb(int xx, int yy);
void SpawnTheodore(int xx, int yy);
void RespawnItem(int i, BOOL FlipFlag);
void DeleteItem(int ii, int i);
void ProcessItems();
void FreeItemGFX();
void GetItemFrm(int i);
void GetItemStr(int i);
void CheckIdentify(int pnum, int cii);
void DoRepair(int pnum, int cii);
void DoRecharge(int pnum, int cii);
void DoOil(int pnum, int cii);
void PrintItemPower(char plidx, ItemStruct *x);
void DrawUniqueInfo();
void PrintItemDetails(ItemStruct *x);
void PrintItemDur(ItemStruct *x);
void UseItem(int p, int Mid, int spl);
BOOL StoreStatOk(ItemStruct *h);
void SpawnSmith(int lvl);
void SpawnPremium(int pnum);
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
void CreateAmulet(int x, int y, int curlv, BOOL sendmsg, BOOL delta);
void CreateMagicWeapon(int x, int y, int imisc, int icurs, BOOL sendmsg, BOOL delta);
BOOL GetItemRecord(int nSeed, WORD wCI, int nIndex);
void SetItemRecord(int nSeed, WORD wCI, int nIndex);
void PutItemRecord(int nSeed, WORD wCI, int nIndex);

/* data */

extern int MaxGold;

extern BYTE ItemCAnimTbl[];
extern int ItemInvSnds[];

#ifdef __cplusplus
}
#endif

DEVILUTION_END_NAMESPACE

#endif /* __ITEMS_H__ */

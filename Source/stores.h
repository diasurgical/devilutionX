/**
 * @file stores.h
 *
 * Interface of functionality for stores and towner dialogs.
 */
#ifndef __STORES_H__
#define __STORES_H__

#include "engine.h"

DEVILUTION_BEGIN_NAMESPACE

#ifdef __cplusplus
extern "C" {
#endif

typedef enum talk_id {
	STORE_NONE,
	STORE_SMITH,
	STORE_SBUY,
	STORE_SSELL,
	STORE_SREPAIR,
	STORE_WITCH,
	STORE_WBUY,
	STORE_WSELL,
	STORE_WRECHARGE,
	STORE_NOMONEY,
	STORE_NOROOM,
	STORE_CONFIRM,
	STORE_BOY,
	STORE_BBOY,
	STORE_HEALER,
	STORE_STORY,
	STORE_HBUY,
	STORE_SIDENTIFY,
	STORE_SPBUY,
	STORE_GOSSIP,
	STORE_IDSHOW,
	STORE_TAVERN,
	STORE_DRUNK,
	STORE_BARMAID,
} talk_id;

typedef struct STextStruct {
	int _sx;
	int _syoff;
	char _sstr[128];
	bool _sjust;
	text_color _sclr;
	int _sline;
	bool _ssel;
	int _sval;
} STextStruct;

/** Shop frame graphics */
extern BYTE *pSTextBoxCels;
/** Small text selection cursor */
extern BYTE *pSPentSpn2Cels;
/** Scrollbar graphics */
extern BYTE *pSTextSlidCels;

/** Currently active store */
extern talk_id stextflag;

/** Current index into storehidx/storehold */
extern int storenumh;
/** Map of inventory items being presented in the store */
extern char storehidx[48];
/** Copies of the players items as presented in the store */
extern ItemStruct storehold[48];

/** Temporary item used to generate gold piles by various function */
extern ItemStruct golditem;

/** Items sold by Griswold */
extern ItemStruct smithitem[SMITH_ITEMS];
/** Number of premium items for sale by Griswold */
extern int numpremium;
/** Base level of current premium items sold by Griswold */
extern int premiumlevel;
/** Premium items sold by Griswold */
extern ItemStruct premiumitem[SMITH_PREMIUM_ITEMS];

/** Items sold by Pepin */
extern ItemStruct healitem[20];

/** Items sold by Adria */
extern ItemStruct witchitem[WITCH_ITEMS];

/** Current level of the item sold by Wirt */
extern int boylevel;
/** Current item sold by Wirt */
extern ItemStruct boyitem;

void AddStoreHoldRepair(ItemStruct *itm, int i);
void InitStores();
int PentSpn2Spin();
void SetupTownStores();
void FreeStoreMem();
void PrintSString(CelOutputBuffer out, int x, int y, bool cjustflag, const char *str, text_color col, int val);
void DrawSLine(CelOutputBuffer out, int y);
void DrawSTextHelp();
void ClearSText(int s, int e);
void StartStore(talk_id s);
void DrawSText(CelOutputBuffer out);
void STextESC();
void STextUp();
void STextDown();
void STextPrior();
void STextNext();
void SetGoldCurs(int pnum, int i);
void SetSpdbarGoldCurs(int pnum, int i);
void TakePlrsMoney(int cost);
void STextEnter();
void CheckStoreBtn();
void ReleaseStoreBtn();

/* rdata */

#ifdef __cplusplus
}
#endif

DEVILUTION_END_NAMESPACE

#endif /* __STORES_H__ */

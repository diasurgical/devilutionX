/**
 * @file pack.h
 *
 * Interface of functions for minifying player data structure.
 */
#pragma once

#include "inv.h"
#include "items.h"

namespace devilution {

#pragma pack(push, 1)
struct PkItemStruct {
	Uint32 iSeed;
	Uint16 iCreateInfo;
	Uint16 idx;
	Uint8 bId;
	Uint8 bDur;
	Uint8 bMDur;
	Uint8 bCh;
	Uint8 bMCh;
	Uint16 wValue;
	Uint32 dwBuff;
};

struct PkPlayerStruct {
	Uint32 dwLowDateTime;
	Uint32 dwHighDateTime;
	Sint8 destAction;
	Sint8 destParam1;
	Sint8 destParam2;
	Uint8 plrlevel;
	Uint8 px;
	Uint8 py;
	Uint8 targx;
	Uint8 targy;
	char pName[PLR_NAME_LEN];
	Sint8 pClass;
	Uint8 pBaseStr;
	Uint8 pBaseMag;
	Uint8 pBaseDex;
	Uint8 pBaseVit;
	Sint8 pLevel;
	Uint8 pStatPts;
	Sint32 pExperience;
	Sint32 pGold;
	Sint32 pHPBase;
	Sint32 pMaxHPBase;
	Sint32 pManaBase;
	Sint32 pMaxManaBase;
	Sint8 pSplLvl[37]; // Should be MAX_SPELLS but set to 37 to make save games compatible
	Uint64 pMemSpells;
	PkItemStruct InvBody[NUM_INVLOC];
	PkItemStruct InvList[NUM_INV_GRID_ELEM];
	Sint8 InvGrid[NUM_INV_GRID_ELEM];
	Uint8 _pNumInv;
	PkItemStruct SpdList[MAXBELTITEMS];
	Sint8 pTownWarps;
	Sint8 pDungMsgs;
	Sint8 pLvlLoad;
	Uint8 pBattleNet;
	Uint8 pManaShield;
	Uint8 pDungMsgs2;
	Sint8 bIsHellfire;
	Sint8 bReserved; // For future use
	Uint16 wReflections;
	Sint16 wReserved2;  // For future use
	Sint8 pSplLvl2[10]; // Hellfire spells
	Sint16 wReserved8;  // For future use
	Uint32 pDiabloKillLevel;
	Uint32 pDifficulty;
	Sint32 pDamAcFlags;
	Sint32 dwReserved[5]; // For future use
};
#pragma pack(pop)

void PackPlayer(PkPlayerStruct *pPack, int pnum, bool manashield);
void UnPackPlayer(PkPlayerStruct *pPack, int pnum, bool netSync);
void PackItem(PkItemStruct *id, const ItemStruct *is);
void UnPackItem(const PkItemStruct *is, ItemStruct *id, bool isHellfire);

}

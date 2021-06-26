/**
 * @file pack.h
 *
 * Interface of functions for minifying player data structure.
 */
#pragma once

#include <cstdint>

#include "player.h"
#include "inv.h"
#include "items.h"

namespace devilution {

#pragma pack(push, 1)
struct PkItemStruct {
	uint32_t iSeed;
	uint16_t iCreateInfo;
	uint16_t idx;
	uint8_t bId;
	uint8_t bDur;
	uint8_t bMDur;
	uint8_t bCh;
	uint8_t bMCh;
	uint16_t wValue;
	int32_t dwBuff;
};

struct PkPlayerStruct {
	uint32_t dwLowDateTime;
	uint32_t dwHighDateTime;
	int8_t destAction;
	int8_t destParam1;
	int8_t destParam2;
	uint8_t plrlevel;
	uint8_t px;
	uint8_t py;
	uint8_t targx;
	uint8_t targy;
	char pName[PLR_NAME_LEN];
	int8_t pClass;
	uint8_t pBaseStr;
	uint8_t pBaseMag;
	uint8_t pBaseDex;
	uint8_t pBaseVit;
	int8_t pLevel;
	uint8_t pStatPts;
	int32_t pExperience;
	int32_t pGold;
	int32_t pHPBase;
	int32_t pMaxHPBase;
	int32_t pManaBase;
	int32_t pMaxManaBase;
	int8_t pSplLvl[37]; // Should be MAX_SPELLS but set to 37 to make save games compatible
	uint64_t pMemSpells;
	PkItemStruct InvBody[NUM_INVLOC];
	PkItemStruct InvList[NUM_INV_GRID_ELEM];
	int8_t InvGrid[NUM_INV_GRID_ELEM];
	uint8_t _pNumInv;
	PkItemStruct SpdList[MAXBELTITEMS];
	int8_t pTownWarps;
	int8_t pDungMsgs;
	int8_t pLvlLoad;
	uint8_t pBattleNet;
	uint8_t pManaShield;
	uint8_t pDungMsgs2;
	/** The format the charater is in, 0: Diablo, 1: Hellfire */
	int8_t bIsHellfire;
	int8_t bReserved; // For future use
	uint16_t wReflections;
	int16_t wReserved2;  // For future use
	int8_t pSplLvl2[10]; // Hellfire spells
	int16_t wReserved8;  // For future use
	uint32_t pDiabloKillLevel;
	uint32_t pDifficulty;
	int32_t pDamAcFlags;
	int32_t dwReserved[5]; // For future use
};
#pragma pack(pop)

void PackPlayer(PkPlayerStruct *pPack, const PlayerStruct &player, bool manashield);
void UnPackPlayer(const PkPlayerStruct *pPack, int pnum, bool netSync);
void PackItem(PkItemStruct *id, const ItemStruct *is);
void UnPackItem(const PkItemStruct *is, ItemStruct *id, bool isHellfire);

} // namespace devilution

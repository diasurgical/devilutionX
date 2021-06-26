/**
 * @file pack.cpp
 *
 * Implementation of functions for minifying player data structure.
 */
#include "pack.h"

#include "init.h"
#include "loadsave.h"
#include "stores.h"
#include "utils/endian.hpp"

namespace devilution {

void PackItem(PkItemStruct *id, const ItemStruct *is)
{
	memset(id, 0, sizeof(*id));
	if (is->isEmpty()) {
		id->idx = 0xFFFF;
	} else {
		auto idx = is->IDidx;
		if (!gbIsHellfire) {
			idx = RemapItemIdxToDiablo(idx);
		}
		if (gbIsSpawn) {
			idx = RemapItemIdxToSpawn(idx);
		}
		id->idx = SDL_SwapLE16(idx);
		if (is->IDidx == IDI_EAR) {
			id->iCreateInfo = is->_iName[8] | (is->_iName[7] << 8);
			id->iSeed = LoadBE32(&is->_iName[9]);
			id->bId = is->_iName[13];
			id->bDur = is->_iName[14];
			id->bMDur = is->_iName[15];
			id->bCh = is->_iName[16];
			id->bMCh = is->_iName[17];
			id->wValue = SDL_SwapLE16(is->_ivalue | (is->_iName[18] << 8) | ((is->_iCurs - ICURS_EAR_SORCERER) << 6));
			id->dwBuff = LoadBE32(&is->_iName[19]);
		} else {
			id->iSeed = SDL_SwapLE32(is->_iSeed);
			id->iCreateInfo = SDL_SwapLE16(is->_iCreateInfo);
			id->bId = (is->_iMagical << 1) | (is->_iIdentified ? 1 : 0);
			id->bDur = is->_iDurability;
			id->bMDur = is->_iMaxDur;
			id->bCh = is->_iCharges;
			id->bMCh = is->_iMaxCharges;
			if (is->IDidx == IDI_GOLD)
				id->wValue = SDL_SwapLE16(is->_ivalue);
			id->dwBuff = is->dwBuff;
		}
	}
}

void PackPlayer(PkPlayerStruct *pPack, const PlayerStruct &player, bool manashield)
{
	memset(pPack, 0, sizeof(*pPack));
	pPack->destAction = player.destAction;
	pPack->destParam1 = player.destParam1;
	pPack->destParam2 = player.destParam2;
	pPack->plrlevel = player.plrlevel;
	pPack->px = player.position.tile.x;
	pPack->py = player.position.tile.y;
	if (gbVanilla) {
		pPack->targx = player.position.tile.x;
		pPack->targy = player.position.tile.y;
	}
	strcpy(pPack->pName, player._pName);
	pPack->pClass = static_cast<int8_t>(player._pClass);
	pPack->pBaseStr = player._pBaseStr;
	pPack->pBaseMag = player._pBaseMag;
	pPack->pBaseDex = player._pBaseDex;
	pPack->pBaseVit = player._pBaseVit;
	pPack->pLevel = player._pLevel;
	pPack->pStatPts = player._pStatPts;
	pPack->pExperience = SDL_SwapLE32(player._pExperience);
	pPack->pGold = SDL_SwapLE32(player._pGold);
	pPack->pHPBase = SDL_SwapLE32(player._pHPBase);
	pPack->pMaxHPBase = SDL_SwapLE32(player._pMaxHPBase);
	pPack->pManaBase = SDL_SwapLE32(player._pManaBase);
	pPack->pMaxManaBase = SDL_SwapLE32(player._pMaxManaBase);
	pPack->pMemSpells = SDL_SwapLE64(player._pMemSpells);

	for (int i = 0; i < 37; i++) // Should be MAX_SPELLS but set to 37 to make save games compatible
		pPack->pSplLvl[i] = player._pSplLvl[i];
	for (int i = 37; i < 47; i++)
		pPack->pSplLvl2[i - 37] = player._pSplLvl[i];

	for (int i = 0; i < NUM_INVLOC; i++) {
		PackItem(&pPack->InvBody[i], &player.InvBody[i]);
	}

	for (int i = 0; i < NUM_INV_GRID_ELEM; i++) {
		PackItem(&pPack->InvList[i], &player.InvList[i]);
	}

	for (int i = 0; i < NUM_INV_GRID_ELEM; i++)
		pPack->InvGrid[i] = player.InvGrid[i];

	pPack->_pNumInv = player._pNumInv;

	for (int i = 0; i < MAXBELTITEMS; i++) {
		PackItem(&pPack->SpdList[i], &player.SpdList[i]);
	}

	pPack->wReflections = SDL_SwapLE16(player.wReflections);
	pPack->pDifficulty = SDL_SwapLE32(player.pDifficulty);
	pPack->pDamAcFlags = SDL_SwapLE32(player.pDamAcFlags);
	pPack->pDiabloKillLevel = SDL_SwapLE32(player.pDiabloKillLevel);
	pPack->bIsHellfire = gbIsHellfire ? 1 : 0;

	if (!gbIsMultiplayer || manashield)
		pPack->pManaShield = SDL_SwapLE32(player.pManaShield);
	else
		pPack->pManaShield = 0;
}

/**
 * Expand a PkItemStruct in to a ItemStruct
 *
 * Note: last slot of item[MAXITEMS+1] used as temporary buffer
 * find real name reference below, possibly [sizeof(item[])/sizeof(ItemStruct)]
 * @param is The source packed item
 * @param id The distination item
 */
void UnPackItem(const PkItemStruct *is, ItemStruct *id, bool isHellfire)
{
	auto idx = static_cast<_item_indexes>(SDL_SwapLE16(is->idx));
	if (idx == 0xFFFF) {
		id->_itype = ITYPE_NONE;
		return;
	}

	if (gbIsSpawn) {
		idx = RemapItemIdxFromSpawn(idx);
	}
	if (!isHellfire) {
		idx = RemapItemIdxFromDiablo(idx);
	}

	if (!IsItemAvailable(idx)) {
		id->_itype = ITYPE_NONE;
		return;
	}

	if (idx == IDI_EAR) {
		RecreateEar(
		    MAXITEMS,
		    SDL_SwapLE16(is->iCreateInfo),
		    SDL_SwapLE32(is->iSeed),
		    is->bId,
		    is->bDur,
		    is->bMDur,
		    is->bCh,
		    is->bMCh,
		    SDL_SwapLE16(is->wValue),
		    SDL_SwapLE32(is->dwBuff));
	} else {
		memset(&items[MAXITEMS], 0, sizeof(*items));
		RecreateItem(MAXITEMS, idx, SDL_SwapLE16(is->iCreateInfo), SDL_SwapLE32(is->iSeed), SDL_SwapLE16(is->wValue), isHellfire);
		items[MAXITEMS]._iMagical = is->bId >> 1;
		items[MAXITEMS]._iIdentified = (is->bId & 1) != 0;
		items[MAXITEMS]._iDurability = is->bDur;
		items[MAXITEMS]._iMaxDur = is->bMDur;
		items[MAXITEMS]._iCharges = is->bCh;
		items[MAXITEMS]._iMaxCharges = is->bMCh;

		RemoveInvalidItem(&items[MAXITEMS]);

		if (isHellfire)
			items[MAXITEMS].dwBuff |= CF_HELLFIRE;
		else
			items[MAXITEMS].dwBuff &= ~CF_HELLFIRE;
	}
	*id = items[MAXITEMS];
}

static void VerifyGoldSeeds(PlayerStruct &player)
{
	for (int i = 0; i < player._pNumInv; i++) {
		if (player.InvList[i].IDidx != IDI_GOLD)
			continue;
		for (int j = 0; j < player._pNumInv; j++) {
			if (i == j)
				continue;
			if (player.InvList[j].IDidx != IDI_GOLD)
				continue;
			if (player.InvList[i]._iSeed != player.InvList[j]._iSeed)
				continue;
			player.InvList[i]._iSeed = AdvanceRndSeed();
			j = -1;
		}
	}
}

void UnPackPlayer(const PkPlayerStruct *pPack, int pnum, bool netSync)
{
	auto &player = plr[pnum];

	player.position.tile = { pPack->px, pPack->py };
	player.position.future = { pPack->px, pPack->py };
	player.plrlevel = pPack->plrlevel;
	ClrPlrPath(player);
	player.destAction = ACTION_NONE;
	strcpy(player._pName, pPack->pName);
	player._pClass = (HeroClass)pPack->pClass;
	InitPlayer(pnum, true);
	player._pBaseStr = pPack->pBaseStr;
	player._pStrength = pPack->pBaseStr;
	player._pBaseMag = pPack->pBaseMag;
	player._pMagic = pPack->pBaseMag;
	player._pBaseDex = pPack->pBaseDex;
	player._pDexterity = pPack->pBaseDex;
	player._pBaseVit = pPack->pBaseVit;
	player._pVitality = pPack->pBaseVit;
	player._pLevel = pPack->pLevel;
	player._pStatPts = pPack->pStatPts;
	player._pExperience = SDL_SwapLE32(pPack->pExperience);
	player._pGold = SDL_SwapLE32(pPack->pGold);
	player._pMaxHPBase = SDL_SwapLE32(pPack->pMaxHPBase);
	player._pHPBase = SDL_SwapLE32(pPack->pHPBase);
	player._pBaseToBlk = ToBlkTbl[static_cast<std::size_t>(player._pClass)];
	if (!netSync)
		if ((int)(player._pHPBase & 0xFFFFFFC0) < 64)
			player._pHPBase = 64;

	player._pMaxManaBase = SDL_SwapLE32(pPack->pMaxManaBase);
	player._pManaBase = SDL_SwapLE32(pPack->pManaBase);
	player._pMemSpells = SDL_SwapLE64(pPack->pMemSpells);

	for (int i = 0; i < 37; i++) // Should be MAX_SPELLS but set to 36 to make save games compatible
		player._pSplLvl[i] = pPack->pSplLvl[i];
	for (int i = 37; i < 47; i++)
		player._pSplLvl[i] = pPack->pSplLvl2[i - 37];

	for (int i = 0; i < NUM_INVLOC; i++) {
		auto packedItem = pPack->InvBody[i];
		bool isHellfire = netSync ? ((packedItem.dwBuff & CF_HELLFIRE) != 0) : (pPack->bIsHellfire != 0);
		UnPackItem(&packedItem, &player.InvBody[i], isHellfire);
	}

	for (int i = 0; i < NUM_INV_GRID_ELEM; i++) {
		auto packedItem = pPack->InvList[i];
		bool isHellfire = netSync ? ((packedItem.dwBuff & CF_HELLFIRE) != 0) : (pPack->bIsHellfire != 0);
		UnPackItem(&packedItem, &player.InvList[i], isHellfire);
	}

	for (int i = 0; i < NUM_INV_GRID_ELEM; i++)
		player.InvGrid[i] = pPack->InvGrid[i];

	player._pNumInv = pPack->_pNumInv;
	VerifyGoldSeeds(player);

	for (int i = 0; i < MAXBELTITEMS; i++) {
		auto packedItem = pPack->SpdList[i];
		bool isHellfire = netSync ? ((packedItem.dwBuff & CF_HELLFIRE) != 0) : (pPack->bIsHellfire != 0);
		UnPackItem(&packedItem, &player.SpdList[i], isHellfire);
	}

	if (pnum == myplr) {
		for (int i = 0; i < 20; i++)
			witchitem[i]._itype = ITYPE_NONE;
	}

	CalcPlrInv(pnum, false);
	player.wReflections = SDL_SwapLE16(pPack->wReflections);
	player.pTownWarps = 0;
	player.pDungMsgs = 0;
	player.pDungMsgs2 = 0;
	player.pLvlLoad = 0;
	player.pDiabloKillLevel = SDL_SwapLE32(pPack->pDiabloKillLevel);
	player.pBattleNet = pPack->pBattleNet != 0;
	player.pManaShield = SDL_SwapLE32(pPack->pManaShield);
	player.pDifficulty = (_difficulty)SDL_SwapLE32(pPack->pDifficulty);
	player.pDamAcFlags = SDL_SwapLE32(pPack->pDamAcFlags);
}

} // namespace devilution

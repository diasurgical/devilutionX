/**
 * @file pack.cpp
 *
 * Implementation of functions for minifying player data structure.
 */
#include "pack.h"

#include "engine/random.hpp"
#include "init.h"
#include "loadsave.h"
#include "stores.h"
#include "utils/endian.hpp"

namespace devilution {

namespace {

void VerifyGoldSeeds(Player &player)
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

} // namespace

void PackItem(ItemPack *id, const Item *is)
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

void PackPlayer(PlayerPack *pPack, const Player &player, bool manashield)
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

	pPack->_pNumInv = player._pNumInv;
	for (int i = 0; i < pPack->_pNumInv; i++) {
		PackItem(&pPack->InvList[i], &player.InvList[i]);
	}

	for (int i = 0; i < NUM_INV_GRID_ELEM; i++)
		pPack->InvGrid[i] = player.InvGrid[i];

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

void UnPackItem(const ItemPack &packedItem, Item &item, bool isHellfire)
{
	auto idx = static_cast<_item_indexes>(SDL_SwapLE16(packedItem.idx));

	if (gbIsSpawn) {
		idx = RemapItemIdxFromSpawn(idx);
	}
	if (!isHellfire) {
		idx = RemapItemIdxFromDiablo(idx);
	}

	if (!IsItemAvailable(idx)) {
		item._itype = ItemType::None;
		return;
	}

	if (idx == IDI_EAR) {
		RecreateEar(
		    item,
		    SDL_SwapLE16(packedItem.iCreateInfo),
		    SDL_SwapLE32(packedItem.iSeed),
		    packedItem.bId,
		    packedItem.bDur,
		    packedItem.bMDur,
		    packedItem.bCh,
		    packedItem.bMCh,
		    SDL_SwapLE16(packedItem.wValue),
		    SDL_SwapLE32(packedItem.dwBuff));
	} else {
		item = {};
		RecreateItem(item, idx, SDL_SwapLE16(packedItem.iCreateInfo), SDL_SwapLE32(packedItem.iSeed), SDL_SwapLE16(packedItem.wValue), isHellfire);
		item._iMagical = static_cast<item_quality>(packedItem.bId >> 1);
		item._iIdentified = (packedItem.bId & 1) != 0;
		item._iDurability = packedItem.bDur;
		item._iMaxDur = packedItem.bMDur;
		item._iCharges = packedItem.bCh;
		item._iMaxCharges = packedItem.bMCh;

		RemoveInvalidItem(item);

		if (isHellfire)
			item.dwBuff |= CF_HELLFIRE;
		else
			item.dwBuff &= ~CF_HELLFIRE;
	}
}

bool UnPackPlayer(const PlayerPack *pPack, Player &player, bool netSync)
{
	Point position { pPack->px, pPack->py };
	if (!InDungeonBounds(position)) {
		return false;
	}

	uint8_t dungeonLevel = pPack->plrlevel;
	if (dungeonLevel >= NUMLEVELS) {
		return false;
	}

	if (pPack->pClass >= enum_size<HeroClass>::value) {
		return false;
	}
	auto heroClass = static_cast<HeroClass>(pPack->pClass);

	if (pPack->pLevel >= MAXCHARLEVEL || pPack->pLevel < 1) {
		return false;
	}
	uint32_t difficulty = SDL_SwapLE32(pPack->pDifficulty);
	if (difficulty > DIFF_LAST) {
		return false;
	}

	player._pLevel = pPack->pLevel;

	player.position.tile = position;
	player.position.future = position;
	player.plrlevel = dungeonLevel;

	player._pClass = heroClass;

	ClrPlrPath(player);
	player.destAction = ACTION_NONE;

	strncpy(player._pName, pPack->pName, PLR_NAME_LEN - 1);
	player._pName[PLR_NAME_LEN - 1] = '\0';

	InitPlayer(player, true);

	player._pBaseStr = pPack->pBaseStr;
	player._pStrength = pPack->pBaseStr;
	player._pBaseMag = pPack->pBaseMag;
	player._pMagic = pPack->pBaseMag;
	player._pBaseDex = pPack->pBaseDex;
	player._pDexterity = pPack->pBaseDex;
	player._pBaseVit = pPack->pBaseVit;
	player._pVitality = pPack->pBaseVit;

	player._pStatPts = pPack->pStatPts;
	player._pExperience = SDL_SwapLE32(pPack->pExperience);
	player._pGold = SDL_SwapLE32(pPack->pGold);
	player._pMaxHPBase = SDL_SwapLE32(pPack->pMaxHPBase);
	player._pHPBase = SDL_SwapLE32(pPack->pHPBase);
	player._pBaseToBlk = BlockBonuses[static_cast<std::size_t>(player._pClass)];
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
		UnPackItem(packedItem, player.InvBody[i], isHellfire);
	}

	player._pNumInv = pPack->_pNumInv;
	for (int i = 0; i < player._pNumInv; i++) {
		auto packedItem = pPack->InvList[i];
		bool isHellfire = netSync ? ((packedItem.dwBuff & CF_HELLFIRE) != 0) : (pPack->bIsHellfire != 0);
		UnPackItem(packedItem, player.InvList[i], isHellfire);
	}

	for (int i = 0; i < NUM_INV_GRID_ELEM; i++)
		player.InvGrid[i] = pPack->InvGrid[i];

	VerifyGoldSeeds(player);

	for (int i = 0; i < MAXBELTITEMS; i++) {
		auto packedItem = pPack->SpdList[i];
		bool isHellfire = netSync ? ((packedItem.dwBuff & CF_HELLFIRE) != 0) : (pPack->bIsHellfire != 0);
		UnPackItem(packedItem, player.SpdList[i], isHellfire);
	}

	if (&player == &Players[MyPlayerId]) {
		for (int i = 0; i < 20; i++)
			witchitem[i]._itype = ItemType::None;
	}

	CalcPlrInv(player, false);
	player.wReflections = SDL_SwapLE16(pPack->wReflections);
	player.pTownWarps = 0;
	player.pDungMsgs = 0;
	player.pDungMsgs2 = 0;
	player.pLvlLoad = 0;
	player.pDiabloKillLevel = SDL_SwapLE32(pPack->pDiabloKillLevel);
	player.pBattleNet = pPack->pBattleNet != 0;
	player.pManaShield = pPack->pManaShield != 0;
	player.pDifficulty = static_cast<_difficulty>(difficulty);
	player.pDamAcFlags = SDL_SwapLE32(pPack->pDamAcFlags);

	return true;
}

} // namespace devilution

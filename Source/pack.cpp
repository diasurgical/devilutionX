/**
 * @file pack.cpp
 *
 * Implementation of functions for minifying player data structure.
 */
#include "pack.h"

#include <cstdint>

#include "engine/random.hpp"
#include "init.h"
#include "loadsave.h"
#include "playerdat.hpp"
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

void PackItem(ItemPack &packedItem, const Item &item, bool isHellfire)
{
	packedItem = {};
	// Arena potions don't exist in vanilla so don't save them to stay backward compatible
	if (item.isEmpty() || item._iMiscId == IMISC_ARENAPOT) {
		packedItem.idx = 0xFFFF;
	} else {
		auto idx = item.IDidx;
		if (!isHellfire) {
			idx = RemapItemIdxToDiablo(idx);
		}
		if (gbIsSpawn) {
			idx = RemapItemIdxToSpawn(idx);
		}
		packedItem.idx = SDL_SwapLE16(idx);
		if (item.IDidx == IDI_EAR) {
			packedItem.iCreateInfo = SDL_SwapLE16(item._iIName[1] | (item._iIName[0] << 8));
			packedItem.iSeed = SDL_SwapLE32(LoadBE32(&item._iIName[2]));
			packedItem.bId = item._iIName[6];
			packedItem.bDur = item._iIName[7];
			packedItem.bMDur = item._iIName[8];
			packedItem.bCh = item._iIName[9];
			packedItem.bMCh = item._iIName[10];
			packedItem.wValue = SDL_SwapLE16(item._ivalue | (item._iIName[11] << 8) | ((item._iCurs - ICURS_EAR_SORCERER) << 6));
			packedItem.dwBuff = SDL_SwapLE32(LoadBE32(&item._iIName[12]));
		} else {
			packedItem.iSeed = SDL_SwapLE32(item._iSeed);
			packedItem.iCreateInfo = SDL_SwapLE16(item._iCreateInfo);
			packedItem.bId = (item._iMagical << 1) | (item._iIdentified ? 1 : 0);
			packedItem.bDur = item._iDurability;
			packedItem.bMDur = item._iMaxDur;
			packedItem.bCh = item._iCharges;
			packedItem.bMCh = item._iMaxCharges;
			if (item.IDidx == IDI_GOLD)
				packedItem.wValue = SDL_SwapLE16(item._ivalue);
			packedItem.dwBuff = item.dwBuff;
		}
	}
}

void PackPlayer(PlayerPack *pPack, const Player &player)
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
		const Item &item = player.InvBody[i];
		bool isHellfire = gbIsHellfire;
		PackItem(pPack->InvBody[i], item, isHellfire);
	}

	pPack->_pNumInv = player._pNumInv;
	for (int i = 0; i < pPack->_pNumInv; i++) {
		const Item &item = player.InvList[i];
		bool isHellfire = gbIsHellfire;
		PackItem(pPack->InvList[i], item, isHellfire);
	}

	for (int i = 0; i < InventoryGridCells; i++)
		pPack->InvGrid[i] = player.InvGrid[i];

	for (int i = 0; i < MaxBeltItems; i++) {
		const Item &item = player.SpdList[i];
		bool isHellfire = gbIsHellfire;
		PackItem(pPack->SpdList[i], item, isHellfire);
	}

	pPack->wReflections = SDL_SwapLE16(player.wReflections);
	pPack->pDifficulty = SDL_SwapLE32(player.pDifficulty);
	pPack->pDamAcFlags = SDL_SwapLE32(static_cast<uint32_t>(player.pDamAcFlags));
	pPack->pDiabloKillLevel = SDL_SwapLE32(player.pDiabloKillLevel);
	pPack->bIsHellfire = gbIsHellfire ? 1 : 0;
	pPack->pManaShield = 0;
}

void PackNetPlayer(PlayerNetPack &packed, const Player &player)
{
	packed.plrlevel = player.plrlevel;
	packed.px = player.position.tile.x;
	packed.py = player.position.tile.y;
	strcpy(packed.pName, player._pName);
	packed.pClass = static_cast<int8_t>(player._pClass);
	packed.pBaseStr = player._pBaseStr;
	packed.pBaseMag = player._pBaseMag;
	packed.pBaseDex = player._pBaseDex;
	packed.pBaseVit = player._pBaseVit;
	packed.pLevel = player._pLevel;
	packed.pStatPts = player._pStatPts;
	packed.pExperience = SDL_SwapLE32(player._pExperience);
	packed.pGold = SDL_SwapLE32(player._pGold);
	packed.pHPBase = SDL_SwapLE32(player._pHPBase);
	packed.pMaxHPBase = SDL_SwapLE32(player._pMaxHPBase);
	packed.pManaBase = SDL_SwapLE32(player._pManaBase);
	packed.pMaxManaBase = SDL_SwapLE32(player._pMaxManaBase);
	packed.pMemSpells = SDL_SwapLE64(player._pMemSpells);

	for (int i = 0; i < MAX_SPELLS; i++)
		packed.pSplLvl[i] = player._pSplLvl[i];

	for (int i = 0; i < NUM_INVLOC; i++)
		PrepareItemForNetwork(player.InvBody[i], packed.InvBody[i]);

	packed._pNumInv = player._pNumInv;
	for (int i = 0; i < packed._pNumInv; i++)
		PrepareItemForNetwork(player.InvList[i], packed.InvList[i]);

	for (int i = 0; i < InventoryGridCells; i++)
		packed.InvGrid[i] = player.InvGrid[i];

	for (int i = 0; i < MaxBeltItems; i++)
		PrepareItemForNetwork(player.SpdList[i], packed.SpdList[i]);

	packed.wReflections = SDL_SwapLE16(player.wReflections);
	packed.pDifficulty = player.pDifficulty;
	packed.pDamAcFlags = player.pDamAcFlags;
	packed.pDiabloKillLevel = player.pDiabloKillLevel;
	packed.pManaShield = player.pManaShield;
	packed.friendlyMode = player.friendlyMode ? 1 : 0;
	packed.isOnSetLevel = player.plrIsOnSetLevel;
}

void UnPackItem(const ItemPack &packedItem, const Player &player, Item &item, bool isHellfire)
{
	auto idx = static_cast<_item_indexes>(SDL_SwapLE16(packedItem.idx));

	if (gbIsSpawn) {
		idx = RemapItemIdxFromSpawn(idx);
	}
	if (!isHellfire) {
		idx = RemapItemIdxFromDiablo(idx);
	}

	if (!IsItemAvailable(idx)) {
		item.clear();
		return;
	}

	if (idx == IDI_EAR) {
		uint16_t ic = SDL_SwapLE16(packedItem.iCreateInfo);
		uint32_t iseed = SDL_SwapLE32(packedItem.iSeed);
		uint16_t ivalue = SDL_SwapLE16(packedItem.wValue);
		int32_t ibuff = SDL_SwapLE32(packedItem.dwBuff);

		char heroName[17];
		heroName[0] = static_cast<char>((ic >> 8) & 0x7F);
		heroName[1] = static_cast<char>(ic & 0x7F);
		heroName[2] = static_cast<char>((iseed >> 24) & 0x7F);
		heroName[3] = static_cast<char>((iseed >> 16) & 0x7F);
		heroName[4] = static_cast<char>((iseed >> 8) & 0x7F);
		heroName[5] = static_cast<char>(iseed & 0x7F);
		heroName[6] = static_cast<char>(packedItem.bId & 0x7F);
		heroName[7] = static_cast<char>(packedItem.bDur & 0x7F);
		heroName[8] = static_cast<char>(packedItem.bMDur & 0x7F);
		heroName[9] = static_cast<char>(packedItem.bCh & 0x7F);
		heroName[10] = static_cast<char>(packedItem.bMCh & 0x7F);
		heroName[11] = static_cast<char>((ivalue >> 8) & 0x7F);
		heroName[12] = static_cast<char>((ibuff >> 24) & 0x7F);
		heroName[13] = static_cast<char>((ibuff >> 16) & 0x7F);
		heroName[14] = static_cast<char>((ibuff >> 8) & 0x7F);
		heroName[15] = static_cast<char>(ibuff & 0x7F);
		heroName[16] = '\0';

		RecreateEar(item, ic, iseed, ivalue & 0xFF, heroName);
	} else {
		item = {};
		RecreateItem(player, item, idx, SDL_SwapLE16(packedItem.iCreateInfo), SDL_SwapLE32(packedItem.iSeed), SDL_SwapLE16(packedItem.wValue), isHellfire);
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

void UnPackPlayer(const PlayerPack *pPack, Player &player)
{
	Point position { pPack->px, pPack->py };

	player = {};
	player._pLevel = clamp<int8_t>(pPack->pLevel, 1, MaxCharacterLevel);
	player._pMaxHPBase = SDL_SwapLE32(pPack->pMaxHPBase);
	player._pHPBase = SDL_SwapLE32(pPack->pHPBase);
	player._pMaxHP = player._pMaxHPBase;
	player._pHitPoints = player._pHPBase;
	player.position.tile = position;
	player.position.future = position;
	player.setLevel(pPack->plrlevel);

	player._pClass = static_cast<HeroClass>(clamp<uint8_t>(pPack->pClass, 0, enum_size<HeroClass>::value - 1));

	ClrPlrPath(player);
	player.destAction = ACTION_NONE;

	strcpy(player._pName, pPack->pName);

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
	player._pBaseToBlk = PlayersData[static_cast<std::size_t>(player._pClass)].blockBonus;
	if ((int)(player._pHPBase & 0xFFFFFFC0) < 64)
		player._pHPBase = 64;

	player._pMaxManaBase = SDL_SwapLE32(pPack->pMaxManaBase);
	player._pManaBase = SDL_SwapLE32(pPack->pManaBase);
	player._pMemSpells = SDL_SwapLE64(pPack->pMemSpells);

	for (int i = 0; i < 37; i++) // Should be MAX_SPELLS but set to 36 to make save games compatible
		player._pSplLvl[i] = pPack->pSplLvl[i];
	for (int i = 37; i < 47; i++)
		player._pSplLvl[i] = pPack->pSplLvl2[i - 37];

	bool isHellfire = pPack->bIsHellfire != 0;

	for (int i = 0; i < NUM_INVLOC; i++)
		UnPackItem(pPack->InvBody[i], player, player.InvBody[i], isHellfire);

	player._pNumInv = pPack->_pNumInv;
	for (int i = 0; i < player._pNumInv; i++)
		UnPackItem(pPack->InvList[i], player, player.InvList[i], isHellfire);

	for (int i = 0; i < InventoryGridCells; i++)
		player.InvGrid[i] = pPack->InvGrid[i];

	VerifyGoldSeeds(player);

	for (int i = 0; i < MaxBeltItems; i++)
		UnPackItem(pPack->SpdList[i], player, player.SpdList[i], isHellfire);

	CalcPlrInv(player, false);
	player.wReflections = SDL_SwapLE16(pPack->wReflections);
	player.pDiabloKillLevel = SDL_SwapLE32(pPack->pDiabloKillLevel);
	player.pBattleNet = pPack->pBattleNet != 0;
	player.pManaShield = false;
	uint32_t difficulty = SDL_SwapLE32(pPack->pDifficulty);
	player.pDifficulty = static_cast<_difficulty>(clamp<uint32_t>(difficulty, 0, DIFF_LAST));
	player.pDamAcFlags = static_cast<ItemSpecialEffectHf>(SDL_SwapLE32(static_cast<uint32_t>(pPack->pDamAcFlags)));
}

bool UnPackNetPlayer(const PlayerNetPack &packed, Player &player)
{
	Point position { packed.px, packed.py };
	if (!InDungeonBounds(position)) {
		return false;
	}

	uint8_t dungeonLevel = packed.plrlevel;
	if (dungeonLevel >= NUMLEVELS) {
		return false;
	}

	if (packed.pClass >= enum_size<HeroClass>::value) {
		return false;
	}

	if (packed.pLevel > MaxCharacterLevel || packed.pLevel < 1) {
		return false;
	}
	uint8_t difficulty = packed.pDifficulty;
	if (difficulty > DIFF_LAST) {
		return false;
	}

	player._pLevel = packed.pLevel;

	player.position.tile = position;
	player.position.future = position;
	player.setLevel(dungeonLevel);

	player._pClass = static_cast<HeroClass>(packed.pClass);

	ClrPlrPath(player);
	player.destAction = ACTION_NONE;

	strcpy(player._pName, packed.pName);

	InitPlayer(player, true);

	player._pBaseStr = packed.pBaseStr;
	player._pStrength = packed.pBaseStr;
	player._pBaseMag = packed.pBaseMag;
	player._pMagic = packed.pBaseMag;
	player._pBaseDex = packed.pBaseDex;
	player._pDexterity = packed.pBaseDex;
	player._pBaseVit = packed.pBaseVit;
	player._pVitality = packed.pBaseVit;

	player._pStatPts = packed.pStatPts;
	player._pExperience = SDL_SwapLE32(packed.pExperience);
	player._pGold = SDL_SwapLE32(packed.pGold);
	player._pMaxHPBase = SDL_SwapLE32(packed.pMaxHPBase);
	player._pHPBase = SDL_SwapLE32(packed.pHPBase);
	player._pBaseToBlk = PlayersData[static_cast<std::size_t>(player._pClass)].blockBonus;

	player._pMaxManaBase = SDL_SwapLE32(packed.pMaxManaBase);
	player._pManaBase = SDL_SwapLE32(packed.pManaBase);
	player._pMemSpells = SDL_SwapLE64(packed.pMemSpells);

	for (int i = 0; i < MAX_SPELLS; i++)
		player._pSplLvl[i] = packed.pSplLvl[i];

	for (int i = 0; i < NUM_INVLOC; i++)
		RecreateItem(player, packed.InvBody[i], player.InvBody[i]);

	player._pNumInv = packed._pNumInv;
	for (int i = 0; i < player._pNumInv; i++)
		RecreateItem(player, packed.InvList[i], player.InvList[i]);

	for (int i = 0; i < InventoryGridCells; i++)
		player.InvGrid[i] = packed.InvGrid[i];

	for (int i = 0; i < MaxBeltItems; i++)
		RecreateItem(player, packed.SpdList[i], player.SpdList[i]);

	CalcPlrInv(player, false);
	player.wReflections = SDL_SwapLE16(packed.wReflections);
	player.pTownWarps = 0;
	player.pDungMsgs = 0;
	player.pDungMsgs2 = 0;
	player.pLvlLoad = 0;
	player.pDiabloKillLevel = packed.pDiabloKillLevel;
	player.pManaShield = packed.pManaShield != 0;
	player.pDifficulty = static_cast<_difficulty>(difficulty);
	player.pDamAcFlags = packed.pDamAcFlags;
	player.friendlyMode = packed.friendlyMode != 0;
	player.plrIsOnSetLevel = packed.isOnSetLevel != 0;

	return true;
}

} // namespace devilution

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
#include "utils/utf8.hpp"

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

void PackNetItem(const Item &item, ItemNetPack &packedItem)
{
	packedItem.def.wIndx = static_cast<_item_indexes>(SDL_SwapLE16(item.IDidx));
	packedItem.def.wCI = SDL_SwapLE16(item._iCreateInfo);
	packedItem.def.dwSeed = SDL_SwapLE32(item._iSeed);
	if (item.IDidx != IDI_EAR)
		PrepareItemForNetwork(item, packedItem.item);
	else
		PrepareEarForNetwork(item, packedItem.ear);
}

void UnPackNetItem(const Player &player, const ItemNetPack &packedItem, Item &item)
{
	item = {};
	_item_indexes idx = static_cast<_item_indexes>(SDL_SwapLE16(packedItem.def.wIndx));
	if (idx < 0 || idx > IDI_LAST)
		return;
	if (idx != IDI_EAR)
		RecreateItem(player, packedItem.item, item);
	else
		RecreateEar(item, SDL_SwapLE16(packedItem.ear.wCI), SDL_SwapLE32(packedItem.ear.dwSeed), packedItem.ear.bCursval, packedItem.ear.heroname);
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

void PackPlayer(PlayerPack &packed, const Player &player)
{
	memset(&packed, 0, sizeof(packed));
	packed.destAction = player.destAction;
	packed.destParam1 = player.destParam1;
	packed.destParam2 = player.destParam2;
	packed.plrlevel = player.plrlevel;
	packed.px = player.position.tile.x;
	packed.py = player.position.tile.y;
	if (gbVanilla) {
		packed.targx = player.position.tile.x;
		packed.targy = player.position.tile.y;
	}
	CopyUtf8(packed.pName, player._pName, sizeof(packed.pName));
	packed.pClass = static_cast<uint8_t>(player._pClass);
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

	for (int i = 0; i < 37; i++) // Should be MAX_SPELLS but set to 37 to make save games compatible
		packed.pSplLvl[i] = player._pSplLvl[i];
	for (int i = 37; i < 47; i++)
		packed.pSplLvl2[i - 37] = player._pSplLvl[i];

	for (int i = 0; i < NUM_INVLOC; i++)
		PackItem(packed.InvBody[i], player.InvBody[i], gbIsHellfire);

	packed._pNumInv = player._pNumInv;
	for (int i = 0; i < packed._pNumInv; i++)
		PackItem(packed.InvList[i], player.InvList[i], gbIsHellfire);

	for (int i = 0; i < InventoryGridCells; i++)
		packed.InvGrid[i] = player.InvGrid[i];

	for (int i = 0; i < MaxBeltItems; i++)
		PackItem(packed.SpdList[i], player.SpdList[i], gbIsHellfire);

	packed.wReflections = SDL_SwapLE16(player.wReflections);
	packed.pDamAcFlags = SDL_SwapLE32(static_cast<uint32_t>(player.pDamAcFlags));
	packed.pDiabloKillLevel = SDL_SwapLE32(player.pDiabloKillLevel);
	packed.bIsHellfire = gbIsHellfire ? 1 : 0;
}

void PackNetPlayer(PlayerNetPack &packed, const Player &player)
{
	packed.plrlevel = player.plrlevel;
	packed.px = player.position.tile.x;
	packed.py = player.position.tile.y;
	CopyUtf8(packed.pName, player._pName, sizeof(packed.pName));
	packed.pClass = static_cast<uint8_t>(player._pClass);
	packed.pBaseStr = player._pBaseStr;
	packed.pBaseMag = player._pBaseMag;
	packed.pBaseDex = player._pBaseDex;
	packed.pBaseVit = player._pBaseVit;
	packed.pLevel = player._pLevel;
	packed.pStatPts = player._pStatPts;
	packed.pExperience = SDL_SwapLE32(player._pExperience);
	packed.pHPBase = SDL_SwapLE32(player._pHPBase);
	packed.pMaxHPBase = SDL_SwapLE32(player._pMaxHPBase);
	packed.pManaBase = SDL_SwapLE32(player._pManaBase);
	packed.pMaxManaBase = SDL_SwapLE32(player._pMaxManaBase);
	packed.pMemSpells = SDL_SwapLE64(player._pMemSpells);

	for (int i = 0; i < MAX_SPELLS; i++)
		packed.pSplLvl[i] = player._pSplLvl[i];

	for (int i = 0; i < NUM_INVLOC; i++)
		PackNetItem(player.InvBody[i], packed.InvBody[i]);

	packed._pNumInv = player._pNumInv;
	for (int i = 0; i < packed._pNumInv; i++)
		PackNetItem(player.InvList[i], packed.InvList[i]);

	for (int i = 0; i < InventoryGridCells; i++)
		packed.InvGrid[i] = player.InvGrid[i];

	for (int i = 0; i < MaxBeltItems; i++)
		PackNetItem(player.SpdList[i], packed.SpdList[i]);

	packed.wReflections = SDL_SwapLE16(player.wReflections);
	packed.pDiabloKillLevel = player.pDiabloKillLevel;
	packed.pManaShield = player.pManaShield;
	packed.friendlyMode = player.friendlyMode ? 1 : 0;
	packed.isOnSetLevel = player.plrIsOnSetLevel;

	packed.pStrength = SDL_SwapLE32(player._pStrength);
	packed.pMagic = SDL_SwapLE32(player._pMagic);
	packed.pDexterity = SDL_SwapLE32(player._pDexterity);
	packed.pVitality = SDL_SwapLE32(player._pVitality);
	packed.pHitPoints = SDL_SwapLE32(player._pHitPoints);
	packed.pMaxHP = SDL_SwapLE32(player._pMaxHP);
	packed.pMana = SDL_SwapLE32(player._pMana);
	packed.pMaxMana = SDL_SwapLE32(player._pMaxMana);
	packed.pDamageMod = SDL_SwapLE32(player._pDamageMod);
	packed.pBaseToBlk = SDL_SwapLE32(player._pBaseToBlk);
	packed.pIMinDam = SDL_SwapLE32(player._pIMinDam);
	packed.pIMaxDam = SDL_SwapLE32(player._pIMaxDam);
	packed.pIAC = SDL_SwapLE32(player._pIAC);
	packed.pIBonusDam = SDL_SwapLE32(player._pIBonusDam);
	packed.pIBonusToHit = SDL_SwapLE32(player._pIBonusToHit);
	packed.pIBonusAC = SDL_SwapLE32(player._pIBonusAC);
	packed.pIBonusDamMod = SDL_SwapLE32(player._pIBonusDamMod);
	packed.pIGetHit = SDL_SwapLE32(player._pIGetHit);
	packed.pIEnAc = SDL_SwapLE32(player._pIEnAc);
	packed.pIFMinDam = SDL_SwapLE32(player._pIFMinDam);
	packed.pIFMaxDam = SDL_SwapLE32(player._pIFMaxDam);
	packed.pILMinDam = SDL_SwapLE32(player._pILMinDam);
	packed.pILMaxDam = SDL_SwapLE32(player._pILMaxDam);
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
		item._iIdentified = (packedItem.bId & 1) != 0;
		item._iMaxDur = packedItem.bMDur;
		item._iDurability = ClampDurability(item, packedItem.bDur);
		item._iMaxCharges = clamp<int>(packedItem.bMCh, 0, item._iMaxCharges);
		item._iCharges = clamp<int>(packedItem.bCh, 0, item._iMaxCharges);

		RemoveInvalidItem(item);

		if (isHellfire)
			item.dwBuff |= CF_HELLFIRE;
		else
			item.dwBuff &= ~CF_HELLFIRE;
	}
}

void UnPackPlayer(const PlayerPack &packed, Player &player)
{
	Point position { packed.px, packed.py };

	player = {};
	player._pLevel = clamp<int8_t>(packed.pLevel, 1, MaxCharacterLevel);
	player._pMaxHPBase = SDL_SwapLE32(packed.pMaxHPBase);
	player._pHPBase = SDL_SwapLE32(packed.pHPBase);
	player._pHPBase = clamp<int32_t>(player._pHPBase, 0, player._pMaxHPBase);
	player._pMaxHP = player._pMaxHPBase;
	player._pHitPoints = player._pHPBase;
	player.position.tile = position;
	player.position.future = position;
	player.setLevel(clamp<int8_t>(packed.plrlevel, 0, NUMLEVELS));

	player._pClass = static_cast<HeroClass>(clamp<uint8_t>(packed.pClass, 0, enum_size<HeroClass>::value - 1));

	ClrPlrPath(player);
	player.destAction = ACTION_NONE;

	CopyUtf8(player._pName, packed.pName, sizeof(player._pName));

	InitPlayer(player, true);

	player._pBaseStr = std::min<uint8_t>(packed.pBaseStr, player.GetMaximumAttributeValue(CharacterAttribute::Strength));
	player._pStrength = player._pBaseStr;
	player._pBaseMag = std::min<uint8_t>(packed.pBaseMag, player.GetMaximumAttributeValue(CharacterAttribute::Magic));
	player._pMagic = player._pBaseMag;
	player._pBaseDex = std::min<uint8_t>(packed.pBaseDex, player.GetMaximumAttributeValue(CharacterAttribute::Dexterity));
	player._pDexterity = player._pBaseDex;
	player._pBaseVit = std::min<uint8_t>(packed.pBaseVit, player.GetMaximumAttributeValue(CharacterAttribute::Vitality));
	player._pVitality = player._pBaseVit;
	player._pStatPts = packed.pStatPts;

	player._pExperience = SDL_SwapLE32(packed.pExperience);
	player._pGold = SDL_SwapLE32(packed.pGold);
	player._pBaseToBlk = PlayersData[static_cast<std::size_t>(player._pClass)].blockBonus;
	if ((int)(player._pHPBase & 0xFFFFFFC0) < 64)
		player._pHPBase = 64;

	player._pMaxManaBase = SDL_SwapLE32(packed.pMaxManaBase);
	player._pManaBase = SDL_SwapLE32(packed.pManaBase);
	player._pManaBase = std::min<int32_t>(player._pManaBase, player._pMaxManaBase);
	player._pMemSpells = SDL_SwapLE64(packed.pMemSpells);

	for (int i = 0; i < 37; i++) // Should be MAX_SPELLS but set to 36 to make save games compatible
		player._pSplLvl[i] = packed.pSplLvl[i];
	for (int i = 37; i < 47; i++)
		player._pSplLvl[i] = packed.pSplLvl2[i - 37];

	bool isHellfire = packed.bIsHellfire != 0;

	for (int i = 0; i < NUM_INVLOC; i++)
		UnPackItem(packed.InvBody[i], player, player.InvBody[i], isHellfire);

	player._pNumInv = packed._pNumInv;
	for (int i = 0; i < player._pNumInv; i++)
		UnPackItem(packed.InvList[i], player, player.InvList[i], isHellfire);

	for (int i = 0; i < InventoryGridCells; i++)
		player.InvGrid[i] = packed.InvGrid[i];

	VerifyGoldSeeds(player);

	for (int i = 0; i < MaxBeltItems; i++)
		UnPackItem(packed.SpdList[i], player, player.SpdList[i], isHellfire);

	CalcPlrInv(player, false);
	player.wReflections = SDL_SwapLE16(packed.wReflections);
	player.pDiabloKillLevel = SDL_SwapLE32(packed.pDiabloKillLevel);
}

bool UnPackNetPlayer(const PlayerNetPack &packed, Player &player)
{
	if (packed.pClass >= enum_size<HeroClass>::value)
		return false;
	player._pClass = static_cast<HeroClass>(packed.pClass);

	Point position { packed.px, packed.py };
	if (!InDungeonBounds(position))
		return false;

	if (packed.plrlevel >= NUMLEVELS)
		return false;

	if (packed.pLevel > MaxCharacterLevel || packed.pLevel < 1)
		return false;

	int32_t baseHpMax = SDL_SwapLE32(packed.pMaxHPBase);
	int32_t baseHp = SDL_SwapLE32(packed.pHPBase);
	if (baseHp > baseHpMax || baseHp < 0)
		return false;

	int32_t baseManaMax = SDL_SwapLE32(packed.pMaxManaBase);
	int32_t baseMana = SDL_SwapLE32(packed.pManaBase);
	if (baseMana > baseManaMax)
		return false;

	if (packed.pBaseStr > player.GetMaximumAttributeValue(CharacterAttribute::Strength))
		return false;
	if (packed.pBaseMag > player.GetMaximumAttributeValue(CharacterAttribute::Magic))
		return false;
	if (packed.pBaseDex > player.GetMaximumAttributeValue(CharacterAttribute::Dexterity))
		return false;
	if (packed.pBaseVit > player.GetMaximumAttributeValue(CharacterAttribute::Vitality))
		return false;

	if (packed._pNumInv >= InventoryGridCells)
		return false;

	player._pLevel = packed.pLevel;
	player.position.tile = position;
	player.position.future = position;
	player.plrlevel = packed.plrlevel;
	player.plrIsOnSetLevel = packed.isOnSetLevel != 0;
	player._pMaxHPBase = baseHpMax;
	player._pHPBase = baseHp;
	player._pMaxHP = baseHpMax;
	player._pHitPoints = baseHp;

	ClrPlrPath(player);
	player.destAction = ACTION_NONE;

	CopyUtf8(player._pName, packed.pName, sizeof(player._pName));

	InitPlayer(player, true);

	player._pBaseStr = packed.pBaseStr;
	player._pStrength = player._pBaseStr;
	player._pBaseMag = packed.pBaseMag;
	player._pMagic = player._pBaseMag;
	player._pBaseDex = packed.pBaseDex;
	player._pDexterity = player._pBaseDex;
	player._pBaseVit = packed.pBaseVit;
	player._pVitality = player._pBaseVit;
	player._pStatPts = packed.pStatPts;

	player._pExperience = SDL_SwapLE32(packed.pExperience);
	player._pBaseToBlk = PlayersData[static_cast<std::size_t>(player._pClass)].blockBonus;
	player._pMaxManaBase = baseManaMax;
	player._pManaBase = baseMana;
	player._pMemSpells = SDL_SwapLE64(packed.pMemSpells);
	player.wReflections = SDL_SwapLE16(packed.wReflections);
	player.pDiabloKillLevel = packed.pDiabloKillLevel;
	player.pManaShield = packed.pManaShield != 0;
	player.friendlyMode = packed.friendlyMode != 0;

	for (int i = 0; i < MAX_SPELLS; i++)
		player._pSplLvl[i] = packed.pSplLvl[i];

	for (int i = 0; i < NUM_INVLOC; i++)
		UnPackNetItem(player, packed.InvBody[i], player.InvBody[i]);

	player._pNumInv = packed._pNumInv;
	for (int i = 0; i < player._pNumInv; i++)
		UnPackNetItem(player, packed.InvList[i], player.InvList[i]);

	for (int i = 0; i < InventoryGridCells; i++)
		player.InvGrid[i] = packed.InvGrid[i];

	for (int i = 0; i < MaxBeltItems; i++)
		UnPackNetItem(player, packed.SpdList[i], player.SpdList[i]);

	CalcPlrInv(player, false);
	player._pGold = CalculateGold(player);

	if (player._pStrength != SDL_SwapLE32(packed.pStrength))
		return false;
	if (player._pMagic != SDL_SwapLE32(packed.pMagic))
		return false;
	if (player._pDexterity != SDL_SwapLE32(packed.pDexterity))
		return false;
	if (player._pVitality != SDL_SwapLE32(packed.pVitality))
		return false;
	if (player._pHitPoints != SDL_SwapLE32(packed.pHitPoints))
		return false;
	if (player._pMaxHP != SDL_SwapLE32(packed.pMaxHP))
		return false;
	if (player._pMana != SDL_SwapLE32(packed.pMana))
		return false;
	if (player._pMaxMana != SDL_SwapLE32(packed.pMaxMana))
		return false;
	if (player._pDamageMod != SDL_SwapLE32(packed.pDamageMod))
		return false;
	if (player._pBaseToBlk != SDL_SwapLE32(packed.pBaseToBlk))
		return false;
	if (player._pIMinDam != SDL_SwapLE32(packed.pIMinDam))
		return false;
	if (player._pIMaxDam != SDL_SwapLE32(packed.pIMaxDam))
		return false;
	if (player._pIAC != SDL_SwapLE32(packed.pIAC))
		return false;
	if (player._pIBonusDam != SDL_SwapLE32(packed.pIBonusDam))
		return false;
	if (player._pIBonusToHit != SDL_SwapLE32(packed.pIBonusToHit))
		return false;
	if (player._pIBonusAC != SDL_SwapLE32(packed.pIBonusAC))
		return false;
	if (player._pIBonusDamMod != SDL_SwapLE32(packed.pIBonusDamMod))
		return false;
	if (player._pIGetHit != SDL_SwapLE32(packed.pIGetHit))
		return false;
	if (player._pIEnAc != SDL_SwapLE32(packed.pIEnAc))
		return false;
	if (player._pIFMinDam != SDL_SwapLE32(packed.pIFMinDam))
		return false;
	if (player._pIFMaxDam != SDL_SwapLE32(packed.pIFMaxDam))
		return false;
	if (player._pILMinDam != SDL_SwapLE32(packed.pILMinDam))
		return false;
	if (player._pILMaxDam != SDL_SwapLE32(packed.pILMaxDam))
		return false;
	if (player._pMaxHPBase > player.calculateBaseLife())
		return false;
	if (player._pMaxManaBase > player.calculateBaseMana())
		return false;

	return true;
}

} // namespace devilution

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
#include "plrmsg.h"
#include "stores.h"
#include "utils/endian.hpp"
#include "utils/log.hpp"
#include "utils/utf8.hpp"

#define ValidateField(logValue, condition)                         \
	do {                                                           \
		if (!(condition)) {                                        \
			LogFailedJoinAttempt(#condition, #logValue, logValue); \
			EventFailedJoinAttempt(player.name);                 \
			return false;                                          \
		}                                                          \
	} while (0)

#define ValidateFields(logValue1, logValue2, condition)                                     \
	do {                                                                                    \
		if (!(condition)) {                                                                 \
			LogFailedJoinAttempt(#condition, #logValue1, logValue1, #logValue2, logValue2); \
			EventFailedJoinAttempt(player.name);                                          \
			return false;                                                                   \
		}                                                                                   \
	} while (0)

namespace devilution {

namespace {

void EventFailedJoinAttempt(const char *playerName)
{
	std::string message = fmt::format("Player '{}' sent invalid player data during attempt to join the game.", playerName);
	EventPlrMsg(message);
}

template <typename T>
void LogFailedJoinAttempt(const char *condition, const char *name, T value)
{
	LogDebug("Remote player validation failed: ValidateField({}: {}, {})", name, value, condition);
}

template <typename T1, typename T2>
void LogFailedJoinAttempt(const char *condition, const char *name1, T1 value1, const char *name2, T2 value2)
{
	LogDebug("Remote player validation failed: ValidateFields({}: {}, {}: {}, {})", name1, value1, name2, value2, condition);
}

void VerifyGoldSeeds(Player &player)
{
	for (int i = 0; i < player.numInventoryItems; i++) {
		if (player.inventorySlot[i].IDidx != IDI_GOLD)
			continue;
		for (int j = 0; j < player.numInventoryItems; j++) {
			if (i == j)
				continue;
			if (player.inventorySlot[j].IDidx != IDI_GOLD)
				continue;
			if (player.inventorySlot[i]._iSeed != player.inventorySlot[j]._iSeed)
				continue;
			player.inventorySlot[i]._iSeed = AdvanceRndSeed();
			j = -1;
		}
	}
}

bool hasMultipleFlags(uint16_t flags)
{
	return (flags & (flags - 1)) > 0;
}

} // namespace

bool IsCreationFlagComboValid(uint16_t iCreateInfo)
{
	iCreateInfo = iCreateInfo & ~CF_LEVEL;
	const bool isTownItem = (iCreateInfo & CF_TOWN) != 0;
	const bool isPregenItem = (iCreateInfo & CF_PREGEN) != 0;
	const bool isUsefulItem = (iCreateInfo & CF_USEFUL) == CF_USEFUL;

	if (isPregenItem) {
		// Pregen flags are discarded when an item is picked up, therefore impossible to have in the inventory
		return false;
	}
	if (isUsefulItem && (iCreateInfo & ~CF_USEFUL) != 0)
		return false;
	if (isTownItem && hasMultipleFlags(iCreateInfo)) {
		// Items from town can only have 1 towner flag
		return false;
	}
	return true;
}

bool IsTownItemValid(uint16_t iCreateInfo, const Player &player)
{
	const uint8_t level = iCreateInfo & CF_LEVEL;
	const bool isBoyItem = (iCreateInfo & CF_BOY) != 0;
	const uint8_t maxTownItemLevel = 30;

	// Wirt items in multiplayer are equal to the level of the player, therefore they cannot exceed the max character level
	if (isBoyItem && level <= player.getMaxCharacterLevel())
		return true;

	return level <= maxTownItemLevel;
}

bool IsUniqueMonsterItemValid(uint16_t iCreateInfo, uint32_t dwBuff)
{
	const uint8_t level = iCreateInfo & CF_LEVEL;

	// Check all unique monster levels to see if they match the item level
	for (const UniqueMonsterData &uniqueMonsterData : UniqueMonstersData) {
		const auto &uniqueMonsterLevel = static_cast<uint8_t>(MonstersData[uniqueMonsterData.mtype].level);

		if (IsAnyOf(uniqueMonsterData.mtype, MT_DEFILER, MT_NAKRUL, MT_HORKDMN)) {
			// These monsters don't use their mlvl for item generation
			continue;
		}

		if (level == uniqueMonsterLevel) {
			// If the ilvl matches the mlvl, we confirm the item is legitimate
			return true;
		}
	}

	return false;
}

bool IsDungeonItemValid(uint16_t iCreateInfo, uint32_t dwBuff)
{
	const uint8_t level = iCreateInfo & CF_LEVEL;
	const bool isHellfireItem = (dwBuff & CF_HELLFIRE) != 0;

	// Check all monster levels to see if they match the item level
	for (int16_t i = 0; i < static_cast<int16_t>(NUM_MTYPES); i++) {
		const auto &monsterData = MonstersData[i];
		auto monsterLevel = static_cast<uint8_t>(monsterData.level);

		if (i != MT_DIABLO && monsterData.availability == MonsterAvailability::Never) {
			// Skip monsters that are unable to appear in the game
			continue;
		}

		if (i == MT_DIABLO && !isHellfireItem) {
			// Adjust The Dark Lord's mlvl if the item isn't a Hellfire item to match the Diablo mlvl
			monsterLevel -= 15;
		}

		if (level == monsterLevel) {
			// If the ilvl matches the mlvl, we confirm the item is legitimate
			return true;
		}
	}

	if (isHellfireItem) {
		uint8_t hellfireMaxDungeonLevel = 24;

		// Hellfire adjusts the currlevel minus 7 in dungeon levels 20-24 for generating items
		hellfireMaxDungeonLevel -= 7;
		return level <= (hellfireMaxDungeonLevel * 2);
	}

	uint8_t diabloMaxDungeonLevel = 16;

	// Diablo doesn't have containers that drop items in dungeon level 16, therefore we decrement by 1
	diabloMaxDungeonLevel--;
	return level <= (diabloMaxDungeonLevel * 2);
}

bool RecreateHellfireSpellBook(const Player &player, const TItem &packedItem, Item *item)
{
	Item spellBook {};
	RecreateItem(player, packedItem, spellBook);

	// Hellfire uses the spell book level when generating items via CreateSpellBook()
	int spellBookLevel = GetSpellBookLevel(spellBook._iSpell);

	// CreateSpellBook() adds 1 to the spell level for ilvl
	spellBookLevel++;

	if (spellBookLevel >= 1 && (spellBook._iCreateInfo & CF_LEVEL) == spellBookLevel * 2) {
		// The ilvl matches the result for a spell book drop, so we confirm the item is legitimate
		if (item != nullptr)
			*item = spellBook;
		return true;
	}

	ValidateFields(spellBook._iCreateInfo, spellBook.dwBuff, IsDungeonItemValid(spellBook._iCreateInfo, spellBook.dwBuff));
	if (item != nullptr)
		*item = spellBook;
	return true;
}

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
			if (item._iMaxDur > 255)
				packedItem.bMDur = 254;
			else
				packedItem.bMDur = item._iMaxDur;
			packedItem.bDur = std::min<int32_t>(item._iDurability, packedItem.bMDur);

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
	packed.destinationAction = player.destinationAction;
	packed.destinationParam1 = player.destinationParam1;
	packed.destinationParam2 = player.destinationParam2;
	packed.dungeonLevel = player.dungeonLevel;
	packed.px = player.position.tile.x;
	packed.py = player.position.tile.y;
	if (gbVanilla) {
		packed.targx = player.position.tile.x;
		packed.targy = player.position.tile.y;
	}
	CopyUtf8(packed.pName, player.name, sizeof(packed.pName));
	packed.pClass = static_cast<uint8_t>(player.heroClass);
	packed.pBaseStr = player.baseStrength;
	packed.pBaseMag = player.baseMagic;
	packed.pBaseDex = player.baseDexterity;
	packed.pBaseVit = player.baseVitality;
	packed.pLevel = player.getCharacterLevel();
	packed.pStatPts = player.statPoints;
	packed.pExperience = SDL_SwapLE32(player.experience);
	packed.pGold = SDL_SwapLE32(player.gold);
	packed.pHPBase = SDL_SwapLE32(player.baseLife);
	packed.pMaxHPBase = SDL_SwapLE32(player.baseMaxLife);
	packed.pManaBase = SDL_SwapLE32(player.baseMana);
	packed.pMaxManaBase = SDL_SwapLE32(player.baseMaxMana);
	packed.pMemSpells = SDL_SwapLE64(player.learnedSpells);

	for (int i = 0; i < 37; i++) // Should be MAX_SPELLS but set to 37 to make save games compatible
		packed.pSplLvl[i] = player.spellLevel[i];
	for (int i = 37; i < 47; i++)
		packed.pSplLvl2[i - 37] = player.spellLevel[i];

	for (int i = 0; i < NUM_INVLOC; i++)
		PackItem(packed.bodySlot[i], player.bodySlot[i], gbIsHellfire);

	packed.numInventoryItems = player.numInventoryItems;
	for (int i = 0; i < packed.numInventoryItems; i++)
		PackItem(packed.inventorySlot[i], player.inventorySlot[i], gbIsHellfire);

	for (int i = 0; i < InventoryGridCells; i++)
		packed.inventoryGrid[i] = player.inventoryGrid[i];

	for (int i = 0; i < MaxBeltItems; i++)
		PackItem(packed.beltSlot[i], player.beltSlot[i], gbIsHellfire);

	packed.reflections = SDL_SwapLE16(player.reflections);
	packed.hellfireFlags = SDL_SwapLE32(static_cast<uint32_t>(player.hellfireFlags));
	packed.difficultyCompletion = SDL_SwapLE32(player.difficultyCompletion);
	packed.bIsHellfire = gbIsHellfire ? 1 : 0;
}

void PackNetItem(const Item &item, ItemNetPack &packedItem)
{
	if (item.isEmpty()) {
		packedItem.def.wIndx = static_cast<_item_indexes>(0xFFFF);
		return;
	}
	packedItem.def.wIndx = static_cast<_item_indexes>(SDL_SwapLE16(item.IDidx));
	packedItem.def.wCI = SDL_SwapLE16(item._iCreateInfo);
	packedItem.def.dwSeed = SDL_SwapLE32(item._iSeed);
	if (item.IDidx != IDI_EAR)
		PrepareItemForNetwork(item, packedItem.item);
	else
		PrepareEarForNetwork(item, packedItem.ear);
}

void PackNetPlayer(PlayerNetPack &packed, const Player &player)
{
	packed.dungeonLevel = player.dungeonLevel;
	packed.px = player.position.tile.x;
	packed.py = player.position.tile.y;
	CopyUtf8(packed.pName, player.name, sizeof(packed.pName));
	packed.pClass = static_cast<uint8_t>(player.heroClass);
	packed.pBaseStr = player.baseStrength;
	packed.pBaseMag = player.baseMagic;
	packed.pBaseDex = player.baseDexterity;
	packed.pBaseVit = player.baseVitality;
	packed.pLevel = player.getCharacterLevel();
	packed.pStatPts = player.statPoints;
	packed.pExperience = SDL_SwapLE32(player.experience);
	packed.pHPBase = SDL_SwapLE32(player.baseLife);
	packed.pMaxHPBase = SDL_SwapLE32(player.baseMaxLife);
	packed.pManaBase = SDL_SwapLE32(player.baseMana);
	packed.pMaxManaBase = SDL_SwapLE32(player.baseMaxMana);
	packed.pMemSpells = SDL_SwapLE64(player.learnedSpells);

	for (int i = 0; i < MAX_SPELLS; i++)
		packed.pSplLvl[i] = player.spellLevel[i];

	for (int i = 0; i < NUM_INVLOC; i++)
		PackNetItem(player.bodySlot[i], packed.bodySlot[i]);

	packed.numInventoryItems = player.numInventoryItems;
	for (int i = 0; i < packed.numInventoryItems; i++)
		PackNetItem(player.inventorySlot[i], packed.inventorySlot[i]);

	for (int i = 0; i < InventoryGridCells; i++)
		packed.inventoryGrid[i] = player.inventoryGrid[i];

	for (int i = 0; i < MaxBeltItems; i++)
		PackNetItem(player.beltSlot[i], packed.beltSlot[i]);

	packed.reflections = SDL_SwapLE16(player.reflections);
	packed.difficultyCompletion = player.difficultyCompletion;
	packed.hasManaShield = player.hasManaShield;
	packed.isFriendlyMode = player.isFriendlyMode ? 1 : 0;
	packed.isOnSetLevel = player.isOnSetLevel;

	packed.pStrength = SDL_SwapLE32(player.strength);
	packed.pMagic = SDL_SwapLE32(player.magic);
	packed.pDexterity = SDL_SwapLE32(player.dexterity);
	packed.pVitality = SDL_SwapLE32(player.vitality);
	packed.pHitPoints = SDL_SwapLE32(player.life);
	packed.pMaxHP = SDL_SwapLE32(player.maxLife);
	packed.pMana = SDL_SwapLE32(player.mana);
	packed.pMaxMana = SDL_SwapLE32(player.maxMana);
	packed.pDamageMod = SDL_SwapLE32(player.damageModifier);
	// we pack base to block as a basic check that remote players are using the same playerdat values as we are
	packed.pBaseToBlk = SDL_SwapLE32(player.getBaseToBlock());
	packed.pIMinDam = SDL_SwapLE32(player.minDamage);
	packed.pIMaxDam = SDL_SwapLE32(player.maxDamage);
	packed.pIAC = SDL_SwapLE32(player.armorClass);
	packed.pIBonusDam = SDL_SwapLE32(player.bonusDamagePercent);
	packed.pIBonusToHit = SDL_SwapLE32(player.bonusToHit);
	packed.pIBonusAC = SDL_SwapLE32(player.bonusArmorClass);
	packed.pIBonusDamMod = SDL_SwapLE32(player.bonusDamage);
	packed.pIGetHit = SDL_SwapLE32(player.damageFromEnemies);
	packed.pIEnAc = SDL_SwapLE32(player.armorPierce);
	packed.pIFMinDam = SDL_SwapLE32(player.minFireDamage);
	packed.pIFMaxDam = SDL_SwapLE32(player.maxFireDamage);
	packed.pILMinDam = SDL_SwapLE32(player.minLightningDamage);
	packed.pILMaxDam = SDL_SwapLE32(player.maxLightningDamage);
}

void UnPackItem(const ItemPack &packedItem, const Player &player, Item &item, bool isHellfire)
{
	if (packedItem.idx == 0xFFFF) {
		item.clear();
		return;
	}

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
		item.dwBuff = SDL_SwapLE32(packedItem.dwBuff);
		RecreateItem(player, item, idx, SDL_SwapLE16(packedItem.iCreateInfo), SDL_SwapLE32(packedItem.iSeed), SDL_SwapLE16(packedItem.wValue), isHellfire);
		item._iIdentified = (packedItem.bId & 1) != 0;
		item._iMaxDur = packedItem.bMDur;
		item._iDurability = ClampDurability(item, packedItem.bDur);
		item._iMaxCharges = std::clamp<int>(packedItem.bMCh, 0, item._iMaxCharges);
		item._iCharges = std::clamp<int>(packedItem.bCh, 0, item._iMaxCharges);
	}
}

void UnPackPlayer(const PlayerPack &packed, Player &player)
{
	Point position { packed.px, packed.py };

	player = {};
	player.setCharacterLevel(packed.pLevel);
	player.baseMaxLife = SDL_SwapLE32(packed.pMaxHPBase);
	player.baseLife = SDL_SwapLE32(packed.pHPBase);
	player.baseLife = std::clamp<int32_t>(player.baseLife, 0, player.baseMaxLife);
	player.maxLife = player.baseMaxLife;
	player.life = player.baseLife;
	player.position.tile = position;
	player.position.future = position;
	player.setLevel(std::clamp<int8_t>(packed.dungeonLevel, 0, NUMLEVELS));

	player.heroClass = static_cast<HeroClass>(std::clamp<uint8_t>(packed.pClass, 0, enum_size<HeroClass>::value - 1));

	ClrPlrPath(player);
	player.destinationAction = ACTION_NONE;

	CopyUtf8(player.name, packed.pName, sizeof(player.name));

	InitPlayer(player, true);

	player.baseStrength = std::min<uint8_t>(packed.pBaseStr, player.GetMaximumAttributeValue(CharacterAttribute::Strength));
	player.strength = player.baseStrength;
	player.baseMagic = std::min<uint8_t>(packed.pBaseMag, player.GetMaximumAttributeValue(CharacterAttribute::Magic));
	player.magic = player.baseMagic;
	player.baseDexterity = std::min<uint8_t>(packed.pBaseDex, player.GetMaximumAttributeValue(CharacterAttribute::Dexterity));
	player.dexterity = player.baseDexterity;
	player.baseVitality = std::min<uint8_t>(packed.pBaseVit, player.GetMaximumAttributeValue(CharacterAttribute::Vitality));
	player.vitality = player.baseVitality;
	player.statPoints = packed.pStatPts;

	player.experience = SDL_SwapLE32(packed.pExperience);
	player.gold = SDL_SwapLE32(packed.pGold);
	if ((int)(player.baseLife & 0xFFFFFFC0) < 64)
		player.baseLife = 64;

	player.baseMaxMana = SDL_SwapLE32(packed.pMaxManaBase);
	player.baseMana = SDL_SwapLE32(packed.pManaBase);
	player.baseMana = std::min<int32_t>(player.baseMana, player.baseMaxMana);
	player.learnedSpells = SDL_SwapLE64(packed.pMemSpells);

	// Only read spell levels for learnable spells (Diablo)
	for (int i = 0; i < 37; i++) { // Should be MAX_SPELLS but set to 36 to make save games compatible
		auto spl = static_cast<SpellID>(i);
		if (GetSpellData(spl).sBookLvl != -1)
			player.spellLevel[i] = packed.pSplLvl[i];
		else
			player.spellLevel[i] = 0;
	}
	// Only read spell levels for learnable spells (Hellfire)
	for (int i = 37; i < 47; i++) {
		auto spl = static_cast<SpellID>(i);
		if (GetSpellData(spl).sBookLvl != -1)
			player.spellLevel[i] = packed.pSplLvl2[i - 37];
		else
			player.spellLevel[i] = 0;
	}
	// These spells are unavailable in Diablo as learnable spells
	if (!gbIsHellfire) {
		player.spellLevel[static_cast<uint8_t>(SpellID::Apocalypse)] = 0;
		player.spellLevel[static_cast<uint8_t>(SpellID::Nova)] = 0;
	}

	bool isHellfire = packed.bIsHellfire != 0;

	for (int i = 0; i < NUM_INVLOC; i++)
		UnPackItem(packed.bodySlot[i], player, player.bodySlot[i], isHellfire);

	player.numInventoryItems = packed.numInventoryItems;
	for (int i = 0; i < player.numInventoryItems; i++)
		UnPackItem(packed.inventorySlot[i], player, player.inventorySlot[i], isHellfire);

	for (int i = 0; i < InventoryGridCells; i++)
		player.inventoryGrid[i] = packed.inventoryGrid[i];

	VerifyGoldSeeds(player);

	for (int i = 0; i < MaxBeltItems; i++)
		UnPackItem(packed.beltSlot[i], player, player.beltSlot[i], isHellfire);

	CalcPlrInv(player, false);
	player.reflections = SDL_SwapLE16(packed.reflections);
	player.difficultyCompletion = SDL_SwapLE32(packed.difficultyCompletion);
}

bool UnPackNetItem(const Player &player, const ItemNetPack &packedItem, Item &item)
{
	item = {};
	_item_indexes idx = static_cast<_item_indexes>(SDL_SwapLE16(packedItem.def.wIndx));
	if (idx < 0 || idx > IDI_LAST)
		return true;
	if (idx == IDI_EAR) {
		RecreateEar(item, SDL_SwapLE16(packedItem.ear.wCI), SDL_SwapLE32(packedItem.ear.dwSeed), packedItem.ear.bCursval, packedItem.ear.heroname);
		return true;
	}

	uint16_t creationFlags = SDL_SwapLE16(packedItem.item.wCI);
	uint32_t dwBuff = SDL_SwapLE16(packedItem.item.dwBuff);
	if (idx != IDI_GOLD)
		ValidateField(creationFlags, IsCreationFlagComboValid(creationFlags));
	if ((creationFlags & CF_TOWN) != 0)
		ValidateField(creationFlags, IsTownItemValid(creationFlags, player));
	else if ((creationFlags & CF_USEFUL) == CF_UPER15)
		ValidateFields(creationFlags, dwBuff, IsUniqueMonsterItemValid(creationFlags, dwBuff));
	else if ((dwBuff & CF_HELLFIRE) != 0 && AllItemsList[idx].iMiscId == IMISC_BOOK)
		return RecreateHellfireSpellBook(player, packedItem.item, &item);
	else
		ValidateFields(creationFlags, dwBuff, IsDungeonItemValid(creationFlags, dwBuff));

	RecreateItem(player, packedItem.item, item);
	return true;
}

bool UnPackNetPlayer(const PlayerNetPack &packed, Player &player)
{
	CopyUtf8(player.name, packed.pName, sizeof(player.name));

	ValidateField(packed.pClass, packed.pClass < enum_size<HeroClass>::value);
	player.heroClass = static_cast<HeroClass>(packed.pClass);

	Point position { packed.px, packed.py };
	ValidateFields(position.x, position.y, InDungeonBounds(position));
	ValidateField(packed.dungeonLevel, packed.dungeonLevel < NUMLEVELS);
	ValidateField(packed.pLevel, packed.pLevel >= 1 && packed.pLevel <= player.getMaxCharacterLevel());

	int32_t baseHpMax = SDL_SwapLE32(packed.pMaxHPBase);
	int32_t baseHp = SDL_SwapLE32(packed.pHPBase);
	int32_t hpMax = SDL_SwapLE32(packed.pMaxHP);
	ValidateFields(baseHp, baseHpMax, baseHp >= (baseHpMax - hpMax) && baseHp <= baseHpMax);

	int32_t baseManaMax = SDL_SwapLE32(packed.pMaxManaBase);
	int32_t baseMana = SDL_SwapLE32(packed.pManaBase);
	ValidateFields(baseMana, baseManaMax, baseMana <= baseManaMax);

	ValidateFields(packed.pClass, packed.pBaseStr, packed.pBaseStr <= player.GetMaximumAttributeValue(CharacterAttribute::Strength));
	ValidateFields(packed.pClass, packed.pBaseMag, packed.pBaseMag <= player.GetMaximumAttributeValue(CharacterAttribute::Magic));
	ValidateFields(packed.pClass, packed.pBaseDex, packed.pBaseDex <= player.GetMaximumAttributeValue(CharacterAttribute::Dexterity));
	ValidateFields(packed.pClass, packed.pBaseVit, packed.pBaseVit <= player.GetMaximumAttributeValue(CharacterAttribute::Vitality));

	ValidateField(packed.numInventoryItems, packed.numInventoryItems <= InventoryGridCells);

	player.setCharacterLevel(packed.pLevel);
	player.position.tile = position;
	player.position.future = position;
	player.dungeonLevel = packed.dungeonLevel;
	player.isOnSetLevel = packed.isOnSetLevel != 0;
	player.baseMaxLife = baseHpMax;
	player.baseLife = baseHp;
	player.maxLife = baseHpMax;
	player.life = baseHp;

	ClrPlrPath(player);
	player.destinationAction = ACTION_NONE;

	InitPlayer(player, true);

	player.baseStrength = packed.pBaseStr;
	player.strength = player.baseStrength;
	player.baseMagic = packed.pBaseMag;
	player.magic = player.baseMagic;
	player.baseDexterity = packed.pBaseDex;
	player.dexterity = player.baseDexterity;
	player.baseVitality = packed.pBaseVit;
	player.vitality = player.baseVitality;
	player.statPoints = packed.pStatPts;

	player.experience = SDL_SwapLE32(packed.pExperience);
	player.baseMaxMana = baseManaMax;
	player.baseMana = baseMana;
	player.learnedSpells = SDL_SwapLE64(packed.pMemSpells);
	player.reflections = SDL_SwapLE16(packed.reflections);
	player.difficultyCompletion = packed.difficultyCompletion;
	player.hasManaShield = packed.hasManaShield != 0;
	player.isFriendlyMode = packed.isFriendlyMode != 0;

	for (int i = 0; i < MAX_SPELLS; i++)
		player.spellLevel[i] = packed.pSplLvl[i];

	for (int i = 0; i < NUM_INVLOC; i++) {
		if (!UnPackNetItem(player, packed.bodySlot[i], player.bodySlot[i]))
			return false;
		if (player.bodySlot[i].isEmpty())
			continue;
		auto loc = static_cast<int8_t>(player.GetItemLocation(player.bodySlot[i]));
		switch (i) {
		case INVLOC_HEAD:
			ValidateField(loc, loc == ILOC_HELM);
			break;
		case INVLOC_RING_LEFT:
		case INVLOC_RING_RIGHT:
			ValidateField(loc, loc == ILOC_RING);
			break;
		case INVLOC_AMULET:
			ValidateField(loc, loc == ILOC_AMULET);
			break;
		case INVLOC_HAND_LEFT:
		case INVLOC_HAND_RIGHT:
			ValidateField(loc, IsAnyOf(loc, ILOC_ONEHAND, ILOC_TWOHAND));
			break;
		case INVLOC_CHEST:
			ValidateField(loc, loc == ILOC_ARMOR);
			break;
		}
	}

	player.numInventoryItems = packed.numInventoryItems;
	for (int i = 0; i < player.numInventoryItems; i++) {
		if (!UnPackNetItem(player, packed.inventorySlot[i], player.inventorySlot[i]))
			return false;
	}

	for (int i = 0; i < InventoryGridCells; i++)
		player.inventoryGrid[i] = packed.inventoryGrid[i];

	for (int i = 0; i < MaxBeltItems; i++) {
		Item &item = player.beltSlot[i];
		if (!UnPackNetItem(player, packed.beltSlot[i], item))
			return false;
		if (item.isEmpty())
			continue;
		Size beltItemSize = GetInventorySize(item);
		int8_t beltItemType = static_cast<int8_t>(item._itype);
		bool beltItemUsable = item.isUsable();
		ValidateFields(beltItemSize.width, beltItemSize.height, (beltItemSize == Size { 1, 1 }));
		ValidateField(beltItemType, item._itype != ItemType::Gold);
		ValidateField(beltItemUsable, beltItemUsable);
	}

	CalcPlrInv(player, false);
	player.gold = CalculateGold(player);

	ValidateFields(player.strength, SDL_SwapLE32(packed.pStrength), player.strength == SDL_SwapLE32(packed.pStrength));
	ValidateFields(player.magic, SDL_SwapLE32(packed.pMagic), player.magic == SDL_SwapLE32(packed.pMagic));
	ValidateFields(player.dexterity, SDL_SwapLE32(packed.pDexterity), player.dexterity == SDL_SwapLE32(packed.pDexterity));
	ValidateFields(player.vitality, SDL_SwapLE32(packed.pVitality), player.vitality == SDL_SwapLE32(packed.pVitality));
	ValidateFields(player.life, SDL_SwapLE32(packed.pHitPoints), player.life == SDL_SwapLE32(packed.pHitPoints));
	ValidateFields(player.maxLife, SDL_SwapLE32(packed.pMaxHP), player.maxLife == SDL_SwapLE32(packed.pMaxHP));
	ValidateFields(player.mana, SDL_SwapLE32(packed.pMana), player.mana == SDL_SwapLE32(packed.pMana));
	ValidateFields(player.maxMana, SDL_SwapLE32(packed.pMaxMana), player.maxMana == SDL_SwapLE32(packed.pMaxMana));
	ValidateFields(player.damageModifier, SDL_SwapLE32(packed.pDamageMod), player.damageModifier == SDL_SwapLE32(packed.pDamageMod));
	ValidateFields(player.getBaseToBlock(), SDL_SwapLE32(packed.pBaseToBlk), player.getBaseToBlock() == SDL_SwapLE32(packed.pBaseToBlk));
	ValidateFields(player.minDamage, SDL_SwapLE32(packed.pIMinDam), player.minDamage == SDL_SwapLE32(packed.pIMinDam));
	ValidateFields(player.maxDamage, SDL_SwapLE32(packed.pIMaxDam), player.maxDamage == SDL_SwapLE32(packed.pIMaxDam));
	ValidateFields(player.armorClass, SDL_SwapLE32(packed.pIAC), player.armorClass == SDL_SwapLE32(packed.pIAC));
	ValidateFields(player.bonusDamagePercent, SDL_SwapLE32(packed.pIBonusDam), player.bonusDamagePercent == SDL_SwapLE32(packed.pIBonusDam));
	ValidateFields(player.bonusToHit, SDL_SwapLE32(packed.pIBonusToHit), player.bonusToHit == SDL_SwapLE32(packed.pIBonusToHit));
	ValidateFields(player.bonusArmorClass, SDL_SwapLE32(packed.pIBonusAC), player.bonusArmorClass == SDL_SwapLE32(packed.pIBonusAC));
	ValidateFields(player.bonusDamage, SDL_SwapLE32(packed.pIBonusDamMod), player.bonusDamage == SDL_SwapLE32(packed.pIBonusDamMod));
	ValidateFields(player.damageFromEnemies, SDL_SwapLE32(packed.pIGetHit), player.damageFromEnemies == SDL_SwapLE32(packed.pIGetHit));
	ValidateFields(player.armorPierce, SDL_SwapLE32(packed.pIEnAc), player.armorPierce == SDL_SwapLE32(packed.pIEnAc));
	ValidateFields(player.minFireDamage, SDL_SwapLE32(packed.pIFMinDam), player.minFireDamage == SDL_SwapLE32(packed.pIFMinDam));
	ValidateFields(player.maxFireDamage, SDL_SwapLE32(packed.pIFMaxDam), player.maxFireDamage == SDL_SwapLE32(packed.pIFMaxDam));
	ValidateFields(player.minLightningDamage, SDL_SwapLE32(packed.pILMinDam), player.minLightningDamage == SDL_SwapLE32(packed.pILMinDam));
	ValidateFields(player.maxLightningDamage, SDL_SwapLE32(packed.pILMaxDam), player.maxLightningDamage == SDL_SwapLE32(packed.pILMaxDam));
	ValidateFields(player.baseMaxLife, player.calculateBaseLife(), player.baseMaxLife <= player.calculateBaseLife());
	ValidateFields(player.baseMaxMana, player.calculateBaseMana(), player.baseMaxMana <= player.calculateBaseMana());

	return true;
}

} // namespace devilution

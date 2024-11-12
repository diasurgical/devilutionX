/**
 * @file items/validation.cpp
 *
 * Implementation of functions for validation of player and item data.
 */

#include "items/validation.h"

#include <cstdint>

#include "items.h"
#include "monstdat.h"
#include "player.h"

namespace devilution {

namespace {

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

bool IsTownItemValid(uint16_t iCreateInfo, uint8_t maxCharacterLevel)
{
	const uint8_t level = iCreateInfo & CF_LEVEL;
	const bool isBoyItem = (iCreateInfo & CF_BOY) != 0;
	const uint8_t maxTownItemLevel = 30;

	// Wirt items in multiplayer are equal to the level of the player, therefore they cannot exceed the max character level
	if (isBoyItem && level <= maxCharacterLevel)
		return true;

	return level <= maxTownItemLevel;
}

bool IsShopPriceValid(const Item &item)
{
	const int boyPriceLimit = gbIsHellfire ? MAX_BOY_VALUE_HF : MAX_BOY_VALUE;
	if ((item._iCreateInfo & CF_BOY) != 0 && item._iIvalue > boyPriceLimit)
		return false;

	const uint16_t smithOrWitch = CF_SMITH | CF_SMITHPREMIUM | CF_WITCH;
	const int smithAndWitchPriceLimit = gbIsHellfire ? MAX_VENDOR_VALUE_HF : MAX_VENDOR_VALUE;
	if ((item._iCreateInfo & smithOrWitch) != 0 && item._iIvalue > smithAndWitchPriceLimit)
		return false;

	return true;
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

bool IsItemValid(const Player &player, const Item &item)
{
	bool isValid = true;

	if (item.IDidx != IDI_GOLD)
		isValid = isValid && IsCreationFlagComboValid(item._iCreateInfo);

	if ((item._iCreateInfo & CF_TOWN) != 0)
		isValid = isValid && IsTownItemValid(item._iCreateInfo, player.getMaxCharacterLevel()) && IsShopPriceValid(item);
	else if ((item._iCreateInfo & CF_USEFUL) == CF_UPER15)
		isValid = isValid && IsUniqueMonsterItemValid(item._iCreateInfo, item.dwBuff);

	isValid = isValid && IsDungeonItemValid(item._iCreateInfo, item.dwBuff);

	return isValid;
}

} // namespace devilution

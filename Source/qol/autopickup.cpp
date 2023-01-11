/**
 * @file autopickup.cpp
 *
 * QoL feature for automatically picking up gold
 */

#include "inv_iterators.hpp"
#include "options.h"
#include "player.h"
#include <algorithm>

namespace devilution {
namespace {

bool HasRoomForGold()
{
	for (int idx : MyPlayer->InvGrid) {
		// Secondary item cell. No need to check those as we'll go through the main item cells anyway.
		if (idx < 0)
			continue;

		// Empty cell. 1x1 space available.
		if (idx == 0)
			return true;

		// Main item cell. Potentially a gold pile so check it.
		auto item = MyPlayer->InvList[idx - 1];
		if (item._itype == ItemType::Gold && item._ivalue < MaxGold)
			return true;
	}

	return false;
}

int NumMiscItemsInInv(ItemMiscID iMiscId)
{
	InventoryAndBeltPlayerItemsRange items { *MyPlayer };
	return std::count_if(items.begin(), items.end(), [iMiscId](const Item &item) { return item._iMiscId == iMiscId; });
}

bool DoPickup(Item item)
{
	if (item._itype == ItemType::Gold && *sgOptions.Gameplay.autoGoldPickup && HasRoomForGold())
		return true;

	if (item._itype == ItemType::Misc
	    && (AutoPlaceItemInInventory(*MyPlayer, item, false) || AutoPlaceItemInBelt(*MyPlayer, item, false))) {
		switch (item._iMiscId) {
		case ItemMiscID::PotionOfHealing:
			return *sgOptions.Gameplay.numHealPotionPickup > NumMiscItemsInInv(item._iMiscId);
		case ItemMiscID::PotionOfFullHealing:
			return *sgOptions.Gameplay.numFullHealPotionPickup > NumMiscItemsInInv(item._iMiscId);
		case ItemMiscID::PotionOfMana:
			return *sgOptions.Gameplay.numManaPotionPickup > NumMiscItemsInInv(item._iMiscId);
		case ItemMiscID::PotionOfFullMana:
			return *sgOptions.Gameplay.numFullManaPotionPickup > NumMiscItemsInInv(item._iMiscId);
		case ItemMiscID::PotionOfRejuvenation:
			return *sgOptions.Gameplay.numRejuPotionPickup > NumMiscItemsInInv(item._iMiscId);
		case ItemMiscID::PotionOfFullRejuvenation:
			return *sgOptions.Gameplay.numFullRejuPotionPickup > NumMiscItemsInInv(item._iMiscId);
		case ItemMiscID::ElixirOfStrength:
		case ItemMiscID::ElixirOfMagic:
		case ItemMiscID::ElixirOfDexterity:
		case ItemMiscID::ElixirOfVitality:
			return *sgOptions.Gameplay.autoElixirPickup;
		case ItemMiscID::OilFirst:
		case ItemMiscID::Oil:
		case ItemMiscID::OilOfAccuracy:
		case ItemMiscID::OilOfMastery:
		case ItemMiscID::OilOfSharpness:
		case ItemMiscID::OilOfDeath:
		case ItemMiscID::OilOfSkill:
		case ItemMiscID::OilBlacksmith:
		case ItemMiscID::OilOfFortitude:
		case ItemMiscID::OilOfPermanence:
		case ItemMiscID::OilOfHardening:
		case ItemMiscID::OilOfImperviousness:
		case ItemMiscID::OilLast:
			return *sgOptions.Gameplay.autoOilPickup;
		default:
			return false;
		}
	}

	return false;
}

} // namespace

void AutoPickup(const Player &player)
{
	if (&player != MyPlayer)
		return;
	if (leveltype == DTYPE_TOWN && !*sgOptions.Gameplay.autoPickupInTown)
		return;

	for (auto pathDir : PathDirs) {
		Point tile = player.position.tile + pathDir;
		if (dItem[tile.x][tile.y] != 0) {
			int itemIndex = dItem[tile.x][tile.y] - 1;
			auto &item = Items[itemIndex];
			if (DoPickup(item)) {
				NetSendCmdGItem(true, CMD_REQUESTAGITEM, player.getId(), itemIndex);
				item._iRequest = true;
			}
		}
	}
}
} // namespace devilution

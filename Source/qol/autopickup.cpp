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

int NumMiscItemsInInv(int iMiscId)
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
		case IMISC_HEAL:
			return *sgOptions.Gameplay.numHealPotionPickup > NumMiscItemsInInv(item._iMiscId);
		case IMISC_FULLHEAL:
			return *sgOptions.Gameplay.numFullHealPotionPickup > NumMiscItemsInInv(item._iMiscId);
		case IMISC_MANA:
			return *sgOptions.Gameplay.numManaPotionPickup > NumMiscItemsInInv(item._iMiscId);
		case IMISC_FULLMANA:
			return *sgOptions.Gameplay.numFullManaPotionPickup > NumMiscItemsInInv(item._iMiscId);
		case IMISC_REJUV:
			return *sgOptions.Gameplay.numRejuPotionPickup > NumMiscItemsInInv(item._iMiscId);
		case IMISC_FULLREJUV:
			return *sgOptions.Gameplay.numFullRejuPotionPickup > NumMiscItemsInInv(item._iMiscId);
		case IMISC_ELIXSTR:
		case IMISC_ELIXMAG:
		case IMISC_ELIXDEX:
		case IMISC_ELIXVIT:
			return *sgOptions.Gameplay.autoElixirPickup;
		case IMISC_OILFIRST:
		case IMISC_OILOF:
		case IMISC_OILACC:
		case IMISC_OILMAST:
		case IMISC_OILSHARP:
		case IMISC_OILDEATH:
		case IMISC_OILSKILL:
		case IMISC_OILBSMTH:
		case IMISC_OILFORT:
		case IMISC_OILPERM:
		case IMISC_OILHARD:
		case IMISC_OILIMP:
		case IMISC_OILLAST:
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

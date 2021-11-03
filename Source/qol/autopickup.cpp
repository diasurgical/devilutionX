/**
* @file autopickup.cpp
*
* QoL feature for automatically picking up gold
*/

#include "options.h"
#include "player.h"
#include "inv_iterators.hpp"

namespace devilution {
namespace {

bool HasRoomForGold()
{
	for (int idx : Players[MyPlayerId].InvGrid) {
		// Secondary item cell. No need to check those as we'll go through the main item cells anyway.
		if (idx < 0)
			continue;

		// Empty cell. 1x1 space available.
		if (idx == 0)
			return true;

		// Main item cell. Potentially a gold pile so check it.
		auto item = Players[MyPlayerId].InvList[idx - 1];
		if (item._itype == ItemType::Gold && item._ivalue < MaxGold)
			return true;
	}

	return false;
}

int numMiscItemsInInv(int iMiscId)
{
	int numItems = 0;
	for (const auto &item : InventoryAndBeltPlayerItemsRange(Players[MyPlayerId])) {
		if (item._iMiscId == iMiscId) {
			numItems++;
		}
	}
	return numItems;
}

} // namespace

void AutoGoldPickup(int pnum)
{
	if (!sgOptions.Gameplay.bAutoGoldPickup)
		return;

	if (pnum != MyPlayerId)
		return;
	if (leveltype == DTYPE_TOWN)
		return;
	if (!HasRoomForGold())
		return;

	for (auto pathDir : PathDirs) {
		Point tile = Players[pnum].position.tile + pathDir;
		if (dItem[tile.x][tile.y] != 0) {
			int itemIndex = dItem[tile.x][tile.y] - 1;
			auto &item = Items[itemIndex];
			if (item._itype == ItemType::Gold) {
				NetSendCmdGItem(true, CMD_REQUESTAGITEM, pnum, pnum, itemIndex);
				item._iRequest = true;
			}
		}
	}
}

void AutoItemPickup(int pnum)
{
	for (auto pathDir : PathDirs) {
		Point tile = Players[pnum].position.tile + pathDir;
		if (dItem[tile.x][tile.y] != 0) {
			int itemIndex = dItem[tile.x][tile.y] - 1;
			auto &item = Items[itemIndex];
			if (AutoPlaceItemInInventory(Players[pnum], item, false) || AutoPlaceItemInBelt(Players[pnum], item, false)) {
				bool doPickup = false;
				switch (item._iMiscId) {
				case IMISC_HEAL:
					doPickup = sgOptions.Gameplay.nHealPotionPickup > numMiscItemsInInv(item._iMiscId);
					break;
				case IMISC_FULLHEAL:
					doPickup = sgOptions.Gameplay.nFullHealPotionPickup > numMiscItemsInInv(item._iMiscId);
					break;
				case IMISC_MANA:
					doPickup = sgOptions.Gameplay.nManaPotionPickup > numMiscItemsInInv(item._iMiscId);
					break;
				case IMISC_FULLMANA:
					doPickup = sgOptions.Gameplay.nFullManaPotionPickup > numMiscItemsInInv(item._iMiscId);
					break;
				case IMISC_REJUV:
					doPickup = sgOptions.Gameplay.nRejuPotionPickup > numMiscItemsInInv(item._iMiscId);
					break;
				case IMISC_FULLREJUV:
					doPickup = sgOptions.Gameplay.nFullRejuPotionPickup > numMiscItemsInInv(item._iMiscId);
					break;
				default:
					break;
				}
				if (doPickup) {
					NetSendCmdGItem(true, CMD_REQUESTAGITEM, pnum, pnum, itemIndex);
					item._iRequest = true;
				}
			}
		}
	}
}
} // namespace devilution

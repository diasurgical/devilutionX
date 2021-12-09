/**
 * @file autopickup.cpp
 *
 * QoL feature for automatically picking up gold
 */

#include "inv_iterators.hpp"
#include "options.h"
#include "player.h"

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

int NumMiscItemsInInv(int iMiscId)
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

void AutoPickup(int pnum)
{
	if (pnum != MyPlayerId)
		return;
	if (leveltype == DTYPE_TOWN && !*sgOptions.Gameplay.autoPickupInTown)
		return;

	bool hasRoomForGold = HasRoomForGold();

	for (auto pathDir : PathDirs) {
		Point tile = Players[pnum].position.tile + pathDir;
		if (dItem[tile.x][tile.y] != 0) {
			int itemIndex = dItem[tile.x][tile.y] - 1;
			auto &item = Items[itemIndex];
			if (hasRoomForGold && item._itype == ItemType::Gold && sgOptions.Gameplay.autoGoldPickup) {
				NetSendCmdGItem(true, CMD_REQUESTAGITEM, pnum, pnum, itemIndex);
				item._iRequest = true;
			}
			if (item._itype == ItemType::Misc && (AutoPlaceItemInInventory(Players[pnum], item, false) || AutoPlaceItemInBelt(Players[pnum], item, false))) {
				bool doPickup = false;
				switch (item._iMiscId) {
				case IMISC_HEAL:
					doPickup = sgOptions.Gameplay.numHealPotionPickup > NumMiscItemsInInv(item._iMiscId);
					break;
				case IMISC_FULLHEAL:
					doPickup = sgOptions.Gameplay.numFullHealPotionPickup > NumMiscItemsInInv(item._iMiscId);
					break;
				case IMISC_MANA:
					doPickup = sgOptions.Gameplay.numManaPotionPickup > NumMiscItemsInInv(item._iMiscId);
					break;
				case IMISC_FULLMANA:
					doPickup = sgOptions.Gameplay.numFullManaPotionPickup > NumMiscItemsInInv(item._iMiscId);
					break;
				case IMISC_REJUV:
					doPickup = sgOptions.Gameplay.numRejuPotionPickup > NumMiscItemsInInv(item._iMiscId);
					break;
				case IMISC_FULLREJUV:
					doPickup = sgOptions.Gameplay.numFullRejuPotionPickup > NumMiscItemsInInv(item._iMiscId);
					break;
				case IMISC_ELIXSTR:
				case IMISC_ELIXMAG:
				case IMISC_ELIXDEX:
				case IMISC_ELIXVIT:
					doPickup = sgOptions.Gameplay.AutoElixirPickup;
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

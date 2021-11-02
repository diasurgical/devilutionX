/**
* @file autopickup.cpp
*
* QoL feature for automatically picking up gold
*/

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

int numMiscItemsInInv(int iMiscId)
{
	int numItems = 0;
	for (int i = 0; i < Players[MyPlayerId]._pNumInv; i++) {
		if (Players[MyPlayerId].InvList[i]._iMiscId == iMiscId) {
			numItems++;
		}
	}

	for (int i = 0; i < MAXBELTITEMS; i++) {
		if (!Players[MyPlayerId].SpdList[i].isEmpty() && Players[MyPlayerId].SpdList[i]._iMiscId == iMiscId) {
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
				if (sgOptions.Gameplay.nHealPotionPickup > numMiscItemsInInv(IMISC_HEAL) && item._iMiscId == IMISC_HEAL) {
					NetSendCmdGItem(true, CMD_REQUESTAGITEM, pnum, pnum, itemIndex);
					item._iRequest = true;
				}
				if (sgOptions.Gameplay.nFullHealPotionPickup > numMiscItemsInInv(IMISC_FULLHEAL) && item._iMiscId == IMISC_FULLHEAL) {
					NetSendCmdGItem(true, CMD_REQUESTAGITEM, pnum, pnum, itemIndex);
					item._iRequest = true;
				}
				if (sgOptions.Gameplay.nManaPotionPickup > numMiscItemsInInv(IMISC_MANA) && item._iMiscId == IMISC_MANA) {
					NetSendCmdGItem(true, CMD_REQUESTAGITEM, pnum, pnum, itemIndex);
					item._iRequest = true;
				}
				if (sgOptions.Gameplay.nFullManaPotionPickup > numMiscItemsInInv(IMISC_FULLMANA) && item._iMiscId == IMISC_FULLMANA) {
					NetSendCmdGItem(true, CMD_REQUESTAGITEM, pnum, pnum, itemIndex);
					item._iRequest = true;
				}
				if (sgOptions.Gameplay.nRejuPotionPickup > numMiscItemsInInv(IMISC_REJUV) && item._iMiscId == IMISC_REJUV) {
					NetSendCmdGItem(true, CMD_REQUESTAGITEM, pnum, pnum, itemIndex);
					item._iRequest = true;
				}
				if (sgOptions.Gameplay.nFullRejuPotionPickup > numMiscItemsInInv(IMISC_FULLREJUV) && item._iMiscId == IMISC_FULLREJUV) {
					NetSendCmdGItem(true, CMD_REQUESTAGITEM, pnum, pnum, itemIndex);
					item._iRequest = true;
				}
			}
		}
	}
}

} // namespace devilution

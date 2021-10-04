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

} // namespace devilution

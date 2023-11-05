#ifdef _DEBUG
#include "lua/modules/dev/items.hpp"

#include <string>

#include <sol/sol.hpp>

#include "cursor.h"
#include "items.h"
#include "lua/metadoc.hpp"
#include "pack.h"
#include "player.h"
#include "utils/str_cat.hpp"

namespace devilution {

namespace {

std::string DebugCmdItemInfo()
{
	Player &myPlayer = *MyPlayer;
	Item *pItem = nullptr;
	if (!myPlayer.HoldItem.isEmpty()) {
		pItem = &myPlayer.HoldItem;
	} else if (pcursinvitem != -1) {
		if (pcursinvitem <= INVITEM_INV_LAST)
			pItem = &myPlayer.InvList[pcursinvitem - INVITEM_INV_FIRST];
		else
			pItem = &myPlayer.SpdList[pcursinvitem - INVITEM_BELT_FIRST];
	} else if (pcursitem != -1) {
		pItem = &Items[pcursitem];
	}
	if (pItem != nullptr) {
		std::string_view netPackValidation { "N/A" };
		if (gbIsMultiplayer) {
			ItemNetPack itemPack;
			Item unpacked;
			PackNetItem(*pItem, itemPack);
			netPackValidation = UnPackNetItem(myPlayer, itemPack, unpacked) ? "Success" : "Failure";
		}
		return StrCat("Name: ", pItem->_iIName,
		    "\nIDidx: ", pItem->IDidx, " (", AllItemsList[pItem->IDidx].iName, ")",
		    "\nSeed: ", pItem->_iSeed,
		    "\nCreateInfo: ", pItem->_iCreateInfo,
		    "\nLevel: ", pItem->_iCreateInfo & CF_LEVEL,
		    "\nOnly Good: ", ((pItem->_iCreateInfo & CF_ONLYGOOD) == 0) ? "False" : "True",
		    "\nUnique Monster: ", ((pItem->_iCreateInfo & CF_UPER15) == 0) ? "False" : "True",
		    "\nDungeon Item: ", ((pItem->_iCreateInfo & CF_UPER1) == 0) ? "False" : "True",
		    "\nUnique Item: ", ((pItem->_iCreateInfo & CF_UNIQUE) == 0) ? "False" : "True",
		    "\nSmith: ", ((pItem->_iCreateInfo & CF_SMITH) == 0) ? "False" : "True",
		    "\nSmith Premium: ", ((pItem->_iCreateInfo & CF_SMITHPREMIUM) == 0) ? "False" : "True",
		    "\nBoy: ", ((pItem->_iCreateInfo & CF_BOY) == 0) ? "False" : "True",
		    "\nWitch: ", ((pItem->_iCreateInfo & CF_WITCH) == 0) ? "False" : "True",
		    "\nHealer: ", ((pItem->_iCreateInfo & CF_HEALER) == 0) ? "False" : "True",
		    "\nPregen: ", ((pItem->_iCreateInfo & CF_PREGEN) == 0) ? "False" : "True",
		    "\nNet Validation: ", netPackValidation);
	}
	return StrCat("Num items: ", ActiveItemCount);
}

} // namespace

sol::table LuaDevItemsModule(sol::state_view &lua)
{
	sol::table table = lua.create_table();
	SetDocumented(table, "info", "()", "Show info of currently selected item.", &DebugCmdItemInfo);
	SetDocumented(table, "spawn", "(name: string)", "Attempt to generate an item.", &DebugSpawnItem);
	SetDocumented(table, "spawnUnique", "(name: string)", "Attempt to generate a unique item.", &DebugSpawnUniqueItem);
	return table;
}

} // namespace devilution
#endif // _DEBUG

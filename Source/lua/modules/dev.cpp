#ifdef _DEBUG
#include "lua/modules/dev.hpp"

#include <sol/sol.hpp>

#include "automap.h"
#include "debug.h"
#include "items.h"
#include "lua/metadoc.hpp"
#include "lua/modules/dev/quests.hpp"
#include "player.h"
#include "spells.h"
#include "utils/str_cat.hpp"

namespace devilution {

namespace {

std::string DebugCmdShowGrid()
{
	DebugGrid = !DebugGrid;
	return StrCat("Tile grid highlighting: ", DebugGrid ? "On" : "Off");
}

std::string DebugCmdLevelUp(int levels = 1)
{
	if (levels <= 0) return "amount must be positive";
	Player &myPlayer = *MyPlayer;
	for (int i = 0; i < levels; i++)
		NetSendCmd(true, CMD_CHEAT_EXPERIENCE);
	return StrCat("New character level: ", myPlayer.getCharacterLevel() + levels);
}

std::string DebugCmdMaxStats()
{
	Player &myPlayer = *MyPlayer;
	ModifyPlrStr(myPlayer, myPlayer.GetMaximumAttributeValue(CharacterAttribute::Strength) - myPlayer._pBaseStr);
	ModifyPlrMag(myPlayer, myPlayer.GetMaximumAttributeValue(CharacterAttribute::Magic) - myPlayer._pBaseMag);
	ModifyPlrDex(myPlayer, myPlayer.GetMaximumAttributeValue(CharacterAttribute::Dexterity) - myPlayer._pBaseDex);
	ModifyPlrVit(myPlayer, myPlayer.GetMaximumAttributeValue(CharacterAttribute::Vitality) - myPlayer._pBaseVit);
	return "Set all character base attributes to maximum.";
}

std::string DebugCmdMinStats()
{
	Player &myPlayer = *MyPlayer;
	ModifyPlrStr(myPlayer, -myPlayer._pBaseStr);
	ModifyPlrMag(myPlayer, -myPlayer._pBaseMag);
	ModifyPlrDex(myPlayer, -myPlayer._pBaseDex);
	ModifyPlrVit(myPlayer, -myPlayer._pBaseVit);
	return "Set all character base attributes to minimum.";
}

std::string DebugCmdGiveGoldCheat(int goldToAdd = GOLD_MAX_LIMIT * InventoryGridCells)
{
	if (goldToAdd <= 0) return "amount must be positive";
	Player &myPlayer = *MyPlayer;
	const int goldAmountBefore = myPlayer._pGold;
	for (int8_t &itemIndex : myPlayer.InvGrid) {
		if (itemIndex < 0)
			continue;

		Item &item = myPlayer.InvList[itemIndex != 0 ? itemIndex - 1 : myPlayer._pNumInv];

		if (itemIndex != 0) {
			if ((!item.isGold() && !item.isEmpty()) || (item.isGold() && item._ivalue == GOLD_MAX_LIMIT))
				continue;
		} else {
			if (item.isEmpty()) {
				MakeGoldStack(item, 0);
				myPlayer._pNumInv++;
				itemIndex = myPlayer._pNumInv;
			}
		}

		int goldThatCanBeAdded = (GOLD_MAX_LIMIT - item._ivalue);
		if (goldThatCanBeAdded >= goldToAdd) {
			item._ivalue += goldToAdd;
			myPlayer._pGold += goldToAdd;
			break;
		}

		item._ivalue += goldThatCanBeAdded;
		goldToAdd -= goldThatCanBeAdded;
		myPlayer._pGold += goldThatCanBeAdded;
	}

	CalcPlrInv(myPlayer, true);

	return StrCat("Set your gold to ", myPlayer._pGold, ", added ", myPlayer._pGold - goldAmountBefore, ".");
}

std::string DebugCmdTakeGoldCheat(int goldToRemove = GOLD_MAX_LIMIT * InventoryGridCells)
{
	Player &myPlayer = *MyPlayer;
	if (goldToRemove <= 0) return "amount must be positive";

	const int goldAmountBefore = myPlayer._pGold;
	for (auto itemIndex : myPlayer.InvGrid) {
		itemIndex -= 1;

		if (itemIndex < 0)
			continue;

		Item &item = myPlayer.InvList[itemIndex];
		if (!item.isGold())
			continue;

		if (item._ivalue >= goldToRemove) {
			myPlayer._pGold -= goldToRemove;
			item._ivalue -= goldToRemove;
			if (item._ivalue == 0)
				myPlayer.RemoveInvItem(itemIndex);
			break;
		}

		myPlayer._pGold -= item._ivalue;
		goldToRemove -= item._ivalue;
		myPlayer.RemoveInvItem(itemIndex);
	}

	return StrCat("Set your gold to ", myPlayer._pGold, ", removed ", goldAmountBefore - myPlayer._pGold, ".");
}

std::string DebugCmdSetSpellsLevel(uint8_t level)
{
	for (uint8_t i = static_cast<uint8_t>(SpellID::Firebolt); i < MAX_SPELLS; i++) {
		if (GetSpellBookLevel(static_cast<SpellID>(i)) != -1) {
			NetSendCmdParam2(true, CMD_CHANGE_SPELL_LEVEL, i, level);
		}
	}
	if (level == 0)
		MyPlayer->_pMemSpells = 0;

	return StrCat("Set all spell levels to ", level);
}

std::string DebugCmdMapReveal()
{
	for (int x = 0; x < DMAXX; x++)
		for (int y = 0; y < DMAXY; y++)
			UpdateAutomapExplorer({ x, y }, MAP_EXP_SHRINE);
	return "Automap fully explored.";
}

std::string DebugCmdMapHide()
{
	for (int x = 0; x < DMAXX; x++)
		for (int y = 0; y < DMAXY; y++)
			AutomapView[x][y] = MAP_EXP_NONE;
	return "Automap exploration removed.";
}

} // namespace

sol::table LuaDevModule(sol::state_view &lua)
{
	sol::table table = lua.create_table();
	SetWithSignatureAndDoc(table, "grid", "()",
	    "Toggles showing grid.", &DebugCmdShowGrid);
	SetWithSignatureAndDoc(table, "giveGold", "(amount: number = MAX)",
	    "Gives the player gold.",
	    sol::overload(
	        []() { return DebugCmdGiveGoldCheat(); },
	        [](int amount) { return DebugCmdGiveGoldCheat(amount); }));
	SetWithSignatureAndDoc(table, "giveLvl", "(amount: number = 1)",
	    "Levels the player up.",
	    sol::overload(
	        []() { return DebugCmdLevelUp(); },
	        [](int amount) { return DebugCmdLevelUp(amount); }));
	SetWithSignatureAndDoc(table, "giveMap", "()",
	    "Reveal the map.",
	    &DebugCmdMapReveal);
	SetWithSignatureAndDoc(table, "quests", "",
	    "Quest-related commands.",
	    LuaDevQuestsModule(lua));
	SetWithSignatureAndDoc(table, "takeGold", "(amount: number = MAX)",
	    "Takes the player's gold away.",
	    sol::overload(
	        []() { return DebugCmdTakeGoldCheat(); },
	        [](int amount) { return DebugCmdTakeGoldCheat(amount); }));
	SetWithSignatureAndDoc(table, "takeMap", "()",
	    "Hide the map.",
	    &DebugCmdMapHide);
	SetWithSignatureAndDoc(table, "maxStats", "()",
	    "Sets all stat values to maximum.",
	    &DebugCmdMaxStats);
	SetWithSignatureAndDoc(table, "minStats", "()",
	    "Sets all stat values to minimum.",
	    &DebugCmdMinStats);
	SetWithSignatureAndDoc(table, "setSpells", "(level: number)",
	    "Set spell level for all spells.", &DebugCmdSetSpellsLevel);
	return table;
}

} // namespace devilution
#endif // _DEBUG

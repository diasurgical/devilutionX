/**
 * @file debug.cpp
 *
 * Implementation of debug functions.
 */

#ifdef _DEBUG

#include <sstream>

#include <fmt/format.h>

#include "debug.h"

#include "automap.h"
#include "control.h"
#include "cursor.h"
#include "engine/load_cel.hpp"
#include "engine/point.hpp"
#include "error.h"
#include "inv.h"
#include "lighting.h"
#include "monstdat.h"
#include "monster.h"
#include "quests.h"
#include "setmaps.h"
#include "spells.h"
#include "towners.h"
#include "utils/language.h"
#include "utils/log.hpp"

namespace devilution {

std::optional<CelSprite> pSquareCel;
bool DebugToggle = false;
bool DebugGodMode = false;
bool DebugVision = false;
bool DebugGrid = false;
std::unordered_map<int, Point> DebugCoordsMap;
bool DebugScrollViewEnabled = false;
std::unordered_map<int, int> DebugIndexToObjectID;

namespace {

enum class DebugGridTextItem : uint16_t {
	None,
	dPiece,
	dTransVal,
	dLight,
	dPreLight,
	dFlags,
	dPlayer,
	dMonster,
	dCorpse,
	dObject,
	dItem,
	dSpecial,

	coords,
	cursorcoords,
	objectindex,

	// take dPiece as index
	nBlockTable,
	nSolidTable,
	nTransTable,
	nMissileTable,
	nTrapTable,

	// megatiles
	AutomapView,
	dungeon,
	pdungeon,
	dflags,
};

DebugGridTextItem SelectedDebugGridTextItem;

int DebugMonsterId;

// Used for debugging level generation
uint32_t glMid1Seed[NUMLEVELS];
uint32_t glMid2Seed[NUMLEVELS];
uint32_t glMid3Seed[NUMLEVELS];
uint32_t glEndSeed[NUMLEVELS];

void SetSpellLevelCheat(spell_id spl, int spllvl)
{
	auto &myPlayer = Players[MyPlayerId];

	myPlayer._pMemSpells |= GetSpellBitmask(spl);
	myPlayer._pSplLvl[spl] = spllvl;
}

void PrintDebugMonster(int m)
{
	char dstr[MAX_SEND_STR_LEN];

	auto &monster = Monsters[m];

	sprintf(dstr, "Monster %i = %s", m, monster.mName);
	NetSendCmdString(1 << MyPlayerId, dstr);
	sprintf(dstr, "X = %i, Y = %i", monster.position.tile.x, monster.position.tile.y);
	NetSendCmdString(1 << MyPlayerId, dstr);
	sprintf(dstr, "Enemy = %i, HP = %i", monster._menemy, monster._mhitpoints);
	NetSendCmdString(1 << MyPlayerId, dstr);
	sprintf(dstr, "Mode = %i, Var1 = %i", static_cast<int>(monster._mmode), monster._mVar1);
	NetSendCmdString(1 << MyPlayerId, dstr);

	bool bActive = false;

	for (int i = 0; i < ActiveMonsterCount; i++) {
		if (ActiveMonsters[i] == m)
			bActive = true;
	}

	sprintf(dstr, "Active List = %i, Squelch = %i", bActive ? 1 : 0, monster._msquelch);
	NetSendCmdString(1 << MyPlayerId, dstr);
}

void ProcessMessages()
{
	tagMSG msg;
	while (FetchMessage(&msg)) {
		if (msg.message == DVL_WM_QUIT) {
			gbRunGameResult = false;
			gbRunGame = false;
			break;
		}
		TranslateMessage(&msg);
		PushMessage(&msg);
	}
}

struct DebugCmdItem {
	const string_view text;
	const string_view description;
	const string_view requiredParameter;
	std::string (*actionProc)(const string_view);
};

extern std::vector<DebugCmdItem> DebugCmdList;

std::string DebugCmdHelp(const string_view parameter)
{
	if (parameter.empty()) {
		std::string ret = "Available Debug Commands: ";
		bool first = true;
		for (const auto &dbgCmd : DebugCmdList) {
			if (first)
				first = false;
			else
				ret.append(" - ");
			ret.append(std::string(dbgCmd.text));
		}
		return ret;
	} else {
		auto debugCmdIterator = std::find_if(DebugCmdList.begin(), DebugCmdList.end(), [&](const DebugCmdItem &elem) { return elem.text == parameter; });
		if (debugCmdIterator == DebugCmdList.end())
			return fmt::format("Debug command {} wasn't found", parameter);
		auto &dbgCmdItem = *debugCmdIterator;
		if (dbgCmdItem.requiredParameter.empty())
			return fmt::format("Description: {}\nParameters: No additional parameter needed.", dbgCmdItem.description);
		return fmt::format("Description: {}\nParameters: {}", dbgCmdItem.description, dbgCmdItem.requiredParameter);
	}
}

std::string DebugCmdGiveGoldCheat(const string_view parameter)
{
	auto &myPlayer = Players[MyPlayerId];

	for (int8_t &itemId : myPlayer.InvGrid) {
		if (itemId != 0)
			continue;

		int ni = myPlayer._pNumInv++;
		SetPlrHandItem(myPlayer.InvList[ni], IDI_GOLD);
		GetPlrHandSeed(&myPlayer.InvList[ni]);
		myPlayer.InvList[ni]._ivalue = GOLD_MAX_LIMIT;
		myPlayer.InvList[ni]._iCurs = ICURS_GOLD_LARGE;
		myPlayer._pGold += GOLD_MAX_LIMIT;
		itemId = myPlayer._pNumInv;
	}
	CalcPlrInv(myPlayer, true);

	return "You are now rich! If only this was as easy in real life...";
}

std::string DebugCmdTakeGoldCheat(const string_view parameter)
{
	auto &myPlayer = Players[MyPlayerId];

	for (auto itemId : myPlayer.InvGrid) {
		itemId -= 1;

		if (itemId < 0)
			continue;
		if (myPlayer.InvList[itemId]._itype != ItemType::Gold)
			continue;

		myPlayer.RemoveInvItem(itemId);
	}

	myPlayer._pGold = 0;

	return "You are poor...";
}

std::string DebugCmdWarpToLevel(const string_view parameter)
{
	auto &myPlayer = Players[MyPlayerId];
	auto level = atoi(parameter.data());
	if (level < 0 || level > (gbIsHellfire ? 24 : 16))
		return fmt::format("Level {} is not known. Do you want to write a mod?", level);
	if (!setlevel && myPlayer.plrlevel == level)
		return fmt::format("I did nothing but fulfilled your wish. You are already at level {}.", level);

	StartNewLvl(MyPlayerId, (level != 21) ? interface_mode::WM_DIABNEXTLVL : interface_mode::WM_DIABTOWNWARP, level);
	return fmt::format("Welcome to level {}.", level);
}

std::string DebugCmdLoadMap(const string_view parameter)
{
	if (parameter.empty()) {
		std::string ret = "What mapid do you want to visit?";
		for (auto &quest : Quests) {
			if (quest._qslvl <= 0)
				continue;
			ret.append(fmt::format(" {} ({})", quest._qslvl, QuestLevelNames[quest._qslvl]));
		}
		return ret;
	}

	auto level = atoi(parameter.data());
	if (level < 1)
		return fmt::format("Map id must be 1 or higher", level);
	if (setlevel && setlvlnum == level)
		return fmt::format("I did nothing but fulfilled your wish. You are already at mapid {}.", level);

	for (auto &quest : Quests) {
		if (level != quest._qslvl)
			continue;

		StartNewLvl(MyPlayerId, (quest._qlevel != 21) ? interface_mode::WM_DIABNEXTLVL : interface_mode::WM_DIABTOWNWARP, quest._qlevel);
		ProcessMessages();

		setlvltype = quest._qlvltype;
		StartNewLvl(MyPlayerId, WM_DIABSETLVL, level);

		return fmt::format("Welcome to {}.", QuestLevelNames[level]);
	}

	return fmt::format("Mapid {} is not known. Do you want to write a mod?", level);
}

std::unordered_map<string_view, _talker_id> TownerShortNameToTownerId = {
	{ "griswold", _talker_id::TOWN_SMITH },
	{ "pepin", _talker_id::TOWN_HEALER },
	{ "ogden", _talker_id::TOWN_TAVERN },
	{ "cain", _talker_id::TOWN_STORY },
	{ "farnham", _talker_id::TOWN_DRUNK },
	{ "adria", _talker_id::TOWN_WITCH },
	{ "gillian", _talker_id::TOWN_BMAID },
	{ "wirt", _talker_id ::TOWN_PEGBOY },
	{ "lester", _talker_id ::TOWN_FARMER },
	{ "girl", _talker_id ::TOWN_GIRL },
	{ "nut", _talker_id::TOWN_COWFARM },
};

std::string DebugCmdVisitTowner(const string_view parameter)
{
	auto &myPlayer = Players[MyPlayerId];

	if (setlevel || myPlayer.plrlevel != 0)
		return "What kind of friends do you have in dungeons?";

	if (parameter.empty()) {
		std::string ret;
		ret = "Who? ";
		for (auto &entry : TownerShortNameToTownerId) {
			ret.append(" ");
			ret.append(std::string(entry.first));
		}
		return ret;
	}

	auto it = TownerShortNameToTownerId.find(parameter);
	if (it == TownerShortNameToTownerId.end())
		return fmt::format("{} is unknown. Perhaps he is a ninja?", parameter);

	for (auto &towner : Towners) {
		if (towner._ttype != it->second)
			continue;

		CastSpell(
		    MyPlayerId,
		    SPL_TELEPORT,
		    myPlayer.position.tile.x,
		    myPlayer.position.tile.y,
		    towner.position.x,
		    towner.position.y,
		    1);

		return fmt::format("Say hello to {} from me.", parameter);
	}

	return fmt::format("Couldn't find {}.", parameter);
}

std::string DebugCmdResetLevel(const string_view parameter)
{
	auto &myPlayer = Players[MyPlayerId];

	std::stringstream paramsStream(parameter.data());
	std::string singleParameter;
	if (!std::getline(paramsStream, singleParameter, ' '))
		return "What level do you want to visit?";
	auto level = atoi(singleParameter.c_str());
	if (level < 0 || level > (gbIsHellfire ? 24 : 16))
		return fmt::format("Level {} is not known. Do you want to write an extension mod?", level);
	myPlayer._pLvlVisited[level] = false;

	if (std::getline(paramsStream, singleParameter, ' ')) {
		uint32_t seed = static_cast<uint32_t>(std::stoul(singleParameter));
		glSeedTbl[level] = seed;
	}

	if (myPlayer.plrlevel == level)
		return fmt::format("Level {} can't be cleaned, cause you still occupy it!", level);
	return fmt::format("Level {} was restored and looks fabulous.", level);
}

std::string DebugCmdGodMode(const string_view parameter)
{
	DebugGodMode = !DebugGodMode;
	if (DebugGodMode)
		return "A god descended.";
	return "You are mortal, beware of the darkness.";
}

std::string DebugCmdLighting(const string_view parameter)
{
	ToggleLighting();

	return "All raindrops are the same.";
}

std::string DebugCmdMapReveal(const string_view parameter)
{
	for (int x = 0; x < DMAXX; x++)
		for (int y = 0; y < DMAXY; y++)
			UpdateAutomapExplorer({ x, y }, MAP_EXP_SHRINE);

	return "The way is made clear when viewed from above";
}

std::string DebugCmdMapHide(const string_view parameter)
{
	for (int x = 0; x < DMAXX; x++)
		for (int y = 0; y < DMAXY; y++)
			AutomapView[x][y] = MAP_EXP_NONE;

	return "The way is made unclear when viewed from below";
}

std::string DebugCmdVision(const string_view parameter)
{
	DebugVision = !DebugVision;
	if (DebugVision)
		return "You see as I do.";

	return "My path is set.";
}

std::string DebugCmdQuest(const string_view parameter)
{
	if (parameter.empty()) {
		std::string ret = "You must provide an id. This could be: all";
		for (auto &quest : Quests) {
			if (IsNoneOf(quest._qactive, QUEST_NOTAVAIL, QUEST_INIT))
				continue;
			ret.append(fmt::format(", {} ({})", quest._qidx, QuestsData[quest._qidx]._qlstr));
		}
		return ret;
	}

	if (parameter.compare("all") == 0) {
		for (auto &quest : Quests) {
			if (IsNoneOf(quest._qactive, QUEST_NOTAVAIL, QUEST_INIT))
				continue;

			quest._qactive = QUEST_ACTIVE;
			quest._qlog = true;
		}

		return "Happy questing";
	}

	int questId = atoi(parameter.data());

	if (questId >= MAXQUESTS)
		return fmt::format("Quest {} is not known. Do you want to write a mod?", questId);
	auto &quest = Quests[questId];

	if (IsNoneOf(quest._qactive, QUEST_NOTAVAIL, QUEST_INIT))
		return fmt::format("{} was already given.", QuestsData[questId]._qlstr);

	quest._qactive = QUEST_ACTIVE;
	quest._qlog = true;

	return fmt::format("{} enabled.", QuestsData[questId]._qlstr);
}

std::string DebugCmdLevelUp(const string_view parameter)
{
	int levels = std::max(1, atoi(parameter.data()));
	for (int i = 0; i < levels; i++)
		NetSendCmd(true, CMD_CHEAT_EXPERIENCE);
	return "New experience leads to new insights.";
}

std::string DebugCmdSetSpellsLevel(const string_view parameter)
{
	int level = std::max(0, atoi(parameter.data()));
	for (int i = SPL_FIREBOLT; i < MAX_SPELLS; i++) {
		if (GetSpellBookLevel((spell_id)i) != -1) {
			SetSpellLevelCheat((spell_id)i, level);
		}
	}
	if (level == 0)
		Players[MyPlayerId]._pMemSpells = 0;

	return "Knowledge is power.";
}

std::string DebugCmdRefillHealthMana(const string_view parameter)
{
	auto &myPlayer = Players[MyPlayerId];
	myPlayer._pMana = myPlayer._pMaxMana;
	myPlayer._pManaBase = myPlayer._pMaxManaBase;
	myPlayer._pHitPoints = myPlayer._pMaxHP;
	myPlayer._pHPBase = myPlayer._pMaxHPBase;
	drawhpflag = true;
	drawmanaflag = true;

	return "Ready for more.";
}

std::string DebugCmdGenerateUniqueItem(const string_view parameter)
{
	return DebugSpawnUniqueItem(parameter.data());
}

std::string DebugCmdGenerateItem(const string_view parameter)
{
	return DebugSpawnItem(parameter.data());
}

std::string DebugCmdExit(const string_view parameter)
{
	gbRunGame = false;
	gbRunGameResult = false;
	return "See you again my Lord.";
}

std::string DebugCmdArrow(const string_view parameter)
{
	auto &myPlayer = Players[MyPlayerId];

	myPlayer._pIFlags &= ~ISPL_FIRE_ARROWS;
	myPlayer._pIFlags &= ~ISPL_LIGHT_ARROWS;

	if (parameter == "normal") {
		// we removed the parameter at the top
	} else if (parameter == "fire") {
		myPlayer._pIFlags |= ISPL_FIRE_ARROWS;
	} else if (parameter == "lightning") {
		myPlayer._pIFlags |= ISPL_LIGHT_ARROWS;
	} else if (parameter == "explosion") {
		myPlayer._pIFlags |= (ISPL_FIRE_ARROWS | ISPL_LIGHT_ARROWS);
	} else {
		return "Unknown is sometimes similar to nothing (unkown effect).";
	}

	return "I can shoot any arrow.";
}

std::string DebugCmdTalkToTowner(const string_view parameter)
{
	if (DebugTalkToTowner(parameter.data())) {
		return "Hello from the other side.";
	}
	return "NPC not found.";
}

std::string DebugCmdShowGrid(const string_view parameter)
{
	DebugGrid = !DebugGrid;
	if (DebugGrid)
		return "A basket full of rectangles and mushrooms.";

	return "Back to boring.";
}

std::string DebugCmdLevelSeed(const string_view parameter)
{
	return fmt::format("Seedinfo for level {}\nseed: {}\nMid1: {}\nMid2: {}\nMid3: {}\nEnd: {}", currlevel, glSeedTbl[currlevel], glMid1Seed[currlevel], glMid2Seed[currlevel], glMid3Seed[currlevel], glEndSeed[currlevel]);
}

std::string DebugCmdSpawnMonster(const string_view parameter)
{
	if (currlevel == 0)
		return "Do you want to kill the towners?!?";

	std::stringstream paramsStream(parameter.data());
	std::string name;
	int count = 1;
	if (std::getline(paramsStream, name, ' ')) {
		count = atoi(name.c_str());
		if (count > 0)
			name.clear();
		else
			count = 1;
		std::getline(paramsStream, name, ' ');
	}

	std::string singleWord;
	while (std::getline(paramsStream, singleWord, ' ')) {
		name.append(" ");
		name.append(singleWord);
	}

	std::transform(name.begin(), name.end(), name.begin(), [](unsigned char c) { return std::tolower(c); });

	int mtype = -1;
	for (int i = 0; i < 138; i++) {
		auto mondata = MonstersData[i];
		std::string monsterName(mondata.mName);
		std::transform(monsterName.begin(), monsterName.end(), monsterName.begin(), [](unsigned char c) { return std::tolower(c); });
		if (monsterName.find(name) == std::string::npos)
			continue;
		mtype = i;
		break;
	}

	if (mtype == -1) {
		for (int i = 0; i < 100; i++) {
			auto mondata = UniqueMonstersData[i];
			std::string monsterName(mondata.mName);
			std::transform(monsterName.begin(), monsterName.end(), monsterName.begin(), [](unsigned char c) { return std::tolower(c); });
			if (monsterName.find(name) == std::string::npos)
				continue;
			mtype = mondata.mtype;
			break;
		}
	}

	if (mtype == -1)
		return "Monster not found!";

	int id = MAX_LVLMTYPES - 1;
	bool found = false;

	for (int i = 0; i < LevelMonsterTypeCount; i++) {
		if (LevelMonsterTypes[i].mtype == mtype) {
			id = i;
			found = true;
			break;
		}
	}

	if (!found) {
		LevelMonsterTypes[id].mtype = static_cast<_monster_id>(mtype);
		InitMonsterGFX(id);
		LevelMonsterTypes[id].mPlaceFlags |= PLACE_SCATTER;
		LevelMonsterTypes[id].mdeadval = 1;
	}

	auto &myPlayer = Players[MyPlayerId];

	int spawnedMonster = 0;

	for (int k : CrawlNum) {
		int ck = k + 2;
		for (auto j = static_cast<uint8_t>(CrawlTable[k]); j > 0; j--, ck += 2) {
			Point pos = myPlayer.position.tile + Displacement { CrawlTable[ck - 1], CrawlTable[ck] };
			if (dPlayer[pos.x][pos.y] != 0 || dMonster[pos.x][pos.y] != 0)
				continue;
			if (!IsTileWalkable(pos))
				continue;

			if (AddMonster(pos, myPlayer._pdir, id, true) < 0)
				return fmt::format("I could only summon {} Monsters. The rest strike for shorter working hours.", spawnedMonster);
			spawnedMonster += 1;

			if (spawnedMonster >= count)
				return "Let the fighting begin!";
		}
	}

	return fmt::format("I could only summon {} Monsters. The rest strike for shorter working hours.", spawnedMonster);
}

std::string DebugCmdShowTileData(const string_view parameter)
{
	std::string paramList[] = {
		"dPiece",
		"dTransVal",
		"dLight",
		"dPreLight",
		"dFlags",
		"dPlayer",
		"dMonster",
		"dCorpse",
		"dObject",
		"dItem",
		"dSpecial",
		"coords",
		"cursorcoords",
		"objectindex",
		"nBlockTable",
		"nSolidTable",
		"nTransTable",
		"nMissileTable",
		"nTrapTable",
		"AutomapView",
		"dungeon",
		"pdungeon",
		"dflags",
	};

	if (parameter == "clear") {
		SelectedDebugGridTextItem = DebugGridTextItem::None;
		return "Tile data cleared!";
	}
	if (parameter == "") {
		std::string list = "clear";
		for (const auto &param : paramList) {
			list += " / " + param;
		}
		return list;
	}
	bool found = false;
	int index = 0;
	for (const auto &param : paramList) {
		index++;
		if (parameter != param)
			continue;
		found = true;
		auto newGridText = static_cast<DebugGridTextItem>(index);
		if (newGridText == SelectedDebugGridTextItem) {
			SelectedDebugGridTextItem = DebugGridTextItem::None;
			return "Tile data toggled... now you see nothing.";
		}
		SelectedDebugGridTextItem = newGridText;
		break;
	}
	if (!found)
		return "Invalid name. Check names using tiledata command.";

	return "Special powers activated.";
}

std::string DebugCmdScrollView(const string_view parameter)
{
	DebugScrollViewEnabled = !DebugScrollViewEnabled;
	if (DebugScrollViewEnabled)
		return "You can see as far as an eagle.";
	InitMultiView();
	return "If you want to see the world, you need to explore it yourself.";
}

std::string DebugCmdItemInfo(const string_view parameter)
{
	auto &myPlayer = Players[MyPlayerId];
	Item *pItem = nullptr;
	if (pcurs >= CURSOR_FIRSTITEM) {
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
		return fmt::format("Name: {}\nIDidx: {}\nSeed: {}\nCreateInfo: {}", pItem->_iIName, pItem->IDidx, pItem->_iSeed, pItem->_iCreateInfo);
	}
	return fmt::format("Numitems: {}", ActiveItemCount);
}

std::string DebugCmdQuestInfo(const string_view parameter)
{
	if (parameter.empty()) {
		std::string ret = "You must provide an id. This could be:";
		for (auto &quest : Quests) {
			if (IsNoneOf(quest._qactive, QUEST_NOTAVAIL, QUEST_INIT))
				continue;
			ret.append(fmt::format(" {} ({})", quest._qidx, QuestsData[quest._qidx]._qlstr));
		}
		return ret;
	}

	int questId = atoi(parameter.data());

	if (questId >= MAXQUESTS)
		return fmt::format("Quest {} is not known. Do you want to write a mod?", questId);
	auto &quest = Quests[questId];
	return fmt::format("\nQuest: {}\nActive: {} Var1: {} Var2: {}", QuestsData[quest._qidx]._qlstr, quest._qactive, quest._qvar1, quest._qvar2);
}

std::string DebugCmdPlayerInfo(const string_view parameter)
{
	int playerId = atoi(parameter.data());
	if (playerId < 0 || playerId >= MAX_PLRS)
		return "My friend, we need a valid playerId.";
	auto &player = Players[playerId];
	if (!player.plractive)
		return "Player is not active";

	const Point target = player.GetTargetPosition();
	return fmt::format("Plr {} is {}\nLvl: {} Changing: {}\nTile.x: {} Tile.y: {} Target.x: {} Target.y: {}\nMode: {} destAction: {} walkpath[0]: {}\nInvincible:{} HitPoints:{}",
	    playerId, player._pName,
	    player.plrlevel, player._pLvlChanging,
	    player.position.tile.x, player.position.tile.y, target.x, target.y,
	    player._pmode, player.destAction, player.walkpath[0],
	    player._pInvincible ? 1 : 0, player._pHitPoints);
}

std::string DebugCmdToggleFPS(const string_view parameter)
{
	frameflag = !frameflag;
	return "";
}

std::vector<DebugCmdItem> DebugCmdList = {
	{ "help", "Prints help overview or help for a specific command.", "({command})", &DebugCmdHelp },
	{ "give gold", "Fills the inventory with gold.", "", &DebugCmdGiveGoldCheat },
	{ "give xp", "Levels the player up (min 1 level or {levels}).", "({levels})", &DebugCmdLevelUp },
	{ "setspells", "Set spell level to {level} for all spells.", "{level}", &DebugCmdSetSpellsLevel },
	{ "take gold", "Removes all gold from inventory.", "", &DebugCmdTakeGoldCheat },
	{ "give quest", "Enable a given quest.", "({id})", &DebugCmdQuest },
	{ "give map", "Reveal the map.", "", &DebugCmdMapReveal },
	{ "take map", "Hide the map.", "", &DebugCmdMapHide },
	{ "changelevel", "Moves to specifided {level} (use 0 for town).", "{level}", &DebugCmdWarpToLevel },
	{ "map", "Load a quest level {level}.", "{level}", &DebugCmdLoadMap },
	{ "visit", "Visit a towner.", "{towner}", &DebugCmdVisitTowner },
	{ "restart", "Resets specified {level}.", "{level} ({seed})", &DebugCmdResetLevel },
	{ "god", "Toggles godmode.", "", &DebugCmdGodMode },
	{ "r_drawvision", "Toggles vision debug rendering.", "", &DebugCmdVision },
	{ "r_fullbright", "Toggles whether light shading is in effect.", "", &DebugCmdLighting },
	{ "fill", "Refills health and mana.", "", &DebugCmdRefillHealthMana },
	{ "dropu", "Attempts to generate unique item {name}.", "{name}", &DebugCmdGenerateUniqueItem },
	{ "drop", "Attempts to generate item {name}.", "{name}", &DebugCmdGenerateItem },
	{ "talkto", "Interacts with a NPC whose name contains {name}.", "{name}", &DebugCmdTalkToTowner },
	{ "exit", "Exits the game.", "", &DebugCmdExit },
	{ "arrow", "Changes arrow effect (normal, fire, lightning, explosion).", "{effect}", &DebugCmdArrow },
	{ "grid", "Toggles showing grid.", "", &DebugCmdShowGrid },
	{ "seedinfo", "Show seed infos for current level.", "", &DebugCmdLevelSeed },
	{ "spawn", "Spawns monster {name}.", "({count}) {name}", &DebugCmdSpawnMonster },
	{ "tiledata", "Toggles showing tile data {name} (leave name empty to see a list).", "{name}", &DebugCmdShowTileData },
	{ "scrollview", "Toggles scroll view feature (with shift+mouse).", "", &DebugCmdScrollView },
	{ "iteminfo", "Shows info of currently selected item.", "", &DebugCmdItemInfo },
	{ "questinfo", "Shows info of quests.", "{id}", &DebugCmdQuestInfo },
	{ "playerinfo", "Shows info of player.", "{playerid}", &DebugCmdPlayerInfo },
	{ "fps", "Toggles displaying FPS", "", &DebugCmdToggleFPS },
};

} // namespace

void LoadDebugGFX()
{
	pSquareCel = LoadCel("Data\\Square.CEL", 64);
}

void FreeDebugGFX()
{
	pSquareCel = std::nullopt;
}

void GetDebugMonster()
{
	int mi1 = pcursmonst;
	if (mi1 == -1) {
		int mi2 = dMonster[cursPosition.x][cursPosition.y];
		if (mi2 != 0) {
			mi1 = abs(mi2) - 1;
		} else {
			mi1 = DebugMonsterId;
		}
	}
	PrintDebugMonster(mi1);
}

void NextDebugMonster()
{
	char dstr[MAX_SEND_STR_LEN];

	DebugMonsterId++;
	if (DebugMonsterId == MAXMONSTERS)
		DebugMonsterId = 0;

	sprintf(dstr, "Current debug monster = %i", DebugMonsterId);
	NetSendCmdString(1 << MyPlayerId, dstr);
}

void SetDebugLevelSeedInfos(uint32_t mid1Seed, uint32_t mid2Seed, uint32_t mid3Seed, uint32_t endSeed)
{
	glMid1Seed[currlevel] = mid1Seed;
	glMid2Seed[currlevel] = mid2Seed;
	glMid3Seed[currlevel] = mid3Seed;
	glEndSeed[currlevel] = endSeed;
}

bool CheckDebugTextCommand(const string_view text)
{
	auto debugCmdIterator = std::find_if(DebugCmdList.begin(), DebugCmdList.end(), [&](const DebugCmdItem &elem) { return text.find(elem.text) == 0 && (text.length() == elem.text.length() || text[elem.text.length()] == ' '); });
	if (debugCmdIterator == DebugCmdList.end())
		return false;

	auto &dbgCmd = *debugCmdIterator;
	string_view parameter = "";
	if (text.length() > (dbgCmd.text.length() + 1))
		parameter = text.substr(dbgCmd.text.length() + 1);
	const auto result = dbgCmd.actionProc(parameter);
	Log("DebugCmd: {} Result: {}", text, result);
	if (result != "")
		InitDiabloMsg(result);
	return true;
}

bool IsDebugGridTextNeeded()
{
	return SelectedDebugGridTextItem != DebugGridTextItem::None;
}

bool IsDebugGridInMegatiles()
{
	switch (SelectedDebugGridTextItem) {
	case DebugGridTextItem::AutomapView:
	case DebugGridTextItem::dungeon:
	case DebugGridTextItem::pdungeon:
	case DebugGridTextItem::dflags:
		return true;
	default:
		return false;
	}
}

bool GetDebugGridText(Point dungeonCoords, char *debugGridTextBuffer)
{
	int info = 0;
	Point megaCoords = { (dungeonCoords.x - 16) / 2, (dungeonCoords.y - 16) / 2 };
	switch (SelectedDebugGridTextItem) {
	case DebugGridTextItem::coords:
		sprintf(debugGridTextBuffer, "%d:%d", dungeonCoords.x, dungeonCoords.y);
		return true;
	case DebugGridTextItem::cursorcoords:
		if (dungeonCoords != cursPosition)
			return false;
		sprintf(debugGridTextBuffer, "%d:%d", dungeonCoords.x, dungeonCoords.y);
		return true;
	case DebugGridTextItem::objectindex: {
		info = 0;
		int objectIndex = dObject[dungeonCoords.x][dungeonCoords.y];
		if (objectIndex != 0 && DebugIndexToObjectID.find(objectIndex) != DebugIndexToObjectID.end()) {
			info = DebugIndexToObjectID[abs(objectIndex) - 1];
		}
		break;
	}
	case DebugGridTextItem::dPiece:
		info = dPiece[dungeonCoords.x][dungeonCoords.y];
		break;
	case DebugGridTextItem::dTransVal:
		info = dTransVal[dungeonCoords.x][dungeonCoords.y];
		break;
	case DebugGridTextItem::dLight:
		info = dLight[dungeonCoords.x][dungeonCoords.y];
		break;
	case DebugGridTextItem::dPreLight:
		info = dPreLight[dungeonCoords.x][dungeonCoords.y];
		break;
	case DebugGridTextItem::dFlags:
		info = static_cast<int>(dFlags[dungeonCoords.x][dungeonCoords.y]);
		break;
	case DebugGridTextItem::dPlayer:
		info = dPlayer[dungeonCoords.x][dungeonCoords.y];
		break;
	case DebugGridTextItem::dMonster:
		info = dMonster[dungeonCoords.x][dungeonCoords.y];
		break;
	case DebugGridTextItem::dCorpse:
		info = dCorpse[dungeonCoords.x][dungeonCoords.y];
		break;
	case DebugGridTextItem::dItem:
		info = dItem[dungeonCoords.x][dungeonCoords.y];
		break;
	case DebugGridTextItem::dSpecial:
		info = dSpecial[dungeonCoords.x][dungeonCoords.y];
		break;
	case DebugGridTextItem::dObject:
		info = dObject[dungeonCoords.x][dungeonCoords.y];
		break;
	case DebugGridTextItem::nBlockTable:
		info = nBlockTable[dPiece[dungeonCoords.x][dungeonCoords.y]];
		break;
	case DebugGridTextItem::nSolidTable:
		info = nSolidTable[dPiece[dungeonCoords.x][dungeonCoords.y]];
		break;
	case DebugGridTextItem::nTransTable:
		info = nTransTable[dPiece[dungeonCoords.x][dungeonCoords.y]];
		break;
	case DebugGridTextItem::nMissileTable:
		info = nMissileTable[dPiece[dungeonCoords.x][dungeonCoords.y]];
		break;
	case DebugGridTextItem::nTrapTable:
		info = nTrapTable[dPiece[dungeonCoords.x][dungeonCoords.y]];
		break;
	case DebugGridTextItem::AutomapView:
		info = AutomapView[megaCoords.x][megaCoords.y];
		break;
	case DebugGridTextItem::dungeon:
		info = dungeon[megaCoords.x][megaCoords.y];
		break;
	case DebugGridTextItem::pdungeon:
		info = pdungeon[megaCoords.x][megaCoords.y];
		break;
	case DebugGridTextItem::dflags:
		info = dflags[megaCoords.x][megaCoords.y];
		break;
	case DebugGridTextItem::None:
		return false;
	}
	if (info == 0)
		return false;
	sprintf(debugGridTextBuffer, "%d", info);
	return true;
}

} // namespace devilution

#endif

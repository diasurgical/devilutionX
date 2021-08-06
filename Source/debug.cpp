/**
 * @file debug.cpp
 *
 * Implementation of debug functions.
 */

#include "cursor.h"
#include "engine/cel_sprite.hpp"
#include "engine/load_cel.hpp"
#include "engine/point.hpp"
#include "inv.h"
#include "spells.h"
#include "utils/language.h"

namespace devilution {

std::optional<CelSprite> pSquareCel;

#ifdef _DEBUG

namespace {

int DebugPlayerId;
int DebugQuestId;
int DebugMonsterId;

void SetSpellLevelCheat(spell_id spl, int spllvl)
{
	auto &myPlayer = Players[MyPlayerId];

	myPlayer._pMemSpells |= GetSpellBitmask(spl);
	myPlayer._pSplLvl[spl] = spllvl;
}

void PrintDebugMonster(int m)
{
	char dstr[128];

	auto &monster = Monsters[m];

	sprintf(dstr, "Monster %i = %s", m, _(monster.mName));
	NetSendCmdString(1 << MyPlayerId, dstr);
	sprintf(dstr, "X = %i, Y = %i", monster.position.tile.x, monster.position.tile.y);
	NetSendCmdString(1 << MyPlayerId, dstr);
	sprintf(dstr, "Enemy = %i, HP = %i", monster._menemy, monster._mhitpoints);
	NetSendCmdString(1 << MyPlayerId, dstr);
	sprintf(dstr, "Mode = %i, Var1 = %i", monster._mmode, monster._mVar1);
	NetSendCmdString(1 << MyPlayerId, dstr);

	bool bActive = false;

	for (int i = 0; i < ActiveMonsterCount; i++) {
		if (ActiveMonsters[i] == m)
			bActive = true;
	}

	sprintf(dstr, "Active List = %i, Squelch = %i", bActive ? 1 : 0, monster._msquelch);
	NetSendCmdString(1 << MyPlayerId, dstr);
}

} // namespace

void LoadDebugGFX()
{
	if (visiondebug)
		pSquareCel = LoadCel("Data\\Square.CEL", 64);
}

void FreeDebugGFX()
{
	pSquareCel = std::nullopt;
}

void GiveGoldCheat()
{
	auto &myPlayer = Players[MyPlayerId];

	for (int8_t &itemId : myPlayer.InvGrid) {
		if (itemId != 0)
			continue;

		int ni = myPlayer._pNumInv++;
		SetPlrHandItem(&myPlayer.InvList[ni], IDI_GOLD);
		GetPlrHandSeed(&myPlayer.InvList[ni]);
		myPlayer.InvList[ni]._ivalue = GOLD_MAX_LIMIT;
		myPlayer.InvList[ni]._iCurs = ICURS_GOLD_LARGE;
		myPlayer._pGold += GOLD_MAX_LIMIT;
		itemId = myPlayer._pNumInv;
	}
}

void TakeGoldCheat()
{
	auto &myPlayer = Players[MyPlayerId];

	for (auto itemId : myPlayer.InvGrid) {
		itemId -= 1;

		if (itemId < 0)
			continue;
		if (myPlayer.InvList[itemId]._itype != ITYPE_GOLD)
			continue;

		myPlayer.RemoveInvItem(itemId);
	}

	myPlayer._pGold = 0;
}

void MaxSpellsCheat()
{
	auto &myPlayer = Players[MyPlayerId];

	for (int i = SPL_FIREBOLT; i < MAX_SPELLS; i++) {
		if (GetSpellBookLevel((spell_id)i) != -1) {
			myPlayer._pMemSpells |= GetSpellBitmask(i);
			myPlayer._pSplLvl[i] = 10;
		}
	}
}

void SetAllSpellsCheat()
{
	SetSpellLevelCheat(SPL_FIREBOLT, 8);
	SetSpellLevelCheat(SPL_CBOLT, 11);
	SetSpellLevelCheat(SPL_HBOLT, 10);
	SetSpellLevelCheat(SPL_HEAL, 7);
	SetSpellLevelCheat(SPL_HEALOTHER, 5);
	SetSpellLevelCheat(SPL_LIGHTNING, 9);
	SetSpellLevelCheat(SPL_FIREWALL, 5);
	SetSpellLevelCheat(SPL_TELEKINESIS, 3);
	SetSpellLevelCheat(SPL_TOWN, 3);
	SetSpellLevelCheat(SPL_FLASH, 3);
	SetSpellLevelCheat(SPL_RNDTELEPORT, 2);
	SetSpellLevelCheat(SPL_MANASHIELD, 2);
	SetSpellLevelCheat(SPL_WAVE, 4);
	SetSpellLevelCheat(SPL_FIREBALL, 3);
	SetSpellLevelCheat(SPL_STONE, 1);
	SetSpellLevelCheat(SPL_CHAIN, 1);
	SetSpellLevelCheat(SPL_GUARDIAN, 4);
	SetSpellLevelCheat(SPL_ELEMENT, 3);
	SetSpellLevelCheat(SPL_NOVA, 1);
	SetSpellLevelCheat(SPL_GOLEM, 2);
	SetSpellLevelCheat(SPL_FLARE, 1);
	SetSpellLevelCheat(SPL_BONESPIRIT, 1);
}

void PrintDebugPlayer(bool bNextPlayer)
{
	char dstr[128];

	if (bNextPlayer)
		DebugPlayerId = ((BYTE)DebugPlayerId + 1) & 3;

	auto &player = Players[DebugPlayerId];

	sprintf(dstr, "Plr %i : Active = %i", DebugPlayerId, player.plractive ? 1 : 0);
	NetSendCmdString(1 << MyPlayerId, dstr);

	if (player.plractive) {
		sprintf(dstr, "  Plr %i is %s", DebugPlayerId, player._pName);
		NetSendCmdString(1 << MyPlayerId, dstr);
		sprintf(dstr, "  Lvl = %i : Change = %i", player.plrlevel, player._pLvlChanging ? 1 : 0);
		NetSendCmdString(1 << MyPlayerId, dstr);
		const Point target = player.GetTargetPosition();
		sprintf(dstr, "  x = %i, y = %i : tx = %i, ty = %i", player.position.tile.x, player.position.tile.y, target.x, target.y);
		NetSendCmdString(1 << MyPlayerId, dstr);
		sprintf(dstr, "  mode = %i : daction = %i : walk[0] = %i", player._pmode, player.destAction, player.walkpath[0]);
		NetSendCmdString(1 << MyPlayerId, dstr);
		sprintf(dstr, "  inv = %i : hp = %i", player._pInvincible ? 1 : 0, player._pHitPoints);
		NetSendCmdString(1 << MyPlayerId, dstr);
	}
}

void PrintDebugQuest()
{
	char dstr[128];

	sprintf(dstr, "Quest %i :  Active = %i, Var1 = %i", DebugQuestId, Quests[DebugQuestId]._qactive, Quests[DebugQuestId]._qvar1);
	NetSendCmdString(1 << MyPlayerId, dstr);

	DebugQuestId++;
	if (DebugQuestId == MAXQUESTS)
		DebugQuestId = 0;
}

void GetDebugMonster()
{
	int mi1 = pcursmonst;
	if (mi1 == -1) {
		int mi2 = dMonster[cursmx][cursmy];
		if (mi2 != 0) {
			mi1 = mi2 - 1;
			if (mi2 <= 0)
				mi1 = -(mi2 + 1);
		} else {
			mi1 = DebugMonsterId;
		}
	}
	PrintDebugMonster(mi1);
}

void NextDebugMonster()
{
	char dstr[128];

	DebugMonsterId++;
	if (DebugMonsterId == MAXMONSTERS)
		DebugMonsterId = 0;

	sprintf(dstr, "Current debug monster = %i", DebugMonsterId);
	NetSendCmdString(1 << MyPlayerId, dstr);
}

#endif

} // namespace devilution

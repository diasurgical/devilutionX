/**
 * @file track.cpp
 *
 * Implementation of functionality tracking what the mouse cursor is pointing at.
 */
#include "track.h"

#include <SDL.h>

#include "cursor.h"
#include "engine/point.hpp"
#include "player.h"

namespace devilution {

namespace {

bool sgbIsScrolling;
Uint32 sgdwLastWalk;
bool sgbIsWalking;

} // namespace

static bool AttackIntervalCheck(Uint32 lastTime)
{
	Uint32 currentTime = SDL_GetTicks();
	if (currentTime - lastTime > (Uint32) gnTickDelay * 4) //Check if it's been at least 200ms
		return true;
	else
		return false;
}

static bool RepeatLeftMouseAttackAction() //Fluffy
{
	if (!(lastLeftMouseButtonAction == MOUSEACTION_ATTACK || lastLeftMouseButtonAction == MOUSEACTION_ATTACK_MONSTERTARGET || lastLeftMouseButtonAction == MOUSEACTION_ATTACK_PLAYERTARGET) || pcurs != CURSOR_HAND || sgbMouseDown != CLICK_LEFT)
		return false;

	//Repeat action if it's been X duration since the attack or spell cast
	if (AttackIntervalCheck(lastLeftMouseButtonTime)) {
		if (lastLeftMouseButtonAction == MOUSEACTION_ATTACK) {
			if (cursmx >= 0 && cursmx < MAXDUNX && cursmy >= 0 && cursmy < MAXDUNY) { 
				if (Players[MyPlayerId]._pwtype == WT_RANGED)
					NetSendCmdLoc(MyPlayerId, true, CMD_RATTACKXY, {cursmx, cursmy});
				else
					NetSendCmdLoc(MyPlayerId, true, CMD_SATTACKXY, {cursmx, cursmy});
			}
		} else if (lastLeftMouseButtonAction == MOUSEACTION_ATTACK_MONSTERTARGET && pcursmonst != -1) {
			if (Players[MyPlayerId]._pwtype == WT_RANGED)
				NetSendCmdParam1(true, CMD_RATTACKID, pcursmonst);
			else
				NetSendCmdParam1(true, CMD_ATTACKID, pcursmonst);
		} else if (lastLeftMouseButtonAction == MOUSEACTION_ATTACK_PLAYERTARGET && pcursplr != -1 && !gbFriendlyMode) {
			if (Players[MyPlayerId]._pwtype == WT_RANGED)
				NetSendCmdParam1(true, CMD_RATTACKPID, pcursplr);
			else
				NetSendCmdParam1(true, CMD_ATTACKPID, pcursplr);
		}
	}
	return true;
}

static bool RepeatRightMouseAction() //Fluffy
{
	if (!(lastRightMouseButtonAction == MOUSEACTION_SPELL || lastRightMouseButtonAction == MOUSEACTION_ATTACK) || pcurs != CURSOR_HAND || sgbMouseDown != CLICK_RIGHT)
		return false;

	//Repeat action if it's been X duration since the attack or spell cast
	if (AttackIntervalCheck(lastRightMouseButtonTime))
		CheckPlrSpell(true);
	return true;
}

void track_process()
{
	if (RepeatLeftMouseAttackAction() || RepeatRightMouseAction()) //Fluffy
		return;

	if (!sgbIsWalking)
		return;

	if (cursmx < 0 || cursmx >= MAXDUNX - 1 || cursmy < 0 || cursmy >= MAXDUNY - 1)
		return;

	const auto &player = Players[MyPlayerId];

	if (player._pmode != PM_STAND && !(player.IsWalking() && player.AnimInfo.GetFrameToUseForRendering() > 6))
		return;

	const Point target = player.GetTargetPosition();
	if (cursmx != target.x || cursmy != target.y) {
		Uint32 tick = SDL_GetTicks();
		int tickMultiplier = 6;
		if (currlevel == 0 && sgGameInitInfo.bRunInTown != 0)
			tickMultiplier = 3;
		if ((int)(tick - sgdwLastWalk) >= gnTickDelay * tickMultiplier) {
			sgdwLastWalk = tick;
			NetSendCmdLoc(MyPlayerId, true, CMD_WALKXY, { cursmx, cursmy });
			if (!sgbIsScrolling)
				sgbIsScrolling = true;
		}
	}
}

void track_repeat_walk(bool rep)
{
	if (sgbIsWalking == rep)
		return;

	sgbIsWalking = rep;
	if (rep) {
		sgbIsScrolling = false;
		sgdwLastWalk = SDL_GetTicks() - gnTickDelay;
		NetSendCmdLoc(MyPlayerId, true, CMD_WALKXY, { cursmx, cursmy });
	} else if (sgbIsScrolling) {
		sgbIsScrolling = false;
	}
}

bool track_isscrolling()
{
	return sgbIsScrolling;
}

} // namespace devilution

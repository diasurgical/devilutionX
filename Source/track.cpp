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
uint32_t sgdwLastWalk;
bool sgbIsWalking;

bool RepeatMouseAction()
{
	if (pcurs != CURSOR_HAND)
		return false;

	if (sgbMouseDown == CLICK_NONE)
		return false;

	auto &myPlayer = Players[MyPlayerId];
	if (myPlayer._pmode == PM_DEATH
	    || myPlayer._pmode == PM_QUIT
	    || myPlayer.destAction != ACTION_NONE
	    || SDL_GetTicks() - LastMouseButtonTime < gnTickDelay * 4) {
		return true;
	}

	bool rangedAttack = myPlayer.UsesRangedWeapon();
	LastMouseButtonTime = SDL_GetTicks();
	switch (LastMouseButtonAction) {
	case MouseActionType::Attack:
		if (cursmx >= 0 && cursmx < MAXDUNX && cursmy >= 0 && cursmy < MAXDUNY)
			NetSendCmdLoc(MyPlayerId, true, rangedAttack ? CMD_RATTACKXY : CMD_SATTACKXY, { cursmx, cursmy });
		break;
	case MouseActionType::AttackMonsterTarget:
		if (pcursmonst != -1)
			NetSendCmdParam1(true, rangedAttack ? CMD_RATTACKID : CMD_ATTACKID, pcursmonst);
		break;
	case MouseActionType::AttackPlayerTarget:
		if (pcursplr != -1 && !gbFriendlyMode)
			NetSendCmdParam1(true, rangedAttack ? CMD_RATTACKPID : CMD_ATTACKPID, pcursplr);
		break;
	case MouseActionType::Spell:
	case MouseActionType::SpellOutOfMana:
		CheckPlrSpell(true);
		break;
	case MouseActionType::Other:
	case MouseActionType::None:
		return false;
	}

	return true;
}

} // namespace

void track_process()
{
	if (RepeatMouseAction())
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
		uint32_t tick = SDL_GetTicks();
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

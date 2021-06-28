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

void track_process()
{
	if (!sgbIsWalking)
		return;

	if (cursPosition.x < 0 || cursPosition.x >= MAXDUNX - 1 || cursPosition.x < 0 || cursPosition.x >= MAXDUNY - 1)
		return;

	const auto &player = plr[myplr];

	if (player._pmode != PM_STAND && !(player.IsWalking() && player.AnimInfo.GetFrameToUseForRendering() > 6))
		return;

	const Point target = player.GetTargetPosition();
	if (cursPosition != target) {
		Uint32 tick = SDL_GetTicks();
		int TickMultiplier = 6;
		if (currlevel == 0 && sgGameInitInfo.bRunInTown != 0)
			TickMultiplier = 3;
		if ((int)(tick - sgdwLastWalk) >= gnTickDelay * TickMultiplier) {
			sgdwLastWalk = tick;
			NetSendCmdLoc(myplr, true, CMD_WALKXY, cursPosition);
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
		NetSendCmdLoc(myplr, true, CMD_WALKXY, cursPosition);
	} else if (sgbIsScrolling) {
		sgbIsScrolling = false;
	}
}

bool track_isscrolling()
{
	return sgbIsScrolling;
}

} // namespace devilution

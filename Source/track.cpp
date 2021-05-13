/**
 * @file track.cpp
 *
 * Implementation of functionality tracking what the mouse cursor is pointing at.
 */
#include <SDL.h>

#include "cursor.h"
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

	if (cursmx < 0 || cursmx >= MAXDUNX - 1 || cursmy < 0 || cursmy >= MAXDUNY - 1)
		return;

	if (plr[myplr].AnimInfo.GetFrameToUseForRendering() <= 6 || (plr[myplr]._pmode != PM_WALK && plr[myplr]._pmode != PM_WALK2 && plr[myplr]._pmode != PM_WALK3 && plr[myplr]._pmode != PM_STAND))
		return;

	const Point target = plr[myplr].GetTargetPosition();
	if (cursmx != target.x || cursmy != target.y) {
		Uint32 tick = SDL_GetTicks();
		if ((int)(tick - sgdwLastWalk) >= gnTickDelay * 6) {
			sgdwLastWalk = tick;
			NetSendCmdLoc(myplr, true, CMD_WALKXY, { cursmx, cursmy });
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
		NetSendCmdLoc(myplr, true, CMD_WALKXY, { cursmx, cursmy });
	} else if (sgbIsScrolling) {
		sgbIsScrolling = false;
	}
}

bool track_isscrolling()
{
	return sgbIsScrolling;
}

} // namespace devilution

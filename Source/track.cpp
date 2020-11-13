/**
 * @file track.cpp
 *
 * Implementation of functionality tracking what the mouse cursor is pointing at.
 */
#include "all.h"

DEVILUTION_BEGIN_NAMESPACE

static BYTE sgbIsScrolling;
static int sgdwLastWalk;
static BOOL sgbIsWalking;

void track_process()
{
	return;
	if (!sgbIsWalking)
	//if (!sgbIsWalking && (sgbMouseDown == 1 && (SDL_GetModState() & KMOD_SHIFT) == 0))
		return;

	if (cursmx < 0 || cursmx >= MAXDUNX - 1 || cursmy < 0 || cursmy >= MAXDUNY - 1)
		return;

	//this check was good for walking but has to be disabled after making this function handle casts / attacks with shift
	if (plr[myplr]._pVar8 <= 6 && plr[myplr]._pmode != PM_STAND)
		return;

	if (cursmx != plr[myplr]._ptargx || cursmy != plr[myplr]._ptargy) {
		if (logicTick - sgdwLastWalk >= 6) {
			sgdwLastWalk = logicTick;
			//RepeatClicks();
			NetSendCmdLoc(TRUE, CMD_WALKXY, cursmx, cursmy);
			if (!sgbIsScrolling)
				sgbIsScrolling = TRUE;
		}
	}
}

void track_repeat_walk(BOOL rep)
{
	if (sgbIsWalking == rep)
		return;

	sgbIsWalking = rep;
	if (rep) {
		sgbIsScrolling = FALSE;
		sgdwLastWalk = logicTick;
		NetSendCmdLoc(TRUE, CMD_WALKXY, cursmx, cursmy);
		//RepeatClicks();
	} else if (sgbIsScrolling) {
		sgbIsScrolling = FALSE;
	}
}

BOOL track_isscrolling()
{
	return sgbIsScrolling;
}

DEVILUTION_END_NAMESPACE

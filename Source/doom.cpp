/**
 * @file doom.cpp
 *
 * Implementation of the map of the stars quest.
 */
#include "doom.h"

#include "control.h"

namespace devilution {

int doom_quest_time;
int doom_stars_drawn;
BYTE *pDoomCel;
bool doomflag;
int DoomQuestState;

/*
void doom_reset_state()
{
    if (DoomQuestState <= 0) {
        DoomQuestState = 0;
    }
}

void doom_play_movie()
{
    if (DoomQuestState < 36001) {
        DoomQuestState++;
        if (DoomQuestState == 36001) {
            PlayInGameMovie("gendata\\doom.smk");
            DoomQuestState++;
        }
    }
}
*/

int doom_get_frame_from_time()
{
	if (DoomQuestState == 36001) {
		return 31;
	}

	return DoomQuestState / 1200;
}

void doom_cleanup()
{
	if (pDoomCel != nullptr) {
		MemFreeDbg(pDoomCel);
		pDoomCel = nullptr;
	}
}

static bool doom_alloc_cel()
{
	doom_cleanup();
	pDoomCel = DiabloAllocPtr(0x39000);
	return pDoomCel != nullptr;
}

static bool doom_load_graphics()
{
	bool ret;

	ret = false;
	strcpy(tempstr, "Items\\Map\\MapZtown.CEL");
	if (LoadFileWithMem(tempstr, pDoomCel))
		ret = true;
	return ret;
}

void doom_init()
{
	if (doom_alloc_cel()) {
		doom_quest_time = doom_get_frame_from_time() == 31 ? 31 : 0;
		if (doom_load_graphics()) {
			doomflag = true;
		} else {
			doom_close();
		}
	}
}

void doom_close()
{
	doomflag = false;
	doom_cleanup();
}

void doom_draw(CelOutputBuffer out)
{
	if (!doomflag) {
		return;
	}

	CelDrawTo(out, PANEL_X, PANEL_Y - 1, pDoomCel, 1, 640);
}

} // namespace devilution

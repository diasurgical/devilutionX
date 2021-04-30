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
std::unique_ptr<uint8_t[]> doomCel;
bool doomflag;
int DoomQuestState;

/*
void doom_reset_state()
{
	DoomQuestState = std::max(DoomQuestState, 0);
}

void doom_play_movie()
{
	if (DoomQuestState >= 36001)
		return;
        DoomQuestState++;
	if (DoomQuestState == 36001) {
		PlayInGameMovie("gendata\\doom.smk");
		DoomQuestState++;
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

static bool doom_load_graphics()
{
	bool ret;

	ret = false;
	strcpy(tempstr, "Items\\Map\\MapZtown.CEL");
	if (LoadFileWithMem(tempstr, doomCel.get()) != 0)
		ret = true;
	return ret;
}

void doom_init()
{
	doomCel = std::make_unique<uint8_t[]>(0x39000);
	doom_quest_time = doom_get_frame_from_time() == 31 ? 31 : 0;
	if (doom_load_graphics()) {
		doomflag = true;
	} else {
		doom_close();
	}
}

void doom_close()
{
	doomflag = false;
	doomCel.reset();
}

void doom_draw(const CelOutputBuffer &out)
{
	if (!doomflag) {
		return;
	}

	CelDrawTo(out, PANEL_X, PANEL_Y - 1, doomCel.get(), 1, 640);
}

} // namespace devilution

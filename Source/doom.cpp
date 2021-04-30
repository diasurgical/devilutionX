/**
 * @file doom.cpp
 *
 * Implementation of the map of the stars quest.
 */
#include "doom.h"

#include "control.h"

namespace devilution {

int DoomQuestTime;
std::unique_ptr<uint8_t[]> DoomCel;
bool DoomFlag;
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
	if (LoadFileWithMem(tempstr, DoomCel.get()) != 0)
		ret = true;
	return ret;
}

void doom_init()
{
	DoomCel = std::make_unique<uint8_t[]>(0x39000);
	DoomQuestTime = doom_get_frame_from_time() == 31 ? 31 : 0;
	if (doom_load_graphics()) {
		DoomFlag = true;
	} else {
		doom_close();
	}
}

void doom_close()
{
	DoomFlag = false;
	DoomCel = nullptr;
}

void doom_draw(const CelOutputBuffer &out)
{
	if (!DoomFlag) {
		return;
	}

	CelDrawTo(out, PANEL_X, PANEL_Y - 1, DoomCel.get(), 1, 640);
}

} // namespace devilution

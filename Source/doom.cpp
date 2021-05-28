/**
 * @file doom.cpp
 *
 * Implementation of the map of the stars quest.
 */
#include "doom.h"

#include "control.h"
#include "engine.h"
#include "engine/render/cel_render.hpp"
#include "utils/stdcompat/optional.hpp"

namespace devilution {
namespace {
std::optional<CelSprite> DoomCel;
} // namespace

int DoomQuestTime;
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
	DoomCel = LoadCel("Items\\Map\\MapZtown.CEL", 640);
	return true;
}

void doom_init()
{
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
	DoomCel = std::nullopt;
}

void doom_draw(const CelOutputBuffer &out)
{
	if (!DoomFlag) {
		return;
	}

	CelDrawTo(out, { PANEL_X, PANEL_Y - 1 }, *DoomCel, 1);
}

} // namespace devilution

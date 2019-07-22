#include "diablo.h"

DEVILUTION_BEGIN_NAMESPACE

int doom_quest_time;
int doom_stars_drawn;
BYTE *pDoomCel;
BOOL doomflag;
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

void doom_alloc_cel()
{
	pDoomCel = DiabloAllocPtr(229376);
}

void doom_cleanup()
{
	MemFreeDbg(pDoomCel);
}

void doom_load_graphics()
{
	if (doom_quest_time == 31) {
		strcpy(tempstr, "Items\\Map\\MapZDoom.CEL");
	} else if (doom_quest_time < 10) {
		sprintf(tempstr, "Items\\Map\\MapZ000%i.CEL", doom_quest_time);
	} else {
		sprintf(tempstr, "Items\\Map\\MapZ00%i.CEL", doom_quest_time);
	}
	LoadFileWithMem(tempstr, pDoomCel);
}

void doom_init()
{
	doomflag = TRUE;
	doom_alloc_cel();
	doom_quest_time = doom_get_frame_from_time() == 31 ? 31 : 0;
	doom_load_graphics();
}

void doom_close()
{
	if (doomflag) {
		doomflag = FALSE;
		doom_cleanup();
	}
}

void doom_draw()
{
	if (!doomflag) {
		return;
	}

	if (doom_quest_time != 31) {
		doom_stars_drawn++;
		if (doom_stars_drawn >= 5) {
			doom_stars_drawn = 0;
			doom_quest_time++;
			if (doom_quest_time > doom_get_frame_from_time()) {
				doom_quest_time = 0;
			}
			doom_load_graphics();
		}
	}

	CelDecodeOnly(SCREEN_X, PANEL_Y - 1, pDoomCel, 1, SCREEN_WIDTH);
}

DEVILUTION_END_NAMESPACE

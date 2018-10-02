//HEADER_GOES_HERE

#include "../types.h"

int doom_quest_time; // weak
int doom_stars_drawn; // weak
#ifndef NO_GLOBALS
void *pDoomCel;
int doomflag; // weak
#endif
int DoomQuestState; // idb

/*
void __cdecl doom_reset_state()
{
	if ( DoomQuestState <= 0 ) {
		DoomQuestState = 0;
	}
}

void __cdecl doom_play_movie()
{
	if ( DoomQuestState < 36001 ) {
		DoomQuestState++;
		if ( DoomQuestState == 36001 ) {
			PlayInGameMovie("gendata\\doom.smk");
			DoomQuestState++;
		}
	}
}
*/

int __cdecl doom_get_frame_from_time()
{
	if ( DoomQuestState == 36001 ) {
		return 31;
	}

	return DoomQuestState / 1200;
}

void __cdecl doom_alloc_cel()
{
	pDoomCel = DiabloAllocPtr(229376);
}

void __cdecl doom_cleanup()
{
	void *ptr = pDoomCel;
	pDoomCel = NULL;
	mem_free_dbg(ptr);
}

void __cdecl doom_load_graphics()
{
	if ( doom_quest_time == 31 )
	{
		strcpy(tempstr, "Items\\Map\\MapZDoom.CEL");
	}
	else if ( doom_quest_time < 10 )
	{
		sprintf(tempstr, "Items\\Map\\MapZ000%i.CEL", doom_quest_time);
	}
	else
	{
		sprintf(tempstr, "Items\\Map\\MapZ00%i.CEL", doom_quest_time);
	}
	LoadFileWithMem(tempstr, pDoomCel);
}
// 525750: using guessed type int doom_quest_time;

void __cdecl doom_init()
{
	doomflag = 1;
	doom_alloc_cel();
	doom_quest_time = doom_get_frame_from_time() == 31 ? 31 : 0;
	doom_load_graphics();
}
// 525750: using guessed type int doom_quest_time;
// 52575C: using guessed type int doomflag;

void __cdecl doom_close()
{
	if ( doomflag ) {
		doomflag = 0;
		doom_cleanup();
	}
}
// 52575C: using guessed type int doomflag;

void __cdecl doom_draw()
{
	if ( !doomflag ) {
		return;
	}

	if ( doom_quest_time != 31 ) {
		doom_stars_drawn++;
		if ( doom_stars_drawn >= 5 ) {
			doom_stars_drawn = 0;
			doom_quest_time++;
			if ( doom_quest_time > doom_get_frame_from_time() ) {
				doom_quest_time = 0;
			}
			doom_load_graphics();
		}
	}

	CelDecodeOnly(64, 511, pDoomCel, 1, 640);
}
// 525750: using guessed type int doom_quest_time;
// 525754: using guessed type int doom_stars_drawn;
// 52575C: using guessed type int doomflag;

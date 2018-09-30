//HEADER_GOES_HERE
#ifndef __DOOM_H__
#define __DOOM_H__

extern int doom_quest_time; // weak
extern int doom_stars_drawn; // weak
extern void *pDoomCel;
extern int doomflag; // weak
extern int DoomQuestState; // idb

/*
void __cdecl doom_reset_state();
void __cdecl doom_play_movie();
*/
int __cdecl doom_get_frame_from_time();
void __cdecl doom_alloc_cel();
void __cdecl doom_cleanup();
void __cdecl doom_load_graphics();
void __cdecl doom_init();
void __cdecl doom_close();
void __cdecl doom_draw();

#endif /* __DOOM_H__ */

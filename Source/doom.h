/**
 * @file doom.h
 *
 * Interface of the map of the stars quest.
 */
#ifndef __DOOM_H__
#define __DOOM_H__

DEVILUTION_BEGIN_NAMESPACE

#ifdef __cplusplus
extern "C" {
#endif

extern int doom_quest_time;
extern int doom_stars_drawn;
extern BYTE *pDoomCel;
extern BOOL doomflag;
extern int DoomQuestState;

/*
void doom_reset_state();
void doom_play_movie();
*/
int doom_get_frame_from_time();
void doom_alloc_cel();
void doom_cleanup();
void doom_load_graphics();
void doom_init();
void doom_close();
void doom_draw();

#ifdef __cplusplus
}
#endif

DEVILUTION_END_NAMESPACE

#endif /* __DOOM_H__ */

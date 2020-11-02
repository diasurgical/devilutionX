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

extern DIABOOL doomflag;
extern int DoomQuestState;

void doom_init();
void doom_close();
void doom_draw();

#ifdef __cplusplus
}
#endif

DEVILUTION_END_NAMESPACE

#endif /* __DOOM_H__ */

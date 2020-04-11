/**
 * @file dthread.h
 *
 * Interface of functions for updating game state from network commands.
 */
#ifndef __DTHREAD_H__
#define __DTHREAD_H__

extern SDL_threadID glpDThreadId;
extern BOOLEAN dthread_running;

void dthread_remove_player(int pnum);
void dthread_send_delta(int pnum, char cmd, void *pbSrc, int dwLen);
void dthread_start();
unsigned int dthread_handler(void *data);
void dthread_cleanup();

/* data */

#endif /* __DTHREAD_H__ */

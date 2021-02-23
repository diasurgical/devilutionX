/**
 * @file nthread.h
 *
 * Interface of functions for managing game ticks.
 */
#ifndef __NTHREAD_H__
#define __NTHREAD_H__

DEVILUTION_BEGIN_NAMESPACE

#ifdef __cplusplus
extern "C" {
#endif

extern BYTE sgbNetUpdateRate;
extern DWORD gdwMsgLenTbl[MAX_PLRS];
extern DWORD gdwDeltaBytesSec;
extern DWORD gdwTurnsInTransit;
extern uintptr_t glpMsgTbl[MAX_PLRS];
extern DWORD gdwLargestMsgSize;
extern DWORD gdwNormalMsgSize;

void nthread_terminate_game(const char *pszFcn);
DWORD nthread_send_and_recv_turn(DWORD cur_turn, int turn_delta);
BOOL nthread_recv_turns(BOOL *pfSendAsync);
void nthread_set_turn_upper_bit();
void nthread_start(BOOL set_turn_upper_bit);
void nthread_cleanup();
void nthread_ignore_mutex(BOOL bStart);
BOOL nthread_has_500ms_passed();

#ifdef __cplusplus
}
#endif

DEVILUTION_END_NAMESPACE

#endif /* __NTHREAD_H__ */

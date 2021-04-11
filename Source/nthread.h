/**
 * @file nthread.h
 *
 * Interface of functions for managing game ticks.
 */
#pragma once

namespace devilution {

extern BYTE sgbNetUpdateRate;
extern DWORD gdwMsgLenTbl[MAX_PLRS];
extern DWORD gdwDeltaBytesSec;
extern DWORD gdwTurnsInTransit;
extern uintptr_t glpMsgTbl[MAX_PLRS];
extern DWORD gdwLargestMsgSize;
extern DWORD gdwNormalMsgSize;

void nthread_terminate_game(const char *pszFcn);
DWORD nthread_send_and_recv_turn(DWORD cur_turn, int turn_delta);
bool nthread_recv_turns(bool *pfSendAsync);
void nthread_set_turn_upper_bit();
void nthread_start(bool set_turn_upper_bit);
void nthread_cleanup();
void nthread_ignore_mutex(bool bStart);
bool nthread_has_500ms_passed();

}

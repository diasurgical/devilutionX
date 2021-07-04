/**
 * @file nthread.h
 *
 * Interface of functions for managing game ticks.
 */
#pragma once

#include "miniwin/miniwin.h"
#include "player.h"

namespace devilution {

extern BYTE sgbNetUpdateRate;
extern DWORD gdwMsgLenTbl[MAX_PLRS];
extern DWORD gdwDeltaBytesSec;
extern DWORD gdwTurnsInTransit;
extern uintptr_t glpMsgTbl[MAX_PLRS];
extern DWORD gdwLargestMsgSize;
extern DWORD gdwNormalMsgSize;
extern float gfProgressToNextGameTick; // the progress as a fraction (0.0f to 1.0f) in time to the next game tick

void nthread_terminate_game(const char *pszFcn);
DWORD nthread_send_and_recv_turn(DWORD cur_turn, int turn_delta);
bool nthread_recv_turns(bool *pfSendAsync = nullptr);
void nthread_set_turn_upper_bit();
void nthread_start(bool set_turn_upper_bit);
void nthread_cleanup();
void nthread_ignore_mutex(bool bStart);
bool nthread_has_500ms_passed();
/**
 * @brief Calculates the progress in time to the next game tick
 * @return Progress as a fraction (0.0f to 1.0f)
 */
void nthread_UpdateProgressToNextGameTick();

} // namespace devilution

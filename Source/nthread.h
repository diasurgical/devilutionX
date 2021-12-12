/**
 * @file nthread.h
 *
 * Interface of functions for managing game ticks.
 */
#pragma once

#include "player.h"
#include "utils/attributes.h"

namespace devilution {

extern BYTE sgbNetUpdateRate;
extern size_t gdwMsgLenTbl[MAX_PLRS];
extern uint32_t gdwTurnsInTransit;
extern uintptr_t glpMsgTbl[MAX_PLRS];
extern uint32_t gdwLargestMsgSize;
extern uint32_t gdwNormalMsgSize;
extern DVL_API_FOR_TEST float gfProgressToNextGameTick; // the progress as a fraction (0.0f to 1.0f) in time to the next game tick
extern int last_tick;

void nthread_terminate_game(const char *pszFcn);
uint32_t nthread_send_and_recv_turn(uint32_t curTurn, int turnDelta);
bool nthread_recv_turns(bool *pfSendAsync = nullptr);
void nthread_set_turn_upper_bit();
void nthread_start(bool setTurnUpperBit);
void nthread_cleanup();
void nthread_ignore_mutex(bool bStart);

/**
 * @brief Checks if it's time for the logic to advance
 * @return True if the engine should tick
 */
bool nthread_has_500ms_passed();
/**
 * @brief Calculates the progress in time to the next game tick
 * @return Progress as a fraction (0.0f to 1.0f)
 */
void nthread_UpdateProgressToNextGameTick();

} // namespace devilution

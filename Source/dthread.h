/**
 * @file dthread.h
 *
 * Interface of functions for updating game state from network commands.
 */
#pragma once

namespace devilution {

void dthread_remove_player(uint8_t pnum);
void dthread_send_delta(int pnum, _cmd_id cmd, byte *pbSrc, int dwLen);
void dthread_start();
void dthread_cleanup();

} // namespace devilution

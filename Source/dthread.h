/**
 * @file dthread.h
 *
 * Interface of functions for updating game state from network commands.
 */
#pragma once

#include <memory>

#include "utils/stdcompat/cstddef.hpp"

namespace devilution {

void dthread_remove_player(uint8_t pnum);
void dthread_send_delta(int pnum, _cmd_id cmd, std::unique_ptr<byte[]> data, uint32_t len);
void dthread_start();
void DThreadCleanup();

} // namespace devilution

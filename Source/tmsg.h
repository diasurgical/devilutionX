/**
 * @file tmsg.h
 *
 * Interface of functionality transmitting chat messages.
 */
#pragma once

#include <cstdint>
#include <memory>

#include "utils/stdcompat/cstddef.hpp"

namespace devilution {

uint8_t tmsg_get(std::unique_ptr<byte[]> *msg);
void tmsg_add(const byte *msg, uint8_t bLen);
void tmsg_start();
void tmsg_cleanup();

} // namespace devilution

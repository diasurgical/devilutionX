/**
 * @file tmsg.h
 *
 * Interface of functionality transmitting chat messages.
 */
#pragma once

#include <cstddef>
#include <cstdint>
#include <memory>

namespace devilution {

uint8_t tmsg_get(std::unique_ptr<std::byte[]> *msg);
void tmsg_add(const std::byte *msg, uint8_t bLen);
void tmsg_start();
void tmsg_cleanup();

} // namespace devilution

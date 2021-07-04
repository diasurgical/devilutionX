/**
 * @file tmsg.h
 *
 * Interface of functionality transmitting chat messages.
 */
#pragma once

#include <cstdint>

#include "utils/stdcompat/cstddef.hpp"

namespace devilution {

#pragma pack(push, 1)
struct TMsgHdr {
	struct TMsg *pNext;
	int32_t dwTime;
	uint8_t bLen;
};

struct TMsg {
	TMsgHdr hdr;
	// this is actually alignment padding, but the message body is appended to the struct
	// so it's convenient to use byte-alignment and name it "body"
	byte body[3];
};
#pragma pack(pop)

size_t tmsg_get(byte *pbMsg);
void tmsg_add(byte *pbMsg, uint8_t bLen);
void tmsg_start();
void tmsg_cleanup();

} // namespace devilution

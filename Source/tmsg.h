/**
 * @file tmsg.h
 *
 * Interface of functionality transmitting chat messages.
 */
#pragma once

#include <cstdint>

#include "miniwin/miniwin.h"

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
	uint8_t body[3];
};
#pragma pack(pop)

int tmsg_get(BYTE *pbMsg);
void tmsg_add(BYTE *pbMsg, uint8_t bLen);
void tmsg_start();
void tmsg_cleanup();

} // namespace devilution

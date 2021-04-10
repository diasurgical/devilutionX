/**
 * @file tmsg.h
 *
 * Interface of functionality transmitting chat messages.
 */
#pragma once

namespace devilution {

#pragma pack(push, 1)
struct TMsgHdr {
	struct TMsg *pNext;
	Sint32 dwTime;
	Uint8 bLen;
};

struct TMsg {
	TMsgHdr hdr;
	// this is actually alignment padding, but the message body is appended to the struct
	// so it's convenient to use byte-alignment and name it "body"
	Uint8 body[3];
};
#pragma pack(pop)

int tmsg_get(Uint8 *pbMsg, Uint32 dwMaxLen);
void tmsg_add(Uint8 *pbMsg, Uint8 bLen);
void tmsg_start();
void tmsg_cleanup();

}

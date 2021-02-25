/**
 * @file tmsg.h
 *
 * Interface of functionality transmitting chat messages.
 */
#ifndef __TMSG_H__
#define __TMSG_H__

DEVILUTION_BEGIN_NAMESPACE

#ifdef __cplusplus
extern "C" {
#endif

#pragma pack(push, 1)
typedef struct TMsg TMsg;

typedef struct TMsgHdr {
	TMsg *pNext;
	Sint32 dwTime;
	Uint8 bLen;
} TMsgHdr;

typedef struct TMsg {
	TMsgHdr hdr;
	// this is actually alignment padding, but the message body is appended to the struct
	// so it's convenient to use byte-alignment and name it "body"
	Uint8 body[3];
} TMsg;
#pragma pack(pop)

int tmsg_get(Uint8 *pbMsg, Uint32 dwMaxLen);
void tmsg_add(Uint8 *pbMsg, Uint8 bLen);
void tmsg_start();
void tmsg_cleanup();

#ifdef __cplusplus
}
#endif

DEVILUTION_END_NAMESPACE

#endif /* __TMSG_H__ */

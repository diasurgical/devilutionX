/**
 * @file tmsg.cpp
 *
 * Implementation of functionality transmitting chat messages.
 */
#include "all.h"

DEVILUTION_BEGIN_NAMESPACE

namespace {

TMsg *sgpTimedMsgHead;

}

int tmsg_get(Uint8 *pbMsg, Uint32 dwMaxLen)
{
	int len;
	TMsg *head;

	if (!sgpTimedMsgHead)
		return 0;

	if ((int)(sgpTimedMsgHead->hdr.dwTime - SDL_GetTicks()) >= 0)
		return 0;
	head = sgpTimedMsgHead;
	sgpTimedMsgHead = head->hdr.pNext;
	len = head->hdr.bLen;
	// BUGFIX: ignores dwMaxLen
	memcpy(pbMsg, head->body, len);
	mem_free_dbg(head);
	return len;
}

void tmsg_add(Uint8 *pbMsg, Uint8 bLen)
{
	TMsg **tail;

	TMsg *msg = (TMsg *)DiabloAllocPtr(bLen + sizeof(*msg));
	msg->hdr.pNext = nullptr;
	msg->hdr.dwTime = SDL_GetTicks() + gnTickDelay * 10;
	msg->hdr.bLen = bLen;
	memcpy(msg->body, pbMsg, bLen);
	for (tail = &sgpTimedMsgHead; *tail; tail = &(*tail)->hdr.pNext) {
		;
	}
	*tail = msg;
}

void tmsg_start()
{
	assert(!sgpTimedMsgHead);
}

void tmsg_cleanup()
{
	TMsg *next;

	while (sgpTimedMsgHead) {
		next = sgpTimedMsgHead->hdr.pNext;
		MemFreeDbg(sgpTimedMsgHead);
		sgpTimedMsgHead = next;
	}
}

DEVILUTION_END_NAMESPACE

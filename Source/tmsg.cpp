/**
 * @file tmsg.cpp
 *
 * Implementation of functionality transmitting chat messages.
 */
#include "tmsg.h"

#include "diablo.h"

namespace devilution {

namespace {

TMsg *sgpTimedMsgHead;

} // namespace

size_t tmsg_get(byte *pbMsg)
{
	TMsg *head;

	if (sgpTimedMsgHead == nullptr)
		return 0;

	if ((int)(sgpTimedMsgHead->hdr.dwTime - SDL_GetTicks()) >= 0)
		return 0;
	head = sgpTimedMsgHead;
	sgpTimedMsgHead = head->hdr.pNext;
	size_t len = head->hdr.bLen;
	// BUGFIX: ignores dwMaxLen
	memcpy(pbMsg, head->body, len);
	std::free(head);
	return len;
}

void tmsg_add(byte *pbMsg, uint8_t bLen)
{
	TMsg **tail;

	TMsg *msg = static_cast<TMsg *>(std::malloc(bLen + sizeof(*msg)));
	msg->hdr.pNext = nullptr;
	msg->hdr.dwTime = SDL_GetTicks() + gnTickDelay * 10;
	msg->hdr.bLen = bLen;
	memcpy(msg->body, pbMsg, bLen);
	for (tail = &sgpTimedMsgHead; *tail != nullptr; tail = &(*tail)->hdr.pNext) {
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

	while (sgpTimedMsgHead != nullptr) {
		next = sgpTimedMsgHead->hdr.pNext;
		std::free(sgpTimedMsgHead);
		sgpTimedMsgHead = next;
	}
}

} // namespace devilution

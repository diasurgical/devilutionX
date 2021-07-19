/**
 * @file tmsg.cpp
 *
 * Implementation of functionality transmitting chat messages.
 */
#include <list>

#include "tmsg.h"
#include "diablo.h"

namespace devilution {

namespace {

struct TMsg {
	uint32_t time;
	std::unique_ptr<byte[]> body;
	uint8_t len;

	TMsg(uint32_t time, byte *data, uint8_t len)
	    : time(time)
	    , body(new byte[len])
	    , len(len)
	{
		memcpy(body.get(), data, len);
	}
};

std::list<TMsg> TimedMsgList;

} // namespace

uint8_t tmsg_get(std::unique_ptr<byte[]> *msg)
{
	if (TimedMsgList.empty())
		return 0;

	TMsg &head = TimedMsgList.front();
	if ((int)(head.time - SDL_GetTicks()) >= 0)
		return 0;

	uint8_t len = head.len;
	*msg = std::move(head.body);
	TimedMsgList.pop_front();
	return len;
}

void tmsg_add(byte *msg, uint8_t len)
{
	uint32_t time = SDL_GetTicks() + gnTickDelay * 10;
	TimedMsgList.emplace_back(time, msg, len);
}

void tmsg_start()
{
	assert(TimedMsgList.empty());
}

void tmsg_cleanup()
{
	TimedMsgList.clear();
}

} // namespace devilution

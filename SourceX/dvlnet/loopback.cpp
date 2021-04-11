#include "dvlnet/loopback.h"
#include "stubs.h"

namespace devilution {
namespace net {

int loopback::create(std::string addrstr, std::string passwd)
{
	return plr_single;
}

int loopback::join(std::string addrstr, std::string passwd)
{
	ABORT();
}

bool loopback::SNetReceiveMessage(int *sender, char **data, int *size)
{
	if (message_queue.empty())
		return false;
	message_last = message_queue.front();
	message_queue.pop();
	*sender = plr_single;
	*size = message_last.size();
	*data = reinterpret_cast<char *>(message_last.data());
	return true;
}

bool loopback::SNetSendMessage(int dest, void *data, unsigned int size)
{
	if (dest == plr_single || dest == SNPLAYER_ALL) {
		unsigned char *raw_message = reinterpret_cast<unsigned char *>(data);
		buffer_t message(raw_message, raw_message + size);
		message_queue.push(message);
	}
	return true;
}

bool loopback::SNetReceiveTurns(char **data, unsigned int *size, DWORD *status)
{
	for (auto i = 0; i < MAX_PLRS; ++i) {
		size[i] = 0;
		data[i] = nullptr;
	}
	return true;
}

bool loopback::SNetSendTurn(char *data, unsigned int size)
{
	return true;
}

int loopback::SNetGetProviderCaps(struct _SNETCAPS *caps)
{
	caps->size = 0;                  // engine writes only ?!?
	caps->flags = 0;                 // unused
	caps->maxmessagesize = 512;      // capped to 512; underflow if < 24
	caps->maxqueuesize = 0;          // unused
	caps->maxplayers = MAX_PLRS;     // capped to 4
	caps->bytessec = 1000000;        // ?
	caps->latencyms = 0;             // unused
	caps->defaultturnssec = 10;      // ?
	caps->defaultturnsintransit = 1; // maximum acceptable number
	                                 // of turns in queue?
	return 1;
}

bool loopback::SNetRegisterEventHandler(event_type evtype,
    SEVTHANDLER func)
{
	// not called in real singleplayer mode
	// not needed in pseudo multiplayer mode (?)
	return true;
}

bool loopback::SNetUnregisterEventHandler(event_type evtype,
    SEVTHANDLER func)
{
	// not called in real singleplayer mode
	// not needed in pseudo multiplayer mode (?)
	return true;
}

bool loopback::SNetLeaveGame(int type)
{
	return true;
}

bool loopback::SNetDropPlayer(int playerid, DWORD flags)
{
	return true;
}

void loopback::setup_gameinfo(buffer_t info)
{
}

bool loopback::SNetGetOwnerTurnsWaiting(DWORD *turns)
{
	*turns = 0;
	return true;
}

bool loopback::SNetGetTurnsInTransit(DWORD *turns)
{
	*turns = 0;
	return true;
}

std::string loopback::make_default_gamename()
{
	return std::string("loopback");
}

} // namespace net
} // namespace devilution

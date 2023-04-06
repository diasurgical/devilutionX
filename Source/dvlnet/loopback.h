#pragma once

#include <queue>
#include <string>

#include "dvlnet/abstract_net.h"

namespace devilution {
namespace net {

class loopback : public abstract_net {
private:
	std::queue<buffer_t> message_queue;
	buffer_t message_last;
	uint8_t plr_single;

public:
	loopback()
	{
		plr_single = 0;
	};

	virtual int create(std::string addrstr);
	virtual int join(std::string addrstr);
	virtual bool SNetReceiveMessage(uint8_t *sender, void **data, uint32_t *size);
	virtual bool SNetSendMessage(int dest, void *data, unsigned int size);
	virtual bool SNetReceiveTurns(char **data, size_t *size, uint32_t *status);
	virtual bool SNetSendTurn(char *data, unsigned int size);
	virtual void SNetGetProviderCaps(struct _SNETCAPS *caps);
	virtual bool SNetRegisterEventHandler(event_type evtype, SEVTHANDLER func);
	virtual bool SNetUnregisterEventHandler(event_type evtype);
	virtual bool SNetLeaveGame(int type);
	virtual bool SNetDropPlayer(int playerid, uint32_t flags);
	virtual bool SNetGetOwnerTurnsWaiting(uint32_t *turns);
	virtual bool SNetGetTurnsInTransit(uint32_t *turns);
	virtual void setup_gameinfo(buffer_t info);
	virtual std::string make_default_gamename();
};

} // namespace net
} // namespace devilution

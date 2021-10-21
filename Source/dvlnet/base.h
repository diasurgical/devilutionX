#pragma once

#include <array>
#include <deque>
#include <map>
#include <memory>
#include <string>

#include "dvlnet/abstract_net.h"
#include "dvlnet/packet.h"

namespace devilution {
namespace net {

class base : public abstract_net {
public:
	virtual int create(std::string addrstr) = 0;
	virtual int join(std::string addrstr) = 0;

	virtual bool SNetReceiveMessage(int *sender, void **data, uint32_t *size);
	virtual bool SNetSendMessage(int playerId, void *data, unsigned int size);
	virtual bool SNetReceiveTurns(char **data, size_t *size, uint32_t *status);
	virtual bool SNetSendTurn(char *data, unsigned int size);
	virtual void SNetGetProviderCaps(struct _SNETCAPS *caps);
	virtual bool SNetRegisterEventHandler(event_type evtype,
	    SEVTHANDLER func);
	virtual bool SNetUnregisterEventHandler(event_type evtype);
	virtual bool SNetLeaveGame(int type);
	virtual bool SNetDropPlayer(int playerid, uint32_t flags);
	virtual bool SNetGetOwnerTurnsWaiting(uint32_t *turns);
	virtual bool SNetGetTurnsInTransit(uint32_t *turns);

	virtual void poll() = 0;
	virtual void send(packet &pkt) = 0;
	virtual void DisconnectNet(plr_t plr);

	void setup_gameinfo(buffer_t info);

	virtual void setup_password(std::string pw);
	virtual void clear_password();

	virtual ~base() = default;

protected:
	std::map<event_type, SEVTHANDLER> registered_handlers;
	buffer_t game_init_info;

	struct message_t {
		int sender; // change int to something else in devilution code later
		buffer_t payload;
		message_t()
		    : sender(-1)
		    , payload({})
		{
		}
		message_t(int s, buffer_t p)
		    : sender(s)
		    , payload(p)
		{
		}
	};

	message_t message_last;
	std::deque<message_t> message_queue;
	std::array<turn_t, MAX_PLRS> turn_last = {};
	std::array<std::deque<turn_t>, MAX_PLRS> turn_queue;
	std::array<bool, MAX_PLRS> connected_table = {};

	plr_t plr_self = PLR_BROADCAST;
	cookie_t cookie_self = 0;

	std::unique_ptr<packet_factory> pktfty;

	void HandleAccept(packet &pkt);
	void RecvLocal(packet &pkt);
	void RunEventHandler(_SNETEVENT &ev);

private:
	plr_t GetOwner();
	void ClearMsg(plr_t plr);
};

} // namespace net
} // namespace devilution

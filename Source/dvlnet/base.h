#pragma once

#include <array>
#include <deque>
#include <map>
#include <memory>
#include <string>

#include "dvlnet/abstract_net.h"
#include "dvlnet/packet.h"
#include "multi.h"
#include "storm/storm_net.hpp"

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

	struct PlayerState {
		bool isConnected = {};
		bool waitForTurns = {};
		std::deque<turn_t> turnQueue;
		int32_t lastTurnValue = {};
	};

	seq_t next_turn;
	message_t message_last;
	std::deque<message_t> message_queue;

	plr_t plr_self = PLR_BROADCAST;
	cookie_t cookie_self = 0;

	std::unique_ptr<packet_factory> pktfty;

	void Connect(plr_t player);
	void RecvLocal(packet &pkt);
	void RunEventHandler(_SNETEVENT &ev);

	[[nodiscard]] bool IsConnected(plr_t player) const;
	virtual bool IsGameHost() = 0;

private:
	std::array<PlayerState, MAX_PLRS> playerStateTable_;

	plr_t GetOwner();
	bool AllTurnsArrived();
	void MakeReady(seq_t sequenceNumber);
	void SendTurnIfReady(turn_t turn);
	void ClearMsg(plr_t plr);

	void HandleAccept(packet &pkt);
	void HandleConnect(packet &pkt);
	void HandleTurn(packet &pkt);
	void HandleDisconnect(packet &pkt);
};

} // namespace net
} // namespace devilution

#pragma once

#include <map>
#include <memory>
#include <set>
#include <string>

#include "dvlnet/base.h"
#include "dvlnet/packet.h"
#include "player.h"
#include "utils/log.hpp"

namespace devilution {
namespace net {

template <class P>
class base_protocol : public base {
public:
	int create(std::string addrstr) override;
	int join(std::string addrstr) override;
	void poll() override;
	void send(packet &pkt) override;
	void DisconnectNet(plr_t plr) override;

	bool SNetLeaveGame(int type) override;

	std::string make_default_gamename() override;
	bool send_info_request() override;
	void clear_gamelist() override;
	std::vector<GameInfo> get_gamelist() override;

	~base_protocol() override = default;

protected:
	bool IsGameHost() override;

private:
	P proto;
	typedef typename P::endpoint endpoint_t;

	struct Peer {
		endpoint_t endpoint;
		std::unique_ptr<std::deque<packet>> sendQueue;
	};

	endpoint_t firstpeer;
	std::string gamename;
	std::map<std::string, std::tuple<GameData, std::vector<std::string>, endpoint_t>> game_list;
	std::array<Peer, MAX_PLRS> peers;
	bool isGameHost_;

	plr_t get_master();
	void InitiateHandshake(plr_t player);
	void SendTo(plr_t player, packet &pkt);
	void DrainSendQueue(plr_t player);
	void recv();
	void handle_join_request(packet &pkt, endpoint_t sender);
	void recv_decrypted(packet &pkt, endpoint_t sender);
	void recv_ingame(packet &pkt, endpoint_t sender);
	bool is_recognized(endpoint_t sender);

	bool wait_network();
	bool wait_firstpeer();
	void wait_join();
};

template <class P>
plr_t base_protocol<P>::get_master()
{
	plr_t ret = plr_self;
	for (plr_t i = 0; i < Players.size(); ++i)
		if (peers[i].endpoint)
			ret = std::min(ret, i);
	return ret;
}

template <class P>
bool base_protocol<P>::wait_network()
{
	// wait for ZeroTier for 5 seconds
	for (auto i = 0; i < 500; ++i) {
		if (proto.network_online())
			break;
		SDL_Delay(10);
	}
	return proto.network_online();
}

template <class P>
void base_protocol<P>::DisconnectNet(plr_t plr)
{
	Peer &peer = peers[plr];
	proto.disconnect(peer.endpoint);
	peer = {};
}

template <class P>
bool base_protocol<P>::wait_firstpeer()
{
	// wait for peer for 5 seconds
	for (auto i = 0; i < 500; ++i) {
		if (game_list.count(gamename)) {
			firstpeer = std::get<2>(game_list[gamename]);
			break;
		}
		send_info_request();
		recv();
		SDL_Delay(10);
	}
	return (bool)firstpeer;
}

template <class P>
bool base_protocol<P>::send_info_request()
{
	if (!proto.network_online())
		return false;
	auto pkt = pktfty->make_packet<PT_INFO_REQUEST>(PLR_BROADCAST, PLR_MASTER);
	proto.send_oob_mc(pkt->Data());
	return true;
}

template <class P>
void base_protocol<P>::wait_join()
{
	cookie_self = packet_out::GenerateCookie();
	auto pkt = pktfty->make_packet<PT_JOIN_REQUEST>(PLR_BROADCAST,
	    PLR_MASTER, cookie_self, game_init_info);
	proto.send(firstpeer, pkt->Data());
	for (auto i = 0; i < 500; ++i) {
		recv();
		if (plr_self != PLR_BROADCAST)
			break; // join successful
		SDL_Delay(10);
	}
}

template <class P>
int base_protocol<P>::create(std::string addrstr)
{
	gamename = addrstr;
	isGameHost_ = true;

	if (wait_network()) {
		plr_self = 0;
		Connect(plr_self);
	}
	return (plr_self == PLR_BROADCAST ? -1 : plr_self);
}

template <class P>
int base_protocol<P>::join(std::string addrstr)
{
	gamename = addrstr;
	isGameHost_ = false;

	if (wait_network()) {
		if (wait_firstpeer())
			wait_join();
	}
	return (plr_self == PLR_BROADCAST ? -1 : plr_self);
}

template <class P>
bool base_protocol<P>::IsGameHost()
{
	return isGameHost_;
}

template <class P>
void base_protocol<P>::poll()
{
	recv();
}

template <class P>
void base_protocol<P>::InitiateHandshake(plr_t player)
{
	Peer &peer = peers[player];

	// The first packet sent will initiate the TCP connection over the ZeroTier network.
	// It will cause problems if both peers attempt to initiate the handshake simultaneously.
	// If the connection is already open, it should be safe to initiate from either end.
	// If not, only the player with the smaller player number should initiate the handshake.
	if (plr_self < player || proto.is_peer_connected(peer.endpoint))
		SendEchoRequest(player);
}

template <class P>
void base_protocol<P>::send(packet &pkt)
{
	plr_t destination = pkt.Destination();
	if (destination < MAX_PLRS) {
		if (destination == MyPlayerId)
			return;
		SendTo(destination, pkt);
	} else if (destination == PLR_BROADCAST) {
		for (plr_t player = 0; player < Players.size(); player++)
			SendTo(player, pkt);
	} else if (destination == PLR_MASTER) {
		throw dvlnet_exception();
	} else {
		throw dvlnet_exception();
	}
}

template <class P>
void base_protocol<P>::SendTo(plr_t player, packet &pkt)
{
	Peer &peer = peers[player];
	if (!peer.endpoint)
		return;

	// The handshake uses echo packets so clients know
	// when they can safely drain their send queues
	if (peer.sendQueue && !IsAnyOf(pkt.Type(), PT_ECHO_REQUEST, PT_ECHO_REPLY))
		peer.sendQueue->push_back(pkt);
	else
		proto.send(peer.endpoint, pkt.Data());
}

template <class P>
void base_protocol<P>::recv()
{
	try {
		buffer_t pkt_buf;
		endpoint_t sender;
		while (proto.recv(sender, pkt_buf)) { // read until kernel buffer is empty?
			try {
				auto pkt = pktfty->make_packet(pkt_buf);
				recv_decrypted(*pkt, sender);
			} catch (packet_exception &e) {
				// drop packet
				proto.disconnect(sender);
				Log("{}", e.what());
			}
		}
		while (proto.get_disconnected(sender)) {
			for (plr_t i = 0; i < Players.size(); ++i) {
				if (peers[i].endpoint == sender) {
					DisconnectNet(i);
					break;
				}
			}
		}
	} catch (std::exception &e) {
		Log("{}", e.what());
		return;
	}
}

template <class P>
void base_protocol<P>::handle_join_request(packet &pkt, endpoint_t sender)
{
	plr_t i;
	for (i = 0; i < Players.size(); ++i) {
		Peer &peer = peers[i];
		if (i != plr_self && !peer.endpoint) {
			peer.endpoint = sender;
			peer.sendQueue = std::make_unique<std::deque<packet>>();
			Connect(i);
			break;
		}
	}
	if (i >= MAX_PLRS) {
		// already full
		return;
	}

	auto senderinfo = sender.serialize();
	for (plr_t j = 0; j < Players.size(); ++j) {
		endpoint_t peer = peers[j].endpoint;
		if ((j != plr_self) && (j != i) && peer) {
			auto peerpkt = pktfty->make_packet<PT_CONNECT>(PLR_MASTER, PLR_BROADCAST, i, senderinfo);
			proto.send(peer, peerpkt->Data());

			auto infopkt = pktfty->make_packet<PT_CONNECT>(PLR_MASTER, PLR_BROADCAST, j, peer.serialize());
			proto.send(sender, infopkt->Data());
		}
	}

	// PT_JOIN_ACCEPT must be sent after all PT_CONNECT packets so the new player does
	// not resume game logic until after having been notified of all existing players
	auto reply = pktfty->make_packet<PT_JOIN_ACCEPT>(plr_self, PLR_BROADCAST,
	    pkt.Cookie(), i,
	    game_init_info);
	proto.send(sender, reply->Data());
	DrainSendQueue(i);
}

template <class P>
void base_protocol<P>::recv_decrypted(packet &pkt, endpoint_t sender)
{
	if (pkt.Source() == PLR_BROADCAST && pkt.Destination() == PLR_MASTER && pkt.Type() == PT_INFO_REPLY) {
		size_t neededSize = sizeof(GameData) + (PlayerNameLength * MAX_PLRS);
		if (pkt.Info().size() < neededSize)
			return;
		const GameData *gameData = (const GameData *)pkt.Info().data();
		std::vector<std::string> playerNames;
		for (size_t i = 0; i < Players.size(); i++) {
			std::string playerName;
			const char *playerNamePointer = (const char *)(pkt.Info().data() + sizeof(GameData) + (i * PlayerNameLength));
			playerName.append(playerNamePointer, strnlen(playerNamePointer, PlayerNameLength));
			if (!playerName.empty())
				playerNames.push_back(playerName);
		}
		std::string gameName;
		size_t gameNameSize = pkt.Info().size() - neededSize;
		gameName.resize(gameNameSize);
		std::memcpy(&gameName[0], pkt.Info().data() + neededSize, gameNameSize);
		game_list[gameName] = std::make_tuple(*gameData, playerNames, sender);
		return;
	}
	recv_ingame(pkt, sender);
}

template <class P>
void base_protocol<P>::recv_ingame(packet &pkt, endpoint_t sender)
{
	if (pkt.Source() == PLR_BROADCAST && pkt.Destination() == PLR_MASTER) {
		if (pkt.Type() == PT_JOIN_REQUEST) {
			handle_join_request(pkt, sender);
		} else if (pkt.Type() == PT_INFO_REQUEST) {
			if ((plr_self != PLR_BROADCAST) && (get_master() == plr_self)) {
				buffer_t buf;
				buf.resize(game_init_info.size() + (PlayerNameLength * MAX_PLRS) + gamename.size());
				std::memcpy(buf.data(), &game_init_info[0], game_init_info.size());
				for (size_t i = 0; i < Players.size(); i++) {
					if (Players[i].plractive) {
						std::memcpy(buf.data() + game_init_info.size() + (i * PlayerNameLength), &Players[i]._pName, PlayerNameLength);
					} else {
						std::memset(buf.data() + game_init_info.size() + (i * PlayerNameLength), '\0', PlayerNameLength);
					}
				}
				std::memcpy(buf.data() + game_init_info.size() + (PlayerNameLength * MAX_PLRS), &gamename[0], gamename.size());
				auto reply = pktfty->make_packet<PT_INFO_REPLY>(PLR_BROADCAST,
				    PLR_MASTER,
				    buf);
				proto.send_oob(sender, reply->Data());
			}
		}
		return;
	} else if (pkt.Source() == PLR_MASTER && pkt.Type() == PT_CONNECT) {
		if (!is_recognized(sender)) {
			LogDebug("Invalid packet: PT_CONNECT received from unrecognized endpoint");
			return;
		}

		// addrinfo packets
		plr_t newPlayer = pkt.NewPlayer();
		Peer &peer = peers[newPlayer];
		peer.endpoint.unserialize(pkt.Info());
		peer.sendQueue = std::make_unique<std::deque<packet>>();
		Connect(newPlayer);

		if (plr_self != PLR_BROADCAST)
			InitiateHandshake(newPlayer);

		return;
	} else if (pkt.Source() >= MAX_PLRS) {
		// normal packets
		LogDebug("Invalid packet: packet source ({}) >= MAX_PLRS", pkt.Source());
		return;
	} else if (sender == firstpeer && pkt.Type() == PT_JOIN_ACCEPT) {
		plr_t src = pkt.Source();
		peers[src].endpoint = sender;
		Connect(src);
		firstpeer = {};
	} else if (sender != peers[pkt.Source()].endpoint) {
		LogDebug("Invalid packet: packet source ({}) received from unrecognized endpoint", pkt.Source());
		return;
	}
	if (pkt.Destination() != plr_self && pkt.Destination() != PLR_BROADCAST)
		return; // packet not for us, drop

	bool wasBroadcast = plr_self == PLR_BROADCAST;
	RecvLocal(pkt);

	if (plr_self != PLR_BROADCAST) {
		if (wasBroadcast) {
			// Send a handshake to everyone just after PT_JOIN_ACCEPT
			for (plr_t player = 0; player < Players.size(); player++)
				InitiateHandshake(player);
		}

		DrainSendQueue(pkt.Source());
	}
}

template <class P>
void base_protocol<P>::DrainSendQueue(plr_t player)
{
	Peer &srcPeer = peers[player];
	if (!srcPeer.sendQueue)
		return;

	std::deque<packet> &sendQueue = *srcPeer.sendQueue;
	while (!sendQueue.empty()) {
		packet &pkt = sendQueue.front();
		proto.send(srcPeer.endpoint, pkt.Data());
		sendQueue.pop_front();
	}

	srcPeer.sendQueue = nullptr;
}

template <class P>
bool base_protocol<P>::is_recognized(endpoint_t sender)
{
	if (!sender)
		return false;

	if (sender == firstpeer)
		return true;

	for (auto player = 0; player <= MAX_PLRS; player++) {
		if (sender == peers[player].endpoint)
			return true;
	}

	return false;
}

template <class P>
void base_protocol<P>::clear_gamelist()
{
	game_list.clear();
}

template <class P>
std::vector<GameInfo> base_protocol<P>::get_gamelist()
{
	recv();
	std::vector<GameInfo> ret;
	for (auto &s : game_list) {
		ret.push_back({ s.first, std::get<0>(s.second), std::get<1>(s.second) });
	}
	return ret;
}

template <class P>
bool base_protocol<P>::SNetLeaveGame(int type)
{
	auto ret = base::SNetLeaveGame(type);
	recv();
	return ret;
}

template <class P>
std::string base_protocol<P>::make_default_gamename()
{
	return proto.make_default_gamename();
}

} // namespace net
} // namespace devilution

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
	virtual int create(std::string addrstr);
	virtual int join(std::string addrstr);
	virtual void poll();
	virtual void send(packet &pkt);
	virtual void DisconnectNet(plr_t plr);

	virtual bool SNetLeaveGame(int type);

	virtual std::string make_default_gamename();
	virtual bool send_info_request();
	virtual void clear_gamelist();
	virtual std::vector<std::string> get_gamelist();

	virtual ~base_protocol() = default;

private:
	P proto;
	typedef typename P::endpoint endpoint;

	endpoint firstpeer;
	std::string gamename;
	std::map<std::string, endpoint> game_list;
	std::array<endpoint, MAX_PLRS> peers;

	plr_t get_master();
	void recv();
	void handle_join_request(packet &pkt, endpoint sender);
	void recv_decrypted(packet &pkt, endpoint sender);
	void recv_ingame(packet &pkt, endpoint sender);

	bool wait_network();
	bool wait_firstpeer();
	void wait_join();
};

template <class P>
plr_t base_protocol<P>::get_master()
{
	plr_t ret = plr_self;
	for (plr_t i = 0; i < MAX_PLRS; ++i)
		if (peers[i])
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
	proto.disconnect(peers[plr]);
	peers[plr] = endpoint();
}

template <class P>
bool base_protocol<P>::wait_firstpeer()
{
	// wait for peer for 5 seconds
	for (auto i = 0; i < 500; ++i) {
		if (game_list.count(gamename)) {
			firstpeer = game_list[gamename];
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

	if (wait_network()) {
		plr_self = 0;
		connected_table[plr_self] = true;
	}
	return (plr_self == PLR_BROADCAST ? -1 : plr_self);
}

template <class P>
int base_protocol<P>::join(std::string addrstr)
{
	gamename = addrstr;
	if (wait_network())
		if (wait_firstpeer())
			wait_join();

	return (plr_self == PLR_BROADCAST ? -1 : plr_self);
}

template <class P>
void base_protocol<P>::poll()
{
	recv();
}

template <class P>
void base_protocol<P>::send(packet &pkt)
{
	if (pkt.Destination() < MAX_PLRS) {
		if (pkt.Destination() == MyPlayerId)
			return;
		if (peers[pkt.Destination()])
			proto.send(peers[pkt.Destination()], pkt.Data());
	} else if (pkt.Destination() == PLR_BROADCAST) {
		for (auto &peer : peers)
			if (peer)
				proto.send(peer, pkt.Data());
	} else if (pkt.Destination() == PLR_MASTER) {
		throw dvlnet_exception();
	} else {
		throw dvlnet_exception();
	}
}

template <class P>
void base_protocol<P>::recv()
{
	try {
		buffer_t pkt_buf;
		endpoint sender;
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
			for (plr_t i = 0; i < MAX_PLRS; ++i) {
				if (peers[i] == sender) {
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
void base_protocol<P>::handle_join_request(packet &pkt, endpoint sender)
{
	plr_t i;
	for (i = 0; i < MAX_PLRS; ++i) {
		if (i != plr_self && !peers[i]) {
			peers[i] = sender;
			break;
		}
	}
	if (i >= MAX_PLRS) {
		// already full
		return;
	}
	for (plr_t j = 0; j < MAX_PLRS; ++j) {
		if ((j != plr_self) && (j != i) && peers[j]) {
			auto infopkt = pktfty->make_packet<PT_CONNECT>(PLR_MASTER, PLR_BROADCAST, j, peers[j].serialize());
			proto.send(sender, infopkt->Data());
		}
	}
	auto reply = pktfty->make_packet<PT_JOIN_ACCEPT>(plr_self, PLR_BROADCAST,
	    pkt.Cookie(), i,
	    game_init_info);
	proto.send(sender, reply->Data());
}

template <class P>
void base_protocol<P>::recv_decrypted(packet &pkt, endpoint sender)
{
	if (pkt.Source() == PLR_BROADCAST && pkt.Destination() == PLR_MASTER && pkt.Type() == PT_INFO_REPLY) {
		std::string pname;
		pname.resize(pkt.Info().size());
		std::memcpy(&pname[0], pkt.Info().data(), pkt.Info().size());
		game_list[pname] = sender;
		return;
	}
	recv_ingame(pkt, sender);
}

template <class P>
void base_protocol<P>::recv_ingame(packet &pkt, endpoint sender)
{
	if (pkt.Source() == PLR_BROADCAST && pkt.Destination() == PLR_MASTER) {
		if (pkt.Type() == PT_JOIN_REQUEST) {
			handle_join_request(pkt, sender);
		} else if (pkt.Type() == PT_INFO_REQUEST) {
			if ((plr_self != PLR_BROADCAST) && (get_master() == plr_self)) {
				buffer_t buf;
				buf.resize(gamename.size());
				std::memcpy(buf.data(), &gamename[0], gamename.size());
				auto reply = pktfty->make_packet<PT_INFO_REPLY>(PLR_BROADCAST,
				    PLR_MASTER,
				    buf);
				proto.send_oob(sender, reply->Data());
			}
		}
		return;
	} else if (pkt.Source() == PLR_MASTER && pkt.Type() == PT_CONNECT) {
		// addrinfo packets
		connected_table[pkt.NewPlayer()] = true;
		peers[pkt.NewPlayer()].unserialize(pkt.Info());
		return;
	} else if (pkt.Source() >= MAX_PLRS) {
		// normal packets
		LogDebug("Invalid packet: packet source ({}) >= MAX_PLRS", pkt.Source());
		return;
	}
	connected_table[pkt.Source()] = true;
	peers[pkt.Source()] = sender;
	if (pkt.Destination() != plr_self && pkt.Destination() != PLR_BROADCAST)
		return; // packet not for us, drop
	RecvLocal(pkt);
}

template <class P>
void base_protocol<P>::clear_gamelist()
{
	game_list.clear();
}

template <class P>
std::vector<std::string> base_protocol<P>::get_gamelist()
{
	recv();
	std::vector<std::string> ret;
	for (auto &s : game_list) {
		ret.push_back(s.first);
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

#include "dvlnet/udp_p2p.h"
#include "options.h"

#include <SDL.h>

#ifdef USE_SDL1
#include "sdl2_to_1_2_backports.h"
#endif

namespace dvl {
namespace net {

const udp_p2p::endpoint udp_p2p::none;

int udp_p2p::create(std::string addrstr, std::string passwd)
{
	sock = asio::ip::udp::socket(io_context); // to be removed later
	setup_password(passwd);
	auto ipaddr = asio::ip::make_address(addrstr);
	if (ipaddr.is_v4())
		sock.open(asio::ip::udp::v4());
	else if (ipaddr.is_v6())
		sock.open(asio::ip::udp::v6());
	sock.non_blocking(true);

	try {
		sock.bind(endpoint(ipaddr, sgOptions.Network.nPort));
	} catch (std::exception &e) {
		SDL_SetError(e.what());
		return -1;
	}
	plr_self = 0;
	return plr_self;
}

int udp_p2p::join(std::string addrstr, std::string passwd)
{
	sock = asio::ip::udp::socket(io_context); // to be removed later
	setup_password(passwd);
	auto ipaddr = asio::ip::make_address(addrstr);
	if (ipaddr.is_v4())
		sock.open(asio::ip::udp::v4());
	else if (ipaddr.is_v6())
		sock.open(asio::ip::udp::v6());
	sock.non_blocking(true);
	endpoint themaster(ipaddr, sgOptions.Network.nPort);
	sock.connect(themaster);
	master = themaster;
	{ // hack: try to join for 5 seconds
		randombytes_buf(reinterpret_cast<unsigned char *>(&cookie_self),
		    sizeof(cookie_t));
		auto pkt = pktfty->make_packet<PT_JOIN_REQUEST>(PLR_BROADCAST,
		    PLR_MASTER, cookie_self,
		    game_init_info);
		send(*pkt);
		for (auto i = 0; i < 5; ++i) {
			recv();
			if (plr_self != PLR_BROADCAST)
				break; // join successful
			SDL_Delay(1000);
		}
	}
	return (plr_self == PLR_BROADCAST ? MAX_PLRS : plr_self);
}

void udp_p2p::poll()
{
	recv();
}

void udp_p2p::send(packet &pkt)
{
	send_internal(pkt, none);
}

void udp_p2p::recv()
{
	try {
		while (1) { // read until kernel buffer is empty?
			try {
				endpoint sender;
				buffer_t pkt_buf(packet_factory::max_packet_size);
				size_t pkt_len;
				pkt_len = sock.receive_from(asio::buffer(pkt_buf), sender);
				pkt_buf.resize(pkt_len);
				auto pkt = pktfty->make_packet(pkt_buf);
				recv_decrypted(*pkt, sender);
			} catch (packet_exception &e) {
				SDL_Log(e.what());
				// drop packet
			}
		}
	} catch (std::exception &e) {
		SDL_Log(e.what());
		return;
	}
}

void udp_p2p::send_internal(packet &pkt, endpoint sender)
{
	for (auto &dest : dests_for_addr(pkt.dest(), sender)) {
		sock.send_to(asio::buffer(pkt.data()), dest);
	}
}

std::set<udp_p2p::endpoint> udp_p2p::dests_for_addr(plr_t dest, endpoint sender)
{
	auto ret = std::set<endpoint>();
	if (dest == plr_self)
		return ret;

	if (dest < MAX_PLRS) {
		if (connected_table[dest])
			ret.insert(nexthop_table[dest]);
	} else if (dest == PLR_BROADCAST) {
		for (auto i = 0; i < MAX_PLRS; ++i)
			if (i != plr_self && connected_table[i])
				ret.insert(nexthop_table[i]);
		ret.insert(connection_requests_pending.begin(),
		    connection_requests_pending.end());
	} else if (dest == PLR_MASTER) {
		if (master != none)
			ret.insert(master);
	}
	ret.erase(sender);
	return ret;
}

void udp_p2p::handle_join_request(packet &pkt, endpoint sender)
{
	plr_t i;
	for (i = 0; i < MAX_PLRS; ++i) {
		if (i != plr_self && nexthop_table[i] == none) {
			nexthop_table[i] = sender;
			break;
		}
	}
	auto reply = pktfty->make_packet<PT_JOIN_ACCEPT>(plr_self, PLR_BROADCAST,
	    pkt.cookie(), i,
	    game_init_info);
	send(*reply);
}

void udp_p2p::recv_decrypted(packet &pkt, endpoint sender)
{
	// 1. route
	send_internal(pkt, sender);
	// 2. handle local
	if (pkt.src() == PLR_BROADCAST && pkt.dest() == PLR_MASTER) {
		connection_requests_pending.insert(sender);
		if (master == none) {
			handle_join_request(pkt, sender);
		}
	}
	// normal packets
	if (pkt.src() >= MAX_PLRS)
		return;                       //drop packet
	if (connected_table[pkt.src()]) { //WRONG?!?
		if (sender != nexthop_table[pkt.src()])
			return; //rpfilter fail: drop packet
	} else {
		nexthop_table[pkt.src()] = sender; // new connection: accept
	}
	connected_table[pkt.src()] = true;
	if (pkt.dest() != plr_self && pkt.dest() != PLR_BROADCAST)
		return; //packet not for us, drop
	recv_local(pkt);
}

} // namespace net
} // namespace dvl

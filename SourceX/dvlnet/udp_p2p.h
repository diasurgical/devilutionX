#pragma once

#include <string>
#include <set>
#include <asio/ts/buffer.hpp>
#include <asio/ts/internet.hpp>
#include <asio/ts/io_context.hpp>
#include <asio/ts/net.hpp>

#include "dvlnet/packet.h"
#include "dvlnet/base.h"

namespace dvl {
namespace net {

class udp_p2p : public base {
public:
	virtual int create(std::string addrstr, std::string passwd);
	virtual int join(std::string addrstr, std::string passwd);
	virtual void poll();
	virtual void send(packet &pkt);

private:
	typedef asio::ip::udp::endpoint endpoint;
	static const endpoint none;

	static constexpr int ACTIVE = 60;

	asio::io_context io_context;
	endpoint master;

	std::set<endpoint> connection_requests_pending;
	std::array<endpoint, MAX_PLRS> nexthop_table;

	asio::ip::udp::socket sock = asio::ip::udp::socket(io_context);

	void recv();
	void handle_join_request(packet &pkt, endpoint sender);
	void send_internal(packet &pkt, endpoint sender = none);
	std::set<endpoint> dests_for_addr(plr_t dest, endpoint sender);
	void recv_decrypted(packet &pkt, endpoint sender);
};

} // namespace net
} // namespace dvl

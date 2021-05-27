#pragma once

#include <string>
#include <memory>
#include <array>
#include <SDL_net.h>

#include "dvlnet/packet.h"
#include "dvlnet/abstract_net.h"
#include "dvlnet/frame_queue.h"

namespace devilution {
namespace net {

class server_exception : public dvlnet_exception {
public:
	const char *what() const throw() override
	{
		return "Invalid player ID";
	}
};

class tcp_server {
public:
	tcp_server(unsigned short port, std::string pw);
	void poll();
	void close();
	virtual ~tcp_server();

private:
	static constexpr int timeout_connect = 30;
	static constexpr int timeout_active = 60;

	struct client_connection {
		frame_queue recv_queue;
		plr_t plr = PLR_BROADCAST;
		TCPsocket socket;
		std::chrono::milliseconds timeout;
		client_connection(TCPsocket socket)
		    : socket(socket)
		{
		}
	};

	typedef std::shared_ptr<client_connection> scc;

	packet_factory pktfty;
	TCPsocket socket;
	SDLNet_SocketSet socketSet = SDLNet_AllocSocketSet(MAX_PLRS);
	std::vector<scc> pendingConnections;
	std::array<scc, MAX_PLRS> connections;
	buffer_t game_init_info;
	buffer_t recv_buffer = buffer_t(frame_queue::max_frame_size);
	int lastTicks = SDL_GetTicks();

	scc make_connection(TCPsocket socket);
	plr_t next_free();
	bool empty();
	bool is_pending_connection_ready();
	void accept();
	void recv();
	void send(const scc &con, packet &pkt);
	void timeout();
	void handle_recv_newplr(const scc &con, packet &pkt);
	void handle_recv_packet(packet &pkt);
	void send_connect(const scc &con);
	void send_packet(packet &pkt);
	void drop_connection(const scc &con);
};

} //namespace net
} //namespace devilution

#pragma once

#include <string>
#include <memory>
#include <array>
#include <asio/ts/buffer.hpp>
#include <asio/ts/internet.hpp>
#include <asio/ts/io_context.hpp>
#include <asio/ts/net.hpp>

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
	tcp_server(asio::io_context &ioc, const std::string &bindaddr,
	    unsigned short port, std::string pw);
	std::string localhost_self();
	void close();
	virtual ~tcp_server();

private:
	static constexpr int timeout_connect = 30;
	static constexpr int timeout_active = 60;

	struct client_connection {
		frame_queue recv_queue;
		buffer_t recv_buffer = buffer_t(frame_queue::max_frame_size);
		plr_t plr = PLR_BROADCAST;
		asio::ip::tcp::socket socket;
		asio::steady_timer timer;
		int timeout;
		client_connection(asio::io_context &ioc)
		    : socket(ioc)
		    , timer(ioc)
		{
		}
	};

	typedef std::shared_ptr<client_connection> scc;

	asio::io_context &ioc;
	packet_factory pktfty;
	std::unique_ptr<asio::ip::tcp::acceptor> acceptor;
	std::array<scc, MAX_PLRS> connections;
	buffer_t game_init_info;

	scc make_connection();
	plr_t next_free();
	bool empty();
	void start_accept();
	void handle_accept(const scc &con, const asio::error_code &ec);
	void start_recv(const scc &con);
	void handle_recv(const scc &con, const asio::error_code &ec, size_t bytesRead);
	void handle_recv_newplr(const scc &con, packet &pkt);
	void handle_recv_packet(packet &pkt);
	void send_connect(const scc &con);
	void send_packet(packet &pkt);
	void start_send(const scc &con, packet &pkt);
	void handle_send(const scc &con, const asio::error_code &ec, size_t bytes_sent);
	void start_timeout(const scc &con);
	void handle_timeout(const scc &con, const asio::error_code &ec);
	void drop_connection(const scc &con);
};

} //namespace net
} //namespace devilution

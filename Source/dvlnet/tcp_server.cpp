#include "dvlnet/tcp_server.h"

#include <chrono>
#include <functional>
#include <memory>
#include <utility>

#include "dvlnet/base.h"
#include "utils/log.hpp"

namespace devilution {
namespace net {

tcp_server::tcp_server(asio::io_context &ioc, const std::string &bindaddr,
    unsigned short port, std::string pw)
    : ioc(ioc)
    , pktfty(std::move(pw))
{
	auto addr = asio::ip::address::from_string(bindaddr);
	auto ep = asio::ip::tcp::endpoint(addr, port);
	acceptor = std::make_unique<asio::ip::tcp::acceptor>(ioc, ep, true);
	start_accept();
}

std::string tcp_server::localhost_self()
{
	auto addr = acceptor->local_endpoint().address();
	if (addr.is_unspecified()) {
		if (addr.is_v4()) {
			return asio::ip::address_v4::loopback().to_string();
		}
		if (addr.is_v6()) {
			return asio::ip::address_v6::loopback().to_string();
		}
		ABORT();
	}
	return addr.to_string();
}

tcp_server::scc tcp_server::make_connection()
{
	return std::make_shared<client_connection>(ioc);
}

plr_t tcp_server::next_free()
{
	for (plr_t i = 0; i < MAX_PLRS; ++i)
		if (!connections[i])
			return i;
	return PLR_BROADCAST;
}

bool tcp_server::empty()
{
	for (plr_t i = 0; i < MAX_PLRS; ++i)
		if (connections[i])
			return false;
	return true;
}

void tcp_server::start_recv(const scc &con)
{
	con->socket.async_receive(
	    asio::buffer(con->recv_buffer),
	    std::bind(&tcp_server::handle_recv, this, con, std::placeholders::_1, std::placeholders::_2));
}

void tcp_server::handle_recv(const scc &con, const asio::error_code &ec,
    size_t bytesRead)
{
	if (ec || bytesRead == 0) {
		drop_connection(con);
		return;
	}
	con->recv_buffer.resize(bytesRead);
	con->recv_queue.write(std::move(con->recv_buffer));
	con->recv_buffer.resize(frame_queue::max_frame_size);
	while (con->recv_queue.packet_ready()) {
		try {
			auto pkt = pktfty.make_packet(con->recv_queue.read_packet());
			if (con->plr == PLR_BROADCAST) {
				handle_recv_newplr(con, *pkt);
			} else {
				con->timeout = timeout_active;
				handle_recv_packet(*pkt);
			}
		} catch (dvlnet_exception &e) {
			Log("Network error: {}", e.what());
			drop_connection(con);
			return;
		}
	}
	start_recv(con);
}

void tcp_server::send_connect(const scc &con)
{
	auto pkt = pktfty.make_packet<PT_CONNECT>(PLR_MASTER, PLR_BROADCAST,
	    con->plr);
	send_packet(*pkt);
}

void tcp_server::handle_recv_newplr(const scc &con, packet &pkt)
{
	auto newplr = next_free();
	if (newplr == PLR_BROADCAST)
		throw server_exception();
	if (empty())
		game_init_info = pkt.info();
	auto reply = pktfty.make_packet<PT_JOIN_ACCEPT>(PLR_MASTER, PLR_BROADCAST,
	    pkt.cookie(), newplr,
	    game_init_info);
	start_send(con, *reply);
	con->plr = newplr;
	connections[newplr] = con;
	con->timeout = timeout_active;
	send_connect(con);
}

void tcp_server::handle_recv_packet(packet &pkt)
{
	send_packet(pkt);
}

void tcp_server::send_packet(packet &pkt)
{
	if (pkt.dest() == PLR_BROADCAST) {
		for (auto i = 0; i < MAX_PLRS; ++i)
			if (i != pkt.src() && connections[i])
				start_send(connections[i], pkt);
	} else {
		if (pkt.dest() >= MAX_PLRS)
			throw server_exception();
		if ((pkt.dest() != pkt.src()) && connections[pkt.dest()])
			start_send(connections[pkt.dest()], pkt);
	}
}

void tcp_server::start_send(const scc &con, packet &pkt)
{
	const auto *frame = new buffer_t(frame_queue::make_frame(pkt.data()));
	auto buf = asio::buffer(*frame);
	asio::async_write(con->socket, buf,
	    [this, con, frame](const asio::error_code &ec, size_t bytesSent) {
		    handle_send(con, ec, bytesSent);
		    delete frame;
	    });
}

void tcp_server::handle_send(const scc &con, const asio::error_code &ec,
    size_t bytesSent)
{
	// empty for now
}

void tcp_server::start_accept()
{
	auto nextcon = make_connection();
	acceptor->async_accept(
	    nextcon->socket,
	    std::bind(&tcp_server::handle_accept, this, nextcon, std::placeholders::_1));
}

void tcp_server::handle_accept(const scc &con, const asio::error_code &ec)
{
	if (ec)
		return;
	if (next_free() == PLR_BROADCAST) {
		drop_connection(con);
	} else {
		asio::ip::tcp::no_delay option(true);
		con->socket.set_option(option);
		con->timeout = timeout_connect;
		start_recv(con);
		start_timeout(con);
	}
	start_accept();
}

void tcp_server::start_timeout(const scc &con)
{
	con->timer.expires_after(std::chrono::seconds(1));
	con->timer.async_wait(
	    std::bind(&tcp_server::handle_timeout, this, con, std::placeholders::_1));
}

void tcp_server::handle_timeout(const scc &con, const asio::error_code &ec)
{
	if (ec) {
		drop_connection(con);
		return;
	}
	if (con->timeout > 0)
		con->timeout -= 1;
	if (con->timeout <= 0) {
		con->timeout = 0;
		drop_connection(con);
		return;
	}
	start_timeout(con);
}

void tcp_server::drop_connection(const scc &con)
{
	if (con->plr != PLR_BROADCAST) {
		auto pkt = pktfty.make_packet<PT_DISCONNECT>(PLR_MASTER, PLR_BROADCAST,
		    con->plr, LEAVE_DROP);
		connections[con->plr] = nullptr;
		send_packet(*pkt);
		// TODO: investigate if it is really ok for the server to
		//       drop a client directly.
	}
	con->timer.cancel();
	con->socket.close();
}

void tcp_server::close()
{
	acceptor->close();
}

tcp_server::~tcp_server()
    = default;

} // namespace net
} // namespace devilution

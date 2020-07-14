#include "dvlnet/tcp_client.h"

#include <functional>
#include <exception>
#include <sstream>
#include <system_error>
#include <stdexcept>
#include <sodium.h>
#include <SDL.h>

#include <asio/connect.hpp>

namespace dvl {
namespace net {

int tcp_client::create(std::string addrstr, std::string passwd)
{
	try {
		auto port = default_port;
		local_server.reset(new tcp_server(ioc, addrstr, port, passwd));
		return join(local_server->localhost_self(), passwd);
	} catch (std::system_error &e) {
		SDL_SetError(e.what());
		return -1;
	}
}

int tcp_client::join(std::string addrstr, std::string passwd)
{
	constexpr int ms_sleep = 10;
	constexpr int no_sleep = 250;

	setup_password(passwd);
	try {
		std::stringstream port;
		port << default_port;
		asio::connect(sock, resolver.resolve(addrstr, port.str()));
		asio::ip::tcp::no_delay option(true);
		sock.set_option(option);
	} catch (std::exception &e) {
		SDL_SetError(e.what());
		return -1;
	}
	start_recv();
	{
		randombytes_buf(reinterpret_cast<unsigned char *>(&cookie_self),
		    sizeof(cookie_t));
		auto pkt = pktfty->make_packet<PT_JOIN_REQUEST>(PLR_BROADCAST,
		    PLR_MASTER, cookie_self,
		    game_init_info);
		send(*pkt);
		for (auto i = 0; i < no_sleep; ++i) {
			try {
				poll();
			} catch (const std::runtime_error &e) {
				SDL_SetError(e.what());
				return -1;
			}
			if (plr_self != PLR_BROADCAST)
				break; // join successful
			SDL_Delay(ms_sleep);
		}
	}
	if (plr_self == PLR_BROADCAST) {
		SDL_SetError("Unable to connect");
		return -1;
	}

	return plr_self;
}

void tcp_client::poll()
{
	ioc.poll();
}

void tcp_client::handle_recv(const asio::error_code &error, size_t bytes_read)
{
	if (error) {
		// error in recv from server
		// returning and doing nothing should be the same
		// as if all connections to other clients were lost
		return;
	}
	if (bytes_read == 0) {
		throw std::runtime_error("error: read 0 bytes from server");
	}
	recv_buffer.resize(bytes_read);
	recv_queue.write(std::move(recv_buffer));
	recv_buffer.resize(frame_queue::max_frame_size);
	while (recv_queue.packet_ready()) {
		auto pkt = pktfty->make_packet(recv_queue.read_packet());
		recv_local(*pkt);
	}
	start_recv();
}

void tcp_client::start_recv()
{
	sock.async_receive(asio::buffer(recv_buffer),
	    std::bind(&tcp_client::handle_recv, this,
	        std::placeholders::_1, std::placeholders::_2));
}

void tcp_client::handle_send(const asio::error_code &error, size_t bytes_sent)
{
	// empty for now
}

void tcp_client::send(packet &pkt)
{
	const auto *frame = new buffer_t(frame_queue::make_frame(pkt.data()));
	auto buf = asio::buffer(*frame);
	asio::async_write(sock, buf, [this, frame](const asio::error_code &error, size_t bytes_sent) {
		handle_send(error, bytes_sent);
		delete frame;
	});
}

bool tcp_client::SNetLeaveGame(int type)
{
	auto ret = base::SNetLeaveGame(type);
	poll();
	local_server.reset();
	return ret;
}

tcp_client::~tcp_client()
{
}

} // namespace net
} // namespace dvl

#include "dvlnet/tcp_client.h"
#include "options.h"
#include "utils/language.h"

#include <SDL.h>
#include <exception>
#include <functional>
#include <memory>
#include <sodium.h>
#include <sstream>
#include <stdexcept>
#include <system_error>

#include <asio/connect.hpp>

namespace devilution {
namespace net {

int tcp_client::create(std::string addrstr, std::string passwd)
{
	try {
		auto port = sgOptions.Network.nPort;
		local_server = std::make_unique<tcp_server>(ioc, addrstr, port, passwd);
		return join(local_server->LocalhostSelf(), passwd);
	} catch (std::system_error &e) {
		SDL_SetError("%s", e.what());
		return -1;
	}
}

int tcp_client::join(std::string addrstr, std::string passwd)
{
	constexpr int MsSleep = 10;
	constexpr int NoSleep = 250;

	setup_password(passwd);
	try {
		std::stringstream port;
		port << sgOptions.Network.nPort;
		asio::connect(sock, resolver.resolve(addrstr, port.str()));
		asio::ip::tcp::no_delay option(true);
		sock.set_option(option);
	} catch (std::exception &e) {
		SDL_SetError("%s", e.what());
		return -1;
	}
	StartReceive();
	{
		randombytes_buf(reinterpret_cast<unsigned char *>(&cookie_self),
		    sizeof(cookie_t));
		auto pkt = pktfty->make_packet<PT_JOIN_REQUEST>(PLR_BROADCAST,
		    PLR_MASTER, cookie_self,
		    game_init_info);
		send(*pkt);
		for (auto i = 0; i < NoSleep; ++i) {
			try {
				poll();
			} catch (const std::runtime_error &e) {
				SDL_SetError("%s", e.what());
				return -1;
			}
			if (plr_self != PLR_BROADCAST)
				break; // join successful
			SDL_Delay(MsSleep);
		}
	}
	if (plr_self == PLR_BROADCAST) {
		SDL_SetError("%s", _("Unable to connect"));
		return -1;
	}

	return plr_self;
}

void tcp_client::poll()
{
	ioc.poll();
}

void tcp_client::HandleReceive(const asio::error_code &error, size_t bytesRead)
{
	if (error) {
		// error in recv from server
		// returning and doing nothing should be the same
		// as if all connections to other clients were lost
		return;
	}
	if (bytesRead == 0) {
		throw std::runtime_error(_("error: read 0 bytes from server"));
	}
	recv_buffer.resize(bytesRead);
	recv_queue.Write(std::move(recv_buffer));
	recv_buffer.resize(frame_queue::max_frame_size);
	while (recv_queue.PacketReady()) {
		auto pkt = pktfty->make_packet(recv_queue.ReadPacket());
		RecvLocal(*pkt);
	}
	StartReceive();
}

void tcp_client::StartReceive()
{
	sock.async_receive(
	    asio::buffer(recv_buffer),
	    std::bind(&tcp_client::HandleReceive, this, std::placeholders::_1, std::placeholders::_2));
}

void tcp_client::HandleSend(const asio::error_code &error, size_t bytesSent)
{
	// empty for now
}

void tcp_client::send(packet &pkt)
{
	auto frame = std::make_unique<buffer_t>(frame_queue::MakeFrame(pkt.Data()));
	auto buf = asio::buffer(*frame);
	asio::async_write(sock, buf, [this, frame = std::move(frame)](const asio::error_code &error, size_t bytesSent) {
		HandleSend(error, bytesSent);
	});
}

bool tcp_client::SNetLeaveGame(int type)
{
	auto ret = base::SNetLeaveGame(type);
	poll();
	if (local_server != nullptr)
		local_server->Close();
	sock.close();
	return ret;
}

std::string tcp_client::make_default_gamename()
{
	return std::string(sgOptions.Network.szBindAddress);
}

tcp_client::~tcp_client()
    = default;

} // namespace net
} // namespace devilution

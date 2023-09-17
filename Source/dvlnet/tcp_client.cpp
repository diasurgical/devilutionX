#include "dvlnet/tcp_client.h"

#include <exception>
#include <functional>
#include <memory>
#include <stdexcept>
#include <system_error>

#include <SDL.h>
#include <asio/connect.hpp>
#include <expected.hpp>

#include "options.h"
#include "utils/language.h"

namespace devilution::net {

int tcp_client::create(std::string addrstr)
{
	auto port = *sgOptions.Network.port;
	local_server = std::make_unique<tcp_server>(ioc, addrstr, port, *pktfty);
	return join(local_server->LocalhostSelf());
}

int tcp_client::join(std::string addrstr)
{
	constexpr int MsSleep = 10;
	constexpr int NoSleep = 250;

	std::string port = StrCat(*sgOptions.Network.port);

	asio::error_code errorCode;
	asio::ip::basic_resolver_results<asio::ip::tcp> range = resolver.resolve(addrstr, port, errorCode);
	if (errorCode) {
		SDL_SetError("%s", errorCode.message().c_str());
		return -1;
	}

	asio::connect(sock, range, errorCode);
	if (errorCode) {
		SDL_SetError("%s", errorCode.message().c_str());
		return -1;
	}

	asio::ip::tcp::no_delay option(true);
	sock.set_option(option, errorCode);
	if (errorCode)
		LogError("Client error setting socket option: {}", errorCode.message());

	StartReceive();
	{
		cookie_self = packet_out::GenerateCookie();
		tl::expected<std::unique_ptr<packet>, PacketError> pkt
		    = pktfty->make_packet<PT_JOIN_REQUEST>(
		        PLR_BROADCAST, PLR_MASTER, cookie_self, game_init_info);
		if (!pkt.has_value()) {
			SDL_SetError("make_packet: %s", pkt.error().what());
			return -1;
		}
		send(**pkt);
		for (auto i = 0; i < NoSleep; ++i) {
			try {
				poll();
			} catch (const dvlnet_exception &e) {
				SDL_SetError("Network error: %s", e.what());
				return -1;
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
		SDL_SetError("%s", _("Unable to connect").data());
		return -1;
	}

	return plr_self;
}

bool tcp_client::IsGameHost()
{
	return local_server != nullptr;
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
		throw std::runtime_error(_("error: read 0 bytes from server").data());
	}
	recv_buffer.resize(bytesRead);
	recv_queue.Write(std::move(recv_buffer));
	recv_buffer.resize(frame_queue::max_frame_size);
	while (recv_queue.PacketReady()) {
		tl::expected<std::unique_ptr<packet>, PacketError> pkt = pktfty->make_packet(recv_queue.ReadPacket());
		if (!pkt.has_value()) {
			LogError("make_packet: {}", pkt.error().what());
			return;
		}
		if (tl::expected<void, PacketError> result = RecvLocal(**pkt); !result.has_value()) {
			LogError("RecvLocal: {}", result.error().what());
			return;
		}
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

} // namespace devilution::net

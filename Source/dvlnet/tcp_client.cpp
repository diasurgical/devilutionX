#include "dvlnet/tcp_client.h"
#include "options.h"
#include "utils/language.h"

#include <SDL.h>
#include <SDL_net.h>
#include <exception>
#include <functional>
#include <memory>
#include <sodium.h>
#include <sstream>
#include <stdexcept>
#include <system_error>

namespace devilution {
namespace net {

int tcp_client::create(std::string addrstr, std::string passwd)
{
	auto port = sgOptions.Network.nPort;
	local_server = std::make_unique<tcp_server>(port, passwd);
	return join(std::string("localhost"), passwd);
}

int tcp_client::join(std::string addrstr, std::string passwd)
{
	constexpr int MsSleep = 10;
	constexpr int NoSleep = 250;

	setup_password(passwd);

	IPaddress ip;
	auto port = sgOptions.Network.nPort;
	if (SDLNet_ResolveHost(&ip, addrstr.c_str(), port) == -1) {
		auto error = SDLNet_GetError();
		SDL_SetError("SDL_net: %s", error);
		return -1;
	}

	socket = SDLNet_TCP_Open(&ip);
	if (!socket) {
		auto error = SDLNet_GetError();
		SDL_SetError("SDL_net: %s", error);
		return -1;
	}
	SDLNet_TCP_AddSocket(socketSet, socket);

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
	if (local_server != nullptr)
		local_server->poll();

	while (SDLNet_CheckSockets(socketSet, 0) > 0) {
		auto *buffer = recv_buffer.data();
		const auto maxLength = frame_queue::max_frame_size;
		const auto bytesRead = SDLNet_TCP_Recv(socket, buffer, maxLength);

		if (bytesRead <= 0) {
			Log("error: read 0 bytes from server");
			SDLNet_TCP_DelSocket(socketSet, socket);
			return;
		}

		recv_buffer.resize(bytesRead);
		recv_queue.write(std::move(recv_buffer));
		recv_buffer.resize(frame_queue::max_frame_size);
		while (recv_queue.packet_ready()) {
			auto pkt = pktfty->make_packet(recv_queue.read_packet());
			recv_local(*pkt);
		}
	}
}

void tcp_client::send(packet &pkt)
{
	const auto *frame = new buffer_t(frame_queue::make_frame(pkt.data()));
	const auto *buffer = frame->data();
	const auto length = frame->size();
	SDLNet_TCP_Send(socket, buffer, length);
	delete frame;
}

bool tcp_client::SNetLeaveGame(int type)
{
	auto ret = base::SNetLeaveGame(type);
	poll();
	SDLNet_TCP_Close(socket);
	SDLNet_FreeSocketSet(socketSet);
	if (local_server != nullptr)
		local_server->close();
	return ret;
}

std::string tcp_client::make_default_gamename()
{
	return std::string("0.0.0.0");
}

tcp_client::~tcp_client()
{
}

} // namespace net
} // namespace devilution

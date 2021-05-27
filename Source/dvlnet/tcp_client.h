#pragma once

#include <string>
#include <memory>
#include <SDL_net.h>

#include "dvlnet/packet.h"
#include "dvlnet/frame_queue.h"
#include "dvlnet/base.h"
#include "dvlnet/tcp_server.h"

namespace devilution {
namespace net {

class tcp_client : public base {
public:
	int create(std::string addrstr, std::string passwd);
	int join(std::string addrstr, std::string passwd);

	virtual void poll();
	virtual void send(packet &pkt);

	virtual bool SNetLeaveGame(int type);

	virtual ~tcp_client();

	virtual std::string make_default_gamename();

private:
	frame_queue recv_queue;
	buffer_t recv_buffer = buffer_t(frame_queue::max_frame_size);

	TCPsocket socket;
	SDLNet_SocketSet socketSet = SDLNet_AllocSocketSet(1);
	std::unique_ptr<tcp_server> local_server; // must be declared *after* ioc
};

} // namespace net
} // namespace devilution

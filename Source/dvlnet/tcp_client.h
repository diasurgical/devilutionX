#pragma once

#include <memory>
#include <string>

#include <asio/ts/buffer.hpp>
#include <asio/ts/internet.hpp>
#include <asio/ts/io_context.hpp>
#include <asio/ts/net.hpp>

#include "dvlnet/base.h"
#include "dvlnet/frame_queue.h"
#include "dvlnet/packet.h"
#include "dvlnet/tcp_server.h"

namespace devilution {
namespace net {

class tcp_client : public base {
public:
	int create(std::string addrstr);
	int join(std::string addrstr);

	virtual void poll();
	virtual void send(packet &pkt);

	virtual bool SNetLeaveGame(int type);

	virtual ~tcp_client();

	virtual std::string make_default_gamename();

private:
	frame_queue recv_queue;
	buffer_t recv_buffer = buffer_t(frame_queue::max_frame_size);

	asio::io_context ioc;
	asio::ip::tcp::resolver resolver = asio::ip::tcp::resolver(ioc);
	asio::ip::tcp::socket sock = asio::ip::tcp::socket(ioc);
	std::unique_ptr<tcp_server> local_server; // must be declared *after* ioc

	void HandleReceive(const asio::error_code &error, size_t bytesRead);
	void StartReceive();
	void HandleSend(const asio::error_code &error, size_t bytesSent);
};

} // namespace net
} // namespace devilution

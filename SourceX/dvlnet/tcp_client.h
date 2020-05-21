#pragma once

#include <string>
#include <memory>
#include <asio/ts/buffer.hpp>
#include <asio/ts/internet.hpp>
#include <asio/ts/io_context.hpp>
#include <asio/ts/net.hpp>

#include "dvlnet/packet.h"
#include "dvlnet/frame_queue.h"
#include "dvlnet/base.h"
#include "dvlnet/tcp_server.h"

namespace dvl {
namespace net {

class tcp_client : public base {
public:
	int create(std::string addrstr, std::string passwd);
	int join(std::string addrstr, std::string passwd);

	constexpr static unsigned short default_port = 6112;

	virtual void poll();
	virtual void send(packet &pkt);

	virtual bool SNetLeaveGame(int type);

	virtual ~tcp_client();

private:
	frame_queue recv_queue;
	buffer_t recv_buffer = buffer_t(frame_queue::max_frame_size);

	asio::io_context ioc;
	asio::ip::tcp::resolver resolver = asio::ip::tcp::resolver(ioc);
	asio::ip::tcp::socket sock = asio::ip::tcp::socket(ioc);
	std::unique_ptr<tcp_server> local_server; // must be declared *after* ioc

	void handle_recv(const asio::error_code &error, size_t bytes_read);
	void start_recv();
	void handle_send(const asio::error_code &error, size_t bytes_sent);
};

} // namespace net
} // namespace dvl

#pragma once

#include <deque>

#include "dvlnet/abstract_net.h"

namespace dvl {
namespace net {

class frame_queue_exception : public dvlnet_exception {};

class frame_queue {
public:
	typedef uint32_t framesize_t;
	constexpr static framesize_t max_frame_size = 0xFFFF;
private:
	size_t current_size = 0;
	std::deque<buffer_t> buffer_deque;
	size_t nextsize = 0;

	size_t size();
	buffer_t read(size_t s);
public:
	bool packet_ready();
	buffer_t read_packet();
	void write(buffer_t buf);

	static buffer_t make_frame(buffer_t packetbuf);
};

}  // namespace net
}  // namespace dvl

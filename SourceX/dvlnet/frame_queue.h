#pragma once

#include <deque>
#include <exception>
#include <vector>
#include <cstdint>

namespace dvl {
namespace net {

typedef std::vector<unsigned char> buffer_t;

class frame_queue_exception : public std::exception {
public:
	const char *what() const throw() override
	{
		return "Incorrect frame size";
	}
};

typedef uint32_t framesize_t;

class frame_queue {
public:
	constexpr static framesize_t max_frame_size = 0xFFFF;

private:
	framesize_t current_size = 0;
	std::deque<buffer_t> buffer_deque;
	framesize_t nextsize = 0;

	framesize_t size();
	buffer_t read(framesize_t s);

public:
	bool packet_ready();
	buffer_t read_packet();
	void write(buffer_t buf);

	static buffer_t make_frame(buffer_t packetbuf);
};

} // namespace net
} // namespace dvl

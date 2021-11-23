#pragma once

#include <cstdint>
#include <deque>
#include <exception>
#include <vector>

namespace devilution {
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

	framesize_t Size() const;
	buffer_t Read(framesize_t s);

public:
	bool PacketReady();
	buffer_t ReadPacket();
	void Write(buffer_t buf);

	static buffer_t MakeFrame(buffer_t packetbuf);
};

} // namespace net
} // namespace devilution

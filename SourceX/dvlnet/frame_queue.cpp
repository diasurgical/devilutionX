#include "dvlnet/frame_queue.h"

#include <cstring>

#include "dvlnet/packet.h"

namespace dvl {
namespace net {

size_t frame_queue::size()
{
	return current_size;
}

buffer_t frame_queue::read(size_t s)
{
	if(current_size < s)
    	printf("AAAA exception error\n");//throw frame_queue_exception();
	buffer_t ret;
	while (s > 0 && s >= buffer_deque.front().size()) {
		s -= buffer_deque.front().size();
		current_size -= buffer_deque.front().size();
		ret.insert(ret.end(),
		           buffer_deque.front().begin(),
		           buffer_deque.front().end());
		buffer_deque.pop_front();
	}
	if(s > 0) {
		ret.insert(ret.end(),
		           buffer_deque.front().begin(),
		           buffer_deque.front().begin()+s);
		buffer_deque.front().erase(buffer_deque.front().begin(),
		                           buffer_deque.front().begin()+s);
		current_size -= s;
	}
	return std::move(ret);
}

void frame_queue::write(buffer_t buf)
{
	current_size += buf.size();
	buffer_deque.push_back(std::move(buf));
}

bool frame_queue::packet_ready()
{
	if(!nextsize) {
		if(size() < sizeof(framesize_t))
			return false;
		auto szbuf = read(sizeof(framesize_t));
		std::memcpy(&nextsize, &szbuf[0], sizeof(nextsize));
		if(!nextsize)
			printf("AAAA exception error\n");
		//throw frame_queue_exception();
	}
	if(size() >= nextsize)
		return true;
	else
		return false;
}

buffer_t frame_queue::read_packet()
{
	if(!nextsize || (size() < nextsize))
		printf("AAAA exception error\n");
	//throw frame_queue_exception();
	auto ret = std::move(read(nextsize));
	nextsize = 0;
	return std::move(ret);
}

buffer_t frame_queue::make_frame(buffer_t packetbuf)
{
	buffer_t ret;
	if(packetbuf.size() > max_frame_size)
		ABORT();
	frame_queue::framesize_t size = packetbuf.size();
	ret.insert(ret.end(), packet_out::begin(size), packet_out::end(size));
	ret.insert(ret.end(), packetbuf.begin(), packetbuf.end());
	return std::move(ret);
}

}  // namespace net
}  // namespace dvl

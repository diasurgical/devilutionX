#pragma once

#include <algorithm>
#include <array>
#include <atomic>
#include <deque>
#include <exception>
#include <map>
#include <set>
#include <string>

#include "dvlnet/frame_queue.h"

namespace devilution {
namespace net {

class protocol_exception : public std::exception {
public:
	const char *what() const throw() override
	{
		return "Protocol error";
	}
};

class protocol_zt {
public:
	class endpoint {
	public:
		std::array<unsigned char, 16> addr = {};

		explicit operator bool() const
		{
			auto empty = std::array<unsigned char, 16> {};
			return (addr != empty);
		}

		bool operator==(const endpoint &rhs) const
		{
			return addr == rhs.addr;
		}

		bool operator!=(const endpoint &rhs) const
		{
			return !(*this == rhs);
		}

		bool operator<(const endpoint &rhs) const
		{
			return addr < rhs.addr;
		}

		buffer_t serialize() const
		{
			return buffer_t(addr.begin(), addr.end());
		}

		void unserialize(const buffer_t &buf)
		{
			if (buf.size() != 16)
				throw protocol_exception();
			std::copy(buf.begin(), buf.end(), addr.begin());
		}

		void from_string(const std::string &str);
	};

	protocol_zt();
	~protocol_zt();
	void disconnect(const endpoint &peer);
	bool send(const endpoint &peer, const buffer_t &data);
	bool send_oob(const endpoint &peer, const buffer_t &data) const;
	bool send_oob_mc(const buffer_t &data) const;
	bool recv(endpoint &peer, buffer_t &data);
	bool get_disconnected(endpoint &peer);
	bool network_online();
	static std::string make_default_gamename();

private:
	static constexpr uint32_t PKTBUF_LEN = 65536;
	static constexpr uint16_t default_port = 6112;

	struct peer_state {
		int fd = -1;
		std::deque<buffer_t> send_queue;
		frame_queue recv_queue;
	};

	std::deque<std::pair<endpoint, buffer_t>> oob_recv_queue;
	std::deque<endpoint> disconnect_queue;

	std::map<endpoint, peer_state> peer_list;
	int fd_tcp = -1;
	int fd_udp = -1;

	static uint64_t current_ms();
	void close_all();

	static void set_nonblock(int fd);
	static void set_nodelay(int fd);
	static void set_reuseaddr(int fd);

	bool send_queued_peer(const endpoint &peer);
	bool recv_peer(const endpoint &peer);
	bool send_queued_all();
	bool recv_from_peers();
	bool recv_from_udp();
	bool accept_all();
};

} // namespace net
} // namespace devilution

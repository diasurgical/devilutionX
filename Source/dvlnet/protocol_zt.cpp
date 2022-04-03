#include "dvlnet/protocol_zt.h"

#include <random>

#include <SDL.h>

#ifdef USE_SDL1
#include "utils/sdl2_to_1_2_backports.h"
#else
#include "utils/sdl2_backports.h"
#endif

#include <lwip/igmp.h>
#include <lwip/mld6.h>
#include <lwip/sockets.h>
#include <lwip/tcpip.h>

#include "dvlnet/zerotier_native.h"
#include "utils/log.hpp"

namespace devilution {
namespace net {

protocol_zt::protocol_zt()
{
	zerotier_network_start();
}

void protocol_zt::set_nonblock(int fd)
{
	static_assert(O_NONBLOCK == 1, "O_NONBLOCK == 1 not satisfied");
	auto mode = lwip_fcntl(fd, F_GETFL, 0);
	mode |= O_NONBLOCK;
	lwip_fcntl(fd, F_SETFL, mode);
}

void protocol_zt::set_nodelay(int fd)
{
	const int yes = 1;
	lwip_setsockopt(fd, IPPROTO_TCP, TCP_NODELAY, (void *)&yes, sizeof(yes));
}

void protocol_zt::set_reuseaddr(int fd)
{
	const int yes = 1;
	lwip_setsockopt(fd, SOL_SOCKET, SO_REUSEADDR, (void *)&yes, sizeof(yes));
}

bool protocol_zt::network_online()
{
	if (!zerotier_network_ready())
		return false;

	struct sockaddr_in6 in6 {
	};
	in6.sin6_port = htons(default_port);
	in6.sin6_family = AF_INET6;
	in6.sin6_addr = in6addr_any;

	if (fd_udp == -1) {
		fd_udp = lwip_socket(AF_INET6, SOCK_DGRAM, 0);
		set_reuseaddr(fd_udp);
		auto ret = lwip_bind(fd_udp, (struct sockaddr *)&in6, sizeof(in6));
		if (ret < 0) {
			Log("lwip, (udp) bind: {}", strerror(errno));
			SDL_SetError("lwip, (udp) bind: %s", strerror(errno));
			throw protocol_exception();
		}
		set_nonblock(fd_udp);
	}
	if (fd_tcp == -1) {
		fd_tcp = lwip_socket(AF_INET6, SOCK_STREAM, 0);
		set_reuseaddr(fd_tcp);
		auto r1 = lwip_bind(fd_tcp, (struct sockaddr *)&in6, sizeof(in6));
		if (r1 < 0) {
			Log("lwip, (tcp) bind: {}", strerror(errno));
			SDL_SetError("lwip, (udp) bind: %s", strerror(errno));
			throw protocol_exception();
		}
		auto r2 = lwip_listen(fd_tcp, 10);
		if (r2 < 0) {
			Log("lwip, listen: {}", strerror(errno));
			SDL_SetError("lwip, listen: %s", strerror(errno));
			throw protocol_exception();
		}
		set_nonblock(fd_tcp);
		set_nodelay(fd_tcp);
	}
	return true;
}

bool protocol_zt::send(const endpoint &peer, const buffer_t &data)
{
	peer_list[peer].send_queue.push_back(frame_queue::MakeFrame(data));
	return true;
}

bool protocol_zt::send_oob(const endpoint &peer, const buffer_t &data) const
{
	struct sockaddr_in6 in6 {
	};
	in6.sin6_port = htons(default_port);
	in6.sin6_family = AF_INET6;
	std::copy(peer.addr.begin(), peer.addr.end(), in6.sin6_addr.s6_addr);
	lwip_sendto(fd_udp, data.data(), data.size(), 0, (const struct sockaddr *)&in6, sizeof(in6));
	return true;
}

bool protocol_zt::send_oob_mc(const buffer_t &data) const
{
	endpoint mc;
	std::copy(dvl_multicast_addr, dvl_multicast_addr + 16, mc.addr.begin());
	return send_oob(mc, data);
}

bool protocol_zt::send_queued_peer(const endpoint &peer)
{
	if (peer_list[peer].fd == -1) {
		peer_list[peer].fd = lwip_socket(AF_INET6, SOCK_STREAM, 0);
		set_nodelay(peer_list[peer].fd);
		set_nonblock(peer_list[peer].fd);
		struct sockaddr_in6 in6 {
		};
		in6.sin6_port = htons(default_port);
		in6.sin6_family = AF_INET6;
		std::copy(peer.addr.begin(), peer.addr.end(), in6.sin6_addr.s6_addr);
		lwip_connect(peer_list[peer].fd, (const struct sockaddr *)&in6, sizeof(in6));
	}
	while (!peer_list[peer].send_queue.empty()) {
		auto len = peer_list[peer].send_queue.front().size();
		auto r = lwip_send(peer_list[peer].fd, peer_list[peer].send_queue.front().data(), len, 0);
		if (r < 0) {
			// handle error
			return false;
		}
		if (decltype(len)(r) < len) {
			// partial send
			auto it = peer_list[peer].send_queue.front().begin();
			peer_list[peer].send_queue.front().erase(it, it + r);
			return true;
		}
		if (decltype(len)(r) == len) {
			peer_list[peer].send_queue.pop_front();
		} else {
			throw protocol_exception();
		}
	}
	return true;
}

bool protocol_zt::recv_peer(const endpoint &peer)
{
	unsigned char buf[PKTBUF_LEN];
	while (true) {
		auto len = lwip_recv(peer_list[peer].fd, buf, sizeof(buf), 0);
		if (len >= 0) {
			peer_list[peer].recv_queue.Write(buffer_t(buf, buf + len));
		} else {
			return errno == EAGAIN || errno == EWOULDBLOCK;
		}
	}
}

bool protocol_zt::send_queued_all()
{
	for (auto &peer : peer_list) {
		if (!send_queued_peer(peer.first)) {
			// handle error?
		}
	}
	return true;
}

bool protocol_zt::recv_from_peers()
{
	for (auto &peer : peer_list) {
		if (peer.second.fd != -1) {
			if (!recv_peer(peer.first)) {
				disconnect_queue.push_back(peer.first);
			}
		}
	}
	return true;
}

bool protocol_zt::recv_from_udp()
{
	unsigned char buf[PKTBUF_LEN];
	struct sockaddr_in6 in6 {
	};
	socklen_t addrlen = sizeof(in6);
	auto len = lwip_recvfrom(fd_udp, buf, sizeof(buf), 0, (struct sockaddr *)&in6, &addrlen);
	if (len < 0)
		return false;
	buffer_t data(buf, buf + len);
	endpoint ep;
	std::copy(in6.sin6_addr.s6_addr, in6.sin6_addr.s6_addr + 16, ep.addr.begin());
	oob_recv_queue.emplace_back(ep, std::move(data));
	return true;
}

bool protocol_zt::accept_all()
{
	struct sockaddr_in6 in6 {
	};
	socklen_t addrlen = sizeof(in6);
	while (true) {
		auto newfd = lwip_accept(fd_tcp, (struct sockaddr *)&in6, &addrlen);
		if (newfd < 0)
			break;
		endpoint ep;
		std::copy(in6.sin6_addr.s6_addr, in6.sin6_addr.s6_addr + 16, ep.addr.begin());
		if (peer_list[ep].fd != -1) {
			Log("protocol_zt::accept_all: WARNING: overwriting connection");
			SDL_SetError("protocol_zt::accept_all: WARNING: overwriting connection");
			lwip_close(peer_list[ep].fd);
		}
		set_nonblock(newfd);
		set_nodelay(newfd);
		peer_list[ep].fd = newfd;
	}
	return true;
}

bool protocol_zt::recv(endpoint &peer, buffer_t &data)
{
	accept_all();
	send_queued_all();
	recv_from_peers();
	recv_from_udp();

	if (!oob_recv_queue.empty()) {
		peer = oob_recv_queue.front().first;
		data = oob_recv_queue.front().second;
		oob_recv_queue.pop_front();
		return true;
	}

	for (auto &p : peer_list) {
		if (p.second.recv_queue.PacketReady()) {
			peer = p.first;
			data = p.second.recv_queue.ReadPacket();
			return true;
		}
	}
	return false;
}

bool protocol_zt::get_disconnected(endpoint &peer)
{
	if (!disconnect_queue.empty()) {
		peer = disconnect_queue.front();
		disconnect_queue.pop_front();
		return true;
	}
	return false;
}

void protocol_zt::disconnect(const endpoint &peer)
{
	if (peer_list.count(peer) != 0) {
		if (peer_list[peer].fd != -1) {
			if (lwip_close(peer_list[peer].fd) < 0) {
				Log("lwip_close: {}", strerror(errno));
				SDL_SetError("lwip_close: %s", strerror(errno));
			}
		}
		peer_list.erase(peer);
	}
}

void protocol_zt::close_all()
{
	if (fd_tcp != -1) {
		lwip_close(fd_tcp);
		fd_tcp = -1;
	}
	if (fd_udp != -1) {
		lwip_close(fd_udp);
		fd_udp = -1;
	}
	for (auto &peer : peer_list) {
		if (peer.second.fd != -1)
			lwip_close(peer.second.fd);
	}
	peer_list.clear();
}

protocol_zt::~protocol_zt()
{
	close_all();
}

void protocol_zt::endpoint::from_string(const std::string &str)
{
	ip_addr_t a;
	if (ipaddr_aton(str.c_str(), &a) == 0)
		return;
	if (!IP_IS_V6_VAL(a))
		return;
	const auto *r = reinterpret_cast<const unsigned char *>(a.u_addr.ip6.addr);
	std::copy(r, r + 16, addr.begin());
}

uint64_t protocol_zt::current_ms()
{
	return 0;
}

bool protocol_zt::is_peer_connected(endpoint &peer)
{
	return peer_list.count(peer) != 0 && peer_list[peer].fd != -1;
}

std::string protocol_zt::make_default_gamename()
{
	std::string ret;
	std::string allowedChars = "abcdefghkopqrstuvwxyz";
	std::random_device rd;
	std::uniform_int_distribution<int> dist(0, allowedChars.size() - 1);
	for (int i = 0; i < 5; ++i) {
		ret += allowedChars.at(dist(rd));
	}
	return ret;
}

} // namespace net
} // namespace devilution

#include "dvlnet/tcp_server.h"

#include <chrono>
#include <functional>
#include <memory>
#include <utility>

#include "dvlnet/base.h"
#include "utils/log.hpp"

namespace devilution {
namespace net {

static std::chrono::milliseconds make_timeout_duration(int seconds)
{
	auto duration = std::chrono::seconds(seconds);
	return std::chrono::milliseconds(duration);
}

tcp_server::tcp_server(unsigned short port, std::string pw)
    : pktfty(std::move(pw))
{
	IPaddress ip;
	if (SDLNet_ResolveHost(&ip, nullptr, port) == -1) {
		auto error = SDLNet_GetError();
		throw std::runtime_error(error);
	}
	socket = SDLNet_TCP_Open(&ip);
	if (!socket) {
		auto error = SDLNet_GetError();
		throw std::runtime_error(error);
	}
}

void tcp_server::poll()
{
	accept();
	recv();
	timeout();
}

bool tcp_server::is_pending_connection_ready()
{
	const auto totalPending = pendingConnections.size();
	const auto pendingSocketSet = SDLNet_AllocSocketSet(totalPending);
	for (auto &connection : pendingConnections)
		SDLNet_TCP_AddSocket(pendingSocketSet, connection->socket);

	const auto count = SDLNet_CheckSockets(pendingSocketSet, 0);
	SDLNet_FreeSocketSet(pendingSocketSet);
	return count > 0;
}

void tcp_server::accept()
{
	while (true) {
		auto clientSocket = SDLNet_TCP_Accept(socket);
		if (!clientSocket) {
			break;
		}
		if (next_free() == PLR_BROADCAST) {
			SDLNet_TCP_Close(clientSocket);
			break;
		}

		auto connection = make_connection(clientSocket);
		connection->timeout = make_timeout_duration(timeout_connect);
		pendingConnections.push_back(connection);
	}

	while (true) {
		if (pendingConnections.empty())
			return;

		if (!is_pending_connection_ready())
			return;

		auto iter = pendingConnections.begin();
		while (iter != pendingConnections.end()) {
			auto &connection = *iter;
			if (!SDLNet_SocketReady(connection->socket)) {
				++iter;
				continue;
			}

			auto buffer = recv_buffer.data();
			const auto maxLength = frame_queue::max_frame_size;
			const auto bytesRead = SDLNet_TCP_Recv(connection->socket, buffer, maxLength);
			if (bytesRead <= 0) {
				drop_connection(connection);
				iter = pendingConnections.erase(iter);
				continue;
			}

			recv_buffer.resize(bytesRead);
			connection->recv_queue.write(std::move(recv_buffer));
			recv_buffer.resize(frame_queue::max_frame_size);
			if (connection->recv_queue.packet_ready()) {
				try {
					auto pkt = pktfty.make_packet(connection->recv_queue.read_packet());
					connection->timeout = make_timeout_duration(timeout_active);
					handle_recv_newplr(connection, *pkt);
				} catch (dvlnet_exception &e) {
					Log("Network error: {}", e.what());
					drop_connection(connection);
				}

				iter = pendingConnections.erase(iter);
				if (next_free() == PLR_BROADCAST) {
					for (auto &connection : pendingConnections)
						drop_connection(connection);

					pendingConnections.clear();
					return;
				}
				continue;
			}

			++iter;
		}
	}
}

void tcp_server::recv()
{
	while (SDLNet_CheckSockets(socketSet, 0) > 0) {
		for (plr_t i = 0; i < MAX_PLRS; ++i) {
			auto &connection = connections[i];
			if (!connection || !SDLNet_SocketReady(connection->socket)) {
				continue;
			}

			auto buffer = recv_buffer.data();
			auto maxLength = frame_queue::max_frame_size;
			auto bytesRead = SDLNet_TCP_Recv(connection->socket, buffer, maxLength);
			if (bytesRead <= 0) {
				drop_connection(connection);
				continue;
			}

			recv_buffer.resize(bytesRead);
			connection->recv_queue.write(std::move(recv_buffer));
			recv_buffer.resize(frame_queue::max_frame_size);
			while (connection->recv_queue.packet_ready()) {
				try {
					auto pkt = pktfty.make_packet(connection->recv_queue.read_packet());
					connection->timeout = make_timeout_duration(timeout_active);
					handle_recv_packet(*pkt);
				} catch (dvlnet_exception &e) {
					Log("Network error: {}", e.what());
					drop_connection(connection);
					continue;
				}
			}
		}
	}
}

void tcp_server::timeout()
{
	auto ticks = SDL_GetTicks();
	auto diff = ticks - lastTicks;

	auto iter = pendingConnections.begin();
	while (iter != pendingConnections.end()) {
		auto &connection = *iter;
		connection->timeout -= std::chrono::milliseconds(diff);
		if (connection->timeout <= std::chrono::milliseconds::zero()) {
			drop_connection(connection);
			iter = pendingConnections.erase(iter);
			continue;
		}
		++iter;
	}

	for (plr_t plr = 0; plr < MAX_PLRS; ++plr) {
		auto &connection = connections[plr];
		if (!connection)
			continue;
		connection->timeout -= std::chrono::milliseconds(diff);
		if (connection->timeout <= std::chrono::milliseconds::zero())
			drop_connection(connection);
	}

	lastTicks = ticks;
}

tcp_server::scc tcp_server::make_connection(TCPsocket socket)
{
	return std::make_shared<client_connection>(socket);
}

plr_t tcp_server::next_free()
{
	for (plr_t i = 0; i < MAX_PLRS; ++i)
		if (!connections[i])
			return i;
	return PLR_BROADCAST;
}

bool tcp_server::empty()
{
	for (plr_t i = 0; i < MAX_PLRS; ++i)
		if (connections[i])
			return false;
	return true;
}

void tcp_server::send_connect(const scc &con)
{
	auto pkt = pktfty.make_packet<PT_CONNECT>(PLR_MASTER, PLR_BROADCAST,
	    con->plr);
	send_packet(*pkt);
}

void tcp_server::handle_recv_newplr(const scc &con, packet &pkt)
{
	auto newplr = next_free();
	if (newplr == PLR_BROADCAST)
		throw server_exception();
	if (empty())
		game_init_info = pkt.info();
	auto reply = pktfty.make_packet<PT_JOIN_ACCEPT>(PLR_MASTER, PLR_BROADCAST,
	    pkt.cookie(), newplr,
	    game_init_info);
	send(con, *reply);
	con->plr = newplr;
	SDLNet_TCP_AddSocket(socketSet, con->socket);
	connections[newplr] = con;
	con->timeout = make_timeout_duration(timeout_active);
	send_connect(con);
}

void tcp_server::handle_recv_packet(packet &pkt)
{
	send_packet(pkt);
}

void tcp_server::send_packet(packet &pkt)
{
	if (pkt.dest() == PLR_BROADCAST) {
		for (auto i = 0; i < MAX_PLRS; ++i)
			if (i != pkt.src() && connections[i])
				send(connections[i], pkt);
	} else {
		if (pkt.dest() >= MAX_PLRS)
			throw server_exception();
		if ((pkt.dest() != pkt.src()) && connections[pkt.dest()])
			send(connections[pkt.dest()], pkt);
	}
}

void tcp_server::send(const scc &con, packet &pkt)
{
	const auto *frame = new buffer_t(frame_queue::make_frame(pkt.data()));
	const auto *buffer = frame->data();
	const auto length = frame->size();
	SDLNet_TCP_Send(con->socket, buffer, length);
	delete frame;
}

void tcp_server::drop_connection(const scc &con)
{
	auto socket = con->socket;
	if (con->plr != PLR_BROADCAST) {
		auto pkt = pktfty.make_packet<PT_DISCONNECT>(PLR_MASTER, PLR_BROADCAST,
		    con->plr, LEAVE_DROP);
		connections[con->plr] = nullptr;
		send_packet(*pkt);
		SDLNet_TCP_DelSocket(socketSet, socket);
		// TODO: investigate if it is really ok for the server to
		//       drop a client directly.
	}
	SDLNet_TCP_Close(socket);
}

void tcp_server::close()
{
	for (plr_t plr = 0; plr < MAX_PLRS; ++plr) {
		auto &connection = connections[plr];
		if (connection)
			drop_connection(connection);
	}
	SDLNet_FreeSocketSet(socketSet);
	SDLNet_TCP_Close(socket);
}

tcp_server::~tcp_server()
{
}

} // namespace net
} // namespace devilution

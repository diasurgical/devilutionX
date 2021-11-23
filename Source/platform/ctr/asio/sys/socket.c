#include <errno.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <sys/uio.h>

ssize_t stream_recvmsg(int socket, struct msghdr *message, int flags)
{
	struct iovec *next = message->msg_iov;
	int iovcount = message->msg_iovlen;

	ssize_t total = 0;
	for (int i = 0; i < iovcount; ++i, ++next) {
		struct iovec *iov = next;
		void *base = iov->iov_base;
		size_t length = iov->iov_len;

		while (length > 0) {
			ssize_t bytesReceived = recv(socket, base, length, flags);
			if (bytesReceived == -1) {
				if (total > 0 && errno == EAGAIN)
					return total;
				if (total > 0 && errno == EWOULDBLOCK)
					return total;
				return -1;
			}
			base += bytesReceived;
			length -= bytesReceived;
			total += bytesReceived;

			if ((flags & MSG_WAITALL) == 0)
				return total;
		}
	}
	return total;
}

ssize_t dgram_recvmsg(int socket, struct msghdr *message, int flags)
{
	return ENOTSUP;
}

ssize_t recvmsg(int socket, struct msghdr *message, int flags)
{
	int type;
	socklen_t length = sizeof(int);
	if (getsockopt(socket, SOL_SOCKET, SO_TYPE, &type, &length) == -1)
		return -1;

	if (type == SOCK_STREAM)
		return stream_recvmsg(socket, message, flags);
	if (type == SOCK_DGRAM)
		return dgram_recvmsg(socket, message, flags);

	errno = ENOTSOCK;
	return -1;
}

ssize_t stream_sendmsg(int socket, const struct msghdr *message, int flags)
{
	struct iovec *next = message->msg_iov;
	int iovcount = message->msg_iovlen;

	ssize_t total = 0;
	for (int i = 0; i < iovcount; ++i, ++next) {
		struct iovec *iov = next;
		void *base = iov->iov_base;
		size_t length = iov->iov_len;
		if (length == 0)
			continue;

		ssize_t bytesSent = send(socket, base, length, flags);
		if (bytesSent == -1) {
			if (total > 0 && errno == EAGAIN)
				return total;
			if (total > 0 && errno == EWOULDBLOCK)
				return total;
			return -1;
		}
		total += bytesSent;
	}
	return total;
}

ssize_t dgram_sendmsg(int socket, const struct msghdr *message, int flags)
{
	return ENOTSUP;
}

ssize_t sendmsg(int socket, const struct msghdr *message, int flags)
{
	int type;
	socklen_t length = sizeof(int);
	if (getsockopt(socket, SOL_SOCKET, SO_TYPE, &type, &length) == -1)
		return -1;

	if (type == SOCK_STREAM)
		return stream_sendmsg(socket, message, flags);
	if (type == SOCK_DGRAM)
		return dgram_sendmsg(socket, message, flags);

	errno = ENOTSOCK;
	return -1;
}

int socketpair(int domain, int type, int protocol, int socket_vector[2])
{
	return ENOTSUP;
}

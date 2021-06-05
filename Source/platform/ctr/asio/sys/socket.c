#include <sys/socket.h>
#include <sys/types.h>
#include <errno.h>

ssize_t recvmsg(int socket, struct msghdr *message, int flags)
{
	return ENOTSUP;
}

ssize_t sendmsg(int socket, const struct msghdr *message, int flags)
{
	return ENOTSUP;
}

int socketpair(int domain, int type, int protocol, int socket_vector[2])
{
	return ENOTSUP;
}

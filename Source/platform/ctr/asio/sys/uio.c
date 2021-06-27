#include <sys/uio.h>
#include <sys/types.h>
#include <errno.h>

ssize_t readv(int __fd, const struct iovec *__iovec, int __count)
{
	return ENOTSUP;
}

ssize_t writev(int __fd, const struct iovec *__iovec, int __count)
{
	return ENOTSUP;
}

#ifndef _SYS_UIO_H
#define _SYS_UIO_H 1

#include <sys/types.h>
#include <sys/_iovec.h>

ssize_t readv(int __fd, const struct iovec *__iovec, int __count);
ssize_t writev(int __fd, const struct iovec *__iovec, int __count);

#endif

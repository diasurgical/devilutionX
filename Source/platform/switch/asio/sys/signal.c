#include <errno.h>
#include <sys/signal.h>

int pthread_sigmask(int, const sigset_t *, sigset_t *)
{
	return ENOTSUP;
}

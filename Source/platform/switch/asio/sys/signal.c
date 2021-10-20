#include <sys/signal.h>
#include <errno.h>

int pthread_sigmask(int, const sigset_t *, sigset_t *)
{
	return ENOTSUP;
}

#include <errno.h>

int pause(void)
{
	errno = ENOSYS;
	return -1;
}
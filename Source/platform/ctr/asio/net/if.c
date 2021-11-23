#include <errno.h>
#include <net/if.h>
#include <stddef.h>

unsigned int if_nametoindex(const char *__ifname)
{
	return ENOTSUP;
}

char *if_indextoname(unsigned int __ifindex, char *__ifname)
{
	return NULL;
}

struct if_nameindex *if_nameindex()
{
	return NULL;
}

void if_freenameindex(struct if_nameindex *__ptr)
{
}

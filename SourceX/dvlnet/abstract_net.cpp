#include "dvlnet/abstract_net.h"

#include "stubs.h"
#ifndef NONET
#include "dvlnet/cdwrap.h"
#include "dvlnet/tcp_client.h"
#include "dvlnet/udp_p2p.h"
#endif
#include "dvlnet/loopback.h"

namespace dvl {
namespace net {

abstract_net* g_pConn = NULL;

abstract_net* abstract_net::make_net(provider_t provider)
{
#ifdef NONET
	g_pConn = new loopback;
	return g_pConn;
#else
	switch (provider) {
	case SELCONN_TCP:
		g_pConn = new cdwrap<tcp_client>;
		return g_pConn;
#ifdef BUGGY
	case SELCONN_UDP:
		g_pConn = new cdwrap<udp_p2p>
		return g_pConn;
#endif
	case SELCONN_LOOPBACK:
		g_pConn = new loopback;
		return g_pConn;
	default:
		ABORT();
	}
#endif
}

abstract_net::~abstract_net()
{
	if(g_pConn)
	{
		delete g_pConn;
		g_pConn = NULL;
	}
}

} // namespace net
} // namespace dvl

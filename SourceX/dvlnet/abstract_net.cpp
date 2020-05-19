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

abstract_net* abstract_net::make_net(provider_t provider)
{
#ifdef NONET
	return new loopback;
#else
	switch (provider) {
	case SELCONN_TCP:
		return new cdwrap<tcp_client>;
#ifdef BUGGY
	case SELCONN_UDP:
		return new cdwrap<udp_p2p>;
#endif
	case SELCONN_LOOPBACK:
		return new loopback;
	default:
		ABORT();
	}
#endif
}

abstract_net::~abstract_net()
{
#if defined(NONET) && !defined(_XBOX)
	delete loopback;
#endif
}

} // namespace net
} // namespace dvl

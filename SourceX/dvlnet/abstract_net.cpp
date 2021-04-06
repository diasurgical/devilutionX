#include "dvlnet/abstract_net.h"

#include "stubs.h"
#ifndef NONET
#include "dvlnet/cdwrap.h"
#include "dvlnet/tcp_client.h"
#include "dvlnet/udp_p2p.h"
#endif
#include "dvlnet/loopback.h"

namespace devilution {
namespace net {

std::unique_ptr<abstract_net> abstract_net::make_net(provider_t provider)
{
#ifdef NONET
	return std::unique_ptr<abstract_net>(new loopback);
#else
	switch (provider) {
	case SELCONN_TCP:
		return std::unique_ptr<abstract_net>(new cdwrap<tcp_client>);
#ifdef BUGGY
	case SELCONN_UDP:
		return std::unique_ptr<abstract_net>(new cdwrap<udp_p2p>);
#endif
	case SELCONN_LOOPBACK:
		return std::unique_ptr<abstract_net>(new loopback);
	default:
		ABORT();
	}
#endif
}

} // namespace net
} // namespace devilution

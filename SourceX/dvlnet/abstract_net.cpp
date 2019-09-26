#include "dvlnet/abstract_net.h"

#include "stubs.h"
#ifndef NONET
#include "dvlnet/tcp_client.h"
#include "dvlnet/udp_p2p.h"
#endif
#include "dvlnet/loopback.h"

namespace dvl {
namespace net {

abstract_net::~abstract_net()
{
}

std::unique_ptr<abstract_net> abstract_net::make_net(provider_t provider)
{
#ifdef NONET
	return std::make_unique<loopback>();
#else
	if (provider == 'TCPN') {
		return std::unique_ptr<abstract_net>(new tcp_client);
	} else if (provider == 'UDPN') {
		return std::unique_ptr<abstract_net>(new udp_p2p);
	} else if (provider == 'SCBL' || provider == 0) {
		return std::unique_ptr<abstract_net>(new loopback);
	} else {
		ABORT();
	}
#endif
}

} // namespace net
} // namespace dvl

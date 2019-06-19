#include "dvlnet/abstract_net.h"

#include "stubs.h"
#ifndef SWITCH
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
#ifdef SWITCH
	return std::make_unique<loopback>();
#else
	if (provider == 'TCPN') {
		return std::make_unique<tcp_client>();
	} else if (provider == 'UDPN') {
		return std::make_unique<udp_p2p>();
	} else if (provider == 'SCBL' || provider == 0) {
		return std::make_unique<loopback>();
	} else {
		ABORT();
	}
#endif
}

}  // namespace net
}  // namespace dvl

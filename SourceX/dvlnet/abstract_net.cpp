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
	//TODO: No networking on Xbox atm
	default:
		ABORT();
	}
#endif
}

abstract_net::~abstract_net()
{
#if 0//def NONET
	delete loopback;
#endif
}

} // namespace net
} // namespace dvl

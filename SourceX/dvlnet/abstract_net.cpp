#include "dvlnet/abstract_net.h"

#include "stubs.h"
#ifndef NONET
#include "dvlnet/cdwrap.h"
#include "dvlnet/tcp_client.h"
#include "dvlnet/base_protocol.h"
#include "dvlnet/protocol_zt.h"
#endif
#include "dvlnet/loopback.h"

namespace dvl {
namespace net {

std::unique_ptr<abstract_net> abstract_net::make_net(provider_t provider)
{
#ifdef NONET
	return std::unique_ptr<abstract_net>(new loopback);
#else
	switch (provider) {
	case SELCONN_TCP:
		return std::unique_ptr<abstract_net>(new cdwrap<tcp_client>);
	case SELCONN_ZT:
		return std::unique_ptr<abstract_net>(new cdwrap<base_protocol<protocol_zt>>);
	case SELCONN_LOOPBACK:
		return std::unique_ptr<abstract_net>(new loopback);
	default:
		ABORT();
	}
#endif
}

} // namespace net
} // namespace dvl

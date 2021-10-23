#include "dvlnet/abstract_net.h"

#include "utils/stubs.h"
#ifndef NONET
#include "dvlnet/base_protocol.h"
#include "dvlnet/cdwrap.h"
#ifndef DISABLE_ZERO_TIER
#include "dvlnet/protocol_zt.h"
#endif
#ifndef DISABLE_TCP
#include "dvlnet/tcp_client.h"
#endif
#endif
#include "dvlnet/loopback.h"

namespace devilution {
namespace net {

std::unique_ptr<abstract_net> abstract_net::MakeNet(provider_t provider)
{
#ifdef NONET
	return std::make_unique<loopback>();
#else
	switch (provider) {
#ifndef DISABLE_TCP
	case SELCONN_TCP:
		return std::make_unique<cdwrap<tcp_client>>();
#endif
#ifndef DISABLE_ZERO_TIER
	case SELCONN_ZT:
		return std::make_unique<cdwrap<base_protocol<protocol_zt>>>();
#endif
	case SELCONN_LOOPBACK:
		return std::make_unique<loopback>();
	default:
		ABORT();
	}
#endif
}

} // namespace net
} // namespace devilution

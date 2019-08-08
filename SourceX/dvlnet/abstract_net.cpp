#include "dvlnet/abstract_net.h"

#include "stubs.h"
#include "dvlnet/loopback.h"

namespace dvl {
namespace net {

abstract_net::~abstract_net()
{
}

std::unique_ptr<abstract_net> abstract_net::make_net(provider_t provider)
{
	return std::make_unique<loopback>();
}

}  // namespace net
}  // namespace dvl

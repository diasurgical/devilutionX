#include "dvlnet/zerotier_native.h"

#include <SDL.h>
#include <atomic>

#ifdef USE_SDL1
#include "utils/sdl2_to_1_2_backports.h"
#else
#include "utils/sdl2_backports.h"
#endif

#include <ZeroTierSockets.h>
#include <cstdlib>

#include "utils/log.hpp"
#include "utils/paths.h"

#include "dvlnet/zerotier_lwip.h"

namespace devilution {
namespace net {

// static constexpr uint64_t zt_earth = 0x8056c2e21c000001;
static constexpr uint64_t ZtNetwork = 0xa84ac5c10a7ebb5f;

static std::atomic_bool zt_network_ready(false);
static std::atomic_bool zt_node_online(false);
static std::atomic_bool zt_joined(false);

static void Callback(struct zts_callback_msg *msg)
{
	// printf("callback %i\n", msg->eventCode);
	if (msg->eventCode == ZTS_EVENT_NODE_ONLINE) {
		Log("ZeroTier: ZTS_EVENT_NODE_ONLINE, nodeId={:x}", (unsigned long long)msg->node->address);
		zt_node_online = true;
		if (!zt_joined) {
			zts_join(ZtNetwork);
			zt_joined = true;
		}
	} else if (msg->eventCode == ZTS_EVENT_NODE_OFFLINE) {
		Log("ZeroTier: ZTS_EVENT_NODE_OFFLINE");
		zt_node_online = false;
	} else if (msg->eventCode == ZTS_EVENT_NETWORK_READY_IP6) {
		Log("ZeroTier: ZTS_EVENT_NETWORK_READY_IP6, networkId={:x}", (unsigned long long)msg->network->nwid);
		zt_ip6setup();
		zt_network_ready = true;
	} else if (msg->eventCode == ZTS_EVENT_ADDR_ADDED_IP6) {
		print_ip6_addr(&(msg->addr->addr));
	}
}

bool zerotier_network_ready()
{
	return zt_network_ready && zt_node_online;
}

void zerotier_network_start()
{
	std::string ztpath = paths::ConfigPath() + "zerotier";
	zts_start(ztpath.c_str(), (void (*)(void *))Callback, 0);
}

} // namespace net
} // namespace devilution

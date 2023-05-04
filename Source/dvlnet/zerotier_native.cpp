#include "dvlnet/zerotier_native.h"

#include <SDL.h>
#include <atomic>

#ifdef USE_SDL1
#include "utils/sdl2_to_1_2_backports.h"
#else
#include "utils/sdl2_backports.h"
#endif

#if (defined(_WIN64) || defined(_WIN32)) && !defined(NXDK)
#include "utils/stdcompat/filesystem.hpp"
#ifdef DVL_HAS_FILESYSTEM
#define DVL_ZT_SYMLINK
#endif
#endif

#ifdef DVL_ZT_SYMLINK
#include <shlobj.h>
#include <sodium.h>

#include "utils/str_cat.hpp"
#include "utils/utf8.hpp"
#endif

#include <ZeroTierSockets.h>
#include <cstdlib>

#include "utils/log.hpp"
#include "utils/paths.h"

#include "dvlnet/zerotier_lwip.h"

namespace devilution {
namespace net {

namespace {

#ifdef DVL_ZT_SYMLINK
bool HasMultiByteChars(string_view path)
{
	return std::any_of(path.begin(), path.end(), IsTrailUtf8CodeUnit);
}

std::string ComputeAlternateFolderName(string_view path)
{
	const size_t hashSize = crypto_generichash_BYTES;
	unsigned char hash[hashSize];

	const int status = crypto_generichash(hash, hashSize,
	    reinterpret_cast<const unsigned char *>(path.data()), path.size(),
	    nullptr, 0);

	if (status != 0)
		return "";

	return fmt::format("{:02x}", fmt::join(hash, ""));
}

std::string ToZTCompliantPath(string_view configPath)
{
	if (!HasMultiByteChars(configPath))
		return std::string(configPath);

	char commonAppDataPath[MAX_PATH];
	if (!SUCCEEDED(SHGetFolderPathA(NULL, CSIDL_COMMON_APPDATA, NULL, 0, commonAppDataPath))) {
		LogVerbose("Failed to retrieve common application data path");
		return std::string(configPath);
	}

	std::error_code err;
	std::string alternateConfigPath = StrCat(commonAppDataPath, "\\diasurgical\\devilution");
	std::filesystem::create_directories(alternateConfigPath, err);
	if (err) {
		LogVerbose("Failed to create directories in ZT-compliant config path");
		return std::string(configPath);
	}

	std::string alternateFolderName = ComputeAlternateFolderName(configPath);
	if (alternateFolderName == "") {
		LogVerbose("Failed to hash config path for ZT");
		return std::string(configPath);
	}

	std::string symlinkPath = StrCat(alternateConfigPath, "\\", alternateFolderName);
	bool symlinkExists = std::filesystem::exists(std::filesystem::u8path(symlinkPath), err);
	if (err) {
		LogVerbose("Failed to determine if symlink for ZT-compliant config path exists");
		return std::string(configPath);
	}

	if (!symlinkExists) {
		std::filesystem::create_directory_symlink(
		    std::filesystem::u8path(configPath),
		    std::filesystem::u8path(symlinkPath),
		    err);

		if (err) {
			LogVerbose("Failed to create symlink for ZT-compliant config path");
			return std::string(configPath);
		}
	}

	return StrCat(symlinkPath, "\\");
}
#endif

} // namespace

// static constexpr uint64_t zt_earth = 0x8056c2e21c000001;
static constexpr uint64_t ZtNetwork = 0xa84ac5c10a7ebb5f;

static std::atomic_bool zt_network_ready(false);
static std::atomic_bool zt_node_online(false);
static std::atomic_bool zt_joined(false);

static void Callback(void *ptr)
{
	zts_event_msg_t *msg = reinterpret_cast<zts_event_msg_t *>(ptr);
	// printf("callback %i\n", msg->eventCode);
	if (msg->event_code == ZTS_EVENT_NODE_ONLINE) {
		Log("ZeroTier: ZTS_EVENT_NODE_ONLINE, nodeId={:x}", (unsigned long long)msg->node->node_id);
		zt_node_online = true;
		if (!zt_joined) {
			zts_net_join(ZtNetwork);
			zt_joined = true;
		}
	} else if (msg->event_code == ZTS_EVENT_NODE_OFFLINE) {
		Log("ZeroTier: ZTS_EVENT_NODE_OFFLINE");
		zt_node_online = false;
	} else if (msg->event_code == ZTS_EVENT_NETWORK_READY_IP6) {
		Log("ZeroTier: ZTS_EVENT_NETWORK_READY_IP6, networkId={:x}", (unsigned long long)msg->network->net_id);
		zt_ip6setup();
		zt_network_ready = true;
	} else if (msg->event_code == ZTS_EVENT_ADDR_ADDED_IP6) {
		print_ip6_addr(&(msg->addr->addr));
	}
}

bool zerotier_network_ready()
{
	return zt_network_ready && zt_node_online;
}

void zerotier_network_start()
{
	std::string configPath = paths::ConfigPath();
#ifdef DVL_ZT_SYMLINK
	configPath = ToZTCompliantPath(configPath);
#endif
	std::string ztpath = configPath + "zerotier";
	zts_init_from_storage(ztpath.c_str());
	zts_init_set_event_handler(&Callback);
	zts_node_start();
}

} // namespace net
} // namespace devilution

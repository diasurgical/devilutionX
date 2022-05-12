#include "platform/switch/romfs.hpp"

#include <cstdlib>

extern "C" {
#include <switch/runtime/devices/romfs_dev.h>
}

void switch_romfs_init()
{
	Result res = romfsInit();
	if (R_SUCCEEDED(res))
		atexit([]() { romfsExit(); });
}
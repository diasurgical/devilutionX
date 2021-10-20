#include <cstdlib>
#include <cstdio>
#include <unistd.h>
#include "platform/vita/network.h"
#include <psp2/sysmodule.h>
#include <psp2/net/net.h>
#include <psp2/net/netctl.h>

void vita_enable_network()
{
	SceNetInitParam param;
	static char memory[64 * 1024];
	int ret;

	ret = sceSysmoduleLoadModule(SCE_SYSMODULE_NET);
	if (ret < 0) {
		return;
	}

	param.memory = memory;
	param.size = sizeof(memory);
	param.flags = 0;
	ret = sceNetInit(&param);
	if (ret < 0) {
		return;
	}

	ret = sceNetCtlInit();
	if (ret < 0) {
		return;
	}
}

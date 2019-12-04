#include <stdlib.h>
#include <3ds.h>
#include "platform/ctr/system.h"

void ctr_sys_init()
{
	bool isN3DS;

	APT_CheckNew3DS(&isN3DS);
	if(isN3DS)
		osSetSpeedupEnable(true);

	romfsInit();
	atexit([]() { romfsExit(); });
}
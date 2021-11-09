#include <cstdlib>
#include <cstdio>
#include <3ds.h>
#include "platform/ctr/system.h"
#include "platform/ctr/random.hpp"
#include "platform/ctr/sockets.hpp"

using namespace devilution;

bool isN3DS;

bool ctr_check_dsp()
{
	FILE *dsp = fopen("sdmc:/3ds/dspfirm.cdc", "r");
	if (dsp == NULL) {
		gfxInitDefault();
		errorConf error;
		errorInit(&error, ERROR_TEXT, CFG_LANGUAGE_EN);
		errorText(&error, "Cannot find DSP firmware!\n\n\"sdmc:/3ds/dspfirm.cdc\"\n\nRun \'DSP1\' atleast once to\ndump your DSP firmware.");
		errorDisp(&error);
		gfxExit();
		return false;
	}
	fclose(dsp);
	return true;
}

void ctr_sys_init()
{
	if (ctr_check_dsp() == false)
		exit(0);

	APT_CheckNew3DS(&isN3DS);
	if (isN3DS)
		osSetSpeedupEnable(true);

	romfsInit();
	atexit([]() { romfsExit(); });

	acInit();
	atexit([]() { acExit(); });

	n3ds_socInit();
	atexit([]() { n3ds_socExit(); });

	randombytes_ctrrandom_init();
	atexit([]() {
		if (psGetSessionHandle())
			psExit();
	});
}

#include <stdlib.h>
#include <stdio.h>
#include <3ds.h>
#include "platform/ctr/system.h"

bool isN3DS;

bool ctr_check_dsp()
{
	FILE *dsp = fopen("sdmc:/3ds/dspfirm.cdc", "r");
	if (dsp == NULL) {
		fclose(dsp);
		errorConf error;
		errorInit(&error, ERROR_TEXT, CFG_LANGUAGE_EN);
		errorText(&error, "Cannot find DSP firmware!\n\n\"sdmc:/3ds/dspfirm.cdc\"\n\nRun \'DSP1\' atleast once to\ndump your DSP firmware.");
		errorDisp(&error);
		return false;
	}
	fclose(dsp);
	return true;
}

void ctr_sys_init()
{
	if( ctr_check_dsp() == false )
		exit(0);

	APT_CheckNew3DS(&isN3DS);
	if(isN3DS)
		osSetSpeedupEnable(true);

	romfsInit();
	atexit([]() { romfsExit(); });
}
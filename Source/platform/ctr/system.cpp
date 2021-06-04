#include <cstdlib>
#include <cstdio>
#include <3ds.h>
#include "platform/ctr/system.h"
#include "platform/ctr/random.hpp"
#include "platform/ctr/sockets.hpp"

bool isN3DS;

aptHookCookie cookie;

void aptHookFunc(APT_HookType hookType, void *param)
{
	switch (hookType) {
	case APTHOOK_ONSUSPEND:
		ctr_lcd_backlight_on();
		break;
	case APTHOOK_ONSLEEP:
		break;
	case APTHOOK_ONRESTORE:
		ctr_lcd_backlight_off();
		break;
	case APTHOOK_ONWAKEUP:
		ctr_lcd_backlight_off();
		break;
	case APTHOOK_ONEXIT:
		ctr_lcd_backlight_on();
		break;
	default:
		break;
	}
}

void ctr_lcd_backlight_on()
{
	gspLcdInit();
	GSPLCD_PowerOnBacklight(GSPLCD_SCREEN_BOTTOM);
	gspLcdExit();
}

void ctr_lcd_backlight_off()
{
	gspLcdInit();
	GSPLCD_PowerOffBacklight(GSPLCD_SCREEN_BOTTOM);
	gspLcdExit();
}

bool ctr_check_dsp()
{
	FILE *dsp = fopen("sdmc:/3ds/dspfirm.cdc", "r");
	if (dsp == NULL) {
		fclose(dsp);
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

	aptHook(&cookie, aptHookFunc, NULL);

	APT_CheckNew3DS(&isN3DS);
	if (isN3DS)
		osSetSpeedupEnable(true);

	ctr_lcd_backlight_off();
	atexit([]() { ctr_lcd_backlight_on(); });

	romfsInit();
	atexit([]() { romfsExit(); });

	randombytes_ctrrandom_init();
	atexit([]() {
		if (psGetSessionHandle())
			psExit();
	});
}

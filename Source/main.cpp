#include <SDL.h>
#ifdef __SWITCH__
#include "platform/switch/network.h"
#endif
#ifdef __3DS__
#include "platform/ctr/system.h"
#endif
#ifdef RUN_TESTS
#include <gtest/gtest.h>
#endif

#include "diablo.h"

#if !defined(__APPLE__)
extern "C" const char *__asan_default_options()
{
	return "halt_on_error=0";
}
#endif

#ifdef __PSP__
#include <pspkernel.h>

PSP_MAIN_THREAD_ATTR(THREAD_ATTR_USER | THREAD_ATTR_VFPU);
PSP_HEAP_SIZE_MAX();

int exit_callback(int arg1, int arg2, void* common){
	sceKernelExitGame();
	return 0;
}
 
int CallbackThread(SceSize args, void* argp) {
	int cbid = sceKernelCreateCallback("Exit Callback", exit_callback, NULL);
	sceKernelRegisterExitCallback(cbid);
	sceKernelSleepThreadCB();
 
	return 0;
}
 
int SetupCallbacks() {
	int thid = sceKernelCreateThread("update_thread", CallbackThread, 0x11, 0xFA0, 0, 0);
	if (thid >= 0) {
		sceKernelStartThread(thid, 0, 0);
	}
	return thid;
}

extern "C"
{
int SDL_main(int argc, char *argv[])
#else
int main(int argc, char **argv)
#endif
{
#ifdef RUN_TESTS
	testing::InitGoogleTest(&argc, argv);
	return RUN_ALL_TESTS();
#endif
#ifdef __SWITCH__
	switch_enable_network();
#endif
#ifdef __3DS__
	ctr_sys_init();
#endif
#ifdef __PSP__
	SetupCallbacks();

#endif

	return devilution::DiabloMain(argc, argv);
}

#if __PSP__
}
#endif

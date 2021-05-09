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
#include <pspdebug.h>

PSP_MODULE_INFO("GETREKT", 0, 1, 1);
PSP_MAIN_THREAD_ATTR(THREAD_ATTR_USER | THREAD_ATTR_VFPU);
PSP_HEAP_SIZE_MAX();

extern "C" int main(int argc, char** argv)
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

	return devilution::DiabloMain(argc, argv);
}

#include <SDL.h>
#ifdef __SWITCH__
#include "platform/switch/network.h"
#include "platform/switch/random.hpp"
#endif
#ifdef __3DS__
#include "platform/ctr/system.h"
#endif
#ifdef GPERF_HEAP_MAIN
#include <gperftools/heap-profiler.h>
#endif

#include "diablo.h"

#if !defined(__APPLE__)
extern "C" const char *__asan_default_options()
{
	return "halt_on_error=0";
}
#endif

#ifdef __ANDROID__
int SDL_main(int argc, char **argv)
#else
int main(int argc, char **argv)
#endif
{
#ifdef __SWITCH__
	switch_enable_network();
	randombytes_switchrandom_init();
#endif
#ifdef __3DS__
	ctr_sys_init();
#endif
#ifdef GPERF_HEAP_MAIN
	HeapProfilerStart("main");
#endif
	const int result = devilution::DiabloMain(argc, argv);
#ifdef GPERF_HEAP_MAIN
	HeapProfilerStop();
#endif
	return result;
}

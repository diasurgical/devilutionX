#include <SDL.h>
#ifdef GPERF_HEAP_MAIN
#include <gperftools/heap-profiler.h>
#endif

#include "diablo.h"

extern "C" const char *__asan_default_options() // NOLINT(bugprone-reserved-identifier, readability-identifier-naming)
{
	return "halt_on_error=0";
}

int main(int argc, char **argv)
{
#ifdef GPERF_HEAP_MAIN
	HeapProfilerStart("main");
#endif
	const int result = devilution::DiabloMain(argc, argv);
#ifdef GPERF_HEAP_MAIN
	HeapProfilerStop();
#endif
	return result;
}

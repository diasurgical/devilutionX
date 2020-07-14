#include <SDL.h>
#ifdef __SWITCH__
#include "platform/switch/network.h"
#endif
#ifdef RUN_TESTS
#include <gtest/gtest.h>
#endif

#include "all.h"

#if !defined(__APPLE__)
extern "C" const char *__asan_default_options()
{
	return "halt_on_error=0";
}
#endif

int main(int argc, char **argv)
{
#ifdef RUN_TESTS
    testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
#endif
#ifdef __SWITCH__
	switch_enable_network();
#endif

	return dvl::DiabloMain(argc, argv);
}

#include <gtest/gtest.h>

#include "diablo.h"
#include "main_loop.hpp"
#include "options.h"

int main(int argc, char **argv)
{
	// Disable error dialogs.
	devilution::gbQuietMode = true;
	// Disable hardware cursor while testing.
	devilution::sgOptions.Graphics.hardwareCursor.SetValue(false);

	devilution::SetMainLoopQuitFn([](int status) {
		devilution::diablo_quit(status);
	});

	testing::InitGoogleTest(&argc, argv);
	return RUN_ALL_TESTS();
}

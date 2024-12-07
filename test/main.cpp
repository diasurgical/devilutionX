#include <gtest/gtest.h>

#include "headless_mode.h"
#include "options.h"

int main(int argc, char **argv)
{
	// Disable error dialogs.
	devilution::HeadlessMode = true;

#if SDL_VERSION_ATLEAST(2, 0, 0)
	// Disable hardware cursor while testing.
	devilution::sgOptions.Graphics.hardwareCursor.SetValue(false);
#endif

	testing::InitGoogleTest(&argc, argv);
	return RUN_ALL_TESTS();
}

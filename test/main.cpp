#include <gtest/gtest.h>

#include "diablo.h"
#include "options.h"

int main(int argc, char **argv)
{
	// Disable error dialogs.
	devilution::gbQuietMode = true;
	// Disable hardware cursor while testing.
	devilution::sgOptions.Graphics.hardwareCursor.SetValue(false);

	testing::InitGoogleTest(&argc, argv);
	return RUN_ALL_TESTS();
}

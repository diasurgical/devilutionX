#include <gtest/gtest.h>

#include "diablo.h"

int main(int argc, char **argv)
{
	// Disable error dialogs.
	devilution::gbQuietMode = true;

	testing::InitGoogleTest(&argc, argv);
	return RUN_ALL_TESTS();
}

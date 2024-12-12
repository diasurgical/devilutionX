#include <gtest/gtest.h>

#include "diablo.h"
#include "options.h"
#include "utils/paths.h"

int main(int argc, char **argv)
{
	// Disable error dialogs.
	devilution::HeadlessMode = true;

#if SDL_VERSION_ATLEAST(2, 0, 0)
	// Disable hardware cursor while testing.
	devilution::sgOptions.Graphics.hardwareCursor.SetValue(false);
#endif

#ifdef __APPLE__
	devilution::paths::SetAssetsPath(
	    devilution::paths::BasePath() + "devilutionx.app/Contents/Resources/");
#endif

	testing::InitGoogleTest(&argc, argv);
	return RUN_ALL_TESTS();
}

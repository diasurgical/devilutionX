#include <gtest/gtest.h>

#include "diablo.h"
#include "utils/paths.h"

int main(int argc, char **argv)
{
	// Disable error dialogs.
	devilution::gbQuietMode = true;

	// Let the tests find `devilutionx.mpq` or `assets/`.
	devilution::paths::SetAssetsPath(devilution::paths::BasePath() + "../assets");
	devilution::paths::SetBasePath(devilution::paths::BasePath() + "..");

	testing::InitGoogleTest(&argc, argv);
	return RUN_ALL_TESTS();
}

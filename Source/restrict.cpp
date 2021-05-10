/**
 * @file restrict.cpp
 *
 * Implementation of functionality for checking if the game will be able run on the system.
 */

#include "appfat.h"
#include "utils/file_util.h"
#include "utils/paths.h"

namespace devilution {

/**
 * @brief Check that we have write access to the game install folder

 */
void ReadOnlyTest()
{
	const std::string path = paths::PrefPath() + "Diablo1ReadOnlyTest.foo";
	auto fileStream = CreateFileStream(path.c_str(), std::ios::in | std::ios::out);
	if (fileStream->fail()) {
		DirErrorDlg(paths::PrefPath().c_str());
	}

	RemoveFile(path.c_str());
}

} // namespace devilution

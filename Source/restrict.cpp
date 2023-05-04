/**
 * @file restrict.cpp
 *
 * Implementation of functionality for checking if the game will be able run on the system.
 */

#include "appfat.h"
#include "utils/file_util.h"
#include "utils/paths.h"

#include <SDL.h>

namespace devilution {

void ReadOnlyTest()
{
	const std::string path = paths::PrefPath() + "Diablo1ReadOnlyTest.foo";
	SDL_RWops *file = SDL_RWFromFile(path.c_str(), "w");
	if (file == nullptr) {
		DirErrorDlg(paths::PrefPath());
	}

	SDL_RWclose(file);
	RemoveFile(path.c_str());
}

} // namespace devilution

/**
 * @file appfat.cpp
 *
 * Implementation of error dialogs.
 */

#include <config.h>

#include <fmt/format.h>

#include "diablo.h"
#include "multi.h"
#include "storm/storm_net.hpp"
#include "utils/language.h"
#include "utils/sdl_thread.h"
#include "utils/str_cat.hpp"
#include "utils/ui_fwd.h"

namespace devilution {

namespace {

/** Set to true when a fatal error is encountered and the application should shut down. */
bool Terminating = false;
/** Thread id of the last callee to FreeDlg(). */
SDL_threadID CleanupThreadId;

/**
 * @brief Cleans up after a fatal application error.
 */
void FreeDlg()
{
	if (Terminating && CleanupThreadId != this_sdl_thread::get_id())
		SDL_Delay(20000);

	Terminating = true;
	CleanupThreadId = this_sdl_thread::get_id();

	if (gbIsMultiplayer) {
		if (SNetLeaveGame(3))
			SDL_Delay(2000);
	}

	SNetDestroy();
}

} // namespace

void app_fatal(string_view str)
{
	FreeDlg();
	UiErrorOkDialog(_("Error"), str);
	diablo_quit(1);
}

#ifdef _DEBUG
void assert_fail(int nLineNo, const char *pszFile, const char *pszFail)
{
	app_fatal(StrCat("assertion failed (", pszFile, ":", nLineNo, ")\n", pszFail));
}
#endif

void ErrDlg(const char *title, string_view error, string_view logFilePath, int logLineNr)
{
	FreeDlg();

	std::string text = fmt::format(fmt::runtime(_(/* TRANSLATORS: Error message that displays relevant information for bug report */ "{:s}\n\nThe error occurred at: {:s} line {:d}")), error, logFilePath, logLineNr);

	UiErrorOkDialog(title, text);
	diablo_quit(1);
}

void InsertCDDlg(string_view archiveName)
{
	std::string text = fmt::format(
	    fmt::runtime(_("Unable to open main data archive ({:s}).\n"
	                   "\n"
	                   "Make sure that it is in the game folder.")),
	    archiveName);

	UiErrorOkDialog(_("Data File Error"), text);
	diablo_quit(1);
}

void DirErrorDlg(string_view error)
{
	std::string text = fmt::format(fmt::runtime(_(/* TRANSLATORS: Error when Program is not allowed to write data */ "Unable to write to location:\n{:s}")), error);

	UiErrorOkDialog(_("Read-Only Directory Error"), text);
	diablo_quit(1);
}

} // namespace devilution

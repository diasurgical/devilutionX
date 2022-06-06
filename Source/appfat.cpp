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
#include "utils/ui_fwd.h"

namespace devilution {

namespace {

/** Set to true when a fatal error is encountered and the application should shut down. */
bool Terminating = false;
/** Thread id of the last callee to FreeDlg(). */
SDL_threadID CleanupThreadId;

/**
 * @brief Displays an error message box based on the given format string and variable argument list.
 * @param pszFmt Error message format
 * @param va Additional parameters for message format
 */
void MsgBox(const char *pszFmt, va_list va)
{
	char text[256];

	vsnprintf(text, sizeof(text), pszFmt, va);

	UiErrorOkDialog(_("Error"), text);
}

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

void app_fatal(const char *pszFmt, ...)
{
	va_list va;

	va_start(va, pszFmt);
	FreeDlg();

	if (pszFmt != nullptr)
		MsgBox(pszFmt, va);

	va_end(va);

	diablo_quit(1);
}

#ifdef _DEBUG
void assert_fail(int nLineNo, const char *pszFile, const char *pszFail)
{
	app_fatal("assertion failed (%s:%i)\n%s", pszFile, nLineNo, pszFail);
}
#endif

void ErrDlg(const char *title, string_view error, string_view logFilePath, int logLineNr)
{
	FreeDlg();

	std::string text = fmt::format(fmt::runtime(_(/* TRANSLATORS: Error message that displays relevant information for bug report */ "{:s}\n\nThe error occurred at: {:s} line {:d}")), error, logFilePath, logLineNr);

	UiErrorOkDialog(title, text);
	app_fatal(nullptr);
}

void InsertCDDlg(string_view archiveName)
{
	std::string text = fmt::format(
	    fmt::runtime(_("Unable to open main data archive ({:s}).\n"
	                   "\n"
	                   "Make sure that it is in the game folder.")),
	    archiveName);

	UiErrorOkDialog(_("Data File Error"), text);
	app_fatal(nullptr);
}

void DirErrorDlg(string_view error)
{
	std::string text = fmt::format(fmt::runtime(_(/* TRANSLATORS: Error when Program is not allowed to write data */ "Unable to write to location:\n{:s}")), error);

	UiErrorOkDialog(_("Read-Only Directory Error"), text);
	app_fatal(nullptr);
}

} // namespace devilution

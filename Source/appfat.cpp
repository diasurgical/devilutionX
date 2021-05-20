/**
 * @file appfat.cpp
 *
 * Implementation of error dialogs.
 */

#include <config.h>

#include <fmt/format.h>

#include "diablo.h"
#include "storm/storm.h"
#include "utils/ui_fwd.h"
#include "utils/language.h"

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
	if (Terminating && CleanupThreadId != SDL_GetThreadID(nullptr))
		SDL_Delay(20000);

	Terminating = true;
	CleanupThreadId = SDL_GetThreadID(nullptr);

	if (gbIsMultiplayer) {
		if (SNetLeaveGame(3))
			SDL_Delay(2000);
	}

	SNetDestroy();
}

} // namespace

/**
 * @brief Terminates the game and displays an error message box.
 * @param pszFmt Optional error message.
 * @param ... (see printf)
 */
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

/**
 * @brief Displays a warning message box based on the given formatted error message.
 * @param pszFmt Error message format
 * @param ... Additional parameters for message format
 */
void DrawDlg(const char *pszFmt, ...)
{
	char text[256];
	va_list va;

	va_start(va, pszFmt);
	vsnprintf(text, sizeof(text), pszFmt, va);
	va_end(va);

	UiErrorOkDialog(PROJECT_NAME, text, false);
}

#ifdef _DEBUG
/**
 * @brief Show an error and exit the application.
 * @param nLineNo The line number of the assertion
 * @param pszFile File name where the assertion is located
 * @param pszFail Fail message
 */
void assert_fail(int nLineNo, const char *pszFile, const char *pszFail)
{
	app_fatal("assertion failed (%s:%i)\n%s", pszFile, nLineNo, pszFail);
}
#endif

/**
 * @brief Terminates the game and displays an error dialog box based on the given dialog_id.
 */
void ErrDlg(const char *title, const char *error, const char *logFilePath, int logLineNr)
{
	char text[1024];

	FreeDlg();

	strncpy(text, fmt::format(_(/* TRANSLATORS: Error message that displays relevant information for bug report */ "{:s}\n\nThe error occurred at: {:s} line {:d}"), error, logFilePath, logLineNr).c_str(), sizeof(text));

	UiErrorOkDialog(title, text);
	app_fatal(nullptr);
}

/**
 * @brief Terminates the game with a file not found error dialog.
 */
void FileErrDlg(const char *error)
{
	char text[1024];

	FreeDlg();

	if (error == nullptr)
		error = "";
	snprintf(
	    text,
	    sizeof(text),
	    _(/* TRANSLATORS: Error Message when diabdat.mpq is broken. Keep values unchanged. */ "Unable to open a required file.\n"
	                                                                                          "\n"
	                                                                                          "Verify that the MD5 of diabdat.mpq matches one of the following values\n"
	                                                                                          "011bc6518e6166206231080a4440b373\n"
	                                                                                          "68f049866b44688a7af65ba766bef75a\n"
	                                                                                          "\n"
	                                                                                          "The problem occurred when loading:\n%s"),
	    error);

	UiErrorOkDialog(_("Data File Error"), text);
	app_fatal(nullptr);
}

/**
 * @brief Terminates the game with an insert CD error dialog.
 */
void InsertCDDlg()
{
	char text[1024];

	snprintf(
	    text,
	    sizeof(text),
	    "%s",
	    _("Unable to open main data archive (diabdat.mpq or spawn.mpq).\n"
	      "\n"
	      "Make sure that it is in the game folder."));

	UiErrorOkDialog(_("Data File Error"), text);
	app_fatal(nullptr);
}

/**
 * @brief Terminates the game with a read-only directory error dialog.
 */
void DirErrorDlg(const char *error)
{
	char text[1024];

	strncpy(text, fmt::format(_(/* TRANSLATORS: Error when Program is not allowed to write data */ "Unable to write to location:\n{:s}"), error).c_str(), sizeof(text));

	UiErrorOkDialog(_("Read-Only Directory Error"), text);
	app_fatal(nullptr);
}

} // namespace devilution

/**
 * @file appfat.h
 *
 * Interface of error dialogs.
 */
#pragma once

#include <SDL.h>

#include "utils/attributes.h"

namespace devilution {

#define ErrSdl() ErrDlg("SDL Error", SDL_GetError(), __FILE__, __LINE__)

#define ErrTtf() ErrDlg("TTF Error", TTF_GetError(), __FILE__, __LINE__)

#undef assert

#ifndef _DEBUG
#define assert(exp)
#define assurance(exp, value) \
	if (!(exp))               \
	return
#else
#define assert(exp) (void)((exp) || (assert_fail(__LINE__, __FILE__, #exp), 0))
#endif

/**
 * @brief Terminates the game and displays an error message box.
 * @param pszFmt Optional error message.
 * @param ... (see printf)
 */
[[noreturn]] void app_fatal(const char *pszFmt, ...) DVL_PRINTF_ATTRIBUTE(1, 2);

/**
 * @brief Displays a warning message box based on the given formatted error message.
 * @param pszFmt Error message format
 * @param ... Additional parameters for message format
 */
void DrawDlg(const char *pszFmt, ...) DVL_PRINTF_ATTRIBUTE(1, 2);
#ifdef _DEBUG
/**
 * @brief Show an error and exit the application.
 * @param nLineNo The line number of the assertion
 * @param pszFile File name where the assertion is located
 * @param pszFail Fail message
 */
[[noreturn]] void assert_fail(int nLineNo, const char *pszFile, const char *pszFail);
#endif
/**
 * @brief Terminates the game and displays an error dialog box based on the given dialog_id.
 */
[[noreturn]] void ErrDlg(const char *title, const char *error, const char *logFilePath, int logLineNr);

/**
 * @brief Terminates the game with an insert CD error dialog.
 */
[[noreturn]] void InsertCDDlg();

/**
 * @brief Terminates the game with a read-only directory error dialog.
 */
[[noreturn]] void DirErrorDlg(const char *error);

} // namespace devilution

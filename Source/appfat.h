/**
 * @file appfat.h
 *
 * Interface of error dialogs.
 */
#pragma once

#include <SDL.h>

#include "utils/attributes.h"
#include "utils/stdcompat/string_view.hpp"

namespace devilution {

#define ErrSdl() ErrDlg("SDL Error", SDL_GetError(), __FILE__, __LINE__)

#undef assert

#ifndef _DEBUG
#define assert(exp)
#else
#define assert(exp) (void)((exp) || (assert_fail(__LINE__, __FILE__, #exp), 0))
#endif

/**
 * @brief Terminates the game and displays an error message box.
 * @param str Error message.
 */
[[noreturn]] void app_fatal(string_view str);

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
[[noreturn]] void ErrDlg(const char *title, string_view error, string_view logFilePath, int logLineNr);

/**
 * @brief Terminates the game with an insert CD error dialog.
 */
[[noreturn]] void InsertCDDlg(string_view archiveName);

/**
 * @brief Terminates the game with a read-only directory error dialog.
 */
[[noreturn]] void DirErrorDlg(string_view error);

} // namespace devilution

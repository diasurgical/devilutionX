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

#undef assert

#ifndef _DEBUG
#define assert(exp)
#define assurance(exp, value) \
	if (!(exp))               \
	return
#define commitment(exp, value) \
	if (!(exp))                \
	return false
#else
#define assert(exp) (void)((exp) || (assert_fail(__LINE__, __FILE__, #exp), 0))
#define assurance(exp, value) (void)((exp) || (app_fatal("%s: %s was %i", __func__, #exp, value), 0))
#define commitment(exp, value) (void)((exp) || (app_fatal("%s: %s was %i", __func__, #exp, value), 0))
#endif

[[noreturn]] void app_fatal(const char *pszFmt, ...) DVL_PRINTF_ATTRIBUTE(1, 2);
void DrawDlg(const char *pszFmt, ...) DVL_PRINTF_ATTRIBUTE(1, 2);
#ifdef _DEBUG
[[noreturn]] void assert_fail(int nLineNo, const char *pszFile, const char *pszFail);
#endif
[[noreturn]] void ErrDlg(const char *title, const char *error, const char *logFilePath, int logLineNr);
[[noreturn]] void FileErrDlg(const char *error);
[[noreturn]] void InsertCDDlg();
[[noreturn]] void DirErrorDlg(const char *error);

} // namespace devilution

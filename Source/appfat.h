/**
 * @file appfat.h
 *
 * Interface of error dialogs.
 */
#pragma once

#include <SDL.h>

namespace devilution {

[[noreturn]] void app_fatal(const char *pszFmt, ...);
void DrawDlg(const char *pszFmt, ...);
#ifdef _DEBUG
[[noreturn]] void assert_fail(int nLineNo, const char *pszFile, const char *pszFail);
#endif
[[noreturn]] void ErrDlg(const char *title, const char *error, const char *log_file_path, int log_line_nr);
[[noreturn]] void FileErrDlg(const char *error);
[[noreturn]] void InsertCDDlg();
[[noreturn]] void DirErrorDlg(const char *error);

}

/**
 * @file appfat.h
 *
 * Interface of error dialogs.
 */
#ifndef __APPFAT_H__
#define __APPFAT_H__

DEVILUTION_BEGIN_NAMESPACE

#ifdef __cplusplus
extern "C" {
#endif

#define TraceLastError SDL_GetError
[[noreturn]] void app_fatal(const char *pszFmt, ...);
void DrawDlg(const char *pszFmt, ...);
#ifdef _DEBUG
[[noreturn]] void assert_fail(int nLineNo, const char *pszFile, const char *pszFail);
#endif
[[noreturn]] void ErrDlg(const char *title, const char *error, const char *log_file_path, int log_line_nr);
[[noreturn]] void FileErrDlg(const char *error);
[[noreturn]] void InsertCDDlg();
[[noreturn]] void DirErrorDlg(const char *error);

#ifdef __cplusplus
}
#endif

DEVILUTION_END_NAMESPACE

#endif /* __APPFAT_H__ */

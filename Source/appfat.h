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

extern char sz_error_buf[256];
extern BOOL terminating;

void TriggerBreak();
char *GetErrorStr(DWORD error_code);
#define TraceLastError SDL_GetError
void app_fatal(const char *pszFmt, ...);
void MsgBox(const char *pszFmt, va_list va);
void FreeDlg();
void DrawDlg(const char *pszFmt, ...);
#ifdef _DEBUG
void assert_fail(int nLineNo, const char *pszFile, const char *pszFail);
#endif
void DDErrMsg(DWORD error_code, int log_line_nr, const char *log_file_path);
void DSErrMsg(DWORD error_code, int log_line_nr, const char *log_file_path);
void center_window(HWND hDlg);
void ErrDlg(const char *title, const char *error, const char *log_file_path, int log_line_nr);
void TextDlg(HWND hDlg, char *text);
void FileErrDlg(const char *error);
void DiskFreeDlg(const char *error);
void InsertCDDlg();
void DirErrorDlg(const char *error);

#ifdef __cplusplus
}
#endif

DEVILUTION_END_NAMESPACE

#endif /* __APPFAT_H__ */

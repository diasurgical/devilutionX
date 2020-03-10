//HEADER_GOES_HERE
#ifndef __APPFAT_H__
#define __APPFAT_H__

extern char sz_error_buf[256];
extern BOOL terminating;
extern int cleanup_thread_id;

void TriggerBreak();
char *GetErrorStr(DWORD error_code);
#define TraceLastError SDL_GetError
void app_fatal(const char *pszFmt, ...);
void MsgBox(const char *pszFmt, va_list va);
void FreeDlg();
void DrawDlg(char *pszFmt, ...);
#ifdef _DEBUG
void assert_fail(int nLineNo, const char *pszFile, const char *pszFail);
#endif
void DDErrMsg(DWORD error_code, int log_line_nr, char *log_file_path);
void DSErrMsg(DWORD error_code, int log_line_nr, char *log_file_path);
void center_window(HWND hDlg);
void ErrDlg(const char *title, const char *error, char *log_file_path, int log_line_nr);
void TextDlg(HWND hDlg, char *text);
void FileErrDlg(const char *error);
void DiskFreeDlg(char *error);
void InsertCDDlg(const char *fileName);
void DirErrorDlg(char *error);

#endif /* __APPFAT_H__ */

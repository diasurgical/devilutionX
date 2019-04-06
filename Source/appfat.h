//HEADER_GOES_HERE
#ifndef __APPFAT_H__
#define __APPFAT_H__

extern char sz_error_buf[256];
extern int terminating;       // weak
extern int cleanup_thread_id; // weak

char *__fastcall GetErrorStr(DWORD error_code);
void __fastcall TraceErrorDD(HRESULT hError, char *pszBuffer, DWORD dwMaxChars);
void __fastcall TraceErrorDS(HRESULT hError, char *pszBuffer, DWORD dwMaxChars);
char *__cdecl TraceLastError();
void app_fatal(const char *pszFmt, ...);
void __fastcall MsgBox(const char *pszFmt, va_list va);
void __cdecl FreeDlg();
void DrawDlg(char *pszFmt, ...);
#ifdef _DEBUG
void __fastcall assert_fail(int nLineNo, const char *pszFile, const char *pszFail);
#endif
void __fastcall DDErrMsg(DWORD error_code, int log_line_nr, char *log_file_path);
void __fastcall DSErrMsg(DWORD error_code, int log_line_nr, char *log_file_path);
void __fastcall center_window(HWND hDlg);
void __fastcall ErrDlg(int template_id, DWORD error_code, char *log_file_path, int log_line_nr);
BOOL __stdcall FuncDlg(HWND hDlg, UINT uMsg, WPARAM wParam, char *text);
void __fastcall TextDlg(HWND hDlg, char *text);
void __fastcall ErrOkDlg(int template_id, DWORD error_code, char *log_file_path, int log_line_nr);
void __fastcall FileErrDlg(const char *error);
void __fastcall DiskFreeDlg(char *error);
BOOL __cdecl InsertCDDlg();
void __fastcall DirErrorDlg(char *error);

#endif /* __APPFAT_H__ */

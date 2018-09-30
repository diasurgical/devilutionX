//HEADER_GOES_HERE
#ifndef __APPFAT_H__
#define __APPFAT_H__

extern char sz_error_buf[256];
extern int terminating; // weak
extern int cleanup_thread_id; // weak
extern char empty_string;

char *__fastcall GetErrorStr(int error_code);
void __fastcall TraceErrorDD(int error_code, char *error_buf, int error_buf_len);
void __fastcall TraceErrorDS(int error_code, char *error_buf, int error_buf_len);
char *__cdecl TraceLastError();
void TermMsg(char *pszFmt, ...);
void __fastcall MsgBox(char *pszFmt, va_list va);
void __cdecl FreeDlg();
void DrawDlg(char *pszFmt, ...);
void __fastcall DDErrMsg(int error_code, int log_line_nr, char *log_file_path);
void __fastcall DSErrMsg(int error_code, int log_line_nr, char *log_file_path);
void __fastcall center_window(HWND hDlg);
void __fastcall ErrDlg(int template_id, int error_code, char *log_file_path, int log_line_nr);
BOOL __stdcall FuncDlg(HWND hDlg, UINT uMsg, WPARAM wParam, char *text);
void __fastcall TextDlg(HWND hDlg, char *text);
void __fastcall ErrOkDlg(int template_id, int error_code, char *log_file_path, int log_line_nr);
void __fastcall FileErrDlg(char *error);
void __fastcall DiskFreeDlg(char *error);
bool __cdecl InsertCDDlg();
void __fastcall DirErrorDlg(char *error);

#endif /* __APPFAT_H__ */

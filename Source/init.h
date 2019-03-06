//HEADER_GOES_HERE
#ifndef __INIT_H__
#define __INIT_H__

extern _SNETVERSIONDATA fileinfo;
extern int gbActive; // weak
extern char diablo_exe_path[260];
extern HANDLE unused_mpq;
extern char patch_rt_mpq_path[260];
extern WNDPROC CurrentProc;
extern HANDLE diabdat_mpq;
extern char diabdat_mpq_path[260];
extern HANDLE patch_rt_mpq;
extern int killed_mom_parent; // weak
extern BOOLEAN screensaver_enabled_prev;

void __fastcall init_cleanup(BOOL show_cursor);
void __cdecl init_run_office_from_start_menu();
void __fastcall init_run_office(char *dir);
void __fastcall init_disable_screensaver(BOOLEAN disable);
void __fastcall init_create_window(int nCmdShow);
void __cdecl init_kill_mom_parent();
HWND __cdecl init_find_mom_parent();
void __cdecl init_await_mom_parent_exit();
void __cdecl init_archives();
HANDLE __fastcall init_test_access(char *mpq_path, char *mpq_name, char *reg_loc, int flags, int fs);
char *__fastcall init_strip_trailing_slash(char *path);
int __fastcall init_read_test_file(char *mpq_path, char *mpq_name, int flags, HANDLE *archive);
void __cdecl init_get_file_info();
LRESULT __stdcall MainWndProc(HWND hWnd, UINT Msg, WPARAM wParam, LPARAM lParam);
void __fastcall init_activate_window(HWND hWnd, BOOLEAN bActive);
LRESULT __stdcall WindowProc(HWND hWnd, UINT Msg, WPARAM wParam, LPARAM lParam);
WNDPROC __fastcall SetWindowProc(WNDPROC NewProc);

/* rdata */
extern const int init_inf; // weak

/* data */

extern char gszVersionNumber[260];
extern char gszProductName[260];

#endif /* __INIT_H__ */

//HEADER_GOES_HERE
#ifndef __INIT_H__
#define __INIT_H__

extern _SNETVERSIONDATA fileinfo;
extern int gbActive;
extern char diablo_exe_path[MAX_PATH];
extern HANDLE unused_mpq;
extern char patch_rt_mpq_path[MAX_PATH];
extern WNDPROC CurrentProc;
extern HANDLE diabdat_mpq;
extern char diabdat_mpq_path[MAX_PATH];
extern HANDLE patch_rt_mpq;
extern BOOL killed_mom_parent;
extern BOOLEAN screensaver_enabled_prev;

void init_cleanup(BOOL show_cursor);
void init_run_office_from_start_menu();
void init_run_office(char *dir);
void init_disable_screensaver(BOOLEAN disable);
void init_create_window(int nCmdShow);
void init_kill_mom_parent();
HWND init_find_mom_parent();
void init_await_mom_parent_exit();
void init_archives();
HANDLE init_test_access(char *mpq_path, char *mpq_name, char *reg_loc, int flags, int fs);
char *init_strip_trailing_slash(char *path);
BOOL init_read_test_file(char *pszPath, char *pszArchive, int flags, HANDLE *phArchive);
void init_get_file_info();
LRESULT __stdcall MainWndProc(HWND hWnd, UINT Msg, WPARAM wParam, LPARAM lParam);
void init_activate_window(HWND hWnd, BOOL bActive);
LRESULT __stdcall WindowProc(HWND hWnd, UINT Msg, WPARAM wParam, LPARAM lParam);
WNDPROC SetWindowProc(WNDPROC NewProc);

/* rdata */

/* data */

extern char gszVersionNumber[MAX_PATH];
extern char gszProductName[MAX_PATH];

#endif /* __INIT_H__ */

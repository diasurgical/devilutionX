/**
 * @file init.h
 *
 * Interface of routines for initializing the environment, disable screen saver, load MPQ.
 */
#ifndef __INIT_H__
#define __INIT_H__

extern _SNETVERSIONDATA fileinfo;
extern int gbActive;
extern char diablo_exe_path[MAX_PATH];
extern HANDLE hellfire_mpq;
extern char patch_rt_mpq_path[MAX_PATH];
extern WNDPROC CurrentProc;
extern HANDLE diabdat_mpq;
extern char diabdat_mpq_path[MAX_PATH];
extern HANDLE patch_rt_mpq;
extern BOOLEAN screensaver_enabled_prev;

void init_cleanup();
void init_disable_screensaver(BOOLEAN disable);
void init_create_window();
void init_archives();
HANDLE init_test_access(char *mpq_path, char *mpq_name, char *reg_loc, int flags, int fs);
void init_get_file_info();
LRESULT MainWndProc(HWND hWnd, UINT Msg, WPARAM wParam, LPARAM lParam);
void init_activate_window(HWND hWnd, BOOL bActive);
WNDPROC SetWindowProc(WNDPROC NewProc);

/* rdata */

/* data */

extern char gszVersionNumber[MAX_PATH];
extern char gszProductName[MAX_PATH];

#endif /* __INIT_H__ */

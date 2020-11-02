/**
 * @file init.h
 *
 * Interface of routines for initializing the environment, disable screen saver, load MPQ.
 */
#ifndef __INIT_H__
#define __INIT_H__

DEVILUTION_BEGIN_NAMESPACE

#ifdef __cplusplus
extern "C" {
#endif

extern _SNETVERSIONDATA fileinfo;
extern int gbActive;
extern HANDLE hellfire_mpq;
extern WNDPROC CurrentProc;
extern HANDLE diabdat_mpq;
extern HANDLE patch_rt_mpq;
extern bool gbIsSpawn;
extern BOOLEAN screensaver_enabled_prev;
#ifdef HELLFIRE
extern HANDLE hfmonk_mpq;
extern HANDLE hfbard_mpq;
extern HANDLE hfbarb_mpq;
extern HANDLE hfmusic_mpq;
extern HANDLE hfvoice_mpq;
extern HANDLE hfopt1_mpq;
extern HANDLE hfopt2_mpq;
#endif

void init_cleanup();
void init_disable_screensaver(BOOLEAN disable);
void init_create_window();
void init_archives();
void init_get_file_info();
void MainWndProc(HWND hWnd, UINT Msg, WPARAM wParam, LPARAM lParam);
void init_activate_window(HWND hWnd, BOOL bActive);
WNDPROC SetWindowProc(WNDPROC NewProc);

extern BOOL was_window_init;   /** defined in dx.cpp */

/* rdata */

/* data */

extern char gszVersionNumber[260];
extern char gszProductName[260];

#ifdef __cplusplus
}
#endif

DEVILUTION_END_NAMESPACE

#endif /* __INIT_H__ */

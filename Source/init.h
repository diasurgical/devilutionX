/**
 * @file init.h
 *
 * Interface of routines for initializing the environment, disable screen saver, load MPQ.
 */
#pragma once

namespace devilution {

#ifdef __cplusplus
extern "C" {
#endif

extern int gbActive;
extern HANDLE hellfire_mpq;
extern WNDPROC CurrentProc;
extern HANDLE spawn_mpq;
extern HANDLE diabdat_mpq;
extern bool gbIsSpawn;
extern bool gbIsHellfire;
extern bool gbVanilla;
extern HANDLE patch_rt_mpq;
extern HANDLE hfmonk_mpq;
extern HANDLE hfbard_mpq;
extern HANDLE hfbarb_mpq;
extern HANDLE hfmusic_mpq;
extern HANDLE hfvoice_mpq;
extern HANDLE hfopt1_mpq;
extern HANDLE hfopt2_mpq;
extern HANDLE devilutionx_mpq;

void init_cleanup();
void init_archives();
void init_create_window();
void MainWndProc(UINT Msg, WPARAM wParam, LPARAM lParam);
WNDPROC SetWindowProc(WNDPROC NewProc);

/* data */

extern char gszVersionNumber[64];
extern char gszProductName[64];

#ifdef __cplusplus
}
#endif

}

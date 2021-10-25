/**
 * @file init.h
 *
 * Interface of routines for initializing the environment, disable screen saver, load MPQ.
 */
#pragma once

#include "miniwin/miniwin.h"

namespace devilution {

extern bool gbActive;
extern HANDLE hellfire_mpq;
extern WNDPROC CurrentProc;
extern HANDLE spawn_mpq;
extern HANDLE diabdat_mpq;
extern bool gbIsSpawn;
extern bool gbIsHellfire;
extern bool gbVanilla;
extern bool forceHellfire;
extern HANDLE hfmonk_mpq;
extern HANDLE hfbard_mpq;
extern HANDLE hfbarb_mpq;
extern HANDLE hfmusic_mpq;
extern HANDLE hfvoice_mpq;
extern HANDLE font_mpq;
extern HANDLE lang_mpq;
extern HANDLE devilutionx_mpq;

void init_cleanup();
void init_archives();
void init_create_window();
void MainWndProc(uint32_t Msg);
WNDPROC SetWindowProc(WNDPROC NewProc);

} // namespace devilution

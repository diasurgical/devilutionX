/**
 * @file init.cpp
 *
 * Implementation of routines for initializing the environment, disable screen saver, load MPQ.
 */
#include "all.h"

#include "../3rdParty/Storm/Source/storm.h"
#include "../DiabloUI/diabloui.h"
#include <SDL.h>
#include <config.h>

#include <iostream>
#include <stdio.h>
#include <string.h>

#include <cstdio>    // fopen, fclose, fread, fwrite, BUFSIZ
//#include <ctime>

#ifdef  ANDROID
 bool FullGame = 0;
#endif

DEVILUTION_BEGIN_NAMESPACE

_SNETVERSIONDATA fileinfo;
int gbActive;
char diablo_exe_path[MAX_PATH];
HANDLE hellfire_mpq;
char patch_rt_mpq_path[MAX_PATH];
WNDPROC CurrentProc;
HANDLE diabdat_mpq = NULL;
char diabdat_mpq_path[MAX_PATH];
HANDLE patch_rt_mpq;

/* data */

char gszVersionNumber[MAX_PATH] = "internal version unknown";
char gszProductName[MAX_PATH] = "Diablo v1.09";

void init_cleanup()
{
	pfile_flush_W();

	if (diabdat_mpq) {
		SFileCloseArchive(diabdat_mpq);
		diabdat_mpq = NULL;
	}
	if (patch_rt_mpq) {
		SFileCloseArchive(patch_rt_mpq);
		patch_rt_mpq = NULL;
	}
	if (hellfire_mpq) {
		SFileCloseArchive(hellfire_mpq);
		hellfire_mpq = NULL;
	}

	NetClose();
}

void init_create_window()
{
	if (!SpawnWindow(PROJECT_NAME, SCREEN_WIDTH, SCREEN_HEIGHT))
		app_fatal("Unable to create main window");
	dx_init(NULL);
	was_window_init = true;
	gbActive = true;
	gpBufStart = &gpBuffer[BUFFER_WIDTH * SCREEN_Y];
	gpBufEnd = (BYTE *)(BUFFER_WIDTH * (SCREEN_HEIGHT + SCREEN_Y));
	SDL_DisableScreenSaver();
}
#ifdef ANDROID
inline bool exists_test (const std::string& name) {
    if (FILE *file = fopen(name.c_str(), "r")) {
        fclose(file);
        return true;
    } else {
        return false;
    }
}



void CopySpawn(std::string Directory){
	SDL_RWops  *mpq = SDL_RWFromFile("spawn.mpq", "rb");
	SDL_RWops *dest = SDL_RWFromFile( Directory.c_str(), "wb+");


	Sint64 size = SDL_RWsize(mpq);
	SDL_Log("SIZE %d", size);
	char* res = (char*)malloc(size + 1);
	char* buf = res;

	Sint64 nb_read_total = 0, nb_read = 1;
	while (nb_read_total < size && nb_read != 0) {
		nb_read = SDL_RWread(mpq, buf, 1, (size - nb_read_total));
		nb_read_total += nb_read;
		buf += nb_read;
	}

	for( int i = 0; i < size; ++i )
	{
		SDL_RWwrite( dest, &res[ i ], sizeof(char), 1 );
	}
	SDL_RWclose( mpq );
	SDL_RWclose( dest );
}

#endif
#include <string.h>
#include <stdio.h>
#include <jni.h>





void init_archives()
{
	HANDLE fh;

	memset(&fileinfo, 0, sizeof(fileinfo));
	fileinfo.size = sizeof(fileinfo);
	fileinfo.versionstring = gszVersionNumber;
	fileinfo.executablefile = diablo_exe_path;
	fileinfo.originalarchivefile = diabdat_mpq_path;
	fileinfo.patcharchivefile = patch_rt_mpq_path;
	init_get_file_info();

#ifdef ANDROID


// if ( !exists_test("/sdcard/devilutionx/diabdat.mpq") ){
// 		FullGame = false;
// 		SDL_Log("DEBUG Diabdat.mpq Not Found!");
// 	}

	// if ( !exists_test("/sdcard/devilutionx/diabdat.mpq") ){
	// 	FullGame = false;
	// 	SDL_Log("DEBUG Diabdat.mpq Not Found!");

	// }
	if (exists_test("/sdcard/devilutionx/diabdat.mpq")){
		FullGame = true;
		SDL_Log("DEBUG diabdat.mpq Found!");
		diabdat_mpq = init_test_access(diabdat_mpq_path, "/sdcard/devilutionx/diabdat.mpq", "DiabloCD", 1000, FS_CD);

	}else{
		FullGame = false;
		if(!exists_test("/sdcard/devilutionx/spawn.mpq")){CopySpawn("/sdcard/devilutionx/spawn.mpq");}else{SDL_Log("DEBUG SPAWN Exists!");}
		diabdat_mpq = init_test_access(diabdat_mpq_path, "/sdcard/devilutionx/spawn.mpq", "DiabloSpawn", 1000, FS_PC);
		if(exists_test("/sdcard/devilutionx/spawn.mpq")){SDL_Log("DEBUG SPAWN Exists");}else{SDL_Log("DEBUG SPAWN Not Found!"); }
	}


#endif
#ifdef SPAWN && ifndef ANDROID
	diabdat_mpq = init_test_access(diabdat_mpq_path, "spawn.mpq", "DiabloSpawn", 1000, FS_PC);
#else
	//diabdat_mpq = init_test_access(diabdat_mpq_path, "/sdcard/devilutionx/diabdat.mpq", "DiabloCD", 1000, FS_CD);
#endif
	if (!SFileOpenFile("ui_art\\title.pcx", &fh))
if(!FullGame){
		InsertCDDlg("spawn.mpq");
}else{
		InsertCDDlg("diabdat.mpq");
}
	SFileCloseFile(fh);
if(!FullGame){
	patch_rt_mpq = init_test_access(patch_rt_mpq_path, "patch_sh.mpq", "DiabloSpawn", 2000, FS_PC);
}else{
	patch_rt_mpq = init_test_access(patch_rt_mpq_path, "patch_rt.mpq", "DiabloInstall", 2000, FS_PC);
 }

}

HANDLE init_test_access(char *mpq_path, char *mpq_name, char *reg_loc, int dwPriority, int fs)
{
	char Buffer[2][MAX_PATH];
	HANDLE archive;

	GetBasePath(Buffer[0], MAX_PATH);
	GetPrefPath(Buffer[1], MAX_PATH);

	for (int i = 0; i < 2; i++) {
		snprintf(mpq_path, MAX_PATH, "%s%s", Buffer[i], mpq_name);
#if !defined(__SWITCH__) && !defined(__AMIGA__)
		if (SFileOpenArchive(mpq_path, dwPriority, MPQ_FLAG_READ_ONLY, &archive)) {
#else
		if (SFileOpenArchive(mpq_path, dwPriority, 0, &archive)) {
#endif
			SFileSetBasePath(Buffer[i]);
			return archive;

		}
	}

	return NULL;
}

void init_get_file_info()
{
	snprintf(gszProductName, MAX_PATH, "%s v%s", PROJECT_NAME, PROJECT_VERSION);
	snprintf(gszVersionNumber, MAX_PATH, "version %s", PROJECT_VERSION);
}

LRESULT MainWndProc(HWND hWnd, UINT Msg, WPARAM wParam, LPARAM lParam)
{
	switch (Msg) {
	case DVL_WM_ERASEBKGND:
		return 0;
	case DVL_WM_PAINT:
		force_redraw = 255;
		break;
	case DVL_WM_CLOSE:
		return 0;
	case DVL_WM_QUERYNEWPALETTE:
		return 1;
	case DVL_WM_QUERYENDSESSION:
		diablo_quit(0);
	}

	return 0;
}

WNDPROC SetWindowProc(WNDPROC NewProc)
{
	WNDPROC OldProc;

	OldProc = CurrentProc;
	CurrentProc = NewProc;
	return OldProc;
}

DEVILUTION_END_NAMESPACE

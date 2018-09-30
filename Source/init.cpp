//HEADER_GOES_HERE

#include "../types.h"

_SNETVERSIONDATA fileinfo;
int init_cpp_init_value; // weak
int gbActive; // weak
char diablo_exe_path[260];
void *unused_mpq;
char patch_rt_mpq_path[260];
WNDPROC CurrentProc;
void *diabdat_mpq;
char diabdat_mpq_path[260];
void *patch_rt_mpq;
int killed_mom_parent; // weak
bool screensaver_enabled_prev;

const int init_inf = 0x7F800000; // weak

/* data */

char gszVersionNumber[260] = "internal version unknown";
char gszProductName[260] = "Diablo v1.09";

struct init_cpp_init
{
	init_cpp_init()
	{
		init_cpp_init_value = init_inf;
	}
} _init_cpp_init;
// 47AE20: using guessed type int init_inf;
// 63497C: using guessed type int init_cpp_init_value;

void __fastcall init_cleanup(bool show_cursor)
{
	int v1; // edi

	v1 = show_cursor;
	pfile_flush_W();
	init_disable_screensaver(0);
	init_run_office_from_start_menu();
	if ( diabdat_mpq )
	{
		SFileCloseArchive(diabdat_mpq);
		diabdat_mpq = 0;
	}
	if ( patch_rt_mpq )
	{
		SFileCloseArchive(patch_rt_mpq);
		patch_rt_mpq = 0;
	}
	if ( unused_mpq )
	{
		SFileCloseArchive(unused_mpq);
		unused_mpq = 0;
	}
	UiDestroy();
	effects_cleanup_sfx();
	sound_cleanup();
	NetClose();
	dx_cleanup();
	MI_Dummy(v1);
	StormDestroy();
	if ( v1 )
		ShowCursor(1);
}

void __cdecl init_run_office_from_start_menu()
{
	HWND v0; // eax
	char pszPath[256]; // [esp+0h] [ebp-104h]
	LPITEMIDLIST ppidl; // [esp+100h] [ebp-4h]

	if ( killed_mom_parent )
	{
		*pszPath = empty_string;
		killed_mom_parent = 0;
		memset(pszPath + 1, 0, sizeof(pszPath) - 1);
		// *(_WORD *)&pszPath[253] = 0;
		//pszPath[255] = 0;
		ppidl = 0;
		v0 = GetDesktopWindow();
		if ( !SHGetSpecialFolderLocation(v0, CSIDL_STARTMENU, &ppidl) )
		{
			SHGetPathFromIDList(ppidl, pszPath);
			init_run_office(pszPath);
		}
	}
}
// 634CA0: using guessed type int killed_mom_parent;

void __fastcall init_run_office(char *dir)
{
	char *v1; // esi
	HANDLE v2; // ebx
	bool v3; // zf
	HWND v4; // eax
	char Directory[260]; // [esp+8h] [ebp-348h]
	char FileName[260]; // [esp+10Ch] [ebp-244h]
	struct _WIN32_FIND_DATAA FindFileData; // [esp+210h] [ebp-140h]

	v1 = dir;
	strcpy(FileName, dir);
	if ( FileName[0] && Directory[strlen(FileName) + 259] == '\\' )
		strcat(FileName, "*");
	else
		strcat(FileName, "\\*");
	v2 = FindFirstFile(FileName, &FindFileData);
	if ( v2 != (HANDLE)-1 )
	{
		do
		{
			if ( FindFileData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY )
			{
				if ( strcmp(FindFileData.cFileName, ".") && strcmp(FindFileData.cFileName, "..") )
				{
					*Directory = empty_string;
					memset(Directory + 1, 0, sizeof(Directory) - 1);
					v3 = *v1 == 0;
					// *(_WORD *)&Directory[257] = 0;
					//Directory[259] = 0;
					if ( v3 || v1[strlen(v1) - 1] != '\\' )
						sprintf(Directory, "%s\\%s\\", v1, FindFileData.cFileName);
					else
						sprintf(Directory, "%s%s\\", v1, FindFileData.cFileName);
					init_run_office(Directory);
				}
			}
			else if ( !_strcmpi(FindFileData.cFileName, "Microsoft Office Shortcut Bar.lnk") )
			{
				v4 = GetDesktopWindow();
				ShellExecute(v4, "open", FindFileData.cFileName, &empty_string, v1, SW_SHOWNORMAL);
			}
		}
		while ( FindNextFile(v2, &FindFileData) );
		FindClose(v2);
	}
}

void __fastcall init_disable_screensaver(bool disable)
{
	bool v1; // al
	char Data[16]; // [esp+4h] [ebp-20h]
	DWORD Type; // [esp+14h] [ebp-10h]
	DWORD cbData; // [esp+18h] [ebp-Ch]
	HKEY phkResult; // [esp+1Ch] [ebp-8h]
	bool v6; // [esp+20h] [ebp-4h]

	// BUGFIX: this is probably the worst possible way to do this. Alternatives: ExtEscape() with SETPOWERMANAGEMENT,
	// SystemParametersInfo() with SPI_SETSCREENSAVEACTIVE/SPI_SETPOWEROFFACTIVE/SPI_SETLOWPOWERACTIVE

	v6 = disable;
	if ( !RegOpenKeyEx(HKEY_CURRENT_USER, "Control Panel\\Desktop", 0, KEY_READ|KEY_WRITE, &phkResult) )
	{
		if ( v6 )
		{
			cbData = 16;
			if ( !RegQueryValueEx(phkResult, "ScreenSaveActive", 0, &Type, (LPBYTE)Data, &cbData) )
				screensaver_enabled_prev = Data[0] != '0';
			v1 = 0;
		}
		else
		{
			v1 = screensaver_enabled_prev;
		}
		Data[1] = 0;
		Data[0] = (v1 != 0) + '0';
		RegSetValueEx(phkResult, "ScreenSaveActive", 0, REG_SZ, (const BYTE *)Data, 2u);
		RegCloseKey(phkResult);
	}
}

void __fastcall init_create_window(int nCmdShow)
{

	
	int nHeight; // eax
	HWND hWnd; // esi
	WNDCLASSEXA wcex; // [esp+8h] [ebp-34h]
	int nWidth; // [esp+38h] [ebp-4h]

	init_kill_mom_parent();
	pfile_init_save_directory();
	memset(&wcex, 0, sizeof(wcex));
	wcex.cbSize = sizeof(wcex);
	wcex.style = CS_HREDRAW|CS_VREDRAW;
	wcex.lpfnWndProc = WindowProc;
	wcex.hInstance = ghInst;
	wcex.hIcon = LoadIcon(ghInst, MAKEINTRESOURCE(IDI_ICON1));
	wcex.hCursor = LoadCursor(0, IDC_ARROW);
	wcex.hbrBackground = (HBRUSH)GetStockObject(BLACK_BRUSH);
	wcex.lpszMenuName = "DIABLO";
	wcex.lpszClassName = "DIABLO";
	wcex.hIconSm = (HICON)LoadImage(ghInst, MAKEINTRESOURCE(IDI_ICON1), IMAGE_ICON, 16, 16, LR_DEFAULTCOLOR);
	if ( !RegisterClassEx(&wcex) )
		TermMsg("Unable to register window class");
	if ( GetSystemMetrics(SM_CXSCREEN) >= 640 )
		nWidth = GetSystemMetrics(SM_CXSCREEN);
	else
		nWidth = 640;
	if ( GetSystemMetrics(SM_CYSCREEN) >= 480 )
		nHeight = GetSystemMetrics(SM_CYSCREEN);
	else
		nHeight = 480;
	hWnd = CreateWindowEx(0, "DIABLO", "DIABLO", WS_POPUP, 0, 0, nWidth, nHeight, NULL, NULL, ghInst, NULL);
	if ( !hWnd )
		TermMsg("Unable to create main window");
	ShowWindow(hWnd, SW_SHOWNORMAL); // nCmdShow used only in beta: ShowWindow(hWnd, nCmdShow)
	UpdateWindow(hWnd);
	init_await_mom_parent_exit();
	dx_init(hWnd);
	BlackPalette();
	snd_init(hWnd);
	init_archives();
	init_disable_screensaver(1);
}

void __cdecl init_kill_mom_parent()
{
	HWND v0; // eax

	v0 = init_find_mom_parent();
	if ( v0 )
	{
		PostMessage(v0, WM_CLOSE, 0, 0);
		killed_mom_parent = 1;
	}
}
// 634CA0: using guessed type int killed_mom_parent;

HWND __cdecl init_find_mom_parent()
{
	HWND i; // eax
	HWND v1; // esi
	char ClassName[256]; // [esp+4h] [ebp-100h]

	for ( i = GetForegroundWindow(); ; i = GetWindow(v1, GW_HWNDNEXT) )
	{
		v1 = i;
		if ( !i )
			break;
		GetClassName(i, ClassName, 255);
		if ( !_strcmpi(ClassName, "MOM Parent") )
			break;
	}
	return v1;
}

void __cdecl init_await_mom_parent_exit()
{
	DWORD v0; // edi

	v0 = GetTickCount();
	do
	{
		if ( !init_find_mom_parent() )
			break;
		Sleep(250);
	}
	while ( GetTickCount() - v0 <= 4000 );
}

void __cdecl init_archives()
{
	void *a1; // [esp+8h] [ebp-8h]
#ifdef COPYPROT
	int v1; // [esp+Ch] [ebp-4h]
#endif

	fileinfo.size = 20;
	fileinfo.versionstring = gszVersionNumber;
	fileinfo.executablefile = diablo_exe_path;
	fileinfo.originalarchivefile = diabdat_mpq_path;
	fileinfo.patcharchivefile = patch_rt_mpq_path;
	init_get_file_info();
#ifdef COPYPROT
	while ( 1 )
	{
#endif
		diabdat_mpq = init_test_access(diabdat_mpq_path, "\\diabdat.mpq", "DiabloCD", 1000, 1);
#ifdef COPYPROT
		if ( diabdat_mpq )
			break;
		UiCopyProtError((int)&v1);
		if ( v1 == COPYPROT_CANCEL )
			FileErrDlg("diabdat.mpq");
	}
#endif
	if ( !WOpenFile("ui_art\\title.pcx", &a1, 1) )
		FileErrDlg("Main program archive: diabdat.mpq");
	WCloseFile(a1);
	patch_rt_mpq = init_test_access(patch_rt_mpq_path, "\\patch_rt.mpq", "DiabloInstall", 2000, 0);
}

void *__fastcall init_test_access(char *mpq_path, char *mpq_name, char *reg_loc, int flags, bool on_cd)
{
	char *v5; // esi
	char *v7; // eax
	char Filename[260]; // [esp+Ch] [ebp-314h]
	char Buffer[260]; // [esp+110h] [ebp-210h]
	char v15[260]; // [esp+214h] [ebp-10Ch]
	char *mpq_namea; // [esp+318h] [ebp-8h]
	void *archive; // [esp+31Ch] [ebp-4h]

	mpq_namea = mpq_name;
	v5 = mpq_path;
	if ( !GetCurrentDirectory(0x104u, Buffer) )
		TermMsg("Can't get program path");
	init_strip_trailing_slash(Buffer);
	if ( !SFileSetBasePath(Buffer) )
		TermMsg("SFileSetBasePath");
	if ( !GetModuleFileName(ghInst, Filename, 0x104u) )
		TermMsg("Can't get program name");
	v7 = strrchr(Filename, '\\');
	if ( v7 )
		*v7 = 0;
	init_strip_trailing_slash(Filename);
	strcpy(v5, Buffer);
	strcat(v5, mpq_namea);
#ifdef COPYPROT
	if ( SFileOpenArchive(v5, flags, on_cd, &archive) )
#else
	if ( SFileOpenArchive(v5, flags, 0, &archive) )
#endif
		return archive;
	if ( strcmp(Filename, Buffer) )
	{
		strcpy(v5, Filename);
		strcat(v5, mpq_namea);
#ifdef COPYPROT
		if ( SFileOpenArchive(v5, flags, on_cd, &archive) )
#else
		if ( SFileOpenArchive(v5, flags, 0, &archive) )
#endif
			return archive;
	}
	v15[0] = 0;
	if ( reg_loc )
	{
		if ( SRegLoadString("Archives", (const char *)reg_loc, 0, v15, 260) )
		{
			init_strip_trailing_slash(v15);
			strcpy(v5, v15);
			strcat(v5, mpq_namea);
#ifdef COPYPROT
			if ( SFileOpenArchive(v5, flags, on_cd, &archive) )
#else
			if ( SFileOpenArchive(v5, flags, 0, &archive) )
#endif
				return archive;
		}
	}
	if ( on_cd && init_read_test_file(v15, mpq_namea, flags, &archive) )
	{
		strcpy(v5, v15);
		return archive;
	}
	return 0;
}

char *__fastcall init_strip_trailing_slash(char *path)
{
	char *result; // eax

	result = strrchr(path, '\\');
	if ( result )
	{
		if ( !result[1] )
			*result = 0;
	}
	return result;
}

int __fastcall init_read_test_file(char *mpq_path, char *mpq_name, int flags, void **archive)
{
	char *v4; // edi
	DWORD v5; // eax
	const char *v7; // ebx
	const char *v8; // esi
	char Buffer[260]; // [esp+Ch] [ebp-108h]
	char *mpq_patha; // [esp+110h] [ebp-4h]

	v4 = mpq_name;
	mpq_patha = mpq_path;
	v5 = GetLogicalDriveStrings(0x104u, Buffer);
	if ( !v5 || v5 > 0x104 )
		return 0;
	while ( *v4 == '\\' )
		++v4;
	v7 = Buffer;
	if ( !Buffer[0] )
		return 0;
	while ( 1 )
	{
		v8 = v7;
		v7 += strlen(v7) + 1;
		if ( GetDriveType(v8) == DRIVE_CDROM )
		{
			strcpy(mpq_patha, v8);
			strcat(mpq_patha, v4);
			if ( SFileOpenArchive(mpq_patha, flags, 1, archive) )
				break;
		}
		if ( !*v7 )
			return 0;
	}
	return 1;
}

void __cdecl init_get_file_info()
{
	int v0; // eax
	DWORD v1; // edi
	void *v2; // ebx
	unsigned int uBytes; // [esp+8h] [ebp-Ch]
	DWORD dwHandle; // [esp+Ch] [ebp-8h]
	VS_FIXEDFILEINFO *lpBuffer; // [esp+10h] [ebp-4h]

	if ( GetModuleFileName(ghInst, diablo_exe_path, 0x104u) )
	{
		v0 = GetFileVersionInfoSize(diablo_exe_path, &dwHandle);
		v1 = v0;
		if ( v0 )
		{
			v2 = DiabloAllocPtr(v0);
			if ( GetFileVersionInfo(diablo_exe_path, 0, v1, v2) )
			{
				if ( VerQueryValue(v2, "\\", (LPVOID *)&lpBuffer, &uBytes) )
					sprintf(
						gszVersionNumber,
						"version %d.%d.%d.%d",
						lpBuffer->dwProductVersionMS >> 16,
						lpBuffer->dwProductVersionMS & 0xFFFF,
						lpBuffer->dwProductVersionLS >> 16,
						lpBuffer->dwProductVersionLS & 0xFFFF);
			}
			mem_free_dbg(v2);
		}
	}
}

LRESULT __stdcall MainWndProc(HWND hWnd, UINT Msg, WPARAM wParam, LPARAM lParam)
{
	if ( Msg > WM_ERASEBKGND )
	{
		if ( Msg == WM_ACTIVATEAPP )
		{
			init_activate_window(hWnd, wParam);
		}
		else
		{
			if ( Msg == WM_QUERYNEWPALETTE )
			{
				SDrawRealizePalette();
				return 1;
			}
			if ( Msg == WM_PALETTECHANGED && (HWND)wParam != hWnd )
				SDrawRealizePalette();
		}
	}
	else
	{
		switch ( Msg )
		{
			case WM_ERASEBKGND:
				return 0;
			case WM_CREATE:
				ghMainWnd = hWnd;
				break;
			case WM_DESTROY:
				init_cleanup(1);
				ghMainWnd = 0;
				PostQuitMessage(0);
				break;
			case WM_PAINT:
				drawpanflag = 255;
				break;
			case WM_CLOSE:
				return 0;
		}
	}
	return DefWindowProc(hWnd, Msg, wParam, lParam);
}
// 52571C: using guessed type int drawpanflag;

void __fastcall init_activate_window(HWND hWnd, bool bActive)
{
	LONG dwNewLong; // eax

	gbActive = bActive;
	UiAppActivate(bActive);
	dwNewLong = GetWindowLong(hWnd, GWL_STYLE);

	if ( gbActive && fullscreen )
		dwNewLong &= ~WS_SYSMENU;
	else
		dwNewLong |= WS_SYSMENU;

	SetWindowLong(hWnd, GWL_STYLE, dwNewLong);

	if ( gbActive )
	{
		drawpanflag = 255;
		ResetPal();
	}
}
// 484364: using guessed type int fullscreen;
// 52571C: using guessed type int drawpanflag;
// 634980: using guessed type int gbActive;

LRESULT __stdcall WindowProc(HWND hWnd, UINT Msg, WPARAM wParam, LPARAM lParam)
{
	LRESULT result; // eax

	if ( CurrentProc )
		result = CurrentProc(hWnd, Msg, wParam, lParam);
	else
		result = MainWndProc(hWnd, Msg, wParam, lParam);
	return result;
}

WNDPROC __stdcall SetWindowProc(WNDPROC NewProc)
{
	WNDPROC OldProc; // eax

	OldProc = CurrentProc;
	CurrentProc = NewProc;
	return OldProc;
}

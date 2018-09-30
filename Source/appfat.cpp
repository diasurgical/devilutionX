//HEADER_GOES_HERE

#include "../types.h"

// +Infinity after initialization of appfat.cpp.
float appfat_cpp_init_value;

#ifndef NO_GLOBALS
char sz_error_buf[256];
int terminating; // weak
int cleanup_thread_id; // weak
#endif

char empty_string;

// appfat_cpp_init initializes the C++ runtime of appfat.cpp.
struct appfat_cpp_init {
	appfat_cpp_init() {
		appfat_cpp_init_value = INFINITY;
	}
} appfat_cpp_init;

// delete overloads the delete operator.
void operator delete(void *ptr) {
	if (ptr != NULL) {
		SMemFree(ptr, "delete", -1, 0);
	}
}

char *__fastcall GetErrorStr(int error_code)
{
	int v1; // edi
	unsigned int v2; // eax
	signed int v4; // eax
	char *i; // ecx

	v1 = error_code;
	v2 = ((unsigned int)error_code >> 16) & 0x1FFF;
	if ( v2 == 0x0878 )
	{
		TraceErrorDS(error_code, sz_error_buf, 256);
	}
	else if ( v2 == 0x0876 )
	{
		TraceErrorDD(error_code, sz_error_buf, 256);
	}
	else
	{
		if ( !SErrGetErrorStr(error_code, sz_error_buf, 256) && !FormatMessage(FORMAT_MESSAGE_FROM_SYSTEM, NULL, v1, 0x400u, sz_error_buf, 0x100u, NULL) )
			wsprintf(sz_error_buf, "unknown error 0x%08x", v1);
	}
	v4 = strlen(sz_error_buf);
	for ( i = &sz_error_buf[v4-1]; v4 > 0; *i = 0 )
	{
		--v4;
		--i;
		if ( *i != '\r' && *i != '\n' )
			break;
	}
	return sz_error_buf;
}

#define CASE_ERROR(v, errName) case errName: v = #errName; break;

void __fastcall TraceErrorDD(int error_code, char *error_buf, int error_buf_len)
{
	const char *v3; // eax
	char v4[20]; // [esp+0h] [ebp-14h]

	switch (error_code) {
	CASE_ERROR(v3, DDERR_CANTPAGEUNLOCK);
	CASE_ERROR(v3, DDERR_NOTPAGELOCKED);
	CASE_ERROR(v3, DD_OK);
	CASE_ERROR(v3, DDERR_CANTPAGELOCK);
	CASE_ERROR(v3, DDERR_BLTFASTCANTCLIP);
	CASE_ERROR(v3, DDERR_NOBLTHW);
	CASE_ERROR(v3, DDERR_NODDROPSHW);
	CASE_ERROR(v3, DDERR_OVERLAYNOTVISIBLE);
	CASE_ERROR(v3, DDERR_NOOVERLAYDEST);
	CASE_ERROR(v3, DDERR_INVALIDPOSITION);
	CASE_ERROR(v3, DDERR_NOTAOVERLAYSURFACE);
	CASE_ERROR(v3, DDERR_EXCLUSIVEMODEALREADYSET);
	CASE_ERROR(v3, DDERR_NOTFLIPPABLE);
	CASE_ERROR(v3, DDERR_CANTDUPLICATE);
	CASE_ERROR(v3, DDERR_NOTLOCKED);
	CASE_ERROR(v3, DDERR_CANTCREATEDC);
	CASE_ERROR(v3, DDERR_NODC);
	CASE_ERROR(v3, DDERR_WRONGMODE);
	CASE_ERROR(v3, DDERR_IMPLICITLYCREATED);
	CASE_ERROR(v3, DDERR_NOTPALETTIZED);
	CASE_ERROR(v3, DDERR_NOMIPMAPHW);
	CASE_ERROR(v3, DDERR_INVALIDSURFACETYPE);
	CASE_ERROR(v3, DDERR_DCALREADYCREATED);
	CASE_ERROR(v3, DDERR_NOPALETTEHW);
	CASE_ERROR(v3, DDERR_DIRECTDRAWALREADYCREATED);
	CASE_ERROR(v3, DDERR_NODIRECTDRAWHW);
	CASE_ERROR(v3, DDERR_PRIMARYSURFACEALREADYEXISTS);
	CASE_ERROR(v3, DDERR_NOEMULATION);
	CASE_ERROR(v3, DDERR_REGIONTOOSMALL);
	CASE_ERROR(v3, DDERR_CLIPPERISUSINGHWND);
	CASE_ERROR(v3, DDERR_NOCLIPPERATTACHED);
	CASE_ERROR(v3, DDERR_NOHWND);
	CASE_ERROR(v3, DDERR_HWNDSUBCLASSED);
	CASE_ERROR(v3, DDERR_HWNDALREADYSET);
	CASE_ERROR(v3, DDERR_NOPALETTEATTACHED);
	CASE_ERROR(v3, DDERR_INVALIDDIRECTDRAWGUID);
	CASE_ERROR(v3, DDERR_UNSUPPORTEDFORMAT);
	CASE_ERROR(v3, DDERR_UNSUPPORTEDMASK);
	CASE_ERROR(v3, DDERR_VERTICALBLANKINPROGRESS);
	CASE_ERROR(v3, DDERR_WASSTILLDRAWING);
	CASE_ERROR(v3, DDERR_XALIGN);
	CASE_ERROR(v3, DDERR_TOOBIGWIDTH);
	CASE_ERROR(v3, DDERR_CANTLOCKSURFACE);
	CASE_ERROR(v3, DDERR_SURFACEISOBSCURED);
	CASE_ERROR(v3, DDERR_SURFACELOST);
	CASE_ERROR(v3, DDERR_SURFACENOTATTACHED);
	CASE_ERROR(v3, DDERR_TOOBIGHEIGHT);
	CASE_ERROR(v3, DDERR_TOOBIGSIZE);
	CASE_ERROR(v3, DDERR_SURFACEBUSY);
	CASE_ERROR(v3, DDERR_OVERLAYCOLORKEYONLYONEACTIVE);
	CASE_ERROR(v3, DDERR_PALETTEBUSY);
	CASE_ERROR(v3, DDERR_COLORKEYNOTSET);
	CASE_ERROR(v3, DDERR_SURFACEALREADYATTACHED);
	CASE_ERROR(v3, DDERR_SURFACEALREADYDEPENDENT);
	CASE_ERROR(v3, DDERR_OVERLAYCANTCLIP);
	CASE_ERROR(v3, DDERR_NOVSYNCHW);
	CASE_ERROR(v3, DDERR_NOZBUFFERHW);
	CASE_ERROR(v3, DDERR_NOZOVERLAYHW);
	CASE_ERROR(v3, DDERR_OUTOFCAPS);
	CASE_ERROR(v3, DDERR_OUTOFVIDEOMEMORY);
	CASE_ERROR(v3, DDERR_NOTEXTUREHW);
	CASE_ERROR(v3, DDERR_NOROTATIONHW);
	CASE_ERROR(v3, DDERR_NOSTRETCHHW);
	CASE_ERROR(v3, DDERR_NOT4BITCOLOR);
	CASE_ERROR(v3, DDERR_NOT4BITCOLORINDEX);
	CASE_ERROR(v3, DDERR_NOT8BITCOLOR);
	CASE_ERROR(v3, DDERR_NORASTEROPHW);
	CASE_ERROR(v3, DDERR_NOEXCLUSIVEMODE);
	CASE_ERROR(v3, DDERR_NOFLIPHW);
	CASE_ERROR(v3, DDERR_NOGDI);
	CASE_ERROR(v3, DDERR_NOMIRRORHW);
	CASE_ERROR(v3, DDERR_NOTFOUND);
	CASE_ERROR(v3, DDERR_NOOVERLAYHW);
	CASE_ERROR(v3, DDERR_NOCOLORKEYHW);
	CASE_ERROR(v3, DDERR_NOALPHAHW);
	CASE_ERROR(v3, DDERR_NOCLIPLIST);
	CASE_ERROR(v3, DDERR_NOCOLORCONVHW);
	CASE_ERROR(v3, DDERR_NOCOOPERATIVELEVELSET);
	CASE_ERROR(v3, DDERR_NOCOLORKEY);
	CASE_ERROR(v3, DDERR_NO3D);
	CASE_ERROR(v3, DDERR_INVALIDMODE);
	CASE_ERROR(v3, DDERR_INVALIDOBJECT);
	CASE_ERROR(v3, DDERR_INVALIDPIXELFORMAT);
	CASE_ERROR(v3, DDERR_INVALIDRECT);
	CASE_ERROR(v3, DDERR_LOCKEDSURFACES);
	CASE_ERROR(v3, DDERR_INVALIDCLIPLIST);
	CASE_ERROR(v3, DDERR_CURRENTLYNOTAVAIL);
	CASE_ERROR(v3, DDERR_EXCEPTION);
	CASE_ERROR(v3, DDERR_HEIGHTALIGN);
	CASE_ERROR(v3, DDERR_INCOMPATIBLEPRIMARY);
	CASE_ERROR(v3, DDERR_INVALIDCAPS);
	CASE_ERROR(v3, DDERR_CANNOTDETACHSURFACE);
	CASE_ERROR(v3, DDERR_UNSUPPORTED);
	CASE_ERROR(v3, DDERR_GENERIC);
	CASE_ERROR(v3, DDERR_OUTOFMEMORY);
	CASE_ERROR(v3, DDERR_INVALIDPARAMS);
	CASE_ERROR(v3, DDERR_ALREADYINITIALIZED);
	CASE_ERROR(v3, DDERR_CANNOTATTACHSURFACE);
	default:
		strcpy(v4, "DDERR unknown 0x%x");
		sprintf(error_buf, v4, error_code);
		return;
	}
	strncpy(error_buf, v3, error_buf_len);
}

void __fastcall TraceErrorDS(int error_code, char *error_buf, int error_buf_len)
{
	const char *v3; // eax
	char v4[20]; // [esp+0h] [ebp-14h]

	switch (error_code) {
	CASE_ERROR(v3, DSERR_PRIOLEVELNEEDED);
	CASE_ERROR(v3, DSERR_BADFORMAT);
	CASE_ERROR(v3, DSERR_NODRIVER);
	CASE_ERROR(v3, DSERR_ALREADYINITIALIZED);
	CASE_ERROR(v3, DSERR_BUFFERLOST);
	CASE_ERROR(v3, DS_OK);
	CASE_ERROR(v3, DSERR_INVALIDCALL);
	CASE_ERROR(v3, E_NOINTERFACE);
	CASE_ERROR(v3, DSERR_NOAGGREGATION);
	CASE_ERROR(v3, DSERR_OUTOFMEMORY);
	CASE_ERROR(v3, DSERR_INVALIDPARAM);
	CASE_ERROR(v3, DSERR_ALLOCATED);
	CASE_ERROR(v3, DSERR_CONTROLUNAVAIL);
	default:
		strcpy(v4, "DSERR unknown 0x%x");
		sprintf(error_buf, v4, error_code);
		return;
	}
	strncpy(error_buf, v3, error_buf_len);
}

char *__cdecl TraceLastError()
{
	int v0; // eax

	v0 = GetLastError();
	return GetErrorStr(v0);
}

void TermMsg(char *pszFmt, ...)
{
	va_list arglist; // [esp+8h] [ebp+8h]

	va_start(arglist, pszFmt);
	FreeDlg();
	if ( pszFmt )
		MsgBox(pszFmt, arglist);
	init_cleanup(0);
	exit(1);
}

void __fastcall MsgBox(char *pszFmt, va_list va)
{
	char Text[256]; // [esp+0h] [ebp-100h]

	wvsprintf(Text, pszFmt, va);
	if ( ghMainWnd )
		SetWindowPos(ghMainWnd, HWND_NOTOPMOST, 0, 0, 0, 0, SWP_NOACTIVATE|SWP_NOMOVE|SWP_NOSIZE);
	MessageBox(ghMainWnd, Text, "ERROR", MB_TASKMODAL|MB_ICONHAND);
}

void __cdecl FreeDlg()
{
	if ( terminating && cleanup_thread_id != GetCurrentThreadId() )
		Sleep(20000u);
	terminating = 1;
	cleanup_thread_id = GetCurrentThreadId();
	dx_cleanup();
	if ( (unsigned char)gbMaxPlayers > 1u )
	{
		if ( SNetLeaveGame(3) )
			Sleep(2000u);
	}
	SNetDestroy();
	ShowCursor(1);
}
// 4B7A34: using guessed type int terminating;
// 4B7A38: using guessed type int cleanup_thread_id;
// 679660: using guessed type char gbMaxPlayers;

void DrawDlg(char *pszFmt, ...)
{
	char text[256]; // [esp+0h] [ebp-100h]
	va_list arglist; // [esp+10Ch] [ebp+Ch]

	va_start(arglist, pszFmt);
	wvsprintf(text, pszFmt, arglist);
	SDrawMessageBox(text, "Diablo", MB_TASKMODAL|MB_ICONEXCLAMATION);
}

void __fastcall DDErrMsg(int error_code, int log_line_nr, char *log_file_path)
{
	int v3; // esi
	char *v4; // eax

	v3 = log_line_nr;
	if ( error_code )
	{
		v4 = GetErrorStr(error_code);
		TermMsg("Direct draw error (%s:%d)\n%s", log_file_path, v3, v4);
	}
}

void __fastcall DSErrMsg(int error_code, int log_line_nr, char *log_file_path)
{
	int v3; // esi
	char *v4; // eax

	v3 = log_line_nr;
	if ( error_code )
	{
		v4 = GetErrorStr(error_code);
		TermMsg("Direct sound error (%s:%d)\n%s", log_file_path, v3, v4);
	}
}

void __fastcall center_window(HWND hDlg)
{
	LONG v1; // esi
	LONG v2; // edi
	int v3; // ebx
	char *v4; // eax
	struct tagRECT Rect; // [esp+Ch] [ebp-1Ch]
	int v6; // [esp+1Ch] [ebp-Ch]
	HDC hdc; // [esp+20h] [ebp-8h]
	HWND hWnd; // [esp+24h] [ebp-4h]

	hWnd = hDlg;
	GetWindowRect(hDlg, &Rect);
	v1 = Rect.right - Rect.left;
	v2 = Rect.bottom - Rect.top;
	hdc = GetDC(hWnd);
	v6 = GetDeviceCaps(hdc, HORZRES);
	v3 = GetDeviceCaps(hdc, VERTRES);
	ReleaseDC(hWnd, hdc);
	if ( !SetWindowPos(hWnd, HWND_TOP, (v6 - v1) / 2, (v3 - v2) / 2, 0, 0, SWP_NOZORDER|SWP_NOSIZE) )
	{
		v4 = TraceLastError();
		TermMsg("center_window: %s", v4);
	}
}

void __fastcall ErrDlg(int template_id, int error_code, char *log_file_path, int log_line_nr)
{
	int v4; // ebx
	int v5; // edi
	char *v6; // esi
	char *v7; // eax
	char *v8; // eax
	LPARAM dwInitParam[128]; // [esp+Ch] [ebp-200h]

	v4 = error_code;
	v5 = template_id;
	FreeDlg();
	v6 = log_file_path;
	v7 = strrchr(log_file_path, '\\');
	if ( v7 )
		v6 = v7 + 1;
	v8 = GetErrorStr(v4);
	wsprintf((LPSTR)dwInitParam, "%s\nat: %s line %d", v8, v6, log_line_nr);
	if ( DialogBoxParam(ghInst, MAKEINTRESOURCE(v5), ghMainWnd, (DLGPROC)FuncDlg, (LPARAM)dwInitParam) == -1 )
		TermMsg("ErrDlg: %d", v5);
	TermMsg(0);
}

BOOL __stdcall FuncDlg(HWND hDlg, UINT uMsg, WPARAM wParam, char *text)
{
	if ( uMsg == WM_INITDIALOG )
	{
		TextDlg(hDlg, text);
	}
	else
	{
		if ( uMsg != WM_COMMAND )
			return 0;
		if ( (_WORD)wParam == 1 )
		{
			EndDialog(hDlg, 1);
		}
		else if ( (_WORD)wParam == 2 )
		{
			EndDialog(hDlg, 0);
		}
	}
	return 1;
}

void __fastcall TextDlg(HWND hDlg, char *text)
{
	char *v2; // esi
	HWND v3; // edi

	v2 = text;
	v3 = hDlg;
	center_window(hDlg);
	if ( v2 )
		SetDlgItemText(v3, 1000, v2);
}

void __fastcall ErrOkDlg(int template_id, int error_code, char *log_file_path, int log_line_nr)
{
	char *v4; // esi
	int v5; // edi
	unsigned short v6; // bx
	char *v7; // eax
	char *v8; // eax
	LPARAM dwInitParam[128]; // [esp+Ch] [ebp-200h]

	v4 = log_file_path;
	v5 = error_code;
	v6 = template_id;
	v7 = strrchr(log_file_path, '\\');
	if ( v7 )
		v4 = v7 + 1;
	v8 = GetErrorStr(v5);
	wsprintf((LPSTR)dwInitParam, "%s\nat: %s line %d", v8, v4, log_line_nr);
	DialogBoxParam(ghInst, MAKEINTRESOURCE(v6), ghMainWnd, (DLGPROC)FuncDlg, (LPARAM)dwInitParam);
}

void __fastcall FileErrDlg(char *error)
{
	char *v1; // esi

	v1 = error;
	FreeDlg();
	if ( !v1 )
		v1 = &empty_string;
	if ( DialogBoxParam(ghInst, MAKEINTRESOURCE(IDD_DIALOG3), ghMainWnd, (DLGPROC)FuncDlg, (LPARAM)v1) == -1 )
		TermMsg("FileErrDlg");
	TermMsg(0);
}

void __fastcall DiskFreeDlg(char *error)
{
	char *v1; // esi

	v1 = error;
	FreeDlg();
	if ( DialogBoxParam(ghInst, MAKEINTRESOURCE(IDD_DIALOG7), ghMainWnd, (DLGPROC)FuncDlg, (LPARAM)v1) == -1 )
		TermMsg("DiskFreeDlg");
	TermMsg(0);
}

bool __cdecl InsertCDDlg()
{
	int v0; // edi

	ShowCursor(1);
	v0 = DialogBoxParam(ghInst, MAKEINTRESOURCE(IDD_DIALOG9), ghMainWnd, (DLGPROC)FuncDlg, (LPARAM)&empty_string);
	if ( v0 == -1 )
		TermMsg("InsertCDDlg");
	ShowCursor(0);
	return v0 == 1;
}

void __fastcall DirErrorDlg(char *error)
{
	char *v1; // esi

	v1 = error;
	FreeDlg();
	if ( DialogBoxParam(ghInst, MAKEINTRESOURCE(IDD_DIALOG11), ghMainWnd, (DLGPROC)FuncDlg, (LPARAM)v1) == -1 )
		TermMsg("DirErrorDlg");
	TermMsg(0);
}

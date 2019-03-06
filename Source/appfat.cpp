//HEADER_GOES_HERE

#include "../types.h"

char sz_error_buf[256];
BOOL terminating;
int cleanup_thread_id;

// delete overloads the delete operator.
//void operator delete(void *ptr)
//{
//	if (ptr != NULL) {
//		SMemFree(ptr, "delete", -1, 0);
//	}
//}

char *__fastcall GetErrorStr(DWORD error_code)
{
	DWORD upper_code;
	int size;
	char *chr;

	upper_code = (error_code >> 16) & 0x1FFF;
	if (upper_code == 0x0878) {
		TraceErrorDS(error_code, sz_error_buf, 256);
	} else if (upper_code == 0x0876) {
		TraceErrorDD(error_code, sz_error_buf, 256);
	} else if (!SErrGetErrorStr(error_code, sz_error_buf, 256)
	    && !FormatMessage(FORMAT_MESSAGE_FROM_SYSTEM, NULL, error_code, 0x400, sz_error_buf, 0x100, NULL)) {
		wsprintf(sz_error_buf, "unknown error 0x%08x", error_code);
	}

	size = strlen(sz_error_buf);

	chr = &sz_error_buf[size - 1];
	while (size > 0) {
		size--;
		chr--;

		if (*chr != '\r' && *chr != '\n')
			break;

		*chr = 0x00;
	}

	return sz_error_buf;
}

#define CASE_ERROR(v, errName) \
	case (DWORD)errName:              \
		v = #errName;          \
		break;

void __fastcall TraceErrorDD(DWORD error_code, char *error_buf, int error_buf_len)
{
	const char *v3; // eax
	char v4[20];    // [esp+0h] [ebp-14h]

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

void __fastcall TraceErrorDS(DWORD error_code, char *error_buf, int error_buf_len)
{
	const char *v3; // eax
	char v4[20];    // [esp+0h] [ebp-14h]

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
	return GetErrorStr(GetLastError());
}

void TermMsg(char *pszFmt, ...)
{
	va_list va;

	va_start(va, pszFmt);
	FreeDlg();

	if (pszFmt)
		MsgBox(pszFmt, va);

	va_end(va);

	init_cleanup(FALSE);
	exit(1);
}

void __fastcall MsgBox(char *pszFmt, va_list va)
{
	char Text[256]; // [esp+0h] [ebp-100h]

	wvsprintf(Text, pszFmt, va);
	if (ghMainWnd)
		SetWindowPos(ghMainWnd, HWND_NOTOPMOST, 0, 0, 0, 0, SWP_NOACTIVATE | SWP_NOMOVE | SWP_NOSIZE);
	MessageBox(ghMainWnd, Text, "ERROR", MB_TASKMODAL | MB_ICONHAND);
}

void __cdecl FreeDlg()
{
	if (terminating && cleanup_thread_id != GetCurrentThreadId())
		Sleep(20000);

	terminating = TRUE;
	cleanup_thread_id = GetCurrentThreadId();

	dx_cleanup();

	if (gbMaxPlayers > 1) {
		if (SNetLeaveGame(3))
			Sleep(2000);
	}

	SNetDestroy();
	ShowCursor(TRUE);
}

void DrawDlg(char *pszFmt, ...)
{
	char text[256];  // [esp+0h] [ebp-100h]
	va_list arglist; // [esp+10Ch] [ebp+Ch]

	va_start(arglist, pszFmt);
	wvsprintf(text, pszFmt, arglist);
	va_end(arglist);
	SDrawMessageBox(text, "Diablo", MB_TASKMODAL | MB_ICONEXCLAMATION);
}

void __fastcall DDErrMsg(DWORD error_code, int log_line_nr, char *log_file_path)
{
	char *msg;

	if (error_code) {
		msg = GetErrorStr(error_code);
		TermMsg("Direct draw error (%s:%d)\n%s", log_file_path, log_line_nr, msg);
	}
}

void __fastcall DSErrMsg(DWORD error_code, int log_line_nr, char *log_file_path)
{
	char *msg;

	if (error_code) {
		msg = GetErrorStr(error_code);
		TermMsg("Direct sound error (%s:%d)\n%s", log_file_path, log_line_nr, msg);
	}
}

void __fastcall center_window(HWND hDlg)
{
	LONG w, h;
	int screenW, screenH;
	struct tagRECT Rect;
	HDC hdc;

	GetWindowRect(hDlg, &Rect);
	w = Rect.right - Rect.left;
	h = Rect.bottom - Rect.top;
	hdc = GetDC(hDlg);
	screenW = GetDeviceCaps(hdc, HORZRES);
	screenH = GetDeviceCaps(hdc, VERTRES);
	ReleaseDC(hDlg, hdc);

	if (!SetWindowPos(hDlg, HWND_TOP, (screenW - w) / 2, (screenH - h) / 2, 0, 0, SWP_NOZORDER | SWP_NOSIZE)) {
		TermMsg("center_window: %s", TraceLastError());
	}
}

void __fastcall ErrDlg(int template_id, DWORD error_code, char *log_file_path, int log_line_nr)
{
	char *size;
	LPARAM dwInitParam[128];

	FreeDlg();

	size = strrchr(log_file_path, '\\');
	if (size)
		log_file_path = size + 1;

	wsprintf((LPSTR)dwInitParam, "%s\nat: %s line %d", GetErrorStr(error_code), log_file_path, log_line_nr);
	if (DialogBoxParam(ghInst, MAKEINTRESOURCE(template_id), ghMainWnd, (DLGPROC)FuncDlg, (LPARAM)dwInitParam) == -1)
		TermMsg("ErrDlg: %d", template_id);

	TermMsg(NULL);
}

BOOL __stdcall FuncDlg(HWND hDlg, UINT uMsg, WPARAM wParam, char *text)
{
	switch (uMsg) {
	case WM_INITDIALOG:
		TextDlg(hDlg, text);
		break;
	case WM_COMMAND:
		if ((WORD)wParam == 1) {
			EndDialog(hDlg, 1);
		} else if ((WORD)wParam == 2) {
			EndDialog(hDlg, 0);
		}
		break;
	default:
		return FALSE;
	}

	return TRUE;
}

void __fastcall TextDlg(HWND hDlg, char *text)
{
	center_window(hDlg);

	if (text)
		SetDlgItemText(hDlg, 1000, text);
}

void __fastcall ErrOkDlg(int template_id, DWORD error_code, char *log_file_path, int log_line_nr)
{
	char *size;
	LPARAM dwInitParam[128];

	size = strrchr(log_file_path, '\\');
	if (size)
		log_file_path = size + 1;

	wsprintf((LPSTR)dwInitParam, "%s\nat: %s line %d", GetErrorStr(error_code), log_file_path, log_line_nr);
	DialogBoxParam(ghInst, MAKEINTRESOURCE(template_id), ghMainWnd, (DLGPROC)FuncDlg, (LPARAM)dwInitParam);
}

void __fastcall FileErrDlg(const char *error)
{
	FreeDlg();

	if (!error)
		error = "";

	if (DialogBoxParam(ghInst, MAKEINTRESOURCE(IDD_DIALOG3), ghMainWnd, (DLGPROC)FuncDlg, (LPARAM)error) == -1)
		TermMsg("FileErrDlg");

	TermMsg(NULL);
}

void __fastcall DiskFreeDlg(char *error)
{
	FreeDlg();

	if (DialogBoxParam(ghInst, MAKEINTRESOURCE(IDD_DIALOG7), ghMainWnd, (DLGPROC)FuncDlg, (LPARAM)error) == -1)
		TermMsg("DiskFreeDlg");

	TermMsg(NULL);
}

BOOL __cdecl InsertCDDlg()
{
	int nResult;

	ShowCursor(TRUE);

	nResult = DialogBoxParam(ghInst, MAKEINTRESOURCE(IDD_DIALOG9), ghMainWnd, (DLGPROC)FuncDlg, (LPARAM) "");
	if (nResult == -1)
		TermMsg("InsertCDDlg");

	ShowCursor(FALSE);

	return nResult == 1;
}

void __fastcall DirErrorDlg(char *error)
{
	FreeDlg();

	if (DialogBoxParam(ghInst, MAKEINTRESOURCE(IDD_DIALOG11), ghMainWnd, (DLGPROC)FuncDlg, (LPARAM)error) == -1)
		TermMsg("DirErrorDlg");

	TermMsg(NULL);
}

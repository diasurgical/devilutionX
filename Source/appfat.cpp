#ifdef SWITCH
	#include <switch.h>
#endif
#include "diablo.h"
#include "../3rdParty/Storm/Source/storm.h"

DEVILUTION_BEGIN_NAMESPACE

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

#ifdef _DEBUG
LONG __stdcall BreakFilter(PEXCEPTION_POINTERS pExc)
{
	if (pExc->ExceptionRecord == NULL) {
		return 0;
	}
	if (pExc->ExceptionRecord->ExceptionCode != EXCEPTION_BREAKPOINT) {
		return 0;
	}

	if (((BYTE *)pExc->ContextRecord->Eip)[0] == 0xCC) { // int 3
		pExc->ContextRecord->Eip++;
	}

	return -1;
}
#endif

char *GetErrorStr(DWORD error_code)
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

void TraceErrorDD(HRESULT hError, char *pszBuffer, DWORD dwMaxChars)
{
	const char *szError;

	switch (hError) {
	case DD_OK:
		szError = "DD_OK";
		break;
	case DDERR_INVALIDOBJECT:
		szError = "DDERR_INVALIDOBJECT";
		break;
	case DDERR_OUTOFMEMORY:
		szError = "DDERR_OUTOFMEMORY";
		break;
	default: {
		const char szUnknown[] = "DDERR unknown 0x%x";
		/// ASSERT: assert(dwMaxChars >= sizeof(szUnknown) + 10);
		sprintf(pszBuffer, szUnknown, hError);
		return;
	}
	}

	strncpy(pszBuffer, szError, dwMaxChars);
}

void TraceErrorDS(HRESULT hError, char *pszBuffer, DWORD dwMaxChars)
{
	const char *szError;

	switch (hError) {
	case DS_OK:
		szError = "DS_OK";
		break;
	default: {
		const char szUnknown[] = "DSERR unknown 0x%x";
		/// ASSERT: assert(dwMaxChars >= sizeof(szUnknown) + 10);
		sprintf(pszBuffer, szUnknown, hError);
		return;
	}
	}

	strncpy(pszBuffer, szError, dwMaxChars);
}

char *TraceLastError()
{
	return GetErrorStr(GetLastError());
}

void __cdecl app_fatal(const char *pszFmt, ...)
{
	va_list va;

#ifdef SWITCH
	svcOutputDebugString("fatal", 20);
#endif

	va_start(va, pszFmt);
	FreeDlg();

	if (pszFmt)
		MsgBox(pszFmt, va);

	va_end(va);

	init_cleanup(FALSE);
	exit(1);
}

void MsgBox(const char *pszFmt, va_list va)
{
	char Text[256];

	wvsprintf(Text, pszFmt, va);
#ifdef SWITCH
	svcOutputDebugString(Text, 256);
#else
	if (ghMainWnd)
		SetWindowPos(ghMainWnd, HWND_NOTOPMOST, 0, 0, 0, 0, SWP_NOACTIVATE | SWP_NOMOVE | SWP_NOSIZE);
	MessageBox(ghMainWnd, Text, "ERROR", MB_TASKMODAL | MB_ICONHAND);
#endif
}

void FreeDlg()
{
#ifndef SWITCH
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
#endif
}

void __cdecl DrawDlg(char *pszFmt, ...)
{
#ifndef SWITCH
	char text[256];
	va_list arglist;

	va_start(arglist, pszFmt);
	wvsprintf(text, pszFmt, arglist);
	va_end(arglist);
	SDrawMessageBox(text, "Diablo", MB_TASKMODAL | MB_ICONEXCLAMATION);
#endif
}

#ifdef _DEBUG
void assert_fail(int nLineNo, const char *pszFile, const char *pszFail)
{
	app_fatal("assertion failed (%d:%s)\n%s", nLineNo, pszFile, pszFail);
}
#endif

void DDErrMsg(DWORD error_code, int log_line_nr, char *log_file_path)
{
	char *msg;

	if (error_code) {
		msg = GetErrorStr(error_code);
		app_fatal("Direct draw error (%s:%d)\n%s", log_file_path, log_line_nr, msg);
	}
}

void DSErrMsg(DWORD error_code, int log_line_nr, char *log_file_path)
{
	char *msg;

	if (error_code) {
		msg = GetErrorStr(error_code);
		app_fatal("Direct sound error (%s:%d)\n%s", log_file_path, log_line_nr, msg);
	}
}

void center_window(HWND hDlg)
{
	LONG w, h;
	int screenW, screenH;
	struct tagRECT Rect;
	HDC hdc;

	GetWindowRect(hDlg, &Rect);
	w = Rect.right - Rect.left;
	h = Rect.bottom - Rect.top;
	screenW = GetDeviceCaps(hdc, HORZRES);
	screenH = GetDeviceCaps(hdc, VERTRES);

	if (!SetWindowPos(hDlg, HWND_TOP, (screenW - w) / 2, (screenH - h) / 2, 0, 0, SWP_NOZORDER | SWP_NOSIZE)) {
		app_fatal("center_window: %s", TraceLastError());
	}
}

void ErrDlg(int template_id, DWORD error_code, char *log_file_path, int log_line_nr)
{
	char *size;
	LPARAM dwInitParam[128];

	FreeDlg();

	size = strrchr(log_file_path, '\\');
	if (size)
		log_file_path = size + 1;

	wsprintf((LPSTR)dwInitParam, "%s\nat: %s line %d", GetErrorStr(error_code), log_file_path, log_line_nr);
	if (DialogBoxParam(ghInst, MAKEINTRESOURCE(template_id), ghMainWnd, (DLGPROC)FuncDlg, (LPARAM)dwInitParam) == -1)
		app_fatal("ErrDlg: %d", template_id);

	app_fatal(NULL);
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

void TextDlg(HWND hDlg, char *text)
{
	center_window(hDlg);

	if (text)
		SetDlgItemText(hDlg, 1000, text);
}

void FileErrDlg(const char *error)
{
	FreeDlg();

	if (!error)
		error = "";

	if (DialogBoxParam(ghInst, MAKEINTRESOURCE(IDD_DIALOG3), ghMainWnd, (DLGPROC)FuncDlg, (LPARAM)error) == -1)
		app_fatal("FileErrDlg");

	app_fatal(NULL);
}

BOOL InsertCDDlg()
{
	int nResult;

	ShowCursor(TRUE);

	nResult = DialogBoxParam(ghInst, MAKEINTRESOURCE(IDD_DIALOG9), ghMainWnd, (DLGPROC)FuncDlg, (LPARAM) "");
	if (nResult == -1)
		app_fatal("InsertCDDlg");

	ShowCursor(FALSE);

	return nResult == 1;
}

void DirErrorDlg(char *error)
{
	FreeDlg();

	if (DialogBoxParam(ghInst, MAKEINTRESOURCE(IDD_DIALOG11), ghMainWnd, (DLGPROC)FuncDlg, (LPARAM)error) == -1)
		app_fatal("DirErrorDlg");

	app_fatal(NULL);
}

DEVILUTION_END_NAMESPACE

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

void TriggerBreak()
{
#ifdef _DEBUG
	LPTOP_LEVEL_EXCEPTION_FILTER pFilter;

	pFilter = SetUnhandledExceptionFilter(BreakFilter);
	__debugbreak();
	SetUnhandledExceptionFilter(pFilter);
#endif
}

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
	case DDERR_CANTPAGEUNLOCK:
		szError = "DDERR_CANTPAGEUNLOCK";
		break;
	case DDERR_NOTPAGELOCKED:
		szError = "DDERR_NOTPAGELOCKED";
		break;
	case DD_OK:
		szError = "DD_OK";
		break;
	case DDERR_CANTPAGELOCK:
		szError = "DDERR_CANTPAGELOCK";
		break;
	case DDERR_BLTFASTCANTCLIP:
		szError = "DDERR_BLTFASTCANTCLIP";
		break;
	case DDERR_NOBLTHW:
		szError = "DDERR_NOBLTHW";
		break;
	case DDERR_NODDROPSHW:
		szError = "DDERR_NODDROPSHW";
		break;
	case DDERR_OVERLAYNOTVISIBLE:
		szError = "DDERR_OVERLAYNOTVISIBLE";
		break;
	case DDERR_NOOVERLAYDEST:
		szError = "DDERR_NOOVERLAYDEST";
		break;
	case DDERR_INVALIDPOSITION:
		szError = "DDERR_INVALIDPOSITION";
		break;
	case DDERR_NOTAOVERLAYSURFACE:
		szError = "DDERR_NOTAOVERLAYSURFACE";
		break;
	case DDERR_EXCLUSIVEMODEALREADYSET:
		szError = "DDERR_EXCLUSIVEMODEALREADYSET";
		break;
	case DDERR_NOTFLIPPABLE:
		szError = "DDERR_NOTFLIPPABLE";
		break;
	case DDERR_CANTDUPLICATE:
		szError = "DDERR_CANTDUPLICATE";
		break;
	case DDERR_NOTLOCKED:
		szError = "DDERR_NOTLOCKED";
		break;
	case DDERR_CANTCREATEDC:
		szError = "DDERR_CANTCREATEDC";
		break;
	case DDERR_NODC:
		szError = "DDERR_NODC";
		break;
	case DDERR_WRONGMODE:
		szError = "DDERR_WRONGMODE";
		break;
	case DDERR_IMPLICITLYCREATED:
		szError = "DDERR_IMPLICITLYCREATED";
		break;
	case DDERR_NOTPALETTIZED:
		szError = "DDERR_NOTPALETTIZED";
		break;
	case DDERR_NOMIPMAPHW:
		szError = "DDERR_NOMIPMAPHW";
		break;
	case DDERR_INVALIDSURFACETYPE:
		szError = "DDERR_INVALIDSURFACETYPE";
		break;
	case DDERR_DCALREADYCREATED:
		szError = "DDERR_DCALREADYCREATED";
		break;
	case DDERR_NOPALETTEHW:
		szError = "DDERR_NOPALETTEHW";
		break;
	case DDERR_DIRECTDRAWALREADYCREATED:
		szError = "DDERR_DIRECTDRAWALREADYCREATED";
		break;
	case DDERR_NODIRECTDRAWHW:
		szError = "DDERR_NODIRECTDRAWHW";
		break;
	case DDERR_PRIMARYSURFACEALREADYEXISTS:
		szError = "DDERR_PRIMARYSURFACEALREADYEXISTS";
		break;
	case DDERR_NOEMULATION:
		szError = "DDERR_NOEMULATION";
		break;
	case DDERR_REGIONTOOSMALL:
		szError = "DDERR_REGIONTOOSMALL";
		break;
	case DDERR_CLIPPERISUSINGHWND:
		szError = "DDERR_CLIPPERISUSINGHWND";
		break;
	case DDERR_NOCLIPPERATTACHED:
		szError = "DDERR_NOCLIPPERATTACHED";
		break;
	case DDERR_NOHWND:
		szError = "DDERR_NOHWND";
		break;
	case DDERR_HWNDSUBCLASSED:
		szError = "DDERR_HWNDSUBCLASSED";
		break;
	case DDERR_HWNDALREADYSET:
		szError = "DDERR_HWNDALREADYSET";
		break;
	case DDERR_NOPALETTEATTACHED:
		szError = "DDERR_NOPALETTEATTACHED";
		break;
	case DDERR_INVALIDDIRECTDRAWGUID:
		szError = "DDERR_INVALIDDIRECTDRAWGUID";
		break;
	case DDERR_UNSUPPORTEDFORMAT:
		szError = "DDERR_UNSUPPORTEDFORMAT";
		break;
	case DDERR_UNSUPPORTEDMASK:
		szError = "DDERR_UNSUPPORTEDMASK";
		break;
	case DDERR_VERTICALBLANKINPROGRESS:
		szError = "DDERR_VERTICALBLANKINPROGRESS";
		break;
	case DDERR_WASSTILLDRAWING:
		szError = "DDERR_WASSTILLDRAWING";
		break;
	case DDERR_XALIGN:
		szError = "DDERR_XALIGN";
		break;
	case DDERR_TOOBIGWIDTH:
		szError = "DDERR_TOOBIGWIDTH";
		break;
	case DDERR_CANTLOCKSURFACE:
		szError = "DDERR_CANTLOCKSURFACE";
		break;
	case DDERR_SURFACEISOBSCURED:
		szError = "DDERR_SURFACEISOBSCURED";
		break;
	case DDERR_SURFACELOST:
		szError = "DDERR_SURFACELOST";
		break;
	case DDERR_SURFACENOTATTACHED:
		szError = "DDERR_SURFACENOTATTACHED";
		break;
	case DDERR_TOOBIGHEIGHT:
		szError = "DDERR_TOOBIGHEIGHT";
		break;
	case DDERR_TOOBIGSIZE:
		szError = "DDERR_TOOBIGSIZE";
		break;
	case DDERR_SURFACEBUSY:
		szError = "DDERR_SURFACEBUSY";
		break;
	case DDERR_OVERLAYCOLORKEYONLYONEACTIVE:
		szError = "DDERR_OVERLAYCOLORKEYONLYONEACTIVE";
		break;
	case DDERR_PALETTEBUSY:
		szError = "DDERR_PALETTEBUSY";
		break;
	case DDERR_COLORKEYNOTSET:
		szError = "DDERR_COLORKEYNOTSET";
		break;
	case DDERR_SURFACEALREADYATTACHED:
		szError = "DDERR_SURFACEALREADYATTACHED";
		break;
	case DDERR_SURFACEALREADYDEPENDENT:
		szError = "DDERR_SURFACEALREADYDEPENDENT";
		break;
	case DDERR_OVERLAYCANTCLIP:
		szError = "DDERR_OVERLAYCANTCLIP";
		break;
	case DDERR_NOVSYNCHW:
		szError = "DDERR_NOVSYNCHW";
		break;
	case DDERR_NOZBUFFERHW:
		szError = "DDERR_NOZBUFFERHW";
		break;
	case DDERR_NOZOVERLAYHW:
		szError = "DDERR_NOZOVERLAYHW";
		break;
	case DDERR_OUTOFCAPS:
		szError = "DDERR_OUTOFCAPS";
		break;
	case DDERR_OUTOFVIDEOMEMORY:
		szError = "DDERR_OUTOFVIDEOMEMORY";
		break;
	case DDERR_NOTEXTUREHW:
		szError = "DDERR_NOTEXTUREHW";
		break;
	case DDERR_NOROTATIONHW:
		szError = "DDERR_NOROTATIONHW";
		break;
	case DDERR_NOSTRETCHHW:
		szError = "DDERR_NOSTRETCHHW";
		break;
	case DDERR_NOT4BITCOLOR:
		szError = "DDERR_NOT4BITCOLOR";
		break;
	case DDERR_NOT4BITCOLORINDEX:
		szError = "DDERR_NOT4BITCOLORINDEX";
		break;
	case DDERR_NOT8BITCOLOR:
		szError = "DDERR_NOT8BITCOLOR";
		break;
	case DDERR_NORASTEROPHW:
		szError = "DDERR_NORASTEROPHW";
		break;
	case DDERR_NOEXCLUSIVEMODE:
		szError = "DDERR_NOEXCLUSIVEMODE";
		break;
	case DDERR_NOFLIPHW:
		szError = "DDERR_NOFLIPHW";
		break;
	case DDERR_NOGDI:
		szError = "DDERR_NOGDI";
		break;
	case DDERR_NOMIRRORHW:
		szError = "DDERR_NOMIRRORHW";
		break;
	case DDERR_NOTFOUND:
		szError = "DDERR_NOTFOUND";
		break;
	case DDERR_NOOVERLAYHW:
		szError = "DDERR_NOOVERLAYHW";
		break;
	case DDERR_NOCOLORKEYHW:
		szError = "DDERR_NOCOLORKEYHW";
		break;
	case DDERR_NOALPHAHW:
		szError = "DDERR_NOALPHAHW";
		break;
	case DDERR_NOCLIPLIST:
		szError = "DDERR_NOCLIPLIST";
		break;
	case DDERR_NOCOLORCONVHW:
		szError = "DDERR_NOCOLORCONVHW";
		break;
	case DDERR_NOCOOPERATIVELEVELSET:
		szError = "DDERR_NOCOOPERATIVELEVELSET";
		break;
	case DDERR_NOCOLORKEY:
		szError = "DDERR_NOCOLORKEY";
		break;
	case DDERR_NO3D:
		szError = "DDERR_NO3D";
		break;
	case DDERR_INVALIDMODE:
		szError = "DDERR_INVALIDMODE";
		break;
	case DDERR_INVALIDOBJECT:
		szError = "DDERR_INVALIDOBJECT";
		break;
	case DDERR_INVALIDPIXELFORMAT:
		szError = "DDERR_INVALIDPIXELFORMAT";
		break;
	case DDERR_INVALIDRECT:
		szError = "DDERR_INVALIDRECT";
		break;
	case DDERR_LOCKEDSURFACES:
		szError = "DDERR_LOCKEDSURFACES";
		break;
	case DDERR_INVALIDCLIPLIST:
		szError = "DDERR_INVALIDCLIPLIST";
		break;
	case DDERR_CURRENTLYNOTAVAIL:
		szError = "DDERR_CURRENTLYNOTAVAIL";
		break;
	case DDERR_EXCEPTION:
		szError = "DDERR_EXCEPTION";
		break;
	case DDERR_HEIGHTALIGN:
		szError = "DDERR_HEIGHTALIGN";
		break;
	case DDERR_INCOMPATIBLEPRIMARY:
		szError = "DDERR_INCOMPATIBLEPRIMARY";
		break;
	case DDERR_INVALIDCAPS:
		szError = "DDERR_INVALIDCAPS";
		break;
	case DDERR_CANNOTDETACHSURFACE:
		szError = "DDERR_CANNOTDETACHSURFACE";
		break;
	case DDERR_UNSUPPORTED:
		szError = "DDERR_UNSUPPORTED";
		break;
	case DDERR_GENERIC:
		szError = "DDERR_GENERIC";
		break;
	case DDERR_OUTOFMEMORY:
		szError = "DDERR_OUTOFMEMORY";
		break;
	case DDERR_INVALIDPARAMS:
		szError = "DDERR_INVALIDPARAMS";
		break;
	case DDERR_ALREADYINITIALIZED:
		szError = "DDERR_ALREADYINITIALIZED";
		break;
	case DDERR_CANNOTATTACHSURFACE:
		szError = "DDERR_CANNOTATTACHSURFACE";
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
	case DSERR_PRIOLEVELNEEDED:
		szError = "DSERR_PRIOLEVELNEEDED";
		break;
	case DSERR_BADFORMAT:
		szError = "DSERR_BADFORMAT";
		break;
	case DSERR_NODRIVER:
		szError = "DSERR_NODRIVER";
		break;
	case DSERR_ALREADYINITIALIZED:
		szError = "DSERR_ALREADYINITIALIZED";
		break;
	case DSERR_BUFFERLOST:
		szError = "DSERR_BUFFERLOST";
		break;
	case DS_OK:
		szError = "DS_OK";
		break;
	case DSERR_INVALIDCALL:
		szError = "DSERR_INVALIDCALL";
		break;
	case E_NOINTERFACE:
		szError = "E_NOINTERFACE";
		break;
	case DSERR_NOAGGREGATION:
		szError = "DSERR_NOAGGREGATION";
		break;
	case DSERR_OUTOFMEMORY:
		szError = "DSERR_OUTOFMEMORY";
		break;
	case DSERR_INVALIDPARAM:
		szError = "DSERR_INVALIDPARAM";
		break;
	case DSERR_ALLOCATED:
		szError = "DSERR_ALLOCATED";
		break;
	case DSERR_CONTROLUNAVAIL:
		szError = "DSERR_CONTROLUNAVAIL";
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

	va_start(va, pszFmt);
	FreeDlg();
#ifdef _DEBUG
	TriggerBreak();
#endif

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
	if (ghMainWnd)
		SetWindowPos(ghMainWnd, HWND_NOTOPMOST, 0, 0, 0, 0, SWP_NOACTIVATE | SWP_NOMOVE | SWP_NOSIZE);
	MessageBox(ghMainWnd, Text, "ERROR", MB_TASKMODAL | MB_ICONHAND);
}

void FreeDlg()
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

void __cdecl DrawDlg(char *pszFmt, ...)
{
	char text[256];
	va_list arglist;

	va_start(arglist, pszFmt);
	wvsprintf(text, pszFmt, arglist);
	va_end(arglist);
	SDrawMessageBox(text, "Diablo", MB_TASKMODAL | MB_ICONEXCLAMATION);
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
	hdc = GetDC(hDlg);
	screenW = GetDeviceCaps(hdc, HORZRES);
	screenH = GetDeviceCaps(hdc, VERTRES);
	ReleaseDC(hDlg, hdc);

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

void ErrOkDlg(int template_id, DWORD error_code, char *log_file_path, int log_line_nr)
{
	char *size;
	LPARAM dwInitParam[128];

	size = strrchr(log_file_path, '\\');
	if (size)
		log_file_path = size + 1;

	wsprintf((LPSTR)dwInitParam, "%s\nat: %s line %d", GetErrorStr(error_code), log_file_path, log_line_nr);
	DialogBoxParam(ghInst, MAKEINTRESOURCE(template_id), ghMainWnd, (DLGPROC)FuncDlg, (LPARAM)dwInitParam);
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

void DiskFreeDlg(char *error)
{
	FreeDlg();

	if (DialogBoxParam(ghInst, MAKEINTRESOURCE(IDD_DIALOG7), ghMainWnd, (DLGPROC)FuncDlg, (LPARAM)error) == -1)
		app_fatal("DiskFreeDlg");

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

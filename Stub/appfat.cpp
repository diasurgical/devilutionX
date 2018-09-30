#include "../types.h"
#include "stubs.h"

char empty_string = 0;

void TermMsg(char *pszFmt, ...)
{
	eprintf("%s: %s\n", __FUNCTION__, pszFmt);
	abort();
}

void __fastcall ErrDlg(int template_id, int error_code, char *log_file_path, int log_line_nr)
{
	UNIMPLEMENTED();
}

void __fastcall ErrOkDlg(int template_id, int error_code, char *log_file_path, int log_line_nr)
{
	UNIMPLEMENTED();
}

void __fastcall DirErrorDlg(char *error)
{
	UNIMPLEMENTED();
}

bool __cdecl InsertCDDlg()
{
	UNIMPLEMENTED();
}

void __fastcall FileErrDlg(char *error)
{
	UNIMPLEMENTED();
}

void __fastcall DDErrMsg(int error_code, int log_line_nr, char *log_file_path)
{
	UNIMPLEMENTED();
}

void __fastcall DiskFreeDlg(char *error)
{
	UNIMPLEMENTED();
}

void DrawDlg(char *pszFmt, ...)
{
	UNIMPLEMENTED();
}

char *__cdecl TraceLastError()
{
	return NULL;
}

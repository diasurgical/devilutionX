//HEADER_GOES_HERE
#ifndef __LOGGING_H__
#define __LOGGING_H__

extern CHAR FileName[MAX_PATH];
extern char log_buffer[388];
extern LPCVOID lpAddress;
extern DWORD nNumberOfBytesToWrite;

void __cdecl log_flush(BOOL force_close);
HANDLE log_create();
void log_get_version(VS_FIXEDFILEINFO *file_info);
void __cdecl log_printf(const char *pszFmt, ...); // LogMessage
void log_dump_computer_info();

/* rdata */

/* data */

extern int log_not_created;
extern HANDLE log_file;

#endif /* __LOGGING_H__ */

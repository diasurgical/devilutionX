//HEADER_GOES_HERE
#ifndef __WAVE_H__
#define __WAVE_H__

DEVILUTION_BEGIN_NAMESPACE

#ifdef __cplusplus
extern "C" {
#endif

void WCloseFile(HANDLE file);
LONG WGetFileSize(HANDLE hsFile, DWORD *lpFileSizeHigh, const char *FileName);
void WGetFileArchive(HANDLE hsFile, DWORD *retry, const char *FileName);
BOOL WOpenFile(const char *FileName, HANDLE *phsFile, BOOL mayNotExist);
void WReadFile(HANDLE hsFile, LPVOID buf, DWORD to_read, const char *FileName);
int WSetFilePointer(HANDLE file1, int offset, HANDLE file2, int whence);

#ifdef __cplusplus
}
#endif

DEVILUTION_END_NAMESPACE

#endif /* __WAVE_H__ */

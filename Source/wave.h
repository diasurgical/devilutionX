//HEADER_GOES_HERE
#ifndef __WAVE_H__
#define __WAVE_H__

BOOL __fastcall WCloseFile(HANDLE file);
LONG __fastcall WGetFileSize(HANDLE hsFile, DWORD *lpFileSizeHigh);
void __fastcall WGetFileArchive(HANDLE hsFile, unsigned int *retry, const char *FileName);
BOOL __fastcall WOpenFile(const char *FileName, HANDLE *phsFile, BOOL mayNotExist);
void __fastcall WReadFile(HANDLE hsFile, LPVOID buf, DWORD to_read);
int __fastcall WSetFilePointer(HANDLE file1, int offset, HANDLE file2, int whence);
BOOL __fastcall LoadWaveFormat(HANDLE hsFile, WAVEFORMATEX *pwfx);
void *__fastcall AllocateMemFile(HANDLE hsFile, MEMFILE *pMemFile, DWORD dwPos);
void __fastcall FreeMemFile(MEMFILE *pMemFile);
BOOL __fastcall ReadWaveFile(MEMFILE *pMemFile, WAVEFORMATEX *pwfx, CKINFO *chunk);
BOOL __fastcall ReadMemFile(MEMFILE *pMemFile, void *lpBuf, size_t length);
void __fastcall FillMemFile(MEMFILE *pMemFile);
int __fastcall SeekMemFile(MEMFILE *pMemFile, ULONG lDist, DWORD dwMethod);
BOOL __fastcall ReadWaveSection(MEMFILE *pMemFile, DWORD id, CKINFO *chunk);
BYTE *__fastcall LoadWaveFile(HANDLE hsFile, WAVEFORMATEX *pwfx, CKINFO *chunk);

#endif /* __WAVE_H__ */

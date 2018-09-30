//HEADER_GOES_HERE
#ifndef __WAVE_H__
#define __WAVE_H__

extern int wave_cpp_init_value; // weak
//int dword_6ABB9C; // weak

void __cdecl wave_cpp_init();
bool __fastcall WCloseFile(void *file);
int __fastcall WGetFileSize(HANDLE hsFile, unsigned long *a2);
void __fastcall WGetFileArchive(HANDLE hsFile, int *a2, char *dwInitParam);
int __fastcall WOpenFile(char *dwInitParam, HANDLE *phsFile, int a3);
char __fastcall WReadFile(HANDLE hsFile, char *buf, int a3);
int __fastcall WSetFilePointer(HANDLE file1, int offset, HANDLE file2, int whence);
int __fastcall LoadWaveFormat(HANDLE hsFile, WAVEFORMATEX *pwfx);
void *__fastcall AllocateMemFile(HANDLE hsFile, MEMFILE *pMemFile, unsigned int dwPos);
void __fastcall FreeMemFile(MEMFILE *pMemFile);
int __fastcall ReadWaveFile(MEMFILE *pMemFile, WAVEFORMATEX *pwfx, int *a3);
int __fastcall ReadMemFile(MEMFILE *pMemFile, void *lpBuf, size_t a3);
void __fastcall FillMemFile(MEMFILE *pMemFile);
int __fastcall SeekMemFile(MEMFILE *pMemFile, unsigned int lDist, int dwMethod);
int __fastcall ReadWaveSection(MEMFILE *pMemFile, int a2, int *a3);
void *__fastcall LoadWaveFile(HANDLE hsFile, WAVEFORMATEX *pwfx, int *a3);
void __fastcall j_engine_mem_free(void *ptr);

/* rdata */

extern const int wave_inf; // weak

#endif /* __WAVE_H__ */

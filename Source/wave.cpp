//HEADER_GOES_HERE

#include "../types.h"

#ifndef NO_GLOBALS
int wave_cpp_init_value; // weak
#endif

const int wave_inf = 0x7F800000; // weak

struct wave_cpp_init
{
	wave_cpp_init()
	{
		wave_cpp_init_value = wave_inf;
	}
} _wave_cpp_init;
// 4802D4: using guessed type int wave_inf;
// 6ABB34: using guessed type int wave_cpp_init_value;

bool __fastcall WCloseFile(void *file)
{
	return SFileCloseFile(file);
}

int __fastcall WGetFileSize(HANDLE hsFile, unsigned long *a2)
{
	unsigned long *v2; // edi
	HANDLE i; // esi
	int result; // eax
	int a2a; // [esp+8h] [ebp-4h]

	a2a = 0;
	v2 = a2;
	for ( i = hsFile; ; WGetFileArchive(i, &a2a, 0) )
	{
		result = SFileGetFileSize(i, v2);
		if ( result )
			break;
	}
	return result;
}

void __fastcall WGetFileArchive(HANDLE hsFile, int *a2, char *dwInitParam)
{
	int *v3; // esi
	HANDLE v4; // edi
	//int v5; // eax
	//int v6; // eax
	HANDLE archive; // [esp+8h] [ebp-4h]

	v3 = a2;
	v4 = hsFile;
	if ( (unsigned int)*a2 >= 5 )
		FileErrDlg(dwInitParam);
	if ( v4 && SFileGetFileArchive(v4, &archive) && archive != diabdat_mpq )
	{
		Sleep(0x14u);
		++*v3;
	}
	else
	{
		//_LOBYTE(v6) = InsertCDDlg();
		if ( !InsertCDDlg() )
			FileErrDlg(dwInitParam);
	}
}

int __fastcall WOpenFile(char *dwInitParam, HANDLE *phsFile, int a3)
{
	HANDLE *v3; // edi
	char *i; // esi
//	int v5; // eax
	int a2a; // [esp+8h] [ebp-4h]

	a2a = 0;
	v3 = phsFile;
	for ( i = dwInitParam; ; WGetFileArchive(0, &a2a, i) )
	{
		//_LOBYTE(v5) = SFileOpenFile(i, v3);
		if ( SFileOpenFile(i, v3) )
			return 1;
		if ( a3 && SErrGetLastError() == 2 )
			break;
	}
	return 0;
}

void __fastcall WReadFile(HANDLE hsFile, LPVOID buf, int a3)
{
	HANDLE v4; // edi
	int v5; // eax
	int nread; // [esp+Ch] [ebp-Ch]
	int offset; // [esp+10h] [ebp-8h]
	int a2a; // [esp+14h] [ebp-4h]

	v4 = hsFile;
	a2a = 0;
	for ( offset = WSetFilePointer(hsFile, 0, 0, 1); ; WSetFilePointer(v4, offset, 0, 0) )
	{
		v5 = SFileReadFile(v4, buf, a3, (unsigned long *)&nread, 0);
		if ( v5 )
			break;
		WGetFileArchive(v4, &a2a, 0);
	}
}

int __fastcall WSetFilePointer(HANDLE file1, int offset, HANDLE file2, int whence)
{
	int v4; // edi
	HANDLE i; // esi
	int result; // eax
	int a2; // [esp+8h] [ebp-4h]

	a2 = 0;
	v4 = offset;
	for ( i = file1; ; WGetFileArchive(i, &a2, 0) )
	{
		result = SFileSetFilePointer(i, v4, file2, whence);
		if ( result != -1 )
			break;
	}
	return result;
}

int __fastcall LoadWaveFormat(HANDLE hsFile, WAVEFORMATEX *pwfx)
{
	WAVEFORMATEX *v2; // esi
	int v3; // esi
	MEMFILE wave_file; // [esp+4h] [ebp-1Ch]

	v2 = pwfx;
	AllocateMemFile(hsFile, &wave_file, 0);
	v3 = ReadWaveFile(&wave_file, v2, 0);
	FreeMemFile(&wave_file);
	return v3;
}

void *__fastcall AllocateMemFile(HANDLE hsFile, MEMFILE *pMemFile, unsigned int dwPos)
{
	MEMFILE *v3; // esi
	HANDLE v4; // edi
	unsigned int v5; // eax
	unsigned int v6; // ecx
	void *result; // eax

	v3 = pMemFile;
	v4 = hsFile;
	memset(pMemFile, 0, 0x1Cu);
	v5 = WGetFileSize(v4, 0);
	v6 = 4096;
	v3->end = v5;
	if ( dwPos > 0x1000 )
		v6 = dwPos;
	v3->buf_len = v6;
	if ( v6 >= v5 )
		v6 = v5;
	v3->buf_len = v6;
	result = DiabloAllocPtr(v6);
	v3->file = (int)v4;
	v3->buf = (char *)result;
	return result;
}

void __fastcall FreeMemFile(MEMFILE *pMemFile)
{
	MEMFILE *v1; // eax
	char *v2; // ecx

	v1 = pMemFile;
	v2 = pMemFile->buf;
	v1->buf = 0;
	mem_free_dbg(v2);
}

int __fastcall ReadWaveFile(MEMFILE *pMemFile, WAVEFORMATEX *pwfx, int *a3)
{
	WAVEFORMATEX *v3; // esi
	MEMFILE *v4; // edi
	WORD v5; // ax
	int result; // eax
	int a2a[5]; // [esp+8h] [ebp-2Ch]
	PCMWAVEFORMAT v8; // [esp+1Ch] [ebp-18h]
	int v9[2]; // [esp+2Ch] [ebp-8h]

	v3 = pwfx;
	v4 = pMemFile;
	if ( !ReadMemFile(pMemFile, a2a, 0xCu)
	  || a2a[0] != 'FFIR'
	  || a2a[2] != 'EVAW'
	  || !ReadWaveSection(v4, ' tmf', v9)
	  || v9[0] < 0x10u
	  || !ReadMemFile(v4, &v8, 0x10u)
	  || SeekMemFile(v4, v9[0] - 16, FILE_CURRENT) == -1 )
	{
		return 0;
	}
	v5 = v8.wf.wFormatTag;
	v3->cbSize = 0;
	v3->wFormatTag = v5;
	v3->nChannels = v8.wf.nChannels;
	v3->nSamplesPerSec = v8.wf.nSamplesPerSec;
	v3->nAvgBytesPerSec = v8.wf.nAvgBytesPerSec;
	v3->nBlockAlign = v8.wf.nBlockAlign;
	v3->wBitsPerSample = v8.wBitsPerSample;
	if ( a3 )
		result = ReadWaveSection(v4, 'atad', a3);
	else
		result = 1;
	return result;
}

int __fastcall ReadMemFile(MEMFILE *pMemFile, void *lpBuf, size_t a3)
{
	size_t v3; // ebx
	void *v4; // ebp
	MEMFILE *v5; // esi
	size_t v6; // edi

	v3 = a3;
	v4 = lpBuf;
	v5 = pMemFile;
	if ( !a3 )
		return 1;
	while ( 1 )
	{
		if ( !v5->bytes_to_read )
			FillMemFile(v5);
		v6 = v5->bytes_to_read;
		if ( v3 < v6 )
			v6 = v3;
		if ( !v6 )
			break;
		memcpy(v4, &v5->buf[v5->dist], v6);
		v5->offset += v6;
		v5->dist += v6;
		v5->bytes_to_read -= v6;
		v3 -= v6;
		if ( !v3 )
			return 1;
	}
	return 0;
}

void __fastcall FillMemFile(MEMFILE *pMemFile)
{
	MEMFILE *v1; // esi
	unsigned int v2; // edi

	v1 = pMemFile;
	WSetFilePointer((HANDLE)pMemFile->file, pMemFile->offset, 0, 0);
	v2 = v1->end - v1->offset;
	if ( v1->buf_len < v2 )
		v2 = v1->buf_len;
	if ( v2 )
		WReadFile((HANDLE)v1->file, v1->buf, v2);
	v1->dist = 0;
	v1->bytes_to_read = v2;
}

int __fastcall SeekMemFile(MEMFILE *pMemFile, unsigned int lDist, int dwMethod)
{
	unsigned int v3; // eax

	v3 = pMemFile->bytes_to_read;
	if ( lDist >= v3 )
	{
		pMemFile->bytes_to_read = 0;
	}
	else
	{
		pMemFile->dist += lDist;
		pMemFile->bytes_to_read = v3 - lDist;
	}
	pMemFile->offset += lDist;
	return pMemFile->offset;
}

int __fastcall ReadWaveSection(MEMFILE *pMemFile, int a2, int *a3)
{
	int v3; // esi
	MEMFILE *v4; // edi
	int v6; // eax
	int a2a[2]; // [esp+8h] [ebp-8h]

	v3 = a2;
	v4 = pMemFile;
	while ( 1 )
	{
		if ( !ReadMemFile(v4, a2a, 8u) )
			return 0;
		if ( a2a[0] == v3 )
			break;
		if ( SeekMemFile(v4, a2a[1], FILE_CURRENT) == -1 )
			return 0;
	}
	*a3 = a2a[1];
	v6 = SeekMemFile(v4, 0, FILE_CURRENT);
	a3[1] = v6;
	return v6 != -1;
}

void *__fastcall LoadWaveFile(HANDLE hsFile, WAVEFORMATEX *pwfx, int *a3)
{
	WAVEFORMATEX *v3; // esi
	MEMFILE wave_file; // [esp+4h] [ebp-1Ch]

	v3 = pwfx;
	AllocateMemFile(hsFile, &wave_file, 0xFFFFFFFF);
	if ( ReadWaveFile(&wave_file, v3, a3) )
		return wave_file.buf;
	FreeMemFile(&wave_file);
	return 0;
}

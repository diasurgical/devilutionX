//HEADER_GOES_HERE

#include "../types.h"

BOOL __fastcall WCloseFile(HANDLE file)
{
	return SFileCloseFile(file);
}

LONG __fastcall WGetFileSize(HANDLE hsFile, DWORD *lpFileSizeHigh)
{
	unsigned int retry = 0;
	LONG ret;

	while ((ret = SFileGetFileSize(hsFile, lpFileSizeHigh)) == 0)
		WGetFileArchive(hsFile, &retry, NULL);

	return ret;
}

void __fastcall WGetFileArchive(HANDLE hsFile, unsigned int *retries, const char *FileName)
{
	HANDLE archive;

	if (*retries >= 5)
		FileErrDlg(FileName);

	if (hsFile && SFileGetFileArchive(hsFile, archive) && archive != diabdat_mpq) {
		Sleep(20);
		(*retries)++;
	} else if (!InsertCDDlg())
		FileErrDlg(FileName);
}

BOOL __fastcall WOpenFile(const char *FileName, HANDLE *phsFile, BOOL mayNotExist)
{
	unsigned int retry = 0;

	while (1) {
		if (SFileOpenFile(FileName, phsFile))
			return TRUE;
		if (mayNotExist && SErrGetLastError() == ERROR_FILE_NOT_FOUND)
			break;
		WGetFileArchive(NULL, &retry, FileName);
	}
	return FALSE;
}

void __fastcall WReadFile(HANDLE hsFile, LPVOID buf, DWORD to_read)
{
	unsigned int retry = 0;
	DWORD readed;
	int initial_pos = WSetFilePointer(hsFile, 0, NULL, FILE_CURRENT);

	while (!SFileReadFile(hsFile, buf, to_read, &readed, NULL)) {
		WGetFileArchive(hsFile, &retry, NULL);
		WSetFilePointer(hsFile, initial_pos, NULL, FILE_BEGIN);
	}
}

int __fastcall WSetFilePointer(HANDLE file1, int offset, HANDLE file2, int whence)
{
	unsigned int retry = 0;
	int result;

	while (1) {
		result = SFileSetFilePointer(file1, offset, file2, whence);
		if (result != -1)
			break;
		WGetFileArchive(file1, &retry, NULL);
	}
	return result;
}

BOOL __fastcall LoadWaveFormat(HANDLE hsFile, WAVEFORMATEX *pwfx)
{
	BOOL ret;
	MEMFILE wave_file;

	AllocateMemFile(hsFile, &wave_file, 0);
	ret = ReadWaveFile(&wave_file, pwfx, NULL);
	FreeMemFile(&wave_file);
	return ret;
}

void *__fastcall AllocateMemFile(HANDLE hsFile, MEMFILE *pMemFile, DWORD dwPos)
{
	DWORD length;

	memset(pMemFile, 0, sizeof(*pMemFile));
	pMemFile->end = WGetFileSize(hsFile, NULL);
	length = 4096;
	if (dwPos > length)
		length = dwPos;
	pMemFile->buf_len = length;
	if (length >= pMemFile->end)
		length = pMemFile->end;
	pMemFile->buf_len = length;
	pMemFile->buf = DiabloAllocPtr(length);
	pMemFile->file = hsFile;
	return pMemFile->buf;
}

void __fastcall FreeMemFile(MEMFILE *pMemFile)
{
	void *mem = pMemFile->buf;
	pMemFile->buf = NULL;
	mem_free_dbg(mem);
}

BOOL __fastcall ReadWaveFile(MEMFILE *pMemFile, WAVEFORMATEX *pwfx, CKINFO *chunk)
{
	MMCKINFO hdr;
	CKINFO fmt;
	PCMWAVEFORMAT wf;

	if (!ReadMemFile(pMemFile, &hdr, 12))
		return FALSE;
	if (hdr.ckid != FOURCC_RIFF || hdr.fccType != MAKEFOURCC('W', 'A', 'V', 'E'))
		return FALSE;
	if (!ReadWaveSection(pMemFile, MAKEFOURCC('f', 'm', 't', ' '), &fmt))
		return FALSE;
	if (fmt.dwSize < sizeof(PCMWAVEFORMAT))
		return FALSE;
	if (!ReadMemFile(pMemFile, &wf, sizeof(wf)))
		return FALSE;
	if (SeekMemFile(pMemFile, fmt.dwSize - sizeof(wf), FILE_CURRENT) == -1)
		return FALSE;

	pwfx->cbSize = 0;
	pwfx->wFormatTag = wf.wf.wFormatTag;
	pwfx->nChannels = wf.wf.nChannels;
	pwfx->nSamplesPerSec = wf.wf.nSamplesPerSec;
	pwfx->nAvgBytesPerSec = wf.wf.nAvgBytesPerSec;
	pwfx->nBlockAlign = wf.wf.nBlockAlign;
	pwfx->wBitsPerSample = wf.wBitsPerSample;
	if (chunk == NULL)
		return TRUE;
	return ReadWaveSection(pMemFile, MAKEFOURCC('d', 'a', 't', 'a'), chunk);
}

BOOL __fastcall ReadMemFile(MEMFILE *pMemFile, void *lpBuf, size_t length)
{
	while (length) {
		size_t to_copy;
		if (!pMemFile->bytes_to_read)
			FillMemFile(pMemFile);
		to_copy = pMemFile->bytes_to_read;
		if (length < to_copy)
			to_copy = length;
		if (!to_copy)
			return FALSE;
		memcpy(lpBuf, &pMemFile->buf[pMemFile->dist], to_copy);
		pMemFile->offset += to_copy;
		pMemFile->dist += to_copy;
		pMemFile->bytes_to_read -= to_copy;
		// BUGFIX: lpBuf is not incremented, next read will overwrite data
		length -= to_copy;
	}
	return TRUE;
}

void __fastcall FillMemFile(MEMFILE *pMemFile)
{
	DWORD to_read;
	WSetFilePointer(pMemFile->file, pMemFile->offset, NULL, FILE_BEGIN);
	to_read = pMemFile->end - pMemFile->offset;
	if (pMemFile->buf_len < to_read)
		to_read = pMemFile->buf_len;
	if (to_read)
		WReadFile(pMemFile->file, pMemFile->buf, to_read);
	pMemFile->dist = 0;
	pMemFile->bytes_to_read = to_read;
}

int __fastcall SeekMemFile(MEMFILE *pMemFile, LONG lDist, DWORD dwMethod)
{
	if (lDist < pMemFile->bytes_to_read) {
		pMemFile->bytes_to_read -= lDist;
		pMemFile->dist += lDist;
	} else
		pMemFile->bytes_to_read = 0;
	pMemFile->offset += lDist;
	return pMemFile->offset;
}

BOOL __fastcall ReadWaveSection(MEMFILE *pMemFile, DWORD id, CKINFO *chunk)
{
	DWORD hdr[2];

	while (1) {
		if (!ReadMemFile(pMemFile, hdr, sizeof(hdr)))
			return FALSE;
		if (hdr[0] == id)
			break;
		if (SeekMemFile(pMemFile, hdr[1], FILE_CURRENT) == -1)
			return FALSE;
	}

	chunk->dwSize = hdr[1];
	chunk->dwOffset = SeekMemFile(pMemFile, 0, FILE_CURRENT);
	return chunk->dwOffset != (DWORD)-1;
}

void *__fastcall LoadWaveFile(HANDLE hsFile, WAVEFORMATEX *pwfx, CKINFO *chunk)
{
	MEMFILE wave_file;

	AllocateMemFile(hsFile, &wave_file, (DWORD)-1);
	if (!ReadWaveFile(&wave_file, pwfx, chunk)) {
		FreeMemFile(&wave_file);
		return NULL;
	}
	return wave_file.buf;
}

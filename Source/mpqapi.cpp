#include <cstdint>
#include <fstream>

#include "diablo.h"
#include "../SourceS/file_util.h"
#include "../3rdParty/Storm/Source/storm.h"

DEVILUTION_BEGIN_NAMESPACE

DWORD sgdwMpqOffset;
char mpq_buf[4096];
_HASHENTRY *sgpHashTbl;
BOOL save_archive_modified;
_BLOCKENTRY *sgpBlockTbl;

//note: 32872 = 32768 + 104 (sizeof(_FILEHEADER))

/* data */

#define RETURN_IF_FAIL(obj, op, ...)                                                                        \
	obj->op(__VA_ARGS__);                                                                                   \
	if (obj->fail()) {                                                                                      \
		SDL_Log("%s->%s(%s) failed in %s at %s:%d", #obj, #op, #__VA_ARGS__, __func__, __FILE__, __LINE__); \
		return FALSE;                                                                                       \
	}

#define GOTO_IF_FAIL(label, obj, op, ...)                                                                   \
	obj->op(__VA_ARGS__);                                                                                   \
	if (obj->fail()) {                                                                                      \
		SDL_Log("%s->%s(%s) failed in %s at %s:%d", #obj, #op, #__VA_ARGS__, __func__, __FILE__, __LINE__); \
		goto label;                                                                                         \
	}

namespace {

static std::fstream *archive = nullptr;

} // namespace

void mpqapi_remove_hash_entry(const char *pszName)
{
	_HASHENTRY *pHashTbl;
	_BLOCKENTRY *blockEntry;
	int hIdx, block_offset, block_size;

	hIdx = FetchHandle(pszName);
	if (hIdx != -1) {
		pHashTbl = &sgpHashTbl[hIdx];
		blockEntry = &sgpBlockTbl[pHashTbl->block];
		pHashTbl->block = -2;
		block_offset = blockEntry->offset;
		block_size = blockEntry->sizealloc;
		memset(blockEntry, 0, sizeof(*blockEntry));
		mpqapi_alloc_block(block_offset, block_size);
		save_archive_modified = TRUE;
	}
}

void mpqapi_alloc_block(int block_offset, int block_size)
{
	_BLOCKENTRY *block;
	int i;

	block = sgpBlockTbl;
	i = 2048;
	while (i-- != 0) {
		if (block->offset && !block->flags && !block->sizefile) {
			if (block->offset + block->sizealloc == block_offset) {
				block_offset = block->offset;
				block_size += block->sizealloc;
				memset(block, 0, sizeof(_BLOCKENTRY));
				mpqapi_alloc_block(block_offset, block_size);
				return;
			}
			if (block_offset + block_size == block->offset) {
				block_size += block->sizealloc;
				memset(block, 0, sizeof(_BLOCKENTRY));
				mpqapi_alloc_block(block_offset, block_size);
				return;
			}
		}
		block++;
	}
	if (block_offset + block_size > sgdwMpqOffset) {
		app_fatal("MPQ free list error");
	}
	if (block_offset + block_size == sgdwMpqOffset) {
		sgdwMpqOffset = block_offset;
	} else {
		block = mpqapi_new_block(NULL);
		block->offset = block_offset;
		block->sizealloc = block_size;
		block->sizefile = 0;
		block->flags = 0;
	}
}

_BLOCKENTRY *mpqapi_new_block(int *block_index)
{
	_BLOCKENTRY *blockEntry;
	DWORD i;

	blockEntry = sgpBlockTbl;

	i = 0;
	while (blockEntry->offset || blockEntry->sizealloc || blockEntry->flags || blockEntry->sizefile) {
		i++;
		blockEntry++;
		if (i >= 2048) {
			app_fatal("Out of free block entries");
			return NULL;
		}
	}
	if (block_index)
		*block_index = i;

	return blockEntry;
}

int FetchHandle(const char *pszName)
{
	return mpqapi_get_hash_index(Hash(pszName, 0), Hash(pszName, 1), Hash(pszName, 2), 0);
}

int mpqapi_get_hash_index(short index, int hash_a, int hash_b, int locale)
{
	int idx, i;

	i = 2048;
	for (idx = index & 0x7FF; sgpHashTbl[idx].block != -1; idx = (idx + 1) & 0x7FF) {
		if (!i--)
			break;
		if (sgpHashTbl[idx].hashcheck[0] == hash_a && sgpHashTbl[idx].hashcheck[1] == hash_b && sgpHashTbl[idx].lcid == locale && sgpHashTbl[idx].block != -2)
			return idx;
	}

	return -1;
}

void mpqapi_remove_hash_entries(BOOL(*fnGetName)(DWORD, char *))
{
	DWORD dwIndex, i;
	char pszFileName[MAX_PATH];

	dwIndex = 1;
	for (i = fnGetName(0, pszFileName); i; i = fnGetName(dwIndex++, pszFileName)) {
		mpqapi_remove_hash_entry(pszFileName);
	}
}

BOOL mpqapi_write_file(const char *pszName, const BYTE *pbData, DWORD dwLen)
{
	_BLOCKENTRY *blockEntry;

	save_archive_modified = TRUE;
	mpqapi_remove_hash_entry(pszName);
	blockEntry = mpqapi_add_file(pszName, 0, 0);
	if (!mpqapi_write_file_contents(pszName, pbData, dwLen, blockEntry)) {
		mpqapi_remove_hash_entry(pszName);
		return FALSE;
	}
	return TRUE;
}

_BLOCKENTRY *mpqapi_add_file(const char *pszName, _BLOCKENTRY *pBlk, int block_index)
{
	DWORD h1, h2, h3;
	int i, hIdx;

	h1 = Hash(pszName, 0);
	h2 = Hash(pszName, 1);
	h3 = Hash(pszName, 2);
	if (mpqapi_get_hash_index(h1, h2, h3, 0) != -1)
		app_fatal("Hash collision between \"%s\" and existing file\n", pszName);
	hIdx = h1 & 0x7FF;
	i = 2048;
	while (i--) {
		if (sgpHashTbl[hIdx].block == -1 || sgpHashTbl[hIdx].block == -2)
			break;
		hIdx = (hIdx + 1) & 0x7FF;
	}
	if (i < 0)
		app_fatal("Out of hash space");
	if (!pBlk)
		pBlk = mpqapi_new_block(&block_index);

	sgpHashTbl[hIdx].hashcheck[0] = h2;
	sgpHashTbl[hIdx].hashcheck[1] = h3;
	sgpHashTbl[hIdx].lcid = 0;
	sgpHashTbl[hIdx].block = block_index;

	return pBlk;
}

BOOL mpqapi_write_file_contents(const char *pszName, const BYTE *pbData, DWORD dwLen, _BLOCKENTRY *pBlk)
{
	DWORD *sectoroffsettable;
	DWORD destsize, num_bytes, block_size, nNumberOfBytesToWrite;
	const BYTE *src;
	const char *tmp, *str_ptr;
	int i, j;

	str_ptr = pszName;
	src = pbData;
	while ((tmp = strchr(str_ptr, ':'))) {
		str_ptr = tmp + 1;
	}
	while ((tmp = strchr(str_ptr, '\\'))) {
		str_ptr = tmp + 1;
	}
	Hash(str_ptr, 3);
	num_bytes = (dwLen + 4095) >> 12;
	nNumberOfBytesToWrite = 4 * num_bytes + 4;
	pBlk->offset = mpqapi_find_free_block(dwLen + nNumberOfBytesToWrite, &pBlk->sizealloc);
	pBlk->sizefile = dwLen;
	pBlk->flags = 0x80000100;
	RETURN_IF_FAIL(archive, seekp, pBlk->offset);
	j = 0;
	const auto start_pos = archive->tellp();
	std::fstream::streampos end_pos;
	sectoroffsettable = NULL;
	while (dwLen != 0) {
		DWORD len;
		for (i = 0; i < 4096; i++)
			mpq_buf[i] -= 86;
		len = dwLen;
		if (dwLen >= 4096)
			len = 4096;
		memcpy(mpq_buf, src, len);
		src += len;
		len = PkwareCompress(mpq_buf, len);
		if (j == 0) {
			nNumberOfBytesToWrite = 4 * num_bytes + 4;
			sectoroffsettable = (DWORD *)DiabloAllocPtr(nNumberOfBytesToWrite);
			memset(sectoroffsettable, 0, nNumberOfBytesToWrite);
			GOTO_IF_FAIL(on_error, archive, write, reinterpret_cast<const char *>(sectoroffsettable), nNumberOfBytesToWrite);
		}
		sectoroffsettable[j] = SwapLE32(archive->tellp() - start_pos);
		GOTO_IF_FAIL(on_error, archive, write, mpq_buf, len);
		j++;
		if (dwLen > 4096)
			dwLen -= 4096;
		else
			dwLen = 0;
	}
	end_pos = archive->tellp();
	destsize = end_pos - start_pos;

	sectoroffsettable[j] = SwapLE32(destsize);
	GOTO_IF_FAIL(on_error, archive, seekp, start_pos);
	GOTO_IF_FAIL(on_error, archive, write, reinterpret_cast<const char *>(sectoroffsettable), nNumberOfBytesToWrite);
	GOTO_IF_FAIL(on_error, archive, seekp, end_pos);

	mem_free_dbg(sectoroffsettable);
	if (destsize < pBlk->sizealloc) {
		block_size = pBlk->sizealloc - destsize;
		if (block_size >= 1024) {
			pBlk->sizealloc = destsize;
			mpqapi_alloc_block(pBlk->sizealloc + pBlk->offset, block_size);
		}
	}
	return TRUE;
on_error:
	if (sectoroffsettable)
		mem_free_dbg(sectoroffsettable);
	return FALSE;
}

int mpqapi_find_free_block(int size, int *block_size)
{
	_BLOCKENTRY *pBlockTbl;
	int i, result;

	pBlockTbl = sgpBlockTbl;
	i = 2048;
	while (1) {
		i--;
		if (pBlockTbl->offset && !pBlockTbl->flags && !pBlockTbl->sizefile && (DWORD)pBlockTbl->sizealloc >= size)
			break;
		pBlockTbl++;
		if (!i) {
			*block_size = size;
			result = sgdwMpqOffset;
			sgdwMpqOffset += size;
			return result;
		}
	}

	result = pBlockTbl->offset;
	*block_size = size;
	pBlockTbl->offset += size;
	pBlockTbl->sizealloc -= size;

	if (!pBlockTbl->sizealloc)
		memset(pBlockTbl, 0, sizeof(*pBlockTbl));

	return result;
}

void mpqapi_rename(char *pszOld, char *pszNew)
{
	int index, block;
	_HASHENTRY *hashEntry;
	_BLOCKENTRY *blockEntry;

	index = FetchHandle(pszOld);
	if (index != -1) {
		hashEntry = &sgpHashTbl[index];
		block = hashEntry->block;
		blockEntry = &sgpBlockTbl[block];
		hashEntry->block = -2;
		mpqapi_add_file(pszNew, blockEntry, block);
		save_archive_modified = TRUE;
	}
}

BOOL mpqapi_has_file(const char *pszName)
{
	return FetchHandle(pszName) != -1;
}

BOOL OpenMPQ(const char *pszArchive, DWORD dwChar)
{
	DWORD dwFlagsAndAttributes;
	DWORD key;
	_FILEHEADER fhdr;

	InitHash();

	if (archive != nullptr) {
		delete archive;
		archive = nullptr;
	}
	const bool exists = FileExists(pszArchive);
	if (exists) {
		archive = new std::fstream(pszArchive, std::ios::in | std::ios::out | std::ios::binary);
	} else {
		archive = new std::fstream(pszArchive, std::ios::in | std::ios::out | std::ios::binary | std::ios::trunc);
	}
	if (archive->fail()) {
		SDL_Log("Failed to OpenMPQ at %s", pszArchive);
		delete archive;
		archive = nullptr;
		return FALSE;
	}
	save_archive_modified = !exists;
	if (sgpBlockTbl == NULL || sgpHashTbl == NULL) {
		memset(&fhdr, 0, sizeof(fhdr));
		if (ParseMPQHeader(&fhdr, &sgdwMpqOffset) == FALSE) {
			goto on_error;
		}
		sgpBlockTbl = (_BLOCKENTRY *)DiabloAllocPtr(0x8000);
		memset(sgpBlockTbl, 0, 0x8000);
		if (fhdr.blockcount) {
			GOTO_IF_FAIL(on_error, archive, seekg, 104);
			GOTO_IF_FAIL(on_error, archive, read, reinterpret_cast<char *>(sgpBlockTbl), 0x8000);
			key = Hash("(block table)", 3);
			Decrypt(sgpBlockTbl, 0x8000, key);
		}
		sgpHashTbl = (_HASHENTRY *)DiabloAllocPtr(0x8000);
		memset(sgpHashTbl, 255, 0x8000);
		if (fhdr.hashcount) {
			GOTO_IF_FAIL(on_error, archive, seekg, 32872);
			GOTO_IF_FAIL(on_error, archive, read, reinterpret_cast<char *>(sgpHashTbl), 0x8000);
			key = Hash("(hash table)", 3);
			Decrypt(sgpHashTbl, 0x8000, key);
		}
	}
	// Set output position to the end to ensure the file is not cleared if the MPQ
	// is closed without having anything written to it.
	if (exists)
		GOTO_IF_FAIL(on_error, archive, seekp, 0, std::ios::end);
	return TRUE;
on_error:
	CloseMPQ(pszArchive, TRUE, dwChar);
	return FALSE;
}

static void byteSwapHdr(_FILEHEADER *pHdr)
{
	pHdr->signature = SDL_SwapLE32(pHdr->signature);
	pHdr->headersize = SDL_SwapLE32(pHdr->headersize);
	pHdr->filesize = SDL_SwapLE32(pHdr->filesize);
	pHdr->version = SDL_SwapLE16(pHdr->version);
	pHdr->sectorsizeid = SDL_SwapLE16(pHdr->sectorsizeid);
	pHdr->hashoffset = SDL_SwapLE32(pHdr->hashoffset);
	pHdr->blockoffset = SDL_SwapLE32(pHdr->blockoffset);
	pHdr->hashcount = SDL_SwapLE32(pHdr->hashcount);
	pHdr->blockcount = SDL_SwapLE32(pHdr->blockcount);
}

BOOL ParseMPQHeader(_FILEHEADER *pHdr, DWORD *pdwNextFileStart)
{
	RETURN_IF_FAIL(archive, seekg, 0, std::ios::end);
	const std::uint32_t size = archive->tellg();
	*pdwNextFileStart = size;

	bool ok = size >= sizeof(*pHdr);
	if (ok) {
		RETURN_IF_FAIL(archive, seekg, 0);
		RETURN_IF_FAIL(archive, read, reinterpret_cast<char *>(pHdr), sizeof(*pHdr));
		byteSwapHdr(pHdr);
	}
	ok = ok && pHdr->signature == '\x1AQPM'
	    && pHdr->headersize == 32
	    && pHdr->version <= 0
	    && pHdr->sectorsizeid == 3
	    && pHdr->filesize == size
	    && pHdr->hashoffset == 32872
	    && pHdr->blockoffset == 104
	    && pHdr->hashcount == 2048
	    && pHdr->blockcount == 2048;
	if (!ok) {
		RETURN_IF_FAIL(archive, seekg, 0, std::ios::end);
		memset(pHdr, 0, sizeof(*pHdr));
		pHdr->signature = '\x1AQPM';
		pHdr->headersize = 32;
		pHdr->sectorsizeid = 3;
		pHdr->version = 0;
		*pdwNextFileStart = 0x10068;
		save_archive_modified = TRUE;
	}

	return TRUE;
}

void CloseMPQ(const char *pszArchive, BOOL bFree, DWORD dwChar)
{
	if (bFree) {
		MemFreeDbg(sgpBlockTbl);
		MemFreeDbg(sgpHashTbl);
	}
	if (archive != nullptr) {
		delete archive;
		archive = nullptr;
	}
	save_archive_modified = FALSE;
}

BOOL mpqapi_flush_and_close(const char *pszArchive, BOOL bFree, DWORD dwChar)
{
	BOOL ret = FALSE;
	if (archive == nullptr)
		ret = TRUE;
	else {
		ret = FALSE;
		if (!save_archive_modified)
			ret = TRUE;
		else if (mpqapi_can_seek() && WriteMPQHeader() && mpqapi_write_block_table()) {
			if (mpqapi_write_hash_table())
				ret = TRUE;
			else
				ret = FALSE;
		}
	}
	CloseMPQ(pszArchive, bFree, dwChar);
	if (ret && sgdwMpqOffset)
		ret = ResizeFile(pszArchive, sgdwMpqOffset);
	return ret;
}

BOOL WriteMPQHeader()
{
	_FILEHEADER fhdr;

	memset(&fhdr, 0, sizeof(fhdr));
	fhdr.signature = SDL_SwapLE32('\x1AQPM');
	fhdr.headersize = SDL_SwapLE32(32);
	fhdr.filesize = SDL_SwapLE32(sgdwMpqOffset);
	fhdr.version = SDL_SwapLE16(0);
	fhdr.sectorsizeid = SDL_SwapLE16(3);
	fhdr.hashoffset = SDL_SwapLE32(32872);
	fhdr.blockoffset = SDL_SwapLE32(104);
	fhdr.hashcount = SDL_SwapLE32(2048);
	fhdr.blockcount = SDL_SwapLE32(2048);

	RETURN_IF_FAIL(archive, seekp, 0);
	RETURN_IF_FAIL(archive, write, reinterpret_cast<const char *>(&fhdr), sizeof(fhdr));
	return TRUE;
}

BOOL mpqapi_write_block_table()
{
	RETURN_IF_FAIL(archive, seekp, 104);
	Encrypt(sgpBlockTbl, 0x8000, Hash("(block table)", 3));
	const BOOL success = [=]() {
		RETURN_IF_FAIL(archive, write, reinterpret_cast<const char *>(sgpBlockTbl), 0x8000);
		return TRUE;
	}();
	Decrypt(sgpBlockTbl, 0x8000, Hash("(block table)", 3));
	return success;
}

BOOL mpqapi_write_hash_table()
{
	RETURN_IF_FAIL(archive, seekp, 32872);
	Encrypt(sgpHashTbl, 0x8000, Hash("(hash table)", 3));
	const BOOL success = [=]() {
		RETURN_IF_FAIL(archive, write, reinterpret_cast<const char *>(sgpHashTbl), 0x8000);
		return TRUE;
	}();
	Decrypt(sgpHashTbl, 0x8000, Hash("(hash table)", 3));
	return success;
}

BOOL mpqapi_can_seek()
{
	RETURN_IF_FAIL(archive, seekp, sgdwMpqOffset);
	return TRUE;
}

DEVILUTION_END_NAMESPACE

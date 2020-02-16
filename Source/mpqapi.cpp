#include <cstdint>
#include <cstring>
#include <cerrno>
#include <fstream>
#include <memory>

#include "all.h"
#include "../SourceS/file_util.h"
#include "../3rdParty/Storm/Source/storm.h"

DEVILUTION_BEGIN_NAMESPACE

// #define FSTREAM_LOG_DEBUG(...) {}
#define FSTREAM_LOG_DEBUG(...) SDL_Log(__VA_ARGS__)

namespace {


const char *DirToString(std::ios::seekdir dir)
{
	switch (dir) {
	case std::ios::beg:
		return "std::ios::beg";
	case std::ios::end:
		return "std::ios::end";
	default:
		return "invalid";
	}
}

std::string OpenModeToString(std::ios::openmode mode)
{
	std::string result;
	if ((mode & std::ios::app) != 0)
		result.append("std::ios::app | ");
	if ((mode & std::ios::ate) != 0)
		result.append("std::ios::ate | ");
	if ((mode & std::ios::binary) != 0)
		result.append("std::ios::binary | ");
	if ((mode & std::ios::in) != 0)
		result.append("std::ios::in | ");
	if ((mode & std::ios::out) != 0)
		result.append("std::ios::out | ");
	if ((mode & std::ios::trunc) != 0)
		result.append("std::ios::trunc | ");
	if (!result.empty())
		result.resize(result.size() - 3);
	return result;
}

// Wraps fstream with error checks and logging.
#define FSTREAM_CHECK(fmt, ...)                                                 \
	if (s_->fail())                                                             \
		SDL_Log(fmt ": failed with \"%s\"", __VA_ARGS__, std::strerror(errno)); \
	else                                                                        \
		FSTREAM_LOG_DEBUG(fmt, __VA_ARGS__);                                    \
	return !s_->fail()

struct FStreamWrapper {
public:
	bool Open(const char *path, std::ios::openmode mode)
	{
		s_.reset(new std::fstream(path, mode));
		FSTREAM_CHECK("new std::fstream(\"%s\", %s)", path, OpenModeToString(mode).c_str());
	}

	void Close()
	{
		s_ = nullptr;
	}

	bool IsOpen() const
	{
		return s_ != nullptr;
	}

	bool seekg(std::streampos pos)
	{
		s_->seekg(pos);
		FSTREAM_CHECK("seekg(%ju)", static_cast<std::uintmax_t>(pos));
	}

	bool seekg(std::streamoff pos, std::ios::seekdir dir)
	{
		s_->seekg(pos, dir);
		FSTREAM_CHECK("seekg(%jd, %s)", static_cast<std::intmax_t>(pos), DirToString(dir));
	}

	bool seekp(std::streampos pos)
	{
		s_->seekp(pos);
		FSTREAM_CHECK("seekp(%ju)", static_cast<std::uintmax_t>(pos));
	}

	bool seekp(std::streamoff pos, std::ios::seekdir dir)
	{
		s_->seekp(pos, dir);
		FSTREAM_CHECK("seekp(%jd, %s)", static_cast<std::intmax_t>(pos), DirToString(dir));
	}

	bool tellg(std::streampos *result)
	{
		*result = s_->tellg();
		FSTREAM_CHECK("tellg() = %ju", static_cast<std::uintmax_t>(*result));
	}

	bool tellp(std::streampos *result)
	{
		*result = s_->tellp();
		FSTREAM_CHECK("tellp() = %ju", static_cast<std::uintmax_t>(*result));
	}

	bool write(const char *data, std::streamsize size)
	{
		s_->write(data, size);
		FSTREAM_CHECK("write(data, %ju)", static_cast<std::uintmax_t>(size));
	}

	bool read(char *out, std::streamsize size)
	{
		s_->read(out, size);
		FSTREAM_CHECK("read(out, %ju)", static_cast<std::uintmax_t>(size));
	}

private:
	std::unique_ptr<std::fstream> s_;
};

constexpr std::size_t kBlockEntrySize = 0x8000;
constexpr std::size_t kHashEntrySize = 0x8000;
constexpr std::ios::off_type kMpqBlockEntryOffset = sizeof(_FILEHEADER);
constexpr std::ios::off_type kMpqHashEntryOffset = kMpqBlockEntryOffset + kBlockEntrySize;

struct Archive {
	FStreamWrapper stream;
	std::string name;
	std::uintmax_t size;
	bool modified;
	bool exists;

	char mpq_buf[4096];
	_HASHENTRY *sgpHashTbl;
	_BLOCKENTRY *sgpBlockTbl;

	bool Open(const char *name)
	{
		Close();
		FSTREAM_LOG_DEBUG("Opening %s", name);
		exists = FileExists(name);
		std::ios::openmode mode = std::ios::in | std::ios::out | std::ios::binary;
		if (exists) {
			if (GetFileSize(name, &size)) {
				FSTREAM_LOG_DEBUG("GetFileSize(\"%s\") = %ju", name, size);
			} else {
				SDL_Log("GetFileSize(\"%s\") failed with \"%s\"", name, std::strerror(errno));
				return false;
			}
		} else {
			mode |= std::ios::trunc;
		}
		if (!stream.Open(name, mode)) {
			stream.Close();
			return false;
		}
		modified = !exists;

		this->name = name;
		return true;
	}

	bool Close(bool clear_tables = true)
	{
		if (!stream.IsOpen())
			return true;
		FSTREAM_LOG_DEBUG("Closing %s", name.c_str());

		bool result = true;
		if (modified && !(stream.seekp(0, std::ios::beg) && WriteHeaderAndTables()))
			result = false;
		stream.Close();
		if (modified && result && size != 0) {
			FSTREAM_LOG_DEBUG("ResizeFile(\"%s\", %ju)", name.c_str(), size);
			result = ResizeFile(name.c_str(), size);
		}
		name.clear();
		if (clear_tables) {
			std::free(sgpHashTbl);
			sgpHashTbl = nullptr;
			std::free(sgpBlockTbl);
			sgpBlockTbl = nullptr;
		}
		return result;
	}

	bool WriteHeaderAndTables() {
		return WriteHeader() && WriteBlockTable() && WriteHashTable();
	}

	~Archive()
	{
		Close();
	}

private:
	bool WriteHeader()
	{
		_FILEHEADER fhdr;

		memset(&fhdr, 0, sizeof(fhdr));
		fhdr.signature = SDL_SwapLE32('\x1AQPM');
		fhdr.headersize = SDL_SwapLE32(32);
		fhdr.filesize = SDL_SwapLE32(static_cast<std::uint32_t>(size));
		fhdr.version = SDL_SwapLE16(0);
		fhdr.sectorsizeid = SDL_SwapLE16(3);
		fhdr.hashoffset = SDL_SwapLE32(kMpqHashEntryOffset);
		fhdr.blockoffset = SDL_SwapLE32(kMpqBlockEntryOffset);
		fhdr.hashcount = SDL_SwapLE32(2048);
		fhdr.blockcount = SDL_SwapLE32(2048);

		if (!stream.write(reinterpret_cast<const char *>(&fhdr), sizeof(fhdr)))
			return false;
		return true;
	}

	bool WriteBlockTable()
	{
		Encrypt(sgpBlockTbl, kBlockEntrySize, Hash("(block table)", 3));
		const bool success = stream.write(reinterpret_cast<const char *>(sgpBlockTbl), kBlockEntrySize);
		Decrypt(sgpBlockTbl, kBlockEntrySize, Hash("(block table)", 3));
		return success;
	}

	bool WriteHashTable()
	{
		Encrypt(sgpHashTbl, kHashEntrySize, Hash("(hash table)", 3));
		const bool success = stream.write(reinterpret_cast<const char *>(sgpHashTbl), kHashEntrySize);
		Decrypt(sgpHashTbl, kHashEntrySize, Hash("(hash table)", 3));
		return success;
	}
};

Archive cur_archive;

void ByteSwapHdr(_FILEHEADER *hdr)
{
	hdr->signature = SDL_SwapLE32(hdr->signature);
	hdr->headersize = SDL_SwapLE32(hdr->headersize);
	hdr->filesize = SDL_SwapLE32(hdr->filesize);
	hdr->version = SDL_SwapLE16(hdr->version);
	hdr->sectorsizeid = SDL_SwapLE16(hdr->sectorsizeid);
	hdr->hashoffset = SDL_SwapLE32(hdr->hashoffset);
	hdr->blockoffset = SDL_SwapLE32(hdr->blockoffset);
	hdr->hashcount = SDL_SwapLE32(hdr->hashcount);
	hdr->blockcount = SDL_SwapLE32(hdr->blockcount);
}

void InitDefaultMpqHeader(Archive *archive, _FILEHEADER *hdr)
{
	std::memset(hdr, 0, sizeof(*hdr));
	hdr->signature = '\x1AQPM';
	hdr->headersize = 32;
	hdr->sectorsizeid = 3;
	hdr->version = 0;
	archive->size = kMpqHashEntryOffset + kHashEntrySize;
	archive->modified = true;
}

bool IsValidMPQHeader(const Archive &archive, _FILEHEADER *hdr)
{
	return hdr->signature == '\x1AQPM'
	    && hdr->headersize == 32
	    && hdr->version <= 0
	    && hdr->sectorsizeid == 3
	    && hdr->filesize == archive.size
	    && hdr->hashoffset == kMpqHashEntryOffset
	    && hdr->blockoffset == sizeof(_FILEHEADER)
	    && hdr->hashcount == 2048
	    && hdr->blockcount == 2048;
}

bool ReadMPQHeader(Archive *archive, _FILEHEADER *hdr)
{
	const bool has_hdr = archive->size >= sizeof(*hdr);
	if (has_hdr) {
		if (!archive->stream.read(reinterpret_cast<char *>(hdr), sizeof(*hdr)))
			return false;
		ByteSwapHdr(hdr);
	}
	if (!has_hdr || !IsValidMPQHeader(*archive, hdr)) {
		InitDefaultMpqHeader(archive, hdr);
	}
	return true;
}

} // namespace

void mpqapi_remove_hash_entry(const char *pszName)
{
	_HASHENTRY *pHashTbl;
	_BLOCKENTRY *blockEntry;
	int hIdx, block_offset, block_size;

	hIdx = FetchHandle(pszName);
	if (hIdx != -1) {
		pHashTbl = &cur_archive.sgpHashTbl[hIdx];
		blockEntry = &cur_archive.sgpBlockTbl[pHashTbl->block];
		pHashTbl->block = -2;
		block_offset = blockEntry->offset;
		block_size = blockEntry->sizealloc;
		memset(blockEntry, 0, sizeof(*blockEntry));
		mpqapi_alloc_block(block_offset, block_size);
		cur_archive.modified = true;
	}
}

void mpqapi_alloc_block(int block_offset, int block_size)
{
	_BLOCKENTRY *block;
	int i;

	block = cur_archive.sgpBlockTbl;
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
	if (block_offset + block_size > cur_archive.size) {
		app_fatal("MPQ free list error");
	}
	if (block_offset + block_size == cur_archive.size) {
		cur_archive.size = block_offset;
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

	blockEntry = cur_archive.sgpBlockTbl;

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
	for (idx = index & 0x7FF; cur_archive.sgpHashTbl[idx].block != -1; idx = (idx + 1) & 0x7FF) {
		if (!i--)
			break;
		if (cur_archive.sgpHashTbl[idx].hashcheck[0] == hash_a && cur_archive.sgpHashTbl[idx].hashcheck[1] == hash_b
		    && cur_archive.sgpHashTbl[idx].lcid == locale
		    && cur_archive.sgpHashTbl[idx].block != -2)
			return idx;
	}

	return -1;
}

void mpqapi_remove_hash_entries(BOOL (*fnGetName)(DWORD, char *))
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

	cur_archive.modified = true;
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
		if (cur_archive.sgpHashTbl[hIdx].block == -1 || cur_archive.sgpHashTbl[hIdx].block == -2)
			break;
		hIdx = (hIdx + 1) & 0x7FF;
	}
	if (i < 0)
		app_fatal("Out of hash space");
	if (!pBlk)
		pBlk = mpqapi_new_block(&block_index);

	cur_archive.sgpHashTbl[hIdx].hashcheck[0] = h2;
	cur_archive.sgpHashTbl[hIdx].hashcheck[1] = h3;
	cur_archive.sgpHashTbl[hIdx].lcid = 0;
	cur_archive.sgpHashTbl[hIdx].block = block_index;

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
	std::streampos start_pos, end_pos;
	if (!cur_archive.stream.seekp(pBlk->offset, std::ios::beg))
		goto on_error;
	j = 0;
	if (!cur_archive.stream.tellp(&start_pos))
		goto on_error;
	sectoroffsettable = NULL;
	while (dwLen != 0) {
		DWORD len;
		for (i = 0; i < 4096; i++)
			cur_archive.mpq_buf[i] -= 86;
		len = dwLen;
		if (dwLen >= 4096)
			len = 4096;
		memcpy(cur_archive.mpq_buf, src, len);
		src += len;
		len = PkwareCompress(cur_archive.mpq_buf, len);
		if (j == 0) {
			nNumberOfBytesToWrite = 4 * num_bytes + 4;
			sectoroffsettable = (DWORD *)DiabloAllocPtr(nNumberOfBytesToWrite);
			memset(sectoroffsettable, 0, nNumberOfBytesToWrite);
			if (!cur_archive.stream.write(reinterpret_cast<const char *>(sectoroffsettable), nNumberOfBytesToWrite))
				goto on_error;
		}
		if (!cur_archive.stream.tellp(&end_pos))
			goto on_error;
		sectoroffsettable[j] = SwapLE32(static_cast<std::uint32_t>(end_pos - start_pos));
		if (!cur_archive.stream.write(cur_archive.mpq_buf, len))
			goto on_error;
		j++;
		if (dwLen > 4096)
			dwLen -= 4096;
		else
			dwLen = 0;
	}
	if (!cur_archive.stream.tellp(&end_pos))
		goto on_error;
	destsize = end_pos - start_pos;

	sectoroffsettable[j] = SwapLE32(destsize);
	if (!cur_archive.stream.seekp(start_pos))
		goto on_error;
	if (!cur_archive.stream.write(reinterpret_cast<const char *>(sectoroffsettable), nNumberOfBytesToWrite))
		goto on_error;
	if (!cur_archive.stream.seekp(end_pos))
		goto on_error;

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

	pBlockTbl = cur_archive.sgpBlockTbl;
	i = 2048;
	while (1) {
		i--;
		if (pBlockTbl->offset && !pBlockTbl->flags && !pBlockTbl->sizefile && (DWORD)pBlockTbl->sizealloc >= size)
			break;
		pBlockTbl++;
		if (!i) {
			*block_size = size;
			result = cur_archive.size;
			cur_archive.size += size;
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
		hashEntry = &cur_archive.sgpHashTbl[index];
		block = hashEntry->block;
		blockEntry = &cur_archive.sgpBlockTbl[block];
		hashEntry->block = -2;
		mpqapi_add_file(pszNew, blockEntry, block);
		cur_archive.modified = true;
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

	if (!cur_archive.Open(pszArchive)) {
		return FALSE;
	}
	if (cur_archive.sgpBlockTbl == NULL || cur_archive.sgpHashTbl == NULL) {
		if (!cur_archive.exists) {
			InitDefaultMpqHeader(&cur_archive, &fhdr);
		} else if (!ReadMPQHeader(&cur_archive, &fhdr)) {
			goto on_error;
		}
		cur_archive.sgpBlockTbl = (_BLOCKENTRY *)DiabloAllocPtr(kBlockEntrySize);
		std::memset(cur_archive.sgpBlockTbl, 0, kBlockEntrySize);
		if (fhdr.blockcount) {
			if (!cur_archive.stream.read(reinterpret_cast<char *>(cur_archive.sgpBlockTbl), kBlockEntrySize))
				goto on_error;
			key = Hash("(block table)", 3);
			Decrypt(cur_archive.sgpBlockTbl, kBlockEntrySize, key);
		}
		cur_archive.sgpHashTbl = (_HASHENTRY *)DiabloAllocPtr(kHashEntrySize);
		std::memset(cur_archive.sgpHashTbl, 255, kHashEntrySize);
		if (fhdr.hashcount) {
			if (!cur_archive.stream.read(reinterpret_cast<char *>(cur_archive.sgpHashTbl), kHashEntrySize))
				goto on_error;
			key = Hash("(hash table)", 3);
			Decrypt(cur_archive.sgpHashTbl, kHashEntrySize, key);
		}

#ifndef __AMIGA__
		// Amiga currently cannot seekp beyond end-of-file, so we fill up the space.
		// The data is incorrect at this point, it will be overwritten on Close.
		// See https://github.com/bebbo/libnix/issues/30
		if (!cur_archive.exists)
			cur_archive.WriteHeaderAndTables();
#endif
	}
	return TRUE;
on_error:
	cur_archive.Close(/*clear_tables=*/true);
	return FALSE;
}

BOOL mpqapi_flush_and_close(const char *pszArchive, BOOL bFree, DWORD dwChar)
{
	return cur_archive.Close(/*clear_tables=*/bFree);
}

DEVILUTION_END_NAMESPACE

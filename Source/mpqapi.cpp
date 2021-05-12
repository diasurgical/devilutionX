/**
 * @file mpqapi.cpp
 *
 * Implementation of functions for creating and editing MPQ files.
 */
#include "mpqapi.h"

#include <cerrno>
#include <cinttypes>
#include <cstddef>
#include <cstdint>
#include <cstring>
#include <fstream>
#include <memory>
#include <type_traits>

#include "appfat.h"
#include "encrypt.h"
#include "engine.h"
#include "utils/file_util.h"
#include "utils/log.hpp"

namespace devilution {

#define INDEX_ENTRIES 2048

// Amiga cannot seekp beyond EOF.
// See https://github.com/bebbo/libnix/issues/30
#ifndef __AMIGA__
#define CAN_SEEKP_BEYOND_EOF
#endif

namespace {

// Validates that a Type is of a particular size and that its alignment is <= the size of the type.
// Done with templates so that error messages include actual size.
template <std::size_t A, std::size_t B>
struct assert_eq : std::true_type {
	static_assert(A == B, "A == B not satisfied");
};
template <std::size_t A, std::size_t B>
struct assert_lte : std::true_type {
	static_assert(A <= B, "A <= B not satisfied");
};
template <typename T, std::size_t S>
struct check_size : assert_eq<sizeof(T), S>, assert_lte<alignof(T), sizeof(T)> {
};

// Check sizes and alignments of the structs that we decrypt and encrypt.
// The decryption algorithm treats them as a stream of 32-bit uints, so the
// sizes must be exact as there cannot be any padding.
static_assert(check_size<_HASHENTRY, 4 * 4>::value, "sizeof(_HASHENTRY) == 4 * 4 && alignof(_HASHENTRY) <= 4 * 4 not satisfied");
static_assert(check_size<_BLOCKENTRY, 4 * 4>::value, "sizeof(_BLOCKENTRY) == 4 * 4 && alignof(_BLOCKENTRY) <= 4 * 4 not satisfied");

const char *DirToString(std::ios::seekdir dir)
{
	switch (dir) {
	case std::ios::beg:
		return "std::ios::beg";
	case std::ios::end:
		return "std::ios::end";
	case std::ios::cur:
		return "std::ios::cur";
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

struct FStreamWrapper {
public:
	bool Open(const char *path, std::ios::openmode mode)
	{
		s_ = CreateFileStream(path, mode);
		return CheckError("new std::fstream(\"%s\", %s)", path, OpenModeToString(mode).c_str());
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
		return CheckError("seekg(%" PRIuMAX ")", static_cast<std::uintmax_t>(pos));
	}

	bool seekg(std::streamoff pos, std::ios::seekdir dir)
	{
		s_->seekg(pos, dir);
		return CheckError("seekg(%" PRIdMAX ", %s)", static_cast<std::intmax_t>(pos), DirToString(dir));
	}

	bool seekp(std::streampos pos)
	{
		s_->seekp(pos);
		return CheckError("seekp(%" PRIuMAX ")", static_cast<std::uintmax_t>(pos));
	}

	bool seekp(std::streamoff pos, std::ios::seekdir dir)
	{
		s_->seekp(pos, dir);
		return CheckError("seekp(%" PRIdMAX ", %s)", static_cast<std::intmax_t>(pos), DirToString(dir));
	}

	bool tellg(std::streampos *result)
	{
		*result = s_->tellg();
		return CheckError("tellg() = %" PRIuMAX, static_cast<std::uintmax_t>(*result));
	}

	bool tellp(std::streampos *result)
	{
		*result = s_->tellp();
		return CheckError("tellp() = %" PRIuMAX, static_cast<std::uintmax_t>(*result));
	}

	bool write(const char *data, std::streamsize size)
	{
		s_->write(data, size);
		return CheckError("write(data, %" PRIuMAX ")", static_cast<std::uintmax_t>(size));
	}

	bool read(char *out, std::streamsize size)
	{
		s_->read(out, size);
		return CheckError("read(out, %" PRIuMAX ")", static_cast<std::uintmax_t>(size));
	}

private:
	template <typename... PrintFArgs>
	bool CheckError(const char *fmt, PrintFArgs... args)
	{
		if (s_->fail()) {
			std::string fmt_with_error = fmt;
			fmt_with_error.append(": failed with \"{}\"");
			const char *error_message = std::strerror(errno);
			if (error_message == nullptr)
				error_message = "";
			LogError(LogCategory::System, fmt_with_error.c_str(), args..., error_message);
#ifdef _DEBUG
		} else {
			LogVerbose(LogCategory::System, fmt, args...);
#endif
		}
		return !s_->fail();
	}

	std::unique_ptr<std::fstream> s_;
};

constexpr std::size_t BlockEntrySize = INDEX_ENTRIES * sizeof(_BLOCKENTRY);
constexpr std::size_t HashEntrySize = INDEX_ENTRIES * sizeof(_HASHENTRY);
constexpr std::ios::off_type MpqBlockEntryOffset = sizeof(_FILEHEADER);
constexpr std::ios::off_type MpqHashEntryOffset = MpqBlockEntryOffset + BlockEntrySize;

struct Archive {
	FStreamWrapper stream;
	std::string name;
	std::uintmax_t size;
	bool modified;
	bool exists;

#ifndef CAN_SEEKP_BEYOND_EOF
	std::streampos stream_begin;
#endif

	_HASHENTRY *sgpHashTbl;
	_BLOCKENTRY *sgpBlockTbl;

	bool Open(const char *name)
	{
		Close();
#ifdef _DEBUG
		Log("Opening {}", name);
#endif
		exists = FileExists(name);
		std::ios::openmode mode = std::ios::in | std::ios::out | std::ios::binary;
		if (exists) {
			if (GetFileSize(name, &size) == 0) {
				Log("GetFileSize(\"{}\") failed with \"{}\"", name, std::strerror(errno));
				return false;
			}
#ifdef _DEBUG
			Log("GetFileSize(\"{}\") = {}", name, size);
#endif
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
#ifdef _DEBUG
		Log("Closing {}", name);
#endif

		bool result = true;
		if (modified && !(stream.seekp(0, std::ios::beg) && WriteHeaderAndTables()))
			result = false;
		stream.Close();
		if (modified && result && size != 0) {
#ifdef _DEBUG
			Log("ResizeFile(\"{}\", {})", name, size);
#endif
			result = ResizeFile(name.c_str(), size);
		}
		name.clear();
		if (clear_tables) {
			delete[] sgpHashTbl;
			sgpHashTbl = nullptr;
			delete[] sgpBlockTbl;
			sgpBlockTbl = nullptr;
		}
		return result;
	}

	bool WriteHeaderAndTables()
	{
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
		fhdr.signature = SDL_SwapLE32(LoadLE32("MPQ\x1A"));
		fhdr.headersize = SDL_SwapLE32(32);
		fhdr.filesize = SDL_SwapLE32(static_cast<uint32_t>(size));
		fhdr.version = SDL_SwapLE16(0);
		fhdr.sectorsizeid = SDL_SwapLE16(3);
		fhdr.hashoffset = SDL_SwapLE32(static_cast<uint32_t>(MpqHashEntryOffset));
		fhdr.blockoffset = SDL_SwapLE32(static_cast<uint32_t>(MpqBlockEntryOffset));
		fhdr.hashcount = SDL_SwapLE32(INDEX_ENTRIES);
		fhdr.blockcount = SDL_SwapLE32(INDEX_ENTRIES);

		if (!stream.write(reinterpret_cast<const char *>(&fhdr), sizeof(fhdr)))
			return false;
		return true;
	}

	bool WriteBlockTable()
	{
		Encrypt((DWORD *)sgpBlockTbl, BlockEntrySize, Hash("(block table)", 3));
		const bool success = stream.write(reinterpret_cast<const char *>(sgpBlockTbl), BlockEntrySize);
		Decrypt((DWORD *)sgpBlockTbl, BlockEntrySize, Hash("(block table)", 3));
		return success;
	}

	bool WriteHashTable()
	{
		Encrypt((DWORD *)sgpHashTbl, HashEntrySize, Hash("(hash table)", 3));
		const bool success = stream.write(reinterpret_cast<const char *>(sgpHashTbl), HashEntrySize);
		Decrypt((DWORD *)sgpHashTbl, HashEntrySize, Hash("(hash table)", 3));
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
	hdr->signature = LoadLE32("MPQ\x1A");
	hdr->headersize = 32;
	hdr->sectorsizeid = 3;
	hdr->version = 0;
	archive->size = MpqHashEntryOffset + HashEntrySize;
	archive->modified = true;
}

bool IsValidMPQHeader(const Archive &archive, _FILEHEADER *hdr)
{
	return hdr->signature == LoadLE32("MPQ\x1A")
	    && hdr->headersize == 32
	    && hdr->version <= 0
	    && hdr->sectorsizeid == 3
	    && hdr->filesize == archive.size
	    && hdr->hashoffset == MpqHashEntryOffset
	    && hdr->blockoffset == sizeof(_FILEHEADER)
	    && hdr->hashcount == INDEX_ENTRIES
	    && hdr->blockcount == INDEX_ENTRIES;
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

static _BLOCKENTRY *mpqapi_new_block(int *block_index)
{
	_BLOCKENTRY *blockEntry = cur_archive.sgpBlockTbl;

	for (DWORD i = 0; i < INDEX_ENTRIES; i++, blockEntry++) {
		if (blockEntry->offset != 0)
			continue;
		if (blockEntry->sizealloc != 0)
			continue;
		if (blockEntry->flags != 0)
			continue;
		if (blockEntry->sizefile != 0)
			continue;

		if (block_index != nullptr)
			*block_index = i;

		return blockEntry;
	}

	app_fatal("Out of free block entries");
}

void mpqapi_alloc_block(uint32_t block_offset, uint32_t block_size)
{
	_BLOCKENTRY *block;
	int i;

	block = cur_archive.sgpBlockTbl;
	i = INDEX_ENTRIES;
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
		block = mpqapi_new_block(nullptr);
		block->offset = block_offset;
		block->sizealloc = block_size;
		block->sizefile = 0;
		block->flags = 0;
	}
}

int mpqapi_find_free_block(uint32_t size, uint32_t *block_size)
{
	int result;

	_BLOCKENTRY *pBlockTbl = cur_archive.sgpBlockTbl;
	for (int i = INDEX_ENTRIES; i--; pBlockTbl++) {
		if (pBlockTbl->offset == 0)
			continue;
		if (pBlockTbl->flags != 0)
			continue;
		if (pBlockTbl->sizefile != 0)
			continue;
		if ((DWORD)pBlockTbl->sizealloc < size)
			continue;

		result = pBlockTbl->offset;
		*block_size = size;
		pBlockTbl->offset += size;
		pBlockTbl->sizealloc -= size;

		if (pBlockTbl->sizealloc == 0)
			memset(pBlockTbl, 0, sizeof(*pBlockTbl));

		return result;
	}

	*block_size = size;
	result = cur_archive.size;
	cur_archive.size += size;
	return result;
}

static int mpqapi_get_hash_index(int index, uint32_t hash_a, uint32_t hash_b)
{
	DWORD idx, i;

	i = INDEX_ENTRIES;
	for (idx = index & 0x7FF; cur_archive.sgpHashTbl[idx].block != -1; idx = (idx + 1) & 0x7FF) {
		if (i-- == 0)
			break;
		if (cur_archive.sgpHashTbl[idx].hashcheck[0] != hash_a)
			continue;
		if (cur_archive.sgpHashTbl[idx].hashcheck[1] != hash_b)
			continue;
		if (cur_archive.sgpHashTbl[idx].block == -2)
			continue;

		return idx;
	}

	return -1;
}

static int FetchHandle(const char *pszName)
{
	return mpqapi_get_hash_index(Hash(pszName, 0), Hash(pszName, 1), Hash(pszName, 2));
}

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

void mpqapi_remove_hash_entries(bool (*fnGetName)(uint8_t, char *))
{
	DWORD dwIndex, i;
	char pszFileName[MAX_PATH];

	dwIndex = 1;
	for (i = fnGetName(0, pszFileName); i; i = fnGetName(dwIndex++, pszFileName)) {
		mpqapi_remove_hash_entry(pszFileName);
	}
}

static _BLOCKENTRY *mpqapi_add_file(const char *pszName, _BLOCKENTRY *pBlk, int block_index)
{
	DWORD h1, h2, h3;
	int i, hIdx;

	h1 = Hash(pszName, 0);
	h2 = Hash(pszName, 1);
	h3 = Hash(pszName, 2);
	if (mpqapi_get_hash_index(h1, h2, h3) != -1)
		app_fatal("Hash collision between \"%s\" and existing file\n", pszName);
	hIdx = h1 & 0x7FF;
	i = INDEX_ENTRIES;
	while (i--) {
		if (cur_archive.sgpHashTbl[hIdx].block == -1 || cur_archive.sgpHashTbl[hIdx].block == -2)
			break;
		hIdx = (hIdx + 1) & 0x7FF;
	}
	if (i < 0)
		app_fatal("Out of hash space");
	if (pBlk == nullptr)
		pBlk = mpqapi_new_block(&block_index);

	cur_archive.sgpHashTbl[hIdx].hashcheck[0] = h2;
	cur_archive.sgpHashTbl[hIdx].hashcheck[1] = h3;
	cur_archive.sgpHashTbl[hIdx].lcid = 0;
	cur_archive.sgpHashTbl[hIdx].block = block_index;

	return pBlk;
}

static bool mpqapi_write_file_contents(const char *pszName, const byte *pbData, size_t dwLen, _BLOCKENTRY *pBlk)
{
	const char *tmp;
	while ((tmp = strchr(pszName, ':')))
		pszName = tmp + 1;
	while ((tmp = strchr(pszName, '\\')))
		pszName = tmp + 1;
	Hash(pszName, 3);

	constexpr size_t sectorSize = 4096;
	const uint32_t num_sectors = (dwLen + (sectorSize - 1)) / sectorSize;
	const uint32_t offset_table_bytesize = sizeof(uint32_t) * (num_sectors + 1);
	pBlk->offset = mpqapi_find_free_block(dwLen + offset_table_bytesize, &pBlk->sizealloc);
	pBlk->sizefile = dwLen;
	pBlk->flags = 0x80000100;

	// We populate the table of sector offset while we write the data.
	// We can't pre-populate it because we don't know the compressed sector sizes yet.
	// First offset is the start of the first sector, last offset is the end of the last sector.
	std::unique_ptr<uint32_t[]> sectoroffsettable { new uint32_t[num_sectors + 1] };

#ifdef CAN_SEEKP_BEYOND_EOF
	if (!cur_archive.stream.seekp(pBlk->offset + offset_table_bytesize, std::ios::beg))
		return false;
#else
	// Ensure we do not seekp beyond EOF by filling the missing space.
	std::streampos stream_end;
	if (!cur_archive.stream.seekp(0, std::ios::end) || !cur_archive.stream.tellp(&stream_end))
		return false;
	const std::uintmax_t cur_size = stream_end - cur_archive.stream_begin;
	if (cur_size < pBlk->offset + offset_table_bytesize) {
		if (cur_size < pBlk->offset) {
			std::unique_ptr<char[]> filler { new char[pBlk->offset - cur_size] };
			if (!cur_archive.stream.write(filler.get(), pBlk->offset - cur_size))
				return false;
		}
		if (!cur_archive.stream.write(reinterpret_cast<const char *>(sectoroffsettable.get()), offset_table_bytesize))
			return false;
	} else {
		if (!cur_archive.stream.seekp(pBlk->offset + offset_table_bytesize, std::ios::beg))
			return false;
	}
#endif

	uint32_t destsize = offset_table_bytesize;
	byte mpq_buf[sectorSize];
	std::size_t cur_sector = 0;
	while (true) {
		uint32_t len = std::min(dwLen, sectorSize);
		memcpy(mpq_buf, pbData, len);
		pbData += len;
		len = PkwareCompress(mpq_buf, len);
		if (!cur_archive.stream.write((char *)mpq_buf, len))
			return false;
		sectoroffsettable[cur_sector++] = SDL_SwapLE32(destsize);
		destsize += len; // compressed length
		if (dwLen > sectorSize)
			dwLen -= sectorSize;
		else
			break;
	}

	sectoroffsettable[num_sectors] = SDL_SwapLE32(destsize);
	if (!cur_archive.stream.seekp(pBlk->offset, std::ios::beg))
		return false;
	if (!cur_archive.stream.write(reinterpret_cast<const char *>(sectoroffsettable.get()), offset_table_bytesize))
		return false;
	if (!cur_archive.stream.seekp(destsize - offset_table_bytesize, std::ios::cur))
		return false;

	if (destsize < pBlk->sizealloc) {
		const uint32_t block_size = pBlk->sizealloc - destsize;
		if (block_size >= 1024) {
			pBlk->sizealloc = destsize;
			mpqapi_alloc_block(pBlk->sizealloc + pBlk->offset, block_size);
		}
	}
	return true;
}

bool mpqapi_write_file(const char *pszName, const byte *pbData, size_t dwLen)
{
	_BLOCKENTRY *blockEntry;

	cur_archive.modified = true;
	mpqapi_remove_hash_entry(pszName);
	blockEntry = mpqapi_add_file(pszName, nullptr, 0);
	if (!mpqapi_write_file_contents(pszName, pbData, dwLen, blockEntry)) {
		mpqapi_remove_hash_entry(pszName);
		return false;
	}
	return true;
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

bool mpqapi_has_file(const char *pszName)
{
	return FetchHandle(pszName) != -1;
}

bool OpenMPQ(const char *pszArchive)
{
	DWORD key;
	_FILEHEADER fhdr;

	InitHash();

	if (!cur_archive.Open(pszArchive)) {
		return false;
	}
	if (cur_archive.sgpBlockTbl == nullptr || cur_archive.sgpHashTbl == nullptr) {
		if (!cur_archive.exists) {
			InitDefaultMpqHeader(&cur_archive, &fhdr);
		} else if (!ReadMPQHeader(&cur_archive, &fhdr)) {
			goto on_error;
		}
		cur_archive.sgpBlockTbl = new _BLOCKENTRY[BlockEntrySize / sizeof(_BLOCKENTRY)];
		std::memset(cur_archive.sgpBlockTbl, 0, BlockEntrySize);
		if (fhdr.blockcount) {
			if (!cur_archive.stream.read(reinterpret_cast<char *>(cur_archive.sgpBlockTbl), BlockEntrySize))
				goto on_error;
			key = Hash("(block table)", 3);
			Decrypt((DWORD *)cur_archive.sgpBlockTbl, BlockEntrySize, key);
		}
		cur_archive.sgpHashTbl = new _HASHENTRY[HashEntrySize / sizeof(_HASHENTRY)];
		std::memset(cur_archive.sgpHashTbl, 255, HashEntrySize);
		if (fhdr.hashcount) {
			if (!cur_archive.stream.read(reinterpret_cast<char *>(cur_archive.sgpHashTbl), HashEntrySize))
				goto on_error;
			key = Hash("(hash table)", 3);
			Decrypt((DWORD *)cur_archive.sgpHashTbl, HashEntrySize, key);
		}

#ifndef CAN_SEEKP_BEYOND_EOF
		if (!cur_archive.stream.seekp(0, std::ios::beg))
			goto on_error;

		// Memorize stream begin, we'll need it for calculations later.
		if (!cur_archive.stream.tellp(&cur_archive.stream_begin))
			goto on_error;

		// Write garbage header and tables because some platforms cannot `seekp` beyond EOF.
		// The data is incorrect at this point, it will be overwritten on Close.
		if (!cur_archive.exists)
			cur_archive.WriteHeaderAndTables();
#endif
	}
	return true;
on_error:
	cur_archive.Close(/*clear_tables=*/true);
	return false;
}

bool mpqapi_flush_and_close(bool bFree)
{
	return cur_archive.Close(/*clear_tables=*/bFree);
}

} // namespace devilution

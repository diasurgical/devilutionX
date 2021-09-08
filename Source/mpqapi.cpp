/**
 * @file mpqapi.cpp
 *
 * Implementation of functions for creating and editing MPQ files.
 */
#include "mpqapi.h"

#include <cerrno>
#include <cstddef>
#include <cstdint>
#include <cstring>
#include <fstream>
#include <memory>
#include <type_traits>
#include <vector>

#include "appfat.h"
#include "encrypt.h"
#include "engine.h"
#include "utils/endian.hpp"
#include "utils/file_util.h"
#include "utils/log.hpp"

namespace devilution {

#define INDEX_ENTRIES 2048

// Amiga cannot Seekp beyond EOF.
// See https://github.com/bebbo/libnix/issues/30
#ifndef __AMIGA__
#define CAN_SEEKP_BEYOND_EOF
#endif

namespace {

struct FileHeader {
	uint32_t signature;
	int headersize;
	uint32_t filesize;
	uint16_t version;
	int16_t sectorsizeid;
	int hashoffset;
	int blockoffset;
	int hashcount;
	int blockcount;
	uint8_t pad[72];
};

struct HashEntry {
	uint32_t hashcheck[2] = { UINT32_MAX, UINT32_MAX };
	uint32_t lcid = UINT32_MAX;
	int32_t block = -1;
};

struct BlockEntry {
	uint32_t offset = 0;
	uint32_t sizealloc = 0;
	uint32_t sizefile = 0;
	uint32_t flags = 0;
};

// Validates that a Type is of a particular size and that its alignment is <= the size of the type.
// Done with templates so that error messages include actual size.
template <std::size_t A, std::size_t B>
struct AssertEq : std::true_type {
	static_assert(A == B, "A == B not satisfied");
};
template <std::size_t A, std::size_t B>
struct AssertLte : std::true_type {
	static_assert(A <= B, "A <= B not satisfied");
};
template <typename T, std::size_t S>
struct CheckSize : AssertEq<sizeof(T), S>, AssertLte<alignof(T), sizeof(T)> {
};

// Check sizes and alignments of the structs that we decrypt and encrypt.
// The decryption algorithm treats them as a stream of 32-bit uints, so the
// sizes must be exact as there cannot be any padding.
static_assert(CheckSize<HashEntry, 4 * 4>::value, "sizeof(HashEntry) == 4 * 4 && alignof(HashEntry) <= 4 * 4 not satisfied");
static_assert(CheckSize<BlockEntry, 4 * 4>::value, "sizeof(BlockEntry) == 4 * 4 && alignof(BlockEntry) <= 4 * 4 not satisfied");

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
		return CheckError("new std::fstream(\"{}\", {})", path, OpenModeToString(mode).c_str());
	}

	void Close()
	{
		s_ = nullptr;
	}

	[[nodiscard]] bool IsOpen() const
	{
		return s_ != nullptr;
	}

	bool Seekp(std::streampos pos)
	{
		s_->seekp(pos);
		return CheckError("seekp({})", pos);
	}

	bool Seekp(std::streamoff pos, std::ios::seekdir dir)
	{
		s_->seekp(pos, dir);
		return CheckError("seekp({}, {})", pos, DirToString(dir));
	}

	bool Tellp(std::streampos *result)
	{
		*result = s_->tellp();
		return CheckError("tellp() = {}", *result);
	}

	bool Write(const char *data, std::streamsize size)
	{
		s_->write(data, size);
		return CheckError("write(data, {})", size);
	}

	bool Read(char *out, std::streamsize size)
	{
		s_->read(out, size);
		return CheckError("read(out, {})", size);
	}

private:
	template <typename... PrintFArgs>
	bool CheckError(const char *fmt, PrintFArgs... args)
	{
		if (s_->fail()) {
			std::string fmtWithError = fmt;
			fmtWithError.append(": failed with \"{}\"");
			const char *errorMessage = std::strerror(errno);
			if (errorMessage == nullptr)
				errorMessage = "";
			LogError(LogCategory::System, fmtWithError.c_str(), args..., errorMessage);
		} else {
			LogVerbose(LogCategory::System, fmt, args...);
		}
		return !s_->fail();
	}

	std::unique_ptr<std::fstream> s_;
};

constexpr std::size_t BlockEntrySize = INDEX_ENTRIES * sizeof(BlockEntry);
constexpr std::size_t HashEntrySize = INDEX_ENTRIES * sizeof(HashEntry);
constexpr std::ios::off_type MpqBlockEntryOffset = sizeof(FileHeader);
constexpr std::ios::off_type MpqHashEntryOffset = MpqBlockEntryOffset + BlockEntrySize;

struct TableCache {
	std::string name;
	std::vector<HashEntry> hashTbl;
	std::vector<BlockEntry> blockTbl;
};

TableCache Cache;

struct Archive {
	FStreamWrapper stream;
	std::string name;
	std::uintmax_t size;
	bool modified;
	bool exists;

#ifndef CAN_SEEKP_BEYOND_EOF
	std::streampos stream_begin;
#endif

	std::vector<HashEntry> hashTbl;
	std::vector<BlockEntry> blockTbl;

	bool Open(const char *path)
	{
		Close();
		LogDebug("Opening {}", path);
		exists = FileExists(path);
		std::ios::openmode mode = std::ios::in | std::ios::out | std::ios::binary;
		if (exists) {
			if (!GetFileSize(path, &size)) {
				Log(R"(GetFileSize("{}") failed with "{}")", path, std::strerror(errno));
				return false;
			}
			LogDebug("GetFileSize(\"{}\") = {}", path, size);
		} else {
			mode |= std::ios::trunc;
		}
		if (!stream.Open(path, mode)) {
			stream.Close();
			return false;
		}
		modified = !exists;

		name = path;
		return true;
	}

	bool Close(bool clearTables = true)
	{
		if (!stream.IsOpen())
			return true;
		LogDebug("Closing {}", name);

		bool result = true;
		if (modified && !(stream.Seekp(0, std::ios::beg) && WriteHeaderAndTables()))
			result = false;
		stream.Close();
		if (modified && result && size != 0) {
			LogDebug("ResizeFile(\"{}\", {})", name, size);
			result = ResizeFile(name.c_str(), size);
		}
		if (!clearTables) {
			Cache.name = std::move(name);
			Cache.hashTbl = std::move(hashTbl);
			Cache.blockTbl = std::move(blockTbl);
		} else {
			Cache.name.clear();
		}
		name.clear();
		hashTbl = {};
		blockTbl = {};
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
		FileHeader fhdr;

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

		return stream.Write(reinterpret_cast<const char *>(&fhdr), sizeof(fhdr));
	}

	bool WriteBlockTable()
	{
		Encrypt((DWORD *)blockTbl.data(), BlockEntrySize, Hash("(block table)", 3));
		const bool success = stream.Write(reinterpret_cast<const char *>(blockTbl.data()), BlockEntrySize);
		Decrypt((DWORD *)blockTbl.data(), BlockEntrySize, Hash("(block table)", 3));
		return success;
	}

	bool WriteHashTable()
	{
		Encrypt((DWORD *)hashTbl.data(), HashEntrySize, Hash("(hash table)", 3));
		const bool success = stream.Write(reinterpret_cast<const char *>(hashTbl.data()), HashEntrySize);
		Decrypt((DWORD *)hashTbl.data(), HashEntrySize, Hash("(hash table)", 3));
		return success;
	}
};

Archive cur_archive;

void ByteSwapHdr(FileHeader *hdr)
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

void InitDefaultMpqHeader(Archive *archive, FileHeader *hdr)
{
	std::memset(hdr, 0, sizeof(*hdr));
	hdr->signature = LoadLE32("MPQ\x1A");
	hdr->headersize = 32;
	hdr->sectorsizeid = 3;
	hdr->version = 0;
	archive->size = MpqHashEntryOffset + HashEntrySize;
	archive->modified = true;
}

bool IsValidMPQHeader(const Archive &archive, FileHeader *hdr)
{
	return hdr->signature == LoadLE32("MPQ\x1A")
	    && hdr->headersize == 32
	    && hdr->version <= 0
	    && hdr->sectorsizeid == 3
	    && hdr->filesize == archive.size
	    && hdr->hashoffset == MpqHashEntryOffset
	    && hdr->blockoffset == sizeof(FileHeader)
	    && hdr->hashcount == INDEX_ENTRIES
	    && hdr->blockcount == INDEX_ENTRIES;
}

bool ReadMPQHeader(Archive *archive, FileHeader *hdr)
{
	const bool hasHdr = archive->size >= sizeof(*hdr);
	if (hasHdr) {
		if (!archive->stream.Read(reinterpret_cast<char *>(hdr), sizeof(*hdr)))
			return false;
		ByteSwapHdr(hdr);
	}
	if (!hasHdr || !IsValidMPQHeader(*archive, hdr)) {
		InitDefaultMpqHeader(archive, hdr);
	}
	return true;
}

BlockEntry *NewBlock(int *blockIndex)
{
	for (int i = 0; i < INDEX_ENTRIES; i++) {
		BlockEntry *blockEntry = &cur_archive.blockTbl[i];
		if (blockEntry->offset != 0)
			continue;
		if (blockEntry->sizealloc != 0)
			continue;
		if (blockEntry->flags != 0)
			continue;
		if (blockEntry->sizefile != 0)
			continue;

		if (blockIndex != nullptr)
			*blockIndex = i;

		return blockEntry;
	}

	app_fatal("Out of free block entries");
}

void AllocBlock(uint32_t blockOffset, uint32_t blockSize)
{
	BlockEntry *block = &cur_archive.blockTbl[0];
	int i = INDEX_ENTRIES;
	while (i-- != 0) {
		if (block->offset != 0 && block->flags == 0 && block->sizefile == 0) {
			if (block->offset + block->sizealloc == blockOffset) {
				blockOffset = block->offset;
				blockSize += block->sizealloc;
				*block = {};
				AllocBlock(blockOffset, blockSize);
				return;
			}
			if (blockOffset + blockSize == block->offset) {
				blockSize += block->sizealloc;
				*block = {};
				AllocBlock(blockOffset, blockSize);
				return;
			}
		}
		block++;
	}
	if (blockOffset + blockSize > cur_archive.size) {
		app_fatal("MPQ free list error");
	}
	if (blockOffset + blockSize == cur_archive.size) {
		cur_archive.size = blockOffset;
	} else {
		block = NewBlock(nullptr);
		block->offset = blockOffset;
		block->sizealloc = blockSize;
		block->sizefile = 0;
		block->flags = 0;
	}
}

int FindFreeBlock(uint32_t size, uint32_t *blockSize)
{
	for (int i = 0; i < INDEX_ENTRIES; i++) {
		BlockEntry *block = &cur_archive.blockTbl[i];
		if (block->offset == 0)
			continue;
		if (block->flags != 0)
			continue;
		if (block->sizefile != 0)
			continue;
		if (block->sizealloc < size)
			continue;

		int result = block->offset;
		*blockSize = size;
		block->offset += size;
		block->sizealloc -= size;

		if (block->sizealloc == 0)
			*block = {};

		return result;
	}

	*blockSize = size;
	int result = cur_archive.size;
	cur_archive.size += size;
	return result;
}

int GetHashIndex(int index, uint32_t hashA, uint32_t hashB)
{
	int i = INDEX_ENTRIES;
	for (int idx = index & 0x7FF; cur_archive.hashTbl[idx].block != -1; idx = (idx + 1) & 0x7FF) {
		if (i-- == 0)
			break;
		if (cur_archive.hashTbl[idx].hashcheck[0] != hashA)
			continue;
		if (cur_archive.hashTbl[idx].hashcheck[1] != hashB)
			continue;
		if (cur_archive.hashTbl[idx].block == -2)
			continue;

		return idx;
	}

	return -1;
}

int FetchHandle(const char *pszName)
{
	return GetHashIndex(Hash(pszName, 0), Hash(pszName, 1), Hash(pszName, 2));
}

BlockEntry *AddFile(const char *pszName, BlockEntry *pBlk, int blockIndex)
{
	uint32_t h1 = Hash(pszName, 0);
	uint32_t h2 = Hash(pszName, 1);
	uint32_t h3 = Hash(pszName, 2);
	if (GetHashIndex(h1, h2, h3) != -1)
		app_fatal("Hash collision between \"%s\" and existing file\n", pszName);
	unsigned int hIdx = h1 & 0x7FF;

	bool hasSpace = false;
	for (int i = 0; i < INDEX_ENTRIES; i++) {
		if (cur_archive.hashTbl[hIdx].block == -1 || cur_archive.hashTbl[hIdx].block == -2) {
			hasSpace = true;
			break;
		}
		hIdx = (hIdx + 1) & 0x7FF;
	}
	if (!hasSpace)
		app_fatal("Out of hash space");

	if (pBlk == nullptr)
		pBlk = NewBlock(&blockIndex);

	cur_archive.hashTbl[hIdx].hashcheck[0] = h2;
	cur_archive.hashTbl[hIdx].hashcheck[1] = h3;
	cur_archive.hashTbl[hIdx].lcid = 0;
	cur_archive.hashTbl[hIdx].block = blockIndex;

	return pBlk;
}

bool WriteFileContents(const char *pszName, const byte *pbData, size_t dwLen, BlockEntry *pBlk)
{
	const char *tmp;
	while ((tmp = strchr(pszName, ':')) != nullptr)
		pszName = tmp + 1;
	while ((tmp = strchr(pszName, '\\')) != nullptr)
		pszName = tmp + 1;
	Hash(pszName, 3);

	constexpr size_t SectorSize = 4096;
	const uint32_t numSectors = (dwLen + (SectorSize - 1)) / SectorSize;
	const uint32_t offsetTableByteSize = sizeof(uint32_t) * (numSectors + 1);
	pBlk->offset = FindFreeBlock(dwLen + offsetTableByteSize, &pBlk->sizealloc);
	pBlk->sizefile = dwLen;
	pBlk->flags = 0x80000100;

	// We populate the table of sector offset while we write the data.
	// We can't pre-populate it because we don't know the compressed sector sizes yet.
	// First offset is the start of the first sector, last offset is the end of the last sector.
	std::unique_ptr<uint32_t[]> sectoroffsettable { new uint32_t[numSectors + 1] };

#ifdef CAN_SEEKP_BEYOND_EOF
	if (!cur_archive.stream.Seekp(pBlk->offset + offsetTableByteSize, std::ios::beg))
		return false;
#else
	// Ensure we do not Seekp beyond EOF by filling the missing space.
	std::streampos stream_end;
	if (!cur_archive.stream.Seekp(0, std::ios::end) || !cur_archive.stream.Tellp(&stream_end))
		return false;
	const std::uintmax_t cur_size = stream_end - cur_archive.stream_begin;
	if (cur_size < pBlk->offset + offsetTableByteSize) {
		if (cur_size < pBlk->offset) {
			std::unique_ptr<char[]> filler { new char[pBlk->offset - cur_size] };
			if (!cur_archive.stream.Write(filler.get(), pBlk->offset - cur_size))
				return false;
		}
		if (!cur_archive.stream.Write(reinterpret_cast<const char *>(sectoroffsettable.get()), offsetTableByteSize))
			return false;
	} else {
		if (!cur_archive.stream.Seekp(pBlk->offset + offsetTableByteSize, std::ios::beg))
			return false;
	}
#endif

	uint32_t destsize = offsetTableByteSize;
	byte mpqBuf[SectorSize];
	std::size_t curSector = 0;
	while (true) {
		uint32_t len = std::min(dwLen, SectorSize);
		memcpy(mpqBuf, pbData, len);
		pbData += len;
		len = PkwareCompress(mpqBuf, len);
		if (!cur_archive.stream.Write((char *)mpqBuf, len))
			return false;
		sectoroffsettable[curSector++] = SDL_SwapLE32(destsize);
		destsize += len; // compressed length
		if (dwLen > SectorSize)
			dwLen -= SectorSize;
		else
			break;
	}

	sectoroffsettable[numSectors] = SDL_SwapLE32(destsize);
	if (!cur_archive.stream.Seekp(pBlk->offset, std::ios::beg))
		return false;
	if (!cur_archive.stream.Write(reinterpret_cast<const char *>(sectoroffsettable.get()), offsetTableByteSize))
		return false;
	if (!cur_archive.stream.Seekp(destsize - offsetTableByteSize, std::ios::cur))
		return false;

	if (destsize < pBlk->sizealloc) {
		const uint32_t blockSize = pBlk->sizealloc - destsize;
		if (blockSize >= 1024) {
			pBlk->sizealloc = destsize;
			AllocBlock(pBlk->sizealloc + pBlk->offset, blockSize);
		}
	}
	return true;
}

} // namespace

void mpqapi_remove_hash_entry(const char *pszName)
{
	int hIdx = FetchHandle(pszName);
	if (hIdx == -1) {
		return;
	}

	HashEntry *pHashTbl = &cur_archive.hashTbl[hIdx];
	BlockEntry *blockEntry = &cur_archive.blockTbl[pHashTbl->block];
	pHashTbl->block = -2;
	int blockOffset = blockEntry->offset;
	int blockSize = blockEntry->sizealloc;
	*blockEntry = {};
	AllocBlock(blockOffset, blockSize);
	cur_archive.modified = true;
}

void mpqapi_remove_hash_entries(bool (*fnGetName)(uint8_t, char *))
{
	char pszFileName[MAX_PATH];

	for (uint8_t i = 0; fnGetName(i, pszFileName); i++) {
		mpqapi_remove_hash_entry(pszFileName);
	}
}

bool mpqapi_write_file(const char *pszName, const byte *pbData, size_t dwLen)
{
	BlockEntry *blockEntry;

	cur_archive.modified = true;
	mpqapi_remove_hash_entry(pszName);
	blockEntry = AddFile(pszName, nullptr, 0);
	if (!WriteFileContents(pszName, pbData, dwLen, blockEntry)) {
		mpqapi_remove_hash_entry(pszName);
		return false;
	}
	return true;
}

void mpqapi_rename(char *pszOld, char *pszNew)
{
	int index = FetchHandle(pszOld);
	if (index == -1) {
		return;
	}

	HashEntry *hashEntry = &cur_archive.hashTbl[index];
	int block = hashEntry->block;
	BlockEntry *blockEntry = &cur_archive.blockTbl[block];
	hashEntry->block = -2;
	AddFile(pszNew, blockEntry, block);
	cur_archive.modified = true;
}

bool mpqapi_has_file(const char *pszName)
{
	return FetchHandle(pszName) != -1;
}

bool OpenMPQ(const char *pszArchive)
{
	FileHeader fhdr;

	if (!cur_archive.Open(pszArchive)) {
		return false;
	}
	if (Cache.name == cur_archive.name) {
		cur_archive.hashTbl = std::move(Cache.hashTbl);
		cur_archive.blockTbl = std::move(Cache.blockTbl);
		Cache = {};
	} else {
		Cache = {};
		if (!cur_archive.exists) {
			InitDefaultMpqHeader(&cur_archive, &fhdr);
		} else if (!ReadMPQHeader(&cur_archive, &fhdr)) {
			goto on_error;
		}
		cur_archive.blockTbl.resize(INDEX_ENTRIES);
		if (fhdr.blockcount > 0) {
			if (!cur_archive.stream.Read(reinterpret_cast<char *>(cur_archive.blockTbl.data()), BlockEntrySize))
				goto on_error;
			uint32_t key = Hash("(block table)", 3);
			Decrypt((DWORD *)cur_archive.blockTbl.data(), BlockEntrySize, key);
		}
		cur_archive.hashTbl.resize(INDEX_ENTRIES);
		if (fhdr.hashcount > 0) {
			if (!cur_archive.stream.Read(reinterpret_cast<char *>(cur_archive.hashTbl.data()), HashEntrySize))
				goto on_error;
			uint32_t key = Hash("(hash table)", 3);
			Decrypt((DWORD *)cur_archive.hashTbl.data(), HashEntrySize, key);
		}

#ifndef CAN_SEEKP_BEYOND_EOF
		if (!cur_archive.stream.Seekp(0, std::ios::beg))
			goto on_error;

		// Memorize stream begin, we'll need it for calculations later.
		if (!cur_archive.stream.Tellp(&cur_archive.stream_begin))
			goto on_error;

		// Write garbage header and tables because some platforms cannot `Seekp` beyond EOF.
		// The data is incorrect at this point, it will be overwritten on Close.
		if (!cur_archive.exists)
			cur_archive.WriteHeaderAndTables();
#endif
	}
	return true;
on_error:
	cur_archive.Close(/*clearTables=*/true);
	return false;
}

bool mpqapi_flush_and_close(bool bFree)
{
	return cur_archive.Close(/*clearTables=*/bFree);
}

} // namespace devilution

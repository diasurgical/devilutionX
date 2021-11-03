/**
 * @file mpqapi.cpp
 *
 * Implementation of functions for creating and editing MPQ files.
 */
#include "utils/mpq_writer.hpp"

#include <cstddef>
#include <cstdint>
#include <cstring>
#include <memory>
#include <type_traits>

#include "appfat.h"
#include "encrypt.h"
#include "engine.h"
#include "utils/endian.hpp"
#include "utils/file_util.h"
#include "utils/log.hpp"

namespace devilution {

#define INDEX_ENTRIES 2048

namespace {

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
static_assert(CheckSize<_HASHENTRY, 4 * 4>::value, "sizeof(_HASHENTRY) == 4 * 4 && alignof(_HASHENTRY) <= 4 * 4 not satisfied");
static_assert(CheckSize<_BLOCKENTRY, 4 * 4>::value, "sizeof(_BLOCKENTRY) == 4 * 4 && alignof(_BLOCKENTRY) <= 4 * 4 not satisfied");

constexpr std::size_t BlockEntrySize = INDEX_ENTRIES * sizeof(_BLOCKENTRY);
constexpr std::size_t HashEntrySize = INDEX_ENTRIES * sizeof(_HASHENTRY);
constexpr std::ios::off_type MpqBlockEntryOffset = sizeof(_FILEHEADER);
constexpr std::ios::off_type MpqHashEntryOffset = MpqBlockEntryOffset + BlockEntrySize;

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

} // namespace

bool MpqWriter::Open(const char *path)
{
	Close(/*clearTables=*/false);
	LogDebug("Opening {}", path);
	exists_ = FileExists(path);
	std::ios::openmode mode = std::ios::in | std::ios::out | std::ios::binary;
	if (exists_) {
		if (!GetFileSize(path, &size_)) {
			Log(R"(GetFileSize("{}") failed with "{}")", path, std::strerror(errno));
			return false;
		}
		LogDebug("GetFileSize(\"{}\") = {}", path, size_);
	} else {
		mode |= std::ios::trunc;
	}
	if (!stream_.Open(path, mode)) {
		stream_.Close();
		return false;
	}
	modified_ = !exists_;

	name_ = path;

	if (blockTable_ == nullptr || hashTable_ == nullptr) {
		_FILEHEADER fhdr;
		if (!exists_) {
			InitDefaultMpqHeader(&fhdr);
		} else if (!ReadMPQHeader(&fhdr)) {
			goto on_error;
		}
		blockTable_ = new _BLOCKENTRY[BlockEntrySize / sizeof(_BLOCKENTRY)];
		std::memset(blockTable_, 0, BlockEntrySize);
		if (fhdr.blockcount > 0) {
			if (!stream_.Read(reinterpret_cast<char *>(blockTable_), BlockEntrySize))
				goto on_error;
			uint32_t key = Hash("(block table)", 3);
			Decrypt((DWORD *)blockTable_, BlockEntrySize, key);
		}
		hashTable_ = new _HASHENTRY[HashEntrySize / sizeof(_HASHENTRY)];
		std::memset(hashTable_, 255, HashEntrySize);
		if (fhdr.hashcount > 0) {
			if (!stream_.Read(reinterpret_cast<char *>(hashTable_), HashEntrySize))
				goto on_error;
			uint32_t key = Hash("(hash table)", 3);
			Decrypt((DWORD *)hashTable_, HashEntrySize, key);
		}

#ifndef CAN_SEEKP_BEYOND_EOF
		if (!stream_.Seekp(0, std::ios::beg))
			goto on_error;

		// Memorize stream begin, we'll need it for calculations later.
		if (!stream_.Tellp(&stream_begin))
			goto on_error;

		// Write garbage header and tables because some platforms cannot `Seekp` beyond EOF.
		// The data is incorrect at this point, it will be overwritten on Close.
		if (!exists_)
			WriteHeaderAndTables();
#endif
	}
	return true;
on_error:
	Close(/*clearTables=*/true);
	return false;
}

bool MpqWriter::Close(bool clearTables)
{
	if (!stream_.IsOpen())
		return true;
	LogDebug("Closing {}", name_);

	bool result = true;
	if (modified_ && !(stream_.Seekp(0, std::ios::beg) && WriteHeaderAndTables()))
		result = false;
	stream_.Close();
	if (modified_ && result && size_ != 0) {
		LogDebug("ResizeFile(\"{}\", {})", name_, size_);
		result = ResizeFile(name_.c_str(), size_);
	}
	name_.clear();
	if (clearTables) {
		delete[] hashTable_;
		hashTable_ = nullptr;
		delete[] blockTable_;
		blockTable_ = nullptr;
	}
	return result;
}

int MpqWriter::FetchHandle(const char *filename) const
{
	return GetHashIndex(Hash(filename, 0), Hash(filename, 1), Hash(filename, 2));
}

void MpqWriter::InitDefaultMpqHeader(_FILEHEADER *hdr)
{
	std::memset(hdr, 0, sizeof(*hdr));
	hdr->signature = LoadLE32("MPQ\x1A");
	hdr->headersize = 32;
	hdr->sectorsizeid = 3;
	hdr->version = 0;
	size_ = MpqHashEntryOffset + HashEntrySize;
	modified_ = true;
}

bool MpqWriter::IsValidMpqHeader(_FILEHEADER *hdr) const
{
	return hdr->signature == LoadLE32("MPQ\x1A")
	    && hdr->headersize == 32
	    && hdr->version <= 0
	    && hdr->sectorsizeid == 3
	    && hdr->filesize == size_
	    && hdr->hashoffset == MpqHashEntryOffset
	    && hdr->blockoffset == sizeof(_FILEHEADER)
	    && hdr->hashcount == INDEX_ENTRIES
	    && hdr->blockcount == INDEX_ENTRIES;
}

bool MpqWriter::ReadMPQHeader(_FILEHEADER *hdr)
{
	const bool hasHdr = size_ >= sizeof(*hdr);
	if (hasHdr) {
		if (!stream_.Read(reinterpret_cast<char *>(hdr), sizeof(*hdr)))
			return false;
		ByteSwapHdr(hdr);
	}
	if (!hasHdr || !IsValidMpqHeader(hdr)) {
		InitDefaultMpqHeader(hdr);
	}
	return true;
}

_BLOCKENTRY *MpqWriter::NewBlock(int *blockIndex)
{
	_BLOCKENTRY *blockEntry = blockTable_;

	for (int i = 0; i < INDEX_ENTRIES; i++, blockEntry++) {
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

void MpqWriter::AllocBlock(uint32_t blockOffset, uint32_t blockSize)
{
	_BLOCKENTRY *block;
	int i;

	block = blockTable_;
	i = INDEX_ENTRIES;
	while (i-- != 0) {
		if (block->offset != 0 && block->flags == 0 && block->sizefile == 0) {
			if (block->offset + block->sizealloc == blockOffset) {
				blockOffset = block->offset;
				blockSize += block->sizealloc;
				memset(block, 0, sizeof(_BLOCKENTRY));
				AllocBlock(blockOffset, blockSize);
				return;
			}
			if (blockOffset + blockSize == block->offset) {
				blockSize += block->sizealloc;
				memset(block, 0, sizeof(_BLOCKENTRY));
				AllocBlock(blockOffset, blockSize);
				return;
			}
		}
		block++;
	}
	if (blockOffset + blockSize > size_) {
		app_fatal("MPQ free list error");
	}
	if (blockOffset + blockSize == size_) {
		size_ = blockOffset;
	} else {
		block = NewBlock(nullptr);
		block->offset = blockOffset;
		block->sizealloc = blockSize;
		block->sizefile = 0;
		block->flags = 0;
	}
}

int MpqWriter::FindFreeBlock(uint32_t size, uint32_t *blockSize)
{
	int result;

	_BLOCKENTRY *pBlockTbl = blockTable_;
	for (int i = 0; i < INDEX_ENTRIES; i++, pBlockTbl++) {
		if (pBlockTbl->offset == 0)
			continue;
		if (pBlockTbl->flags != 0)
			continue;
		if (pBlockTbl->sizefile != 0)
			continue;
		if (pBlockTbl->sizealloc < size)
			continue;

		result = pBlockTbl->offset;
		*blockSize = size;
		pBlockTbl->offset += size;
		pBlockTbl->sizealloc -= size;

		if (pBlockTbl->sizealloc == 0)
			memset(pBlockTbl, 0, sizeof(*pBlockTbl));

		return result;
	}

	*blockSize = size;
	result = size_;
	size_ += size;
	return result;
}

int MpqWriter::GetHashIndex(int index, uint32_t hashA, uint32_t hashB) const
{
	int i = INDEX_ENTRIES;
	for (int idx = index & 0x7FF; hashTable_[idx].block != -1; idx = (idx + 1) & 0x7FF) {
		if (i-- == 0)
			break;
		if (hashTable_[idx].hashcheck[0] != hashA)
			continue;
		if (hashTable_[idx].hashcheck[1] != hashB)
			continue;
		if (hashTable_[idx].block == -2)
			continue;

		return idx;
	}

	return -1;
}

bool MpqWriter::WriteHeaderAndTables()
{
	return WriteHeader() && WriteBlockTable() && WriteHashTable();
}

_BLOCKENTRY *MpqWriter::AddFile(const char *pszName, _BLOCKENTRY *pBlk, int blockIndex)
{
	uint32_t h1 = Hash(pszName, 0);
	uint32_t h2 = Hash(pszName, 1);
	uint32_t h3 = Hash(pszName, 2);
	if (GetHashIndex(h1, h2, h3) != -1)
		app_fatal("Hash collision between \"%s\" and existing file\n", pszName);
	unsigned int hIdx = h1 & 0x7FF;

	bool hasSpace = false;
	for (int i = 0; i < INDEX_ENTRIES; i++) {
		if (hashTable_[hIdx].block == -1 || hashTable_[hIdx].block == -2) {
			hasSpace = true;
			break;
		}
		hIdx = (hIdx + 1) & 0x7FF;
	}
	if (!hasSpace)
		app_fatal("Out of hash space");

	if (pBlk == nullptr)
		pBlk = NewBlock(&blockIndex);

	hashTable_[hIdx].hashcheck[0] = h2;
	hashTable_[hIdx].hashcheck[1] = h3;
	hashTable_[hIdx].lcid = 0;
	hashTable_[hIdx].block = blockIndex;

	return pBlk;
}

bool MpqWriter::WriteFileContents(const char *pszName, const byte *pbData, size_t dwLen, _BLOCKENTRY *pBlk)
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
	if (!stream_.Seekp(pBlk->offset + offsetTableByteSize, std::ios::beg))
		return false;
#else
	// Ensure we do not Seekp beyond EOF by filling the missing space.
	std::streampos stream_end;
	if (!stream_.Seekp(0, std::ios::end) || !stream_.Tellp(&stream_end))
		return false;
	const std::uintmax_t cur_size = stream_end - stream_begin;
	if (cur_size < pBlk->offset + offsetTableByteSize) {
		if (cur_size < pBlk->offset) {
			std::unique_ptr<char[]> filler { new char[pBlk->offset - cur_size] };
			if (!stream_.Write(filler.get(), pBlk->offset - cur_size))
				return false;
		}
		if (!stream_.Write(reinterpret_cast<const char *>(sectoroffsettable.get()), offsetTableByteSize))
			return false;
	} else {
		if (!stream_.Seekp(pBlk->offset + offsetTableByteSize, std::ios::beg))
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
		if (!stream_.Write(reinterpret_cast<const char *>(&mpqBuf[0]), len))
			return false;
		sectoroffsettable[curSector++] = SDL_SwapLE32(destsize);
		destsize += len; // compressed length
		if (dwLen <= SectorSize)
			break;

		dwLen -= SectorSize;
	}

	sectoroffsettable[numSectors] = SDL_SwapLE32(destsize);
	if (!stream_.Seekp(pBlk->offset, std::ios::beg))
		return false;
	if (!stream_.Write(reinterpret_cast<const char *>(sectoroffsettable.get()), offsetTableByteSize))
		return false;
	if (!stream_.Seekp(destsize - offsetTableByteSize, std::ios::cur))
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

bool MpqWriter::WriteHeader()
{
	_FILEHEADER fhdr;

	memset(&fhdr, 0, sizeof(fhdr));
	fhdr.signature = SDL_SwapLE32(LoadLE32("MPQ\x1A"));
	fhdr.headersize = SDL_SwapLE32(32);
	fhdr.filesize = SDL_SwapLE32(static_cast<uint32_t>(size_));
	fhdr.version = SDL_SwapLE16(0);
	fhdr.sectorsizeid = SDL_SwapLE16(3);
	fhdr.hashoffset = SDL_SwapLE32(static_cast<uint32_t>(MpqHashEntryOffset));
	fhdr.blockoffset = SDL_SwapLE32(static_cast<uint32_t>(MpqBlockEntryOffset));
	fhdr.hashcount = SDL_SwapLE32(INDEX_ENTRIES);
	fhdr.blockcount = SDL_SwapLE32(INDEX_ENTRIES);

	return stream_.Write(reinterpret_cast<const char *>(&fhdr), sizeof(fhdr));
}

bool MpqWriter::WriteBlockTable()
{
	Encrypt((DWORD *)blockTable_, BlockEntrySize, Hash("(block table)", 3));
	const bool success = stream_.Write(reinterpret_cast<const char *>(blockTable_), BlockEntrySize);
	Decrypt((DWORD *)blockTable_, BlockEntrySize, Hash("(block table)", 3));
	return success;
}

bool MpqWriter::WriteHashTable()
{
	Encrypt((DWORD *)hashTable_, HashEntrySize, Hash("(hash table)", 3));
	const bool success = stream_.Write(reinterpret_cast<const char *>(hashTable_), HashEntrySize);
	Decrypt((DWORD *)hashTable_, HashEntrySize, Hash("(hash table)", 3));
	return success;
}

void MpqWriter::RemoveHashEntry(const char *filename)
{
	int hIdx = FetchHandle(filename);
	if (hIdx == -1) {
		return;
	}

	_HASHENTRY *pHashTbl = &hashTable_[hIdx];
	_BLOCKENTRY *blockEntry = &blockTable_[pHashTbl->block];
	pHashTbl->block = -2;
	int blockOffset = blockEntry->offset;
	int blockSize = blockEntry->sizealloc;
	memset(blockEntry, 0, sizeof(*blockEntry));
	AllocBlock(blockOffset, blockSize);
	modified_ = true;
}

void MpqWriter::RemoveHashEntries(bool (*fnGetName)(uint8_t, char *))
{
	char pszFileName[MAX_PATH];

	for (uint8_t i = 0; fnGetName(i, pszFileName); i++) {
		RemoveHashEntry(pszFileName);
	}
}

bool MpqWriter::WriteFile(const char *filename, const byte *data, size_t size)
{
	_BLOCKENTRY *blockEntry;

	modified_ = true;
	RemoveHashEntry(filename);
	blockEntry = AddFile(filename, nullptr, 0);
	if (!WriteFileContents(filename, data, size, blockEntry)) {
		RemoveHashEntry(filename);
		return false;
	}
	return true;
}

void MpqWriter::RenameFile(const char *name, const char *newName)
{
	int index = FetchHandle(name);
	if (index == -1) {
		return;
	}

	_HASHENTRY *hashEntry = &hashTable_[index];
	int block = hashEntry->block;
	_BLOCKENTRY *blockEntry = &blockTable_[block];
	hashEntry->block = -2;
	AddFile(newName, blockEntry, block);
	modified_ = true;
}

bool MpqWriter::HasFile(const char *name) const
{
	return FetchHandle(name) != -1;
}

} // namespace devilution

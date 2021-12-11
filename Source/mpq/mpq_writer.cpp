#include "mpq/mpq_writer.hpp"

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

namespace {

// Validates that a Type is of a particular size and that its alignment is <= the size of the type.
// Done with templates so that error messages include actual size.
template <size_t A, size_t B>
struct AssertEq : std::true_type {
	static_assert(A == B, "A == B not satisfied");
};
template <size_t A, size_t B>
struct AssertLte : std::true_type {
	static_assert(A <= B, "A <= B not satisfied");
};
template <typename T, size_t S>
struct CheckSize : AssertEq<sizeof(T), S>, AssertLte<alignof(T), sizeof(T)> {
};

// Check sizes and alignments of the structs that we decrypt and encrypt.
// The decryption algorithm treats them as a stream of 32-bit uints, so the
// sizes must be exact as there cannot be any padding.
static_assert(CheckSize<MpqHashEntry, static_cast<size_t>(4 * 4)>::value, "sizeof(MpqHashEntry) == 4 * 4 && alignof(MpqHashEntry) <= 4 * 4 not satisfied");
static_assert(CheckSize<MpqBlockEntry, static_cast<size_t>(4 * 4)>::value, "sizeof(MpqBlockEntry) == 4 * 4 && alignof(MpqBlockEntry) <= 4 * 4 not satisfied");

// We use fixed size block and hash entry tables.
constexpr uint32_t HashEntriesCount = 2048;
constexpr uint32_t BlockEntriesCount = 2048;
constexpr uint32_t BlockEntrySize = HashEntriesCount * sizeof(MpqBlockEntry);
constexpr uint32_t HashEntrySize = BlockEntriesCount * sizeof(MpqHashEntry);

// We store the block and the hash entry tables immediately after the header.
// This is unlike most other MPQ archives, that store these at the end of the file.
constexpr std::ios::off_type MpqBlockEntryOffset = sizeof(MpqFileHeader);
constexpr std::ios::off_type MpqHashEntryOffset = MpqBlockEntryOffset + BlockEntrySize;

// Special return value for `GetHashIndex` and `GetHandle`.
constexpr uint32_t HashEntryNotFound = -1;

// We use 4096-byte blocks, generally.
constexpr uint16_t BlockSizeFactor = 3;
constexpr uint32_t BlockSize = 512 << BlockSizeFactor; // 4096

// Sometimes we can end up with smaller blocks.
constexpr uint32_t MinBlockSize = 1024;

void ByteSwapHdr(MpqFileHeader *hdr)
{
	hdr->signature = SDL_SwapLE32(hdr->signature);
	hdr->headerSize = SDL_SwapLE32(hdr->headerSize);
	hdr->fileSize = SDL_SwapLE32(hdr->fileSize);
	hdr->version = SDL_SwapLE16(hdr->version);
	hdr->blockSizeFactor = SDL_SwapLE16(hdr->blockSizeFactor);
	hdr->hashEntriesOffset = SDL_SwapLE32(hdr->hashEntriesOffset);
	hdr->blockEntriesOffset = SDL_SwapLE32(hdr->blockEntriesOffset);
	hdr->hashEntriesCount = SDL_SwapLE32(hdr->hashEntriesCount);
	hdr->blockEntriesCount = SDL_SwapLE32(hdr->blockEntriesCount);
}

bool IsAllocatedUnusedBlock(const MpqBlockEntry *block)
{
	return block->offset != 0 && block->flags == 0 && block->unpackedSize == 0;
}

bool IsUnallocatedBlock(const MpqBlockEntry *block)
{
	return block->offset == 0 && block->packedSize == 0 && block->unpackedSize == 0 && block->flags == 0;
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
		MpqFileHeader fhdr;
		if (!exists_) {
			InitDefaultMpqHeader(&fhdr);
		} else if (!ReadMPQHeader(&fhdr)) {
			goto on_error;
		}
		blockTable_ = new MpqBlockEntry[BlockEntriesCount];
		std::memset(blockTable_, 0, BlockEntriesCount * sizeof(MpqBlockEntry));
		if (fhdr.blockEntriesCount > 0) {
			if (!stream_.Read(reinterpret_cast<char *>(blockTable_), static_cast<std::streamsize>(fhdr.blockEntriesCount * sizeof(MpqBlockEntry))))
				goto on_error;
			uint32_t key = Hash("(block table)", 3);
			Decrypt(reinterpret_cast<uint32_t *>(blockTable_), fhdr.blockEntriesCount * sizeof(MpqBlockEntry), key);
		}
		hashTable_ = new MpqHashEntry[HashEntriesCount];

		// We fill with 0xFF so that the `block` field defaults to -1 (a null block pointer).
		std::memset(hashTable_, 0xFF, HashEntriesCount * sizeof(MpqHashEntry));

		if (fhdr.hashEntriesCount > 0) {
			if (!stream_.Read(reinterpret_cast<char *>(hashTable_), static_cast<std::streamsize>(fhdr.hashEntriesCount * sizeof(MpqHashEntry))))
				goto on_error;
			uint32_t key = Hash("(hash table)", 3);
			Decrypt(reinterpret_cast<uint32_t *>(hashTable_), fhdr.hashEntriesCount * sizeof(MpqHashEntry), key);
		}

#ifndef CAN_SEEKP_BEYOND_EOF
		if (!stream_.Seekp(0, std::ios::beg))
			goto on_error;

		// Memorize stream begin, we'll need it for calculations later.
		if (!stream_.Tellp(&streamBegin_))
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

uint32_t MpqWriter::FetchHandle(const char *filename) const
{
	return GetHashIndex(Hash(filename, 0), Hash(filename, 1), Hash(filename, 2));
}

void MpqWriter::InitDefaultMpqHeader(MpqFileHeader *hdr)
{
	std::memset(hdr, 0, sizeof(*hdr));
	hdr->signature = MpqFileHeader::DiabloSignature;
	hdr->headerSize = MpqFileHeader::DiabloSize;
	hdr->blockSizeFactor = BlockSizeFactor;
	hdr->version = 0;
	size_ = MpqHashEntryOffset + HashEntrySize;
	modified_ = true;
}

bool MpqWriter::IsValidMpqHeader(MpqFileHeader *hdr) const
{
	return hdr->signature == MpqFileHeader::DiabloSignature
	    && hdr->headerSize == MpqFileHeader::DiabloSize
	    && hdr->version <= 0
	    && hdr->blockSizeFactor == BlockSizeFactor
	    && hdr->fileSize == size_
	    && hdr->hashEntriesOffset == MpqHashEntryOffset
	    && hdr->blockEntriesOffset == sizeof(MpqFileHeader)
	    && hdr->hashEntriesCount == HashEntriesCount
	    && hdr->blockEntriesCount == BlockEntriesCount;
}

bool MpqWriter::ReadMPQHeader(MpqFileHeader *hdr)
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

MpqBlockEntry *MpqWriter::NewBlock(uint32_t *blockIndex)
{
	MpqBlockEntry *blockEntry = blockTable_;

	for (unsigned i = 0; i < BlockEntriesCount; ++i, ++blockEntry) {
		if (!IsUnallocatedBlock(blockEntry))
			continue;

		if (blockIndex != nullptr)
			*blockIndex = i;

		return blockEntry;
	}

	app_fatal("Out of free block entries");
}

void MpqWriter::AllocBlock(uint32_t blockOffset, uint32_t blockSize)
{
	MpqBlockEntry *block;
	bool expand;
	do {
		block = blockTable_;
		expand = false;
		for (unsigned i = BlockEntriesCount; i-- != 0; ++block) {
			// Expand to adjacent blocks.
			if (!IsAllocatedUnusedBlock(block))
				continue;
			if (block->offset + block->packedSize == blockOffset) {
				blockOffset = block->offset;
				blockSize += block->packedSize;
				memset(block, 0, sizeof(MpqBlockEntry));
				expand = true;
				break;
			}
			if (blockOffset + blockSize == block->offset) {
				blockSize += block->packedSize;
				memset(block, 0, sizeof(MpqBlockEntry));
				expand = true;
				break;
			}
		}
	} while (expand);
	if (blockOffset + blockSize > size_) {
		// Expanded beyond EOF, this should never happen.
		app_fatal("MPQ free list error");
	}
	if (blockOffset + blockSize == size_) {
		size_ = blockOffset;
	} else {
		block = NewBlock();
		block->offset = blockOffset;
		block->packedSize = blockSize;
		block->unpackedSize = 0;
		block->flags = 0;
	}
}

uint32_t MpqWriter::FindFreeBlock(uint32_t size)
{
	uint32_t result;

	MpqBlockEntry *block = blockTable_;
	for (unsigned i = 0; i < BlockEntriesCount; ++i, ++block) {
		// Find a block entry to use space from.
		if (!IsAllocatedUnusedBlock(block) || block->packedSize < size)
			continue;

		result = block->offset;
		block->offset += size;
		block->packedSize -= size;

		// Clear the block entry if we used its entire capacity.
		if (block->packedSize == 0)
			memset(block, 0, sizeof(*block));

		return result;
	}

	result = size_;
	size_ += size;
	return result;
}

uint32_t MpqWriter::GetHashIndex(uint32_t index, uint32_t hashA, uint32_t hashB) const // NOLINT(bugprone-easily-swappable-parameters)
{
	uint32_t i = HashEntriesCount;
	for (unsigned idx = index & 0x7FF; hashTable_[idx].block != MpqHashEntry::NullBlock; idx = (idx + 1) & 0x7FF) {
		if (i-- == 0)
			break;
		if (hashTable_[idx].hashA != hashA)
			continue;
		if (hashTable_[idx].hashB != hashB)
			continue;
		if (hashTable_[idx].block == MpqHashEntry::DeletedBlock)
			continue;

		return idx;
	}

	return HashEntryNotFound;
}

bool MpqWriter::WriteHeaderAndTables()
{
	return WriteHeader() && WriteBlockTable() && WriteHashTable();
}

MpqBlockEntry *MpqWriter::AddFile(const char *filename, MpqBlockEntry *block, uint32_t blockIndex)
{
	uint32_t h1 = Hash(filename, 0);
	uint32_t h2 = Hash(filename, 1);
	uint32_t h3 = Hash(filename, 2);
	if (GetHashIndex(h1, h2, h3) != HashEntryNotFound)
		app_fatal("Hash collision between \"%s\" and existing file\n", filename);
	unsigned int hIdx = h1 & 0x7FF;

	bool hasSpace = false;
	for (unsigned i = 0; i < HashEntriesCount; ++i) {
		if (hashTable_[hIdx].block == MpqHashEntry::NullBlock || hashTable_[hIdx].block == MpqHashEntry::DeletedBlock) {
			hasSpace = true;
			break;
		}
		hIdx = (hIdx + 1) & 0x7FF;
	}
	if (!hasSpace)
		app_fatal("Out of hash space");

	if (block == nullptr)
		block = NewBlock(&blockIndex);

	MpqHashEntry &entry = hashTable_[hIdx];
	entry.hashA = h2;
	entry.hashB = h3;
	entry.locale = 0;
	entry.platform = 0;
	entry.block = blockIndex;

	return block;
}

bool MpqWriter::WriteFileContents(const char *filename, const byte *fileData, size_t fileSize, MpqBlockEntry *block)
{
	const char *tmp;
	while ((tmp = strchr(filename, ':')) != nullptr)
		filename = tmp + 1;
	while ((tmp = strchr(filename, '\\')) != nullptr)
		filename = tmp + 1;
	Hash(filename, 3);

	const uint32_t numSectors = (fileSize + (BlockSize - 1)) / BlockSize;
	const uint32_t offsetTableByteSize = sizeof(uint32_t) * (numSectors + 1);
	block->offset = FindFreeBlock(fileSize + offsetTableByteSize);
	// `packedSize` is reduced at the end of the function if it turns out to be smaller.
	block->packedSize = fileSize + offsetTableByteSize;
	block->unpackedSize = fileSize;
	block->flags = MpqBlockEntry::FlagExists | MpqBlockEntry::CompressPkZip;

	// We populate the table of sector offsets while we write the data.
	// We can't pre-populate it because we don't know the compressed sector sizes yet.
	// First offset is the start of the first sector, last offset is the end of the last sector.
	std::unique_ptr<uint32_t[]> offsetTable { new uint32_t[numSectors + 1] };

#ifdef CAN_SEEKP_BEYOND_EOF
	if (!stream_.Seekp(block->offset + offsetTableByteSize, std::ios::beg))
		return false;
#else
	// Ensure we do not Seekp beyond EOF by filling the missing space.
	std::streampos stream_end;
	if (!stream_.Seekp(0, std::ios::end) || !stream_.Tellp(&stream_end))
		return false;
	const std::uintmax_t cur_size = stream_end - streamBegin_;
	if (cur_size < block->offset + offsetTableByteSize) {
		if (cur_size < block->offset) {
			std::unique_ptr<char[]> filler { new char[block->offset - cur_size] };
			if (!stream_.Write(filler.get(), block->offset - cur_size))
				return false;
		}
		if (!stream_.Write(reinterpret_cast<const char *>(offsetTable.get()), offsetTableByteSize))
			return false;
	} else {
		if (!stream_.Seekp(block->offset + offsetTableByteSize, std::ios::beg))
			return false;
	}
#endif

	uint32_t destSize = offsetTableByteSize;
	byte mpqBuf[BlockSize];
	size_t curSector = 0;
	while (true) {
		uint32_t len = std::min<uint32_t>(fileSize, BlockSize);
		memcpy(mpqBuf, fileData, len);
		fileData += len;
		len = PkwareCompress(mpqBuf, len);
		if (!stream_.Write(reinterpret_cast<const char *>(&mpqBuf[0]), len))
			return false;
		offsetTable[curSector++] = SDL_SwapLE32(destSize);
		destSize += len; // compressed length
		if (fileSize <= BlockSize)
			break;

		fileSize -= BlockSize;
	}

	offsetTable[numSectors] = SDL_SwapLE32(destSize);
	if (!stream_.Seekp(block->offset, std::ios::beg))
		return false;
	if (!stream_.Write(reinterpret_cast<const char *>(offsetTable.get()), offsetTableByteSize))
		return false;
	if (!stream_.Seekp(destSize - offsetTableByteSize, std::ios::cur))
		return false;

	if (destSize < block->packedSize) {
		const uint32_t remainingBlockSize = block->packedSize - destSize;
		if (remainingBlockSize >= MinBlockSize) {
			// Allocate another block if we didn't use all of this one.
			block->packedSize = destSize;
			AllocBlock(block->packedSize + block->offset, remainingBlockSize);
		}
	}
	return true;
}

bool MpqWriter::WriteHeader()
{
	MpqFileHeader fhdr;

	memset(&fhdr, 0, sizeof(fhdr));
	fhdr.signature = MpqFileHeader::DiabloSignature;
	fhdr.headerSize = MpqFileHeader::DiabloSize;
	fhdr.fileSize = static_cast<uint32_t>(size_);
	fhdr.version = 0;
	fhdr.blockSizeFactor = BlockSizeFactor;
	fhdr.hashEntriesOffset = MpqHashEntryOffset;
	fhdr.blockEntriesOffset = MpqBlockEntryOffset;
	fhdr.hashEntriesCount = HashEntriesCount;
	fhdr.blockEntriesCount = BlockEntriesCount;
	ByteSwapHdr(&fhdr);

	return stream_.Write(reinterpret_cast<const char *>(&fhdr), sizeof(fhdr));
}

bool MpqWriter::WriteBlockTable()
{
	Encrypt(reinterpret_cast<uint32_t *>(blockTable_), BlockEntrySize, Hash("(block table)", 3));
	const bool success = stream_.Write(reinterpret_cast<const char *>(blockTable_), BlockEntrySize);
	Decrypt(reinterpret_cast<uint32_t *>(blockTable_), BlockEntrySize, Hash("(block table)", 3));
	return success;
}

bool MpqWriter::WriteHashTable()
{
	Encrypt(reinterpret_cast<uint32_t *>(hashTable_), HashEntrySize, Hash("(hash table)", 3));
	const bool success = stream_.Write(reinterpret_cast<const char *>(hashTable_), HashEntrySize);
	Decrypt(reinterpret_cast<uint32_t *>(hashTable_), HashEntrySize, Hash("(hash table)", 3));
	return success;
}

void MpqWriter::RemoveHashEntry(const char *filename)
{
	uint32_t hIdx = FetchHandle(filename);
	if (hIdx == HashEntryNotFound) {
		return;
	}

	MpqHashEntry *hashEntry = &hashTable_[hIdx];
	MpqBlockEntry *block = &blockTable_[hashEntry->block];
	hashEntry->block = MpqHashEntry::DeletedBlock;
	const uint32_t blockOffset = block->offset;
	const uint32_t blockSize = block->packedSize;
	memset(block, 0, sizeof(*block));
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
	MpqBlockEntry *blockEntry;

	modified_ = true;
	RemoveHashEntry(filename);
	blockEntry = AddFile(filename, nullptr, 0);
	if (!WriteFileContents(filename, data, size, blockEntry)) {
		RemoveHashEntry(filename);
		return false;
	}
	return true;
}

void MpqWriter::RenameFile(const char *name, const char *newName) // NOLINT(bugprone-easily-swappable-parameters)
{
	uint32_t index = FetchHandle(name);
	if (index == HashEntryNotFound) {
		return;
	}

	MpqHashEntry *hashEntry = &hashTable_[index];
	uint32_t block = hashEntry->block;
	MpqBlockEntry *blockEntry = &blockTable_[block];
	hashEntry->block = MpqHashEntry::DeletedBlock;
	AddFile(newName, blockEntry, block);
	modified_ = true;
}

bool MpqWriter::HasFile(const char *name) const
{
	return FetchHandle(name) != HashEntryNotFound;
}

} // namespace devilution

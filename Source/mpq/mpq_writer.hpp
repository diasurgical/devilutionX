/**
 * @file mpq/mpq_writer.hpp
 *
 * Interface of functions for creating and editing MPQ files.
 */
#pragma once

#include <cstdint>

#include "mpq/mpq_common.hpp"
#include "utils/logged_fstream.hpp"
#include "utils/stdcompat/cstddef.hpp"

namespace devilution {
class MpqWriter {
public:
	bool Open(const char *path);

	bool Close(bool clearTables = true);

	~MpqWriter()
	{
		Close();
	}

	bool HasFile(const char *name) const;

	void RemoveHashEntry(const char *filename);
	void RemoveHashEntries(bool (*fnGetName)(uint8_t, char *));
	bool WriteFile(const char *filename, const byte *data, size_t size);
	void RenameFile(const char *name, const char *newName);

private:
	bool IsValidMpqHeader(MpqFileHeader *hdr) const;
	uint32_t GetHashIndex(uint32_t index, uint32_t hashA, uint32_t hashB) const;
	uint32_t FetchHandle(const char *filename) const;

	bool ReadMPQHeader(MpqFileHeader *hdr);
	MpqBlockEntry *AddFile(const char *filename, MpqBlockEntry *block, uint32_t blockIndex);
	bool WriteFileContents(const char *filename, const byte *fileData, size_t fileSize, MpqBlockEntry *block);

	// Returns an unused entry in the block entry table.
	MpqBlockEntry *NewBlock(uint32_t *blockIndex = nullptr);

	// Marks space at `blockOffset` of size `blockSize` as free (unused) space.
	void AllocBlock(uint32_t blockOffset, uint32_t blockSize);

	// Returns the file offset that is followed by empty space of at least the given size.
	uint32_t FindFreeBlock(uint32_t size);

	bool WriteHeaderAndTables();
	bool WriteHeader();
	bool WriteBlockTable();
	bool WriteHashTable();
	void InitDefaultMpqHeader(MpqFileHeader *hdr);

	LoggedFStream stream_;
	std::string name_;
	std::uintmax_t size_;
	bool modified_;
	bool exists_;
	MpqHashEntry *hashTable_;
	MpqBlockEntry *blockTable_;

// Amiga cannot Seekp beyond EOF.
// See https://github.com/bebbo/libnix/issues/30
#ifndef __AMIGA__
#define CAN_SEEKP_BEYOND_EOF
#endif

#ifndef CAN_SEEKP_BEYOND_EOF
	std::streampos streamBegin_;
#endif
};

} // namespace devilution

/**
 * @file mpq/mpq_writer.hpp
 *
 * Interface of functions for creating and editing MPQ files.
 */
#pragma once

#include <cstdint>

#include "utils/logged_fstream.hpp"
#include "utils/stdcompat/cstddef.hpp"

namespace devilution {

struct _FILEHEADER {
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

struct _HASHENTRY {
	uint32_t hashcheck[2];
	uint32_t lcid;
	int32_t block;
};

struct _BLOCKENTRY {
	uint32_t offset;
	uint32_t sizealloc;
	uint32_t sizefile;
	uint32_t flags;
};

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
	bool IsValidMpqHeader(_FILEHEADER *hdr) const;
	int GetHashIndex(int index, uint32_t hashA, uint32_t hashB) const;
	int FetchHandle(const char *filename) const;

	bool ReadMPQHeader(_FILEHEADER *hdr);
	_BLOCKENTRY *AddFile(const char *pszName, _BLOCKENTRY *pBlk, int blockIndex);
	bool WriteFileContents(const char *pszName, const byte *pbData, size_t dwLen, _BLOCKENTRY *pBlk);
	_BLOCKENTRY *NewBlock(int *blockIndex);
	void AllocBlock(uint32_t blockOffset, uint32_t blockSize);
	int FindFreeBlock(uint32_t size, uint32_t *blockSize);
	bool WriteHeaderAndTables();
	bool WriteHeader();
	bool WriteBlockTable();
	bool WriteHashTable();
	void InitDefaultMpqHeader(_FILEHEADER *hdr);

	LoggedFStream stream_;
	std::string name_;
	std::uintmax_t size_;
	bool modified_;
	bool exists_;
	_HASHENTRY *hashTable_;
	_BLOCKENTRY *blockTable_;

// Amiga cannot Seekp beyond EOF.
// See https://github.com/bebbo/libnix/issues/30
#ifndef __AMIGA__
#define CAN_SEEKP_BEYOND_EOF
#endif

#ifndef CAN_SEEKP_BEYOND_EOF
	std::streampos stream_begin;
#endif
};

} // namespace devilution

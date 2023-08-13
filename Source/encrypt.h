/**
 * @file encrypt.h
 *
 * Interface of functions for compression and decompressing MPQ data.
 */
#pragma once

#include <cstddef>
#include <cstdint>

namespace devilution {

struct TDataInfo {
	std::byte *srcData;
	uint32_t srcOffset;
	std::byte *destData;
	uint32_t destOffset;
	uint32_t size;
};

void Decrypt(uint32_t *castBlock, uint32_t size, uint32_t key);
void Encrypt(uint32_t *castBlock, uint32_t size, uint32_t key);
uint32_t Hash(const char *s, int type);
uint32_t PkwareCompress(std::byte *srcData, uint32_t size);
void PkwareDecompress(std::byte *inBuff, uint32_t recvSize, int maxBytes);

} // namespace devilution

/**
 * @file encrypt.h
 *
 * Interface of functions for compression and decompressing MPQ data.
 */
#pragma once

#include <stdint.h>

#include "miniwin/miniwin.h"

namespace devilution {

struct TDataInfo {
	BYTE *srcData;
	uint32_t srcOffset;
	BYTE *destData;
	uint32_t destOffset;
	uint32_t size;
};

void Decrypt(DWORD *castBlock, DWORD size, DWORD key);
void Encrypt(DWORD *castBlock, DWORD size, DWORD key);
uint32_t Hash(const char *s, int type);
void InitHash();
DWORD PkwareCompress(BYTE *srcData, DWORD size);
void PkwareDecompress(BYTE *pbInBuff, int recv_size, int dwMaxBytes);

} // namespace devilution

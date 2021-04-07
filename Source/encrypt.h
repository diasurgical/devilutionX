/**
 * @file encrypt.h
 *
 * Interface of functions for compression and decompressing MPQ data.
 */
#pragma once

namespace devilution {

#ifdef __cplusplus
extern "C" {
#endif

typedef struct TDataInfo {
	Uint8 *srcData;
	Uint32 srcOffset;
	Uint8 *destData;
	Uint32 destOffset;
	Uint32 size;
} TDataInfo;

void Decrypt(DWORD *castBlock, DWORD size, DWORD key);
void Encrypt(DWORD *castBlock, DWORD size, DWORD key);
DWORD Hash(const char *s, int type);
void InitHash();
DWORD PkwareCompress(BYTE *srcData, DWORD size);
void PkwareDecompress(BYTE *pbInBuff, int recv_size, int dwMaxBytes);

#ifdef __cplusplus
}
#endif

}

/**
 * @file encrypt.cpp
 *
 * Implementation of functions for compression and decompressing MPQ data.
 */
#include "all.h"
#include "../3rdParty/PKWare/pkware.h"

DEVILUTION_BEGIN_NAMESPACE

DWORD hashtable[5][256];

void Decrypt(DWORD *castBlock, DWORD size, DWORD key)
{
	DWORD seed, i;

	seed = 0xEEEEEEEE;
	for (i = 0; i < (size >> 2); i++) {
		DWORD t = SwapLE32(*castBlock);
		seed += hashtable[4][(key & 0xFF)];
		t ^= seed + key;
		*castBlock = t;
		seed += t + (seed << 5) + 3;
		castBlock++;
		key = (((key << 0x15) ^ 0xFFE00000) + 0x11111111) | (key >> 0x0B);
	}
}

void Encrypt(DWORD *castBlock, DWORD size, DWORD key)
{
	DWORD seed, i, ch;

	seed = 0xEEEEEEEE;
	for (i = 0; i < (size >> 2); i++) {
		DWORD t = ch = *castBlock;
		seed += hashtable[4][(key & 0xFF)];
		t ^= seed + key;
		*castBlock = SwapLE32(t);
		castBlock++;
		seed += ch + (seed << 5) + 3;
		key = ((key << 0x15) ^ 0xFFE00000) + 0x11111111 | (key >> 0x0B);
	}
}

DWORD Hash(const char *s, int type)
{
	char ch;
	DWORD seed1, seed2;

	seed1 = 0x7FED7FED;
	seed2 = 0xEEEEEEEE;
	while (s != NULL && *s) {
		ch = *s++;
		ch = toupper(ch);
		seed1 = hashtable[type][ch] ^ (seed1 + seed2);
		seed2 += ch + seed1 + (seed2 << 5) + 3;
	}
	return seed1;
}

void InitHash()
{
	DWORD seed, ch;
	int i, j;

	seed = 0x00100001;

	for (i = 0; i < 256; i++) {
		for (j = 0; j < 5; j++) {
			seed = (125 * seed + 3) % 0x2AAAAB;
			ch = (seed & 0xFFFF);
			seed = (125 * seed + 3) % 0x2AAAAB;
			hashtable[j][i] = ch << 16 | (seed & 0xFFFF);
		}
	}
}

static unsigned int PkwareBufferRead(char *buf, unsigned int *size, void *param)
{
	TDataInfo *pInfo;
	DWORD sSize;

	pInfo = (TDataInfo *)param;

	if (*size >= pInfo->size - pInfo->srcOffset) {
		sSize = pInfo->size - pInfo->srcOffset;
	} else {
		sSize = *size;
	}

	memcpy(buf, pInfo->srcData + pInfo->srcOffset, sSize);
	pInfo->srcOffset += sSize;

	return sSize;
}

static void PkwareBufferWrite(char *buf, unsigned int *size, void *param)
{
	TDataInfo *pInfo;

	pInfo = (TDataInfo *)param;

	memcpy(pInfo->destData + pInfo->destOffset, buf, *size);
	pInfo->destOffset += *size;
}

DWORD PkwareCompress(BYTE *srcData, DWORD size)
{
	BYTE *destData;
	char *ptr;
	unsigned int destSize, type, dsize;
	TDataInfo param;

	ptr = (char *)DiabloAllocPtr(CMP_BUFFER_SIZE);

	destSize = 2 * size;
	if (destSize < 2 * 4096)
		destSize = 2 * 4096;

	destData = (BYTE *)DiabloAllocPtr(destSize);

	param.srcData = srcData;
	param.srcOffset = 0;
	param.destData = destData;
	param.destOffset = 0;
	param.size = size;

	type = 0;
	dsize = 4096;
	implode(PkwareBufferRead, PkwareBufferWrite, ptr, &param, &type, &dsize);

	if (param.destOffset < size) {
		memcpy(srcData, destData, param.destOffset);
		size = param.destOffset;
	}

	mem_free_dbg(ptr);
	mem_free_dbg(destData);

	return size;
}

void PkwareDecompress(BYTE *pbInBuff, int recv_size, int dwMaxBytes)
{
	char *ptr;
	BYTE *pbOutBuff;
	TDataInfo info;

	ptr = (char *)DiabloAllocPtr(CMP_BUFFER_SIZE);
	pbOutBuff = DiabloAllocPtr(dwMaxBytes);

	info.srcData = pbInBuff;
	info.srcOffset = 0;
	info.destData = pbOutBuff;
	info.destOffset = 0;
	info.size = recv_size;

	explode(PkwareBufferRead, PkwareBufferWrite, ptr, &info);
	memcpy(pbInBuff, pbOutBuff, info.destOffset);
	mem_free_dbg(ptr);
	mem_free_dbg(pbOutBuff);
}

DEVILUTION_END_NAMESPACE

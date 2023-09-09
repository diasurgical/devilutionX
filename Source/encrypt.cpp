/**
 * @file encrypt.cpp
 *
 * Implementation of functions for compression and decompressing MPQ data.
 */
#include <array>
#include <cctype>
#include <cstdint>
#include <cstring>
#include <memory>

#include <SDL.h>
#include <pkware.h>

#include "encrypt.h"

namespace devilution {

namespace {

struct TDataInfo {
	std::byte *srcData;
	uint32_t srcOffset;
	std::byte *destData;
	uint32_t destOffset;
	uint32_t size;
};

unsigned int PkwareBufferRead(char *buf, unsigned int *size, void *param) // NOLINT(readability-non-const-parameter)
{
	auto *pInfo = reinterpret_cast<TDataInfo *>(param);

	uint32_t sSize;
	if (*size >= pInfo->size - pInfo->srcOffset) {
		sSize = pInfo->size - pInfo->srcOffset;
	} else {
		sSize = *size;
	}

	memcpy(buf, pInfo->srcData + pInfo->srcOffset, sSize);
	pInfo->srcOffset += sSize;

	return sSize;
}

void PkwareBufferWrite(char *buf, unsigned int *size, void *param) // NOLINT(readability-non-const-parameter)
{
	auto *pInfo = reinterpret_cast<TDataInfo *>(param);

	memcpy(pInfo->destData + pInfo->destOffset, buf, *size);
	pInfo->destOffset += *size;
}

} // namespace

uint32_t PkwareCompress(std::byte *srcData, uint32_t size)
{
	std::unique_ptr<char[]> ptr = std::make_unique<char[]>(CMP_BUFFER_SIZE);

	unsigned destSize = 2 * size;
	if (destSize < 2 * 4096)
		destSize = 2 * 4096;

	std::unique_ptr<std::byte[]> destData { new std::byte[destSize] };

	TDataInfo param;
	param.srcData = srcData;
	param.srcOffset = 0;
	param.destData = destData.get();
	param.destOffset = 0;
	param.size = size;

	unsigned type = 0;
	unsigned dsize = 4096;
	implode(PkwareBufferRead, PkwareBufferWrite, ptr.get(), &param, &type, &dsize);

	if (param.destOffset < size) {
		memcpy(srcData, destData.get(), param.destOffset);
		size = param.destOffset;
	}

	return size;
}

void PkwareDecompress(std::byte *inBuff, uint32_t recvSize, int maxBytes)
{
	std::unique_ptr<char[]> ptr = std::make_unique<char[]>(CMP_BUFFER_SIZE);
	std::unique_ptr<std::byte[]> outBuff { new std::byte[maxBytes] };

	TDataInfo info;
	info.srcData = inBuff;
	info.srcOffset = 0;
	info.destData = outBuff.get();
	info.destOffset = 0;
	info.size = recvSize;

	explode(PkwareBufferRead, PkwareBufferWrite, ptr.get(), &info);
	memcpy(inBuff, outBuff.get(), info.destOffset);
}

} // namespace devilution

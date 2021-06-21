#pragma once

#include <cstdint>
#include <cstring>

#include <SDL_endian.h>

#include "utils/endian.hpp"
#include "utils/stdcompat/cstddef.hpp"

namespace devilution {

inline byte *CelGetFrameStart(byte *pCelBuff, int nCel)
{
	const auto *pFrameTable = reinterpret_cast<const std::uint32_t *>(pCelBuff);

	return &pCelBuff[SDL_SwapLE32(pFrameTable[nCel])];
}

inline byte *CelGetFrame(byte *pCelBuff, int nCel, int *nDataSize)
{
	const std::uint32_t nCellStart = LoadLE32(&pCelBuff[nCel * sizeof(std::uint32_t)]);
	*nDataSize = static_cast<int>(LoadLE32(&pCelBuff[(nCel + 1) * sizeof(std::uint32_t)]) - nCellStart);
	return &pCelBuff[nCellStart];
}

inline const byte *CelGetFrame(const byte *pCelBuff, int nCel, int *nDataSize)
{
	const std::uint32_t nCellStart = LoadLE32(&pCelBuff[nCel * sizeof(std::uint32_t)]);
	*nDataSize = static_cast<int>(LoadLE32(&pCelBuff[(nCel + 1) * sizeof(std::uint32_t)]) - nCellStart);
	return &pCelBuff[nCellStart];
}

struct FrameHeader {
	uint16_t row0;
	uint16_t row32;
	uint16_t row64;
	uint16_t row96;
	uint16_t row128;
};

inline const byte *CelGetFrameClipped(const byte *pCelBuff, int nCel, int *nDataSize)
{
	const byte *pRLEBytes = CelGetFrame(pCelBuff, nCel, nDataSize);

	FrameHeader frameHeader;
	std::memcpy(&frameHeader, pRLEBytes, sizeof(FrameHeader));

	std::uint16_t nDataStart = SDL_SwapLE16(frameHeader.row0);
	*nDataSize -= nDataStart;

	return &pRLEBytes[nDataStart];
}

} // namespace devilution

/**
 * @file codec.h
 *
 * Interface of save game encryption algorithm.
 */
#pragma once

#include <cstddef>

#include "miniwin/miniwin.h"

namespace devilution {

int codec_decode(BYTE *pbSrcDst, DWORD size, const char *pszPassword);
std::size_t codec_get_encoded_len(std::size_t dwSrcBytes);
void codec_encode(BYTE *pbSrcDst, std::size_t size, std::size_t size_64, const char *pszPassword);

} // namespace devilution

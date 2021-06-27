/**
 * @file codec.h
 *
 * Interface of save game encryption algorithm.
 */
#pragma once

#include "utils/stdcompat/cstddef.hpp"

namespace devilution {

std::size_t codec_decode(byte *pbSrcDst, std::size_t size, const char *pszPassword);
std::size_t codec_get_encoded_len(std::size_t dwSrcBytes);
void codec_encode(byte *pbSrcDst, std::size_t size, std::size_t size_64, const char *pszPassword);

} // namespace devilution

/**
 * @file codec.h
 *
 * Interface of save game encryption algorithm.
 */
#pragma once

namespace devilution {

#ifdef __cplusplus
extern "C" {
#endif

int codec_decode(BYTE *pbSrcDst, DWORD size, const char *pszPassword);
DWORD codec_get_encoded_len(DWORD dwSrcBytes);
void codec_encode(BYTE *pbSrcDst, DWORD size, int size_64, const char *pszPassword);

#ifdef __cplusplus
}
#endif

}

/**
 * @file codec.cpp
 *
 * Implementation of save game encryption algorithm.
 */

#include <cstdint>

#include "appfat.h"
#include "miniwin/miniwin.h"
#include "sha.h"
#include "utils/stdcompat/cstddef.hpp"

namespace devilution {
namespace  {

struct CodecSignature {
	uint32_t checksum;
	uint8_t error;
	uint8_t lastChunkSize;
	uint16_t unused;
};

#define BlockSize 64

void CodecInitKey(const char *pszPassword)
{
	char key[136]; // last 64 bytes are the SHA1
	uint32_t randState = 0x7058;
	for (char &notch : key) {
		randState = randState * 214013 + 2531011;
		notch = randState >> 16; // Downcasting to char keeps the 2 least-significant bytes
	}

	char pw[64]; // Repeat password until 64 char long
	std::size_t j = 0;
	for (std::size_t i = 0; i < sizeof(pw); i++, j++) {
		if (pszPassword[j] == '\0')
			j = 0;
		pw[i] = pszPassword[j];
	}

	char digest[SHA1HashSize];
	SHA1Reset(0);
	SHA1Calculate(0, pw, digest);
	SHA1Clear();
	for (std::size_t i = 0; i < sizeof(key); i++)
		key[i] ^= digest[i % SHA1HashSize];
	memset(pw, 0, sizeof(pw));
	memset(digest, 0, sizeof(digest));
	for (int n = 0; n < 3; ++n) {
		SHA1Reset(n);
		SHA1Calculate(n, &key[72], nullptr);
	}
	memset(key, 0, sizeof(key));
}
} // namespace

std::size_t codec_decode(byte *pbSrcDst, std::size_t size, const char *pszPassword)
{
	char buf[128];
	char dst[SHA1HashSize];

	CodecInitKey(pszPassword);
	if (size <= sizeof(CodecSignature))
		return 0;
	size -= sizeof(CodecSignature);
	if (size % BlockSize != 0)
		return 0;
	for (int i = size; i != 0; pbSrcDst += BlockSize, i -= BlockSize) {
		memcpy(buf, pbSrcDst, BlockSize);
		SHA1Result(0, dst);
		for (int j = 0; j < BlockSize; j++) {
			buf[j] ^= dst[j % SHA1HashSize];
		}
		SHA1Calculate(0, buf, nullptr);
		memset(dst, 0, sizeof(dst));
		memcpy(pbSrcDst, buf, BlockSize);
	}

	memset(buf, 0, sizeof(buf));
	auto *sig = (CodecSignature *)pbSrcDst;
	if (sig->error > 0) {
		goto error;
	}

	SHA1Result(0, dst);
	if (sig->checksum != *(uint32_t *)dst) {
		memset(dst, 0, sizeof(dst));
		goto error;
	}

	size += sig->lastChunkSize - BlockSize;
	SHA1Clear();
	return size;
error:
	SHA1Clear();
	return 0;
}

std::size_t codec_get_encoded_len(std::size_t dwSrcBytes)
{
	if (dwSrcBytes % BlockSize != 0)
		dwSrcBytes += BlockSize - (dwSrcBytes % BlockSize);
	return dwSrcBytes + sizeof(CodecSignature);
}

void codec_encode(byte *pbSrcDst, std::size_t size, std::size_t size64, const char *pszPassword)
{
	char buf[128];
	char tmp[SHA1HashSize];
	char dst[SHA1HashSize];

	if (size64 != codec_get_encoded_len(size))
		app_fatal("Invalid encode parameters");
	CodecInitKey(pszPassword);

	uint16_t lastChunk = 0;
	while (size != 0) {
		uint16_t chunk = size < BlockSize ? size : BlockSize;
		memcpy(buf, pbSrcDst, chunk);
		if (chunk < BlockSize)
			memset(buf + chunk, 0, BlockSize - chunk);
		SHA1Result(0, dst);
		SHA1Calculate(0, buf, nullptr);
		for (int j = 0; j < BlockSize; j++) {
			buf[j] ^= dst[j % SHA1HashSize];
		}
		memset(dst, 0, sizeof(dst));
		memcpy(pbSrcDst, buf, BlockSize);
		lastChunk = chunk;
		pbSrcDst += BlockSize;
		size -= chunk;
	}
	memset(buf, 0, sizeof(buf));
	SHA1Result(0, tmp);
	auto *sig = (CodecSignature *)pbSrcDst;
	sig->error = 0;
	sig->unused = 0;
	sig->checksum = *(uint32_t *)&tmp[0];
	sig->lastChunkSize = lastChunk;
	SHA1Clear();
}

} // namespace devilution

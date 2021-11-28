/**
 * @file codec.cpp
 *
 * Implementation of save game encryption algorithm.
 */

#include <array>
#include <cstdint>
#include <cstring>

#include "appfat.h"
#include "sha.h"
#include "utils/endian.hpp"
#include "utils/stdcompat/cstddef.hpp"

namespace devilution {
namespace {

struct CodecSignature {
	uint32_t checksum;
	uint8_t error;
	uint8_t lastChunkSize;
	uint16_t unused;
};

// https://stackoverflow.com/a/45172360 - helper to make up for not having an implicit initializer for std::byte
template <typename... Ts>
std::array<byte, sizeof...(Ts)> make_bytes(Ts &&...args) noexcept
{
	return { byte(std::forward<Ts>(args))... };
}

void CodecInitKey(const char *pszPassword)
{
	byte pw[BlockSize]; // Repeat password until 64 char long
	std::size_t j = 0;
	for (std::size_t i = 0; i < sizeof(pw); i++, j++) {
		if (pszPassword[j] == '\0')
			j = 0;
		pw[i] = static_cast<byte>(pszPassword[j]);
	}

	byte digest[SHA1HashSize];
	SHA1Reset(0);
	SHA1Calculate(0, pw, digest);
	SHA1Clear();

	// declaring key as a std::array to make the initialization easier, otherwise we would need to explicitly
	// declare every value as a byte on platforms that use std::byte.
	std::array<byte, BlockSize> key = make_bytes( // clang-format off
		0xbf, 0x2f, 0x63, 0xad, 0xd0, 0x56, 0x27, 0xf7, 0x6e, 0x43, 0x47, 0x27, 0x70, 0xc7, 0x5b, 0x42,
		0x58, 0xac, 0x1e, 0xea, 0xca, 0x50, 0x7d, 0x28, 0x43, 0x93, 0xee, 0x68, 0x07, 0xf3, 0x03, 0xc5,
		0x5b, 0xf6, 0x3f, 0x87, 0xf5, 0xc9, 0x28, 0xea, 0xb1, 0x26, 0x9d, 0x22, 0x85, 0x7a, 0x6a, 0x9b,
		0xb7, 0x48, 0x1a, 0x85, 0x4d, 0xc8, 0x0d, 0x90, 0xc6, 0xd5, 0xca, 0xf4, 0x07, 0x06, 0x95, 0xb5
	); // clang-format on

	for (std::size_t i = 0; i < sizeof(key); i++)
		key[i] ^= digest[(i + 12) % SHA1HashSize];
	memset(pw, 0, sizeof(pw));
	memset(digest, 0, sizeof(digest));
	for (int n = 0; n < 3; ++n) {
		SHA1Reset(n);
		SHA1Calculate(n, key.data(), nullptr);
	}
	memset(key.data(), 0, sizeof(key));
}
} // namespace

std::size_t codec_decode(byte *pbSrcDst, std::size_t size, const char *pszPassword)
{
	byte buf[BlockSize];
	byte dst[SHA1HashSize];

	CodecInitKey(pszPassword);
	if (size <= sizeof(CodecSignature))
		return 0;
	size -= sizeof(CodecSignature);
	if (size % BlockSize != 0)
		return 0;
	for (auto i = size; i != 0; pbSrcDst += BlockSize, i -= BlockSize) {
		memcpy(buf, pbSrcDst, BlockSize);
		SHA1Result(0, dst);
		for (unsigned j = 0; j < BlockSize; j++) {
			buf[j] ^= dst[j % SHA1HashSize];
		}
		SHA1Calculate(0, buf, nullptr);
		memset(dst, 0, sizeof(dst));
		memcpy(pbSrcDst, buf, BlockSize);
	}

	memset(buf, 0, sizeof(buf));
	auto *sig = reinterpret_cast<CodecSignature *>(pbSrcDst);
	if (sig->error > 0) {
		goto error;
	}

	SHA1Result(0, dst);
	if (sig->checksum != *reinterpret_cast<uint32_t *>(dst)) {
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
	byte buf[BlockSize];
	byte tmp[SHA1HashSize];
	byte dst[SHA1HashSize];

	if (size64 != codec_get_encoded_len(size))
		app_fatal("Invalid encode parameters");
	CodecInitKey(pszPassword);

	size_t lastChunk = 0;
	while (size != 0) {
		size_t chunk = size < BlockSize ? size : BlockSize;
		memcpy(buf, pbSrcDst, chunk);
		if (chunk < BlockSize)
			memset(buf + chunk, 0, BlockSize - chunk);
		SHA1Result(0, dst);
		SHA1Calculate(0, buf, nullptr);
		for (unsigned j = 0; j < BlockSize; j++) {
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
	auto *sig = reinterpret_cast<CodecSignature *>(pbSrcDst);
	sig->error = 0;
	sig->unused = 0;
	sig->checksum = *reinterpret_cast<uint32_t *>(tmp);
	sig->lastChunkSize = static_cast<uint8_t>(lastChunk); // lastChunk is at most 64 so will always fit in an 8 bit var
	SHA1Clear();
}

} // namespace devilution

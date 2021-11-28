/**
 * @file sha.cpp
 *
 * Implementation of functionality for calculating X-SHA-1 (a flawed implementation of SHA-1).
 */
#include "sha.h"

#include <cstdint>
#include <cstring>

#include <SDL.h>

#include "appfat.h"

namespace devilution {

// NOTE: Diablo's "SHA1" is different from actual SHA1 in that it uses arithmetic
// right shifts (sign bit extension).

namespace {

struct SHA1Context {
	uint32_t state[SHA1HashSize / sizeof(uint32_t)];
	uint32_t buffer[BlockSize / sizeof(uint32_t)];
};

SHA1Context sgSHA1[3];

/**
 * Diablo-"SHA1" circular left shift, portable version.
 */
uint32_t SHA1CircularShift(uint32_t word, size_t bits)
{
	assert(bits < 32);
	assert(bits > 0);

	// The SHA-like algorithm as originally implemented treated word as a signed value and used arithmetic right shifts
	//  (sign-extending). This results in the high 32-`bits` bits being set to 1.
	if ((word & (1 << 31)) != 0)
		return (0xFFFFFFFF << bits) | (word >> (32 - bits));
	return (word << bits) | (word >> (32 - bits));
}

void SHA1Init(SHA1Context *context)
{
	context->state[0] = 0x67452301;
	context->state[1] = 0xEFCDAB89;
	context->state[2] = 0x98BADCFE;
	context->state[3] = 0x10325476;
	context->state[4] = 0xC3D2E1F0;
}

void SHA1ProcessMessageBlock(SHA1Context *context)
{
	std::uint32_t w[80];

	for (int i = 0; i < 16; i++)
		w[i] = SDL_SwapLE32(context->buffer[i]);

	for (int i = 16; i < 80; i++) {
		w[i] = w[i - 16] ^ w[i - 14] ^ w[i - 8] ^ w[i - 3];
	}

	std::uint32_t a = context->state[0];
	std::uint32_t b = context->state[1];
	std::uint32_t c = context->state[2];
	std::uint32_t d = context->state[3];
	std::uint32_t e = context->state[4];

	for (int i = 0; i < 20; i++) {
		std::uint32_t temp = SHA1CircularShift(a, 5) + ((b & c) | ((~b) & d)) + e + w[i] + 0x5A827999;
		e = d;
		d = c;
		c = SHA1CircularShift(b, 30);
		b = a;
		a = temp;
	}

	for (int i = 20; i < 40; i++) {
		std::uint32_t temp = SHA1CircularShift(a, 5) + (b ^ c ^ d) + e + w[i] + 0x6ED9EBA1;
		e = d;
		d = c;
		c = SHA1CircularShift(b, 30);
		b = a;
		a = temp;
	}

	for (int i = 40; i < 60; i++) {
		std::uint32_t temp = SHA1CircularShift(a, 5) + ((b & c) | (b & d) | (c & d)) + e + w[i] + 0x8F1BBCDC;
		e = d;
		d = c;
		c = SHA1CircularShift(b, 30);
		b = a;
		a = temp;
	}

	for (int i = 60; i < 80; i++) {
		std::uint32_t temp = SHA1CircularShift(a, 5) + (b ^ c ^ d) + e + w[i] + 0xCA62C1D6;
		e = d;
		d = c;
		c = SHA1CircularShift(b, 30);
		b = a;
		a = temp;
	}

	context->state[0] += a;
	context->state[1] += b;
	context->state[2] += c;
	context->state[3] += d;
	context->state[4] += e;
}

void SHA1Input(SHA1Context *context, const byte *messageArray, std::size_t len)
{
	for (auto i = len / BlockSize; i != 0; i--) {
		memcpy(context->buffer, messageArray, BlockSize);
		SHA1ProcessMessageBlock(context);
		messageArray += BlockSize;
	}
}

} // namespace

void SHA1Clear()
{
	memset(sgSHA1, 0, sizeof(sgSHA1));
}

void SHA1Result(int n, byte messageDigest[SHA1HashSize])
{
	std::uint32_t *messageDigestBlock = reinterpret_cast<std::uint32_t *>(messageDigest);
	if (messageDigest != nullptr) {
		for (auto &block : sgSHA1[n].state) {
			*messageDigestBlock = SDL_SwapLE32(block);
			messageDigestBlock++;
		}
	}
}

void SHA1Calculate(int n, const byte data[BlockSize], byte messageDigest[SHA1HashSize])
{
	SHA1Input(&sgSHA1[n], data, BlockSize);
	if (messageDigest != nullptr)
		SHA1Result(n, messageDigest);
}

void SHA1Reset(int n)
{
	SHA1Init(&sgSHA1[n]);
}

} // namespace devilution

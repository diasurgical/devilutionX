/**
 * @file sha.cpp
 *
 * Implementation of functionality for calculating X-SHA-1 (a flawed implementation of SHA-1).
 */
#include "sha.h"

#include <SDL.h>
#include <cstdint>

#include "appfat.h"

namespace devilution {

// NOTE: Diablo's "SHA1" is different from actual SHA1 in that it uses arithmetic
// right shifts (sign bit extension).

namespace {

/**
 * Diablo-"SHA1" circular left shift, portable version.
 */
uint32_t SHA1CircularShift(uint32_t bits, uint32_t word)
{
	assert(bits < 32);
	assert(bits > 0);

	if ((word & 0x80000000) != 0) {
		//moving this part to a separate volatile variable fixes saves in x64-release build in visual studio 2017
		volatile uint32_t tmp = ((~word) >> (32 - bits));
		return (word << bits) | (~tmp);
	}
	return (word << bits) | (word >> (32 - bits));
}

} // namespace

SHA1Context sgSHA1[3];

static void SHA1Init(SHA1Context *context)
{
	context->count[0] = 0;
	context->count[1] = 0;
	context->state[0] = 0x67452301;
	context->state[1] = 0xEFCDAB89;
	context->state[2] = 0x98BADCFE;
	context->state[3] = 0x10325476;
	context->state[4] = 0xC3D2E1F0;
}

static void SHA1ProcessMessageBlock(SHA1Context *context)
{
	std::uint32_t w[80];

	auto *buf = (std::uint32_t *)context->buffer;
	for (int i = 0; i < 16; i++)
		w[i] = SDL_SwapLE32(buf[i]);

	for (int i = 16; i < 80; i++) {
		w[i] = w[i - 16] ^ w[i - 14] ^ w[i - 8] ^ w[i - 3];
	}

	std::uint32_t a = context->state[0];
	std::uint32_t b = context->state[1];
	std::uint32_t c = context->state[2];
	std::uint32_t d = context->state[3];
	std::uint32_t e = context->state[4];

	for (int i = 0; i < 20; i++) {
		std::uint32_t temp = SHA1CircularShift(5, a) + ((b & c) | ((~b) & d)) + e + w[i] + 0x5A827999;
		e = d;
		d = c;
		c = SHA1CircularShift(30, b);
		b = a;
		a = temp;
	}

	for (int i = 20; i < 40; i++) {
		std::uint32_t temp = SHA1CircularShift(5, a) + (b ^ c ^ d) + e + w[i] + 0x6ED9EBA1;
		e = d;
		d = c;
		c = SHA1CircularShift(30, b);
		b = a;
		a = temp;
	}

	for (int i = 40; i < 60; i++) {
		std::uint32_t temp = SHA1CircularShift(5, a) + ((b & c) | (b & d) | (c & d)) + e + w[i] + 0x8F1BBCDC;
		e = d;
		d = c;
		c = SHA1CircularShift(30, b);
		b = a;
		a = temp;
	}

	for (int i = 60; i < 80; i++) {
		std::uint32_t temp = SHA1CircularShift(5, a) + (b ^ c ^ d) + e + w[i] + 0xCA62C1D6;
		e = d;
		d = c;
		c = SHA1CircularShift(30, b);
		b = a;
		a = temp;
	}

	context->state[0] += a;
	context->state[1] += b;
	context->state[2] += c;
	context->state[3] += d;
	context->state[4] += e;
}

static void SHA1Input(SHA1Context *context, const char *messageArray, std::uint32_t len)
{
	std::uint32_t count = context->count[0] + 8 * len;
	if (count < context->count[0])
		context->count[1]++;

	context->count[0] = count;
	context->count[1] += len >> 29;

	for (int i = len; i >= 64; i -= 64) {
		memcpy(context->buffer, messageArray, sizeof(context->buffer));
		SHA1ProcessMessageBlock(context);
		messageArray += 64;
	}
}

void SHA1Clear()
{
	memset(sgSHA1, 0, sizeof(sgSHA1));
}

void SHA1Result(int n, char messageDigest[SHA1HashSize])
{
	std::uint32_t *messageDigestBlock;
	int i;

	messageDigestBlock = (std::uint32_t *)messageDigest;
	if (messageDigest != nullptr) {
		for (i = 0; i < 5; i++) {
			*messageDigestBlock = SDL_SwapLE32(sgSHA1[n].state[i]);
			messageDigestBlock++;
		}
	}
}

void SHA1Calculate(int n, const char *data, char messageDigest[SHA1HashSize])
{
	SHA1Input(&sgSHA1[n], data, 64);
	if (messageDigest != nullptr)
		SHA1Result(n, messageDigest);
}

void SHA1Reset(int n)
{
	SHA1Init(&sgSHA1[n]);
}

} // namespace devilution

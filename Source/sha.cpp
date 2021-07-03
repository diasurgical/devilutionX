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
	std::uint32_t W[80];

	auto *buf = (std::uint32_t *)context->buffer;
	for (int i = 0; i < 16; i++)
		W[i] = SDL_SwapLE32(buf[i]);

	for (int i = 16; i < 80; i++) {
		W[i] = W[i - 16] ^ W[i - 14] ^ W[i - 8] ^ W[i - 3];
	}

	std::uint32_t A = context->state[0];
	std::uint32_t B = context->state[1];
	std::uint32_t C = context->state[2];
	std::uint32_t D = context->state[3];
	std::uint32_t E = context->state[4];

	for (int i = 0; i < 20; i++) {
		std::uint32_t temp = SHA1CircularShift(5, A) + ((B & C) | ((~B) & D)) + E + W[i] + 0x5A827999;
		E = D;
		D = C;
		C = SHA1CircularShift(30, B);
		B = A;
		A = temp;
	}

	for (int i = 20; i < 40; i++) {
		std::uint32_t temp = SHA1CircularShift(5, A) + (B ^ C ^ D) + E + W[i] + 0x6ED9EBA1;
		E = D;
		D = C;
		C = SHA1CircularShift(30, B);
		B = A;
		A = temp;
	}

	for (int i = 40; i < 60; i++) {
		std::uint32_t temp = SHA1CircularShift(5, A) + ((B & C) | (B & D) | (C & D)) + E + W[i] + 0x8F1BBCDC;
		E = D;
		D = C;
		C = SHA1CircularShift(30, B);
		B = A;
		A = temp;
	}

	for (int i = 60; i < 80; i++) {
		std::uint32_t temp = SHA1CircularShift(5, A) + (B ^ C ^ D) + E + W[i] + 0xCA62C1D6;
		E = D;
		D = C;
		C = SHA1CircularShift(30, B);
		B = A;
		A = temp;
	}

	context->state[0] += A;
	context->state[1] += B;
	context->state[2] += C;
	context->state[3] += D;
	context->state[4] += E;
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
	std::uint32_t *Message_Digest_Block;
	int i;

	Message_Digest_Block = (std::uint32_t *)messageDigest;
	if (messageDigest != nullptr) {
		for (i = 0; i < 5; i++) {
			*Message_Digest_Block = SDL_SwapLE32(sgSHA1[n].state[i]);
			Message_Digest_Block++;
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

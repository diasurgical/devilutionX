/**
 * @file sha.cpp
 *
 * Interface of functionality for calculating X-SHA-1 (a flawed implementation of SHA-1).
 */
#pragma once

#include <cstdint>

#include "utils/stdcompat/cstddef.hpp"

namespace devilution {

#define SHA1HashSize 20

struct SHA1Context {
	uint32_t state[5];
	uint32_t count[2];
	uint32_t buffer[16];
};

void SHA1Clear();
void SHA1Result(int n, byte messageDigest[SHA1HashSize]);
void SHA1Calculate(int n, const byte *data, byte messageDigest[SHA1HashSize]);
void SHA1Reset(int n);

} // namespace devilution

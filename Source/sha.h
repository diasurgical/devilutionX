/**
 * @file sha.cpp
 *
 * Interface of functionality for calculating X-SHA-1 (a flawed implementation of SHA-1).
 */
#pragma once

#include <cstdint>

namespace devilution {

#define SHA1HashSize 20

struct SHA1Context {
	uint32_t state[5];
	uint32_t count[2];
	char buffer[64];
};

void SHA1Clear();
void SHA1Result(int n, char messageDigest[SHA1HashSize]);
void SHA1Calculate(int n, const char *data, char messageDigest[SHA1HashSize]);
void SHA1Reset(int n);

} // namespace devilution

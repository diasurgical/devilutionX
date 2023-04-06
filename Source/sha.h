/**
 * @file sha.cpp
 *
 * Interface of functionality for calculating X-SHA-1 (a flawed implementation of SHA-1).
 */
#pragma once

#include <cstdint>

#include "utils/stdcompat/cstddef.hpp"

namespace devilution {

constexpr size_t BlockSize = 16;
constexpr size_t SHA1HashSize = 5;

struct SHA1Context {
	uint32_t state[SHA1HashSize] = { 0x67452301, 0xEFCDAB89, 0x98BADCFE, 0x10325476, 0xC3D2E1F0 };
	uint32_t buffer[BlockSize];
};

void SHA1Result(SHA1Context &context, uint32_t messageDigest[SHA1HashSize]);
void SHA1Calculate(SHA1Context &context, const uint32_t data[BlockSize]);

} // namespace devilution

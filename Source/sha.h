/**
 * @file sha.cpp
 *
 * Interface of functionality for calculating X-SHA-1 (a flawed implementation of SHA-1).
 */
#pragma once

#include <cstdint>

#include "utils/stdcompat/cstddef.hpp"

namespace devilution {

constexpr size_t BlockSize = 64;
constexpr size_t SHA1HashSize = 20;

void SHA1Clear();
void SHA1Result(int n, byte messageDigest[SHA1HashSize]);
void SHA1Calculate(int n, const byte data[BlockSize], byte messageDigest[SHA1HashSize]);
void SHA1Reset(int n);

} // namespace devilution

/**
 * @file sha.cpp
 *
 * Interface of functionality for calculating X-SHA-1 (a flawed implementation of SHA-1).
 */
#pragma once

namespace devilution {

#ifdef __cplusplus
extern "C" {
#endif

#define SHA1HashSize 20

typedef struct SHA1Context {
	Uint32 state[5];
	Uint32 count[2];
	char buffer[64];
} SHA1Context;

void SHA1Clear();
void SHA1Result(int n, char Message_Digest[SHA1HashSize]);
void SHA1Calculate(int n, const char *data, char Message_Digest[SHA1HashSize]);
void SHA1Reset(int n);

#ifdef __cplusplus
}
#endif

}

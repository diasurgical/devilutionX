#include "platform/switch/random.hpp"

#include <sys/types.h>

#include <sodium.h>

extern "C" {
#include <switch/services/csrng.h>
#include <switch/sf/service.h>
}

static const char *randombytes_switchrandom_implementation_name()
{
	return "switchrandom";
}

static bool randombytes_switchrandom_tryfill(void *const buf, const size_t size)
{
	Result res;
	Service *csrngService = csrngGetServiceSession();
	if (!serviceIsActive(csrngService)) {
		res = csrngInitialize();
		if (!R_SUCCEEDED(res))
			return false;
	}
	res = csrngGetRandomBytes(buf, size);
	return R_SUCCEEDED(res);
}

static uint32_t randombytes_switchrandom()
{
	uint32_t num;
	if (!randombytes_switchrandom_tryfill(&num, sizeof(uint32_t)))
		sodium_misuse();
	return num;
}

static void randombytes_switchrandom_buf(void *const buf, const size_t size)
{
	if (!randombytes_switchrandom_tryfill(buf, size))
		sodium_misuse();
}

struct randombytes_implementation randombytes_switchrandom_implementation = {
	randombytes_switchrandom_implementation_name,
	randombytes_switchrandom,
	nullptr,
	nullptr,
	randombytes_switchrandom_buf,
	nullptr
};

void randombytes_switchrandom_init()
{
	randombytes_set_implementation(&randombytes_switchrandom_implementation);
	atexit(csrngExit);
}

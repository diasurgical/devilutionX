#include <psp2/kernel/rng.h>
#include <sodium.h>
#include <sys/types.h>

static const char *randombytes_vitarandom_implementation_name()
{
	return "vitarandom";
}

static uint32_t randombytes_vitarandom()
{
	uint32_t num;
	sceKernelGetRandomNumber(&num, sizeof(uint32_t));
	return num;
}

static void randombytes_vitarandom_buf(void *const buf, const size_t size)
{
	sceKernelGetRandomNumber(buf, size);
}

struct randombytes_implementation randombytes_vitarandom_implementation = {
	randombytes_vitarandom_implementation_name,
	randombytes_vitarandom,
	nullptr,
	nullptr,
	randombytes_vitarandom_buf,
	nullptr
};

void randombytes_vitarandom_init()
{
	randombytes_set_implementation(&randombytes_vitarandom_implementation);
}

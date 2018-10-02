#include "Stub/asserts.h"
#include <assert.h>
#include <cstdio>
#include <sys/mman.h>

#include "hook.h"

#pragma pack(push, 1)
struct hook_jmp_rel32 {
	uint8_t op;
	int32_t offset;
};
#pragma pack(pop)

static void hook_assemble_jump(void *at, void *to)
{
	hook_jmp_rel32 *jmp = (hook_jmp_rel32 *)at;

	jmp->op = 0xE9;
	jmp->offset = (intptr_t)to - ((intptr_t)at + sizeof(*jmp));
}

static int hook_unprotect(void *address, size_t size)
{
	// Align to bottom of page
	address = (void *)((uintptr_t)address & ~(4096 - 1));
	return mprotect(address, size, PROT_READ | PROT_WRITE | PROT_EXEC);
}

void hook(void *at, void *to)
{
	ASSERT(hook_unprotect(at, sizeof(hook_jmp_rel32)) == 0);
	hook_assemble_jump(at, to);
}

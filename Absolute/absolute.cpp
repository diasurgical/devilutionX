#include <assert.h>
#include <string.h>
#include <sys/mman.h>

#include "Stub/stubs.h"
#include "absolute.h"
#include "hook.h"

static void missing_import()
{
	UNIMPLEMENTED();
}

static void link_imports()
{
#define LINK_IMPORT(imp_name, name) imp_name = (void *)name;
#define MISSING_IMPORT(imp_name, name) imp_name = (void *)missing_import;

#include "absolute-imports.h"
}

static void link_stubs()
{
#define LINK_STUB(orig, dest) hook((void *)orig, (void *)dest);
#include "absolute-stubs.h"

	LINK_STUB(d_srand, srand_miniwin);
	LINK_STUB(d_rand, rand_miniwin);
	LINK_STUB(d_time, time);

	LINK_STUB(d_sprintf, sprintf); // Calls into kernel

	// These functions potentially access uninitialized memory
	LINK_STUB(d_strcat, strcat);
	LINK_STUB(d_strcpy, strcpy);
	LINK_STUB(d_strlen, strlen);

	// strchr() is overloaded in glibc C++ mode, pick right function....
	char *(*strchr_overload)(char *, int) = strchr;
	LINK_STUB(d_strchr, strchr_overload);
}

static void protect_rdata(int prot)
{
	mprotect(&__start_rdata, &__end_rdata - &__start_rdata, prot);
}

void init_absolute()
{
	protect_rdata(PROT_READ | PROT_WRITE);
	link_imports();
	protect_rdata(PROT_READ);

	link_stubs();
}
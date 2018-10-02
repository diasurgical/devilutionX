#pragma once

#include <assert.h>
#include <stdio.h>
#include <stdlib.h>

#define ASSERT(x) \
	if (!(x)) { \
		fprintf(stderr, "Assertion failed in %s:%d: %s\n", __FILE__, __LINE__, #x); \
		abort(); \
	}

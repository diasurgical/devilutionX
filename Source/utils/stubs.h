#pragma once

#include "utils/log.hpp"

#define UNIMPLEMENTED()                                                                        \
	do {                                                                                       \
		::devilution::LogDebug("UNIMPLEMENTED: {} @ {}:{}", __FUNCTION__, __FILE__, __LINE__); \
		abort();                                                                               \
	} while (0)

#define ABORT()                                                                           \
	do {                                                                                  \
		::devilution::LogCritical("ABORT: {} @ {}:{}", __FUNCTION__, __FILE__, __LINE__); \
		abort();                                                                          \
	} while (0)

#define ASSERT(x)                                                                           \
	if (!(x)) {                                                                             \
		::devilution::LogCritical("Assertion failed in {}:{}: {}", __FILE__, __LINE__, #x); \
		abort();                                                                            \
	}

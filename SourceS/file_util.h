#pragma once

#if _POSIX_C_SOURCE >= 200112L
#include <unistd.h>
#else
#include <cstdio>
#endif

namespace dvl {

inline bool FileExists(const char *path)
{
#if _POSIX_C_SOURCE >= 200112L
	return ::access(path, F_OK) == 0;
#else
	auto *file = std::fopen(path, "rb");
	if (file == nullptr) return false;
	std::fclose(file);
	return true;
#endif
}

} // namespace dvl

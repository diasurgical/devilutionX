#pragma once

#include <array>
#include <memory>

#include "appfat.h"
#include "utils/stdcompat/cstddef.hpp"

namespace devilution {

size_t GetFileSize(const char *pszName);

void LoadFileData(const char *pszName, byte *buffer, size_t fileLen);

template <typename T>
void LoadFileInMem(const char *path, T *data, std::size_t count = 0)
{
	if (count == 0)
		count = GetFileSize(path);

	LoadFileData(path, reinterpret_cast<byte *>(data), count * sizeof(T));
}

template <typename T, std::size_t N>
void LoadFileInMem(const char *path, std::array<T, N> &data)
{
	LoadFileInMem(path, &data, N);
}

/**
 * @brief Load a file in to a buffer
 * @param path Path of file
 * @param elements Number of T elements read
 * @return Buffer with content of file
 */
template <typename T = byte>
std::unique_ptr<T[]> LoadFileInMem(const char *path, size_t *elements = nullptr)
{
	const size_t fileLen = GetFileSize(path);

	if ((fileLen % sizeof(T)) != 0)
		app_fatal("File size does not align with type\n%s", path);

	if (elements != nullptr)
		*elements = fileLen / sizeof(T);

	std::unique_ptr<T[]> buf { new T[fileLen / sizeof(T)] };

	LoadFileData(path, reinterpret_cast<byte *>(buf.get()), fileLen);

	return buf;
}

} // namespace devilution

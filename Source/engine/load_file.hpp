#pragma once

#include <array>
#include <cstddef>
#include <cstdint>
#include <memory>

#include <fmt/core.h>

#include "appfat.h"
#include "diablo.h"
#include "engine/assets.hpp"
#include "utils/static_vector.hpp"
#include "utils/stdcompat/cstddef.hpp"
#include "utils/str_cat.hpp"

namespace devilution {

template <typename T>
void LoadFileInMem(const char *path, T *data)
{
	size_t size;
	AssetHandle handle = OpenAsset(path, size);
	if (!ValidateHandle(path, handle))
		return;
	if ((size % sizeof(T)) != 0)
		app_fatal(StrCat("File size does not align with type\n", path));
	handle.read(data, size);
}

template <typename T>
void LoadFileInMem(const char *path, T *data, std::size_t count)
{
	AssetHandle handle = OpenAsset(path);
	if (!ValidateHandle(path, handle))
		return;
	handle.read(data, count * sizeof(T));
}

template <typename T>
bool LoadOptionalFileInMem(const char *path, T *data, std::size_t count)
{
	AssetHandle handle = OpenAsset(path);
	return handle.ok() && handle.read(data, count * sizeof(T));
}

template <typename T, std::size_t N>
void LoadFileInMem(const char *path, std::array<T, N> &data)
{
	LoadFileInMem(path, data.data(), N);
}

/**
 * @brief Load a file in to a buffer
 * @param path Path of file
 * @param numRead Number of T elements read
 * @return Buffer with content of file
 */
template <typename T = byte>
std::unique_ptr<T[]> LoadFileInMem(const char *path, std::size_t *numRead = nullptr)
{
	size_t size;
	AssetHandle handle = OpenAsset(path, size);
	if (!ValidateHandle(path, handle))
		return nullptr;
	if ((size % sizeof(T)) != 0)
		app_fatal(StrCat("File size does not align with type\n", path));

	if (numRead != nullptr)
		*numRead = size / sizeof(T);

	std::unique_ptr<T[]> buf { new T[size / sizeof(T)] };
	handle.read(buf.get(), size);
	return buf;
}

/**
 * @brief Reads multiple files into a single buffer
 *
 * @tparam MaxFiles maximum number of files
 */
template <size_t MaxFiles>
struct MultiFileLoader {
	struct DefaultFilterFn {
		bool operator()(size_t i) const
		{
			return true;
		}
	};

	/**
	 * @param numFiles number of files to read
	 * @param pathFn a function that returns the path for the given index
	 * @param outOffsets a buffer index for the start of each file will be written here, then the total file size at the end.
	 * @param filterFn a function that returns whether to load a file for the given index
	 * @return std::unique_ptr<byte[]> the buffer with all the files
	 */
	template <typename PathFn, typename FilterFn = DefaultFilterFn>
	[[nodiscard]] std::unique_ptr<byte[]> operator()(size_t numFiles, PathFn &&pathFn, uint32_t *outOffsets,
	    FilterFn filterFn = DefaultFilterFn {})
	{
		StaticVector<AssetHandle, MaxFiles> files;
		StaticVector<uint32_t, MaxFiles> sizes;
		size_t totalSize = 0;
		for (size_t i = 0, j = 0; i < numFiles; ++i) {
			if (!filterFn(i))
				continue;
			size_t size;
			const char *path = pathFn(i);
			files.emplace_back(OpenAsset(path, size));
			if (!ValidateHandle(path, files.back()))
				return nullptr;

			sizes.emplace_back(static_cast<uint32_t>(size));
			outOffsets[j] = static_cast<uint32_t>(totalSize);
			totalSize += size;
			++j;
		}
		outOffsets[files.size()] = totalSize;
		std::unique_ptr<byte[]> buf { new byte[totalSize] };
		for (size_t i = 0, j = 0; i < numFiles; ++i) {
			if (!filterFn(i))
				continue;
			files[j].read(&buf[outOffsets[j]], sizes[j]);
			++j;
		}
		return buf;
	}
};

} // namespace devilution

#pragma once

#include <array>
#include <cstddef>
#include <cstdint>
#include <memory>

#include "appfat.h"
#include "diablo.h"
#include "engine/assets.hpp"
#include "utils/static_vector.hpp"
#include "utils/stdcompat/cstddef.hpp"

namespace devilution {

class SFile {
public:
	explicit SFile(const char* path)
	{
		handle_ = OpenAsset(path);

		if (handle_ == nullptr) {
			if (!gbQuietMode) {
				app_fatal("Failed to open file:\n%s\n\n%s", path, SDL_GetError());
			}
		}
	}

	~SFile()
	{
		if (handle_ != nullptr)
			SDL_RWclose(handle_);
	}

	[[nodiscard]] bool Ok() const
	{
		return handle_ != nullptr;
	}

	[[nodiscard]] std::size_t Size() const
	{
		return SDL_RWsize(handle_);
	}

	bool Read(void *buffer, std::size_t len) const
	{
		return SDL_RWread(handle_, buffer, len, 1);
	}

private:
	SDL_RWops *handle_;
};

template <typename T>
void LoadFileInMem(const char *path, T *data)
{
	SFile file { path };
	if (!file.Ok())
		return;
	const std::size_t fileLen = file.Size();
	if ((fileLen % sizeof(T)) != 0)
		app_fatal("File size does not align with type\n%s", path);
	file.Read(reinterpret_cast<byte *>(data), fileLen);
}

template <typename T>
void LoadFileInMem(const char *path, T *data, std::size_t count)
{
	SFile file { path };
	if (!file.Ok())
		return;
	file.Read(reinterpret_cast<byte *>(data), count * sizeof(T));
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
	SFile file { path };
	if (!file.Ok())
		return nullptr;
	const std::size_t fileLen = file.Size();
	if ((fileLen % sizeof(T)) != 0)
		app_fatal("File size does not align with type\n%s", path);

	if (numRead != nullptr)
		*numRead = fileLen / sizeof(T);

	std::unique_ptr<T[]> buf { new T[fileLen / sizeof(T)] };
	file.Read(reinterpret_cast<byte *>(buf.get()), fileLen);
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
	 * @param outOffsets a buffer index for the start of each file will be written here
	 * @param filterFn a function that returns whether to load a file for the given index
	 * @return std::unique_ptr<byte[]> the buffer with all the files
	 */
	template <typename PathFn, typename FilterFn = DefaultFilterFn>
	[[nodiscard]] std::unique_ptr<byte[]> operator()(size_t numFiles, PathFn &&pathFn, uint32_t *outOffsets,
	    FilterFn filterFn = DefaultFilterFn {})
	{
		StaticVector<SFile, MaxFiles> files;
		StaticVector<uint32_t, MaxFiles> sizes;
		size_t totalSize = 0;
		for (size_t i = 0; i < numFiles; ++i) {
			if (!filterFn(i))
				continue;
			const size_t size = files.emplace_back(pathFn(i)).Size();
			sizes.emplace_back(static_cast<uint32_t>(size));
			outOffsets[i] = static_cast<uint32_t>(totalSize);
			totalSize += size;
		}
		std::unique_ptr<byte[]> buf { new byte[totalSize] };
		size_t j = 0;
		for (size_t i = 0; i < numFiles; ++i) {
			if (!filterFn(i))
				continue;
			files[j].Read(&buf[outOffsets[i]], sizes[j]);
			++j;
		}
		return buf;
	}
};

} // namespace devilution

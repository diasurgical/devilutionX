#pragma once

#include <array>
#include <cstddef>
#include <cstdint>
#include <memory>

#include "appfat.h"
#include "diablo.h"
#include "engine/assets.hpp"
#include "utils/stdcompat/cstddef.hpp"

namespace devilution {

class SFile {
public:
	explicit SFile(const char *path)
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

} // namespace devilution

#pragma once

#include <array>
#include <cstddef>
#include <cstdint>
#include <memory>

#include "appfat.h"
#include "diablo.h"
#include "storm/storm.h"
#include "utils/stdcompat/cstddef.hpp"

namespace devilution {

class SFile {
public:
	explicit SFile(const char *path)
	{
		if (!SFileOpenFile(path, &handle_)) {
			handle_ = nullptr;
			if (!gbQuietMode) {
				const std::uint32_t code = SErrGetLastError();
				if (code == STORM_ERROR_FILE_NOT_FOUND) {
					app_fatal("Failed to open file:\n%s\n\nFile not found", path);
				} else {
					app_fatal("Failed to open file:\n%s\n\nError Code: %u", path, code);
				}
			}
		}
	}

	~SFile()
	{
		if (handle_ != nullptr)
			SFileCloseFileThreadSafe(handle_);
	}

	[[nodiscard]] bool Ok() const
	{
		return handle_ != nullptr;
	}

	[[nodiscard]] std::size_t Size() const
	{
		return SFileGetFileSize(handle_);
	}

	bool Read(void *buffer, std::size_t len) const
	{
		return SFileReadFileThreadSafe(handle_, buffer, len);
	}

private:
	HANDLE handle_;
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
	LoadFileInMem(path, &data, N);
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

#pragma once

#include <cstddef>
#include <cstdint>
#include <string>

#include <SDL.h>

#include "appfat.h"
#include "diablo.h"
#include "mpq/mpq_reader.hpp"
#include "utils/file_util.h"
#include "utils/str_cat.hpp"
#include "utils/string_or_view.hpp"

namespace devilution {

#ifdef UNPACKED_MPQS
struct AssetRef {
	std::string path;

	[[nodiscard]] bool ok() const
	{
		return !path.empty();
	}

	// NOLINTNEXTLINE(readability-convert-member-functions-to-static)
	[[nodiscard]] const char *error() const
	{
		return "File not found";
	}

	[[nodiscard]] size_t size() const
	{
		uintmax_t fileSize;
		if (!GetFileSize(path.c_str(), &fileSize))
			return 0;
		return fileSize;
	}
};

struct AssetHandle {
	std::optional<std::fstream> handle;

	[[nodiscard]] bool ok() const
	{
		return handle && !handle->fail();
	}

	bool read(void *buffer, size_t len)
	{
		handle->read(static_cast<char *>(buffer), len);
		return !handle->fail();
	}

	bool seek(std::ios::pos_type pos)
	{
		handle->seekg(pos);
		return !handle->fail();
	}

	[[nodiscard]] const char *error() const
	{
		return std::strerror(errno);
	}
};
#else
struct AssetRef {
	// An MPQ file reference:
	MpqArchive *archive = nullptr;
	uint32_t fileNumber;
	const char *filename;

	// Alternatively, a direct SDL_RWops handle:
	SDL_RWops *directHandle = nullptr;

	AssetRef() = default;

	AssetRef(AssetRef &&other) noexcept
	    : archive(other.archive)
	    , fileNumber(other.fileNumber)
	    , filename(other.filename)
	    , directHandle(other.directHandle)
	{
		other.directHandle = nullptr;
	}

	AssetRef &operator=(AssetRef &&other) noexcept
	{
		if (directHandle != nullptr)
			SDL_RWclose(directHandle);
		archive = other.archive;
		fileNumber = other.fileNumber;
		filename = other.filename;
		directHandle = other.directHandle;
		other.directHandle = nullptr;
		return *this;
	}

	~AssetRef()
	{
		if (directHandle != nullptr)
			SDL_RWclose(directHandle);
	}

	[[nodiscard]] bool ok() const
	{
		return directHandle != nullptr || archive != nullptr;
	}

	// NOLINTNEXTLINE(readability-convert-member-functions-to-static)
	[[nodiscard]] const char *error() const
	{
		return SDL_GetError();
	}

	[[nodiscard]] size_t size() const
	{
		if (archive != nullptr) {
			int32_t error;
			return archive->GetUnpackedFileSize(fileNumber, error);
		}
		return SDL_RWsize(directHandle);
	}
};

struct AssetHandle {
	SDL_RWops *handle = nullptr;

	AssetHandle() = default;

	explicit AssetHandle(SDL_RWops *handle)
	    : handle(handle)
	{
	}

	AssetHandle(AssetHandle &&other) noexcept
	    : handle(other.handle)
	{
		other.handle = nullptr;
	}

	AssetHandle &operator=(AssetHandle &&other) noexcept
	{
		if (handle != nullptr) {
			SDL_RWclose(handle);
		}
		handle = other.handle;
		other.handle = nullptr;
		return *this;
	}

	~AssetHandle()
	{
		if (handle != nullptr)
			SDL_RWclose(handle);
	}

	[[nodiscard]] bool ok() const
	{
		return handle != nullptr;
	}

	bool read(void *buffer, size_t len)
	{
		return handle->read(handle, buffer, len, 1) == 1;
	}

	bool seek(std::ios::pos_type pos)
	{
		return handle->seek(handle, pos, RW_SEEK_SET) != -1;
	}

	[[nodiscard]] const char *error() const
	{
		return SDL_GetError();
	}

	SDL_RWops *release() &&
	{
		SDL_RWops *result = handle;
		handle = nullptr;
		return result;
	}
};
#endif

inline bool ValidateHandle(const char *path, const AssetHandle &handle)
{
	if (handle.ok())
		return true;
	if (!HeadlessMode) {
		app_fatal(StrCat("Failed to open file:\n", path, "\n\n", handle.error()));
	}
	return false;
}

AssetRef FindAsset(const char *filename);

AssetHandle OpenAsset(AssetRef &&ref, bool threadsafe = false);
AssetHandle OpenAsset(const char *filename, bool threadsafe = false);
AssetHandle OpenAsset(const char *filename, size_t &fileSize, bool threadsafe = false);

SDL_RWops *OpenAssetAsSdlRwOps(const char *filename, bool threadsafe = false);

} // namespace devilution

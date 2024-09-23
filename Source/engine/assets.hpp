#pragma once

#include <cstddef>
#include <cstdint>
#include <cstdio>
#include <string>
#include <string_view>

#include <SDL.h>
#include <expected.hpp>

#include <fmt/format.h>

#include "appfat.h"
#include "diablo.h"
#include "mpq/mpq_reader.hpp"
#include "utils/file_util.h"
#include "utils/language.h"
#include "utils/str_cat.hpp"
#include "utils/string_or_view.hpp"

namespace devilution {

#ifdef UNPACKED_MPQS
struct AssetRef {
	static constexpr size_t PathBufSize = 4088;

	char path[PathBufSize];

	[[nodiscard]] bool ok() const
	{
		return path[0] != '\0';
	}

	// NOLINTNEXTLINE(readability-convert-member-functions-to-static)
	[[nodiscard]] const char *error() const
	{
		return "File not found";
	}

	[[nodiscard]] size_t size() const
	{
		uintmax_t fileSize;
		if (!GetFileSize(path, &fileSize))
			return 0;
		return fileSize;
	}
};

struct AssetHandle {
	FILE *handle = nullptr;

	AssetHandle() = default;

	AssetHandle(FILE *handle)
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
		handle = other.handle;
		other.handle = nullptr;
		return *this;
	}

	~AssetHandle()
	{
		if (handle != nullptr)
			std::fclose(handle);
	}

	[[nodiscard]] bool ok() const
	{
		return handle != nullptr && std::ferror(handle) == 0;
	}

	bool read(void *buffer, size_t len)
	{
		return std::fread(buffer, len, 1, handle) == 1;
	}

	bool seek(long pos)
	{
		return std::fseek(handle, pos, SEEK_SET) == 0;
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
	std::string_view filename;

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
		return static_cast<size_t>(SDL_RWsize(directHandle));
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
#if SDL_VERSION_ATLEAST(2, 0, 0)
		return handle->read(handle, buffer, len, 1) == 1;
#else
		return handle->read(handle, buffer, static_cast<int>(len), 1) == 1;
#endif
	}

	bool seek(long pos)
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

[[noreturn]] inline void FailedToOpenFileError(std::string_view path, std::string_view error)
{
	app_fatal(fmt::format(fmt::runtime(_("Failed to open file:\n{:s}\n\n{:s}\n\nThe MPQ file(s) might be damaged. Please check the file integrity.")), path, error));
}

inline bool ValidatAssetRef(std::string_view path, const AssetRef &ref)
{
	if (ref.ok())
		return true;
	if (!HeadlessMode) {
		FailedToOpenFileError(path, ref.error());
	}
	return false;
}

inline bool ValidateHandle(std::string_view path, const AssetHandle &handle)
{
	if (handle.ok())
		return true;
	if (!HeadlessMode) {
		FailedToOpenFileError(path, handle.error());
	}
	return false;
}

AssetRef FindAsset(std::string_view filename);

AssetHandle OpenAsset(AssetRef &&ref, bool threadsafe = false);
AssetHandle OpenAsset(std::string_view filename, bool threadsafe = false);
AssetHandle OpenAsset(std::string_view filename, size_t &fileSize, bool threadsafe = false);

SDL_RWops *OpenAssetAsSdlRwOps(std::string_view filename, bool threadsafe = false);

struct AssetData {
	std::unique_ptr<char[]> data;
	size_t size;

	explicit operator std::string_view() const
	{
		return std::string_view(data.get(), size);
	}
};

tl::expected<AssetData, std::string> LoadAsset(std::string_view path);

} // namespace devilution

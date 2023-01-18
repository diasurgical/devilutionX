#include "engine/assets.hpp"

#include <algorithm>
#include <cstdint>
#include <cstring>

#include "init.h"
#include "utils/file_util.h"
#include "utils/log.hpp"
#include "utils/paths.h"
#include "utils/str_cat.hpp"

#ifndef UNPACKED_MPQS
#include "mpq/mpq_sdl_rwops.hpp"
#endif

namespace devilution {

namespace {

#ifdef UNPACKED_MPQS
char *FindUnpackedMpqFile(char *relativePath)
{
	char *path = nullptr;
	const auto at = [&](const std::optional<std::string> &unpackedDir) -> bool {
		if (!unpackedDir)
			return false;
		path = relativePath - unpackedDir->size();
		std::memcpy(path, unpackedDir->data(), unpackedDir->size());
		if (FileExists(path))
			return true;
		path = nullptr;
		return false;
	};
	at(font_data_path) || at(lang_data_path)
	    || (gbIsHellfire && at(hellfire_data_path))
	    || at(spawn_data_path) || at(diabdat_data_path);
	return path;
}
#else
bool IsDebugLogging()
{
	return SDL_LOG_PRIORITY_DEBUG >= SDL_LogGetPriority(SDL_LOG_CATEGORY_APPLICATION);
}

SDL_RWops *OpenOptionalRWops(const std::string &path)
{
	// SDL always logs an error in Debug mode.
	// We check the file presence in Debug mode to avoid this.
	if (IsDebugLogging() && !FileExists(path.c_str()))
		return nullptr;
	return SDL_RWFromFile(path.c_str(), "rb");
};

bool FindMpqFile(const char *filename, MpqArchive **archive, uint32_t *fileNumber)
{
	const MpqArchive::FileHash fileHash = MpqArchive::CalculateFileHash(filename);
	const auto at = [=](std::optional<MpqArchive> &src) -> bool {
		if (src && src->GetFileNumber(fileHash, *fileNumber)) {
			*archive = &(*src);
			return true;
		}
		return false;
	};

	return at(font_mpq) || at(lang_mpq) || at(devilutionx_mpq)
	    || (gbIsHellfire && (at(hfvoice_mpq) || at(hfmusic_mpq) || at(hfbarb_mpq) || at(hfbard_mpq) || at(hfmonk_mpq) || at(hellfire_mpq))) || at(spawn_mpq) || at(diabdat_mpq);
}
#endif

} // namespace

#ifdef UNPACKED_MPQS
AssetRef FindAsset(const char *filename)
{
	AssetRef result;
	result.path[0] = '\0';

	const string_view filenameStr = filename;
	char pathBuf[AssetRef::PathBufSize];
	char *const pathEnd = pathBuf + AssetRef::PathBufSize;
	char *const relativePath = &pathBuf[AssetRef::PathBufSize - filenameStr.size() - 1];
	*BufCopy(relativePath, filenameStr) = '\0';

#ifndef _WIN32
	std::replace(relativePath, pathEnd, '\\', '/');
#endif
	// Absolute path:
	if (relativePath[0] == '/') {
		if (FileExists(relativePath)) {
			*BufCopy(result.path, string_view(relativePath, filenameStr.size())) = '\0';
		}
		return result;
	}

	// Unpacked MPQ file:
	char *const unpackedMpqPath = FindUnpackedMpqFile(relativePath);
	if (unpackedMpqPath != nullptr) {
		*BufCopy(result.path, string_view(unpackedMpqPath, pathEnd - unpackedMpqPath)) = '\0';
		return result;
	}

	// The `/assets` directory next to the devilutionx binary.
	const std::string &assetsPathPrefix = paths::AssetsPath();
	char *assetsPath = relativePath - assetsPathPrefix.size();
	std::memcpy(assetsPath, assetsPathPrefix.data(), assetsPathPrefix.size());
	if (FileExists(assetsPath)) {
		*BufCopy(result.path, string_view(assetsPath, pathEnd - assetsPath)) = '\0';
	}
	return result;
}
#else
AssetRef FindAsset(const char *filename)
{
	AssetRef result;
	std::string relativePath = filename;
#ifndef _WIN32
	std::replace(relativePath.begin(), relativePath.end(), '\\', '/');
#endif

	if (relativePath[0] == '/') {
		result.directHandle = SDL_RWFromFile(relativePath.c_str(), "rb");
		if (result.directHandle != nullptr) {
			return result;
		}
	}

	// Files in the `PrefPath()` directory can override MPQ contents.
	{
		const std::string path = paths::PrefPath() + relativePath;
		result.directHandle = OpenOptionalRWops(path);
		if (result.directHandle != nullptr) {
			LogVerbose("Loaded MPQ file override: {}", path);
			return result;
		}
	}

	// Look for the file in all the MPQ archives:
	if (FindMpqFile(filename, &result.archive, &result.fileNumber)) {
		result.filename = filename;
		return result;
	}

	// Load from the `/assets` directory next to the devilutionx binary.
	result.directHandle = OpenOptionalRWops(paths::AssetsPath() + relativePath);
	if (result.directHandle != nullptr)
		return result;

#if defined(__ANDROID__) || defined(__APPLE__)
	// Fall back to the bundled assets on supported systems.
	// This is handled by SDL when we pass a relative path.
	if (!paths::AssetsPath().empty()) {
		result.directHandle = SDL_RWFromFile(relativePath.c_str(), "rb");
		if (result.directHandle != nullptr)
			return result;
	}
#endif

	return result;
}
#endif

AssetHandle OpenAsset(AssetRef &&ref, bool threadsafe)
{
#if UNPACKED_MPQS
	return AssetHandle { OpenFile(ref.path, "rb") };
#else
	if (ref.archive != nullptr)
		return AssetHandle { SDL_RWops_FromMpqFile(*ref.archive, ref.fileNumber, ref.filename, threadsafe) };
	if (ref.directHandle != nullptr) {
		// Transfer handle ownership:
		SDL_RWops *handle = ref.directHandle;
		ref.directHandle = nullptr;
		return AssetHandle { handle };
	}
	return AssetHandle { nullptr };
#endif
}

AssetHandle OpenAsset(const char *filename, bool threadsafe)
{
	AssetRef ref = FindAsset(filename);
	if (!ref.ok())
		return AssetHandle {};
	return OpenAsset(std::move(ref), threadsafe);
}

AssetHandle OpenAsset(const char *filename, size_t &fileSize, bool threadsafe)
{
	AssetRef ref = FindAsset(filename);
	if (!ref.ok())
		return AssetHandle {};
	fileSize = ref.size();
	return OpenAsset(std::move(ref), threadsafe);
}

SDL_RWops *OpenAssetAsSdlRwOps(const char *filename, bool threadsafe)
{
#ifdef UNPACKED_MPQS
	AssetRef ref = FindAsset(filename);
	if (!ref.ok())
		return nullptr;
	return SDL_RWFromFile(ref.path, "rb");
#else
	return OpenAsset(filename, threadsafe).release();
#endif
}

} // namespace devilution

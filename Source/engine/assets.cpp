#include "engine/assets.hpp"

#include <algorithm>
#include <cstdint>
#include <cstring>

#include "init.h"
#include "mpq/mpq_sdl_rwops.hpp"
#include "utils/file_util.h"
#include "utils/log.hpp"
#include "utils/paths.h"

namespace devilution {

namespace {

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

#ifdef UNPACKED_MPQS
SDL_RWops *OpenUnpackedMpqFile(const std::string &relativePath)
{
	SDL_RWops *result = nullptr;
	std::string tmpPath;
	const auto at = [&](const std::optional<std::string> &unpackedDir) -> bool {
		if (!unpackedDir)
			return false;
		tmpPath.clear();
		tmpPath.reserve(unpackedDir->size() + relativePath.size());
		result = OpenOptionalRWops(tmpPath.append(*unpackedDir).append(relativePath));
		return result != nullptr;
	};
	at(font_data_path) || at(lang_data_path)
	    || (gbIsHellfire && at(hellfire_data_path))
	    || at(spawn_data_path) || at(diabdat_data_path);
	return result;
}
#else
bool OpenMpqFile(const char *filename, MpqArchive **archive, uint32_t *fileNumber)
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

SDL_RWops *OpenAsset(const char *filename, bool threadsafe)
{
	std::string relativePath = filename;
#ifndef _WIN32
	std::replace(relativePath.begin(), relativePath.end(), '\\', '/');
#endif

	if (relativePath[0] == '/')
		return SDL_RWFromFile(relativePath.c_str(), "rb");

	SDL_RWops *rwops;

	// Files in the `PrefPath()` directory can override MPQ contents.
	{
		const std::string path = paths::PrefPath() + relativePath;
		if ((rwops = OpenOptionalRWops(path)) != nullptr) {
			LogVerbose("Loaded MPQ file override: {}", path);
			return rwops;
		}
	}

	// Load from all the MPQ archives.
#ifdef UNPACKED_MPQS
	if ((rwops = OpenUnpackedMpqFile(relativePath)) != nullptr)
		return rwops;
#else
	MpqArchive *archive;
	uint32_t fileNumber;
	if (OpenMpqFile(filename, &archive, &fileNumber))
		return SDL_RWops_FromMpqFile(*archive, fileNumber, filename, threadsafe);
#endif

	// Load from the `/assets` directory next to the devilutionx binary.
	if ((rwops = OpenOptionalRWops(paths::AssetsPath() + relativePath)))
		return rwops;

#if defined(__ANDROID__) || defined(__APPLE__)
	// Fall back to the bundled assets on supported systems.
	// This is handled by SDL when we pass a relative path.
	if (!paths::AssetsPath().empty() && (rwops = SDL_RWFromFile(relativePath.c_str(), "rb")))
		return rwops;
#endif

	return nullptr;
}

} // namespace devilution

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

	// SDL always logs an error in Debug mode.
	// We check the file presence in Debug mode to avoid this.
	const bool isDebug = SDL_LOG_PRIORITY_DEBUG >= SDL_LogGetPriority(SDL_LOG_CATEGORY_APPLICATION);

	const auto loadFile = [&rwops, isDebug](const std::string &path) {
		return (!isDebug || FileExists(path.c_str()))
		    && (rwops = SDL_RWFromFile(path.c_str(), "rb")) != nullptr;
	};

	// Files in the `PrefPath()` directory can override MPQ contents.
	{
		const std::string path = paths::PrefPath() + relativePath;
		if (loadFile(path)) {
			LogVerbose("Loaded MPQ file override: {}", path);
			return rwops;
		}
	}

	// Load from all the MPQ archives.
	MpqArchive *archive;
	uint32_t fileNumber;
	if (OpenMpqFile(filename, &archive, &fileNumber))
		return SDL_RWops_FromMpqFile(*archive, fileNumber, filename, threadsafe);

	// Load from the `/assets` directory next to the devilutionx binary.
	if (loadFile(paths::AssetsPath() + relativePath))
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

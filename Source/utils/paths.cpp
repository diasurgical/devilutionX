#include "utils/paths.h"

#include <SDL.h>

#include "utils/file_util.h"
#include "utils/log.hpp"
#include "utils/sdl_ptrs.h"

#ifdef __IPHONEOS__
#include "platform/ios/ios_paths.h"
#endif

#ifdef USE_SDL1
#include "utils/sdl2_to_1_2_backports.h"
#endif

namespace devilution {

namespace paths {

namespace {

std::optional<std::string> basePath;
std::optional<std::string> prefPath;
std::optional<std::string> configPath;
std::optional<std::string> assetsPath;
std::optional<std::string> mpqDir;

void AddTrailingSlash(std::string &path)
{
#ifdef _WIN32
	if (!path.empty() && path.back() != '\\')
		path += '\\';
#else
	if (!path.empty() && path.back() != '/')
		path += '/';
#endif
}

std::string FromSDL(char *s)
{
	SDLUniquePtr<char> pinned(s);
	std::string result = (s != nullptr ? s : "");
	if (s == nullptr) {
		Log("{}", SDL_GetError());
		SDL_ClearError();
	}
	return result;
}

} // namespace

const std::string &BasePath()
{
	if (!basePath) {
		basePath = FromSDL(SDL_GetBasePath());
	}
	return *basePath;
}

const std::string &PrefPath()
{
	if (!prefPath) {
#ifndef __IPHONEOS__
		prefPath = FromSDL(SDL_GetPrefPath("diasurgical", "devilution"));
		if (FileExistsAndIsWriteable("diablo.ini")) {
			prefPath = std::string("./");
		}
#else
		prefPath = FromSDL(IOSGetPrefPath());
#endif
	}
	return *prefPath;
}

const std::string &ConfigPath()
{
	if (!configPath) {
#ifndef __IPHONEOS__
		configPath = FromSDL(SDL_GetPrefPath("diasurgical", "devilution"));
		if (FileExistsAndIsWriteable("diablo.ini")) {
			configPath = std::string("./");
		}
#else
		configPath = FromSDL(IOSGetPrefPath());
#endif
	}
	return *configPath;
}

const std::string &AssetsPath()
{
	if (!assetsPath)
#if __EMSCRIPTEN__
		assetsPath.emplace("assets/");
#else
		assetsPath.emplace(FromSDL(SDL_GetBasePath()) + "assets/");
#endif
	return *assetsPath;
}

const std::optional<std::string> &MpqDir()
{
	return mpqDir;
}

void SetBasePath(const std::string &path)
{
	basePath = path;
	AddTrailingSlash(*basePath);
}

void SetPrefPath(const std::string &path)
{
	prefPath = path;
	AddTrailingSlash(*prefPath);
}

void SetConfigPath(const std::string &path)
{
	configPath = path;
	AddTrailingSlash(*configPath);
}

void SetAssetsPath(const std::string &path)
{
	assetsPath = path;
	AddTrailingSlash(*assetsPath);
}

void SetMpqDir(const std::string &path)
{
	mpqDir = std::string(path);
}

} // namespace paths

} // namespace devilution

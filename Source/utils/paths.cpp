#include "utils/paths.h"

#include <SDL.h>

#include "utils/file_util.h"
#include "utils/log.hpp"
#include "utils/stdcompat/optional.hpp"
#include "utils/sdl_ptrs.h"

#ifdef USE_SDL1
#include "utils/sdl2_to_1_2_backports.h"
#endif

#ifndef MO_LANG_DIR
#define MO_LANG_DIR ""
#endif

namespace devilution {

namespace paths {

namespace {

std::optional<std::string> basePath;
std::optional<std::string> prefPath;
std::optional<std::string> configPath;
std::optional<std::string> langPath;

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
		prefPath = FromSDL(SDL_GetPrefPath("diasurgical", "devilution"));
		if (FileExistsAndIsWriteable("diablo.ini")) {
			prefPath = std::string("./");
		}
	}
	return *prefPath;
}

const std::string &ConfigPath()
{
	if (!configPath) {
		configPath = FromSDL(SDL_GetPrefPath("diasurgical", "devilution"));
		if (FileExistsAndIsWriteable("diablo.ini")) {
			configPath = std::string("./");
		}
	}
	return *configPath;
}

const std::string &LangPath()
{
	if (!langPath)
		langPath.emplace(MO_LANG_DIR);
	return *langPath;
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

void SetLangPath(const std::string &path)
{
	langPath = path;
	AddTrailingSlash(*langPath);
}

} // namespace paths

} // namespace devilution

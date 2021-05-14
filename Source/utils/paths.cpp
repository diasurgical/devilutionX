#include "utils/paths.h"

#include <SDL.h>

#include "utils/stdcompat/optional.hpp"
#include "utils/log.hpp"

#ifdef USE_SDL1
#include "utils/sdl2_to_1_2_backports.h"
#endif

#ifndef TTF_FONT_DIR
#define TTF_FONT_DIR ""
#endif

#ifndef TTF_FONT_NAME
#define TTF_FONT_NAME "CharisSILB.ttf"
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
std::optional<std::string> ttfPath;
std::optional<std::string> ttfName;

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
	std::string result = (s != nullptr ? s : "");
	if (s != nullptr) {
		SDL_free(s);
	} else {
#ifndef __PSP__
		Log("{}", SDL_GetError());
#endif
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
	if (!prefPath)
		prefPath = FromSDL(SDL_GetPrefPath("diasurgical", "devilution"));
	return *prefPath;
}

const std::string &ConfigPath()
{
	if (!configPath)
		configPath = FromSDL(SDL_GetPrefPath("diasurgical", "devilution"));
	return *configPath;
}

const std::string &LangPath()
{
	if (!langPath)
		langPath.emplace(MO_LANG_DIR);
	return *langPath;
}

const std::string &TtfPath()
{
	if (!ttfPath)
		ttfPath.emplace(TTF_FONT_DIR);
	return *ttfPath;
}

const std::string &TtfName()
{
	if (!ttfName)
		ttfName.emplace(TTF_FONT_NAME);
	return *ttfName;
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

void SetTtfPath(const std::string &path)
{
	ttfPath = path;
	AddTrailingSlash(*ttfPath);
}

void SetTtfName(const std::string &name)
{
	ttfName = name;
}

} // namespace paths

} // namespace devilution

#include "paths.h"

#include <SDL.h>

#ifdef USE_SDL1
#include "sdl2_to_1_2_backports.h"
#endif

namespace dvl {

namespace {

std::string *basePath = NULL;
std::string *prefPath = NULL;

void AddTrailingSlash(std::string *path) {
#ifdef _WIN32
	if (!path->empty() && path->back() != '\\')
		*path += '\\';
#else
	if (!path->empty() && path->back() != '/')
		*path += '/';
#endif
}

std::string *FromSDL(char *s) {
	std::string *result = new std::string(s != NULL ? s : "");
	if (s != NULL) {
		SDL_free(s);
	} else {
		SDL_Log("%s", SDL_GetError());
		SDL_ClearError();
	}
	return result;
}

} // namespace

const std::string &GetBasePath()
{
	if (basePath == NULL) basePath = FromSDL(SDL_GetBasePath());
	return *basePath;
}

const std::string &GetPrefPath()
{
	if (prefPath == NULL) prefPath = FromSDL(SDL_GetPrefPath("diasurgical", "devilution"));
	return *prefPath;
}

void SetBasePath(const char *path)
{
	if (basePath == NULL) basePath = new std::string;
	*basePath = path;
	AddTrailingSlash(basePath);
}

void SetPrefPath(const char *path)
{
	if (prefPath == NULL) prefPath = new std::string;
	*prefPath = path;
	AddTrailingSlash(prefPath);
}

} // namespace dvl

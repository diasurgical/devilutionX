#pragma once

#include "clx_sprite.hpp"

namespace devilution {

OwnedClxSpriteListOrSheet LoadClxListOrSheet(const char *path);

inline OwnedClxSpriteList LoadClx(const char *path)
{
	return LoadClxListOrSheet(path).list();
}

inline OwnedClxSpriteSheet LoadClxSheet(const char *path)
{
	return LoadClxListOrSheet(path).sheet();
}

OptionalOwnedClxSpriteListOrSheet LoadOptionalClxListOrSheet(const char *path);

inline OptionalOwnedClxSpriteList LoadOptionalClx(const char *path)
{
	OptionalOwnedClxSpriteListOrSheet result = LoadOptionalClxListOrSheet(path);
	if (!result)
		return std::nullopt;
	return std::move(*result).list();
}

} // namespace devilution

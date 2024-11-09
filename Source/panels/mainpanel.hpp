#pragma once

#include <string>

#include <expected.hpp>

#include "engine/clx_sprite.hpp"

namespace devilution {

extern OptionalOwnedClxSpriteList PanelButtonDown;
extern OptionalOwnedClxSpriteList TalkButton;

tl::expected<void, std::string> LoadMainPanel();
void FreeMainPanel();

} // namespace devilution

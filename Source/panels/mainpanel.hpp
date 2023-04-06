#pragma once

#include "engine/clx_sprite.hpp"

namespace devilution {

extern OptionalOwnedClxSpriteList PanelButtonDown;
extern OptionalOwnedClxSpriteList TalkButton;

void LoadMainPanel();
void FreeMainPanel();

} // namespace devilution

/**
 * @file plrmsg.h
 *
 * Interface of functionality for printing the ingame chat messages.
 */
#pragma once

#include "SDL.h"
#include <cstdint>
#include <string>

#include "DiabloUI/ui_flags.hpp"
#include "engine.h"
#include "player.h"
#include "utils/stdcompat/string_view.hpp"

namespace devilution {

void plrmsg_delay(bool delay);
void EventPlrMsg(string_view text, UiFlags style = UiFlags::ColorWhitegold);
void SendPlrMsg(Player &player, string_view text);
void InitPlrMsg();
void DrawPlrMsg(const Surface &out);

} // namespace devilution

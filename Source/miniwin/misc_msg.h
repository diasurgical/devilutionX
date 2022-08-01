/**
 * @file miniwin/misc_msg.h
 *
 * Contains most of the the demomode specific logic
 */
#pragma once

#include <SDL.h>
#include <cstdint>

#ifdef USE_SDL1
#include "utils/sdl2_to_1_2_backports.h"
#endif

#include "engine/point.hpp"

namespace devilution {

using EventHandler = void (*)(const SDL_Event &event, uint16_t modState);

void SetCursorPos(Point position);
void FocusOnCharInfo();

void SetMouseButtonEvent(SDL_Event &event, uint32_t type, uint8_t button, Point position);
bool FetchMessage(SDL_Event *event, uint16_t *modState);

void HandleMessage(const SDL_Event &event, uint16_t modState);

} // namespace devilution

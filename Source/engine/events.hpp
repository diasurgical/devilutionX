#pragma once

#include <cstdint>

#include <SDL.h>

#include "engine/point.hpp"

#ifdef USE_SDL1
#include "utils/sdl2_to_1_2_backports.h"
#endif

namespace devilution {

using EventHandler = void (*)(const SDL_Event &event, uint16_t modState);

/** @brief The current input handler function */
extern EventHandler CurrentEventHandler;

EventHandler SetEventHandler(EventHandler NewProc);

bool FetchMessage(SDL_Event *event, uint16_t *modState);

void HandleMessage(const SDL_Event &event, uint16_t modState);

} // namespace devilution

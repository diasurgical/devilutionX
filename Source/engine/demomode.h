/**
 * @file demomode.h
 *
 * Contains most of the the demomode specific logic
 */
#pragma once

#include <SDL.h>

#include "miniwin/misc_msg.h"

namespace devilution {

namespace demo {

void InitPlayBack(int demoNumber, bool timedemo);
void InitRecording(int recordNumber, bool createDemoReference);
void OverrideOptions();

bool IsRunning();
bool IsRecording();

bool GetRunGameLoop(bool &drawGame, bool &processInput);
bool FetchMessage(SDL_Event *event, uint16_t *modState);
void RecordGameLoopResult(bool runGameLoop);
void RecordMessage(const SDL_Event &event, uint16_t modState);

void NotifyGameLoopStart();
void NotifyGameLoopEnd();

} // namespace demo

} // namespace devilution

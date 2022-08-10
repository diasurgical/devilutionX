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

#ifndef DISABLE_DEMOMODE
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
#else
inline void OverrideOptions()
{
}
inline bool IsRunning()
{
	return false;
}
inline bool IsRecording()
{
	return false;
}
inline bool GetRunGameLoop(bool &, bool &)
{
	return false;
}
inline bool FetchMessage(SDL_Event *, uint16_t *)
{
	return false;
}
inline void RecordGameLoopResult(bool)
{
}
inline void RecordMessage(const SDL_Event &, uint16_t)
{
}
inline void NotifyGameLoopStart()
{
}
inline void NotifyGameLoopEnd()
{
}
#endif

} // namespace demo

} // namespace devilution

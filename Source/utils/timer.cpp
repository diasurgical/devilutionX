#include "engine/demomode.h"

namespace devilution {

uint32_t GetTicks()
{
	return (demo::IsRunning() || demo::IsRecording()) ? demo::GetTicks() : SDL_GetTicks();
}

} // namespace devilution

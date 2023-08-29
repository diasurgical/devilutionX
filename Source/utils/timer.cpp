#include "engine/demomode.h"

namespace devilution {

uint32_t GetMillisecondsSinceStartup()
{
	return (demo::IsRunning() || demo::IsRecording()) ? demo::SimulateMillisecondsSinceStartup() : SDL_GetTicks();
}

} // namespace devilution

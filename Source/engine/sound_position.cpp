#include "engine/sound_position.hpp"

#include "engine/sound_defs.hpp"
#include "player.h"

namespace devilution {

bool CalculateSoundPosition(Point soundPosition, int *plVolume, int *plPan)
{
	const Point playerPosition { MyPlayer->position.tile };
	const Displacement delta = soundPosition - playerPosition;

	const int pan = (delta.deltaX - delta.deltaY) * 256;
	*plPan = std::clamp(pan, PAN_MIN, PAN_MAX);

	const int volume = playerPosition.ApproxDistance(soundPosition) * -64;

	if (volume <= ATTENUATION_MIN)
		return false;

	*plVolume = volume;

	return true;
}

} // namespace devilution

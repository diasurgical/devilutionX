#include "engine/sound_position.hpp"

#include "engine/sound_defs.hpp"
#include "player.h"

namespace devilution {

bool CalculateSoundPosition(Point soundPosition, int *plVolume, int *plPan)
{
	const auto &playerPosition = MyPlayer->position.tile;
	const auto delta = soundPosition - playerPosition;

	int pan = (delta.deltaX - delta.deltaY) * 256;
	*plPan = clamp(pan, PAN_MIN, PAN_MAX);

	int volume = playerPosition.ApproxDistance(soundPosition);
	volume *= -64;

	if (volume <= ATTENUATION_MIN)
		return false;

	*plVolume = volume;

	return true;
}

} // namespace devilution

// Stubbed implementations of effects for the NOSOUND mode.
#include "effects.h"

#include "engine/random.hpp"

namespace devilution {
int sfxdelay;
_sfx_id sfxdnum;

// Disable clang-format here because our config says:
// AllowShortFunctionsOnASingleLine: None
// clang-format off
bool effect_is_playing(int nSFX) { return false; }
void stream_stop() { }
void InitMonsterSND(int monst) { }
void FreeMonsterSnd() { }
bool CalculateSoundPosition(Point soundPosition, int *plVolume, int *plPan) { return false; }
void PlaySFX(_sfx_id psfx)
{
	switch (psfx) {
	case PS_WARR69:
	case PS_MAGE69:
	case PS_ROGUE69:
	case PS_MONK69:
	case PS_SWING:
	case LS_ACID:
	case IS_FMAG:
	case IS_MAGIC:
	case IS_BHIT:
	case PS_WARR14:
	case PS_WARR15:
	case PS_WARR16:
	case PS_WARR2:
	case PS_ROGUE14:
	case PS_MAGE14:
	case PS_MONK14:
		AdvanceRndSeed();
		break;
	default:
		break;
	}
}
void PlaySfxLoc(_sfx_id psfx, Point position, bool randomizeByCategory)
{
	if (!randomizeByCategory)
		return;

	PlaySFX(psfx);
}
void sound_stop() { }
void sound_update() { }
void effects_cleanup_sfx() { }
void sound_init() { }
void ui_sound_init() { }
void effects_play_sound(const char *snd_file) { }
// clang-format off

} // namespace devilution

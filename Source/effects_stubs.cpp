// Stubbed implementations of effects for the NOSOUND mode.
#include "effects.h"

#include "engine/random.hpp"

namespace devilution {
int sfxdelay;
SfxID sfxdnum;

bool effect_is_playing(SfxID nSFX) { return false; }
void stream_stop() { }
void PlaySFX(SfxID psfx)
{
	switch (psfx) {
	case SfxID::Warrior69:
	case SfxID::Sorceror69:
	case SfxID::Rogue69:
	case SfxID::Monk69:
	case SfxID::Swing:
	case SfxID::SpellAcid:
	case SfxID::OperateShrine:
	case SfxID::Warrior14:
	case SfxID::Warrior15:
	case SfxID::Warrior16:
	case SfxID::Warrior2:
	case SfxID::Rogue14:
	case SfxID::Sorceror14:
	case SfxID::Monk14:
		AdvanceRndSeed();
		break;
	default:
		break;
	}
}
void PlaySfxLoc(SfxID psfx, Point position, bool randomizeByCategory)
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
void effects_play_sound(SfxID id) { }
int GetSFXLength(SfxID nSFX) { return 0; }

} // namespace devilution

// Stubbed implementations of effects for the NOSOUND mode.
#include "effects.h"

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
void PlaySFX(_sfx_id psfx) { }
void PlaySfxLoc(_sfx_id psfx, Point position, bool randomizeByCategory) { }
void sound_stop() { }
void sound_update() { }
void effects_cleanup_sfx() { }
void sound_init() { }
void ui_sound_init() { }
void effects_play_sound(const char *snd_file) { }
// clang-format off

} // namespace devilution

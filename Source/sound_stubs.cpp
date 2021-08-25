// Stubbed implementations of sound functions for the NOSOUND mode.
#include "sound.h"

namespace devilution {

bool gbSndInited;
bool gbMusicOn;
bool gbSoundOn;

// Disable clang-format here because our config says:
// AllowShortFunctionsOnASingleLine: None
// clang-format off
void ClearDuplicateSounds() { }
void snd_play_snd(TSnd *pSnd, int lVolume, int lPan) { }
std::unique_ptr<TSnd> sound_file_load(const char *path, bool stream) { return nullptr; }
void snd_init() { }
void snd_deinit() { }
void music_stop() { }
void music_start(uint8_t nTrack) { }
void sound_disable_music(bool disable) { }
int sound_get_or_set_music_volume(int volume) { return 0; }
int sound_get_or_set_sound_volume(int volume) { return 0; }
// clang-format on

} // namespace devilution

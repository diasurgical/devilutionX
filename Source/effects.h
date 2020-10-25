/**
 * @file effects.h
 *
 * Interface of functions for loading and playing sounds.
 */
#ifndef __EFFECTS_H__
#define __EFFECTS_H__

DEVILUTION_BEGIN_NAMESPACE

#ifdef __cplusplus
extern "C" {
#endif

extern int sfxdelay;
extern int sfxdnum;
extern HANDLE sghStream;
extern TSFX *sgpStreamSFX;

BOOL effect_is_playing(int nSFX);
void stream_stop();
void InitMonsterSND(int monst);
void FreeMonsterSnd();
void PlayEffect(int i, int mode);
BOOL calc_snd_position(int x, int y, int *plVolume, int *plPan);
void PlaySFX(int psfx);
void PlaySFX_priv(TSFX *pSFX, BOOL loc, int x, int y);
void stream_play(TSFX *pSFX, int lVolume, int lPan);
int RndSFX(int psfx);
void PlaySfxLoc(int psfx, int x, int y);
void sound_stop();
void sfx_stop();
void sound_update();
void stream_update();
void effects_cleanup_sfx();
void sound_init();
void priv_sound_init(BYTE bLoadMask);
void ui_sound_init();
void effects_play_sound(const char *snd_file);

/* rdata */

extern const char MonstSndChar[];

/* data */
#ifdef __cplusplus
}
#endif

DEVILUTION_END_NAMESPACE

#endif /* __EFFECTS_H__ */

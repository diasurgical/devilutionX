//HEADER_GOES_HERE
#ifndef __EFFECTS_H__
#define __EFFECTS_H__

extern int sfxdelay;
extern int sfxdnum;
extern HANDLE sfx_stream;
extern TSFX *sfx_data_cur;

BOOL effect_is_playing(int nSFX);
void sfx_stop();
void InitMonsterSND(int monst);
void FreeEffects();
void PlayEffect(int i, int mode);
BOOL calc_snd_position(int x, int y, int *plVolume, int *plPan);
void PlaySFX(int psfx);
void PlaySFX_priv(TSFX *pSFX, BOOL loc, int x, int y);
void stream_play(TSFX *pSFX, int lVolume, int lPan);
int RndSFX(int psfx);
void PlaySfxLoc(int psfx, int x, int y);
void FreeMonsterSnd();
void sound_stop();
void sound_update();
void effects_update();
void effects_cleanup_sfx();
void stream_update();
void priv_sound_init(BYTE bLoadMask);
void sound_init();
void effects_play_sound(char *snd_file);

/* rdata */

extern const char MonstSndChar[];

/* data */

#endif /* __EFFECTS_H__ */

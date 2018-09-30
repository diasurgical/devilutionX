//HEADER_GOES_HERE
#ifndef __EFFECTS_H__
#define __EFFECTS_H__

extern int effects_cpp_init_value; // weak
extern int sfxdelay; // weak
extern int sfxdnum;
extern void *sfx_stream;
extern TSFX *sfx_data_cur;

void __cdecl effects_cpp_init();
bool __fastcall effect_is_playing(int nSFX);
void __cdecl sfx_stop();
void __fastcall InitMonsterSND(int monst);
void __cdecl FreeEffects();
void __fastcall PlayEffect(int i, int mode);
int __fastcall calc_snd_position(int x, int y, int *plVolume, int *plPan);
void __fastcall PlaySFX(int psfx);
void __fastcall PlaySFX_priv(TSFX *pSFX, char loc, int x, int y);
void __fastcall stream_play(TSFX *pSFX, int lVolume, int lPan);
int __fastcall RndSFX(int psfx);
void __fastcall PlaySfxLoc(int psfx, int x, int y);
void __cdecl FreeMonsterSnd();
void __cdecl sound_stop();
void __cdecl sound_update();
void __cdecl effects_cleanup_sfx();
void __cdecl stream_update();
void __fastcall priv_sound_init(int bLoadMask);
void __cdecl sound_init();
void __stdcall effects_play_sound(char *snd_file);

/* rdata */

extern const int effects_inf; // weak
extern const char monster_action_sounds[]; // idb

/* data */

extern TSFX sgSFX[NUM_SFX];

#endif /* __EFFECTS_H__ */

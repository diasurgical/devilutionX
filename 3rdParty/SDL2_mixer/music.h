/*
  SDL_mixer:  An audio mixer library based on the SDL library
  Copyright (C) 1997-2018 Sam Lantinga <slouken@libsdl.org>

  This software is provided 'as-is', without any express or implied
  warranty.  In no event will the authors be held liable for any damages
  arising from the use of this software.

  Permission is granted to anyone to use this software for any purpose,
  including commercial applications, and to alter it and redistribute it
  freely, subject to the following restrictions:

  1. The origin of this software must not be misrepresented; you must not
     claim that you wrote the original software. If you use this software
     in a product, an acknowledgment in the product documentation would be
     appreciated but is not required.
  2. Altered source versions must be plainly marked as such, and must not be
     misrepresented as being the original software.
  3. This notice may not be removed or altered from any source distribution.
*/
#include "SDL_mixer.h"

#ifndef MUSIC_H_
#define MUSIC_H_

/* Supported music APIs, in order of preference */

typedef enum
{
    MIX_MUSIC_CMD,
    MIX_MUSIC_WAVE,
    MIX_MUSIC_MODPLUG,
    MIX_MUSIC_MIKMOD,
    MIX_MUSIC_FLUIDSYNTH,
    MIX_MUSIC_TIMIDITY,
    MIX_MUSIC_NATIVEMIDI,
    MIX_MUSIC_OGG,
    MIX_MUSIC_MPG123,
    MIX_MUSIC_MAD,
    MIX_MUSIC_FLAC,
    MIX_MUSIC_OPUS,
    MIX_MUSIC_LAST
} Mix_MusicAPI;


/* Music API implementation */

typedef struct
{
    const char *tag;
    Mix_MusicAPI api;
    Mix_MusicType type;
    SDL_bool loaded;
    SDL_bool opened;

    /* Load the library */
    int (*Load)(void);

    /* Initialize for the audio output */
    int (*Open)(const SDL_AudioSpec *spec);

    /* Create a music object from an SDL_RWops stream
     * If the function returns NULL, 'src' will be freed if needed by the caller.
     */
    void *(*CreateFromRW)(SDL_RWops *src, int freesrc);

    /* Create a music object from a file, if SDL_RWops are not supported */
    void *(*CreateFromFile)(const char *file);

    /* Set the volume */
    void (*SetVolume)(void *music, int volume);

    /* Start playing music from the beginning with an optional loop count */
    int (*Play)(void *music, int play_count);

    /* Returns SDL_TRUE if music is still playing */
    SDL_bool (*IsPlaying)(void *music);

    /* Get music data, returns the number of bytes left */
    int (*GetAudio)(void *music, void *data, int bytes);

    /* Seek to a play position (in seconds) */
    int (*Seek)(void *music, double position);

    /* Pause playing music */
    void (*Pause)(void *music);

    /* Resume playing music */
    void (*Resume)(void *music);

    /* Stop playing music */
    void (*Stop)(void *music);

    /* Delete a music object */
    void (*Delete)(void *music);

    /* Close the library and clean up */
    void (*Close)(void);

    /* Unload the library */
    void (*Unload)(void);

} Mix_MusicInterface;


extern int get_num_music_interfaces(void);
extern Mix_MusicInterface *get_music_interface(int index);
extern Mix_MusicType detect_music_type_from_magic(const Uint8 *magic);
extern SDL_bool load_music_type(Mix_MusicType type);
extern SDL_bool open_music_type(Mix_MusicType type);
extern SDL_bool has_music(Mix_MusicType type);
extern void open_music(const SDL_AudioSpec *spec);
extern int music_pcm_getaudio(void *context, void *data, int bytes, int volume,
                              int (*GetSome)(void *context, void *data, int bytes, SDL_bool *done));
extern void SDLCALL music_mixer(void *udata, Uint8 *stream, int len);
extern void close_music(void);
extern void unload_music(void);

extern char *music_cmd;
extern SDL_AudioSpec music_spec;

#endif /* MUSIC_H_ */

/* vi: set ts=4 sw=4 expandtab: */

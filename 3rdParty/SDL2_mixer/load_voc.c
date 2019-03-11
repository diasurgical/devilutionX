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

  This is the source needed to decode a Creative Labs VOC file into a
  waveform. It's pretty straightforward once you get going. The only
  externally-callable function is Mix_LoadVOC_RW(), which is meant to
  act as identically to SDL_LoadWAV_RW() as possible.

  This file by Ryan C. Gordon (icculus@icculus.org).

  Heavily borrowed from sox v12.17.1's voc.c.
        (http://www.freshmeat.net/projects/sox/)
*/

#include "SDL_mixer.h"
#include "load_voc.h"

/* Private data for VOC file */
typedef struct vocstuff {
    Uint32  rest;           /* bytes remaining in current block */
    Uint32  rate;           /* rate code (byte) of this chunk */
    int     silent;         /* sound or silence? */
    Uint32  srate;          /* rate code (byte) of silence */
    Uint32  blockseek;      /* start of current output block */
    Uint32  samples;        /* number of samples output */
    Uint32  size;           /* word length of data */
    Uint8   channels;       /* number of sound channels */
    int     has_extended;   /* Has an extended block been read? */
} vs_t;

/* Size field */
/* SJB: note that the 1st 3 are sometimes used as sizeof(type) */
#define ST_SIZE_BYTE    1
#define ST_SIZE_8BIT    1
#define ST_SIZE_WORD    2
#define ST_SIZE_16BIT   2
#define ST_SIZE_DWORD   4
#define ST_SIZE_32BIT   4
#define ST_SIZE_FLOAT   5
#define ST_SIZE_DOUBLE  6
#define ST_SIZE_IEEE    7   /* IEEE 80-bit floats. */

/* Style field */
#define ST_ENCODING_UNSIGNED    1 /* unsigned linear: Sound Blaster */
#define ST_ENCODING_SIGN2       2 /* signed linear 2's comp: Mac */
#define ST_ENCODING_ULAW        3 /* U-law signed logs: US telephony, SPARC */
#define ST_ENCODING_ALAW        4 /* A-law signed logs: non-US telephony */
#define ST_ENCODING_ADPCM       5 /* Compressed PCM */
#define ST_ENCODING_IMA_ADPCM   6 /* Compressed PCM */
#define ST_ENCODING_GSM         7 /* GSM 6.10 33-byte frame lossy compression */

#define VOC_TERM        0
#define VOC_DATA        1
#define VOC_CONT        2
#define VOC_SILENCE     3
#define VOC_MARKER      4
#define VOC_TEXT        5
#define VOC_LOOP        6
#define VOC_LOOPEND     7
#define VOC_EXTENDED    8
#define VOC_DATA_16     9


static int voc_check_header(SDL_RWops *src)
{
    /* VOC magic header */
    Uint8  signature[20];  /* "Creative Voice File\032" */
    Uint16 datablockofs;

    SDL_RWseek(src, 0, RW_SEEK_SET);

    if (SDL_RWread(src, signature, sizeof (signature), 1) != 1)
        return(0);

    if (SDL_memcmp(signature, "Creative Voice File\032", sizeof (signature)) != 0) {
        SDL_SetError("Unrecognized file type (not VOC)");
        return(0);
    }

        /* get the offset where the first datablock is located */
    if (SDL_RWread(src, &datablockofs, sizeof (Uint16), 1) != 1)
        return(0);

    datablockofs = SDL_SwapLE16(datablockofs);

    if (SDL_RWseek(src, datablockofs, RW_SEEK_SET) != datablockofs)
        return(0);

    return(1);  /* success! */
} /* voc_check_header */


/* Read next block header, save info, leave position at start of data */
static int voc_get_block(SDL_RWops *src, vs_t *v, SDL_AudioSpec *spec)
{
    Uint8 bits24[3];
    Uint8 uc, block;
    Uint32 sblen;
    Uint16 new_rate_short;
    Uint32 new_rate_long;
    Uint8 trash[6];
    Uint16 period;
    unsigned int i;

    v->silent = 0;
    while (v->rest == 0)
    {
        if (SDL_RWread(src, &block, sizeof (block), 1) != 1)
            return 1;  /* assume that's the end of the file. */

        if (block == VOC_TERM)
            return 1;

        if (SDL_RWread(src, bits24, sizeof (bits24), 1) != 1)
            return 1;  /* assume that's the end of the file. */

        /* Size is an 24-bit value. Ugh. */
        sblen = ((bits24[0]) | (bits24[1] << 8) | (bits24[2] << 16));

        switch(block)
        {
            case VOC_DATA:
                if (SDL_RWread(src, &uc, sizeof (uc), 1) != 1)
                    return 0;

                /* When DATA block preceeded by an EXTENDED     */
                /* block, the DATA blocks rate value is invalid */
                if (!v->has_extended)
                {
                    if (uc == 0)
                    {
                        SDL_SetError("VOC Sample rate is zero?");
                        return 0;
                    }

                    if ((v->rate != -1) && (uc != v->rate))
                    {
                        SDL_SetError("VOC sample rate codes differ");
                        return 0;
                    }

                    v->rate = uc;
                    spec->freq = (Uint16)(1000000.0/(256 - v->rate));
                    v->channels = 1;
                }

                if (SDL_RWread(src, &uc, sizeof (uc), 1) != 1)
                    return 0;

                if (uc != 0)
                {
                    SDL_SetError("VOC decoder only interprets 8-bit data");
                    return 0;
                }

                v->has_extended = 0;
                v->rest = sblen - 2;
                v->size = ST_SIZE_BYTE;
                return 1;

            case VOC_DATA_16:
                if (SDL_RWread(src, &new_rate_long, sizeof (new_rate_long), 1) != 1)
                    return 0;
                new_rate_long = SDL_SwapLE32(new_rate_long);
                if (new_rate_long == 0)
                {
                    SDL_SetError("VOC Sample rate is zero?");
                    return 0;
                }
                if ((v->rate != -1) && (new_rate_long != v->rate))
                {
                    SDL_SetError("VOC sample rate codes differ");
                    return 0;
                }
                v->rate = new_rate_long;
                spec->freq = new_rate_long;

                if (SDL_RWread(src, &uc, sizeof (uc), 1) != 1)
                    return 0;

                switch (uc)
                {
                    case 8:  v->size = ST_SIZE_BYTE; break;
                    case 16: v->size = ST_SIZE_WORD; break;
                    default:
                        SDL_SetError("VOC with unknown data size");
                        return 0;
                }

                if (SDL_RWread(src, &v->channels, sizeof (Uint8), 1) != 1)
                    return 0;

                if (SDL_RWread(src, trash, sizeof (Uint8), 6) != 6)
                    return 0;

                v->rest = sblen - 12;
                return 1;

            case VOC_CONT:
                v->rest = sblen;
                return 1;

            case VOC_SILENCE:
                if (SDL_RWread(src, &period, sizeof (period), 1) != 1)
                    return 0;
                period = SDL_SwapLE16(period);

                if (SDL_RWread(src, &uc, sizeof (uc), 1) != 1)
                    return 0;
                if (uc == 0)
                {
                    SDL_SetError("VOC silence sample rate is zero");
                    return 0;
                }

                /*
                 * Some silence-packed files have gratuitously
                 * different sample rate codes in silence.
                 * Adjust period.
                 */
                if ((v->rate != -1) && (uc != v->rate))
                    period = (Uint16)((period * (256 - uc))/(256 - v->rate));
                else
                    v->rate = uc;
                v->rest = period;
                v->silent = 1;
                return 1;

            case VOC_LOOP:
            case VOC_LOOPEND:
                for(i = 0; i < sblen; i++)   /* skip repeat loops. */
                {
                    if (SDL_RWread(src, trash, sizeof (Uint8), 1) != 1)
                        return 0;
                }
                break;

            case VOC_EXTENDED:
                /* An Extended block is followed by a data block */
                /* Set this byte so we know to use the rate      */
                /* value from the extended block and not the     */
                /* data block.                     */
                v->has_extended = 1;
                if (SDL_RWread(src, &new_rate_short, sizeof (new_rate_short), 1) != 1)
                    return 0;
                new_rate_short = SDL_SwapLE16(new_rate_short);
                if (new_rate_short == 0)
                {
                   SDL_SetError("VOC sample rate is zero");
                   return 0;
                }
                if ((v->rate != -1) && (new_rate_short != v->rate))
                {
                   SDL_SetError("VOC sample rate codes differ");
                   return 0;
                }
                v->rate = new_rate_short;

                if (SDL_RWread(src, &uc, sizeof (uc), 1) != 1)
                    return 0;

                if (uc != 0)
                {
                    SDL_SetError("VOC decoder only interprets 8-bit data");
                    return 0;
                }

                if (SDL_RWread(src, &uc, sizeof (uc), 1) != 1)
                    return 0;

                if (uc)
                    spec->channels = 2;  /* Stereo */
                /* Needed number of channels before finishing
                   compute for rate */
                spec->freq = (256000000L/(65536L - v->rate))/spec->channels;
                /* An extended block must be followed by a data */
                /* block to be valid so loop back to top so it  */
                /* can be grabed.                */
                continue;

            case VOC_MARKER:
                if (SDL_RWread(src, trash, sizeof (Uint8), 2) != 2)
                    return 0;

                /* Falling! Falling! */

            default:  /* text block or other krapola. */
                for(i = 0; i < sblen; i++)
                {
                    if (SDL_RWread(src, &trash, sizeof (Uint8), 1) != 1)
                        return 0;
                }

                if (block == VOC_TEXT)
                    continue;    /* get next block */
        }
    }

    return 1;
}


static int voc_read(SDL_RWops *src, vs_t *v, Uint8 *buf, SDL_AudioSpec *spec)
{
    Uint32 done = 0;
    Uint8 silence = 0x80;

    if (v->rest == 0)
    {
        if (!voc_get_block(src, v, spec))
            return 0;
    }

    if (v->rest == 0)
        return 0;

    if (v->silent)
    {
        if (v->size == ST_SIZE_WORD)
            silence = 0x00;

        /* Fill in silence */
        SDL_memset(buf, silence, v->rest);
        done = v->rest;
        v->rest = 0;
    }

    else
    {
        done = (Uint32)SDL_RWread(src, buf, 1, v->rest);
        v->rest -= done;
        if (v->size == ST_SIZE_WORD)
        {
            #if (SDL_BYTEORDER == SDL_BIG_ENDIAN)
                Uint16 *samples = (Uint16 *)buf;
                for (; v->rest > 0; v->rest -= 2)
                {
                    *samples = SDL_SwapLE16(*samples);
                    samples++;
                }
            #endif
            done >>= 1;
        }
    }

    return done;
} /* voc_read */


/* don't call this directly; use Mix_LoadWAV_RW() for now. */
SDL_AudioSpec *Mix_LoadVOC_RW (SDL_RWops *src, int freesrc,
        SDL_AudioSpec *spec, Uint8 **audio_buf, Uint32 *audio_len)
{
    vs_t v;
    int was_error = 1;
    int samplesize;
    Uint8 *fillptr;
    void *ptr;

    if ((!src) || (!audio_buf) || (!audio_len))   /* sanity checks. */
        goto done;

    if (!voc_check_header(src))
        goto done;

    v.rate = -1;
    v.rest = 0;
    v.has_extended = 0;
    *audio_buf = NULL;
    *audio_len = 0;
    SDL_memset(spec, '\0', sizeof (SDL_AudioSpec));

    if (!voc_get_block(src, &v, spec))
        goto done;

    if (v.rate == -1)
    {
        SDL_SetError("VOC data had no sound!");
        goto done;
    }

    spec->format = ((v.size == ST_SIZE_WORD) ? AUDIO_S16 : AUDIO_U8);
    if (spec->channels == 0)
        spec->channels = v.channels;

    *audio_len = v.rest;
    *audio_buf = SDL_malloc(v.rest);
    if (*audio_buf == NULL)
        goto done;

    fillptr = *audio_buf;

    while (voc_read(src, &v, fillptr, spec) > 0)
    {
        if (!voc_get_block(src, &v, spec))
            goto done;

        *audio_len += v.rest;
        ptr = SDL_realloc(*audio_buf, *audio_len);
        if (ptr == NULL)
        {
            SDL_free(*audio_buf);
            *audio_buf = NULL;
            *audio_len = 0;
            goto done;
        }

        *audio_buf = ptr;
        fillptr = ((Uint8 *) ptr) + (*audio_len - v.rest);
    }

    spec->samples = (Uint16)(*audio_len / v.size);

    was_error = 0;  /* success, baby! */

    /* Don't return a buffer that isn't a multiple of samplesize */
    samplesize = ((spec->format & 0xFF)/8)*spec->channels;
    *audio_len &= ~(samplesize-1);

done:
    if (freesrc && src) {
        SDL_RWclose(src);
    }

    if (was_error) {
        spec = NULL;
    }

    return(spec);
} /* Mix_LoadVOC_RW */

/* end of load_voc.c ... */

/* vi: set ts=4 sw=4 expandtab: */

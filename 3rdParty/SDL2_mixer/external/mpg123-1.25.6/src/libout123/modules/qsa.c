/*
    qsa: sound output with QNX Sound Architecture 0.5.2 API

    copyright 2013 by the mpg123 project - free software under the terms of the LGPL 2.1
    see COPYING and AUTHORS files in distribution or http://mpg123.org

    written by Mike Gorchak <mike.gorchak.qnx@gmail.com>
*/

#include "out123_int.h"
#include <errno.h>

#include <stdint.h>
#include <sys/asoundlib.h>

#include "debug.h"

typedef struct _qsa_mp_map
{
    uint32_t qsa_format;
    int      mp_format;
} qsa_mp_map_t;

/* in order best format first */
qsa_mp_map_t format_map[]=
{
#ifdef WORDS_BIGENDIAN
    {SND_PCM_SFMT_FLOAT64_BE, MPG123_ENC_FLOAT_64     },
    {SND_PCM_SFMT_FLOAT_BE,   MPG123_ENC_FLOAT_32     },
    {SND_PCM_SFMT_S32_BE,     MPG123_ENC_SIGNED_32    },
    {SND_PCM_SFMT_U32_BE,     MPG123_ENC_UNSIGNED_32  },
    {SND_PCM_SFMT_S24_BE,     MPG123_ENC_SIGNED_24    },
    {SND_PCM_SFMT_U24_BE,     MPG123_ENC_UNSIGNED_24  },
    {SND_PCM_SFMT_S16_BE,     MPG123_ENC_SIGNED_16    },
    {SND_PCM_SFMT_U16_BE,     MPG123_ENC_UNSIGNED_16  },
#else
    {SND_PCM_SFMT_FLOAT64_LE, MPG123_ENC_FLOAT_64     },
    {SND_PCM_SFMT_FLOAT_LE,   MPG123_ENC_FLOAT_32     },
    {SND_PCM_SFMT_S32_LE,     MPG123_ENC_SIGNED_32    },
    {SND_PCM_SFMT_U32_LE,     MPG123_ENC_UNSIGNED_32  },
    {SND_PCM_SFMT_S24_LE,     MPG123_ENC_SIGNED_24    },
    {SND_PCM_SFMT_U24_LE,     MPG123_ENC_UNSIGNED_24  },
    {SND_PCM_SFMT_S16_LE,     MPG123_ENC_SIGNED_16    },
    {SND_PCM_SFMT_U16_LE,     MPG123_ENC_UNSIGNED_16  },
#endif
    {SND_PCM_SFMT_U8,         MPG123_ENC_UNSIGNED_8   },
    {SND_PCM_SFMT_S8,         MPG123_ENC_SIGNED_8     },
    {SND_PCM_SFMT_A_LAW,      MPG123_ENC_ALAW_8       },
    {SND_PCM_SFMT_MU_LAW,     MPG123_ENC_ULAW_8       },
    {0,                       0                       },
};

typedef struct _qsa_internal
{
    int cardno;
    int deviceno;
    snd_pcm_t* audio_handle;
    snd_pcm_channel_params_t cpars;
} qsa_internal_t;

static int open_qsa(out123_handle* ao)
{
    int status;
    int cardno;
    int deviceno;
    int it;
    snd_pcm_t* audio_handle;
    qsa_internal_t* userptr;

    ao->userptr=NULL;

    status=snd_pcm_open_preferred(&audio_handle, &cardno, &deviceno, SND_PCM_OPEN_PLAYBACK);
    if (status<0)
    {
        return FALSE;
    }

    status=snd_pcm_plugin_set_disable(audio_handle, PLUGIN_DISABLE_MMAP);
    if (status<0)
    {
        return FALSE;
    }

    userptr=calloc(1, sizeof(qsa_internal_t));
    if (userptr==NULL)
    {
        return FALSE;
    }
    ao->userptr=userptr;
    userptr->audio_handle=audio_handle;
    userptr->cardno=cardno;
    userptr->deviceno=deviceno;

    memset(&userptr->cpars, 0, sizeof(userptr->cpars));

    userptr->cpars.channel=SND_PCM_CHANNEL_PLAYBACK;
    userptr->cpars.mode=SND_PCM_MODE_BLOCK;
    userptr->cpars.start_mode=SND_PCM_START_DATA;
    userptr->cpars.stop_mode=SND_PCM_STOP_STOP;
    userptr->cpars.format.format=0;
    it=0;
    do {
        if ((format_map[it].qsa_format==0) && (format_map[it].mp_format==0))
        {
            break;
        }
        if (ao->format==format_map[it].mp_format)
        {
            userptr->cpars.format.format=format_map[it].qsa_format;
            break;
        }
        it++;
    } while(1);
    userptr->cpars.format.interleave=1;
    userptr->cpars.format.rate=ao->rate;
    userptr->cpars.format.voices=ao->channels;
    userptr->cpars.buf.block.frag_size=4096;
    userptr->cpars.buf.block.frags_min=8;
    userptr->cpars.buf.block.frags_max=16;

    if ((ao->channels!=-1) && (ao->rate!=-1))
    {
        status=snd_pcm_plugin_params(userptr->audio_handle, &userptr->cpars);
        if (status<0)
        {
            return FALSE;
        }

        status=snd_pcm_plugin_prepare(userptr->audio_handle, SND_PCM_CHANNEL_PLAYBACK);
        if (status<0)
        {
            return FALSE;
        }
    }

    return TRUE;
}

static int get_formats_qsa(out123_handle* ao)
{
    qsa_internal_t* userptr;
    int status;
    int it=0;

    userptr=ao->userptr;
    if (userptr!=NULL);
    {
        userptr->cpars.format.rate=ao->rate;
        userptr->cpars.format.voices=ao->channels;
        it=0;
        do {
            if ((format_map[it].qsa_format==0) && (format_map[it].mp_format==0))
            {
                break;
            }
            userptr->cpars.format.format=format_map[it].qsa_format;
            status=snd_pcm_plugin_params(userptr->audio_handle, &userptr->cpars);
            if (status<0)
            {
                it++;
            }
            else
            {
                return format_map[it].mp_format;
            }
        } while(1);
    }

    return 0;
}

static int write_qsa(out123_handle* ao, unsigned char* buf, int bytes)
{
    int written;
    int status;
    snd_pcm_channel_status_t cstatus;
    qsa_internal_t* userptr;

    userptr=ao->userptr;
    if (userptr!=NULL);
    {
        written=snd_pcm_plugin_write(userptr->audio_handle, buf, bytes);
        if (written!=bytes)
        {
            /* Check if samples playback got stuck somewhere in hardware or in */
            /* the audio device driver */
            if ((errno==EAGAIN) && (written==0))
            {
                return 0;
            }
            if ((errno==EINVAL) || (errno==EIO))
            {
                memset(&cstatus, 0, sizeof(cstatus));
                cstatus.channel=SND_PCM_CHANNEL_PLAYBACK;
                status=snd_pcm_plugin_status(userptr->audio_handle, &cstatus);
                if (status>0)
                {
                    return 0;
                }
                if ((cstatus.status == SND_PCM_STATUS_UNDERRUN) ||
                    (cstatus.status == SND_PCM_STATUS_READY))
                {
                    status=snd_pcm_plugin_prepare(userptr->audio_handle, SND_PCM_CHANNEL_PLAYBACK);
                    if (status<0)
                    {
                        return 0;
                    }
                }
            }
        }
    }

    return written;
}

static void flush_qsa(out123_handle* ao)
{
    qsa_internal_t* userptr;

    userptr=ao->userptr;
    if (userptr!=NULL);
    {
        snd_pcm_playback_flush(userptr->audio_handle);
    }
}

static int close_qsa(out123_handle* ao)
{
    qsa_internal_t* userptr;

    userptr=ao->userptr;
    if (userptr!=NULL);
    {
        snd_pcm_close(userptr->audio_handle);
        free(ao->userptr);
    }

    return TRUE;
}

static int deinit_qsa(out123_handle* ao)
{
    return TRUE;
}

static int init_qsa(out123_handle* ao)
{
    if (ao==NULL) return -1;

    /* Set callbacks */
    ao->open = open_qsa;
    ao->flush = flush_qsa;
    ao->write = write_qsa;
    ao->get_formats = get_formats_qsa;
    ao->close = close_qsa;
    ao->deinit = deinit_qsa;

    /* Success */
    return 0;
}

/*
    Module information data structure
*/
mpg123_module_t mpg123_output_module_info = {
    /* api_version */    MPG123_MODULE_API_VERSION,
    /* name */           "qsa",
    /* description */    "Output audio using QNX Sound Architecture (QSA).",
    /* revision */       "$Rev:$",
    /* handle */         NULL,
    /* init_output */    init_qsa,
};

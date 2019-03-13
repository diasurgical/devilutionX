/*
	openal.c: audio output on OpenAL

	copyright 1995-2016 by the mpg123 project - free software under the terms of the LGPL 2.1
	see COPYING and AUTHORS files in distribution or http://mpg123.org
	initially written by Taihei Monma
*/

/* Need usleep(). */
#define _DEFAULT_SOURCE
#define _BSD_SOURCE

#include "out123_int.h"

#ifdef OPENAL_SUBDIR_OPENAL
	#include <OpenAL/al.h>
	#include <OpenAL/alc.h>
#elif defined(OPENAL_SUBDIR_AL)
	#include <AL/al.h>
	#include <AL/alc.h>
#else
	#include <al.h>
	#include <alc.h>
#endif
#include <errno.h>
#include <unistd.h>

#include "debug.h"

#define NUM_BUFFERS 16

#ifndef AL_FORMAT_MONO_FLOAT32
#define AL_FORMAT_MONO_FLOAT32 0x10010
#endif
#ifndef AL_FORMAT_STEREO_FLOAT32
#define AL_FORMAT_STEREO_FLOAT32 0x10011
#endif

typedef struct
{
	ALCdevice *device;
	ALCcontext *context;
	ALuint source, buffer;
	ALenum format;
	ALsizei rate;
} mpg123_openal_t;


static int open_openal(out123_handle *ao)
{
	mpg123_openal_t* al = (mpg123_openal_t*)ao->userptr;

	al->device = alcOpenDevice(NULL);
	al->context = alcCreateContext(al->device, NULL);
	alcMakeContextCurrent(al->context);
	alGenSources(1, &al->source);

	al->rate = ao->rate;
	if(ao->format == MPG123_ENC_SIGNED_16 && ao->channels == 2) al->format = AL_FORMAT_STEREO16;
	else if(ao->format == MPG123_ENC_SIGNED_16 && ao->channels == 1) al->format = AL_FORMAT_MONO16;
	else if(ao->format == MPG123_ENC_UNSIGNED_8 && ao->channels == 2) al->format = AL_FORMAT_STEREO8;
	else if(ao->format == MPG123_ENC_UNSIGNED_8 && ao->channels == 1) al->format = AL_FORMAT_MONO8;
	else if(ao->format == MPG123_ENC_FLOAT_32 && ao->channels == 2) al->format = AL_FORMAT_STEREO_FLOAT32;
	else if(ao->format == MPG123_ENC_FLOAT_32 && ao->channels == 1) al->format = AL_FORMAT_MONO_FLOAT32;
	
	return 0;
}

static int get_formats_openal(out123_handle *ao)
{
	return MPG123_ENC_SIGNED_16|MPG123_ENC_UNSIGNED_8|((alIsExtensionPresent((ALubyte*)"AL_EXT_float32") == AL_TRUE) ? MPG123_ENC_FLOAT_32 : 0);
}

static int write_openal(out123_handle *ao, unsigned char *buf, int len)
{
	ALint state, n;
	mpg123_openal_t* al = (mpg123_openal_t*)ao->userptr;

	alGetSourcei(al->source, AL_BUFFERS_QUEUED, &n);
	if(n < NUM_BUFFERS)
	{
		alGenBuffers(1, &al->buffer);
	}
	else
	{
		alGetSourcei(al->source, AL_SOURCE_STATE, &state);
		if(state != AL_PLAYING)
		{
			alSourcePlay(al->source);
		}
		while(alGetSourcei(al->source, AL_BUFFERS_PROCESSED, &n), n == 0)
		{	
			usleep(10000);
		}
		alSourceUnqueueBuffers(al->source, 1, &al->buffer);
	}
	
	alBufferData(al->buffer, al->format, buf, len, al->rate);
	alSourceQueueBuffers(al->source, 1, &al->buffer);
	
	return len;
}

static int close_openal(out123_handle *ao)
{
	ALint state, n;
	mpg123_openal_t* al = (mpg123_openal_t*)ao->userptr;

	if (al)
	{
		/* wait until all buffers are consumed */
		while(alGetSourcei(al->source, AL_SOURCE_STATE, &state), state == AL_PLAYING)
		{
			usleep(10000);
		}
		/* free all processed buffers */
		while(alGetSourcei(al->source, AL_BUFFERS_PROCESSED, &n), n > 0)
		{
			alSourceUnqueueBuffers(al->source, 1, &al->buffer);
			alDeleteBuffers(1, &al->buffer);
		}
		alDeleteSources(1, &al->source);
		alcMakeContextCurrent(NULL);
		alcDestroyContext(al->context);
		alcCloseDevice(al->device);
	}
	
	return 0;
}

static void flush_openal(out123_handle *ao)
{
	ALint n;
	mpg123_openal_t* al = (mpg123_openal_t*)ao->userptr;

	if (al)
	{
		/* stop playing and flush all buffers */
		alSourceStop(al->source);
		while(alGetSourcei(al->source, AL_BUFFERS_PROCESSED, &n), n > 0)
		{
			alSourceUnqueueBuffers(al->source, 1, &al->buffer);
			alDeleteBuffers(1, &al->buffer);
		}
	}
}

static int deinit_openal(out123_handle* ao)
{
	/* Free up memory */
	if(ao->userptr)
	{
		free( ao->userptr );
		ao->userptr = NULL;
	}

	/* Success */
	return 0;
}

static int init_openal(out123_handle* ao)
{
	if (ao==NULL) return -1;

	/* Set callbacks */
	ao->open = open_openal;
	ao->flush = flush_openal;
	ao->write = write_openal;
	ao->get_formats = get_formats_openal;
	ao->close = close_openal;
	ao->deinit = deinit_openal;

	/* Allocate memory for data structure */
	ao->userptr = malloc( sizeof( mpg123_openal_t ) );
	if(ao->userptr==NULL)
	{
		if(!AOQUIET)
			error("failed to malloc memory for 'mpg123_openal_t'");
		return -1;
	}
	memset( ao->userptr, 0, sizeof(mpg123_openal_t) );

	/* Success */
	return 0;
}


/* 
	Module information data structure
*/
mpg123_module_t mpg123_output_module_info = {
	/* api_version */	MPG123_MODULE_API_VERSION,
	/* name */			"openal",
	/* description */	"Output audio using OpenAL.",
	/* revision */		"$Rev:$",
	/* handle */		NULL,
	
	/* init_output */	init_openal,
};



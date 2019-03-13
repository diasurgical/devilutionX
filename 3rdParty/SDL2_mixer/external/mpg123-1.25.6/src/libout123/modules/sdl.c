/*
	sdl: audio output via SDL cross-platform API

	copyright 2006-2016 by the mpg123 project - free software under the terms of the LGPL 2.1
	see COPYING and AUTHORS files in distribution or http://mpg123.org
	initially written by Nicholas J. Humfrey
*/

#include "out123_int.h"
#include <math.h>

#include <SDL.h>

#ifdef WIN32
#include <windows.h>
#endif

/* Including the sfifo code locally, to avoid module linkage issues. */
#define SFIFO_STATIC
#include "sfifo.c"

#include "debug.h"


#define SAMPLE_SIZE			(2)
#define FRAMES_PER_BUFFER	(256)
/* Performance of SDL with ALSA is a bit of a mystery to me. Regardless
   of buffer size here, I just cannot avoid buffer underruns on my system.
   SDL always chooses 1024x2 periods, which seems to be just not quite
   enough on the Thinkpad:-/ Choosing 0.2 s as a plentiful default instead
   of 0.5 s which is just a lie. */
#define FIFO_DURATION		(ao->device_buffer > 0. ? ao->device_buffer : 0.2)
#define BUFFER_SAMPLES		((FIFO_DURATION*ao->rate)/2)

struct handle
{
	int finished; /* A flag for communicating end, one-way. */
	sfifo_t fifo;
};

/* Some busy waiting. Proper stuff like semaphores might add
   dependencies (POSIX) that the platform does not know. */
static void ms_sleep(int milliseconds)
{
#ifdef WIN32
		Sleep(milliseconds);
#else
		usleep(milliseconds*1000);
#endif
}

/* The audio function callback takes the following parameters:
       stream:  A pointer to the audio buffer to be filled
       len:     The length (in bytes) of the audio buffer
*/
static void audio_callback_sdl(void *udata, Uint8 *stream, int len)
{
	out123_handle *ao = (out123_handle*)udata;
	struct handle *sh = (struct handle*)ao->userptr;
	sfifo_t *fifo = &sh->fifo;
	int bytes_read;
	int bytes_avail;

	/* Until the finished flag is set, we will wait for more.
	   As the exact value does not matter and late detection of
	   a change is kindof OK, I do not see a thread safety problem here. */
	while((bytes_avail = sfifo_used(fifo)) < len && !sh->finished)
	{
		int ms = (len-bytes_avail)/ao->framesize*1000/ao->rate;
		debug1("waiting for more input, %d ms missing", ms);
		ms_sleep(ms/10);
	}
	/* Read audio from FIFO to SDL's buffer */
	if(bytes_avail > len)
		bytes_avail = len;
	bytes_read = sfifo_read( fifo, stream, bytes_avail );
	if(bytes_read != bytes_avail)
		warning2("Error reading from the FIFO (wanted=%d, bytes_read=%d).\n"
		,	bytes_avail, bytes_read);
	if(bytes_read < 0)
		bytes_read = 0;
	/* Ensure that any remaining space is filled with zero bytes. */
	if(bytes_read < len)
		memset(stream+bytes_read, 0, len-bytes_read);
}

static int open_sdl(out123_handle *ao)
{
	struct handle *sh = (struct handle*)ao->userptr;
	sfifo_t *fifo = &sh->fifo;

	/* Open an audio I/O stream. */
	if (ao->rate > 0 && ao->channels >0 ) {
		size_t ringbuffer_len;
		SDL_AudioSpec wanted;
	
		/* L16 uncompressed audio data, using 16-bit signed representation in twos 
		   complement notation - system endian-ness. */
		wanted.format = AUDIO_S16SYS;
		/* Seems reasonable to demand a buffer size related to the device
		   buffer. */
		wanted.samples = BUFFER_SAMPLES;
		wanted.callback = audio_callback_sdl; 
		wanted.userdata = ao; 
		wanted.channels = ao->channels; 
		wanted.freq = ao->rate; 

		sh->finished = 0;
		/* Open the audio device, forcing the desired format
		   Actually, it is still subject to constraints by hardware.
		   Need to have sample rate checked beforehand! SDL will
		   happily play 22 kHz files with 44 kHz hardware rate!
		   Same with channel count. No conversion. The manual is a bit
		   misleading on that (only talking about sample format, I guess). */
		if ( SDL_OpenAudio(&wanted, NULL) )
		{
			if(!AOQUIET)
				error1("Couldn't open SDL audio: %s\n", SDL_GetError());
			return -1;
		}
		
		/* Initialise FIFO */
		ringbuffer_len = ao->rate * FIFO_DURATION * SAMPLE_SIZE *ao->channels;
		debug2( "Allocating %d byte ring-buffer (%f seconds)", (int)ringbuffer_len, (float)FIFO_DURATION);
		if (sfifo_init( fifo, ringbuffer_len ) && !AOQUIET)
			error1( "Failed to initialise FIFO of size %d bytes", (int)ringbuffer_len );
	}
	
	return(0);
}


static int get_formats_sdl(out123_handle *ao)
{
	/* Got no better idea than to just take 16 bit and run with it */
	return MPG123_ENC_SIGNED_16;
#if 0
	/*
		This code would "properly" test audio format support.
		But thing is, SDL will always say yes and amen to everything, but it takes
		an awful amount of time to get all the variants tested (about 2 seconds,
		for example). I have seen SDL builds that do proper format conversion
		behind your back, I have seen builds that do not. Every build seems to
		claim that it does, though. Just hope you're lucky and your SDL works.
		Otherwise, use a proper audio output API.
	*/
	SDL_AudioSpec wanted, got;

	/* Only implemented Signed 16-bit audio for now.
	   The SDL manual doesn't suggest more interesting formats
	   like S24 or S32 anyway. */
	wanted.format = AUDIO_S16SYS;
	wanted.samples = BUFFER_SAMPLES;
	wanted.callback = audio_callback_sdl;
	wanted.userdata = ao;
	wanted.channels = ao->channels;
	wanted.freq = ao->rate;

	if(SDL_OpenAudio(&wanted, &got)) return 0;
	SDL_CloseAudio();
fprintf(stderr, "wanted rate: %li got rate %li\n", (long)wanted.freq, (long)got.freq);
	return (got.freq == ao->rate && got.channels == ao->channels)
		? MPG123_ENC_SIGNED_16
		: 0;
#endif
}


static int write_sdl(out123_handle *ao, unsigned char *buf, int len)
{
	struct handle *sh = (struct handle*)ao->userptr;
	sfifo_t *fifo = &sh->fifo;
	int len_remain = len;

	/* Some busy waiting, but feed what is possible. */
	while(len_remain) /* Note: input len is multiple of framesize! */
	{
		int block = sfifo_space(fifo);
		block -= block % ao->framesize;
		if(block > len_remain)
			block = len_remain;
		if(block)
		{
			sfifo_write(fifo, buf, block);
			len_remain -= block;
			buf += block;
			/* Unpause once the buffer is 50% full */
			if (sfifo_used(fifo) > (sfifo_size(fifo)/2) )
				SDL_PauseAudio(0);
		}
		if(len_remain)
		{
			debug1("Still need to write %d bytes, sleeping a bit.", len_remain);
			ms_sleep(0.1*FIFO_DURATION*1000);
		}
	}
	return len;
}

static int close_sdl(out123_handle *ao)
{
	int stuff;
	struct handle *sh = (struct handle*)ao->userptr;
	sfifo_t *fifo = &sh->fifo;

	debug1("close_sdl with %d", sfifo_used(fifo));
	sh->finished = 1;
	/* Wait at least until SDL emptied the FIFO. */
	while((stuff = sfifo_used(fifo))>0)
	{
		int ms = stuff/ao->framesize*1000/ao->rate;
		debug1("still stuff for about %i ms there", ms);
		ms_sleep(ms/2);
	}

	SDL_CloseAudio();
	
	/* Free up the memory used by the FIFO */
	sfifo_close( fifo );
	
	return 0;
}

static void flush_sdl(out123_handle *ao)
{
	struct handle *sh = (struct handle*)ao->userptr;

	SDL_PauseAudio(1);
	sfifo_flush(&sh->fifo);
}

/* You can only rely on that being called after successful init_sdl()!
   And sdl_close() should be called before to free the sfifo. */
static int deinit_sdl(out123_handle* ao)
{
	/* Free up memory */
	if (ao->userptr) {
		free( ao->userptr );
		ao->userptr = NULL;
	}

	/* Shut down SDL */
	SDL_Quit();

	/* Success */
	return 0;
}

/* Remember: If this returns failure, no additional cleanup happens.
   Resources must be freed here. */
static int init_sdl(out123_handle* ao)
{
	struct handle *sh;

	if (ao==NULL) return -1;
	
	/* Set callbacks */
	ao->open = open_sdl;
	ao->flush = flush_sdl;
	ao->write = write_sdl;
	ao->get_formats = get_formats_sdl;
	ao->close = close_sdl;
	ao->deinit = deinit_sdl;

	/* Initialise SDL */
	if (SDL_Init( SDL_INIT_AUDIO ) )
	{
		if(!AOQUIET)
			error1("Failed to initialise SDL: %s\n", SDL_GetError());
		return -1;
	}
	/* Allocate memory _after_ checking that SDL is available, so we do not
	   have to free after failure. */
	ao->userptr = sh = malloc( sizeof(struct handle) );
	if (ao->userptr==NULL)
	{
		if(!AOQUIET)
			error( "Failed to allocated memory for FIFO structure" );
		return -1;
	}
	sh->finished = 0;
	/* Not exactly necessary; only for somewhat safe sdl_close after a fake
	   sdl_open(). */
	memset( &sh->fifo, 0, sizeof(sfifo_t) );

	/* Success */
	return 0;
}


/* 
	Module information data structure
*/
mpg123_module_t mpg123_output_module_info = {
	/* api_version */	MPG123_MODULE_API_VERSION,
	/* name */			"sdl",
	/* description */	"Output audio using SDL (Simple DirectMedia Layer).",
	/* revision */		"$Rev:$",
	/* handle */		NULL,
	
	/* init_output */	init_sdl,
};

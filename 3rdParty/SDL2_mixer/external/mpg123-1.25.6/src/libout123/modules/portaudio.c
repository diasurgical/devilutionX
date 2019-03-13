/*
	portaudio: audio output via PortAudio cross-platform audio API

	copyright 2006-2016 by the mpg123 project - free software under the terms of the LGPL 2.1
	see COPYING and AUTHORS files in distribution or http://mpg123.org
	initially written by Nicholas J. Humfrey
*/

/* Need usleep(). */
#define _DEFAULT_SOURCE
#define _BSD_SOURCE
#include "out123_int.h"
#include <math.h>
#include <portaudio.h>

#ifdef WIN32
#include <windows.h>
#endif

/* Including the sfifo code locally, to avoid module linkage issues. */
#define SFIFO_STATIC
#include "sfifo.c"

#include "debug.h"

#define SAMPLE_SIZE			(2)
#define FRAMES_PER_BUFFER	(256)
#define FIFO_DURATION		(ao->device_buffer > 0. ? ao->device_buffer : 0.5f)


typedef struct {
	PaStream *stream;
	sfifo_t fifo;
	int finished;
} mpg123_portaudio_t;

#ifdef PORTAUDIO18
#define PaTime PaTimestamp
#define Pa_IsStreamActive Pa_StreamActive
#endif

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

#ifdef PORTAUDIO18
static int paCallback( void *inputBuffer, void *outputBuffer,
			 unsigned long framesPerBuffer,
			 PaTime outTime, void *userData )
#else
static int paCallback(
    const void *inputBuffer, void *outputBuffer,
    unsigned long framesPerBuffer,
    const PaStreamCallbackTimeInfo* timeInfo,
    PaStreamCallbackFlags statusFlags,
    void *userData )
#endif
{
	out123_handle *ao = userData;
	mpg123_portaudio_t *pa = (mpg123_portaudio_t*)ao->userptr;
	unsigned long bytes = framesPerBuffer * SAMPLE_SIZE * ao->channels;
	int bytes_avail;
	int bytes_read;

	while((bytes_avail=sfifo_used(&pa->fifo))<bytes && !pa->finished)
	{
		int ms = (bytes-bytes_avail)/ao->framesize*1000/ao->rate;
		debug3("waiting for more input, %d ms missing (%i < %lu)"
		,	ms, bytes_avail, bytes);
		ms_sleep(ms/10);
	}
	if(bytes_avail > bytes)
		bytes_avail = bytes;
	bytes_read = sfifo_read(&pa->fifo, outputBuffer, bytes_avail);
	if(bytes_read != bytes_avail)
		warning2("Error reading from the FIFO (wanted=%d, bytes_read=%d).\n"
		,	bytes_avail, bytes_read);
	if(bytes_read < 0)
		bytes_read = 0;
	/* Ensure that any remaining space is filled with zero bytes. */
	if(bytes_read >= 0 && bytes_read < bytes)
		memset((char*)outputBuffer+bytes_read, 0, bytes-bytes_read);

	debug1("callback successfully passed along %i B", bytes_read);
	return 0;
}


static int open_portaudio(out123_handle *ao)
{
	mpg123_portaudio_t *pa = (mpg123_portaudio_t*)ao->userptr;
	PaError err;

	pa->finished = 0;
	/* Open an audio I/O stream. */
	if (ao->rate > 0 && ao->channels >0 ) {
	
		err = Pa_OpenDefaultStream(
					&pa->stream,
					0,          	/* no input channels */
					ao->channels,	/* number of output channels */
					paInt16,		/* signed 16-bit samples */
					ao->rate,		/* sample rate */
					FRAMES_PER_BUFFER,	/* frames per buffer */
#ifdef PORTAUDIO18
					0,				/* number of buffers, if zero then use default minimum */
#endif
					paCallback,		/* no callback - use blocking IO */
					ao );
			
		if( err != paNoError ) {
			if(!AOQUIET)
				error1( "Failed to open PortAudio default stream: %s"
				,	Pa_GetErrorText(err) );
			return -1;
		}
		
		/* Initialise FIFO */
		sfifo_init( &pa->fifo, ao->rate * FIFO_DURATION * SAMPLE_SIZE *ao->channels );
									   
	}
	
	return(0);
}


static int get_formats_portaudio(out123_handle *ao)
{
	/* Only implemented Signed 16-bit audio for now */
	return MPG123_ENC_SIGNED_16;
}


static int write_portaudio(out123_handle *ao, unsigned char *buf, int len)
{
	mpg123_portaudio_t *pa = (mpg123_portaudio_t*)ao->userptr;
	PaError err;
	int len_remain = len;

	/* Some busy waiting, but feed what is possible. */
	while(len_remain) /* Note: input len is multiple of framesize! */
	{
		int block = sfifo_space(&pa->fifo);
		block -= block % ao->framesize;
		debug1("space for writing: %i", block);
		if(block > len_remain)
			block = len_remain;
		if(block)
		{
			sfifo_write(&pa->fifo, buf, block);
			len_remain -= block;
			buf += block;
			/* Start stream if not ative and 50 % full.*/
			if(sfifo_used(&pa->fifo) > (sfifo_size(&pa->fifo)/2))
			{
				pa->finished = 0;
				err = Pa_IsStreamActive( pa->stream );
				if (err == 0) {
					err = Pa_StartStream( pa->stream );
					if( err != paNoError ) {
						if(!AOQUIET)
							error1( "Failed to start PortAudio stream: %s"
							,	Pa_GetErrorText(err) );
						return -1; /* triggering exit here is not good, better handle that somehow... */
					}
					else
						debug("started stream");
				} else if (err < 0)
				{
					if(!AOQUIET)
						error1( "Failed to check state of PortAudio stream: %s"
						,	Pa_GetErrorText(err) );
					return -1;
				}
				else
					debug("stream already active");
			}
		}
		if(len_remain)
		{
			debug1("Still need to write %d bytes, sleeping a bit.", len_remain);
			ms_sleep(0.1*FIFO_DURATION*1000);
		}
	}

	return len;
}

static int close_portaudio(out123_handle *ao)
{
	mpg123_portaudio_t *pa = (mpg123_portaudio_t*)ao->userptr;
	PaError err;
	int stuff;

	debug1("close_portaudio with %d", sfifo_used(&pa->fifo));
	pa->finished = 1;
	/* Wait at least until the FIFO is empty. */
	while((stuff = sfifo_used(&pa->fifo))>0)
	{
		int ms = stuff/ao->framesize*1000/ao->rate;
		debug1("still stuff for about %i ms there", ms);
		ms_sleep(ms/2);
	}

	if (pa->stream) {
		/* stop the stream if it is active */
		if (Pa_IsStreamActive( pa->stream ) == 1) {
			err = Pa_StopStream( pa->stream );
			if( err != paNoError )
			{
				if(!AOQUIET)
					error1( "Failed to stop PortAudio stream: %s"
					,	Pa_GetErrorText(err) );
				return -1;
			}
		}
	
		/* and then close the stream */
		err = Pa_CloseStream( pa->stream );
		if( err != paNoError )
		{
			if(!AOQUIET)
				error1( "Failed to close PortAudio stream: %s"
				,	Pa_GetErrorText(err) );
			return -1;
		}
		
		pa->stream = NULL;
	}
	
	/* and free memory used by fifo */
	sfifo_close( &pa->fifo );
    
	return 0;
}


static void flush_portaudio(out123_handle *ao)
{
	mpg123_portaudio_t *pa = (mpg123_portaudio_t*)ao->userptr;
	/*PaError err;*/
	
	/* throw away contents of FIFO */
	sfifo_flush( &pa->fifo );

	/* and empty out PortAudio buffers */
	/*err = */
	Pa_AbortStream( pa->stream );
}


static int deinit_portaudio(out123_handle* ao)
{
	/* Free up memory */
	if (ao->userptr) {
		free( ao->userptr );
		ao->userptr = NULL;
	}

	/* Shut down PortAudio */
	Pa_Terminate();

	/* Success */
	return 0;
}


static int init_portaudio(out123_handle* ao)
{
	int err = paNoError;
	mpg123_portaudio_t *handle;

	if (ao==NULL) return -1;
	
	/* Set callbacks */
	ao->open = open_portaudio;
	ao->flush = flush_portaudio;
	ao->write = write_portaudio;
	ao->get_formats = get_formats_portaudio;
	ao->close = close_portaudio;
	ao->deinit = deinit_portaudio;

	/* Initialise PortAudio */
	err = Pa_Initialize();
	if( err != paNoError )
	{
		if(!AOQUIET)
			error1( "Failed to initialise PortAudio: %s"
			,	Pa_GetErrorText(err) );
		return -1;
	}

	/* Allocate memory for handle */
	ao->userptr = handle = malloc( sizeof(mpg123_portaudio_t) );
	if (ao->userptr==NULL)
	{
		if(!AOQUIET)
			error( "Failed to allocated memory for driver structure" );
		return -1;
	}
	handle->finished = 0;
	handle->stream = NULL;
	memset(&handle->fifo, 0, sizeof(sfifo_t));

	/* Success */
	return 0;
}



/* 
	Module information data structure
*/
mpg123_module_t mpg123_output_module_info = {
	/* api_version */	MPG123_MODULE_API_VERSION,
	/* name */			"portaudio",						
	/* description */	"Output audio using PortAudio",
	/* revision */		"$Rev:$",						
	/* handle */		NULL,
	
	/* init_output */	init_portaudio,						
};


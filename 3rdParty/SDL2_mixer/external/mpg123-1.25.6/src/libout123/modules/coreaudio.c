/*
	coreaudio: audio output on MacOS X

	copyright ?-2016 by the mpg123 project - free software under the terms of the GPL 2
	see COPYING and AUTHORS files in distribution or http://mpg123.org
	initially written by Guillaume Outters
	modified by Nicholas J Humfrey to use SFIFO code
	modified by Taihei Monma to use AudioUnit and AudioConverter APIs
*/


#include "out123_int.h"

/* has been around since at least 10.4 */
#include <AvailabilityMacros.h>

/* Use AudioComponents API when compiling for >= 10.6, otherwise fall back to
 * Components Manager, which is deprecated since 10.8.
 * MAC_OS_X_VERSION_MIN_REQUIRED defaults to the host system version and can be
 * governed by MACOSX_DEPLOYMENT_TARGET environment variable and
 * -mmacosx-version-min= when running the compiler. */
#if MAC_OS_X_VERSION_MIN_REQUIRED >= 1060 || __IPHONE_OS_VERSION_MIN_REQUIRED >= 20000
#define HAVE_AUDIOCOMPONENTS 1
#endif

#if HAVE_AUDIOCOMPONENTS
#define MPG123_AUDIOCOMPONENTDESCRIPTION AudioComponentDescription
#define MPG123_AUDIOCOMPONENT AudioComponent
#define MPG123_AUDIOCOMPONENTFINDNEXT AudioComponentFindNext
/* Funky API twist: AudioUnit is actually typedef'd AudioComponentInstance */
#define MPG123_AUDIOCOMPONENTINSTANCENEW AudioComponentInstanceNew
#define MPG123_AUDIOCOMPONENTINSTANCEDISPOSE AudioComponentInstanceDispose
#else
#include <CoreServices/CoreServices.h>
#define MPG123_AUDIOCOMPONENTDESCRIPTION ComponentDescription
#define MPG123_AUDIOCOMPONENT Component
#define MPG123_AUDIOCOMPONENTFINDNEXT FindNextComponent
#define MPG123_AUDIOCOMPONENTINSTANCENEW OpenAComponent
#define MPG123_AUDIOCOMPONENTINSTANCEDISPOSE CloseComponent
#endif
#include <AudioUnit/AudioUnit.h>
#include <AudioToolbox/AudioToolbox.h>
#include <errno.h>

/* Including the sfifo code locally, to avoid module linkage issues. */
#define SFIFO_STATIC
#include "sfifo.c"

#include "debug.h"

/* Duration of the ring buffer in seconds.
   Is that all that there is to tunable latency?
   Size of 200 ms should be enough for a default value, rare is the
   hardware that actually allows such large buffers. */
#define FIFO_DURATION (ao->device_buffer > 0. ? ao->device_buffer : 0.2)


typedef struct mpg123_coreaudio
{
	AudioConverterRef converter;
	AudioUnit outputUnit;
	int open;
	char play;
	int channels;
	int bps;
	int play_done;
	int decode_done;

	/* Convertion buffer */
	unsigned char * buffer;
	size_t buffer_size;
	
	/* Ring buffer */
	sfifo_t fifo;

} mpg123_coreaudio_t;



static OSStatus playProc(AudioConverterRef inAudioConverter,
						 UInt32 *ioNumberDataPackets,
                         AudioBufferList *outOutputData,
                         AudioStreamPacketDescription **outDataPacketDescription,
                         void* inClientData)
{
	out123_handle *ao = (out123_handle*)inClientData;
	mpg123_coreaudio_t *ca = (mpg123_coreaudio_t *)ao->userptr;
	long n;

	/* This is not actually a loop. See the early break. */
	for(n = 0; n < outOutputData->mNumberBuffers; n++)
	{
		unsigned int wanted = *ioNumberDataPackets * ca->channels * ca->bps;
		unsigned char *dest;
		unsigned int read;
		int avail;

		/* Any buffer count > 1 would wreck havoc with this code. */
		if(n > 0)
			break;

		if(ca->buffer_size < wanted) {
			debug1("Allocating %d byte sample conversion buffer", wanted);
			ca->buffer = realloc( ca->buffer, wanted);
			ca->buffer_size = wanted;
		}
		dest = ca->buffer;
		if(!dest)
			return -1;

		/* Only play if we have data left */
		while((avail=sfifo_used( &ca->fifo )) < wanted && !ca->decode_done)
		{
			int ms = (wanted-avail)/ao->framesize*1000/ao->rate;
			debug3("waiting for more input, %d ms missing (%i < %u)"
			,	ms, avail, wanted);
			usleep(ms*100); /* Wait for 1/10th of the missing duration. Might want to adjust. */
		}
		if(avail > wanted)
			avail = wanted;
		else if(ca->decode_done)
			ca->play_done = 1;

		/* Read audio from FIFO to CoreAudio's buffer */
		read = sfifo_read(&ca->fifo, dest, avail);
		
		if(read!=avail)
			warning2("Error reading from the ring buffer (avail=%u, read=%u).\n", avail, read);
		
		outOutputData->mBuffers[n].mDataByteSize = read;
		outOutputData->mBuffers[n].mData = dest;
	}
	
	return noErr; 
}

static OSStatus convertProc(void *inRefCon, AudioUnitRenderActionFlags *inActionFlags,
                            const AudioTimeStamp *inTimeStamp, UInt32 inBusNumber,
                            UInt32 inNumFrames, AudioBufferList *ioData)
{
	AudioStreamPacketDescription* outPacketDescription = NULL;
	out123_handle *ao = (out123_handle*)inRefCon;
	mpg123_coreaudio_t *ca = (mpg123_coreaudio_t *)ao->userptr;
	OSStatus err= noErr;
	
	err = AudioConverterFillComplexBuffer(ca->converter, playProc, inRefCon, &inNumFrames, ioData, outPacketDescription);
	
	return err;
}

static int open_coreaudio(out123_handle *ao)
{
	mpg123_coreaudio_t* ca = (mpg123_coreaudio_t*)ao->userptr;
	UInt32 size;
	MPG123_AUDIOCOMPONENTDESCRIPTION desc;
	MPG123_AUDIOCOMPONENT comp;
	AudioStreamBasicDescription inFormat;
	AudioStreamBasicDescription outFormat;
	AURenderCallbackStruct  renderCallback;
	Boolean outWritable;
	
	/* Initialize our environment */
	ca->play = 0;
	ca->buffer = NULL;
	ca->buffer_size = 0;
	ca->play_done = 0;
	ca->decode_done = 0;
	
	/* Get the default audio output unit */
	desc.componentType = kAudioUnitType_Output;
#if TARGET_OS_IPHONE
	desc.componentSubType = kAudioUnitSubType_RemoteIO;
#else
	desc.componentSubType = kAudioUnitSubType_DefaultOutput;
#endif
	desc.componentManufacturer = kAudioUnitManufacturer_Apple;
	desc.componentFlags = 0;
	desc.componentFlagsMask = 0;
	comp = MPG123_AUDIOCOMPONENTFINDNEXT(NULL, &desc);
	if(comp == NULL)
	{
		if(!AOQUIET)
			error("AudioComponentFindNext failed");
		return(-1);
	}
	
	if(MPG123_AUDIOCOMPONENTINSTANCENEW(comp, &(ca->outputUnit)))
	{
		if(!AOQUIET)
			error("AudioComponentInstanceNew failed");
		return (-1);
	}
	
	if(AudioUnitInitialize(ca->outputUnit))
	{
		if(!AOQUIET)
			error("AudioUnitInitialize failed");
		return (-1);
	}
	
	/* Specify the output PCM format */
	AudioUnitGetPropertyInfo(ca->outputUnit, kAudioUnitProperty_StreamFormat, kAudioUnitScope_Output, 0, &size, &outWritable);
	if(AudioUnitGetProperty(ca->outputUnit, kAudioUnitProperty_StreamFormat, kAudioUnitScope_Output, 0, &outFormat, &size))
	{
		if(!AOQUIET)
			error("AudioUnitGetProperty(kAudioUnitProperty_StreamFormat) failed");
		return (-1);
	}
	
	if(AudioUnitSetProperty(ca->outputUnit, kAudioUnitProperty_StreamFormat, kAudioUnitScope_Input, 0, &outFormat, size))
	{
		if(!AOQUIET)
			error("AudioUnitSetProperty(kAudioUnitProperty_StreamFormat) failed");
		return (-1);
	}
	
	/* Specify the input PCM format */
	ca->channels = ao->channels;
	inFormat.mSampleRate = ao->rate;
	inFormat.mChannelsPerFrame = ao->channels;
	inFormat.mFormatID = kAudioFormatLinearPCM;
#ifdef _BIG_ENDIAN
	inFormat.mFormatFlags = kLinearPCMFormatFlagIsPacked | kLinearPCMFormatFlagIsBigEndian;
#else
	inFormat.mFormatFlags = kLinearPCMFormatFlagIsPacked;
#endif
	
	switch(ao->format)
	{
		case MPG123_ENC_SIGNED_16:
			inFormat.mFormatFlags |= kLinearPCMFormatFlagIsSignedInteger;
			ca->bps = 2;
			break;
		case MPG123_ENC_SIGNED_8:
			inFormat.mFormatFlags |= kLinearPCMFormatFlagIsSignedInteger;
			ca->bps = 1;
			break;
		case MPG123_ENC_UNSIGNED_8:
			ca->bps = 1;
			break;
		case MPG123_ENC_SIGNED_32:
			inFormat.mFormatFlags |= kLinearPCMFormatFlagIsSignedInteger;
			ca->bps = 4;
			break;
		case MPG123_ENC_FLOAT_32:
			inFormat.mFormatFlags |= kLinearPCMFormatFlagIsFloat;
			ca->bps = 4;
			break;
	}
	
	inFormat.mBitsPerChannel = ca->bps << 3;
	inFormat.mBytesPerPacket = ca->bps*inFormat.mChannelsPerFrame;
	inFormat.mFramesPerPacket = 1;
	inFormat.mBytesPerFrame = ca->bps*inFormat.mChannelsPerFrame;
	
	/* Add our callback - but don't start it yet */
	memset(&renderCallback, 0, sizeof(AURenderCallbackStruct));
	renderCallback.inputProc = convertProc;
	renderCallback.inputProcRefCon = ao;
	if(AudioUnitSetProperty(ca->outputUnit, kAudioUnitProperty_SetRenderCallback, kAudioUnitScope_Input, 0, &renderCallback, sizeof(AURenderCallbackStruct)))
	{
		if(!AOQUIET)
			error("AudioUnitSetProperty(kAudioUnitProperty_SetRenderCallback) failed");
		return(-1);
	}
	
	
	/* Open an audio I/O stream and create converter */
	if (ao->rate > 0 && ao->channels >0 ) {
		int ringbuffer_len;

		if(AudioConverterNew(&inFormat, &outFormat, &(ca->converter)))
		{
			if(!AOQUIET)
				error("AudioConverterNew failed");
			return(-1);
		}
		if(ao->channels == 1) {
			SInt32 channelMap[2] = { 0, 0 };
			if(AudioConverterSetProperty(ca->converter, kAudioConverterChannelMap, sizeof(channelMap), channelMap))
			{
				if(!AOQUIET)
					error("AudioConverterSetProperty(kAudioConverterChannelMap) failed");
				return(-1);
			}
		}
		
		/* Initialise FIFO */
		ringbuffer_len = ao->rate * FIFO_DURATION * ca->bps * ao->channels;
		debug2( "Allocating %d byte ring-buffer (%f seconds)", ringbuffer_len, (float)FIFO_DURATION);
		sfifo_init( &ca->fifo, ringbuffer_len );
	}
	
	return(0);
}

static int get_formats_coreaudio(out123_handle *ao)
{
	return MPG123_ENC_SIGNED_16|MPG123_ENC_SIGNED_8|MPG123_ENC_UNSIGNED_8|MPG123_ENC_SIGNED_32|MPG123_ENC_FLOAT_32;
}

static int write_coreaudio(out123_handle *ao, unsigned char *buf, int len)
{
	mpg123_coreaudio_t* ca = (mpg123_coreaudio_t*)ao->userptr;
	int len_remain = len;

	/* Some busy waiting, but feed what is possible. */
	while(len_remain) /* Note: input len is multiple of framesize! */
	{
		int block = sfifo_space(&ca->fifo);
		block -= block % ao->framesize;
		if(block > len_remain)
			block = len_remain;
		if(block)
		{
			sfifo_write(&ca->fifo, buf, block);
			len_remain -= block;
			buf += block;
			/* Start playback now that we have something to play */
			if(!ca->play && (sfifo_used(&ca->fifo) > (sfifo_size(&ca->fifo)/2)))
			{
				if(AudioOutputUnitStart(ca->outputUnit))
				{
					if(!AOQUIET)
						error("AudioOutputUnitStart failed");
					return(-1);
				}
				ca->play = 1;
			}
		}
		/* If there is no room, then sleep for a bit, but not too long. */
		if(len_remain)
			usleep( (0.1*FIFO_DURATION) * 1000000 );
	}

	return len;
}

static int close_coreaudio(out123_handle *ao)
{
	mpg123_coreaudio_t* ca = (mpg123_coreaudio_t*)ao->userptr;

	if (ca) {
		ca->decode_done = 1;
		while(!ca->play_done && ca->play)
			usleep((0.1*FIFO_DURATION)*1000000);
		/* No matter the error code, we want to close it (by brute force if necessary) */
		AudioOutputUnitStop(ca->outputUnit);
		AudioUnitUninitialize(ca->outputUnit);
		MPG123_AUDIOCOMPONENTINSTANCEDISPOSE(ca->outputUnit);
		AudioConverterDispose(ca->converter);
	
	    /* Free the ring buffer */
		sfifo_close( &ca->fifo );
		
		/* Free the conversion buffer */
		if (ca->buffer) {
			free( ca->buffer );
			ca->buffer = NULL;
		}
		
	}
	
	return 0;
}

static void flush_coreaudio(out123_handle *ao)
{
	mpg123_coreaudio_t* ca = (mpg123_coreaudio_t*)ao->userptr;

	/* Flush AudioConverter's buffer */
	if(AudioConverterReset(ca->converter))
	{
		if(!AOQUIET)
			error("AudioConverterReset failed");
	}
	
	/* Empty out the ring buffer */
	sfifo_flush( &ca->fifo );	
}

static int deinit_coreaudio(out123_handle* ao)
{
	/* Free up memory */
	if (ao->userptr) {
		free( ao->userptr );
		ao->userptr = NULL;
	}

	/* Success */
	return 0;
}

static int init_coreaudio(out123_handle* ao)
{
	if (ao==NULL) return -1;

	/* Set callbacks */
	ao->open = open_coreaudio;
	ao->flush = flush_coreaudio;
	ao->write = write_coreaudio;
	ao->get_formats = get_formats_coreaudio;
	ao->close = close_coreaudio;
	ao->deinit = deinit_coreaudio;

	/* Allocate memory for data structure */
	ao->userptr = malloc( sizeof( mpg123_coreaudio_t ) );
	if (ao->userptr==NULL)
	{
		if(!AOQUIET)
			error("failed to malloc memory for 'mpg123_coreaudio_t'");
		return -1;
	}
	memset( ao->userptr, 0, sizeof(mpg123_coreaudio_t) );

	/* Success */
	return 0;
}



/* 
	Module information data structure
*/
mpg123_module_t mpg123_output_module_info = {
	/* api_version */	MPG123_MODULE_API_VERSION,
	/* name */			"coreaudio",						
	/* description */	"Output audio using Mac OS X's CoreAudio.",
	/* revision */		"$Rev:$",
	/* handle */		NULL,
	
	/* init_output */	init_coreaudio,						
};



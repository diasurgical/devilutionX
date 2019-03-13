/*
	aix: Driver for IBM RS/6000 with AIX Ultimedia Services

	copyright ?-2016 by the mpg123 project - free software under the terms of the LGPL 2.1
	see COPYING and AUTHORS files in distribution or http://mpg123.org
	initially written by Juergen Schoew and Tomas Oegren
*/

#include "out123_int.h"

#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>

#include <errno.h>
#include <fcntl.h>
#include <sys/audio.h>
#include <stropts.h>
#include <sys/types.h>
#include <sys/file.h>
#include <sys/stat.h>
#include <sys/param.h>

#include "debug.h"

/* use AUDIO_BSIZE to set the msec for audio buffering in Ultimedia library
 */
/* #define AUDIO_BSIZE AUDIO_IGNORE */
#define AUDIO_BSIZE ( ao->device_buffer > 0. \
? (long)(ao->device_buffer*1000) \
: 200 )

static int rate_best_match(out123_handle *ao)
{
	static long valid [ ] = {  5510,  6620,  8000,  9600, 11025, 16000, 18900,
							22050, 27420, 32000, 33075, 37800, 44100, 48000, 0 };
	int  i = 0;
	long best = 8000;

	if(!ao || ao->fn < 0 || ao->rate < 0) {
		return -1;
	} 
	
	while (valid [i]) {
		if (abs(valid[i] - ao->rate) < abs(best - ao->rate))
		{
			best = valid [i];
		}
		i = i + 1;
	}
	
	ao->rate = best;
	return best;
}

static int reset_parameters(out123_handle *ao)
{
	audio_control  acontrol;
	audio_change   achange;
	audio_init     ainit;
	int ret;

	memset ( & achange, '\0', sizeof (achange));
	memset ( & acontrol, '\0', sizeof (acontrol));
	
	achange.balance        = 0x3fff0000;
	achange.balance_delay  = 0;
	achange.volume         = (long) (0x7fff << 16);
	achange.volume_delay   = 0;
	achange.input          = AUDIO_IGNORE;
	if (ao->flags == -1) achange.output = INTERNAL_SPEAKER;
	else achange.output      = 0;
	
	if(ao->flags & OUT123_INTERNAL_SPEAKER)
	achange.output     |= INTERNAL_SPEAKER;
	if(ao->flags & OUT123_HEADPHONES)
	achange.output     |= EXTERNAL_SPEAKER;
	if(ao->flags & OUT123_LINE_OUT)
	achange.output     |= OUTPUT_1;
	if(ao->flags == 0)
	achange.output      = AUDIO_IGNORE;

	achange.treble         = AUDIO_IGNORE;
	achange.bass          = AUDIO_IGNORE;
	achange.pitch          = AUDIO_IGNORE;
	achange.monitor        = AUDIO_IGNORE;
	achange.dev_info       = (char *) NULL;
	
	acontrol.ioctl_request = AUDIO_CHANGE;
	acontrol.position      = 0;
	acontrol.request_info  = (char *) & achange;
	
	ret = ioctl (ao->fn, AUDIO_CONTROL, & acontrol);
	if (ret < 0)
	return ret;
	
	/* Init Device for new values */
	if (ao->rate >0) {
		memset ( & ainit, '\0', sizeof (ainit));
		ainit.srate                 = rate_best_match(ao);
		if (ao->channels > 0)
		ainit.channels          = ao->channels;
		else
		ainit.channels           = 1;
		switch (ao->format) {
			default :
				ainit.mode             = PCM;
				ainit.bits_per_sample  = 8;
				ainit.flags            = BIG_ENDIAN | TWOS_COMPLEMENT;
			break;
			case MPG123_ENC_SIGNED_16:
				ainit.mode             = PCM;
				ainit.bits_per_sample  = 16;
				ainit.flags            = BIG_ENDIAN | TWOS_COMPLEMENT;
			break;
			case MPG123_ENC_SIGNED_8:
				ainit.mode             = PCM;
				ainit.bits_per_sample  = 8;
				ainit.flags            = BIG_ENDIAN | TWOS_COMPLEMENT;
			break;
			case MPG123_ENC_UNSIGNED_16:
				ainit.mode             = PCM;
				ainit.bits_per_sample  = 16;
				ainit.flags            = BIG_ENDIAN | TWOS_COMPLEMENT | SIGNED;
			break;
			case MPG123_ENC_UNSIGNED_8:
				ainit.mode             = PCM;
				ainit.bits_per_sample  = 8;
				ainit.flags            = BIG_ENDIAN | TWOS_COMPLEMENT | SIGNED;
			break;
			case MPG123_ENC_ULAW_8:
				ainit.mode             = MU_LAW;
				ainit.bits_per_sample  = 8;
				ainit.flags            = BIG_ENDIAN | TWOS_COMPLEMENT;
			break;
			case MPG123_ENC_ALAW_8:
				ainit.mode             = A_LAW;
				ainit.bits_per_sample  = 8;
				ainit.flags            = BIG_ENDIAN | TWOS_COMPLEMENT;
			break;
		}
		ainit.operation            = PLAY;
		ainit.bsize                = AUDIO_BSIZE;
		
		ret = ioctl (ao->fn, AUDIO_INIT, & ainit);
		if (ret < 0) {
			error("Can't set new audio parameters!");
			return ret;
		}
	}
	
	acontrol.ioctl_request   = AUDIO_START;
	acontrol.request_info    = NULL;
	acontrol.position        = 0;
	
	ret = ioctl (ao->fn, AUDIO_CONTROL, & acontrol);
	if (ret < 0) {
	error("Can't reset audio!");
	return ret;
	}
	return 0;
}

static int open_aix(out123_handle *ao)
{
	audio_init ainit;
	int ret;
	const char *dev = ao->device;

	if(!dev) {
		if(getenv("AUDIODEV")) {
			dev = getenv("AUDIODEV");
			ao->fn = open(dev,O_WRONLY);
		} else {
			dev = "/dev/paud0/1";                   /* paud0 for PCI */
			ao->fn = open(dev,O_WRONLY);
			if ((ao->fn == -1) & (errno == ENOENT)) {
				dev = "/dev/baud0/1";                 /* baud0 for MCA */
				ao->fn = open(dev,O_WRONLY);
			}  
		}
	} else ao->fn = open(dev,O_WRONLY);
	
	if(ao->fn < 0) {
		error("Can't open audio device!");
		return ao->fn;
	}
	
	/* Init to default values */
	memset ( & ainit, '\0', sizeof (ainit));
	ainit.srate            = 44100;
	ainit.channels         = 2;
	ainit.mode             = PCM;
	ainit.bits_per_sample  = 16;
	ainit.flags            = BIG_ENDIAN | TWOS_COMPLEMENT;
	ainit.operation        = PLAY;
	ainit.bsize            = AUDIO_BSIZE;
	
	ret = ioctl (ao->fn, AUDIO_INIT, & ainit);
	if (ret < 0) return ret;
	
	reset_parameters(ao);
	return ao->fn;
}



static int get_formats_aix(out123_handle *ao)
{
	/* ULTIMEDIA DOCUMENTATION SAYS:
	The Ultimedia Audio Adapter supports fourteen sample rates you can use to
	capture and playback audio data. The rates are (in kHz): 5.51, 6.62, 8.0,
	9.6, 11.025, 16.0, 18.9, 22.050, 27.42, 32.0, 33.075, 37.8, 44.1, and 48.0.
	These rates are supported for mono and stereo PCM (8- and 16-bit), mu-law,
	and A-law. 
	*/
	
	long rate;
	
	rate = ao->rate;
	rate_best_match(ao);
	if (ao->rate == rate)
		return (MPG123_ENC_SIGNED_16|MPG123_ENC_UNSIGNED_16|
			MPG123_ENC_UNSIGNED_8|MPG123_ENC_SIGNED_8|
			MPG123_ENC_ULAW_8|MPG123_ENC_ALAW_8);
	else
		return 0;
}

static int write_aix(out123_handle *ao,unsigned char *buf,int len)
{
	return write(ao->fn,buf,len);
}

static int close_aix(out123_handle *ao)
{
	audio_control acontrol;
	audio_buffer  abuffer;
	int           ret,i;
	
	/* Don't close the audio-device until it's played all its contents */
	memset ( & acontrol, '\0', sizeof ( acontrol ) );
	acontrol.request_info = &abuffer;
	acontrol.position = 0;
	i=50;   /* Don't do this forever on a bad day :-) */
	
	while (i-- > 0) {
		if ((ioctl(ao->fn, AUDIO_BUFFER, &acontrol))< 0) {
			error1("buffer read failed: %d", errno);
			break;
		} else {
			if (abuffer.flags <= 0) break;
		}
		usleep(200000); /* sleep 0.2 sec */
	}
	
	memset ( & acontrol, '\0', sizeof ( acontrol ) );
	acontrol.ioctl_request = AUDIO_STOP;
	acontrol.request_info  = NULL;
	acontrol.position      = 0;
	
	ret = ioctl ( ao->fn, AUDIO_CONTROL, & acontrol );
	if (ret < 0) error("Can't close audio!");
	
	ret = close (ao->fn);
	if (ret < 0) error("Can't close audio!");
	
	return 0;
}

static void flush_aix(out123_handle *ao)
{
}

static int init_aix(out123_handle* ao)
{
	if (ao==NULL) return -1;

	/* Set callbacks */
	ao->open = open_aix;
	ao->flush = flush_aix;
	ao->write = write_aix;
	ao->get_formats = get_formats_aix;
	ao->close = close_aix;

	/* Success */
	return 0;
}



/* 
	Module information data structure
*/
mpg123_module_t mpg123_output_module_info = {
	/* api_version */	MPG123_MODULE_API_VERSION,
	/* name */			"aix",						
	/* description */	"Output audio on IBM RS/6000 with AIX Ultimedia Services.",
	/* revision */		"$Rev: 932 $",						
	/* handle */		NULL,
	
	/* init_output */	init_aix,						
};


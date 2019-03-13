/*
	oss: audio output via Open Sound System

	copyright ?-2006 by the mpg123 project - free software under the terms of the LGPL 2.1
	see COPYING and AUTHORS files in distribution or http://mpg123.org
	initially written by Michael Hipp
*/

#include "out123_int.h"

#include <sys/ioctl.h>
#include <fcntl.h>

#ifdef HAVE_LINUX_SOUNDCARD_H
#include <linux/soundcard.h>
#endif

#ifdef HAVE_SYS_SOUNDCARD_H
#include <sys/soundcard.h>
#endif

#ifdef HAVE_MACHINE_SOUNDCARD_H
#include <machine/soundcard.h>
#endif

#ifndef AFMT_S16_NE
# ifdef OSS_BIG_ENDIAN
#  define AFMT_S16_NE AFMT_S16_BE
# else
#  define AFMT_S16_NE AFMT_S16_LE
# endif
#endif

#ifndef AFMT_U16_NE
# ifdef OSS_BIG_ENDIAN
#  define AFMT_U16_NE AFMT_U16_BE
# else
#  define AFMT_U16_NE AFMT_U16_LE
# endif
#endif

#include "debug.h"

struct oss_stuff
{
	int fragment; /* size of one fragment */
	int nfrag;    /* number of fragments  */
};

static int rate_best_match_oss(out123_handle *ao)
{
	int ret,dsp_rate;
	
	if(!ao || ao->fn < 0 || ao->rate < 0) return -1;
	dsp_rate = ao->rate;
	
	ret = ioctl(ao->fn, SNDCTL_DSP_SPEED,&dsp_rate);
	if(ret < 0) return ret;
	ao->rate = dsp_rate;
	return 0;
}

static int set_rate_oss(out123_handle *ao)
{
	int dsp_rate;
	int ret = 0;
	
	if(ao->rate >= 0) {
		dsp_rate = ao->rate;
		ret = ioctl(ao->fn, SNDCTL_DSP_SPEED,&dsp_rate);
	}
	return ret;
}

static int set_channels_oss(out123_handle *ao)
{
	int chan = ao->channels - 1;
	int ret;
	
	if(ao->channels < 0) return 0;
	
	ret = ioctl(ao->fn, SNDCTL_DSP_STEREO, &chan);
	if(chan != (ao->channels-1)) return -1;

	return ret;
}

static int set_format_oss(out123_handle *ao)
{
	int fmts;
	int sf,ret;

	if(ao->format == -1) return 0;

	switch(ao->format) {
		case MPG123_ENC_SIGNED_16:
		default:
			fmts = AFMT_S16_NE;
			break;
		case MPG123_ENC_UNSIGNED_8:
			fmts = AFMT_U8;
		break;
		case MPG123_ENC_SIGNED_8:
			fmts = AFMT_S8;
		break;
		case MPG123_ENC_ULAW_8:
			fmts = AFMT_MU_LAW;
		break;
		case MPG123_ENC_ALAW_8:
			fmts = AFMT_A_LAW;
		break;
		case MPG123_ENC_UNSIGNED_16:
			fmts = AFMT_U16_NE;
		break;
	}
	
	sf = fmts;
	ret = ioctl(ao->fn, SNDCTL_DSP_SETFMT, &fmts);
	if(sf != fmts) return -1;

	return ret;
}


static int reset_parameters_oss(out123_handle *ao)
{
	int ret;
	ret = ioctl(ao->fn, SNDCTL_DSP_RESET, NULL);
	if(ret < 0 && !AOQUIET) error("Can't reset audio!");
	ret = set_format_oss(ao);
	if (ret == -1) goto err;
	ret = set_channels_oss(ao);
	if (ret == -1) goto err;
	ret = set_rate_oss(ao);
	if (ret == -1) goto err;

	/* Careful here.  As per OSS v1.1, the next ioctl() commits the format
	 * set above, so we must issue SNDCTL_DSP_RESET before we're allowed to
	 * change it again. [dk]
	 */
   
/*  FIXME: this needs re-enabled (but not using global variables this time):
	if (ioctl(ao->fn, SNDCTL_DSP_GETBLKSIZE, &outburst) == -1 ||
      outburst > MAXOUTBURST)
    outburst = MAXOUTBURST;
*/

err:
	return ret;
}


static int open_oss(out123_handle *ao)
{
	char usingdefdev = 0;
	const char *dev;

	if(!ao) return -1;

	dev = ao->device;
	if(!dev) {
		dev = "/dev/dsp";
		usingdefdev = 1;
	}
	
	ao->fn = open(dev,O_WRONLY);  
	
	if(ao->fn < 0)
	{
		if(usingdefdev) {
			dev = "/dev/sound/dsp";
			ao->fn = open(dev,O_WRONLY);
			if(ao->fn < 0) {
				if(!AOQUIET) error("Can't open default sound device!");
				return -1;
			}
		} else {
			if(!AOQUIET) error1("Can't open %s!",dev);
			return -1;
		}
	}
	
	if(reset_parameters_oss(ao) < 0) {
		close(ao->fn);
		return -1;
	}
	
	if(ao->gain >= 0) {
		int e,mask;
		e = ioctl(ao->fn , SOUND_MIXER_READ_DEVMASK ,&mask);
		if(e < 0) {
			if(!AOQUIET) error("audio/gain: Can't get audio device features list.");
		}
		else if(mask & SOUND_MASK_PCM) {
			int gain = (ao->gain<<8)|(ao->gain);
			e = ioctl(ao->fn, SOUND_MIXER_WRITE_PCM , &gain);
		}
		else if(!(mask & SOUND_MASK_VOLUME)) {
			if(!AOQUIET) error1("audio/gain: setable Volume/PCM-Level not supported by your audio device: %#04x",mask);
		}
		else { 
			int gain = (ao->gain<<8)|(ao->gain);
			e = ioctl(ao->fn, SOUND_MIXER_WRITE_VOLUME , &gain);
		}
	}

	return ao->fn;
}



/*
 * get formats for specific channel/rate parameters
 */
static int get_formats_oss(out123_handle *ao)
{
	int fmt = 0;
	int r = ao->rate;
	int c = ao->channels;
	int i;
	
	static int fmts[] = { 
		MPG123_ENC_ULAW_8 , MPG123_ENC_SIGNED_16 ,
		MPG123_ENC_UNSIGNED_8 , MPG123_ENC_SIGNED_8 ,
		MPG123_ENC_UNSIGNED_16 , MPG123_ENC_ALAW_8
	};
	
	/* Reset is required before we're allowed to set the new formats. [dk] */
	ioctl(ao->fn, SNDCTL_DSP_RESET, NULL);
	
	for(i=0;i<6;i++) {
		ao->format = fmts[i];
		if(set_format_oss(ao) < 0) {
			continue;
		}
		ao->channels = c;
		if(set_channels_oss(ao) < 0) {
			continue;
		}
		ao->rate = r;
		if(rate_best_match_oss(ao) < 0) {
			continue;
		}
		if( (ao->rate*100 > r*(100-AUDIO_RATE_TOLERANCE)) && (ao->rate*100 < r*(100+AUDIO_RATE_TOLERANCE)) ) {
			fmt |= fmts[i];
		}
	}


#if 0
	if(ioctl(ao->fn,SNDCTL_DSP_GETFMTS,&fmts) < 0) {
		if(!AOQUIET) error("Failed to get SNDCTL_DSP_GETFMTS");
		return -1;
	}

	if(fmts & AFMT_MU_LAW)
		ret |= MPG123_ENC_ULAW_8;
	if(fmts & AFMT_S16_NE)
		ret |= MPG123_ENC_SIGNED_16;
	if(fmts & AFMT_U8)
		ret |= MPG123_ENC_UNSIGNED_8;
	if(fmts & AFMT_S8)
		ret |= MPG123_ENC_SIGNED_8;
	if(fmts & AFMT_U16_NE)
		ret |= MPG123_ENC_UNSIGNED_16;
	if(fmts & AFMT_A_LAW)
		ret |= MPG123_ENC_ALAW_8;
#endif

	return fmt;
}

static int write_oss(out123_handle *ao,unsigned char *buf,int len)
{
	return write(ao->fn,buf,len);
}

static int close_oss(out123_handle *ao)
{
	close(ao->fn);
	return 0;
}

static void flush_oss(out123_handle *ao)
{
}




static int init_oss(out123_handle* ao)
{
	if (ao==NULL) return -1;

	/* Set callbacks */
	ao->open = open_oss;
	ao->flush = flush_oss;
	ao->write = write_oss;
	ao->get_formats = get_formats_oss;
	ao->close = close_oss;
	
	/* Success */
	return 0;
}



/* 
	Module information data structure
*/
mpg123_module_t mpg123_output_module_info = {
	/* api_version */	MPG123_MODULE_API_VERSION,
	/* name */			"oss",
	/* description */	"Output audio using OSS",
	/* revision */		"$Rev: 4021 $",
	/* handle */		NULL,
	
	/* init_output */	init_oss,
};



/*
	sun: audio output for Sun systems

	copyright ?-2006 by the mpg123 project - free software under the terms of the LGPL 2.1
	see COPYING and AUTHORS files in distribution or http://mpg123.org
	initially written by Michael Hipp
*/

#include "out123_int.h"

#ifdef HAVE_SYS_IOCTL_H
#include <sys/ioctl.h>
#endif

#ifdef HAVE_SUN_AUDIOIO_H
#include <sun/audioio.h>
#endif

#ifdef HAVE_SYS_AUDIOIO_H
#include <sys/audioio.h>
#endif

#ifdef HAVE_SYS_AUDIO_H
#include <sys/audio.h>
#endif

#ifdef HAVE_ASM_AUDIOIO_H
#include <asm/audioio.h>
#endif

#include <fcntl.h>
#include "debug.h"

static void set_format_helper(out123_handle *ao, audio_info_t *ainfo)
{
	
	switch(ao->format) {
		case -1:
		case MPG123_ENC_SIGNED_16:
		default:
#ifndef AUDIO_ENCODING_LINEAR	/* not supported */
#define AUDIO_ENCODING_LINEAR 3
#endif
			ainfo->play.encoding = AUDIO_ENCODING_LINEAR;
			ainfo->play.precision = 16;
		break;
		case MPG123_ENC_UNSIGNED_8:
#if defined(SOLARIS) || defined(SPARCLINUX)
			ainfo->play.encoding = AUDIO_ENCODING_LINEAR8;
			ainfo->play.precision = 8;
		break;
#endif
		case MPG123_ENC_SIGNED_8:
			if(!AOQUIET)
				error("Linear signed 8 bit not supported!");
		return;
		case MPG123_ENC_ULAW_8:
			ainfo->play.encoding = AUDIO_ENCODING_ULAW;
			ainfo->play.precision = 8;
		break;
		case MPG123_ENC_ALAW_8:
			ainfo->play.encoding = AUDIO_ENCODING_ALAW;
			ainfo->play.precision = 8;
		break;
	}
  
}


static int reset_parameters_sun(out123_handle *ao)
{
	audio_info_t ainfo;

	AUDIO_INITINFO(&ainfo);
	if(ao->rate != -1) ainfo.play.sample_rate = ao->rate;

	if(ao->channels >= 0) ainfo.play.channels = ao->channels;

	set_format_helper(ao,&ainfo);
	if(ioctl(ao->fn, AUDIO_SETINFO, &ainfo) == -1) return -1;

	return 0;
}

static int rate_best_match(out123_handle *ao)
{
	audio_info_t ainfo;
	AUDIO_INITINFO(&ainfo);
	
	ainfo.play.sample_rate = ao->rate;
	if(ioctl(ao->fn, AUDIO_SETINFO, &ainfo) < 0) {
		ao->rate = 0;
		return 0;
	}
	if(ioctl(ao->fn, AUDIO_GETINFO, &ainfo) < 0) {
		return -1;
	}
	
	ao->rate = ainfo.play.sample_rate;
	return 0;
}

static int set_rate(out123_handle *ao)
{
	audio_info_t ainfo;
	
	if(ao->rate != -1) {
		AUDIO_INITINFO(&ainfo);
		ainfo.play.sample_rate = ao->rate;
		if(ioctl(ao->fn, AUDIO_SETINFO, &ainfo) == -1) return -1;
		return 0;
	}
	
	return -1;
}

static int set_channels(out123_handle *ao)
{
	audio_info_t ainfo;
	
	AUDIO_INITINFO(&ainfo);
	ainfo.play.channels = ao->channels;
	if(ioctl(ao->fn, AUDIO_SETINFO, &ainfo) == -1)
		return -1;

	return 0;
}

static int set_format(out123_handle *ao)
{
	audio_info_t ainfo;
	
	AUDIO_INITINFO(&ainfo);
	set_format_helper(ao,&ainfo);
	if(ioctl(ao->fn, AUDIO_SETINFO, &ainfo) == -1)
		return -1;
	
	return 0;
}

static int open_sun(out123_handle *ao)
{
	audio_info_t ainfo;
	const char *dev = ao->device;

	if(!dev) {
		if(getenv("AUDIODEV")) {
			dev = getenv("AUDIODEV");
		} else {
			dev = "/dev/audio";
		}
	}
	
	ao->fn = open(dev,O_WRONLY);
	if(ao->fn < 0) return ao->fn;
	
#if defined(SUNOS)  &&  defined(AUDIO_GETDEV)
	{
		int type;
		if(ioctl(ao->fn, AUDIO_GETDEV, &type) == -1) return -1;
		if(type == AUDIO_DEV_UNKNOWN || type == AUDIO_DEV_AMD)
			return -1;
	}
#else
#if defined(SOLARIS) || defined(SPARCLINUX)
	{
		struct audio_device ad;
		if(ioctl(ao->fn, AUDIO_GETDEV, &ad) == -1)
			return -1;
		if(!strstr(ad.name,"dbri") && !strstr(ad.name,"CS4231"))
			warning1("Unknown sound system %s. But we try it.",ad.name);
	}
#endif
#endif
	
	if(reset_parameters_sun(ao) < 0) return -1;
	
	AUDIO_INITINFO(&ainfo);
	
	if(ao->flags > 0)
		ainfo.play.port = 0;
	if(ao->flags & OUT123_INTERNAL_SPEAKER)
		ainfo.play.port |= AUDIO_SPEAKER;
	if(ao->flags & OUT123_HEADPHONES)
		ainfo.play.port |= AUDIO_HEADPHONE;
	#ifdef AUDIO_LINE_OUT
	if(ao->flags & OUT123_LINE_OUT)
		ainfo.play.port |= AUDIO_LINE_OUT;
	#endif
	
	if(ao->gain != -1)
		ainfo.play.gain = ao->gain;
	
	if(ioctl(ao->fn, AUDIO_SETINFO, &ainfo) == -1)
		return -1;
	
	return ao->fn;
}




static int get_formats_sun(out123_handle *ao)
{
	static int tab[][3] = {
		{ AUDIO_ENCODING_ULAW , 8,  MPG123_ENC_ULAW_8 } ,
		{ AUDIO_ENCODING_ALAW , 8,  MPG123_ENC_ALAW_8 } ,
		{ AUDIO_ENCODING_LINEAR , 16,  MPG123_ENC_SIGNED_16 } ,
#if 0
#if defined(SOLARIS) || defined(SPARCLINUX)
		{ AUDIO_ENCODING_LINEAR8 , 8,  MPG123_ENC_UNSIGNED_8 } ,
#endif
#endif
	};

	audio_info_t ainfo;
	int i,fmts=0;

	for(i=0;i<sizeof(tab)/sizeof(tab[0]);i++) {
		AUDIO_INITINFO(&ainfo);
		ainfo.play.encoding = tab[i][0];
		ainfo.play.precision = tab[i][1];
#if 1
		ainfo.play.sample_rate = ao->rate;
		ainfo.play.channels = ao->channels;
#endif
		if(ioctl(ao->fn, AUDIO_SETINFO, &ainfo) >= 0) {
			fmts |= tab[i][2];
		}
	}
	return fmts;
}

static int write_sun(out123_handle *ao,unsigned char *buf,int len)
{
	return write(ao->fn,buf,len);
}

static int close_sun(out123_handle *ao)
{
	close (ao->fn);
	return 0;
}

static void flush_sun(out123_handle *ao)
{
	/*ioctl (ao->fn, I_FLUSH, FLUSHRW);*/
}


static int init_sun(out123_handle* ao)
{
	if (ao==NULL) return -1;

	/* Set callbacks */
	ao->open = open_sun;
	ao->flush = flush_sun;
	ao->write = write_sun;
	ao->get_formats = get_formats_sun;
	ao->close = close_sun;

	/* Success */
	return 0;
}





/* 
	Module information data structure
*/
mpg123_module_t mpg123_output_module_info = {
	/* api_version */	MPG123_MODULE_API_VERSION,
	/* name */			"sun",						
	/* description */	"Audio output for Sun Audio.",
	/* revision */		"$Rev: 3915 $",						
	/* handle */		NULL,
	
	/* init_output */	init_sun,						
};



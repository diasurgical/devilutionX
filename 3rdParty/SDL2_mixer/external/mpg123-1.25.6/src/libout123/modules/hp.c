/*
	hp: audio output for HP-UX

	copyright ?-2006 by the mpg123 project - free software under the terms of the LGPL 2.1
	see COPYING and AUTHORS files in distribution or http://mpg123.org
	initially written by Michael Hipp
*/

#include "out123_int.h"
#include <fcntl.h>
#include <sys/audio.h>
#include "debug.h"


static int set_rate(out123_handle *ao)
{
	if(ao->rate >= 0) {
		return ioctl(ao->fn,AUDIO_SET_SAMPLE_RATE,ao->rate);
	} else {
		return 0;
	}
}

static int set_channels(out123_handle *ao)
{
	if(ao->channels<0) return 0;
	return ioctl(ao->fn,AUDIO_SET_CHANNELS,ao->channels);
}

static int set_format(out123_handle *ao)
{
	int fmt;
	
	switch(ao->format) {
		case -1:
		case MPG123_ENC_SIGNED_16:
		default: 
			fmt = MPG123_ENC_LINEAR16BIT;
		break;
		case MPG123_ENC_UNSIGNED_8:
			error("unsigned 8 bit linear not supported");
			return -1;
		case MPG123_ENC_SIGNED_8:
			error("signed 8 bit linear not supported");
			return -1;
		case MPG123_ENC_ALAW_8:
			fmt = MPG123_ENC_ALAW;
		break;
		case MPG123_ENC_ULAW_8:
			fmt = MPG123_ENC_ULAW;
		break;
	}
	return ioctl(ao->fn,AUDIO_SET_DATA_FORMAT,fmt);
}

static int get_formats(out123_handle *ao)
{
	return MPG123_ENC_SIGNED_16;
}

static int reset_parameters(out123_handle *ao)
{
	int ret;
		ret = set_format(ai);
	if(ret >= 0)
		ret = set_channels_hp(ai);
	if(ret >= 0)
		ret = set_rate_hp(ai);
	return ret;
}


static int open_hp(out123_handle *ao)
{
	struct audio_describe ades;
	struct audio_gain again;
	int i,audio;
	
	ao->fn = open("/dev/audio",O_RDWR);
	
	if(ao->fn < 0)
		return -1;
	
	
	ioctl(ao->fn,AUDIO_DESCRIBE,&ades);
	
	if(ao->gain != -1)
	{
		if(ao->gain > ades.max_transmit_gain)
		{
			error("your gainvalue was to high -> set to maximum.");
			ao->gain = ades.max_transmit_gain;
		}
		if(ao->gain < ades.min_transmit_gain)
		{
			error("your gainvalue was to low -> set to minimum.");
			ao->gain = ades.min_transmit_gain;
		}
		again.channel_mask = AUDIO_CHANNEL_0 | AUDIO_CHANNEL_1;
		ioctl(ao->fn,AUDIO_GET_GAINS,&again);
		again.cgain[0].transmit_gain = ao->gain;
		again.cgain[1].transmit_gain = ao->gain;
		again.channel_mask = AUDIO_CHANNEL_0 | AUDIO_CHANNEL_1;
		ioctl(ao->fn,AUDIO_SET_GAINS,&again);
	}
	
	if(ao->flags != -1)
	{
		if(ao->flags & OUT123_INTERNAL_SPEAKER)
			ioctl(ao->fn,AUDIO_SET_OUTPUT,OUT123_SPEAKER);
		else if(ao->flags & OUT123_HEADPHONES)
			ioctl(ao->fn,AUDIO_SET_OUTPUT,OUT123_HEADPHONE);
		else if(ao->flags & OUT123_LINE_OUT)
			ioctl(ao->fn,AUDIO_SET_OUTPUT,OUT123_LINE);
	}
	
	if(ao->rate == -1)
		ao->rate = 44100;
	
	for(i=0;i<ades.nrates;i++)
	{
		if(ao->rate == ades.sample_rate[i])
			break;
	}
	if(i == ades.nrates)
	{
		error1("Can't set sample-rate to %ld.\n",ao->rate);
		i = 0;
	}
	
	if(reset_parameters(ai) < 0)
		return -1;
	
	return ao->fn;
}



static int write_hp(out123_handle *ao,unsigned char *buf,int len)
{
	return write(ao->fn,buf,len);
}

static int close_hp(out123_handle *ao)
{
	close (ao->fn);
	return 0;
}

static void flush_hp(out123_handle *ao)
{
}



static int init_hp(out123_handle* ao)
{
	if (ao==NULL) return -1;

	/* Set callbacks */
	ao->open = open_hp;
	ao->flush = flush_hp;
	ao->write = write_hp;
	ao->get_formats = get_formats_hp;
	ao->close = close_hp;

	/* Success */
	return 0;
}



/* 
	Module information data structure
*/
mpg123_module_t mpg123_output_module_info = {
	/* api_version */	MPG123_MODULE_API_VERSION,
	/* name */			"hp",						
	/* description */	"Output audio HP-UX",
	/* revision */		"$Rev:$",						
	/* handle */		NULL,
	
	/* init_output */	init_hp,						
};

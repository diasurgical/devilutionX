/*
	sgi: audio output on SGI boxen

	copyright ?-2013 by the mpg123 project - free software under the terms of the LGPL 2.1
	see COPYING and AUTHORS files in distribution or http://mpg123.org
	initially written (as it seems) by Thomas Woerner
*/

#include "out123_int.h"
#include <fcntl.h>
#include <dmedia/audio.h>
#include "errno.h"
#include "debug.h"

static int set_rate(out123_handle *ao, ALconfig config)
{
	int dev = alGetDevice(config);
	ALpv params[1];

	/* Make sure the device is OK */
	if(dev < 0)
	{
		error1("set_rate: %s", alGetErrorString(oserror()));
		return -1;
	}
	if(ao->rate > 0)
	{
		params[0].param = AL_RATE;
		params[0].value.ll = alDoubleToFixed((double)ao->rate);
		if(alSetParams(dev, params, 1) < 0)
		{
			error1("set_rate: %s", alGetErrorString(oserror()));
			return -1;
		}
	}
	return 0;
}


static int set_channels(out123_handle *ao, ALconfig config)
{
	int ret;

	if(ao->channels == 2)
	ret = alSetChannels(config, AL_STEREO);
	else
	ret = alSetChannels(config, AL_MONO);

	if(ret < 0)
	{
		error1("set_channels : %s", alGetErrorString(oserror()));
		return -1;
	}
	return 0;
}

static int set_format(out123_handle *ao, ALconfig config)
{
	if(ao->format == MPG123_ENC_FLOAT_32)
	{
		if(alSetSampFmt(config, AL_SAMPFMT_FLOAT) < 0)
		{
			error1("SetSampFmt: %s", alGetErrorString(oserror()));
			return -1;
		}
	} else
	{
		if(alSetSampFmt(config, AL_SAMPFMT_TWOSCOMP) < 0)
		{
			error1("SetSampFmt: %s", alGetErrorString(oserror()));
			return -1;
		}
		if(alSetWidth(config, AL_SAMPLE_16) < 0)
		{
			error1("SetWidth: %s", alGetErrorString(oserror()));
			return -1;
		}
	}
	return 0;
}


static int open_sgi(out123_handle *ao)
{
	int current_dev;
	ALport port = NULL;
	ALconfig config = alNewConfig();

	ao->userptr = NULL;

	/* Test for correct completion */
	if(config == 0)
	{
		error1("open_sgi: %s", alGetErrorString(oserror()));
		return -1;
	}

	/* Setup output device to specified device name. If there is no device name
	specified in ao structure, use the default for output */
	if((ao->device) != NULL)
	{
		current_dev = alGetResourceByName(AL_SYSTEM, ao->device, AL_OUTPUT_DEVICE_TYPE);

		debug2("Dev: %s %i", ao->device, current_dev);

		if(!current_dev)
		{
			int i, numOut;
			char devname[32];
			ALpv pv[1];
			ALvalue *alvalues;

			error2("Invalid audio resource: %s (%s)", ao->device, alGetErrorString(oserror()));

			if((numOut= alQueryValues(AL_SYSTEM,AL_DEFAULT_OUTPUT,0,0,0,0))>=0)
			fprintf(stderr, "There are %d output devices on this system.\n", numOut);
			else
			{
				fprintf(stderr, "Can't find output devices. alQueryValues failed: %s\n", alGetErrorString(oserror()));
				goto open_sgi_bad;
			}

			alvalues = malloc(sizeof(ALvalue) * numOut);
			i = alQueryValues(AL_SYSTEM, AL_DEFAULT_OUTPUT, alvalues, numOut, pv, 0);
			if(i == -1)
			error1("alQueryValues: %s", alGetErrorString(oserror()));
			else
			{
				for(i=0; i < numOut; i++)
				{
					pv[0].param = AL_NAME;
					pv[0].value.ptr = devname;
					pv[0].sizeIn = 32;
					alGetParams(alvalues[i].i, pv, 1);

					fprintf(stderr, "%i: %s\n", i, devname);
				}
			}
			free(alvalues);

			goto open_sgi_bad;
		}

		if(alSetDevice(config, current_dev) < 0)
		{
			error1("open: alSetDevice : %s",alGetErrorString(oserror()));
			goto open_sgi_bad;
		}
	} else
	current_dev = AL_DEFAULT_OUTPUT;

	/* Set the device */
	if(alSetDevice(config, current_dev) < 0)
	{
		error1("open_sgi: %s", alGetErrorString(oserror()));
		goto open_sgi_bad;
	}

	/* Set port parameters */

	if(alSetQueueSize(config, 131069) < 0)
	{
		error1("open_sgi: setting audio buffer failed: %s", alGetErrorString(oserror()));
		goto open_sgi_bad;
	}
	
	if(   set_format(ao, config) < 0
	   || set_rate(ao, config) < 0
	   || set_channels(ao, config) < 0 )
	goto open_sgi_bad;
	
	/* Open the audio port */
	port = alOpenPort("mpg123-VSC", "w", config);
	if(port == NULL)
	{
		error1("Unable to open audio channel: %s", alGetErrorString(oserror()));
		goto open_sgi_bad;
	}

	ao->userptr = (void*)port;

	alFreeConfig(config);
	return 1;

open_sgi_bad:
	/* clean up and return error */
	alFreeConfig(config);
	return -1;
}


static int get_formats_sgi(out123_handle *ao)
{
	return MPG123_ENC_SIGNED_16|MPG123_ENC_FLOAT_32;
}


static int write_sgi(out123_handle *ao, unsigned char *buf, int len)
{
	int length = len;

	if(!ao || !ao->userptr) return -1;

	ALport port = (ALport)ao->userptr;

	if(ao->channels == 2) length >>= 2;
	else length >>= 1;

	if(ao->format == MPG123_ENC_FLOAT_32) length >>=1;

	/* Not much error checking ... */
	alWriteFrames(port, buf, length);

	return len;
}


static int close_sgi(out123_handle *ao)
{
	if(!ao || !ao->userptr) return -1;

	ALport port = (ALport)ao->userptr;

	if(port)
	{
		/* play all remaining samples */
		while(alGetFilled(port) > 0) sginap(1);

		alClosePort(port);
		ao->userptr=NULL;
	}
	return 0;
}

static void flush_sgi(out123_handle *ao)
{
	ALport port = (ALport)ao->userptr;

	if(port) alDiscardFrames(port, alGetFilled(port));
}

static int init_sgi(out123_handle* ao)
{
	if(ao == NULL) return -1;

	/* Set callbacks */
	ao->open = open_sgi;
	ao->flush = flush_sgi;
	ao->write = write_sgi;
	ao->get_formats = get_formats_sgi;
	ao->close = close_sgi;

	/* Success */
	return 0;
}

/*
	Module information data structure
*/
mpg123_module_t mpg123_output_module_info =
{
	/* api_version */	MPG123_MODULE_API_VERSION,
	/* name */			"sgi",
	/* description */	"Audio output for SGI.",
	/* revision */		"$Rev:$",
	/* handle */		NULL,

	/* init_output */	init_sgi,
};

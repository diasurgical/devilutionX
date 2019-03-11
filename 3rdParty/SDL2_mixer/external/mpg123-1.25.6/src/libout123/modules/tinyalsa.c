/*
	tinyalsa: sound output with TINY Advanced Linux Sound Architecture

	copyright 2006-8 by the mpg123 project - free software under the terms of the LGPL 2.1
	see COPYING and AUTHORS files in distribution or http://mpg123.org

	initially written by Jarno Lehtinen <lehtinen@sci.fi>
*/

#include "out123_int.h"
#include <errno.h>

#include <tinyalsa/asoundlib.h>

#include "debug.h"


typedef struct
{
	struct pcm *pcm;
	struct pcm_params *params;
	struct pcm_config config;

	unsigned int device;
	unsigned int card;
} mpg123_tinyalsa_t;


static int initialize_device(out123_handle *ao)
{
	mpg123_tinyalsa_t* ta = (mpg123_tinyalsa_t*)ao->userptr;

	ta->config.channels = ao->channels;
	ta->config.rate = ao->rate;
    	ta->config.period_size = 1024;
    	ta->config.period_count = 4;
       	ta->config.format = PCM_FORMAT_S16_LE;
    	ta->config.start_threshold = 0;
    	ta->config.stop_threshold = 0;
	ta->config.silence_threshold = 0;

	ta->pcm = pcm_open(ta->card, ta->device, PCM_OUT, &ta->config);
	if (!ta->pcm || !pcm_is_ready(ta->pcm))
	{
		if(!AOQUIET)
			error3( "(open) Unable to open card %u PCM device %u (%s)\n"
			,	ta->card, ta->device, pcm_get_error(ta->pcm) );
		return -1;
	}

	return 0;
}


static int open_tinyalsa(out123_handle *ao)
{
        debug("open_tinyalsa()");

	mpg123_tinyalsa_t* ta = (mpg123_tinyalsa_t*)ao->userptr;

        if (ao->format != -1)
	{
                /* we're going to play: initalize sample format */
                return initialize_device(ao);
        }
	else
	{
                /* query mode; sample format will be set for each query */
                return 0;
        }
}


static int get_formats_tinyalsa(out123_handle *ao)
{
        debug("get_formats_tinyalsa()");

	mpg123_tinyalsa_t* ta = (mpg123_tinyalsa_t*)ao->userptr;

	if ( ao->rate >= pcm_params_get_min(ta->params, PCM_PARAM_RATE) && ao->rate <= pcm_params_get_max(ta->params, PCM_PARAM_RATE) && ao->channels >= pcm_params_get_min(ta->params, PCM_PARAM_CHANNELS) && ao->channels <= pcm_params_get_max(ta->params, PCM_PARAM_CHANNELS) ) 
	{
        	return MPG123_ENC_SIGNED_16;
	}
	else
	{
		return 0;
	}
}


static int write_tinyalsa(out123_handle *ao, unsigned char *buf, int bytes)
{
	mpg123_tinyalsa_t* ta = (mpg123_tinyalsa_t*)ao->userptr;

	if (ta->pcm)
	{
		if(pcm_write(ta->pcm, buf, bytes))
		{
			if(!AOQUIET)
				error("Error playing sample\n");
			return -1;
		}
	}
	return bytes;
}


static void flush_tinyalsa(out123_handle *ao)
{
        debug("flush_tinyalsa()");
}


static int close_tinyalsa(out123_handle *ao)
{
        debug("close_tinyalsa()");

	mpg123_tinyalsa_t* ta = (mpg123_tinyalsa_t*)ao->userptr;

	if (ta->pcm)
	{
		pcm_close(ta->pcm);
	}

	return 0;
}


static int deinit_tinyalsa(out123_handle* ao)
{
	mpg123_tinyalsa_t* ta = (mpg123_tinyalsa_t*)ao->userptr;

	/* Free up card/device parameters */
	pcm_params_free(ta->params);

        /* Free up memory */
        if(ao->userptr)
        {
                free( ao->userptr );
                ao->userptr = NULL;
        }

        /* Success */
        return 0;
}


static int init_tinyalsa(out123_handle* ao)
{
	if (ao==NULL) return -1;

	/* Set callbacks */
	ao->open = open_tinyalsa;
	ao->flush = flush_tinyalsa;
	ao->write = write_tinyalsa;
	ao->get_formats = get_formats_tinyalsa;
	ao->close = close_tinyalsa;
	ao->deinit = deinit_tinyalsa;

	/* Allocate memory for data structure */
	ao->userptr = malloc( sizeof( mpg123_tinyalsa_t ) );
	if(ao->userptr==NULL)
	{
		if(!AOQUIET)
			error("failed to malloc memory for 'mpg123_tinyalsa_t'");
		return -1;
	}
	memset( ao->userptr, 0, sizeof(mpg123_tinyalsa_t) );

	/* Set card and device */
	mpg123_tinyalsa_t* ta = (mpg123_tinyalsa_t*)ao->userptr;

	ta->card = 0;
	ta->device = 0;

	if (ao->device)
	{
		char *ptr = ao->device;
		ta->card = (unsigned int)strtol(ptr, &ptr, 10);
		if (strlen(ptr) > 0)
		{
	    		ta->device = (unsigned int)strtol(++ptr, &ptr, 10);
	  	}
	}

	/* Get card/device parameters */
	ta->params = pcm_params_get(ta->card, ta->device, PCM_OUT);
	if (ta->params == NULL)
	{
		if(!AOQUIET)
			error2( "(params) Unable to open card %u PCM device %u.\n"
			,	ta->card, ta->device );
		return -1;
	}

	/* Success */
	return 0;
}


/*
	Module information data structure
*/
mpg123_module_t mpg123_output_module_info = {
	/* api_version */	MPG123_MODULE_API_VERSION,
	/* name */		"tinyalsa",
	/* description */	"Output audio using TINY Advanced Linux Sound Architecture (TINYALSA).",
	/* revision */		"$Rev:$",
	/* handle */		NULL,

	/* init_output */	init_tinyalsa,
};


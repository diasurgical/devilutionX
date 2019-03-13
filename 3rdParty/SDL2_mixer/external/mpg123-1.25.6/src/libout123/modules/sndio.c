/*
 *	sndio: sndio audio output
 *
 * Copyright (c) 2008 Christian Weisgerber <naddy@openbsd.org>,
 *                    Alexandre Ratchov <alex@caoua.org>
 *
 * Permission to use, copy, modify, and distribute this software for any
 * purpose with or without fee is hereby granted, provided that the above
 * copyright notice and this permission notice appear in all copies.
 *
 * THE SOFTWARE IS PROVIDED "AS IS" AND THE AUTHOR DISCLAIMS ALL WARRANTIES
 * WITH REGARD TO THIS SOFTWARE INCLUDING ALL IMPLIED WARRANTIES OF
 * MERCHANTABILITY AND FITNESS. IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR
 * ANY SPECIAL, DIRECT, INDIRECT, OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES
 * WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER IN AN
 * ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT OF
 * OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.
 */

#include "out123_int.h"

#include <sndio.h>

#include "debug.h"

static int open_sndio(out123_handle *ao)
{
	struct sio_hdl *hdl;
	struct sio_par par;

	hdl = sio_open(ao->device /* NULL is fine */, SIO_PLAY, 0);
	if (hdl == NULL)
		return -1;

	sio_initpar(&par);
	par.rate = ao->rate;
	par.pchan = ao->channels;
	par.le = SIO_LE_NATIVE;
	switch(ao->format) {
	case MPG123_ENC_SIGNED_32:
		par.sig = 1;
		par.bits = 32;
		break;
	case MPG123_ENC_UNSIGNED_32:
		par.sig = 0;
		par.bits = 32;
		break;
	case MPG123_ENC_SIGNED_16:
	case -1: /* query mode */
		par.sig = 1;
		par.bits = 16;
		break;
	case MPG123_ENC_UNSIGNED_16:
		par.sig = 0;
		par.bits = 16;
		break;
	case MPG123_ENC_UNSIGNED_8:
		par.sig = 0;
		par.bits = 8;
		break;
	case MPG123_ENC_SIGNED_8:
		par.sig = 1;
		par.bits = 8;
		break;
	default:
		if (!AOQUIET)
			error1("open_sndio: invalid sample format %d",
			    ao->format);
		sio_close(hdl);
		return -1;
	}

	if (!sio_setpar(hdl, &par) || !sio_getpar(hdl, &par) || 
	    !sio_start(hdl)) {
		sio_close(hdl);
		return -1;
	}
	if ((par.bits != 8 && par.bits != 16 && par.bits != 32) ||
	    par.le != SIO_LE_NATIVE) {
		sio_close(hdl);
		return -1;
	}
	ao->rate = par.rate;
	ao->channels = par.pchan;
	switch (par.bits) {
	case 8:
		ao->format = par.sig ? MPG123_ENC_SIGNED_8 :
		    MPG123_ENC_UNSIGNED_8;
		break;
	case 16:
		ao->format = par.sig ? MPG123_ENC_SIGNED_16 :
		    MPG123_ENC_UNSIGNED_16;
		break;
	case 32:
		ao->format = par.sig ? MPG123_ENC_SIGNED_32 :
		    MPG123_ENC_UNSIGNED_32;
		break;
	}
	ao->userptr = hdl;
	return 0;
}

static int get_formats_sndio(out123_handle *ao)
{
	return (MPG123_ENC_SIGNED_32|MPG123_ENC_UNSIGNED_32|
	    MPG123_ENC_SIGNED_16|MPG123_ENC_UNSIGNED_16|
	    MPG123_ENC_UNSIGNED_8|MPG123_ENC_SIGNED_8);
}

static int write_sndio(out123_handle *ao, unsigned char *buf, int len)
{
	struct sio_hdl *hdl = (struct sio_hdl *)ao->userptr;
	int count;

	count = (int)sio_write(hdl, buf, len);
	if (count == 0 && sio_eof(hdl))
		return -1;
	return count;
}

static void flush_sndio(out123_handle *ao)
{
	return;
}

static int close_sndio(out123_handle *ao)
{
	struct sio_hdl *hdl = (struct sio_hdl *)ao->userptr;

	sio_close(hdl);
	return 0;
}

static int init_sndio(out123_handle* ao)
{
	if (ao == NULL)
		return -1;
	
	/* Set callbacks */
	ao->open = open_sndio;
	ao->flush = flush_sndio;	/* required */
	ao->write = write_sndio;
	ao->get_formats = get_formats_sndio;
	ao->close = close_sndio;

	/* Success */
	return 0;
}

/* 
	Module information data structure
*/
mpg123_module_t mpg123_output_module_info = {
	/* api_version */	MPG123_MODULE_API_VERSION,
	/* name */		"sndio",						
	/* description */	"Output audio using sndio library",
	/* revision */		"$Rev:$",						
	/* handle */		NULL,
	
	/* init_output */	init_sndio,						
};

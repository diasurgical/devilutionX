/*
	audio: audio output interface

	copyright ?-2016 by the mpg123 project - free software under the terms of the LGPL 2.1
	see COPYING and AUTHORS files in distribution or http://mpg123.org
	initially written by Michael Hipp
*/

#include "out123_int.h"
#include "wav.h"
#ifndef NOXFERMEM
#include "buffer.h"
static int have_buffer(out123_handle *ao)
{
	return (ao->buffer_pid != -1);
}
#endif
#include "stringlists.h"

#include "debug.h"

/* An output that is live and does not deal with pausing itself.
   The device needs to be closed if we stop feeding. */
#define SENSITIVE_OUTPUT(ao) \
  (    (ao)->propflags & OUT123_PROP_LIVE \
  && !((ao)->propflags & OUT123_PROP_PERSISTENT) )

static const char *default_name = "out123";

static int modverbose(out123_handle *ao, int final)
{
	debug3("modverbose: %x %x %x"
	,	(unsigned)ao->flags, (unsigned)ao->auxflags, (unsigned)OUT123_QUIET);
	return AOQUIET
	?	(final ? 0 : -1)
	:	ao->verbose;
}

static void check_output_module( out123_handle *ao
,	const char *name, const char *device, int final );

static void out123_clear_module(out123_handle *ao)
{
	ao->open = NULL;
	ao->get_formats = NULL;
	ao->write = NULL;
	ao->flush = NULL;
	ao->drain = NULL;
	ao->close = NULL;
	ao->deinit = NULL;

	ao->module = NULL;
	ao->userptr = NULL;
	ao->fn = -1;
	/* The default is live output devices, files are the special case. */
	ao->propflags = OUT123_PROP_LIVE;
}

/* Ensure that real name is not leaked, needs to be freed before any call to
   ao->open(ao). One might free it on closing already, but it might be sensible
   to keep it around, might still be the same after re-opening. */
static int aoopen(out123_handle *ao)
{
	if(ao->realname)
	{
		free(ao->realname);
		ao->realname = NULL;
	}
	return ao->open(ao);
}

out123_handle* attribute_align_arg out123_new(void)
{
	out123_handle* ao = malloc( sizeof( out123_handle ) );
	if(!ao)
		return NULL;
	ao->errcode = 0;
#ifndef NOXFERMEM
	ao->buffer_pid = -1;
	ao->buffer_fd[0] = -1;
	ao->buffer_fd[1] = -1;
	ao->buffermem = NULL;
#endif

	out123_clear_module(ao);
	ao->name = compat_strdup(default_name);
	ao->realname = NULL;
	ao->driver = NULL;
	ao->device = NULL;

	ao->flags = OUT123_KEEP_PLAYING;
	ao->rate = -1;
	ao->gain = -1;
	ao->channels = -1;
	ao->format = -1;
	ao->framesize = 0;
	ao->state = play_dead;
	ao->auxflags = 0;
	ao->preload = 0.;
	ao->verbose = 0;
	ao->device_buffer = 0.;
	ao->bindir = NULL;
	return ao;
}

void attribute_align_arg out123_del(out123_handle *ao)
{
	debug2("[%ld]out123_del(%p)", (long)getpid(), (void*)ao);
	if(!ao) return;

	out123_close(ao); /* TODO: That talks to the buffer if present. */
	out123_set_buffer(ao, 0);
#ifndef NOXFERMEM
	if(have_buffer(ao)) buffer_exit(ao);
#endif
	if(ao->name)
		free(ao->name);
	if(ao->bindir)
		free(ao->bindir);
	free(ao);
}

/* Error reporting */

/* Carefully keep that in sync with the error enum! */
static const char *const errstring[OUT123_ERRCOUNT] =
{
	"no problem"
,	"out of memory"
,	"bad driver name"
,	"failure loading driver module"
,	"no driver loaded"
,	"no active audio device"
,	"some device playback error"
,	"failed to open device"
,	"buffer (communication) error"
,	"basic module system error"
,	"unknown parameter code"
,	"attempt to set read-only parameter"
,	"invalid out123 handle"
};

const char* attribute_align_arg out123_strerror(out123_handle *ao)
{
	return out123_plain_strerror(out123_errcode(ao));
}

int out123_errcode(out123_handle *ao)
{
	if(!ao) return OUT123_BAD_HANDLE;
	else    return ao->errcode;
}

const char* attribute_align_arg out123_plain_strerror(int errcode)
{
	if(errcode >= OUT123_ERRCOUNT || errcode < 0)
		return "invalid error code";

	/* Let's be paranoid, one _may_ forget to extend errstrings when
	   adding a new entry to the enum. */
	if(errcode >= sizeof(errstring)/sizeof(char*))
		return "outdated error list (library bug)";

	return errstring[errcode];
}

static int out123_seterr(out123_handle *ao, enum out123_error errcode)
{
	if(!ao)
		return OUT123_ERR;
	ao->errcode = errcode;
	return errcode == OUT123_OK ? OUT123_OK : OUT123_ERR;
}

/* pre-playback setup */

int attribute_align_arg
out123_set_buffer(out123_handle *ao, size_t buffer_bytes)
{
	debug2("out123_set_buffer(%p, %"SIZE_P")", (void*)ao, (size_p)buffer_bytes);
	if(!ao)
		return OUT123_ERR;
	ao->errcode = 0;
	/* Close any audio output module if present, also kill of buffer if present,
	   then start new buffer process with newly allocated storage if given
	   size is non-zero. */
	out123_close(ao);
#ifndef NOXFERMEM
	if(have_buffer(ao))
		buffer_exit(ao);
	if(buffer_bytes)
		return buffer_init(ao, buffer_bytes);
#endif
	return 0;
}

int attribute_align_arg
out123_param( out123_handle *ao, enum out123_parms code
            , long value, double fvalue, const char *svalue )
{
	int ret = 0;

	debug4("out123_param(%p, %i, %li, %g)", (void*)ao, (int)code, value, fvalue);
	if(!ao)
		return OUT123_ERR;
	ao->errcode = 0;

	switch(code)
	{
		case OUT123_FLAGS:
			ao->flags = (int)value;
		break;
		case OUT123_PRELOAD:
			ao->preload = fvalue;
		break;
		case OUT123_GAIN:
			ao->gain = value;
		break;
		case OUT123_VERBOSE:
			ao->verbose = (int)value;
		break;
		case OUT123_DEVICEBUFFER:
			ao->device_buffer = fvalue;
		break;
		case OUT123_PROPFLAGS:
			ao->errcode = OUT123_SET_RO_PARAM;
			ret = OUT123_ERR;
		break;
		case OUT123_NAME:
			if(ao->name)
				free(ao->name);
			ao->name = compat_strdup(svalue ? svalue : default_name);
		break;
		case OUT123_BINDIR:
			if(ao->bindir)
				free(ao->bindir);
			ao->bindir = compat_strdup(svalue);
		break;
		default:
			ao->errcode = OUT123_BAD_PARAM;
			if(!AOQUIET) error1("bad parameter code %i", (int)code);
			ret = OUT123_ERR;
	}
#ifndef NOXFERMEM
	/* If there is a buffer, it needs to update its copy of parameters. */
	if(have_buffer(ao))
		/* No error check; if that fails, buffer is dead and we will notice
		   soon enough. */
		buffer_sync_param(ao);
#endif
	return ret;
}

int attribute_align_arg
out123_getparam( out123_handle *ao, enum out123_parms code
               , long *ret_value, double *ret_fvalue, char* *ret_svalue )
{
	int ret = 0;
	long value = 0;
	double fvalue = 0.;
	char *svalue = NULL;

	debug4( "out123_getparam(%p, %i, %p, %p)"
	,	(void*)ao, (int)code, (void*)ret_value, (void*)ret_fvalue );
	if(!ao)
		return OUT123_ERR;
	ao->errcode = 0;

	switch(code)
	{
		case OUT123_FLAGS:
			value = ao->flags;
		break;
		case OUT123_PRELOAD:
			fvalue = ao->preload;
		break;
		case OUT123_GAIN:
			value = ao->gain;
		break;
		case OUT123_VERBOSE:
			value = ao->verbose;
		break;
		case OUT123_DEVICEBUFFER:
			fvalue = ao->device_buffer;
		break;
		case OUT123_PROPFLAGS:
			value = ao->propflags;
		break;
		case OUT123_NAME:
			svalue = ao->realname ? ao->realname : ao->name;
		break;
		case OUT123_BINDIR:
			svalue = ao->bindir;
		break;
		default:
			if(!AOQUIET) error1("bad parameter code %i", (int)code);
			ao->errcode = OUT123_BAD_PARAM;
			ret = OUT123_ERR;
	}
	if(!ret)
	{
		if(ret_value)  *ret_value  = value;
		if(ret_fvalue) *ret_fvalue = fvalue;
		if(ret_svalue) *ret_svalue = svalue;
	}
	return ret;
}

int attribute_align_arg
out123_param_from(out123_handle *ao, out123_handle* from_ao)
{
	debug2("out123_param_from(%p, %p)", (void*)ao, (void*)from_ao);
	if(!ao || !from_ao) return -1;

	ao->flags     = from_ao->flags;
	ao->preload   = from_ao->preload;
	ao->gain      = from_ao->gain;
	ao->device_buffer = from_ao->device_buffer;
	ao->verbose   = from_ao->verbose;
	if(ao->name)
		free(ao->name);
	ao->name = compat_strdup(from_ao->name);
	if(ao->bindir)
		free(ao->bindir);
	ao->bindir = compat_strdup(from_ao->bindir);

	return 0;
}

#ifndef NOXFERMEM
/* Serialization of tunable parameters to communicate them between
   main process and buffer. Make sure these two stay in sync ... */

int write_parameters(out123_handle *ao, int who)
{
	int fd = ao->buffermem->fd[who];
	if(
		GOOD_WRITEVAL(fd, ao->flags)
	&&	GOOD_WRITEVAL(fd, ao->preload)
	&&	GOOD_WRITEVAL(fd, ao->gain)
	&&	GOOD_WRITEVAL(fd, ao->device_buffer)
	&&	GOOD_WRITEVAL(fd, ao->verbose)
	&&	GOOD_WRITEVAL(fd, ao->propflags)
	&& !xfer_write_string(ao, who, ao->name)
	&& !xfer_write_string(ao, who, ao->bindir)
	)
		return 0;
	else
		return -1;
}

int read_parameters(out123_handle *ao
,	int who, byte *prebuf, int *preoff, int presize)
{
	int fd = ao->buffermem->fd[who];
#define GOOD_READVAL_BUF(fd, val) \
	!read_buf(fd, &val, sizeof(val), prebuf, preoff, presize)
	if(
		GOOD_READVAL_BUF(fd, ao->flags)
	&&	GOOD_READVAL_BUF(fd, ao->preload)
	&&	GOOD_READVAL_BUF(fd, ao->gain)
	&&	GOOD_READVAL_BUF(fd, ao->device_buffer)
	&&	GOOD_READVAL_BUF(fd, ao->verbose)
	&&	GOOD_READVAL_BUF(fd, ao->propflags)
	&& !xfer_read_string(ao, who, &ao->name)
	&& !xfer_read_string(ao, who, &ao->bindir)
	)
		return 0;
	else
		return -1;
#undef GOOD_READVAL_BUF
}
#endif

int attribute_align_arg
out123_open(out123_handle *ao, const char* driver, const char* device)
{
	debug4( "[%ld]out123_open(%p, %s, %s)", (long)getpid(), (void*)ao
	,	driver ? driver : "<nil>", device ? device : "<nil>" );
	if(!ao)
		return OUT123_ERR;
	ao->errcode = 0;

	out123_close(ao);
	debug("out123_open() continuing");

	/* Ensure that audio format is freshly set for "no format yet" mode.
	   In out123_start*/
	ao->rate = -1;
	ao->channels = -1;
	ao->format = -1;

#ifndef NOXFERMEM
	if(have_buffer(ao))
	{
		if(buffer_open(ao, driver, device))
			return OUT123_ERR;
	}
	else
#endif
	{
		/* We just quickly check if the device can be accessed at all,
		   same as out123_encodings! */
		char *nextname, *modnames;
		const char *names = driver ? driver : DEFAULT_OUTPUT_MODULE;

		if(!names) return out123_seterr(ao, OUT123_BAD_DRIVER_NAME);

		/* It is ridiculous how these error messages are larger than the pieces
		   of memory they are about! */
		if(device && !(ao->device = compat_strdup(device)))
		{
			if(!AOQUIET) error("OOM device name copy");
			return out123_seterr(ao, OUT123_DOOM);
		}

		if(!(modnames = compat_strdup(names)))
		{
			out123_close(ao); /* Frees ao->device, too. */
			if(!AOQUIET) error("OOM driver names");
			return out123_seterr(ao, OUT123_DOOM);
		}

		/* Now loop over the list of possible modules to find one that works. */
		nextname = strtok(modnames, ",");
		while(!ao->open && nextname)
		{
			char *curname = nextname;
			nextname = strtok(NULL, ",");
			check_output_module(ao, curname, device, !nextname);
			if(ao->open)
			{
				if(AOVERBOSE(2))
					fprintf(stderr, "Chosen output module: %s\n", curname);
				/* A bit redundant, but useful when it's a fake module. */
				if(!(ao->driver = compat_strdup(curname)))
				{
					out123_close(ao);
					if(!AOQUIET) error("OOM driver name");
					return out123_seterr(ao, OUT123_DOOM);
				}
			}
		}

		free(modnames);

		if(!ao->open) /* At least an open() routine must be present. */
		{
			if(!AOQUIET)
				error2("Found no driver out of [%s] working with device %s."
				,	names, device ? device : "<default>");
			/* Proper more detailed error code could be set already. */
			if(ao->errcode == OUT123_OK)
				ao->errcode = OUT123_BAD_DRIVER;
			return OUT123_ERR;
		}
	}
	/* Got something. */
	ao->state = play_stopped;
	return OUT123_OK;
}

/* Be resilient, always do cleanup work regardless of state. */
void attribute_align_arg out123_close(out123_handle *ao)
{
	debug2("[%ld]out123_close(%p)", (long)getpid(), (void*)ao);
	if(!ao)
		return;
	ao->errcode = 0;

	out123_drain(ao);
	out123_stop(ao);

#ifndef NOXFERMEM
	if(have_buffer(ao))
		buffer_close(ao);
	else
#endif
	{
		if(ao->deinit)
			ao->deinit(ao);
		if(ao->module)
			close_module(ao->module, modverbose(ao, 0));
		/* Null module methods and pointer. */
		out123_clear_module(ao);
	}

	/* These copies exist in addition to the ones for the buffer. */
	if(ao->driver)
		free(ao->driver);
	ao->driver = NULL;
	if(ao->device)	
		free(ao->device);
	ao->device = NULL;
	if(ao->realname)
		free(ao->realname);
	ao->realname = NULL;

	ao->state = play_dead;
}

int attribute_align_arg 
out123_start(out123_handle *ao, long rate, int channels, int encoding)
{
	debug5( "[%ld]out123_start(%p, %li, %i, %i)", (long)getpid()
	,	(void*)ao, rate, channels, encoding );
	if(!ao)
		return OUT123_ERR;
	ao->errcode = 0;

	out123_stop(ao);
	debug("out123_start() continuing");
	if(ao->state != play_stopped)
		return out123_seterr(ao, OUT123_NO_DRIVER);

	/* Stored right away as parameters for ao->open() and also for reference.
	   framesize needed for out123_play(). */
	ao->rate      = rate;
	ao->channels  = channels;
	ao->format    = encoding;
	ao->framesize = out123_encsize(encoding)*channels;

#ifndef NOXFERMEM
	if(have_buffer(ao))
	{
		if(!buffer_start(ao))
		{
			ao->state = play_live;
			return OUT123_OK;
		}
		else
			return OUT123_ERR;
	}
	else
#endif
	{
		if(aoopen(ao) < 0)
			return out123_seterr(ao, OUT123_DEV_OPEN);
		ao->state = play_live;
		return OUT123_OK;
	}
}

void attribute_align_arg out123_pause(out123_handle *ao)
{
	debug3( "[%ld]out123_pause(%p) %i", (long)getpid()
	,	(void*)ao, ao ? (int)ao->state : -1 );
	if(ao && ao->state == play_live)
	{
#ifndef NOXFERMEM
		if(have_buffer(ao)){ debug("pause with buffer"); buffer_pause(ao); }
		else
#endif
		{
debug1("pause without buffer, sensitive=%d", SENSITIVE_OUTPUT(ao));
			/* Close live devices to avoid underruns. */
			if( SENSITIVE_OUTPUT(ao)
			  && ao->close && ao->close(ao) && !AOQUIET )
				error("trouble closing device");
		}
		ao->state = play_paused;
	}
}

void attribute_align_arg out123_continue(out123_handle *ao)
{
	debug3( "[%ld]out123_continue(%p) %i", (long)getpid()
	,	(void*)ao, ao ? (int)ao->state : -1 );
	if(ao && ao->state == play_paused)
	{
#ifndef NOXFERMEM
		if(have_buffer(ao)) buffer_continue(ao);
		else
#endif
		/* Re-open live devices to avoid underruns. */
		if(SENSITIVE_OUTPUT(ao) && aoopen(ao) < 0)
		{
			/* Will be overwritten by following out123_play() ... */
			ao->errcode = OUT123_DEV_OPEN;
			if(!AOQUIET)
				error("failed re-opening of device after pause");
			return;
		}
		ao->state = play_live;
	}
}

void attribute_align_arg out123_stop(out123_handle *ao)
{
	debug2("[%ld]out123_stop(%p)", (long)getpid(), (void*)ao);
	if(!ao)
		return;
	ao->errcode = 0;
	if(!(ao->state == play_paused || ao->state == play_live))
		return;
#ifndef NOXFERMEM
	if(have_buffer(ao))
		buffer_stop(ao);
	else
#endif
	if(   ao->state == play_live
	  || (ao->state == play_paused && !SENSITIVE_OUTPUT(ao)) )
	{
		if(ao->close && ao->close(ao) && !AOQUIET)
			error("trouble closing device");
	}
	ao->state = play_stopped;
}

size_t attribute_align_arg
out123_play(out123_handle *ao, void *bytes, size_t count)
{
	size_t sum = 0;
	int written;

	debug5( "[%ld]out123_play(%p, %p, %"SIZE_P") (%i)", (long)getpid()
	,	(void*)ao, bytes, (size_p)count, ao ? (int)ao->state : -1 );
	if(!ao)
		return 0;
	ao->errcode = 0;
	/* If paused, automatically continue. Other states are an error. */
	if(ao->state != play_live)
	{
		if(ao->state == play_paused)
			out123_continue(ao);
		if(ao->state != play_live)
		{
			ao->errcode = OUT123_NOT_LIVE;
			return 0;
		}
	}

	/* Ensure that we are writing whole PCM frames. */
	count -= count % ao->framesize;
	if(!count) return 0;

#ifndef NOXFERMEM
	if(have_buffer(ao))
		return buffer_write(ao, bytes, count);
	else
#endif
	do /* Playback in a loop to be able to continue after interruptions. */
	{
		errno = 0;
		written = ao->write(ao, (unsigned char*)bytes, (int)count);
		debug4( "written: %d errno: %i (%s), keep_on=%d"
		,	written, errno, strerror(errno)
		,	ao->flags & OUT123_KEEP_PLAYING );
		if(written >= 0){ sum+=written; count -= written; }
		else if(errno != EINTR)
		{
			ao->errcode = OUT123_DEV_PLAY;
			if(!AOQUIET)
				error1("Error in writing audio (%s?)!", strerror(errno));
			/* If written < 0, this is a serious issue ending this playback round. */
			break;
		}
	} while(count && ao->flags & OUT123_KEEP_PLAYING);

	debug3( "out123_play(%p, %p, ...) = %"SIZE_P
	,	(void*)ao, bytes, (size_p)sum );
	return sum;
}

/* Drop means to flush it down. Quickly. */
void attribute_align_arg out123_drop(out123_handle *ao)
{
	debug2("[%ld]out123_drop(%p)", (long)getpid(), (void*)ao);
	if(!ao)
		return;
	ao->errcode = 0;
#ifndef NOXFERMEM
	if(have_buffer(ao))
		buffer_drop(ao);
	else
#endif
	if(ao->state == play_live)
	{
		if(ao->propflags & OUT123_PROP_LIVE && ao->flush)
			ao->flush(ao);
	}
}

void attribute_align_arg out123_drain(out123_handle *ao)
{
	debug2("[%ld]out123_drain(%p) ", (long)getpid(), (void*)ao);
	if(!ao)
		return;
	ao->errcode = 0;
	/* If paused, automatically continue. */
	if(ao->state != play_live)
	{
		if(ao->state == play_paused)
			out123_continue(ao);
		if(ao->state != play_live)
			return;
	}
#ifndef NOXFERMEM
	if(have_buffer(ao))
		buffer_drain(ao);
	else
#endif
	{
		if(ao->drain)
			ao->drain(ao);
		out123_pause(ao);
	}
}

void attribute_align_arg out123_ndrain(out123_handle *ao, size_t bytes)
{
	debug3("[%ld]out123_ndrain(%p, %"SIZE_P")", (long)getpid(), (void*)ao, (size_p)bytes);
	if(!ao)
		return;
	ao->errcode = 0;
	/* If paused, automatically continue. */
	if(ao->state != play_live)
	{
		if(ao->state == play_paused)
			out123_continue(ao);
		if(ao->state != play_live)
			return;
	}
#ifndef NOXFERMEM
	if(have_buffer(ao))
		buffer_ndrain(ao, bytes);
	else
#endif
	{
		if(ao->drain)
			ao->drain(ao);
		out123_pause(ao);
	}
}


/* A function that does nothing and returns nothing. */
static void builtin_nothing(out123_handle *ao){}
static int test_open(out123_handle *ao)
{
	debug("test_open");
	return OUT123_OK;
}
static int test_get_formats(out123_handle *ao)
{
	debug("test_get_formats");
	return MPG123_ENC_ANY;
}
static int test_write(out123_handle *ao, unsigned char *buf, int len)
{
	debug2("test_write: %i B from %p", len, (void*)buf);
	return len;
}
static void test_flush(out123_handle *ao)
{
	debug("test_flush");
}
static void test_drain(out123_handle *ao)
{
	debug("test_drain");
}
static int test_close(out123_handle *ao)
{
	debug("test_drain");
	return 0;
}

/* Open one of our builtin driver modules. */
static int open_fake_module(out123_handle *ao, const char *driver)
{
	if(!strcmp("test", driver))
	{
		ao->propflags &= ~OUT123_PROP_LIVE;
		ao->open  = test_open;
		ao->get_formats = test_get_formats;
		ao->write = test_write;
		ao->flush = test_flush;
		ao->drain = test_drain;
		ao->close = test_close;
	}
	else
	if(!strcmp("raw", driver))
	{
		ao->propflags &= ~OUT123_PROP_LIVE;
		ao->open  = raw_open;
		ao->get_formats = raw_formats;
		ao->write = wav_write;
		ao->flush = builtin_nothing;
		ao->drain = wav_drain;
		ao->close = raw_close;
	}
	else
	if(!strcmp("wav", driver))
	{
		ao->propflags &= ~OUT123_PROP_LIVE;
		ao->open = wav_open;
		ao->get_formats = wav_formats;
		ao->write = wav_write;
		ao->flush = builtin_nothing;
		ao->drain = wav_drain;
		ao->close = wav_close;
	}
	else
	if(!strcmp("cdr", driver))
	{
		ao->propflags &= ~OUT123_PROP_LIVE;
		ao->open  = cdr_open;
		ao->get_formats = cdr_formats;
		ao->write = wav_write;
		ao->flush = builtin_nothing;
		ao->drain = wav_drain;
		ao->close = raw_close;
	}
	else
	if(!strcmp("au", driver))
	{
		ao->propflags &= ~OUT123_PROP_LIVE;
		ao->open  = au_open;
		ao->get_formats = au_formats;
		ao->write = wav_write;
		ao->flush = builtin_nothing;
		ao->drain = wav_drain;
		ao->close = au_close;
	}
	else return OUT123_ERR;

	return OUT123_OK;
}

/* Check if given output module is loadable and has a working device.
   final flag triggers printing and storing of errors. */
static void check_output_module( out123_handle *ao
,	const char *name, const char *device, int final )
{
	int result;

	debug3("check_output_module %p %p %p", (void*)ao, (void*)device, (void*)ao->device);
	if(AOVERBOSE(1))
		fprintf( stderr, "Trying output module: %s, device: %s\n"
		,	name, ao->device ? ao->device : "<nil>" );

	/* Use internal code. */
	if(open_fake_module(ao, name) == OUT123_OK)
		return;

	/* Open the module, initial check for availability+libraries. */
	ao->module = open_module( "output", name, modverbose(ao, final), ao->bindir);
	if(!ao->module)
		return;
	/* Check if module supports output */
	if(!ao->module->init_output)
	{
		if(final)
			error1("Module '%s' does not support audio output.", name);
		goto check_output_module_cleanup;
	}

	/* Should I do funny stuff with stderr file descriptor instead? */
	if(final)
	{
		if(AOVERBOSE(2))
			fprintf(stderr
			,	"Note: %s is the last output option... showing you any error messages now.\n"
			,	name);
	}
	else ao->auxflags |= OUT123_QUIET; /* Probing, so don't spill stderr with errors. */
	result = ao->module->init_output(ao);
	if(result == 0)
	{ /* Try to open the device. I'm only interested in actually working modules. */
		ao->format = -1;
		result = aoopen(ao);
		debug1("ao->open() = %i", result);
		if(result >= 0) /* Opening worked, close again. */
			ao->close(ao);
		else if(ao->deinit)
			ao->deinit(ao); /* Failed, ensure that cleanup after init_output() occurs. */
	}
	else if(!AOQUIET)
		error2("Module '%s' init failed: %i", name, result);

	ao->auxflags &= ~OUT123_QUIET;

	if(result >= 0)
		return;

check_output_module_cleanup:
	/* Only if module did not check out we get to clean up here. */
	close_module(ao->module, modverbose(ao, final));
	out123_clear_module(ao);
	return;
}

/*
static void audio_output_dump(out123_handle *ao)
{
	fprintf(stderr, "ao->fn=%d\n", ao->fn);
	fprintf(stderr, "ao->userptr=%p\n", ao->userptr);
	fprintf(stderr, "ao->rate=%ld\n", ao->rate);
	fprintf(stderr, "ao->gain=%ld\n", ao->gain);
	fprintf(stderr, "ao->device='%s'\n", ao->device);
	fprintf(stderr, "ao->channels=%d\n", ao->channels);
	fprintf(stderr, "ao->format=%d\n", ao->format);
}
*/

int attribute_align_arg
out123_drivers(out123_handle *ao, char ***names, char ***descr)
{
	char **tmpnames;
	char **tmpdescr;
	int count;
	int i;

	if(!ao)
		return -1;

	debug3("out123_drivers(%p, %p, %p)", (void*)ao, (void*)names, (void*)descr);
	/* Wrap the call to isolate the lower levels from the user not being
	   interested in both lists. it's a bit wasteful, but the code looks
	   ugly enough already down there. */
	count = list_modules("output", &tmpnames, &tmpdescr, modverbose(ao, 0), ao->bindir);
	debug1("list_modules()=%i", count);
	if(count < 0)
	{
		if(!AOQUIET)
			error("Dynamic module search failed.");
		count = 0;
	}

	if(
		stringlists_add( &tmpnames, &tmpdescr
		,	"raw", "raw headerless stream (builtin)", &count )
	||	stringlists_add( &tmpnames, &tmpdescr
		,	"cdr", "compact disc digital audio stream (builtin)", &count )
	||	stringlists_add( &tmpnames, &tmpdescr
		,	"wav", "RIFF WAVE file (builtin)", &count )
	||	stringlists_add( &tmpnames, &tmpdescr
		,	"au", "Sun AU file (builtin)", &count )
	||	stringlists_add( &tmpnames, &tmpdescr
		,	"test", "output into the void (builtin)", &count )
	)
		if(!AOQUIET)
			error("OOM");

	/* Return or free gathered lists of names or descriptions. */
	if(names)
		*names = tmpnames;
	else
	{
		for(i=0; i<count; ++i)
			free(tmpnames[i]);
		free(tmpnames);
	}
	if(descr)
		*descr = tmpdescr;
	else
	{
		for(i=0; i<count; ++i)
			free(tmpdescr[i]);
		free(tmpdescr);
	}
	return count;
}

/* We always have ao->driver and ao->device set, also with buffer.
   The latter can be positively NULL, though. */
int attribute_align_arg
out123_driver_info(out123_handle *ao, char **driver, char **device)
{
	debug3( "out123_driver_info(%p, %p, %p)"
	,	(void*)ao, (void*)driver, (void*)device );
	if(!ao)
		return OUT123_ERR;
	if(!ao->driver)
		return out123_seterr(ao, OUT123_NO_DRIVER);

	if(driver)
		*driver = ao->driver;
	if(device)
		*device = ao->device;
	return OUT123_OK;
}

int attribute_align_arg
out123_encodings(out123_handle *ao, long rate, int channels)
{
	debug4( "[%ld]out123_encodings(%p, %li, %i)", (long)getpid()
	,	(void*)ao, rate, channels );
	if(!ao)
		return OUT123_ERR;
	ao->errcode = OUT123_OK;

	out123_stop(ao); /* That brings the buffer into waiting state, too. */

	if(ao->state != play_stopped)
		return out123_seterr(ao, OUT123_NO_DRIVER);

	ao->channels = channels;
	ao->rate     = rate;
#ifndef NOXFERMEM
	if(have_buffer(ao))
		return buffer_encodings(ao);
	else
#endif
	{
		int enc = 0;
		/* This tells outputs to choose a fitting format so that ao->open() succeeds
		   They possibly set a sample rate and channel count they like best.
		   We should add API to retrieve those defaults, too. */
		ao->format   = -1;
		if(aoopen(ao) >= 0)
		{
			/* Need to reset those since the choose-your-format open
			   call might have changed them. */
			ao->channels = channels;
			ao->rate     = rate;
			enc = ao->get_formats(ao);
			ao->close(ao);
			return enc;
		}
		else
			return out123_seterr(ao, (ao->errcode != OUT123_OK
			?	ao->errcode
			:	OUT123_DEV_OPEN));
	}
}

int attribute_align_arg out123_encsize(int encoding)
{
	return MPG123_SAMPLESIZE(encoding);
}

int attribute_align_arg
out123_formats( out123_handle *ao, const long *rates, int ratecount
              , int minchannels, int maxchannels
              , struct mpg123_fmt **fmtlist )
{
	debug7( "[%ld]out123_formats(%p, %p, %i, %i, %i, %p)", (long)getpid()
	,	(void*)ao, (void*)rates, ratecount, minchannels, maxchannels
	,	(void*)fmtlist );
	if(!ao)
		return OUT123_ERR;
	ao->errcode = OUT123_OK;

	out123_stop(ao); /* That brings the buffer into waiting state, too. */

	if(ao->state != play_stopped)
		return out123_seterr(ao, OUT123_NO_DRIVER);

	if(ratecount > 0 && !rates)
		return out123_seterr(ao, OUT123_ARG_ERROR);
	if(!fmtlist || minchannels > maxchannels)
		return out123_seterr(ao, OUT123_ARG_ERROR);
	*fmtlist = NULL; /* Initialize so free(fmtlist) is always allowed. */

#ifndef NOXFERMEM
	if(have_buffer(ao))
		return buffer_formats( ao, rates, ratecount
		                     , minchannels, maxchannels, fmtlist );
	else
#endif
	{
		/* This tells outputs to choose a fitting format so that ao->open()
		   succeeds. */
		ao->format   = -1;
		ao->rate     = -1;
		ao->channels = -1;
		if(aoopen(ao) >= 0)
		{
			struct mpg123_fmt *fmts;
			int ri, ch;
			int fi = 0;
			int fmtcount = 1; /* Always the default format. */
			if(ratecount > 0)
				fmtcount += ratecount*(maxchannels-minchannels+1);
			if(!(fmts = malloc(sizeof(*fmts)*fmtcount)))
			{
				ao->close(ao);
				return out123_seterr(ao, OUT123_DOOM);
			}
			/* Store default format if present. */
			if(ao->format > 0 && ao->channels > 0 && ao->rate > 0)
			{
				fmts[0].rate     = ao->rate;
				fmts[0].channels = ao->channels;
				fmts[0].encoding = ao->format;
			}
			else
			{ /* Ensure consistent -1 in all entries. */
				fmts[0].rate     = -1;
				fmts[0].channels = -1;
				fmts[0].encoding = -1;
			}
			/* Test all combinations of rate and channel count. */
			for(ri=0; ri<ratecount; ++ri)
			for(ch=minchannels; ch<=maxchannels; ++ch)
			{
				++fi;
				ao->rate = rates[ri];
				ao->channels = ch;
				fmts[fi].rate     = ao->rate;
				fmts[fi].channels = ao->channels;
				fmts[fi].encoding = ao->get_formats(ao);
			}
			ao->close(ao);

			*fmtlist = fmts;
			return fmtcount;
		}
		else
			return out123_seterr(ao, (ao->errcode != OUT123_OK
			?	ao->errcode
			:	OUT123_DEV_OPEN));
	}
}


size_t attribute_align_arg out123_buffered(out123_handle *ao)
{
	debug2("[%ld]out123_buffered(%p)", (long)getpid(), (void*)ao);
	if(!ao)
		return 0;
	ao->errcode = 0;
#ifndef NOXFERMEM
	if(have_buffer(ao))
	{
		size_t fill = buffer_fill(ao);
		debug2("out123_buffered(%p) = %"SIZE_P, (void*)ao, (size_p)fill);
		return fill;
	}
	else
#endif
		return 0;
}

int attribute_align_arg out123_getformat( out123_handle *ao
,	long *rate, int *channels, int *encoding, int *framesize )
{
	if(!ao)
		return OUT123_ERR;

	if(!(ao->state == play_paused || ao->state == play_live))
		return out123_seterr(ao, OUT123_NOT_LIVE);

	if(rate)
		*rate = ao->rate;
	if(channels)
		*channels = ao->channels;
	if(encoding)
		*encoding = ao->format;
	if(framesize)
		*framesize = ao->framesize;
	return OUT123_OK;
}

struct enc_desc
{
	int code; /* MPG123_ENC_SOMETHING */
	const char *longname; /* signed bla bla */
	const char *name; /* sXX, short name */
};

static const struct enc_desc encdesc[] =
{
	{ MPG123_ENC_SIGNED_16,   "signed 16 bit",   "s16"  }
,	{ MPG123_ENC_UNSIGNED_16, "unsigned 16 bit", "u16"  }
,	{ MPG123_ENC_SIGNED_32,   "signed 32 bit",   "s32"  }
,	{ MPG123_ENC_UNSIGNED_32, "unsigned 32 bit", "u32"  }
,	{ MPG123_ENC_SIGNED_24,   "signed 24 bit",   "s24"  }
,	{ MPG123_ENC_UNSIGNED_24, "unsigned 24 bit", "u24"  }
,	{ MPG123_ENC_FLOAT_32,    "float (32 bit)",  "f32"  }
,	{ MPG123_ENC_FLOAT_64,    "float (64 bit)",  "f64"  }
,	{ MPG123_ENC_SIGNED_8,    "signed 8 bit",    "s8"   }
,	{ MPG123_ENC_UNSIGNED_8,  "unsigned 8 bit",  "u8"   }
,	{ MPG123_ENC_ULAW_8,      "mu-law (8 bit)",  "ulaw" }
,	{ MPG123_ENC_ALAW_8,      "a-law (8 bit)",   "alaw" }
};
#define KNOWN_ENCS (sizeof(encdesc)/sizeof(struct enc_desc))

int attribute_align_arg out123_enc_list(int **enclist)
{
	int i;
	if(!enclist)
		return OUT123_ERR;
	*enclist = malloc(sizeof(int)*KNOWN_ENCS);
	if(!(*enclist))
		return OUT123_ERR;
	for(i=0;i<KNOWN_ENCS;++i)
		(*enclist)[i] = encdesc[i].code;
	return KNOWN_ENCS;
}

int attribute_align_arg out123_enc_byname(const char *name)
{
	int i;
	if(!name)
		return OUT123_ERR;
	for(i=0; i<KNOWN_ENCS; ++i) if(
		!strcasecmp(encdesc[i].name, name)
	||	!strcasecmp(encdesc[i].longname, name)
	)
		return encdesc[i].code;
	return OUT123_ERR;
}

const char* attribute_align_arg out123_enc_name(int encoding)
{
	int i;
	for(i=0; i<KNOWN_ENCS; ++i) if(encdesc[i].code == encoding)
		return encdesc[i].name;
	return NULL;
}

const char* attribute_align_arg out123_enc_longname(int encoding)
{
	int i;
	for(i=0; i<KNOWN_ENCS; ++i) if(encdesc[i].code == encoding)
		return encdesc[i].longname;
	return NULL;
}

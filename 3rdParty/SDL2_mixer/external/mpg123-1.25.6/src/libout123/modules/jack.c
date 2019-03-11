/*
	jack: audio output via JACK Audio Connection Kit

	copyright 2006-2016 by the mpg123 project - free software under the terms of the LGPL 2.1
	see COPYING and AUTHORS files in distribution or http://mpg123.org
	initially written by Nicholas J. Humfrey

	I reworked the processing logic. Only one ringbuffer, deinterleaving in the
	processing callback. A semaphore to avoid using usleep() for waiting. Up
	to 99 channels. Only float input (ensures that libmpg123 selects f32
	encoding). This is still a hack to shoehorn the JACK API into our model.

	Damn. I'm wary of he semaphore. I'm sure I constructed a deadlock there.
	There's always a deadlock. --ThOr
*/

#include "out123_int.h"

#include <math.h>

#include <jack/jack.h>
#include <jack/ringbuffer.h>
/* Using some pthread to provide synchronization between process callback
   and writer part. The JACK API is not meant for this. Libpthread is
   pulled in as libjack dependency anyway. */
#include <semaphore.h>
#include <sys/errno.h>

#include "debug.h"

typedef struct {
	int alive;
	sem_t sem; /* semaphore to avoid busy waiting */
	int channels;
	int encoding;
	int framesize;
	jack_default_audio_sample_t **ports_buf;
	jack_port_t **ports;
	jack_ringbuffer_t *rb;
	size_t rb_size; /* in bytes */
	jack_client_t *client;
	char *procbuf;
	size_t procbuf_frames; /* in PCM frames */
} jack_handle_t, *jack_handle_ptr;

static jack_handle_t* alloc_jack_handle(out123_handle *ao)
{
	jack_handle_t *handle=NULL;
	int i;

	handle = malloc(sizeof(jack_handle_t));
	if (!handle)
		return NULL;
	handle->channels = ao->channels;
	handle->encoding = ao->format;
	handle->framesize = ao->framesize;
	handle->rb = NULL;
	handle->ports_buf = malloc( sizeof(jack_default_audio_sample_t*)
	*	ao->channels );
	handle->ports = malloc(sizeof(jack_port_t*)*ao->channels);
	if(!handle->ports_buf || !handle->ports)
	{
		if(handle->ports_buf)
			free(handle->ports_buf);
		if(handle->ports)
			free(handle->ports);
		free(handle);
		return NULL;
	}
	for(i=0; i<ao->channels; ++i)
	{
		handle->ports_buf[i] = NULL;
		handle->ports[i] = NULL;
	}
	if(sem_init(&handle->sem, 0, 0))
	{
		if(!AOQUIET)
			error("Semaphore init failed.");
		free(handle->ports_buf);
		free(handle->ports);
		free(handle);
		return NULL;
	}
	handle->alive = 0;
	handle->client = NULL;
	handle->procbuf = NULL;
	handle->rb_size = 0;
	handle->procbuf_frames = 0;

	return handle;
}


static void free_jack_handle( jack_handle_t* handle )
{
	int i;

	if(handle->ports)
	{
		if(handle->client)
		for(i=0; i<handle->channels; i++)
		{
			/* Close the port for channel*/
			if(handle->ports[i])
				jack_port_unregister(handle->client, handle->ports[i]);
		}
		free(handle->ports);
	}
	if(handle->ports_buf)
		free(handle->ports_buf);
	/* Free up the ring buffer for channel*/
	if(handle->rb)
		jack_ringbuffer_free(handle->rb);
	if (handle->client)
		jack_client_close(handle->client);
	if (handle->procbuf)
		free(handle->procbuf);
	sem_destroy(&handle->sem);
	free(handle);
}


static int process_callback( jack_nframes_t nframes, void *arg )
{
	int c;
	jack_handle_t* handle = (jack_handle_t*)arg;
	size_t to_read = nframes;

	for(c=0; c<handle->channels; ++c)
		handle->ports_buf[c] =
			jack_port_get_buffer(handle->ports[c], nframes);

	/* One ringbuffer to rule them all, getting interleaved data piecewise
	   and appending to non-interleaved buffers. */
	while(to_read)
	{
		/* Need to read into temporary storage, then deinterleave to JACK
		   buffers. */
		size_t got_piece;
		size_t avail_piece;
		size_t piece = to_read > handle->procbuf_frames
		?	handle->procbuf_frames
		:	to_read;
		/* Ensure we get only full PCM frames by checking available byte count
		   and reducing expectation. */
		avail_piece = jack_ringbuffer_read_space(handle->rb)/handle->framesize;
		got_piece = jack_ringbuffer_read( handle->rb
		,	handle->procbuf, (avail_piece > piece ? piece : avail_piece)
		*	handle->framesize ) / handle->framesize;
		debug2( "fetched %"SIZE_P" frames from ringbuffer (wanted %"SIZE_P")"
		,	(size_p)got_piece, (size_p)piece );
		/* If this is the last piece, fill up, not time to wait. */
		if(to_read > piece)
			piece = got_piece; /* We got further loop cycle(s) to get the rest. */
		else
		{
			if(piece > got_piece)
			{
				debug("filling up with zeros");
				bzero( handle->procbuf+got_piece*handle->framesize
				,	(piece-got_piece)*handle->framesize );
			}
		}
		/* Now extract the pieces for the channels. */
		for (c=0; c < handle->channels; ++c)
		{
			size_t n;
			jack_default_audio_sample_t *dst = handle->ports_buf[c];
			if(handle->encoding == MPG123_ENC_FLOAT_32)
			{
				float* src = (float*)handle->procbuf;
				for(n=0; n<piece; ++n)
					*(dst++) = src[(n*handle->channels)+c];
			}
			else /* MPG123_ENC_FLOAT_64 */
			{
				double* src = (double*)handle->procbuf;
				for(n=0; n<piece; ++n)
					*(dst++) = src[(n*handle->channels)+c];
			}
			/* Store output buffer offset. */
			handle->ports_buf[c] = dst;
		}
		/* Give the writer a hint about the time passed. */
		sem_post(&handle->sem);
		to_read -= piece;
	}
	/* Success*/
	return 0;
}

/* This is triggered on server shutdown and very much necessary to avoid
   out123 hanging on the processor semaphore. */
static void shutdown_callback(void *arg)
{
	jack_handle_t* handle = (jack_handle_t*)arg;
	handle->alive = 0;
	sem_post(&handle->sem);
	debug("shutdown_callback()");
}

/* connect to jack ports named in the NULL-terminated wishlist */
static int real_connect_jack_ports(out123_handle *ao
,	jack_handle_t* handle, const char** wishlist)
{
	const char **wish = wishlist;
	int ch, err;
	int ch_wrap = 0, wish_wrap = 0;

	if(!wish)
		return 0;
	if(wish != NULL && *wish == NULL)
		return 1; /* success, nothing connected as wanted */
	ch=0;
	/* Connect things as long as there are sources or sinks left. */
	while(!wish_wrap || !ch_wrap)
	{
		const char* in = jack_port_name(handle->ports[ch]);

		if((err = jack_connect(handle->client, in, *wish)) != 0 && err != EEXIST)
		{
			if(!AOQUIET)
				error4( "connect_jack_ports(): failed to jack_connect() ch%i (%s) to %s: %d"
				,	ch, in ? in : "<nil>", *wish, err );
			return 0;
		}
		/*
			Increment channel and wishlist, both possibly wrapping around, to
			ensure we connected all channels to some output port and provided
			some input to all ports in the wishlist. Both cases of less channels
			than output ports (splitting) and more channels	than output ports
			(downmix) are sensible.
		*/
		if(++ch == handle->channels)
		{
			ch = 0;
			++ch_wrap;
		}
		if(!*(++wish))
		{
			wish = wishlist;
			++wish_wrap;
		}
	}

	return 1;
}

/* crude way of automatically connecting up jack ports */
/* 0 on error */
static int autoconnect_jack_ports(out123_handle *ao, jack_handle_t* handle)
{
	const char **all_ports;
	unsigned int ch=0;
	int err,i;

	/* Get a list of all the jack ports*/
	all_ports = jack_get_ports (handle->client, NULL, NULL, JackPortIsInput);
	if(!all_ports)
	{
		if(!AOQUIET)
			error("connect_jack_ports(): jack_get_ports() returned NULL.");
		return 0;
	}
	/* Step through each port name*/
	for (i = 0; all_ports[i]; ++i)
	{
		const char* in = jack_port_name( handle->ports[ch] );
		const char* out = all_ports[i];

		if ((err = jack_connect(handle->client, in, out)) != 0 && err != EEXIST)
		{
			if(!AOQUIET)
				error1("connect_jack_ports(): failed to jack_connect() ports: %d",err);
			return 0;
		}

		/* Found enough ports ?*/
		if (++ch >= handle->channels) break;
	}
	jack_free(all_ports);
	return 1;
}


static int connect_jack_ports(out123_handle *ao
,	jack_handle_t* handle)
{
	debug1("connect_jack_ports with dev=%s", ao->device ? ao->device : "<nil>");
	if(ao->device==NULL || strcmp(ao->device, "auto")==0)
		return autoconnect_jack_ports(ao, handle);
	else
	{
		/* Parse device for a set of ports, comma separated. */
		const char** wishlist; /* Channels and end marker. */
		int wish_channels = 1; /* Numper of entries in wishlist. */
		char *devcopy, *chr;
		int ret;
		int c;
		size_t len;
		len = strlen(ao->device);
		/* We connect as many JACK ports as desired, possibly duplicating. */
		for(chr=ao->device; *chr; ++chr)
			if(*chr == ',')
				++wish_channels;
		debug1("wish_channels: %i", wish_channels);
		wishlist = malloc(sizeof(char*)*(wish_channels+1));
		devcopy = compat_strdup(ao->device);
		if(devcopy == NULL || wishlist == NULL)
		{
			if(devcopy)
				free(devcopy);
			if(wishlist)
				free(wishlist);
			if(!AOQUIET)
				error("OOM");
			return 0;
		}

		for(c=0;c<=wish_channels;++c)
			wishlist[c] = NULL;
		if(len && strcmp(devcopy, "none"))
		{
			size_t i=0;
			wishlist[0] = devcopy;
			for(c=0;c<wish_channels;++c)
			{
				while(devcopy[i] != 0 && devcopy[i] != ',') ++i;
				debug2("devcopy[%"SIZE_P"]=%i", i, devcopy[i]);
				if(devcopy[i] == ',')
				{
					/* Terminate previous port name, assign next one. */
					devcopy[i++] = 0;
					debug2("terminaled wish %i: %s", c, wishlist[c]);
					if(c+1 < wish_channels)
						wishlist[c+1] = devcopy+i;
				}
				else
					break;
			}
		}
		if(wishlist[0] == NULL && !AOQUIET)
			warning("Not connecting up jack ports as requested.");

		ret = real_connect_jack_ports(ao, handle, wishlist);
		free(devcopy);
		free(wishlist);
		return ret;
	}
	return 1;
}

static void drain_jack(out123_handle *ao)
{
	jack_handle_t *handle = (jack_handle_t*)ao->userptr;

	debug("drain_jack().");

	do errno = 0;
	while(sem_trywait(&handle->sem) == 0 || errno == EINTR);
	/* For some reason, a single byte is reserved by JACK?! */
	while(  handle && handle->alive && handle->rb
	     && jack_ringbuffer_write_space(handle->rb)+1 < handle->rb_size )
	{
		debug2( "JACK close wait %"SIZE_P" < %"SIZE_P"\n"
		,	(size_p)jack_ringbuffer_write_space(handle->rb)
		,	(size_p)handle->rb_size );
		sem_wait(&handle->sem);
	}
}

static int close_jack(out123_handle *ao)
{
	jack_handle_t *handle = (jack_handle_t*)ao->userptr;

	debug("close_jack().");
	/* Close and shutdown*/
	if(handle)
	{
		free_jack_handle(handle);
		ao->userptr = NULL;
	}
	return 0;
}


static int open_jack(out123_handle *ao)
{
	jack_handle_t *handle=NULL;
	jack_options_t jopt = JackNullOption|JackNoStartServer;
	jack_status_t jstat = 0;
	unsigned int i;
	char *realname;

	debug("jack open");
	if(!ao)
		return -1;

	/* Return if already open*/
	if(ao->userptr)
	{
		if(!AOQUIET)
			error("audio device is already open.");
		return -1;
	}

	/* The initial open lets me choose the settings. */
	if (ao->format==-1)
	{
		ao->format = MPG123_ENC_FLOAT_32;
		ao->channels = 2;
		/* Really need a framesize defined for callback. */
		ao->framesize = 2*4;
	}
	else if(!(ao->format & MPG123_ENC_FLOAT))
	{
		if(!AOQUIET)
			error("JACK only wants float!");
		return -1;
	}

	/* Create some storage for ourselves*/
	if((handle = alloc_jack_handle(ao)) == NULL)
		return -1;
	ao->userptr = (void*)handle;

	/* Register with Jack*/
	if((handle->client = jack_client_open(ao->name, jopt, &jstat)) == 0)
	{
		if(!AOQUIET)
			error1("Failed to open jack client: 0x%x", jstat);
		close_jack(ao);
		return -1;
	}

	realname = jack_get_client_name(handle->client);
	/* Display the unique client name allocated to us */
	if(AOVERBOSE(1))
		fprintf( stderr, "Registered as JACK client %s.\n"
		,	realname ? realname : "<nil>" );

	/* Just make sure. */
	ao->rate   = jack_get_sample_rate(handle->client);

	/* Check the sample rate is correct*/
	if (jack_get_sample_rate( handle->client ) != (jack_nframes_t)ao->rate)
	{
		if(!AOQUIET)
			error("JACK Sample Rate is different to sample rate of file.");
		close_jack(ao);
		return -1;
	}

	/* Register ports with Jack*/
	if(handle->channels > 0 && handle->channels < 100)
	{
		for(i=0;i<handle->channels;++i)
		{
			char numbuf[3]; /* two digits, zero byte */
			sprintf(numbuf, "%d", i+1);
			if( !(handle->ports[i] = jack_port_register( handle->client
			                         , numbuf, JACK_DEFAULT_AUDIO_TYPE
			                         , JackPortIsOutput, 0 )) )
			{
				if(!AOQUIET)
					error1("Cannot register JACK output port '%s'.", numbuf);
				close_jack(ao);
				return -1;
			}
		}
	}
	else
	{
		if(!AOQUIET)
			error1("excessive number of output channels (%d).", handle->channels);
		close_jack(ao);
		return -1;
	}

	/* Use device_buffer parameter for ring buffer, but ensure that two
	   JACK buffers fit in there. We do not support that buffer increasing
	   later on. */
	handle->rb_size = (size_t)( ao->device_buffer
	*	jack_get_sample_rate(handle->client)
	+	0.5 ); /* PCM frames */
	handle->procbuf_frames = jack_get_buffer_size(handle->client);
	if(handle->rb_size < 2*handle->procbuf_frames)
		handle->rb_size = 2*handle->procbuf_frames;
	debug1("JACK ringbuffer for %"SIZE_P" PCM frames", (size_p)handle->rb_size);
	/* Convert to bytes. */
	handle->rb_size *= handle->framesize;
	handle->rb = jack_ringbuffer_create(handle->rb_size);
	handle->procbuf = malloc(handle->procbuf_frames*handle->framesize);
	if(!handle->rb || !handle->procbuf)
	{
		if(!AOQUIET)
			error("failed to allocate buffers");
		close_jack(ao);
		return -1;
	}

	/* Set the callbacks*/
	jack_set_process_callback(handle->client, process_callback, (void*)handle);
	jack_on_shutdown(handle->client, shutdown_callback, (void*)handle);
	handle->alive = 1;
	/* Activate client*/
	if(jack_activate(handle->client))
	{
		if(!AOQUIET)
			error("Can't activate client.");
		close_jack(ao);
		return -1;
	}

	/* Connect up the portsm, return */
	if(!connect_jack_ports(ao, handle))
	{
		/* deregistering of ports will not work but should just fail, then,
		   and let the rest clean up */
		close_jack(ao);
		return -1;
	}

	debug("Jack open successful.\n");
	ao->realname = compat_strdup(realname);
	return 0;
}


/* Jack prefers floats, I actually assume it does _only_ float/double
   (as it is nowadays)! */
static int get_formats_jack(out123_handle *ao)
{
	jack_handle_t *handle = (jack_handle_t*)ao->userptr;

	if(jack_get_sample_rate(handle->client) != (jack_nframes_t)ao->rate)
		return 0;
	else
		return MPG123_ENC_FLOAT_32|MPG123_ENC_FLOAT_64;
}

static int write_jack(out123_handle *ao, unsigned char *buf, int len)
{
	jack_handle_t *handle = (jack_handle_t*)ao->userptr;
	size_t bytes_left;
	unsigned int strike = 0;

	bytes_left = len;
	while(bytes_left && handle->alive)
	{
		size_t piece;

		debug("writing to ringbuffer");
		/* No help: piece1 = jack_ringbuffer_write_space(handle->rb); */
		piece = jack_ringbuffer_write(handle->rb, (char*)buf, bytes_left);
		debug1("wrote %"SIZE_P" B", (size_p)piece);
		buf += piece;
		bytes_left -= piece;
		/* Allow nothing being written some times, but not too often. 
		   Don't know how often in a row that would be supposed to happen. */
		if(!piece)
		{
			if(++strike > 100)
			{
				if(!AOQUIET)
					error("Cannot write to ringbuffer.");
				break;
			}
			/* Avoid busy waiting and semaphore accumulation:
			   Wait once on the semaphore, then clear it. We count on it being
			   posted by the process callback and we are going to push new data
			   so that that one gets the chance. */
			sem_wait(&handle->sem);
			do errno = 0;
			while(sem_trywait(&handle->sem) == 0 || errno == EINTR);
		}
		else
			strike = 0;
	}

	return len-bytes_left;
}

static void flush_jack(out123_handle *ao)
{
	jack_handle_t *handle = (jack_handle_t*)ao->userptr;
	/* Reset the ring buffers*/
	jack_ringbuffer_reset(handle->rb);
}

static int init_jack(out123_handle* ao)
{
	if (ao==NULL)
		return -1;
	/* Set callbacks */
	ao->open = open_jack;
	ao->flush = flush_jack;
	ao->drain = drain_jack;
	ao->write = write_jack;
	ao->get_formats = get_formats_jack;
	ao->close = close_jack;
	ao->propflags |= OUT123_PROP_PERSISTENT;
	/* Success */
	return 0;
}

/* 
	Module information data structure
*/
mpg123_module_t mpg123_output_module_info = {
	/* api_version */	MPG123_MODULE_API_VERSION,
	/* name */			"jack",
	/* description */	"Output audio using JACK (JACK Audio Connection Kit).",
	/* revision */		"$Rev:$",
	/* handle */		NULL,

	/* init_output */	init_jack
};

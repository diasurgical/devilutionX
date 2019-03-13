/*
	alib: audio output for HP-UX using alib

	copyright ?-2006 by the mpg123 project - free software under the terms of the LGPL 2.1
	see COPYING and AUTHORS files in distribution or http://mpg123.org
	initially written by Erwan Ducroquet
	based on source code from HP (Audio SDK)
*/

/*
 *
 * for mpg123 :
 * hpux:
 *  $(MAKE) \
 *  CC=cc \
 *  LDFLAGS=-L/opt/audio/lib \
 *  AUDIO_LIB=-lAlib \
 *  OBJECTS=decode.o dct64.o \
 *  CFLAGS=-Ae +O3 -DREAL_IS_FLOAT -D_HPUX_SOURCE -DHPUX -I/opt/audio/include \
 *  mpg123
 */


/*
 * For the user :
 * If you launch mpg123 on a XTerm with sound capabilities, it's OK
 * Else, you have to set the environment variable "AUDIO" to the name of
 * an HP Xterm with sound card.
 */

/**************************************************************************/

#include "out123_int.h"

#include <fcntl.h>

#include <sys/socket.h>
#include <netdb.h>
#include <netinet/in.h>
#include <netinet/tcp.h>
#include <errno.h>


#include <Alib.h>   /* /opt/audio/include */
#include <CUlib.h>  /* /opt/audio/include */

#include "debug.h"

/**************************************************************************/


/* FIXME: These globals should be moved into a structure */
static Audio *audioServer = (Audio *) NULL;
static struct protoent *tcpProtocolEntry;
static ATransID xid;

static void printAudioError(Audio *audio,char *message,int errorCode) {
	char    errorbuff[132];
	AGetErrorText(audio, errorCode, errorbuff, 131);
	error2("%s: %s", message, errorbuff);
}
static long myHandler(Audio *audio,AErrorEvent *err_event) {
	printAudioError( audio, "Audio error", err_event->error_code ); 
	/* we cannot just do random exists, that messes terminal up
	 need proper error propagation in that case for future, setting intflag or such */
	/* exit(1); */
}

/**************************************************************************/

/*
 * Set the fn element of ai
 * Use ao->rate and ao->channels
 * Doesn't set any volume
 */

/* return on error leaves stuff dirty here... */
static int open_alib(out123_handle *ao)
{
	AudioAttributes Attribs;
	AudioAttrMask   AttribsMask;
	AGainEntry      gainEntry[4];
	SSPlayParams    playParams;
	SStream	  audioStream;
	AErrorHandler   prevHandler;
	char		  server[1];
	int		  i;
	long            status;
	
	if (audioServer) {
		error("openAudio: audio already open");
		return -1;
	}
	
	prevHandler = ASetErrorHandler(myHandler);
	
	server[0] = '\0';
	audioServer = AOpenAudio( server, NULL );
	if (audioServer==NULL) {
		error("Error: could not open audio\n");
		return -1;
	}
	
	ao->fn = socket( AF_INET, SOCK_STREAM, 0 );
	if(ao->fn<0) {
		error("Socket creation failed");
		return -1;
	}
	
	Attribs.type = ATSampled;
	Attribs.attr.sampled_attr.sampling_rate = ao->rate;
	Attribs.attr.sampled_attr.channels	  = ao->channels;
	Attribs.attr.sampled_attr.data_format	  = ADFLin16;
	AttribsMask = ASSamplingRateMask | ASChannelsMask  | ASDataFormatMask;
	
	gainEntry[0].gain = AUnityGain;
	gainEntry[0].u.o.out_ch  = AOCTMono;
	gainEntry[0].u.o.out_dst = AODTDefaultOutput;
	
	playParams.gain_matrix.type = AGMTOutput;  /* gain matrix */
	playParams.gain_matrix.num_entries = 1;
	playParams.gain_matrix.gain_entries = gainEntry;
	playParams.play_volume = AUnityGain;       /* play volume */
	playParams.priority = APriorityNormal;     /* normal priority */
	playParams.event_mask = 0;                 /* don't solicit any events */
	
	xid=APlaySStream(audioServer,AttribsMask,&Attribs,
	&playParams,&audioStream,NULL);
	
	status=connect(ao->fn,
	(struct sockaddr *) &audioStream.tcp_sockaddr,
	sizeof(struct sockaddr_in) );
	if (status<0) {
		error("Connect failed");
		return -1;
	}
	
	i=-1;
	tcpProtocolEntry=getprotobyname("tcp");
	setsockopt(ao->fn,tcpProtocolEntry->p_proto,TCP_NODELAY,&i,sizeof(i));
	
	return ao->fn;
}

/**************************************************************************/

static int close_alib(out123_handle *ao)
{
	close(ao->fn);
	ASetCloseDownMode( audioServer, AKeepTransactions, NULL );
	ACloseAudio( audioServer, NULL );
	audioServer = (Audio *) NULL;
	return 0;
}

/**************************************************************************/

/*
 * very simple
 * deserv to be inline
 */

static int write_alib(out123_handle *ao,unsigned char *buf,int len)
{
	return write(ao->fn,buf,len*2);
}

/**************************************************************************/

static int get_formats_alib(out123_handle *ao)
{
	return MPG123_ENC_SIGNED_16;
}

static void flush_alib(out123_handle *ao)
{
}


static int init_alib(out123_handle* ao)
{
	if (ao==NULL) return -1;

	/* Set callbacks */
	ao->open = open_alib;
	ao->flush = flush_alib;
	ao->write = write_alib;
	ao->get_formats = get_formats_alib;
	ao->close = close_alib;

	/* Success */
	return 0;
}



/* 
	Module information data structure
*/
mpg123_module_t mpg123_output_module_info = {
	/* api_version */	MPG123_MODULE_API_VERSION,
	/* name */			"alib",						
	/* description */	"Output audio HP-UX using alib.",
	/* revision */		"$Rev:$",						
	/* handle */		NULL,
	
	/* init_output */	init_alib,						
};


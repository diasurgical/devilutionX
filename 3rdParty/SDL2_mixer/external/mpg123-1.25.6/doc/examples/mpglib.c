/*
	mpglib: test program for libmpg123, in the style of the legacy mpglib test program

	copyright 2007 by the mpg123 project - free software under the terms of the LGPL 2.1
	see COPYING and AUTHORS files in distribution or http://mpg123.org
	initially written by Thomas Orgis
*/

#include <mpg123.h>

/* unistd.h is not available under MSVC, 
 io.h defines the read and write functions */
#ifndef _MSC_VER
#include <unistd.h>
#else
#include <io.h>
#endif

#ifdef _WIN32
#include <fcntl.h>
#endif

#include <stdio.h>

#define INBUFF  16384
#define OUTBUFF 32768 

int main(int argc, char **argv)
{
	size_t size;
	unsigned char buf[INBUFF];  /* input buffer  */
	unsigned char out[OUTBUFF]; /* output buffer */
	ssize_t len;
	int ret;
	size_t in = 0, outc = 0;
	mpg123_handle *m;

#ifdef _WIN32
_setmode(_fileno(stdin),_O_BINARY);
_setmode(_fileno(stdout),_O_BINARY);
#endif

	mpg123_init();
	m = mpg123_new(argc > 1 ? argv[1] : NULL, &ret);
	if(m == NULL)
	{
		fprintf(stderr,"Unable to create mpg123 handle: %s\n", mpg123_plain_strerror(ret));
		return -1;
	}
	mpg123_param(m, MPG123_VERBOSE, 2, 0); /* Brabble a bit about the parsing/decoding. */

	/* Now mpg123 is being prepared for feeding. The main loop will read chunks from stdin and feed them to mpg123;
	   then take decoded data as available to write to stdout. */
	mpg123_open_feed(m);
	if(m == NULL) return -1;

	fprintf(stderr, "Feed me some MPEG audio to stdin, I will decode to stdout.\n");
	while(1) /* Read and write until everything is through. */
	{
		len = read(0,buf,INBUFF);
		if(len <= 0)
		{
			fprintf(stderr, "input data end\n");
			break;
		}
		in += len;
		/* Feed input chunk and get first chunk of decoded audio. */
		ret = mpg123_decode(m,buf,len,out,OUTBUFF,&size);
		if(ret == MPG123_NEW_FORMAT)
		{
			long rate;
			int channels, enc;
			mpg123_getformat(m, &rate, &channels, &enc);
			fprintf(stderr, "New format: %li Hz, %i channels, encoding value %i\n", rate, channels, enc);
		}
		write(1,out,size);
		outc += size;
		while(ret != MPG123_ERR && ret != MPG123_NEED_MORE)
		{ /* Get all decoded audio that is available now before feeding more input. */
			ret = mpg123_decode(m,NULL,0,out,OUTBUFF,&size);
			write(1,out,size);
			outc += size;
		}
		if(ret == MPG123_ERR){ fprintf(stderr, "some error: %s", mpg123_strerror(m)); break; }
	}
	fprintf(stderr, "%lu bytes in, %lu bytes out\n", (unsigned long)in, (unsigned long)outc);

	/* Done decoding, now just clean up and leave. */
	mpg123_delete(m);
	mpg123_exit();
	return 0;
}

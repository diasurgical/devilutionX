/*
	extract_frams: utlize the framebyframe API and mpg123_framedata to extract the MPEG frames out of a stream (strip off anything else).

	copyright 2011-2013 by the mpg123 project - free software under the terms of the LGPL 2.1
	see COPYING and AUTHORS files in distribution or http://mpg123.org
	initially written by Thomas Orgis
*/

#include "config.h"
#include "compat.h"
#include <mpg123.h>

#include "getlopt.h"


static struct
{
	int info;
	long icy_interval;
	int verbose;
} param =
{
	 TRUE
	,0
	,0
};

static const char* progname;

static void usage(int err)
{
	FILE* o = stdout;
	if(err)
	{
		o = stderr; 
		fprintf(o, "You made some mistake in program usage... let me briefly remind you:\n\n");
	}
	fprintf(o, "Extract only MPEG frames from a stream using libmpg123 (stdin to stdout)\n");
	fprintf(o, "\tversion %s; written and copyright by Thomas Orgis and the mpg123 project\n", PACKAGE_VERSION);
	fprintf(o,"\nusage: %s [option(s)] < input > output\n", progname);
	fprintf(o,"\noptions:\n");
	fprintf(o," -h     --help              give usage help\n");
	fprintf(o," -i <n> --icy-interval <n>  stream has ICY metadata present with this interval\n");
	fprintf(o," -n     --no-info           also strip info frame at beginning\n");
	fprintf(o," -v[*]  --verbose           increase verbosity level\n");
	exit(err);
}

static void want_usage(char* bla)
{
	usage(0);
}

static void set_verbose (char *arg)
{
    param.verbose++;
}

static topt opts[] =
{
	 {'h', "help", 0, want_usage, 0, 0}
	,{'i', "icy-interval", GLO_ARG|GLO_LONG, 0, &param.icy_interval, 0}
	,{'n', "no-info", GLO_INT, 0, &param.info, FALSE}
	,{'v', "verbose", 0, set_verbose, 0, 0}
	,{0, 0, 0, 0, 0, 0}
};

int do_work(mpg123_handle *m);

int main(int argc, char **argv)
{
	int ret = 0;
	mpg123_handle *m;

	progname = argv[0];

	while ((ret = getlopt(argc, argv, opts)))
	switch (ret) {
		case GLO_UNKNOWN:
			fprintf (stderr, "%s: Unknown option \"%s\".\n", 
				progname, loptarg);
			usage(1);
		case GLO_NOARG:
			fprintf (stderr, "%s: Missing argument for option \"%s\".\n",
				progname, loptarg);
			usage(1);
	}

	mpg123_init();
	m = mpg123_new(NULL, &ret);

	if(m == NULL)
	{
		fprintf(stderr, "Cannot create handle: %s", mpg123_plain_strerror(ret));
	}
	else
	{
		ret = mpg123_param(m, MPG123_VERBOSE, param.verbose, 0.);
		if(ret == MPG123_OK)
		{
			if(param.verbose)
			fprintf(stderr, "Info frame handling: %s\n",
				param.info ? "pass-through" : "remove");
			ret = param.info /* If info frame is ignored, it is treated as MPEG data. */
				? mpg123_param(m, MPG123_ADD_FLAGS, MPG123_IGNORE_INFOFRAME, 0.)
				: mpg123_param(m, MPG123_REMOVE_FLAGS, MPG123_IGNORE_INFOFRAME, 0.);
		}
		if(ret == MPG123_OK && param.icy_interval > 0)
		{
			if(param.verbose) fprintf(stderr, "ICY interval: %li\n", param.icy_interval);
			ret = mpg123_param(m, MPG123_ICY_INTERVAL, param.icy_interval, 0);
		}

		if(ret == MPG123_OK) ret = do_work(m);

		if(ret != MPG123_OK) fprintf(stderr, "Some error occured: %s\n", mpg123_strerror(m));

		mpg123_delete(m); /* Closes, too. */
	}
	mpg123_exit();

	return ret;
}

int do_work(mpg123_handle *m)
{
	int ret;
	size_t count = 0;
	ret = mpg123_open_fd(m, STDIN_FILENO);
	if(ret != MPG123_OK) return ret;

	while( (ret = mpg123_framebyframe_next(m)) == MPG123_OK || ret == MPG123_NEW_FORMAT )
	{
		unsigned long header;
		unsigned char *bodydata;
		size_t bodybytes;
		if(mpg123_framedata(m, &header, &bodydata, &bodybytes) == MPG123_OK)
		{
			/* Need to extract the 4 header bytes from the native storage in the correct order. */
			unsigned char hbuf[4];
			int i;
			for(i=0; i<4; ++i) hbuf[i] = (unsigned char) ((header >> ((3-i)*8)) & 0xff);

			/* Now write out both header and data, fire and forget. */
			write(STDOUT_FILENO, hbuf, 4);
			write(STDOUT_FILENO, bodydata, bodybytes);
			if(param.verbose)
			fprintf(stderr, "%"SIZE_P": header 0x%08lx, %"SIZE_P" body bytes\n"
			, (size_p)++count, header, (size_p)bodybytes);
		}
	}

	if(ret != MPG123_DONE)
	fprintf(stderr, "Some error occured (non-fatal?): %s\n", mpg123_strerror(m));

	if(param.verbose) fprintf(stderr, "Done with %"SIZE_P" MPEG frames.\n"
	, (size_p)count);

	return MPG123_OK;
}

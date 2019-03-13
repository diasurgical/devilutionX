/*
	id3dump: Print ID3 tags of files, scanned using libmpg123.

	copyright 2007 by the mpg123 project - free software under the terms of the LGPL 2.1
	see COPYING and AUTHORS files in distribution or http://mpg123.org
	initially written by Thomas Orgis
*/

/* Need snprintf(). */
#define _DEFAULT_SOURCE
#define _BSD_SOURCE
#include "config.h"
#include "compat.h"
#include "mpg123.h"
#include "getlopt.h"
#include <errno.h>
#include <ctype.h>
#include "debug.h"
#include "win32_support.h"

static int errors = 0;

static struct
{
	int store_pics;
	int do_scan;
} param =
{
	  FALSE
	, TRUE
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
	fprintf(o, "Tool to dump ID3 meta data from MPEG audio files using libmpg123\n");
	fprintf(o, "\tversion %s; written and copyright by Thomas Orgis and the mpg123 project\n", PACKAGE_VERSION);
	fprintf(o,"\nusage: %s [option(s)] file(s)\n", progname);
	fprintf(o,"\noptions:\n");
	fprintf(o," -h     --help              give usage help\n");
	fprintf(o," -n     --no-scan           do not scan entire file (just beginning)\n");
	fprintf(o," -p     --store-pics        write APIC frames (album art pictures) to files\n");
	fprintf(o,"                            file names using whole input file name as prefix\n");
	fprintf(o,"\nNote that text output will always be in UTF-8, regardless of locale.\n");
	exit(err);
}
static void want_usage(char* bla)
{
	usage(0);
}

static topt opts[] =
{
	 {'h', "help",         0,       want_usage, 0,                 0}
	,{'n', "no-scan",      GLO_INT, 0,          &param.do_scan,    FALSE}
	,{'p', "store-pics",   GLO_INT, 0,          &param.store_pics, TRUE}
	,{0, 0, 0, 0, 0, 0}
};

/* Helper for v1 printing, get these strings their zero byte. */
void safe_print(char* name, char *data, size_t size)
{
	char safe[31];
	if(size>30) return;

	memcpy(safe, data, size);
	safe[size] = 0;
	printf("%s: %s\n", name, safe);
}

/* Print out ID3v1 info. */
void print_v1(mpg123_id3v1 *v1)
{
	safe_print("Title",   v1->title,   sizeof(v1->title));
	safe_print("Artist",  v1->artist,  sizeof(v1->artist));
	safe_print("Album",   v1->album,   sizeof(v1->album));
	safe_print("Year",    v1->year,    sizeof(v1->year));
	safe_print("Comment", v1->comment, sizeof(v1->comment));
	printf("Genre: %i", v1->genre);
}

/* Split up a number of lines separated by \n, \r, both or just zero byte
   and print out each line with specified prefix. */
void print_lines(const char* prefix, mpg123_string *inlines)
{
	size_t i;
	int hadcr = 0, hadlf = 0;
	char *lines = NULL;
	char *line  = NULL;
	size_t len = 0;

	if(inlines != NULL && inlines->fill)
	{
		lines = inlines->p;
		len   = inlines->fill;
	}
	else return;

	line = lines;
	for(i=0; i<len; ++i)
	{
		if(lines[i] == '\n' || lines[i] == '\r' || lines[i] == 0)
		{
			char save = lines[i]; /* saving, changing, restoring a byte in the data */
			if(save == '\n') ++hadlf;
			if(save == '\r') ++hadcr;
			if((hadcr || hadlf) && hadlf % 2 == 0 && hadcr % 2 == 0) line = "";

			if(line)
			{
				lines[i] = 0;
				printf("%s%s\n", prefix, line);
				line = NULL;
				lines[i] = save;
			}
		}
		else
		{
			hadlf = hadcr = 0;
			if(line == NULL) line = lines+i;
		}
	}
}

/* Print out the named ID3v2  fields. */
void print_v2(mpg123_id3v2 *v2)
{
	print_lines("Title: ",   v2->title);
	print_lines("Artist: ",  v2->artist);
	print_lines("Album: ",   v2->album);
	print_lines("Year: ",    v2->year);
	print_lines("Comment: ", v2->comment);
	print_lines("Genre: ",   v2->genre);
}

/* Easy conversion to string via lookup. */
const char* pic_types[] = 
{
	 "other"
	,"icon"
	,"other icon"
	,"front cover"
	,"back cover"
	,"leaflet"
	,"media"
	,"lead"
	,"artist"
	,"conductor"
	,"orchestra"
	,"composer"
	,"lyricist"
	,"location"
	,"recording"
	,"performance"
	,"video"
	,"fish"
	,"illustration"
	,"artist logo"
	,"publisher logo"
};
static const char* pic_type(int id)
{
	return (id >= 0 && id < (sizeof(pic_types)/sizeof(char*)))
		? pic_types[id]
		: "invalid type";
}

/* Print out all stored ID3v2 fields with their 4-character IDs. */
void print_raw_v2(mpg123_id3v2 *v2)
{
	size_t i;
	for(i=0; i<v2->texts; ++i)
	{
		char id[5];
		char lang[4];
		memcpy(id, v2->text[i].id, 4);
		id[4] = 0;
		memcpy(lang, v2->text[i].lang, 3);
		lang[3] = 0;
		if(v2->text[i].description.fill)
		printf("%s language(%s) description(%s)\n", id, lang, v2->text[i].description.p);
		else printf("%s language(%s)\n", id, lang);

		print_lines(" ", &v2->text[i].text);
	}
	for(i=0; i<v2->extras; ++i)
	{
		char id[5];
		memcpy(id, v2->extra[i].id, 4);
		id[4] = 0;
		printf( "%s description(%s)\n",
		        id,
		        v2->extra[i].description.fill ? v2->extra[i].description.p : "" );
		print_lines(" ", &v2->extra[i].text);
	}
	for(i=0; i<v2->comments; ++i)
	{
		char id[5];
		char lang[4];
		memcpy(id, v2->comment_list[i].id, 4);
		id[4] = 0;
		memcpy(lang, v2->comment_list[i].lang, 3);
		lang[3] = 0;
		printf( "%s description(%s) language(%s):\n",
		        id,
		        v2->comment_list[i].description.fill ? v2->comment_list[i].description.p : "",
		        lang );
		print_lines(" ", &v2->comment_list[i].text);
	}
	for(i=0; i<v2->pictures; ++i)
	{
		mpg123_picture* pic;

		pic = &v2->picture[i];
		fprintf(stderr, "APIC type(%i, %s) mime(%s) size(%"SIZE_P")\n",
			pic->type, pic_type(pic->type), pic->mime_type.p, (size_p)pic->size);
		print_lines(" ", &pic->description);
	}
}

const char* unknown_end = "picture";

static char* mime2end(mpg123_string* mime)
{
	size_t len;
	char* end;
	if(strncasecmp("image/",mime->p,6))
	{
		len = strlen(unknown_end)+1;
		end = malloc(len);
		memcpy(end, unknown_end, len);
		return end;
	}

	/* Else, use fmt out of image/fmt ... but make sure that usage stops at
	   non-alphabetic character, as MIME can have funny stuff following a ";". */
	for(len=1; len<mime->fill-6; ++len)
	{
		if(!isalnum(mime->p[len-1+6])) break;
	}
	/* len now containing the number of bytes after the "/" up to the next
	   invalid char or null */
	if(len < 1) return "picture";

	end = malloc(len);
	if(!end) exit(11); /* Come on, is it worth wasting lines for a message? 
	                      If we're so broke, fprintf will also likely fail. */

	memcpy(end, mime->p+6,len-1);
	end[len-1] = 0;
	return end;
}

/* Construct a sane file name without introducing spaces, then open.
   Example: /some/where/some.mp3.front_cover.jpeg
   If multiple ones are there: some.mp3.front_cover2.jpeg */
int open_picfile(const char* prefix, mpg123_picture* pic)
{
	char *end, *typestr, *pfn;
	const char* pictype;
	size_t i, len;
	int fd;
	unsigned long count = 1;

	pictype = pic_type(pic->type);
	len = strlen(pictype);
	if(!(typestr = malloc(len+1))) exit(11);
	memcpy(typestr, pictype, len);
	for(i=0; i<len; ++i) if(typestr[i] == ' ') typestr[i] = '_';

	typestr[len] = 0;
	end = mime2end(&pic->mime_type);
	len = strlen(prefix)+1+strlen(typestr)+1+strlen(end);
	if(!(pfn = malloc(len+1))) exit(11);

	sprintf(pfn, "%s.%s.%s", prefix, typestr, end);
	pfn[len] = 0;

	errno = 0;
	fd = compat_open(pfn, O_CREAT|O_WRONLY|O_EXCL);
	while(fd < 0 && errno == EEXIST && ++count < ULONG_MAX)
	{
		char dum;
		size_t digits;

		digits = snprintf(&dum, 1, "%lu", count);
		if(!(pfn=safe_realloc(pfn, len+digits+1))) exit(11);

		sprintf(pfn, "%s.%s%lu.%s", prefix, typestr, count, end);
		pfn[len+digits] = 0;
		errno = 0;		
		fd = compat_open(pfn, O_CREAT|O_WRONLY|O_EXCL);
	}
	printf("writing %s\n", pfn);
	if(fd < 0)
	{
		error("Cannot open for writing (counter exhaust? permissions?).");
		++errors;
	}

	free(end);
	free(typestr);
	free(pfn);
	return fd;
}

static void store_pictures(const char* prefix, mpg123_id3v2 *v2)
{
	int i;

	for(i=0; i<v2->pictures; ++i)
	{
		int fd;
		mpg123_picture* pic;

		pic = &v2->picture[i];
		fd = open_picfile(prefix, pic);
		if(fd >= 0)
		{ /* stream I/O for not having to care about interruptions */
			FILE* picfile = compat_fdopen(fd, "w");
			if(picfile)
			{
				if(fwrite(pic->data, pic->size, 1, picfile) != 1)
				{
					error("Failure to write data.");
					++errors;
				}
				if(fclose(picfile))
				{
					error("Failure to close (flush?).");
					++errors;
				}
			}
			else
			{
				error1("Unable to fdopen output: %s)", strerror(errno));
				++errors;
			}
		}
	}
}

int main(int argc, char **argv)
{
	int i, result;
	mpg123_handle* m;
#if defined(WANT_WIN32_UNICODE)
	win32_cmdline_utf8(&argc,&argv);
#endif
	progname = argv[0];

	while ((result = getlopt(argc, argv, opts)))
	switch (result) {
		case GLO_UNKNOWN:
			fprintf (stderr, "%s: Unknown option \"%s\".\n", 
				progname, loptarg);
			usage(1);
		case GLO_NOARG:
			fprintf (stderr, "%s: Missing argument for option \"%s\".\n",
				progname, loptarg);
			usage(1);
	}

#ifdef WIN32
	fprintf(stderr, "WARNING: This tool is not yet adapted to run on Windows (file I/O, unicode arguments)!\n");
#endif
	if(loptind >= argc) usage(1);

	mpg123_init();
	m = mpg123_new(NULL, NULL);
	mpg123_param(m, MPG123_ADD_FLAGS, MPG123_PICTURE, 0.);

	for(i=loptind; i < argc; ++i)
	{
		mpg123_id3v1 *v1;
		mpg123_id3v2 *v2;
		int meta;
		if(mpg123_open(m, argv[i]) != MPG123_OK)
		{
			fprintf(stderr, "Cannot open %s: %s\n", argv[i], mpg123_strerror(m));
			continue;
		}
		if(param.do_scan) mpg123_scan(m);
		mpg123_seek(m, 0, SEEK_SET);
		meta = mpg123_meta_check(m);
		if(meta & MPG123_ID3 && mpg123_id3(m, &v1, &v2) == MPG123_OK)
		{
			printf("FILE: %s\n", argv[i]);
			printf("\n====      ID3v1       ====\n");
			if(v1 != NULL) print_v1(v1);

			printf("\n====      ID3v2       ====\n");
			if(v2 != NULL) print_v2(v2);

			printf("\n==== ID3v2 Raw frames ====\n");
			if(v2 != NULL)
			{
				print_raw_v2(v2);
				if(param.store_pics)
				store_pictures(argv[i], v2);
			}
		}
		else printf("Nothing found for %s.\n", argv[i]);

		mpg123_close(m);
	}
	mpg123_delete(m);
	mpg123_exit();

	if(errors) error1("Encountered %i errors along the way.", errors);
	return errors != 0;
#if defined(WANT_WIN32_UNICODE)
	win32_cmdline_free(argc,argv);
#endif
}

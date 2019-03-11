/*
	id3print: display routines for ID3 tags (including filtering of UTF8 to ASCII)

	copyright 2006-2016 by the mpg123 project - free software under the terms of the LGPL 2.1
	see COPYING and AUTHORS files in distribution or http://mpg123.org
	initially written by Thomas Orgis
*/

/* Need snprintf(). */
#define _DEFAULT_SOURCE
#define _BSD_SOURCE
#include "mpg123app.h"
#include "common.h"
#include "genre.h"
#include "debug.h"

static const char joker_symbol = '*';

/* Metadata name field texts with index enumeration. */
enum tagcode { TITLE=0, ARTIST, ALBUM, COMMENT, YEAR, GENRE, FIELDS };
static const char* name[FIELDS] =
{
	"Title"
,	"Artist"
,	"Album"
,	"Comment"
,	"Year"
,	"Genre"
};

/* Two-column printing: max length of left and right name strings.
   see print_id3 for what goes left or right.
   Choose namelen[0] >= namelen[1]! */
static const int namelen[2] = {7, 6};
/* Overhead is Name + ": " and also plus "  " for right column. */
/* pedantic C89 does not like:
const int overhead[2] = { namelen[0]+2, namelen[1]+4 }; */
static const int overhead[2] = { 9, 10 };

static void utf8_ascii(mpg123_string *dest, mpg123_string *source);
/* Copy UTF-8 string or melt it down to ASCII, also returning the character length. */
static size_t transform(mpg123_string *dest, mpg123_string *source)
{
	debug("transform!");
	if(source == NULL) return 0;

	if(utf8env) mpg123_copy_string(source, dest);
	else        utf8_ascii(dest, source);

	return mpg123_strlen(dest, utf8env);
}

static void id3_gap(mpg123_string *dest, size_t count, char *v1, size_t *len)
{
	if(!dest->fill)
	{
		if(dest->size >= count+1 || mpg123_resize_string(dest, count+1))
		{
			strncpy(dest->p,v1,count);
			dest->p[count] = 0;
			*len = strlen(dest->p);
			dest->fill = *len + 1;
			/* We have no idea what encoding this is.
			   So, to prevent mess up of our UTF-8 display, filter anything above ASCII.
			   But in non-UTF-8 mode, we pray that the verbatim contents are meaningful to the user. Might filter non-printable characters, though. */
			if(utf8env)
			{
				size_t i;
				for(i=0; i<dest->fill-1; ++i)
				if(dest->p[i] & 0x80) dest->p[i] = joker_symbol;
			}
		}
	}
}

/* Print one metadata entry on a line, aligning the beginning. */
static void print_oneline( FILE* out
,	const mpg123_string *tag, enum tagcode fi, int long_mode )
{
	char fmt[14]; /* "%s:%-XXXs%s\n" plus one null */
	if(!tag[fi].fill && !long_mode)
		return;

	if(long_mode)
		fprintf(out, "\t");
	snprintf( fmt, sizeof(fmt)-1, "%%s:%%-%ds%%s\n"
	,	1+namelen[0]-(int)strlen(name[fi]) );
	fprintf(out, fmt, name[fi], " ", tag[fi].fill ? tag[fi].p : "");
}

/*
	Print a pair of tag name-value pairs along each other in two columns or
	each on a line if that is not sensible.
	This takes a given length (in columns) into account, not just bytes.
	If that length would be computed taking grapheme clusters into account, things
	could be fine for the whole world of Unicode. So far we ride only on counting
	possibly multibyte characters (unless mpg123_strlen() got adapted meanwhile).
*/
static void print_pair
(
	FILE* out /* Output stream. */
,	const int *climit /* Maximum width of columns (two values). */
,	const mpg123_string *tag /* array of tag value strings */
,	const size_t *len /* array of character/column lengths */
,	enum tagcode f0, enum tagcode f1 /* field indices for column 0 and 1 */
){
	/* Two-column printout if things match, dumb printout otherwise. */
	if(  tag[f0].fill         && tag[f1].fill
	  && len[f0] <= (size_t)climit[0] && len[f1] <= (size_t)climit[1] )
	{
		char cfmt[35]; /* "%s:%-XXXs%-XXXs  %s:%-XXXs%-XXXs\n" plus one extra null from snprintf */
		int chardiff[2];
		size_t bytelen;

		/* difference between character length and byte length */
		bytelen = strlen(tag[f0].p);
		chardiff[0] = len[f0] < bytelen ? bytelen-len[f0] : 0;
		bytelen = strlen(tag[f1].p);
		chardiff[1] = len[f1] < bytelen ? bytelen-len[f1] : 0;

		/* Two-column format string with added padding for multibyte chars. */
		snprintf( cfmt, sizeof(cfmt)-1, "%%s:%%-%ds%%-%ds  %%s:%%-%ds%%-%ds\n"
		,	1+namelen[0]-(int)strlen(name[f0]), climit[0]+chardiff[0]
		,	1+namelen[1]-(int)strlen(name[f1]), climit[1]+chardiff[1] );
		/* Actual printout of name and value pairs. */
		fprintf(out, cfmt, name[f0], " ", tag[f0].p, name[f1], " ", tag[f1].p);
	}
	else
	{
		print_oneline(out, tag, f0, FALSE);
		print_oneline(out, tag, f1, FALSE);
	}
}

/* Print tags... limiting the UTF-8 to ASCII, if necessary. */
void print_id3_tag(mpg123_handle *mh, int long_id3, FILE *out)
{
	char genre_from_v1 = 0;
	enum tagcode ti;
	mpg123_string tag[FIELDS];
	size_t len[FIELDS];
	mpg123_id3v1 *v1;
	mpg123_id3v2 *v2;
	/* no memory allocated here, so return is safe */
	for(ti=0; ti<FIELDS; ++ti){ len[ti]=0; mpg123_init_string(&tag[ti]); }
	/* extract the data */
	mpg123_id3(mh, &v1, &v2);
	/* Only work if something there... */
	if(v1 == NULL && v2 == NULL) return;
	if(v2 != NULL) /* fill from ID3v2 data */
	{
		len[TITLE]   = transform(&tag[TITLE],   v2->title);
		len[ARTIST]  = transform(&tag[ARTIST],  v2->artist);
		len[ALBUM]   = transform(&tag[ALBUM],   v2->album);
		len[COMMENT] = transform(&tag[COMMENT], v2->comment);
		len[YEAR]    = transform(&tag[YEAR],    v2->year);
		len[GENRE]   = transform(&tag[GENRE],   v2->genre);
	}
	if(v1 != NULL) /* fill gaps with ID3v1 data */
	{
		/* I _could_ skip the recalculation of fill ... */
		id3_gap(&tag[TITLE],   30, v1->title,   &len[TITLE]);
		id3_gap(&tag[ARTIST],  30, v1->artist,  &len[ARTIST]);
		id3_gap(&tag[ALBUM],   30, v1->album,   &len[ALBUM]);
		id3_gap(&tag[COMMENT], 30, v1->comment, &len[COMMENT]);
		id3_gap(&tag[YEAR],    4,  v1->year,    &len[YEAR]);
		/*
			genre is special... v1->genre holds an index, id3v2 genre may contain indices in textual form and raw textual genres...
		*/
		if(!tag[GENRE].fill)
		{
			if(tag[GENRE].size >= 31 || mpg123_resize_string(&tag[GENRE],31))
			{
				if(v1->genre <= genre_count)
				{
					strncpy(tag[GENRE].p, genre_table[v1->genre], 30);
				}
				else
				{
					strncpy(tag[GENRE].p,"Unknown",30);
				}
				tag[GENRE].p[30] = 0;
				/* V1 was plain ASCII, so strlen is fine. */
				len[GENRE] = strlen(tag[GENRE].p);
				tag[GENRE].fill = len[GENRE] + 1;
				genre_from_v1 = 1;
			}
		}
	}

	if(tag[GENRE].fill && !genre_from_v1)
	{
		/*
			id3v2.3 says (id)(id)blabla and in case you want ot have (blabla) write ((blabla)
			also, there is
			(RX) Remix
			(CR) Cover
			id3v2.4 says
			"one or several of the ID3v1 types as numerical strings"
			or define your own (write strings), RX and CR 

			Now I am very sure that I'll encounter hellishly mixed up id3v2 frames, so try to parse both at once.
		*/
		mpg123_string tmp;
		mpg123_init_string(&tmp);
		debug1("interpreting genre: %s\n", tag[GENRE].p);
		if(mpg123_copy_string(&tag[GENRE], &tmp))
		{
			size_t num = 0;
			size_t nonum = 0;
			size_t i;
			enum { nothing, number, outtahere } state = nothing;
			tag[GENRE].fill = 0; /* going to be refilled */
			/* number\n -> id3v1 genre */
			/* (number) -> id3v1 genre */
			/* (( -> ( */
			for(i = 0; i < tmp.fill; ++i)
			{
				debug1("i=%lu", (unsigned long) i);
				switch(state)
				{
					case nothing:
						nonum = i;
						if(tmp.p[i] == '(')
						{
							num = i+1; /* number starting as next? */
							state = number;
							debug1("( before number at %lu?", (unsigned long) num);
						}
						/* you know an encoding where this doesn't work? */
						else if(tmp.p[i] >= '0' && tmp.p[i] <= '9')
						{
							num = i;
							state = number;
							debug1("direct number at %lu", (unsigned long) num);
						}
						else state = outtahere;
					break;
					case number:
						/* fake number alert: (( -> ( */
						if(tmp.p[i] == '(')
						{
							nonum = i;
							state = outtahere;
							debug("no, it was ((");
						}
						else if(tmp.p[i] == ')' || tmp.p[i] == '\n' || tmp.p[i] == 0)
						{
							if(i-num > 0)
							{
								/* we really have a number */
								int gid;
								char* genre = "Unknown";
								tmp.p[i] = 0;
								gid = atoi(tmp.p+num);

								/* get that genre */
								if(gid >= 0 && gid <= genre_count) genre = genre_table[gid];
								debug1("found genre: %s", genre);

								if(tag[GENRE].fill) mpg123_add_string(&tag[GENRE], ", ");
								mpg123_add_string(&tag[GENRE], genre);
								nonum = i+1; /* next possible stuff */
								state = nothing;
								debug1("had a number: %i", gid);
							}
							else
							{
								/* wasn't a number, nonum is set */
								state = outtahere;
								debug("no (num) thing...");
							}
						}
						else if(!(tmp.p[i] >= '0' && tmp.p[i] <= '9'))
						{
							/* no number at last... */
							state = outtahere;
							debug("nothing numeric here");
						}
						else
						{
							debug("still number...");
						}
					break;
					default: break;
				}
				if(state == outtahere) break;
			}
			/* Small hack: Avoid repeating genre in case of stuff like
			   (144)Thrash Metal being given. The simple cases. */
			if(
				nonum < tmp.fill-1 &&
				(!tag[GENRE].fill || strncmp(tag[GENRE].p, tmp.p+nonum, tag[GENRE].fill))
			)
			{
				if(tag[GENRE].fill) mpg123_add_string(&tag[GENRE], ", ");
				mpg123_add_string(&tag[GENRE], tmp.p+nonum);
			}
			/* Do not like that ... assumes plain ASCII ... */
			len[GENRE] = strlen(tag[GENRE].p);
		}
		mpg123_free_string(&tmp);
	}

	if(long_id3)
	{
		fprintf(out,"\n");
		/* print id3v2 */
		print_oneline(out, tag, TITLE,   TRUE);
		print_oneline(out, tag, ARTIST,  TRUE);
		print_oneline(out, tag, ALBUM,   TRUE);
		print_oneline(out, tag, YEAR,    TRUE);
		print_oneline(out, tag, GENRE,   TRUE);
		print_oneline(out, tag, COMMENT, TRUE);
		fprintf(out,"\n");
	}
	else
	{
		/* We are trying to be smart here and conserve some vertical space.
		   So we will skip tags not set, and try to show them in two parallel
		   columns if they are short, which is by far the most common case. */
		int linelimit;
		int climit[2];

		/* Adapt formatting width to terminal if possible. */
		linelimit = term_width(fileno(out));
		if(linelimit < 0)
			linelimit=overhead[0]+30+overhead[1]+30; /* the old style, based on ID3v1 */
		if(linelimit > 200)
			linelimit = 200; /* Not too wide. Also for format string safety. */
		/* Divide the space between the two columns, not wasting any. */
		climit[1] = linelimit/2-overhead[0];
		climit[0] = linelimit-linelimit/2-overhead[1];
		debug3("linelimits: %i  < %i | %i >", linelimit, climit[0], climit[1]);

		if(climit[0] <= 0 || climit[1] <= 0)
		{
			/* Ensure disabled column printing, no play with signedness in comparisons. */
			climit[0] = 0;
			climit[1] = 0;
		}
		fprintf(out,"\n"); /* Still use one separator line. Too ugly without. */
		print_pair(out, climit, tag, len, TITLE,   ARTIST);
		print_pair(out, climit, tag, len, COMMENT, ALBUM );
		print_pair(out, climit, tag, len, YEAR,    GENRE );
	}
	for(ti=0; ti<FIELDS; ++ti) mpg123_free_string(&tag[ti]);

	if(v2 != NULL && APPFLAG(MPG123APP_LYRICS))
	{
		/* find and print texts that have USLT IDs */
		size_t i;
		for(i=0; i<v2->texts; ++i)
		{
			if(!memcmp(v2->text[i].id, "USLT", 4))
			{
				/* split into lines, ensure usage of proper local line end */
				size_t a=0;
				size_t b=0;
				char lang[4]; /* just a 3-letter ASCII code, no fancy encoding */
				mpg123_string innline;
				mpg123_string outline;
				mpg123_string *uslt = &v2->text[i].text;

				memcpy(lang, &v2->text[i].lang, 3);
				lang[3] = 0;
				printf("Lyrics begin, language: %s; %s\n\n", lang,  v2->text[i].description.fill ? v2->text[i].description.p : "");

				mpg123_init_string(&innline);
				mpg123_init_string(&outline);
				while(a < uslt->fill)
				{
					b = a;
					while(b < uslt->fill && uslt->p[b] != '\n' && uslt->p[b] != '\r') ++b;
					/* Either found end of a line or end of the string (null byte) */
					mpg123_set_substring(&innline, uslt->p, a, b-a);
					transform(&outline, &innline);
					printf(" %s\n", outline.p);

					if(uslt->p[b] == uslt->fill) break; /* nothing more */

					/* Swallow CRLF */
					if(uslt->fill-b > 1 && uslt->p[b] == '\r' && uslt->p[b+1] == '\n') ++b;

					a = b + 1; /* next line beginning */
				}
				mpg123_free_string(&innline);
				mpg123_free_string(&outline);

				printf("\nLyrics end.\n");
			}
		}
	}
}

void print_icy(mpg123_handle *mh, FILE *outstream)
{
	char* icy;
	if(MPG123_OK == mpg123_icy(mh, &icy))
	{
		mpg123_string in;
		mpg123_init_string(&in);
		if(mpg123_store_utf8(&in, mpg123_text_icy, (unsigned char*)icy, strlen(icy)+1))
		{
			mpg123_string out;
			mpg123_init_string(&out);

			transform(&out, &in);
			if(out.fill)
			fprintf(outstream, "\nICY-META: %s\n", out.p);

			mpg123_free_string(&out);
		}
		mpg123_free_string(&in);
	}
}

static void utf8_ascii(mpg123_string *dest, mpg123_string *source)
{
	size_t spos = 0;
	size_t dlen = 0;
	char *p;

	/* Find length of ASCII string (count non-continuation bytes).
	   Do _not_ change this to mpg123_strlen()!
	   It needs to match the loop below. Especially dlen should not stop at embedded null bytes. You can get any trash from ID3! */
	for(spos=0; spos < source->fill; ++spos)
	if((source->p[spos] & 0xc0) == 0x80) continue;
	else ++dlen;

	/* The trailing zero is included in dlen; if there is none, one character will be cut. Bad input -> bad output. */
	if(!mpg123_resize_string(dest, dlen)){ mpg123_free_string(dest); return; }
	/* Just ASCII, we take it easy. */
	p = dest->p;

	for(spos=0; spos < source->fill; ++spos)
	{
		/* UTF-8 continuation byte 0x10?????? */
		if((source->p[spos] & 0xc0) == 0x80) continue;
		/* UTF-8 lead byte 0x11?????? */
		else if(source->p[spos] & 0x80) *p = joker_symbol;
		/* just ASCII, 0x0??????? */
		else *p = source->p[spos];

		++p; /* next output char */
	}
	/* Always close the string, trailing zero might be missing. */
	if(dest->size) dest->p[dest->size-1] = 0;

	dest->fill = dest->size;
}

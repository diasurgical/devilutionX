/* Just printing out ID3 tags with plain data from libmpg123 and explicitly called conversion routine. */

#include "compat.h"
#include <mpg123.h>
#include "debug.h"

int test_whence(const char* path, int scan_before)
{
	int err = MPG123_OK;
	mpg123_handle* mh = NULL;
	off_t length, pos;

	mh = mpg123_new(NULL, &err );
	if(mh == NULL) return -1;

	err = mpg123_open(mh, path );
	if(err != MPG123_OK) return -1;

	if(scan_before) mpg123_scan(mh);

	pos = mpg123_seek( mh, 0, SEEK_END);
	if(pos < 0){ error1("seek failed: %s", mpg123_strerror(mh)); return -1; }

	pos = mpg123_tell(mh);
	length = mpg123_length(mh);

	/* Later: Read samples and compare different whence values with identical seek positions. */

	mpg123_close(mh);
	mpg123_delete(mh);

	fprintf(stdout, "length %"OFF_P" vs. pos %"OFF_P"\n", length, pos);

	return (pos == length) ? 0 : -1;
}

void print_field(const char *name, mpg123_string *sb)
{
	const unsigned char *sbp = (unsigned char*)sb->p;
	enum mpg123_text_encoding enc;
	mpg123_string printer;
	mpg123_init_string(&printer);
	printf("\n=== %s ===\n", name);
	if(sb->fill == 0)
	{
		printf("Oh, empty. Totally.\n");
		return;
	}

	enc = mpg123_enc_from_id3(sbp[0]);

	printf("From encoding: %i (in ID3: %i)\n", enc, (int)sbp[0]) ;
	if(mpg123_store_utf8(&printer, enc, sbp+1, sb->fill-1))
	{
		/* Not caring for multiple strings separated via null bytes here. */
		printf("Value: %s\n", printer.p);
	}
	else error("Conversion failed!");
	mpg123_free_string(&printer);
}


int main(int argc, char **argv)
{
	int err;
	int ret = 0;
	mpg123_handle *mh;
	mpg123_id3v2 *id3;

	if(argc < 2)
	{
		printf("Gimme a MPEG file name...\n");
		return 0;
	}

	mpg123_init();
	mh = mpg123_new(NULL, &err);
	if(err != MPG123_OK) goto badend;

	mpg123_param(mh, MPG123_ADD_FLAGS, MPG123_PLAIN_ID3TEXT, 0.);

	err = mpg123_open(mh, argv[1]);
	if(err != MPG123_OK) goto badend;

	err = mpg123_scan(mh);
	if(err != MPG123_OK) goto badend;

	err = mpg123_id3(mh, NULL, &id3);
	if(err != MPG123_OK) goto badend;

	if(id3 == NULL)
	{
		error("No ID3 data found.");
		goto badend;
	}

	print_field("artist", id3->artist);
	print_field("title", id3->title);
	print_field("album", id3->album);
	print_field("comment", id3->comment);

	goto end;
badend:
	ret = -1;
end:
	mpg123_delete(mh);
	mpg123_exit();
	return ret;
}

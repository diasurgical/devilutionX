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


int main(int argc, char **argv)
{
	int err = 0, errsum = 0;
	if(argc < 2)
	{
		printf("Gimme a MPEG file name...\n");
		return 0;
	}
	mpg123_init();
	fprintf(stderr, "End seek without (explicit) scanning: ");
	err = test_whence(argv[1], 0);
	fprintf(stdout, "%s\n", err == 0 ? "PASS" : "FAIL");
	errsum += err;
	fprintf(stderr, "End seek with explicit scanning: ");
	err = test_whence(argv[1], 1);
	fprintf(stdout, "%s\n", err == 0 ? "PASS" : "FAIL");
	errsum += err;
	mpg123_exit();
	printf("%s\n", errsum ? "FAIL" : "PASS");
	return errsum;
}

#include "config.h"
#include "compat.h"
#include "dither.h"
#include "debug.h"

/* Directly include the code for testing, avoiding
   build of same object with and without libtool. */
#include "../libmpg123/dither_impl.h"

const char *typenames[] = { "white", "tpdf", "highpass_tpdf" };
enum mpg123_noise_type types[] = { mpg123_white_noise, mpg123_tpdf_noise, mpg123_highpass_tpdf_noise };

int main(int argc, char **argv)
{
	size_t i;
	size_t count = DITHERSIZE;
	float *table;
	enum mpg123_noise_type type = mpg123_highpass_tpdf_noise;

	fprintf(stderr, "Note: You could give two optional arguments: noise type and table size (number count).\n");
	if(argc > 1)
	{
		const char *typename = argv[1];
		for(i=0; i<sizeof(typenames)/sizeof(char*); ++i)
		if(strcmp(typename, typenames[i]) == 0)
		{
			type = types[i];
			break;
		}

		if(i == sizeof(typenames)/sizeof(char*))
		{
			error("Unknown noise type!");
			return -1;
		}
	}
	if(argc > 2)
	{
		count = (size_t) atol(argv[2]);
	}

	table = malloc(sizeof(float)*count);
	if(table == NULL)
	{
		error("Cannot allocate memory for noise table!");
		return -2;
	}

	mpg123_noise(table, count, type);
	for(i=0; i<count; ++i)
	printf("%g\n", table[i]);

	free(table);

	return 0;
}

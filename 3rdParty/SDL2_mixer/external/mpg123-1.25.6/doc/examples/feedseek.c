/*
	feedseek: test program for libmpg123, showing how to use fuzzy seeking in feeder mode
	copyright 2008 by the mpg123 project - free software under the terms of the LGPL 2.1
	see COPYING and AUTHORS files in distribution or http://mpg123.org	
*/

#include <mpg123.h>
#include <stdio.h>

#define INBUFF  16384 * 2 * 2
#define WAVE_FORMAT_PCM 0x0001
#define WAVE_FORMAT_IEEE_FLOAT 0x0003

FILE *out;
size_t totaloffset, dataoffset;
long rate;
int channels, enc;
unsigned short bitspersample, wavformat;

// write wav header
void initwav()
{
	unsigned int tmp32 = 0;
	unsigned short tmp16 = 0;

	fwrite("RIFF", 1, 4, out);
	totaloffset = ftell(out);

	fwrite(&tmp32, 1, 4, out); // total size
	fwrite("WAVE", 1, 4, out);
	fwrite("fmt ", 1, 4, out);
	tmp32 = 16;
	fwrite(&tmp32, 1, 4, out); // format length
	tmp16 = wavformat;
	fwrite(&tmp16, 1, 2, out); // format
	tmp16 = channels;
	fwrite(&tmp16, 1, 2, out); // channels
	tmp32 = rate;
	fwrite(&tmp32, 1, 4, out); // sample rate
	tmp32 = rate * bitspersample/8 * channels;
	fwrite(&tmp32, 1, 4, out); // bytes / second
	tmp16 = bitspersample/8 * channels; // float 16 or signed int 16
	fwrite(&tmp16, 1, 2, out); // block align
	tmp16 = bitspersample;
	fwrite(&tmp16, 1, 2, out); // bits per sample
	fwrite("data ", 1, 4, out);
	tmp32 = 0;
	dataoffset = ftell(out);
	fwrite(&tmp32, 1, 4, out); // data length
}

// rewrite wav header with final length infos
void closewav()
{
	unsigned int tmp32 = 0;
	unsigned short tmp16 = 0;

	long total = ftell(out);
	fseek(out, totaloffset, SEEK_SET);
	tmp32 = total - (totaloffset + 4);
	fwrite(&tmp32, 1, 4, out);
	fseek(out, dataoffset, SEEK_SET);
	tmp32 = total - (dataoffset + 4);

	fwrite(&tmp32, 1, 4, out);
}

// determine correct wav format and bits per sample
// from mpg123 enc value
void initwavformat()
{
	if(enc & MPG123_ENC_FLOAT_64)
	{
		bitspersample = 64;
		wavformat = WAVE_FORMAT_IEEE_FLOAT;
	}
	else if(enc & MPG123_ENC_FLOAT_32)
	{
		bitspersample = 32;
		wavformat = WAVE_FORMAT_IEEE_FLOAT;
	}
	else if(enc & MPG123_ENC_16)
	{
		bitspersample = 16;
		wavformat = WAVE_FORMAT_PCM;
	}
	else
	{
		bitspersample = 8;
		wavformat = WAVE_FORMAT_PCM;
	}
}

int main(int argc, char **argv)
{
	unsigned char buf[INBUFF];
	unsigned char *audio;
	FILE *in;
	mpg123_handle *m;
	int ret, state;
	size_t inc, outc;
	off_t len, num;
	size_t bytes;
	off_t inoffset;
	inc = outc = 0;

	if(argc < 3)
	{
		fprintf(stderr,"Please supply in and out filenames\n");
		return -1;
	}

	mpg123_init();

	m = mpg123_new(NULL, &ret);
	if(m == NULL)
	{
		fprintf(stderr,"Unable to create mpg123 handle: %s\n", mpg123_plain_strerror(ret));
		return -1;
	}

	mpg123_param(m, MPG123_VERBOSE, 2, 0);

	ret = mpg123_param(m, MPG123_FLAGS, MPG123_FUZZY | MPG123_SEEKBUFFER | MPG123_GAPLESS, 0);
	if(ret != MPG123_OK)
	{
		fprintf(stderr,"Unable to set library options: %s\n", mpg123_plain_strerror(ret));
		return -1;
	}

	// Let the seek index auto-grow and contain an entry for every frame
	ret = mpg123_param(m, MPG123_INDEX_SIZE, -1, 0);
	if(ret != MPG123_OK)
	{
		fprintf(stderr,"Unable to set index size: %s\n", mpg123_plain_strerror(ret));
		return -1;
	}

	ret = mpg123_format_none(m);
	if(ret != MPG123_OK)
	{
		fprintf(stderr,"Unable to disable all output formats: %s\n", mpg123_plain_strerror(ret));
		return -1;
	}
	
	// Use float output
	ret = mpg123_format(m, 44100, MPG123_MONO | MPG123_STEREO,  MPG123_ENC_FLOAT_32);
	if(ret != MPG123_OK)
	{
		fprintf(stderr,"Unable to set float output formats: %s\n", mpg123_plain_strerror(ret));
		return -1;
	}

	ret = mpg123_open_feed(m);
	if(ret != MPG123_OK)
	{
		fprintf(stderr,"Unable open feed: %s\n", mpg123_plain_strerror(ret));
		return -1;
	}

	in = fopen(argv[1], "rb");
	if(in == NULL)
	{
		fprintf(stderr,"Unable to open input file %s\n", argv[1]);
		return -1;
	}
	
	out = fopen(argv[2], "wb");
	if(out == NULL)
	{
		fclose(in);
		fprintf(stderr,"Unable to open output file %s\n", argv[2]);
		return -1;
	}

	fprintf(stderr, "Seeking...\n");
	/* That condition is tricky... parentheses are crucial... */
	while((ret = mpg123_feedseek(m, 95000, SEEK_SET, &inoffset)) == MPG123_NEED_MORE)
	{
		len = fread(buf, sizeof(unsigned char), INBUFF, in);
		if(len <= 0)
			break;
		inc += len;

		state = mpg123_feed(m, buf, len);
		if(state == MPG123_ERR)
		{
			fprintf(stderr, "Error: %s", mpg123_strerror(m));
			return -1; 
		}
	}
	if(ret == MPG123_ERR)
	{
		fprintf(stderr, "Feedseek failed: %s\n", mpg123_strerror(m));
		return -1;
	}

	fseek(in, inoffset, SEEK_SET);	
	
	fprintf(stderr, "Starting decode...\n");
	while(1)
	{
		len = fread(buf, sizeof(unsigned char), INBUFF, in);
		if(len <= 0)
			break;
		inc += len;
		ret = mpg123_feed(m, buf, len);

		while(ret != MPG123_ERR && ret != MPG123_NEED_MORE)
		{
			ret = mpg123_decode_frame(m, &num, &audio, &bytes);
			if(ret == MPG123_NEW_FORMAT)
			{
				mpg123_getformat(m, &rate, &channels, &enc);
				initwavformat();
				initwav();
				fprintf(stderr, "New format: %li Hz, %i channels, encoding value %i\n", rate, channels, enc);
			}
			fwrite(audio, sizeof(unsigned char), bytes, out);
			outc += bytes;
		}

		if(ret == MPG123_ERR)
		{
			fprintf(stderr, "Error: %s", mpg123_strerror(m));
			break; 
		}
	}

	fprintf(stderr, "Finished\n", (unsigned long)inc, (unsigned long)outc);

	closewav();
	fclose(out);
	fclose(in);
	mpg123_delete(m);
	mpg123_exit();
	return 0;
}

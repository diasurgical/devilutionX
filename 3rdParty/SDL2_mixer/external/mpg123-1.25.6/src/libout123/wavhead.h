/*
	wavhead.h: wav file header, to be included twice for integer and float wavs

	copyright ?-2015 by the mpg123 project - free software under the terms of the LGPL 2.1
	see COPYING and AUTHORS files in distribution or http://mpg123.org
	initially written by Samuel Audet
*/

struct RIFF_STRUCT_NAME
{
	byte riffheader[4];
	byte WAVElen[4]; /* should this include riffheader or not? */
	struct
	{
		byte WAVEID[4];
		byte fmtheader[4];
		byte fmtlen[4];
		struct
		{
			byte FormatTag[2];
			byte Channels[2];
			byte SamplesPerSec[4];
			byte AvgBytesPerSec[4];
			byte BlockAlign[2];
			byte BitsPerSample[2]; /* format specific for PCM */
			#ifdef FLOATOUT
			byte cbSize[2];
			#endif
		} fmt;
		#ifdef FLOATOUT
		byte factheader[4];
		byte factlen[4];
		struct
		{
			byte samplelen[4];
		} fact;
		#endif
		struct
		{
			byte dataheader[4];
			byte datalen[4];
			/* from here you insert your PCM data */
		} data;
	} WAVE;
} const RIFF_NAME = 
{
	{ 'R','I','F','F' } ,
	{ sizeof(RIFF_NAME.WAVE),0,0,0 } , 
	{
		{ 'W','A','V','E' },
		{ 'f','m','t',' ' },
		{ sizeof(RIFF_NAME.WAVE.fmt),0,0,0 } ,
		{
			{WAVE_FORMAT,0} , {0,0},{0,0,0,0},{0,0,0,0},{0,0},{0,0}
			#ifdef FLOATOUT
			,{0,0}
			#endif
		} ,
		#ifdef FLOATOUT
		{ 'f','a','c','t' },
		{ sizeof(RIFF_NAME.WAVE.fact),0,0,0 },
		{
			{0,0,0,0} /* to be filled later, like datalen and wavelen */
		},
		#endif
		{ { 'd','a','t','a' }  , {0,0,0,0} }
	}
};

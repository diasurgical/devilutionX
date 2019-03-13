/*
	mpg123clr: MPEG Audio Decoder library Common Language Runtime version.

	copyright 2009 by Malcolm Boczek - free software under the terms of the LGPL 2.1
	mpg123clr.dll is a derivative work of libmpg123 - all original mpg123 licensing terms apply.

	All rights to this work freely assigned to the mpg123 project.
*/
/*
	libmpg123: MPEG Audio Decoder library

	copyright 1995-2011 by the mpg123 project - free software under the terms of the LGPL 2.1
	see COPYING and AUTHORS files in distribution or http://mpg123.org

*/
/*
	1.8.1.0	04-Aug-09	Initial release.
	1.10.0.0 30-Nov-09	release match - added mpg123_feature (mb)
	1.13.0.0 13-Jan-11	release match (mb)
*/

#pragma once

#pragma warning(disable : 4635)
#include "mpg123.h"
#pragma warning(default : 4635)


namespace mpg123clr
{
	///<summary>Mpg123 enumerations.</summary>
	namespace mpg
	{

		///<summary>Enumeration of the parameters types that it is possible to set/get.</summary>
		public enum class parms
		{
			verbose = MPG123_VERBOSE,				/// set verbosity value for enabling messages to stderr, >= 0 makes sense (integer)
			flags = MPG123_FLAGS,					/// set all flags, p.ex val = MPG123_GAPLESS|MPG123_MONO_MIX (integer)
			add_flags = MPG123_ADD_FLAGS,			/// add some flags (integer)
			force_rate = MPG123_FORCE_RATE,			/// when value > 0, force output rate to that value (integer)
			down_sample = MPG123_DOWN_SAMPLE,		/// 0=native rate, 1=half rate, 2=quarter rate (integer)
			rva = MPG123_RVA,						/// one of the RVA choices above (integer)
			downspeed = MPG123_DOWNSPEED,			/// play a frame N times (integer)
			upspeed = MPG123_UPSPEED,				/// play every Nth frame (integer)
			start_frame = MPG123_START_FRAME,		/// start with this frame (skip frames before that, integer)
			decode_frames = MPG123_DECODE_FRAMES,	/// decode only this number of frames (integer)
			icy_interval = MPG123_ICY_INTERVAL,		/// stream contains ICY metadata with this interval (integer)
			outscale = MPG123_OUTSCALE,				/// the scale for output samples (amplitude - integer or float according to mpg123 output format, normally integer)
			timeout = MPG123_TIMEOUT,				/// timeout for reading from a stream (not supported on win32, integer)
			remove_flags = MPG123_REMOVE_FLAGS,		/// remove some flags (inverse of MPG123_ADD_FLAGS, integer)
			resync_limit = MPG123_RESYNC_LIMIT,		/// Try resync on frame parsing for that many bytes or until end of stream (<0 ... integer).
			index_size = MPG123_INDEX_SIZE,			/// Set the frame index size (if supported). Values <0 mean that the index is allowed to grow dynamically in these steps (in positive direction, of course) -- Use this when you really want a full index with every individual frame.
			preframes = MPG123_PREFRAMES			/// Decode/ignore that many frames in advance for layer 3. This is needed to fill bit reservoir after seeking, for example (but also at least one frame in advance is needed to have all "normal" data for layer 3). Give a positive integer value, please.

		};


		///<summary>Parameter flag bits.</summary>
		///<remarks>Equivalent to MPG123_FLAGS, use the usual binary or ( | ) to combine.</remarks>
		public enum class param_flags
		{
			force_mono = MPG123_FORCE_MONO,			///     0111 Force some mono mode: This is a test bitmask for seeing if any mono forcing is active. 
			mono_left = MPG123_MONO_LEFT,			///     0001 Force playback of left channel only.  
			mono_right = MPG123_MONO_RIGHT,			///     0010 Force playback of right channel only. 
			mono_mix = MPG123_MONO_MIX,				///     0100 Force playback of mixed mono.         
			force_stereo = MPG123_FORCE_STEREO,		///     1000 Force stereo output.                  
			force_8bit = MPG123_FORCE_8BIT,			/// 00010000 Force 8bit formats.                   
			quiet = MPG123_QUIET,					/// 00100000 Suppress any printouts (overrules verbose).                    
			gapless = MPG123_GAPLESS,				/// 01000000 Enable gapless decoding (default on if libmpg123 has support). 
			no_resync = MPG123_NO_RESYNC,			/// 10000000 Disable resync stream after error.                             
			seekbuffer = MPG123_SEEKBUFFER,			/// 000100000000 Enable small buffer on non-seekable streams to allow some peek-ahead (for better MPEG sync). 
			fuzzy = MPG123_FUZZY,					/// 001000000000 Enable fuzzy seeks (guessing byte offsets or using approximate seek points from Xing TOC) 
			force_float = MPG123_FORCE_FLOAT,		/// 010000000000 Force floating point output (32 or 64 bits depends on mpg123 internal precision). 

			///<summary>Prevent ID3 text translation to UTF-8.
			///<para>NOTE: Do not set if you intend to use CLR id3v2 objects.</para>
			///</summary>
			plain_id3text = MPG123_PLAIN_ID3TEXT,	/// 100000000000 Do not translate ID3 text data to UTF-8. ID3 strings will contain the raw text data, with the first byte containing the ID3 encoding code.

			ignore_streamlength = MPG123_IGNORE_STREAMLENGTH,	/// 1000000000000 Ignore any stream length information contained in the stream, which can be contained in a 'TLEN' frame of an ID3v2 tag or a Xing tag.

			// 1.13.0.0
			skip_id3v2 = MPG123_SKIP_ID3V2,			/// 10 0000 0000 0000 Do not parse ID3v2 tags, just skip them.
		};

		///<summary>RVA enumeration.</summary>
		///<remarks>Equivalent to MPG123_RVA.</remarks>
		public enum class rva
		{
			rva_off   = MPG123_RVA_OFF,		/// RVA disabled (default).   
			rva_mix   = MPG123_RVA_MIX,		/// Use mix/track/radio gain. 
			rva_album = MPG123_RVA_ALBUM,	/// Use album/audiophile gain 
			rva_max   = MPG123_RVA_ALBUM,	/// The maximum RVA code, may increase in future. 
		};

		///<summary>Feature set available for query with mpg123_feature. </summary>
		///<remarks>Equivalent to MPG123_FEATURE_SET.</remarks>
		public enum class feature_set
		{
			feature_abi_utf8open = MPG123_FEATURE_ABI_UTF8OPEN,				/// mpg123 expects path names to be given in UTF-8 encoding instead of plain native.
			feature_output_8bit = MPG123_FEATURE_OUTPUT_8BIT,				/// 8bit output  
			feature_output_16bit = MPG123_FEATURE_OUTPUT_16BIT,				/// 16bit output
			feature_output_32bit = MPG123_FEATURE_OUTPUT_32BIT,				/// 32bit output
			feature_index = MPG123_FEATURE_INDEX,							/// support for building a frame index for accurate seeking
			feature_parse_id3v2 = MPG123_FEATURE_PARSE_ID3V2,				/// id3v2 parsing
			feature_decode_layer1 = MPG123_FEATURE_DECODE_LAYER1,			/// mpeg layer-1 decoder enabled
			feature_decode_layer2 = MPG123_FEATURE_DECODE_LAYER2,			/// mpeg layer-2 decoder enabled
			feature_decode_layer3 = MPG123_FEATURE_DECODE_LAYER3,			/// mpeg layer-3 decoder enabled
			feature_decode_accurate = MPG123_FEATURE_DECODE_ACCURATE,		/// accurate decoder rounding
			feature_decode_downsample = MPG123_FEATURE_DECODE_DOWNSAMPLE,	/// downsample (sample omit)
			feature_decode_ntom = MPG123_FEATURE_DECODE_NTOM,				/// flexible rate decoding
			feature_parse_icy = MPG123_FEATURE_PARSE_ICY,					/// ICY support
			feature_timeout_read = MPG123_FEATURE_TIMEOUT_READ,				/// Reader with timeout (network)
		};



		///<summary>An enum over all sample types possibly known to mpg123.</summary>
		///<remarks><para>The values are designed as bit flags to allow bitmasking for encoding families.</para>
		///
		///<para>Note that (your build of) libmpg123 does not necessarily support all these.
		/// Usually, you can expect the 8bit encodings and signed 16 bit.
		/// Also 32bit float will be usual beginning with mpg123-1.7.0 .</para>
		///
		///<para>What you should bear in mind is that (SSE, etc) optimized routines may be absent
		/// for some formats. We do have SSE for 16, 32 bit and float, though.
		/// 24 bit integer is done via postprocessing of 32 bit output -- just cutting
		/// the last byte, no rounding, even. If you want better, do it yourself.</para>
		///
		///<para>All formats are in native byte order. On a little endian machine this should mean
		/// that you can just feed the MPG123_ENC_SIGNED_32 data to common 24bit hardware that
		/// ignores the lowest byte (or you could choose to do rounding with these lower bits).</para>
		///</remarks>
		public enum class enc
		{
			enc_8			= MPG123_ENC_8,				/// 0000 0000 1111 Some 8 bit  integer encoding. 
			enc_16			= MPG123_ENC_16,			/// 0000 0100 0000 Some 16 bit integer encoding.
			enc_24			= MPG123_ENC_24,			/// 0100 0000 0000 0000 Some 24 bit integer encoding. (r1.13)
			enc_32			= MPG123_ENC_32,			/// 0001 0000 0000 Some 32 bit integer encoding.
			enc_signed		= MPG123_ENC_SIGNED,		/// 0000 1000 0000 Some signed integer encoding.
			enc_float		= MPG123_ENC_FLOAT,			/// 1110 0000 0000 Some float encoding.
			enc_signed_16   = MPG123_ENC_SIGNED_16,		///           1101 0000 signed 16 bit
			enc_unsigned_16 = MPG123_ENC_UNSIGNED_16,	///           0110 0000 unsigned 16 bit
			enc_unsigned_8  = MPG123_ENC_UNSIGNED_8,    ///           0000 0001 unsigned 8 bit
			enc_signed_8    = MPG123_ENC_SIGNED_8,      ///           1000 0010 signed 8 bit
			enc_ulaw_8		= MPG123_ENC_ULAW_8,		///           0000 0100 ulaw 8 bit
			enc_alaw_8		= MPG123_ENC_ALAW_8,		///           0000 1000 alaw 8 bit
			enc_signed_32   = MPG123_ENC_SIGNED_32,		/// 0001 0001 1000 0000 signed 32 bit
			enc_unsigned_32 = MPG123_ENC_UNSIGNED_32,   /// 0010 0001 0000 0000 unsigned 32 bit
			enc_signed_24	= MPG123_ENC_SIGNED_24,		/// 0101 0000 1000 0000 signed 24 bit (r1.13)
			enc_unsigned_24	= MPG123_ENC_UNSIGNED_24,	/// 0110 0000 0000 0000 unsigned 24 bit (r1.13)
			enc_float_32    = MPG123_ENC_FLOAT_32,      ///      0010 0000 0000 32bit float
			enc_float_64    = MPG123_ENC_FLOAT_64,      ///      0100 0000 0000 64bit float
			enc_any			= MPG123_ENC_ANY,			/// any encoding
		};


		///<summary>Channel count enumeration</summary>
		///<remarks>clr added <cref name="both">both</cref></remarks>
		public enum class channelcount
		{
			mono   = MPG123_MONO,
			stereo = MPG123_STEREO,
			both   = MPG123_MONO | MPG123_STEREO,
		};

		///<summary>Channel enumeration.</summary>
		public enum class channels
		{
			left	= MPG123_LEFT,	/// The Left Channel. 
			right	= MPG123_RIGHT,	/// The Right Channel. 
			both	= MPG123_LR,	/// Both left and right channel; same as MPG123_LEFT|MPG123_RIGHT 
		};

		///<summary>VBR enumeration.</summary>
		public enum class mpeg_vbr
		{
			cbr = MPG123_CBR,		/// Constant Bitrate Mode (default) 
			vbr = MPG123_VBR,		/// Variable Bitrate Mode 
			abr = MPG123_ABR,		/// Average Bitrate Mode 
		};

		///<summary>MPEG Version enumeration.</summary>
		public enum class mpeg_version
		{
			mpeg_1_0 = MPG123_1_0,	/// MPEG Version 1.0 
			mpeg_2_0 = MPG123_2_0,	/// MPEG Version 2.0 
			mpeg_2_5 = MPG123_2_5,	/// MPEG Version 2.5 
		};

		///<summary>MPEG Mode enumeration.</summary>
		public enum class mpeg_mode
		{
			m_stereo	= MPG123_M_STEREO,	/// Standard Stereo. 
			m_joint		= MPG123_M_JOINT,	/// Joint Stereo. 
			m_dual		= MPG123_M_DUAL,	/// Dual Channel. 
			m_mono		= MPG123_M_MONO,	/// Single Channel. 
		};

		///<summary>MPEG Flags enumeration.</summary>
		public enum class mpeg_flags
		{
			CRC			= MPG123_CRC,		/// The bitstream is error protected using 16-bit CRC. 
			COPYRIGHT	= MPG123_COPYRIGHT,	/// The bitstream is copyrighted. 
			PRIVATE		= MPG123_PRIVATE,	/// The private bit has been set. 
			ORIGINAL	= MPG123_ORIGINAL,	/// The bitstream is an original, not a copy. 
		};

		///<summary>Positional state.</summary>
		public enum class state
		{
			accurate	= MPG123_ACCURATE	/// Query if positons are currently accurate (integer value, 0 if false, 1 if true) 
		};


	}
}
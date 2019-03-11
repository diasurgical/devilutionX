/*
	mpg123clr: MPEG Audio Decoder library Common Language Runtime version.

	copyright 2009 by Malcolm Boczek - free software under the terms of the LGPL 2.1
	mpg123clr.dll is a derivative work of libmpg123 - all original mpg123 licensing terms apply.

	All rights to this work freely assigned to the mpg123 project.
*/
/*
	libmpg123: MPEG Audio Decoder library

	copyright 1995-2008 by the mpg123 project - free software under the terms of the LGPL 2.1
	see COPYING and AUTHORS files in distribution or http://mpg123.org

*/
/*
	1.8.1.0	04-Aug-09	Initial release.
	1.9.0.0 24-Sep-09	Function names harmonized with libmpg123 (mb)
	1.12.0.0 14-Apr-10	Revision harmonization (catch-up)
*/

#pragma once

#pragma warning(disable : 4635)
#include "mpg123.h"
#pragma warning(default : 4635)

#include "enum.h"

using namespace System;

namespace mpg123clr
{
	// defgroup mpg123_error mpg123 error handling
	//
	// Functions to get text version of the error numbers and an enumeration
	// of the error codes returned by libmpg123.
	//
	// Most functions operating on a mpg123_handle simply return MPG123_OK on success and MPG123_ERR on failure (setting the internal error variable of the handle to the specific error code).
	// Decoding/seek functions may also return message codes MPG123_DONE, MPG123_NEW_FORMAT and MPG123_NEED_MORE (please read up on these on how to react!).
	// The positive range of return values is used for "useful" values when appropriate.
	//
	//
	//


	namespace mpg
	{
		///<summary>Enumeration of the message and error codes as returned by libmpg123 functions.</summary>
		public enum class ErrorCode
		{
			done = MPG123_DONE,								/// Message: Track ended. 
			new_format = MPG123_NEW_FORMAT,					/// Message: Output format will be different on next call. 
			need_more = MPG123_NEED_MORE,					/// Message: For feed reader: "Feed me more!" 
			err = MPG123_ERR,								/// Generic Error 
			ok = MPG123_OK,									/// Success 
			bad_outformat = MPG123_BAD_OUTFORMAT,			/// Unable to set up output format! 
			bad_channel = MPG123_BAD_CHANNEL,				/// Invalid channel number specified. 
			bad_rate = MPG123_BAD_RATE,						/// Invalid sample rate specified.  
			err_16to8table = MPG123_ERR_16TO8TABLE,			/// Unable to allocate memory for 16 to 8 converter table! 
			bad_param = MPG123_BAD_PARAM,					/// Bad parameter id! 
			bad_buffer = MPG123_BAD_BUFFER,					/// Bad buffer given -- invalid pointer or too small size. 
			out_of_mem = MPG123_OUT_OF_MEM,					/// Out of memory -- some malloc() failed. 
			not_initialized = MPG123_NOT_INITIALIZED,		/// You didn't initialize the library! 
			bad_decoder = MPG123_BAD_DECODER,				/// Invalid decoder choice. 
			bad_handle = MPG123_BAD_HANDLE,					/// Invalid mpg123 handle. 
			no_buffers = MPG123_NO_BUFFERS,					/// Unable to initialize frame buffers (out of memory?). 
			bad_rva = MPG123_BAD_RVA,						/// Invalid RVA mode. 
			no_gapless = MPG123_NO_GAPLESS,					/// This build doesn't support gapless decoding. 
			no_space = MPG123_NO_SPACE,						/// Not enough buffer space. 
			bad_types = MPG123_BAD_TYPES,					/// Incompatible numeric data types. 
			bad_band = MPG123_BAD_BAND,						/// Bad equalizer band. 
			err_null = MPG123_ERR_NULL,						/// Null pointer given where valid storage address needed. 
			err_reader = MPG123_ERR_READER,					/// Error reading the stream. 
			no_seek_from_end = MPG123_NO_SEEK_FROM_END,		/// Cannot seek from end (end is not known).
			bad_whence = MPG123_BAD_WHENCE,					/// Invalid 'whence' for seek function.
			no_timeout = MPG123_NO_TIMEOUT,					/// Build does not support stream timeouts. 
			bad_file = MPG123_BAD_FILE,						/// File access error. 
			no_seek = MPG123_NO_SEEK,						/// Seek not supported by stream. 
			no_reader = MPG123_NO_READER,					/// No stream opened. 
			bad_pars = MPG123_BAD_PARS,						/// Bad parameter handle. 
			bad_index_par = MPG123_BAD_INDEX_PAR,			/// Bad parameters to mpg123_index() 
			out_of_sync = MPG123_OUT_OF_SYNC,				/// Lost track in bytestream and did not try to resync. 
			resync_fail = MPG123_RESYNC_FAIL,				/// Resync failed to find valid MPEG data. 
			no_8bit = MPG123_NO_8BIT,						/// No 8bit encoding possible. 
			bad_align = MPG123_BAD_ALIGN,					/// Stack aligmnent error 
			null_buffer = MPG123_NULL_BUFFER,				/// NULL input buffer with non-zero size... 
			no_relseek = MPG123_NO_RELSEEK,					/// Relative seek not possible (screwed up file offset) 
			null_pointer = MPG123_NULL_POINTER,				/// You gave a null pointer somewhere where you shouldn't have. 
			bad_key = MPG123_BAD_KEY,						/// Bad key value given. 
			no_index = MPG123_NO_INDEX,						/// No frame index in this build. 
			index_fail = MPG123_INDEX_FAIL,					/// Something with frame index went wrong. 
			bad_decoder_setup = MPG123_BAD_DECODER_SETUP,	/// Something prevents a proper decoder setup 
			missing_feature = MPG123_MISSING_FEATURE,		/// This feature has not been built into libmpg123. 
			/* 1.8.1 */
			bad_value = MPG123_BAD_VALUE,					/// A bad value has been given, somewhere. 
			lseek_failed = MPG123_LSEEK_FAILED,				/// Low-level seek failed.
			/* 1.12.0 */
			bad_custom_io = MPG123_BAD_CUSTOM_IO,			/// Custom I/O not prepared.
			lfs_overflow = MPG123_LFS_OVERFLOW,				/// Offset value overflow during translation of large file API calls,
															/// - your client program cannot handle that large file.

		};
	}

	///<summary>Wrapper for mpg123_errors</summary>
	public ref class mpg123error
	{
	protected:
		///<summary>Finalizer.
		///<para>Cleanly handles mpg123_delete of internal (unmanaged) handles.</para></summary>
		/// Implementation of CLR Finalize().
		!mpg123error(void);

	public:
		///<summary>Constructor.</summary>
		mpg123error(void);

		///<summary>Destructor. Used for final object deletion.
		///<para>Calls finalizer for clean disposal of internal (unmanaged) library handles.</para>
		///</summary>
		/// Implementation of CLR Dispose().
		/// ~Destructor and !Finalizer are the prescribed implementation of Dispose() and Finalize().
		/// See Destructors and Finalizers in Visual C++
		~mpg123error(void);

		///<summary>Obtain a string description of the errcode meaning.
		///<para>Returns a String description representing the error code.</para>
		///</summary>
		///<param name="errcode">The error code to be described.</param>
		///<returns>Returns a String description representing the error code.</returns>
		static String^ __clrcall mpg123_plain_strerror(mpg123clr::mpg::ErrorCode errcode);
	};
}
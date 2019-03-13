/*
	mpg123clr: MPEG Audio Decoder library Common Language Runtime version.

	copyright 2009-2011 by Malcolm Boczek - free software under the terms of the LGPL 2.1
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
	1.9.0.0 24-Sep-09	Function names harmonized with libmpg123 (mb)
	1.10.0.0 30-Nov-09	release match - added mpg123_feature (mb)
	1.12.0.0 14-Apr-10	release match - added framebyframe and "handle" ReplaceReaders (mb)
	1.13.0.0 13-Jan-11	release match - added encsize (mb)
*/

#pragma once

#pragma warning(disable : 4635)
#include "mpg123.h"
#pragma warning(default : 4635)

#include "enum.h"
#include "error.h"
#include "id3v1.h"
#include "id3v2.h"
#include "advanced.h"

#include <io.h>		// for posix proof of concept only

#include <string>
#include <iostream>
#include <stdarg.h>

#include <vcclr.h>
using namespace std;
using namespace System;
using namespace System::IO;
using namespace System::Runtime::InteropServices;

namespace mpg123clr
{


	// Recommended usage when creating reference type on the managed heap (not using stack semantics 
	//  for reference types...) [see (Microsoft) Destructors and Finalizers in Visual C++]
	//
	// C++/CLI
	// A myA;
	//		// use myA here
	//
	// Equivalent user code:
	//
	// A ^ myA = gcnew A;
	// try { /* use myA here */ } finally { delete myA; }
	//
	//
	// C# (from wikipedia)
	// using(A myA = new A()) { /* Use myA here */ }
    // Compiler calls myA.Dispose(), in a "finally" of a "try/finally" block
	//
    // Equivalent user code:
	//
    //  A myA = new myA();
    //  try { /* Use myA here */ } finally { myA.Dispose(); }
	//
	// Otherwise Finalizer will be nondeterministically called by GC


	///<summary>Wrapper for libmpg123.</summary>
	public ref class mpg123
	{

#pragma region Callback and Delegate

		// Functional callbacks for low-level I/O ReplaceReader

	public:
		///<summary>Seek Delegate.
		///<para>Callback seek function to provide low-level stream access to posix-like seek operations.
		/// Requires user supplied static callback fuction of form off_t fn(int fd, off_t offset, int origin).</para>
		///<para>Returns callee supplied resulting offset from start of file or -1 for error.</para>
		///</summary>
		///<param name="fd">Contains file descriptor.</param>
		///<param name="offset">Contains required offset value.</param>
		///<param name="origin">Contains relative origin value (whence) applied to offset.</param>
		///<returns>Callee supplied resulting offset from start of file or -1 for error.</returns>
		[UnmanagedFunctionPointer(CallingConvention::Cdecl)]		
		delegate off_t SeekDelegate(int fd, off_t offset, int origin);

		[UnmanagedFunctionPointer(CallingConvention::Cdecl)]		
		delegate off_t SeekHandleDelegate(void* handle, off_t offset, int origin);

		///<summary>Read Delegate.
		///<para>Callback read function to provide low-level stream access to posix-like read operations.
		/// Requires user supplied static callback fuction of form ssize_t fn(int fd, void* buffer, size_t size).</para>
		///<para>Returns callee supplied resulting actual number of bytes read or -1 for error.</para>
		///</summary>
		///<param name="fd">Contains file descriptor.</param>
		///<param name="offset">Contains address of buffer.</param>
		///<param name="origin">Contains size of buffer.</param>
		///<returns>Callee supplied resulting actual number of bytes read or -1 for error.</returns>
		[UnmanagedFunctionPointer(CallingConvention::Cdecl)]		
		delegate ssize_t ReadDelegate(int fd, void* buffer, size_t size);

		[UnmanagedFunctionPointer(CallingConvention::Cdecl)]		
		delegate ssize_t ReadHandleDelegate(void* handle, void* buffer, size_t size);

		[UnmanagedFunctionPointer(CallingConvention::Cdecl)]		
		delegate void CleanupHandleDelegate(void* handle);

	private:
		// Delegate "keep alive" fields to prevent GC of delegate.
		SeekDelegate^ seekDel;
		ReadDelegate^ readDel;

		SeekHandleDelegate^ seekHDel;
		ReadHandleDelegate^ readHDel;
		CleanupHandleDelegate^ cleanHDel;

		GCHandle userObjectHandle;

		// Temporary delegate store, replacereader action is defered until next stream 'open' action
		SeekDelegate^ r_seekDel;
		ReadDelegate^ r_readDel;

		SeekHandleDelegate^ r_seekHDel;
		ReadHandleDelegate^ r_readHDel;
		CleanupHandleDelegate^ r_cleanHDel;

		bool useHandleReplacement;
		bool lastReplacementWasHandle;

#pragma endregion -Callback and Delegate


#pragma region Library and Handle Setup

	// \defgroup mpg123_init mpg123 library and handle setup
	//
	// Functions to initialise and shutdown the mpg123 library and handles.
	// The parameters of handles have workable defaults, you only have to tune them when you want to tune something;-)
	// Tip: Use a RVA setting...
	//

	private:
		mpg123_handle* mh;

	internal:
		mpg123(mpg123_handle* mh);

	protected:
		///<summary>Finalizer.
		///<para>Cleanly handles mpg123_delete of internal (unmanaged) mpg123 handle.</para></summary>
		/// Implementation of CLR Finalize().
		!mpg123(void);

	public:
		///<summary>Constructor.
		///<para>Only creates mpg123 object, you must call one of the New() methods to obtain a decoder library handle.</para>
		///<para>Recommended usage: using(mpg123 obj = new mpg123()){ use obj here }</para></summary>
		mpg123(void);

		///<summary>Destructor. Used for final object deletion.
		///<para>Calls finalizer for clean disposal of internal (unmanaged) library handles.</para>
		///</summary>
		/// Implementation of CLR Dispose().
		/// ~Destructor and !Finalizer are the prescribed implementation of Dispose() and Finalize().
		/// See Destructors and Finalizers in Visual C++
		~mpg123(void);

		///<summary>(mpg123_init) Function to initialise the mpg123 library. 
		///<para>This function is not thread-safe. Call it exactly once per process, before any other (possibly threaded) work with the library.</para>
		///<para>Returns MPG123_OK if successful, otherwise an error number.</para></summary>
		///<returns>Returns MPG123_OK if successful, otherwise an error number.</returns>
		static mpg123clr::mpg::ErrorCode __clrcall mpg123_init(void);

		///<summary>(mpg123_exit) Function to close down the mpg123 library. 
		///<para>This function is not thread-safe. Call it exactly once per process, before any other (possibly threaded) work with the library.</para></summary>
		static void __clrcall mpg123_exit(void);

		///<summary>(mpg123_new) Obtain am mpg123 handle with designated decoder.
		///<para>Returns MPG123_OK or applicable error code.</para>
		///</summary>
		///<param name="decoder">Name of the decoder to attach.</param>
		///<returns>MPG123_OK or applicable error code.</returns>
		mpg123clr::mpg::ErrorCode __clrcall mpg123_new(String ^ decoder);

		///<summary>Obtain an mpg123 handle with default decoder.
		///<para>Returns MPG123_OK or applicable error code.</para>
		///</summary>
		///<returns>MPG123_OK or applicable error code.</returns>
		mpg123clr::mpg::ErrorCode __clrcall mpg123_new(void);

		///<summary>Obtain am mpg123 handle with default decoder.
		///<para>Allows use of common parms object for multiple connections.</para> 
		///<para>Returns MPG123_OK or applicable error code.</para>
		///</summary>
		///<param name="par">Supplied Advanced parameter object.</param>
		///<returns>MPG123_OK or applicable error code.</returns>
		mpg123clr::mpg::ErrorCode __clrcall mpg123_parnew(mpg123clr::advpars^ par);

		///<summary>Obtain am mpg123 handle with designated decoder.
		///<para>Allows use of common parms object for multiple connections.</para> 
		///<para>Returns MPG123_OK or applicable error code.</para>
		///</summary>
		///<param name="par">Supplied Advanced parameter object.</param>
		///<param name="decoder">Name of the decoder to attach.</param>
		///<returns>MPG123_OK or applicable error code.</returns>
		mpg123clr::mpg::ErrorCode __clrcall mpg123_parnew(mpg123clr::advpars^ par, String^ decoder);

		///<summary>(mpg123_delete) Delete internal mpg123 handle.
		///<para>The framework will dispose of the object when it goes out of scope - you do not need to explicitly call Delete().
		/// However it is available to allow reuse of this object with successive handles - using New()/Delete() pairs.</para>
		///</summary>
		void __clrcall mpg123_delete(void);

	public:

		///<summary>Set a specific parameter value.
		///<para>Returns MPG123_OK or applicable error code.</para>
		///</summary>
		///<param name="type">Define the parameter to be set. (parms enumeration)</param>
		///<param name="val">Integer value to apply.</param>
		///<param name="fval">Real value to apply.</param>
		///<returns>MPG123_OK or applicable error code.</returns>
		mpg123clr::mpg::ErrorCode __clrcall mpg123_param(mpg123clr::mpg::parms type, int val, double fval);

		///<summary>Get a specific parameter value.
		///<para>Returns MPG123_OK or applicable error code.</para>
		///</summary>
		///<param name="type">Define the parameter to get. (parms enumeration)</param>
		///<param name="val">Returned integer value.</param>
		///<param name="fval">Returned real value.</param>
		///<returns>MPG123_OK or applicable error code.</returns>
		mpg123clr::mpg::ErrorCode __clrcall mpg123_getparam(mpg123clr::mpg::parms type, [Out] int% val, [Out] double% fval);

		///<summary>Query libmpg123 feature.
		///<para>Returns 1 for success, 0 for unimplemented feature.</para>
		///</summary>
		///<param name="key">Define the feature to query. (feature_set enumeration)</param>
		///<returns>1 for success, 0 for unimplemented feature.</returns>
		static int __clrcall mpg123_feature(mpg123clr::mpg::feature_set key);

#pragma endregion -Library and Handle Setup

#pragma region Internal Helpers

	internal:
		array<String^>^ StringArrayFromPtr(const char ** ptr);

#pragma endregion -Internal Helpers

#pragma region Error Handling

		// \defgroup mpg123_error mpg123 error handling
		//
		// Functions to get text version of the error numbers and an enumeration
		// of the error codes returned by libmpg123.
		//
		// Most functions operating on a mpg123_handle simply return MPG123_OK on success and MPG123_ERR on failure (setting the internal error variable of the handle to the specific error code).
		// Decoding/seek functions may also return message codes MPG123_DONE, MPG123_NEW_FORMAT and MPG123_NEED_MORE (please read up on these on how to react!).
		// The positive range of return values is used for "useful" values when appropriate.
		//

	public:

		///<summary>Get string describing what error has occured in the context of this object.
		///<para>When a function operating on an mpg123 handle returns MPG123_ERR, you should use this function to check the actual reason.</para>
		///<para>This function will catch mh == NULL and return the message for MPG123_BAD_HANDLE.</para>
		///<para>Returns text representation of last error (incl. None) encountered by this object.</para>
		///</summary>
		///<returns>Text representation of last error (incl. None) encountered by this object.</returns>
		String^ __clrcall mpg123_strerror(void);

		///<summary>Get last error encountered in the context of this object.
		///<para>When a function operating on an mpg123 handle returns MPG123_ERR, you should use this function to check the actual reason.</para>
		///<para>This function will catch internal handle == NULL and return MPG123_BAD_HANDLE.</para>
		///<para>Returns ErrorCode for last encountered error.</para>
		///</summary>
		///<returns>The plain errcode intead of a string of last error (incl. None) encountered by this object.</returns>
		mpg123clr::mpg::ErrorCode __clrcall mpg123_errcode(void);

#pragma endregion -Error Handling

#pragma region Decoder Selection

		// \defgroup mpg123_decoder mpg123 decoder selection
		//
		// Functions to list and select the available decoders.
		// Perhaps the most prominent feature of mpg123: You have several (optimized) decoders to choose from (on x86 and PPC (MacOS) systems, that is).
		//

	public:

		///<summary>Obtain list of generally available decoder names (plain 8bit ASCII).
		///<para>Retuns a string array of generally available decoder names (plain 8bit ASCII).</para>
		///</summary>
		///<returns>A string array of generally available decoder names (plain 8bit ASCII).</returns>
		array<String^>^ __clrcall mpg123_decoders(void);

		///<summary>Obtain list of the decoders supported by the CPU (plain 8bit ASCII).
		///<para>Returns a string array of the decoders supported by the CPU (plain 8bit ASCII).</para>
		///</summary>
		///<returns>A string array of the decoders supported by the CPU (plain 8bit ASCII).</returns>
		array<String^>^ __clrcall mpg123_supported_decoders(void);

		///<summary>Select the decoder to use.
		///<para>Returns MPG123_OK or applicable error code.</para>
		///</summary>
		///<param name="name">Name of the required decoder. (should be in SupportedDecoders list)</param>
		///<returns>MPG123_OK or applicable error code.</returns>
		mpg123clr::mpg::ErrorCode __clrcall mpg123_decoder(String^ name);

		///<summary>Get the currently active decoder engine name.
		///<para>The active decoder engine can vary depening on output constraints, mostly non-resampling, integer output is accelerated via 3DNow and Co. 
		/// but for other modes a fallback engine kicks in.</para>
		///<para>Note that this can return a decoder that is ony active in the hidden and not available as decoder choice from the outside.</para>
		///<para>Returns the decoder name or String.Empty on error.</para>
		///</summary>
		///<returns>The decoder name or String.Empty on error.</returns>
		String^ __clrcall mpg123_current_decoder(void);

#pragma endregion -Decoder Selection

#pragma region Output Audio Format

		// \defgroup mpg123_output mpg123 output audio format 
		//
		// Functions to get and select the format of the decoded audio.
		//

	public:

		///<summary>Get an array of supported standard sample rates.
		///<para>These are possible native sample rates of MPEG audio files.
		/// You can still force mpg123 to resample to a different one, but by default you will only get audio in one of these samplings.</para>
		///<para>Returns array of sample rates.</para>
		///</summary>
		///<returns>An array of sample rates.</returns>
		array<long>^ __clrcall mpg123_rates(void);

		///<summary>An array of supported audio encodings.
		///<para>An audio encoding is one of the fully qualified members of mpg.enc</para>
		///<para>Returns array of supported Encodings.</para>
		///</summary>
		///<returns>An array of supported Encodings.</returns>
		array<mpg123clr::mpg::enc>^ __clrcall mpg123_encodings(void);

		///<summary>Get the size (in bytes) of one mono sample of the named encoding.
		///<para>Returns the positive size of encoding in bytes, 0 on invalid encoding.</para>
		///</summary>
		///<param name="encoding">The encoding value to analyze.</param>
		///<returns>The positive size of encoding in bytes, 0 on invalid encoding.</returns>
		static int __clrcall mpg123_encsize(mpg123clr::mpg::enc encoding);

		///<summary>Configure mpg123 to accept no output format at all.
		///<para>Use to clear default parameters prior to applying specific settings.</para>
		///<para>Returns MPG123 error codes.</para>
		///</summary>
		///<returns>MPG123 error codes.</returns>
		mpg123clr::mpg::ErrorCode __clrcall mpg123_format_none(void);

		///<summary>Configure mpg123 to accept all formats including any custom formats - this is the default.
		///<para>Returns MPG123 error codes.</para>
		///</summary>
		///<returns>MPG123 error codes.</returns>
		mpg123clr::mpg::ErrorCode __clrcall mpg123_format_all(void);

		///<summary>Configure detailed output formats.
		///<para>Returns MPG123 error codes.</para>
		///</summary>
		///<param name="rate">Sample rate (Hertz)</param>
		///<param name="channels">Combination of channelcount.stereo and channelcount.mono</param>
		///<param name="encodings">Combination of accepted encodings for rate and channels e.g. enc.enc_signed16 | enc.enc_ulaw_8 (or 0 for none)</param>
		///<returns>MPG123 error codes.</returns>
		mpg123clr::mpg::ErrorCode __clrcall mpg123_format(int rate, mpg123clr::mpg::channelcount channels, mpg123clr::mpg::enc encodings);

		///<summary>Get available support for supplied rate and encoding.
		///<para>Returns 0 for no support (includes invalid parameters), or combination of channelcount.stereo and channelcount.mono.</para>
		///</summary>
		///<param name="rate">Sample rate (Hertz)</param>
		///<param name="encoding">Combination of accepted encodings for rate and channels e.g. enc.enc_signed16 | enc.enc_ulaw_8 (or 0 for none)</param>
		///<returns>Returns 0 for no support (includes invalid parameters), or combination of channelcount.stereo and channelcount.mono.</returns>
		mpg123clr::mpg::channelcount __clrcall mpg123_format_support(int rate, mpg123clr::mpg::enc encoding);

		///<summary>Get current output format.
		///<para>Returns MPG123 error codes.</para>
		///</summary>
		///<param name="rate">Returns current sample rate (Hertz)</param>
		///<param name="channels">Returns combination of channelcount.stereo and channelcount.mono</param>
		///<param name="encoding">Returns combination of accepted encodings for rate and channels e.g. enc.enc_signed16 | enc.enc_ulaw_8 (or 0 for none)</param>
		///<returns>MPG123 error codes.</returns>
		mpg123clr::mpg::ErrorCode __clrcall mpg123_getformat([Out] int% rate, [Out] mpg123clr::mpg::channelcount% channels, [Out] mpg123clr::mpg::enc% encoding);

#pragma endregion -Output Audio Format

#pragma region File Input and Decoding

		// \defgroup mpg123_input mpg123 file input and decoding
		//
		// Functions for input bitstream and decoding operations.
		// Decoding/seek functions may also return message codes MPG123_DONE, MPG123_NEW_FORMAT and MPG123_NEED_MORE (please read up on these on how to react!).
		//

	public:

		///<summary>Open and prepare to decode the file specified by ANSI filename.
		///<para>This does not open HTTP urls; the mpg library contains no networking code.
		/// If you want to decode internet streams, use Open(fd) or Open().</para>
		///
		///<para>Returns MPG123 error codes</para>
		///</summary>
		///<param name="path">ANSI file path. Accepts ANSI path characters. For Unicode paths use tOpen.
		/// NOTE: can be used in Unicode environment as long as wide-char codepages are avoided.</param>
		///<returns>MPG123 error codes.</returns>
		mpg123clr::mpg::ErrorCode __clrcall mpg123_open(String^ path);

		///<summary>Use a previously obtained file descriptor as the bitstream input.
		///<para>NOTE: Close() will NOT close a file opened with this method.</para>
		///<para>Returns MPG123 error codes.</para>
		///</summary>
		///<param name="fd">File Descriptor of pre required file.</param>
		///<returns>MPG123 error codes.</returns>
		mpg123clr::mpg::ErrorCode __clrcall mpg123_open_fd(int fd);

		///<summary>Use an opaque handle as bitstream input.
		///<para>This works only with the replaced I/O from mpg123_replace_reader_handle()!</para>
		///<para>mpg123_close() will call the cleanup callback for your handle (if you gave one).</para>
		///<para>Returns MPG123 error codes.</para>
		///</summary>
		///<returns>MPG123 error codes.</returns>
		mpg123clr::mpg::ErrorCode __clrcall mpg123_open_handle(System::Object^ obj);

		///<summary>Open a new bitstream and prepare for direct feeding.
		///<para>This works together with Decode(); you are responsible for reading and feeding the input bitstream.</para>
		///<para>Returns MPG123 error codes.</para>
		///</summary>
		///<returns>MPG123 error codes.</returns>
		mpg123clr::mpg::ErrorCode __clrcall mpg123_open_feed(void);

		///<summary>Closes the source, if the library opened it.
		///<para>Returns MPG123 error codes.</para>
		///</summary>
		///<returns>MPG123 error codes.</returns>
		mpg123clr::mpg::ErrorCode __clrcall mpg123_close(void);

		///<summary>Open and prepare to decode the file specified by UNICODE (wide-character) filename.
		///<para>This does not open HTTP urls; the mpg library contains no networking code.
		/// If you want to decode internet streams, use Open(fd) or Open().</para>
		///
		///<para>Returns MPG123 error codes</para>
		///</summary>
		///<param name="path">UNICODE wide-character file path. See also ANSI Open(path).</param>
		///<returns>MPG123 error codes.</returns>
		mpg123clr::mpg::ErrorCode __clrcall mpg123_topen(String^ path);

		///<summary>Closes the file opened with tOpen.
		///<para>Returns MPG123 error codes.</para>
		///</summary>
		///<returns>MPG123 error codes.</returns>
		mpg123clr::mpg::ErrorCode __clrcall mpg123_tclose(void);

		///<summary>Read from stream and decode.
		///<para>Equivalent to mpg123_read(,,,)</para>
		///<para>Returns MPG123 error codes. (watch out for MPG123_DONE and friends!)</para>
		///</summary>
		///<param name="buffer">Supplied buffer in which to return audio output data.</param>
		///<param name="count">Returns number of actual audio output bytes returned.</param>
		///<returns>MPG123 error codes. (watch out for MPG123_DONE and friends!)</returns>
		mpg123clr::mpg::ErrorCode __clrcall mpg123_read(array<unsigned char>^ buffer, [Out] size_t% count);

		///<summary>Read from stream and decode.
		///<para>Equivalent to mpg123_read(,,,) but modified to better support CLR Stream.Read</para>
		///<para>Returns MPG123 error codes. (watch out for MPG123_DONE and friends!)</para>
		///</summary>
		///<param name="buffer">Supplied buffer in which to return audio output data.</param>
		///<param name="offset">Offset in buffer at which to begin storing data.</param>
		///<param name="size">Maximum number of bytes to return.</param>
		///<param name="count">Returns number of actual audio output bytes returned.</param>
		///<returns>MPG123 error codes. (watch out for MPG123_DONE and friends!)</returns>
		mpg123clr::mpg::ErrorCode __clrcall mpg123_read(array<unsigned char>^ buffer, size_t offset, size_t size, [Out] size_t% count);

		///<summary>Feed data for a stream that has been opened with Open() - (mpg123_open_feed).
		///<para>Equivalent to mpg123_feed(...), it's give and take: You provide the bytestream, mpg123 gives you the decoded samples.</para>
		///<para>Returns MPG123 error codes.</para>
		///</summary>
		///<param name="inbuffer">Input buffer.</param>
		///<param name="size">Number of input bytes.</param>
		///<returns>MPG123 error codes.</returns>
		mpg123clr::mpg::ErrorCode __clrcall mpg123_feed(array<unsigned char>^ inbuffer, size_t size);

		///<summary>Decode MPEG Audio from inmemory to outmemory.
		///<para>This is very close to a drop-in replacement for old mpglib.</para>
		///<para>When you give zero-sized output buffer the input will be parsed until 
		/// decoded data is available. This enables you to get MPG123_NEW_FORMAT (and query it) 
		/// without taking decoded data.</para>
		///<para>Think of this function being the union of Read() and Feed() (which it actually is, sort of;-).
		/// You can actually always decide if you want those specialized functions in separate steps or one call this one here.</para>
		///<para>Returns MPG123 error codes. (watch out especially for MPG123_NEED_MORE)</para>
		///</summary>
		///<param name="inbuffer">Input buffer.</param>
		///<param name="insize">Number of input bytes.</param>
		///<param name="outbuffer">Supplied buffer in which to return audio output data.</param>
		///<param name="outsize">Size in bytes of buffer.</param>
		///<param name="count">Returns number of actual audio output bytes returned.</param>
		///<returns>MPG123 error codes. (watch out especially for MPG123_NEED_MORE)</returns>
		mpg123clr::mpg::ErrorCode __clrcall mpg123_decode(array<unsigned char>^ inbuffer, size_t insize, array<unsigned char>^ outbuffer, size_t outsize, [Out] size_t% count);

		///<summary>Decode next MPEG frame to internal buffer or read a frame and return after setting a new format.
		///<para>Returns MPG123 error codes. (watch out for MPG123_NEW_FORMAT)</para>
		///</summary>
		///<param name="num">Returns current frame offset.</param>
		///<param name="audio">Returns pointer to internal buffer to read the decoded audio from. (Can be NULL for NEW_FORMAT)</param>
		///<param name="count">Returns number of actual audio output bytes ready in the buffer.</param>
		///<returns>MPG123 error codes. (watch out for MPG123_NEW_FORMAT)</returns>
		mpg123clr::mpg::ErrorCode __clrcall mpg123_decode_frame([Out] off_t% num, [Out] IntPtr% audio, [Out] size_t% count);

		///<summary>Decode current MPEG frame to internal buffer.
		///<para>Use with mpg123_framebyframe_next to progress through data.</para>
		///<para>Warning: This is experimental API that might change in future releases!
		/// Please watch mpg123 development closely when using it.</para>
		///<para>Returns MPG123 error codes.</para>
		///</summary>
		///<param name="num">Returns current frame offset.</param>
		///<param name="audio">Returns pointer to internal buffer to read the decoded audio from. (Can be NULL for NEW_FORMAT)</param>
		///<param name="bytes">Returns number of actual audio output bytes ready in the buffer.</param>
		///<returns>MPG123 error codes.</returns>
		mpg123clr::mpg::ErrorCode __clrcall mpg123_framebyframe_decode([Out] off_t% num, [Out] IntPtr% audio, [Out] size_t% bytes);

		///<summary>Find, read and parse the next mp3 frame.
		///<para>Use with mpg123_framebyframe_decode to obtain frame data.</para>
		///<para>Warning: This is experimental API that might change in future releases!
		/// Please watch mpg123 development closely when using it.</para>
		///<para>Returns MPG123 error codes. (watch out for MPG123_NEW_FORMAT)</para>
		///</summary>
		///<returns>MPG123 error codes. (watch out for MPG123_NEW_FORMAT)</returns>
		mpg123clr::mpg::ErrorCode __clrcall mpg123_framebyframe_next(void);

#pragma endregion -File Input and Decoding

#pragma region Position and Seeking

		// \defgroup mpg123_seek mpg123 position and seeking
		//
		// Functions querying and manipulating position in the decoded audio bitstream.
		// The position is measured in decoded audio samples, or MPEG frame offset for the specific functions.
		// If gapless code is in effect, the positions are adjusted to compensate the skipped padding/delay - meaning, you should not care about that at all and just use the position defined for the samples you get out of the decoder;-)
		// The general usage is modelled after stdlib's ftell() and fseek().
		// Especially, the whence parameter for the seek functions has the same meaning as the one for fseek() and needs the same constants from stdlib.h: 
		// - SEEK_SET: set position to (or near to) specified offset
		// - SEEK_CUR: change position by offset from now
		// - SEEK_END: set position to offset from end
		//
		// Note that sample-accurate seek only works when gapless support has been enabled at compile time; seek is frame-accurate otherwise.
		// Also, really sample-accurate seeking (meaning that you get the identical sample value after seeking compared to plain decoding up to the position) is only guaranteed when you do not mess with the position code by using MPG123_UPSPEED, MPG123_DOWNSPEED or MPG123_START_FRAME. The first two mainly should cause trouble with NtoM resampling, but in any case with these options in effect, you have to keep in mind that the sample offset is not the same as counting the samples you get from decoding since mpg123 counts the skipped samples, too (or the samples played twice only once)!
		// Short: When you care about the sample position, don't mess with those parameters;-)
		// Also, seeking is not guaranteed to work for all streams (underlying stream may not support it).
		//

	public:

		///<summary>Get the current position in samples. On the next read, you'd get that sample.
		///<para>Returns the current sample position.</para>
		///</summary>
		///<returns>The current sample position.</returns>
		long long __clrcall mpg123_tell();

		///<summary>Get the frame number that the next read will give you data from.
		///<para>Returns the current frame number position.</para>
		///</summary>
		///<returns>The current frame number position.</returns>
		long long __clrcall mpg123_tellframe();

		///<summary>Get the current byte offset in the input stream.
		///<para>Returns the current byte offset.</para>
		///</summary>
		///<returns>The current byte offset.</returns>
		long long __clrcall mpg123_tell_stream();

		///<summary>Seek to a desired sample offset.
		///<para>Returns the resulting offset >= 0 or error/message code.</para>
		///</summary>
		///<param name="offset">The distance to move.</param>
		///<param name="origin">(whence) The relative location to move from.(SeekOrigin.Begin, SeekOrigin.Current, SeekOrigin.End)</param>
		///<returns>Returns the resulting offset >= 0 or error/message code.</returns>
		long long __clrcall mpg123_seek(long long offset, SeekOrigin origin);

		///<summary>Seek to a desired sample offset in data feeding mode.
		///<para>This just prepares things to be right only if you ensure that the next chunk of input data will be from input_offset byte position.</para>
		///<para>Returns the resulting offset >= 0 or error/message code.</para>
		///</summary>
		///<param name="offset">The distance to move.</param>
		///<param name="origin">(whence) The relative location to move from.(SeekOrigin.Begin, SeekOrigin.Current, SeekOrigin.End)</param>
		///<param name="input_offset">Returns the position it expects to be at the next time data is fed to Decode().</param>
		///<returns>Returns the resulting offset >= 0 or error/message code.</returns>
		long long __clrcall mpg123_feedseek(long long offset, SeekOrigin origin, [Out] long long% input_offset);

		///<summary>Seek to a desired MPEG frame index.
		///<para>Returns the resulting offset >= 0 or error/message code.</para>
		///</summary>
		///<param name="frameoffset">The numberof frames to move.</param>
		///<param name="origin">(whence) The relative location to move from.(SEEK_SET, SEEK_CUR or SEEK_END)</param>
		///<returns>Returns the resulting offset >= 0 or error/message code.</returns>
		long long __clrcall mpg123_seek_frame(long long frameoffset, SeekOrigin origin);

		///<summary>Seek to an absolute MPEG frame offset corresponding to an offset in seconds.
		///<para>This assumes that the samples per frame do not change in the file/stream, which is a good assumption for any sane file/stream only.</para>
		///<para>Returns the resulting offset >= 0 or error/message code.</para>
		///</summary>
		///<param name="seconds">The absolute time offset required.</param>
		///<returns>Returns the resulting offset >= 0 or error/message code.</returns>
		long long __clrcall mpg123_timeframe(double seconds);

		///<summary>Get a copy of the frame index table. Somewhat equivalent to mpg123_index(,,).
		///<para>The library requests not to modify table values. Since this is a copy, modification is meaningless - it has no effect on library routines.</para>
		///<para>NOTE: The returned index table value types are Int64 independant of library build.</para>
		///<para>Returns MPG123 error codes.</para>
		///</summary>
		///<param name="index">Returns array of source file position offsets (not output samples). Length of array is equivalent to mpg123_index "fill" parameter.</param>
		///<param name="step">Returns number of MPEG frames per index entry.</param>
		///<returns>MPG123 error codes.</returns>
		mpg123clr::mpg::ErrorCode __clrcall mpg123_index([Out] array<long long>^% index, [Out] long long% step);

		///<summary>Get a pointer to the frame index table. Equivalent to mpg123_index(,,).
		///<para>Do not modify table values unless you really know what you are doing!</para>
		///<para>NOTE: The returned index table value types may be Int32 or Int64 depending on library build.</para>
		///<para>Returns MPG123 error codes.</para>
		///</summary>
		///<param name="indexarr">Returns pointer to source file position offset index array (not output samples).</param>
		///<param name="step">Returns number of MPEG frames per index entry.</param>
		///<param name="fill">Returns number of recorded index offsets; size of the array.</param>
		///<returns>MPG123 error codes.</returns>
		mpg123clr::mpg::ErrorCode __clrcall mpg123_index([Out] IntPtr% indexarr, [Out] long long% step, [Out] size_t% fill);

		///<summary>Get information about current and remaining frames/seconds. Equivalent to mpg123_position(,,,,,,).
		///<para>WARNING: This function is there because of special usage by standalone mpg123 and may be removed in the final version of libmpg123!</para>
		///<para>You provide an offset (in frames) from now and a number of output bytes 
		/// served by libmpg123 but not yet played. You get the projected current frame 
		/// and seconds, as well as the remaining frames/seconds. This does _not_ care 
		/// about skipped samples due to gapless playback.</para>
		///<para>Returns MPG123 error codes.</para>
		///</summary>
		///<param name="frameoffset">Offset (in frames) from now.</param>
		///<param name="bufferedbytes">Number of output bytes served by library but not yet played.</param>
		///<param name="currentframe">Returns projected current frame.</param>
		///<param name="framesleft">Returns projected frames remaining.</param>
		///<param name="currentseconds">Returns projected current seconds.</param>
		///<param name="secondsleft">Returns projected seconds remaining.</param>
		///<returns>MPG123 error codes.</returns>
		mpg123clr::mpg::ErrorCode __clrcall mpg123_position(long long frameoffset, long long bufferedbytes, 
			[Out] long long% currentframe, [Out] long long% framesleft,
			[Out] double% currentseconds, [Out] double% secondsleft);

#pragma endregion -Position and Seeking

#pragma region Volume and Equalizer

		// \defgroup mpg123_voleq mpg123 volume and equalizer
		//

	public:

		///<summary>Set the 32 Band Audio Equalizer settings.
		///<para>Returns MPG123 error codes.</para>
		///</summary>
		///<param name="channel">Can be MPG123_LEFT, MPG123_RIGHT or MPG123_LEFT|MPG123_RIGHT for both. (enum mpg.channels)</param>
		///<param name ="band">The equaliser band to change (from 0 to 31)</param>
		///<param name="fval">The (linear) adjustment factor to be applied.</param>
		///<returns>MPG123 error codes.</returns>
		mpg123clr::mpg::ErrorCode __clrcall mpg123_eq(mpg123clr::mpg::channels channel, int band, double fval);

		///<summary>Get the 32 Band Audio Equalizer settings.
		///<para>Rreturns the (linear) adjustment factor.</para>
		///</summary>
		///<param name="channel">Can be MPG123_LEFT, MPG123_RIGHT or MPG123_LEFT|MPG123_RIGHT for (arithmetic mean of) both. (enum mpg.channels)</param>
		///<param name="band">The equaliser band to get (from 0 to 31)</param>
		///<return>The (linear) adjustment factor.</return>
		double __clrcall mpg123_geteq(mpg123clr::mpg::channels channel, int band);

		///<summary>Reset the 32 Band Audio Equalizer settings to flat.
		///<para>Returns MPG123 error codes.</para>
		///</summary>
		///<returns>MPG123 error codes.</returns>
		mpg123clr::mpg::ErrorCode __clrcall mpg123_reset_eq();

		///<summary>Set the absolute output volume including the RVA setting.
		///<para>Returns MPG123 error codes.</para>
		///</summary>
		///<param name="volume">The (linear) adjustment factor to be applied, volume &lt; 0 just applies (a possibly changed) RVA setting.</param>
		///<returns>MPG123 error codes.</returns>
		mpg123clr::mpg::ErrorCode __clrcall mpg123_volume(double volume);

		///<summary>Adjust output volume including the RVA setting.
		///<para>Returns MPG123 error codes.</para>
		///</summary>
		///<param name="change">The (linear) adjustment factor to be applied.</param>
		///<returns>MPG123 error codes.</returns>
		mpg123clr::mpg::ErrorCode __clrcall mpg123_volume_change(double change);

		///<summary>Get the current volume setting, the actual value due to RVA, and the RVA adjustment itself.
		///<para>Returns MPG123 error codes.</para>
		///</summary>
		///<param name="basevol">Returns the current linear volume factor. (not percent)</param>
		///<param name="really">Returns the actual linear volume factor due to RVA. (not percent)</param>
		///<param name="rva_db">Returns the RVA adjustment in decibels.</param>
		///<returns>MPG123 error codes.</returns>
		mpg123clr::mpg::ErrorCode __clrcall mpg123_getvolume([Out] double% basevol, [Out] double% really, [Out] double% rva_db);

#pragma endregion -Volume and Equalizer

#pragma region Status and Information

		// \defgroup mpg123_status mpg123 status and information
		//
		//
		//

		[StructLayout(LayoutKind::Sequential, CharSet=CharSet::Ansi, Pack=1)]
		ref struct mpeg_frameinfo
		{
			mpg123clr::mpg::mpeg_version version;	// The MPEG version (1.0/2.0/2.5).
			int layer;								// The MPEG Audio Layer (MP1/MP2/MP3).
			long rate; 								// The sampling rate in Hz.
			mpg123clr::mpg::mpeg_mode mode;			// The audio mode (Mono, Stereo, Joint-stero, Dual Channel).
			int mode_ext;							// The mode extension bit flag.
			int framesize;							// The size of the frame (in bytes).
			mpg123clr::mpg::mpeg_flags flags;		// MPEG Audio flag bits.
			int emphasis;							// The emphasis type.
			int bitrate;							// Bitrate of the frame (kbps).
			int abr_rate;							// The target average bitrate.
			mpg123clr::mpg::mpeg_vbr vbr;			// The VBR mode.
		};

		///<summary>Get the frame information about the MPEG audio bitstream.
		///<para>Returns MPG123 error codes.</para>
		///</summary>
		///<param name="finfo">Returns the frame information.</param>
		///<returns>MPG123 error codes.</returns>
		mpg123clr::mpg::ErrorCode __clrcall mpg123_info([Out]mpeg_frameinfo^% finfo);

		///<summary>Get the frame information about the MPEG audio bitstream.
		///<para>SafeInfo uses "safe" managed structures but is somewhat slower than Info().
		/// Recommend using Info() unless it gives specific problems.</para>
		///<para>Returns MPG123 error codes.</para>
		///</summary>
		///<param name="finfo">Returns the frame information.</param>
		///<returns>MPG123 error codes.</returns>
		mpg123clr::mpg::ErrorCode __clrcall mpg123_safeinfo([Out]mpeg_frameinfo^% finfo);

		///<summary>Get the safe output buffer size for all cases (when you want to replace the internal buffer).
		///<para>Returns safe buffer size.</para>
		///</summary>
		///<returns>Safe buffer size.</returns>
		static size_t __clrcall mpg123_safe_buffer(void);

		///<summary>Make a full parsing scan of each frame in the file. 
		///<para>ID3 tags are found. An accurate length value is stored. Seek index will be filled. 
		/// A seek back to current position is performed.</para>
		///<para>This function refuses work when stream is not seekable.</para>
		///<para>Returns MPG123 error codes.</para>
		///</summary>
		///<returns>MPG123 error codes.</returns>
		mpg123clr::mpg::ErrorCode __clrcall mpg123_scan(void);

		///<summary>Return, if possible, the full (expected) length of current track in samples.
		///<para>Returns length (>= 0) or MPG123_ERR if there is no length guess possible. (Multiply by BlockAlign for byte-count)</para>
		///</summary>
		///<returns>Length (>= 0) or MPG123_ERR if there is no length guess possible. (Multiply by BlockAlign for byte-count)</returns>
		long long __clrcall mpg123_length(void);

		///<summary>Override the value for file size in bytes.
		///<para>Useful for getting sensible track length values in feed mode or for HTTP streams.</para>
		///<para>Returns MPG123 error codes.</para>
		///</summary>
		///<param name="size">Size to set.</param>
		///<returns>MPG123 error codes.</returns>
		mpg123clr::mpg::ErrorCode __clrcall mpg123_set_filesize(long long size);

		///<summary>Time Per Frame (seconds).
		///<para>Returns time per frame in seconds ( &lt; 0 is error ).</para>
		///</summary>
		///<returns>Time per frame in seconds ( &lt; 0 is error ).</returns>
		double __clrcall mpg123_tpf(void);

		///<summary>Get and reset the clip count.
		///<para>Returns the number of previously encountered clips.</para>
		///</summary>
		///<returns>The number of previously encountered clips.</returns>
		long __clrcall mpg123_clip();

		///<summary>Get various current decoder/stream state information.
		///<para>Returns MPG123 error codes.</para>
		///</summary>
		///<param name="key">The key to identify the information to get (enum mpg.state).</param>
		///<param name="val">Returns integer values.</param>
		///<param name="fval">Returns real values.</param>
		///<returns>MPG123 error codes.</returns>
		mpg123clr::mpg::ErrorCode __clrcall mpg123_getstate(mpg123clr::mpg::state key, [Out] int% val, [Out] double% fval);

#pragma endregion -Status and Information

#pragma region Metadata Handling

		// \defgroup mpg123_metadata mpg123 metadata handling
		//
		// Functions to retrieve the metadata from MPEG Audio files and streams.
		// Also includes string handling functions.
		//

		// clr - much of the metadata has been relocated to class files for maintainability
		//       see string.h, text.h, id3v1.h and id3v2.h										
		//

		///<summary>Query if there is (new) meta info, be it ID3 or ICY (or something new in future).
		///<para>Returns a combination of flags. (enum id3.id3check)</para>
		///</summary>
		///<returns>Returns a combination of flags. (enum id3.id3check)</returns>
		mpg123clr::id3::id3check __clrcall mpg123_meta_check(void); /* On error (no valid handle) just 0 is returned. */

		///<summary>Get ID3 data. Data structures may change on any (next) read/decode function call.
		///<para>v1 and/or v2 may be Empty if no corresponding data exists.</para>
		///<para>Returns MPG123_OK or MPG123_ERR.</para>
		///</summary>
		///<param name="v1">Returns mpg123id3v1 data structure containing ID3v1 data (usually from end of file).</param>
		///<param name="v2">Returns mpg123id3v2 data structure containing ID3v2 data (usually - but not restricted to - from beginning of file).</param>
		///<returns>Returns MPG123_OK or MPG123_ERR.</returns>
		mpg123clr::mpg::ErrorCode __clrcall mpg123_id3([Out]id3::mpg123id3v1^% v1, [Out]id3::mpg123id3v2^% v2);

		///<summary>Get ICY meta data. Data structure may change on any (next) read/decode function call.
		///<para>Icy_meta may be null if no corresponding data exists.</para>
		///<para>Returns MPG123_OK or MPG123_ERR.</para>
		///</summary>
		///<param name="icy_meta">Returns ICY meta data (windows-1252 encoded).</param>
		///<returns>Returns MPG123_OK or MPG123_ERR.</returns>
		mpg123clr::mpg::ErrorCode __clrcall mpg123_icy([Out]IntPtr% icy_meta); /* same for ICY meta string */

		///<summary>Decode from windows-1252 (the encoding ICY metainfo used) to UTF-8.
		///<para>Returns byte array of utf8 encoded data.</para>
		///</summary>
		///<param name="icy_text">ICY meta data in ICY encoding.</param>
		///<returns>Returns byte array of utf8 encoded data.</returns>
		static array<unsigned char>^ __clrcall mpg123_icy2utf8(IntPtr icy_text);

#pragma endregion -Metadata Handling

#pragma region Advanced Parameter API

		// \defgroup mpg123_advpar mpg123 advanced parameter API
		//
		// Direct access to a parameter set without full handle around it.
		// Possible uses:
		//  - Influence behaviour of library _during_ initialization of handle (MPG123_VERBOSE).
		//  - Use one set of parameters for multiple handles.
		//
		// The functions for handling mpg123_pars (mpg123_par() and mpg123_fmt() 
		//  family) directly return a fully qualified mpg123 error code, the ones 
		//  operating on full handles normally MPG123_OK or MPG123_ERR, storing the 
		//  specific error code itself inside the handle. 
		//

		// see advanced.h

#pragma endregion -Advanced Parameter API

#pragma region Low Level I/O

		// \defgroup mpg123_lowio mpg123 low level I/O
		// You may want to do tricky stuff with I/O that does not work with mpg123's default file access or you want to make it decode into your own pocket...
		//

		///<summary>Replace default internal buffer with user-supplied buffer.
		///<para>Instead of working on it's own private buffer, mpg123 will directly use the one you provide for storing decoded audio.</para>
		///<para>The data buffer should be pinned before calling this function.</para>
		///<para>Returns MPG123_OK or MPG123 error code.</para>
		///</summary>
		///<param name="data">Pointer to supplied buffer.</param>
		///<param name="size">Size of supplied buffer.</param>
		///<returns>MPG123_OK or MPG123_ERR code.</returns>
		mpg123clr::mpg::ErrorCode __clrcall mpg123_replace_buffer(IntPtr data, size_t size);

		///<summary>The max size of one frame's decoded output with current settings.
		///<para>Use that to determine an appropriate minimum buffer size for decoding one frame.</para>
		///<para>Returns size of required buffer.</para>
		///</summary>
		///<returns>Size of required buffer.</returns>
		size_t __clrcall mpg123_outblock(void);

		///<summary>Replace low-level stream access functions; read and lseek as known in POSIX.
		///<para>You can use this to make any fancy file opening/closing yourself, 
		/// using mpg123_open_fd() to set the file descriptor for your read/lseek (doesn't need to be a 'real' file descriptor...).</para>
		///<para>Setting a function to NULL means that the default internal function is used (active from next Open call onward).</para>
		/////////////////////////
		///<para>Always returns MPG123_OK.</para>
		///</summary>
		///<param name="r_read">Delegate for read function, null for default.</param>
		///<param name="r_lseek">Delegate for lseek function, null for default.</param>
		///<returns>Always MPG123_OK.</returns>
		mpg123clr::mpg::ErrorCode __clrcall mpg123_replace_reader(ReadDelegate^ r_read, SeekDelegate^ r_lseek);

		///////////////////////////////////////
		mpg123clr::mpg::ErrorCode __clrcall mpg123_replace_reader_handle(ReadHandleDelegate^ rh_read, SeekHandleDelegate^ rh_lseek, CleanupHandleDelegate^ rh_clean);

	private:

		// Defered internal implementation of ReplaceReader - action is defered until next 'Open' operation.
		void __clrcall _ReplaceReader(void);

	public:

		// These functions are not part of the mpg123clr wrapper but are included as proof of concept
		// of how to implement callback functions.

		///<summary>Proof of concept posix-like lseek function.
		///<para>This routine should not normally be called from your callback routine, it is a sample function
		/// showing how such a callback could be implemented.</para>
		///</summary>
		///<param name="fd">File descriptor.</param>
		///<param name="offset">Required position.</param>
		///<param name="origin">Whence.</param>
		///<returns>Resultant position.</returns>
		static long PosixSeek(int fd, long offset, int origin)
		{
			long ret =  _lseek(fd, offset, origin);
			return ret;
		}

		///<summary>Proof of concept posix-like read function.
		///<para>This routine should not normally be called from your callback routine, it is a sample function
		/// showing how such a callback could be implemented.</para>
		///</summary>
		///<param name="fd">File descriptor.</param>
		///<param name="buf">Buffer address.</param>
		///<param name="count">Size of buffer.</param>
		///<returns>Actual bytes read.</returns>
		static int PosixRead(int fd, void*buf, unsigned int count)
		{
			int ret = _read(fd, buf, count);
			return ret;
		}

#pragma endregion -Low Level I/O

	};

}